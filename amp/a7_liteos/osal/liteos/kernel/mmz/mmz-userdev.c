/* userdev.c
*
* Copyright (c) 2006 Hisilicon Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
*/

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <sys/mman.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/dma-mapping.h>

#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/cacheflush.h>
#include <asm/dma.h>
#include "osal_mmz.h"
#include "hi_osal.h"

extern hil_mmb_t *hil_mmb_getby_phys_2(unsigned long addr, unsigned long *Outoffset);
extern int hil_mmb_put(hil_mmb_t *mmb);
extern int hil_mmb_get(hil_mmb_t *mmb);

#define error(s...)                                        \
    do {                                                   \
        printk(KERN_ERR "mmz_userdev:%s: ", __FUNCTION__); \
        printk(s);                                         \
    } while (0)
#define warning(s...)                                          \
    do {                                                       \
        printk(KERN_WARNING "mmz_userdev:%s: ", __FUNCTION__); \
        printk(s);                                             \
    } while (0)

struct mmz_userdev_info {
    pid_t pid;
    pid_t mmap_pid;
    struct semaphore sem;
    struct osal_list_head list;
};

typedef struct hiUMAP_DEVICE_S {
    #define MAX_LEN 32
    char devfs_name[MAX_LEN];
    const struct file_operations_vfs *fops;
    mode_t mode;
    void *priv;
} UMAP_DEVICE_S;

static UMAP_DEVICE_S mmz_userdev;

static int mmz_flush_dcache_mmb_dirty(struct dirty_area *p_area)
{
    if (p_area == NULL) {
        return -EINVAL;
    }

    dma_cache_clean(p_area->dirty_phys_start,
                    p_area->dirty_phys_start + p_area->dirty_size);
    dma_cache_inv(p_area->dirty_phys_start,
                  p_area->dirty_phys_start + p_area->dirty_size);

    return 0;
}

static int mmz_flush_dcache_mmb(struct mmb_info *pmi)
{
    hil_mmb_t *mmb = NULL;

    if (pmi == NULL) {
        return -EINVAL;
    }

    mmb = pmi->mmb;

    if ((mmb == NULL) || (pmi->map_cached == 0)) {
        printk("%s->%d,error!\n", __func__, __LINE__);
        return -EINVAL;
    }

    dma_cache_clean(mmb->phys_addr, mmb->phys_addr + mmb->length);
    dma_cache_inv(mmb->phys_addr, mmb->phys_addr + mmb->length);

    return 0;
}

/* this function must not called with local irq disabled. because on_each_cpu
  marco will raise ipi interrupt. */
int mmz_flush_dcache_all(void)
{
    printk("[%s], [%d] do not support in liteos.\n", __func__, __LINE__);
    return 0;
}

static int mmz_userdev_open(struct file *file)
{
    struct mmz_userdev_info *pmu = NULL;

    pmu = kmalloc(sizeof(*pmu), GFP_KERNEL);

    if (pmu == NULL) {
        error("alloc mmz_userdev_info failed!\n");
        return -ENOMEM;
    }
    memset(pmu, 0, sizeof(*pmu));

    pmu->pid = pthread_self();
    pmu->mmap_pid = 0;
    sema_init(&pmu->sem, 1);
    OSAL_INIT_LIST_HEAD(&pmu->list);

    file->f_priv = (void *)pmu;

    return 0;
}

static int ioctl_mmb_alloc(struct file *file, unsigned int iocmd, struct mmb_info *pmi)
{
    struct mmz_userdev_info *pmu = file->f_priv;
    struct mmb_info *new_mmbinfo = NULL;
    hil_mmb_t *mmb = NULL;

    mmb = hil_mmb_alloc(pmi->mmb_name, pmi->size, pmi->align, pmi->gfp, pmi->mmz_name);

    if (mmb == NULL) {
        error("hil_mmb_alloc(%s, %lu, 0x%08lX, %lu, %s) failed!\n",
              pmi->mmb_name, pmi->size, pmi->align, pmi->gfp, pmi->mmz_name);
        return -ENOMEM;
    }

    new_mmbinfo = kmalloc(sizeof(*new_mmbinfo), GFP_KERNEL);

    if (new_mmbinfo == NULL) {
        hil_mmb_free(mmb);
        error("alloc mmb_info failed!\n");
        return -ENOMEM;
    }

    memcpy(new_mmbinfo, pmi, sizeof(*new_mmbinfo));
    new_mmbinfo->phys_addr = hil_mmb_phys(mmb);
    new_mmbinfo->mmb = mmb;
    new_mmbinfo->prot = PROT_READ;
    new_mmbinfo->flags = MAP_SHARED;
    osal_list_add_tail(&new_mmbinfo->list, &pmu->list);

    pmi->phys_addr = new_mmbinfo->phys_addr;

    hil_mmb_get(mmb);

    return 0;
}

