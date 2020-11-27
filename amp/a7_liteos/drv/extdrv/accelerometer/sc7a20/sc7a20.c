#include <linux/kernel.h>
#include <linux/fcntl.h>
#ifndef __LITEOS__
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/poll.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#else
#include "asm-generic/ioctl.h"
#include <i2c.h>
#endif
#include "i2cdev.h"

#define SENSITIVITY             _IOWR('w', 4, unsigned int)
#define EXCEED_THRESHOLD_FLAG   _IOWR('r', 5, unsigned int)

static int I2C_NUM = -1;
#ifndef __LITEOS__
module_param(I2C_NUM, int, 0644);
MODULE_PARM_DESC(I2C_NUM, "i2c number");
#endif

static int LEVEL = 0;
#ifndef __LITEOS__
module_param(LEVEL, int, 0644);
MODULE_PARM_DESC(LEVEL, "electrical level");
#endif


#define i2c_write_byte(X,Y) \
do{  \
    hi_i2c_write(X,1,Y,1);\
}while(0)

#define print_info(fmt, arg...)   \
    do{  \
        printk("fun:%s,%d "fmt, __func__,__LINE__, ##arg);  \
    }while(0)

#ifndef __LITEOS__
static const struct i2c_device_id mc_id[] =
{
    { "sc7a20", },
    { }
};

MODULE_DEVICE_TABLE(i2c, mc_id);
#endif

static void sc7a20_init(void)
{
    i2c_write_byte(0x24, 0x80);
    i2c_write_byte(0x20, 0x37);
    i2c_write_byte(0x23, 0x80);
    i2c_write_byte(0x3a, 0x30);
    i2c_write_byte(0x3b, 0x7f);
    i2c_write_byte(0x3c, 0x64);// 40 * 0x64 =  4s
    i2c_write_byte(0x22, 0x00);
    i2c_write_byte(0x38, 0x00);
    i2c_write_byte(0x21, 0x04);
    i2c_write_byte(0x25, 0x00);  /**high level */
}

static void sc7a20_onoff_interrupt(unsigned char onoff)// 1 on 0 off
{
     if(onoff)
     {
         //i2c_write_byte(0x07,0x03);  /**standby mode*/
         i2c_write_byte(0x22, 0x80);
         i2c_write_byte(0x38, 0x15);
     }
     else
     {
         i2c_write_byte(0x22, 0x00);
         i2c_write_byte(0x38, 0x00);
     }
}

static void sc7a20_process(unsigned int proess_value,unsigned int *con_value)
{
     unsigned char value = 0;

     if(SENSITIVITY == proess_value)
     {
         if(0 == *con_value)
         {
             sc7a20_onoff_interrupt(0);
         }
         else
         {
             sc7a20_onoff_interrupt(1);
             if(1 == *con_value)/**senstivity low*/
             {
                 i2c_write_byte(0x3a, 0x46);
             }
             else if(2 == *con_value)/**senstivity middle*/
             {
                 i2c_write_byte(0x3a, 0x3c);
             }
             else/**senstivity heigh*/
             {
                 i2c_write_byte(0x3a, 0x28);
             }
         }
     }
     else if(EXCEED_THRESHOLD_FLAG == proess_value)
     {
         hi_i2c_read(0x39,&value);
         if(value & 0x40)
         {
            value = 0x1;
         }
         else
         {
             value = 0x0;
         }

         *con_value = value;
     }
}

#ifndef __LITEOS__
static long gsensor_ioctl(struct file* filp, unsigned int cmd, unsigned long arg)
#else
static int gsensor_ioctl(struct file* filp, int cmd, unsigned long arg)
#endif
{
    unsigned int value = 0;
    int ret = 0;

    switch (_IOC_NR(cmd))
    {
        case _IOC_NR(SENSITIVITY):
            ret = copy_from_user(&value, (unsigned int*)arg, sizeof(unsigned int));
            sc7a20_process(SENSITIVITY,&value);
            break;

        case _IOC_NR(EXCEED_THRESHOLD_FLAG):
            sc7a20_process(EXCEED_THRESHOLD_FLAG,&value);
            ret = copy_to_user((void __user*)arg, &value, sizeof(unsigned int));
            if (0 != ret)
            {
                 return -1;
            }

            break;
    }
    return 0;

}

#ifndef __LITEOS__
static int gsensor_open(struct inode* inode, struct file* filp)
#else
static int gsensor_open(struct file* filp)
#endif
{
    return 0;
}

#ifndef __LITEOS__
static int gsensor_close(struct inode* inode, struct file* filp)
#else
static int gsensor_close(struct file* filp)
#endif
{
    return 0;
}

#ifndef __LITEOS__
static struct file_operations gsensor_fops = {
    .owner      = THIS_MODULE,
    .unlocked_ioctl = gsensor_ioctl,
    .open       = gsensor_open,
    .release    = gsensor_close
};
static struct miscdevice gsensor_dev = {
   .minor       = MISC_DYNAMIC_MINOR,
   .name        = "gsensor",
   .fops        = &gsensor_fops,
};
#else
static const struct file_operations_vfs gsensor_fops =
{
    .open  = gsensor_open,       /* open */
    .close = gsensor_close,    /* close */
    .ioctl = gsensor_ioctl,       /* ioctl */
};
#endif

#ifndef __LITEOS__
static int __init gsesnor_init(void)
{
#else
int __init gsesnor_init(int i2c_num,int level)
{
    LEVEL = level;
    I2C_NUM = i2c_num;
#endif
    int ret;
    hi_i2cdev_init(I2C_NUM);
    sc7a20_init();
#ifndef __LITEOS__
    ret = misc_register(&gsensor_dev);
#else
    ret = register_driver("/dev/gsensor", &gsensor_fops, 0666, 0);
#endif
    if(ret<0)
    {
        print_info("gsesnor_dev_register fail!\n");
        return -1;
    }
    else
    {
        print_info("gsesnor_dev_register ok!\n");
    }
    return 0;
}

void __exit gsesnor_exit(void)
{
    print_info("gsesnor driver: exit\n");
#ifndef __LITEOS__
    misc_deregister(&gsensor_dev);
#else
    unregister_driver("/dev/gsensor");
#endif
    hi_i2cdev_exit();
}

#ifndef __LITEOS__
module_init(gsesnor_init);
module_exit(gsesnor_exit);
MODULE_AUTHOR("monster");
MODULE_DESCRIPTION("gsensor driver");
MODULE_LICENSE("GPL");
#endif