static struct mmb_info *get_mmbinfo(unsigned long addr, struct mmz_userdev_info *pmu)
{
    struct mmb_info *p = NULL;

    osal_list_for_each_entry(p, &pmu->list, list) {
        if ((addr >= p->phys_addr) && (addr < (p->phys_addr + p->size))) {
            break;
        }
    }

    if (&p->list == &pmu->list) {
        return NULL;
    }

    return p;
}

static struct mmb_info *get_mmbinfo_safe(unsigned long addr, struct mmz_userdev_info *pmu)
{
    struct mmb_info *p = NULL;

    p = get_mmbinfo(addr, pmu);

    if (p == NULL) {
        error("mmb(0x%08lX) not found!\n", addr);
        return NULL;
    }

    return p;
}

static int ioctl_mmb_user_unmap(struct file *file, unsigned int iocmd, struct mmb_info *pmi);

static int _usrdev_mmb_free(struct mmb_info *p)
{
    int ret = 0;

    osal_list_del(&p->list);
    hil_mmb_put(p->mmb);
    ret = hil_mmb_free(p->mmb);
    kfree(p);

    return ret;
}

static int ioctl_mmb_free(struct file *file, unsigned int iocmd, struct mmb_info *pmi)
{
    int ret = 0;
    struct mmz_userdev_info *pmu = file->f_priv;
    struct mmb_info *p = NULL;

    if ((p = get_mmbinfo_safe(pmi->phys_addr, pmu)) == NULL) {
        return -EPERM;
    }

    if (p->delayed_free) {
        warning("mmb<%s> is delayed_free, can not free again!\n", p->mmb->name);
        return -EBUSY;
    }

    if ((p->map_ref > 0) || (p->mmb_ref > 0)) {
        warning("mmb<%s> is still used!\n", p->mmb->name);
        p->delayed_free = 1;
        return -EBUSY;
    }

    ret = _usrdev_mmb_free(p);

    return ret;
}

static int ioctl_mmb_attr(struct file *file, unsigned int iocmd, struct mmb_info *pmi)
{
    struct mmz_userdev_info *pmu = file->f_priv;
    struct mmb_info *p = NULL;

    if ((p = get_mmbinfo_safe(pmi->phys_addr, pmu)) == NULL) {
        return -EPERM;
    }

    memcpy(pmi, p, sizeof(*pmi));
    return 0;
}

static int ioctl_mmb_user_remap(struct file *file, unsigned int iocmd, struct mmb_info *pmi, int cached)
{
    struct mmz_userdev_info *pmu = file->f_priv;
    struct mmb_info *p = NULL;

    unsigned long addr, prot, flags, pgoff;

    if ((p = get_mmbinfo_safe(pmi->phys_addr, pmu)) == NULL) {
        return -EPERM;
    }

    if (p->mapped && (p->map_ref > 0)) {
        if (cached != p->map_cached) {
            error("mmb<%s> already mapped %s, can not be remap to %s.\n", p->mmb->name,
                  p->map_cached ? "cached" : "non-cached",
                  cached ? "cached" : "non-cached");
            return -EINVAL;
        }

        p->map_ref++;
        p->mmb_ref++;

        hil_mmb_get(p->mmb);

        /*
         * pmi->phys may not always start at p->phys,
         * and may start with offset from p->phys.
         * so, we need to calculate with the offset.
         */
        pmi->mapped = (unsigned char *)p->mapped + (pmi->phys_addr - p->phys_addr);

        return 0;
    }

    if (p->phys_addr & ~PAGE_MASK) {
        return -EINVAL;
    }

    addr = 0;
    // len = p->size;
    prot = pmi->prot;
    flags = pmi->flags;

    if (prot == 0) {
        prot = p->prot;
    }

    if (flags == 0) {
        flags = p->flags;
    }

    p->map_cached = cached;

    pgoff = p->phys_addr;
    addr = pgoff;

    pmu->mmap_pid = 0;
    p->mapped = (void *)addr;

    if (cached) {
        p->mapped = osal_ioremap_cached(pmi->phys_addr, pmi->size);
    } else {
        p->mapped = osal_ioremap_nocache(pmi->phys_addr, pmi->size);
    }

    p->prot = prot;
    p->flags = flags;

    p->map_ref++;
    p->mmb_ref++;
    hil_mmb_get(p->mmb);

    /*
     * pmi->phys may not always start at p->phys,
     * and may start with offset from p->phys.
     * so, we need to calculate with the offset.
     */
    pmi->mapped = (unsigned char *)p->mapped + (pmi->phys_addr - p->phys_addr);

    return 0;
}

static int ioctl_mmb_user_unmap(struct file *file, unsigned int iocmd, struct mmb_info *pmi)
{
    int ret = 0;
    unsigned long len;
    struct mmb_info *p = NULL;
    struct mmz_userdev_info *pmu = file->f_priv;

    if ((p = get_mmbinfo_safe(pmi->phys_addr, pmu)) == NULL) {
        return -EPERM;
    }

    if (p->mapped == NULL) {
        printk(KERN_WARNING "mmb(0x%08lX) have'nt been user-mapped yet!\n", p->phys_addr);
        pmi->mapped = NULL;
        return -EIO;
    }

    if (!((p->map_ref > 0) && (p->mmb_ref > 0))) {
        error("mmb<%s> has invalid refer: map_ref = %d, mmb_ref = %d.\n", p->mmb->name, p->map_ref, p->mmb_ref);
        return -EIO;
    }

    p->map_ref--;
    p->mmb_ref--;
    hil_mmb_put(p->mmb);

    if (p->map_ref > 0) {
        return 0;
    }

    len = p->size;

    if (p->map_cached) {
        dma_cache_clean(p->phys_addr, p->phys_addr + len);
        dma_cache_inv(p->phys_addr, p->phys_addr + len);
    }

    p->mapped = NULL;
    pmi->mapped = NULL;

    if (p->delayed_free && (p->map_ref == 0) && (p->mmb_ref == 0)) {
        _usrdev_mmb_free(p);
    }

    return ret;
}

unsigned long usr_virt_to_phys(unsigned long virt)
{
    return virt;
}

static int ioctl_mmb_virt2phys(struct file *file, unsigned int iocmd, struct mmb_info *pmi)
{
    int ret = 0;
    unsigned long virt = 0, phys = 0;
    unsigned long offset = 0;
#if 1
    virt = (unsigned long)pmi->mapped;
    phys = usr_virt_to_phys(virt);

    if (!phys) {
        ret = -ENOMEM;
    }

    if (!hil_mmb_getby_phys_2(phys, &offset)) {
        return -EINVAL;
    }

    pmi->phys_addr = phys;
#endif

    return ret;
}

static int mmz_userdev_ioctl_m(struct file *file, unsigned int cmd, struct mmb_info *pmi)
{
    int ret = 0;

    switch (_IOC_NR(cmd)) {
        case _IOC_NR(IOC_MMB_ALLOC):
            ret = ioctl_mmb_alloc(file, cmd, pmi);
            break;

        case _IOC_NR(IOC_MMB_ALLOC_V2):
            // ret = ioctl_mmb_alloc_v2(file, cmd, pmi);
            printk("[%s], [%d] do not support in liteos.\n", __func__, __LINE__);
            break;

        case _IOC_NR(IOC_MMB_ATTR):
            ret = ioctl_mmb_attr(file, cmd, pmi);
            break;

        case _IOC_NR(IOC_MMB_FREE):
            ret = ioctl_mmb_free(file, cmd, pmi);
            break;

        case _IOC_NR(IOC_MMB_USER_REMAP):
            ret = ioctl_mmb_user_remap(file, cmd, pmi, 0);
            break;

        case _IOC_NR(IOC_MMB_USER_REMAP_CACHED):
            ret = ioctl_mmb_user_remap(file, cmd, pmi, 1);
            break;

        case _IOC_NR(IOC_MMB_USER_UNMAP):
            ret = ioctl_mmb_user_unmap(file, cmd, pmi);
            break;

        case _IOC_NR(IOC_MMB_VIRT_GET_PHYS):
            ret = ioctl_mmb_virt2phys(file, cmd, pmi);
            break;

        default:
            error("invalid ioctl cmd = %08X\n", cmd);
            ret = -EINVAL;
            break;
    }

    return ret;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
static int mmz_userdev_ioctl_r(struct inode *inode, struct file *file, unsigned int cmd, struct mmb_info *pmi)
#else
static int mmz_userdev_ioctl_r(struct file *file, unsigned int cmd, struct mmb_info *pmi)
#endif

{
    switch (_IOC_NR(cmd)) {
        case _IOC_NR(IOC_MMB_ADD_REF):
            pmi->mmb_ref++;
            hil_mmb_get(pmi->mmb);
            break;

        case _IOC_NR(IOC_MMB_DEC_REF):
            if (pmi->mmb_ref <= 0) {
                error("mmb<%s> mmb_ref is %d!\n", pmi->mmb->name, pmi->mmb_ref);
                return -EPERM;
            }

            pmi->mmb_ref--;
            hil_mmb_put(pmi->mmb);

            if (pmi->delayed_free && (pmi->mmb_ref == 0) && (pmi->map_ref == 0)) {
                _usrdev_mmb_free(pmi);
            }
            break;

        default:
            return -EINVAL;
            break;
    }

    return 0;
}

/* just for test */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
static int mmz_userdev_ioctl_t(struct inode *inode, struct file *file, unsigned int cmd, struct mmb_info *pmi);
#else
static int mmz_userdev_ioctl_t(struct file *file, unsigned int cmd, struct mmb_info *pmi);
#endif

static int mmz_userdev_ioctl(struct file *file, int cmd, unsigned long arg)
{
    int ret = 0;
    struct mmz_userdev_info *pmu = file->f_priv;
    down(&pmu->sem);

    if (_IOC_TYPE(cmd) == 'm') {
        struct mmb_info mi;

        if ((_IOC_SIZE(cmd) > sizeof(mi)) || (arg == 0)) {
            error("_IOC_SIZE(cmd)=%d, arg==0x%08lX\n", _IOC_SIZE(cmd), arg);
            ret = -EINVAL;
            goto __error_exit;
        }

        memset(&mi, 0, sizeof(mi));

        if (copy_from_user(&mi, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
            printk("\nmmz_userdev_ioctl: copy_from_user error.\n");
            ret = -EFAULT;
            goto __error_exit;
        }

        mi.mmz_name[HIL_MMZ_NAME_LEN - 1] = '\0';
        mi.mmb_name[HIL_MMB_NAME_LEN - 1] = '\0';

        ret = mmz_userdev_ioctl_m(file, cmd, &mi);

        if (!ret && (cmd & IOC_OUT)) {
            if (copy_to_user((void *)(uintptr_t)arg, &mi, _IOC_SIZE(cmd))) {
                printk("\nmmz_userdev_ioctl: copy_to_user error.\n");
                ret = -EFAULT;
                goto __error_exit;
            }
        }

    } else if (_IOC_TYPE(cmd) == 'r') {
        struct mmb_info *pmi;

        if ((pmi = get_mmbinfo_safe(arg, pmu)) == NULL) {
            ret = -EPERM;
            goto __error_exit;
        }

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
        ret = mmz_userdev_ioctl_r(inode, file, cmd, pmi);
#else
        ret = mmz_userdev_ioctl_r(file, cmd, pmi);
#endif

    } else if (_IOC_TYPE(cmd) == 'c') {
        struct mmb_info *pmi;

        if (arg == 0) {
            mmz_flush_dcache_all();
            goto __error_exit;
        }

        if ((pmi = get_mmbinfo_safe(arg, pmu)) == NULL) {
            ret = -EPERM;
            goto __error_exit;
        }

        switch (_IOC_NR(cmd)) {
            case _IOC_NR(IOC_MMB_FLUSH_DCACHE):
                mmz_flush_dcache_mmb(pmi);
                break;

            default:
                ret = -EINVAL;
                break;
        }

    } else if (_IOC_TYPE(cmd) == 'd') {
        hil_mmb_t *mmb = NULL;
        struct mmb_info *pmi = NULL;
        struct dirty_area area;
        unsigned long offset, orig_addr;
        unsigned long virt_addr;

        if ((_IOC_SIZE(cmd) != sizeof(area)) || (arg == 0)) {
            error("_IOC_SIZE(cmd)=%d, arg==0x%08lx\n", _IOC_SIZE(cmd), arg);
            ret = -EINVAL;
            goto __error_exit;
        }

        memset(&area, 0, sizeof(area));

        if (copy_from_user(&area, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
            printk(KERN_WARNING "\nmmz_userdev_ioctl: copy_from_user error.\n");
            ret = -EFAULT;
            goto __error_exit;
        }

        if ((mmb = hil_mmb_getby_phys_2(area.dirty_phys_start, &offset)) == NULL) {
            error("dirty_phys_addr=0x%08lx\n", area.dirty_phys_start);
            ret = -EFAULT;
            goto __error_exit;
        }

        pmi = get_mmbinfo_safe(mmb->phys_addr, pmu);

        if (pmi == NULL) {
            ret = -EPERM;
            goto __error_exit;
        }

        if ((unsigned long)(area.dirty_virt_start) != (unsigned long)pmi->mapped + offset) {
            printk(KERN_WARNING
                   "dirty_virt_start addr was not consistent with dirty_phys_start addr!\n");
            ret = -EFAULT;
            goto __error_exit;
        }

        if (area.dirty_phys_start + area.dirty_size > mmb->phys_addr + mmb->length) {
            printk(KERN_WARNING "\ndirty area overflow!\n");
            ret = -EFAULT;
            goto __error_exit;
        }

        /* cache line aligned */
        orig_addr = area.dirty_phys_start;
        area.dirty_phys_start &= ~(CACHE_LINE_SIZE - 1);
        virt_addr = (unsigned long)area.dirty_virt_start;
        virt_addr &= ~(CACHE_LINE_SIZE - 1);
        area.dirty_virt_start = (void *)virt_addr;
        // area.dirty_virt_start &= ~(CACHE_LINE_SIZE - 1);
        area.dirty_size = (area.dirty_size + (orig_addr - area.dirty_phys_start) +
                           (CACHE_LINE_SIZE - 1)) & ~(CACHE_LINE_SIZE - 1);

        mmz_flush_dcache_mmb_dirty(&area);

    } else if (_IOC_TYPE(cmd) == 't') {
        struct mmb_info mi;

        memset(&mi, 0, sizeof(mi));
        if ((void *)(uintptr_t)arg == NULL) {
            ret = -EFAULT;
            goto __error_exit;
        }

        if (copy_from_user(&mi, (void *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
            printk("\nmmz_userdev_ioctl: copy_from_user error.\n");
            ret = -EFAULT;
            goto __error_exit;
        }

        if ((get_mmbinfo_safe(mi.phys_addr, pmu)) == NULL) {
            ret = -EPERM;
            goto __error_exit;
        }

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
        ret = mmz_userdev_ioctl_t(inode, file, cmd, &mi);
#else
        ret = mmz_userdev_ioctl_t(file, cmd, &mi);
#endif

    } else {
        ret = -EINVAL;
    }

__error_exit:

    up(&pmu->sem);

    return ret;
}

static int mmz_userdev_release(struct file *file)
{
    struct mmz_userdev_info *pmu = file->f_priv;
    struct mmb_info *p = NULL, *n = NULL;

    osal_list_for_each_entry_safe(p, n, &pmu->list, list) {
        printk(KERN_ERR "MMB LEAK(pid=%d): 0x%08lX, %lu bytes, '%s'\n",
               pmu->pid, hil_mmb_phys(p->mmb),
               hil_mmb_length(p->mmb),
               hil_mmb_name(p->mmb));

        /* we do not need to release mapped-area here, system will do it for us */
        if (p->mapped != NULL) {
            printk(KERN_WARNING "mmz_userdev_release: mmb<0x%08lX> mapped to userspace 0x%p will be force unmaped!\n", p->phys_addr,
                   p->mapped);
        }

        for (; p->mmb_ref > 0; p->mmb_ref--) {
            hil_mmb_put(p->mmb);
        }

        _usrdev_mmb_free(p);
    }

    file->f_priv = NULL;
    sema_destory(&pmu->sem);

    kfree(pmu);

    return 0;
}

const static struct file_operations_vfs mmz_user_fops = {
    .open = mmz_userdev_open,
    .close = mmz_userdev_release,
    .ioctl = mmz_userdev_ioctl,
};

int __init mmz_userdev_init(void)
{
    int ret = 0;

    snprintf(mmz_userdev.devfs_name, 32, "%s", "/dev/mmz_userdev");
    mmz_userdev.fops = &mmz_user_fops;
    mmz_userdev.mode = 0666;
    mmz_userdev.priv = NULL;

    ret = register_driver(mmz_userdev.devfs_name, mmz_userdev.fops, mmz_userdev.mode, NULL);

    return ret;
}

void __exit mmz_userdev_exit(void)
{
    unregister_driver(mmz_userdev.devfs_name);
}

/* Test func */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
static int mmz_userdev_ioctl_t(struct inode *inode, struct file *file, unsigned int cmd, struct mmb_info *pmi)
#else
static int mmz_userdev_ioctl_t(struct file *file, unsigned int cmd, struct mmb_info *pmi)
#endif
{
    return 0;
}

