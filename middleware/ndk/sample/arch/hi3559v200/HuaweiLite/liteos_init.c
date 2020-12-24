/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    liteos_init.c
 * @brief   liteos init function
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */

#include "sys_config.h"
#include "board.h"
#include "los_typedef.h"

#include "stdlib.h"
#include "stdio.h"

#include "mpi_sys.h"
#include "hi_common.h"
#include "hi_mapi_log.h"
#include "hi_mapi_sys.h"
#include "sample_comm.h"

#include "hi_mapi_hal_ahd.h"
#include "hi_mapi_hal_ahd_define.h"

#ifdef LOSCFG_DRIVERS_VIDEO
#include "linux/fb.h"
#endif
#ifdef LOSCFG_SHELL
#include "shell.h"
#include "shcmd.h"

extern HI_VOID Load_sdk(void);
extern HI_VOID CatLogShell(HI_VOID);
extern HI_VOID FASTBOOT_Sample(HI_VOID);
extern HI_VOID Liteos_Test(HI_VOID);

#endif

#ifdef LOSCFG_DRIVERS_UART
#include "console.h"
#include "hisoc/uart.h"
#include "uart.h"
#endif

#if defined(LOSCFG_DRIVERS_HIGMAC) || defined(LOSCFG_DRIVERS_HIETH_SF)
#include "eth_drv.h"
#endif
#ifndef LOSCFG_CORTEX_M7
extern AARCHPTR g_sys_mem_addr_end;
void board_config(void)
{
    g_sys_mem_addr_end = SYS_MEM_BASE + SYS_MEM_SIZE_DEFAULT;
#ifdef LOSCFG_DRIVERS_USB
    extern unsigned long g_usb_mem_addr_start;
    extern unsigned long g_usb_mem_size;
    g_usb_mem_addr_start = g_sys_mem_addr_end;
    g_usb_mem_size = 0x20000;  // recommend 128K nonCache for usb
#endif
#ifdef LOSCFG_DRIVERS_EMMC
    size_t part0_start_sector = 16 * (0x100000 / 512);
    size_t part0_count_sector = 1024 * (0x100000 / 512);
    size_t part1_start_sector = 16 * (0x100000 / 512) + part0_count_sector;
    size_t part1_count_sector = 1024 * (0x100000 / 512);
    extern struct disk_divide_info emmc;
    add_mmc_partition(&emmc, part0_start_sector, part0_count_sector);
    add_mmc_partition(&emmc, part1_start_sector, part1_count_sector);
#endif
#if defined(LOSCFG_DRIVERS_HIGMAC) || defined(LOSCFG_DRIVERS_HIETH_SF)
    //different board should set right mode:"rgmii" "rmii" "mii"
    //if you don't set :
#if defined(LOSCFG_PLATFORM_HI3559AV100) || defined(LOSCFG_PLATFORM_HI3519AV100)
    hisi_eth_set_phy_mode("rgmii");
#endif
#if defined(LOSCFG_PLATFORM_HI3559AV100)
    // use eth1 higmac,the phy addr is 3
    hisi_eth_set_phy_addr(3);  // 0~31
#endif
    // different board should set right addr:0~31
    // if you don't set ,driver will detect it automatically
    // hisi_eth_set_phy_addr(0);//0~31
#endif
}
#endif

#ifdef LOSCFG_DRIVERS_NETDEV
#include "lwip/tcpip.h"
#include "lwip/netif.h"

void net_init(void)
{
    struct netif *pnetif;

    extern int secure_func_register(void);
    (void)secure_func_register();
    extern void tcpip_init(tcpip_init_done_fn initfunc, void *arg);
    tcpip_init(NULL, NULL);
    static unsigned int overtime = 0;
    PRINTK("Ethernet start.");

#ifdef LOSCFG_DRIVERS_HIGMAC
    extern int /* __init */ ethnet_higmac_init(void);
    (void)ethnet_higmac_init();
#endif
    extern void get_defaultNetif(struct netif * *pnetif);
    get_defaultNetif(&pnetif);

    (void)netifapi_netif_set_up(pnetif);
    do {
        LOS_Msleep(60);
        overtime++;
        if (overtime > 100) {
            PRINTK("netif_is_link_up overtime!\n");
            break;
        }
    } while (netif_is_link_up(pnetif) == 0);
}
#endif

extern int app_main(int argc, char *argv[]);

#define ARGS_SIZE_T   20
#define ARG_BUF_LEN_T 256

#if 1
void media_sys_reset(void)
{
    if (0 != HI_MAPI_Sys_Deinit()) {
        dprintf("reset media sys --> deinit fail...\n");
    }
    if (0 != HI_MAPI_Sys_Init()) {
        dprintf("reset media sys --> init fail...\n");
    }
}

void bind_acap_ao(int argc, const char **argv)
{
    MPP_CHN_S stSrcChn, stDestChn;
    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32DevId = atoi(argv[0]);
    stSrcChn.s32ChnId = 0;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = atoi(argv[1]);
    stDestChn.s32ChnId = 0;

    if (0 != HI_MPI_SYS_Bind(&stSrcChn, &stDestChn)) {
        dprintf("HI_MPI_SYS_Bind fail...\n");
    }
}

void log_debug(void)
{
    HI_MAPI_LOG_SetEnabledLevel(HI_MAPI_LOG_LEVEL_DEBUG);
}

void cmd_register(void)
{
    osCmdReg(CMD_TYPE_EX, "media_sys_reset", 0, (CMD_CBK_FUNC)media_sys_reset);
    osCmdReg(CMD_TYPE_EX, "bind_acap_ao", 0, (CMD_CBK_FUNC)bind_acap_ao);
    osCmdReg(CMD_TYPE_EX, "log_debug", 0, (CMD_CBK_FUNC)log_debug);
    osCmdReg(CMD_TYPE_EX, "fastboot_test", 0, (CMD_CBK_FUNC)FASTBOOT_Sample);
}
#endif


void sample_command(void)
{
    osCmdReg(CMD_TYPE_EX, "sample", 0, (CMD_CBK_FUNC)app_main);
}

HI_S32 VCAP_MST_AhdInit(struct hi_HAL_AHD_DEV_S* pstHalAhdDev)
{
   printf("func:%s, line:%d \n", __FUNCTION__, __LINE__);
   return HI_SUCCESS;
}

HI_S32 VCAP_MST_AhdDeinit(struct hi_HAL_AHD_DEV_S* pstHalAhdDev)
{
    printf("func:%s, line:%d \n", __FUNCTION__, __LINE__);
    return HI_SUCCESS;
}

HI_S32 VCAP_MST_AhdStart( struct hi_HAL_AHD_DEV_S* pstHalAhdDev)
{
    printf("func:%s, line:%d \n", __FUNCTION__, __LINE__);
    return HI_SUCCESS;
}

HI_S32 VCAP_MST_AhdStop( struct hi_HAL_AHD_DEV_S* pstHalAhdDev)
{
    printf("func:%s, line:%d \n", __FUNCTION__, __LINE__);
    return HI_SUCCESS;
}

HI_S32 VCAP_MST_AhdStartChn( struct hi_HAL_AHD_DEV_S* pstHalAhdDev, HI_U32 u32Chn)
{
    printf("func:%s, line:%d, chn:%d \n", __FUNCTION__, __LINE__, u32Chn);
    return HI_SUCCESS;
}

HI_S32 VCAP_MST_AhdStopChn( struct hi_HAL_AHD_DEV_S* pstHalAhdDev, HI_U32 u32Chn)
{
    printf("func:%s, line:%d, chn:%d  \n", __FUNCTION__, __LINE__, u32Chn);
    return HI_SUCCESS;
}

HI_S32 VCAP_MST_AhdSetAttr( struct hi_HAL_AHD_DEV_S* pstHalAhdDev, const HI_HAL_AHD_CFG_S* pstAHDCfg)
{
    printf("func:%s, line:%d \n", __FUNCTION__, __LINE__);
    return HI_SUCCESS;
}

HI_S32 VCAP_MST_AhdGetAttr( struct hi_HAL_AHD_DEV_S* pstHalAhdDev, HI_HAL_AHD_CFG_S* pstAHDCfg)
{
    printf("func:%s, line:%d \n", __FUNCTION__, __LINE__);
    return HI_SUCCESS;
}

HI_S32 VCAP_MST_AhdGetStatus( struct hi_HAL_AHD_DEV_S* pstHalAhdDev, HI_HAL_AHD_STATUS_E astAhdStatus[], HI_U32 u32Cnt)
{
    printf("func:%s, line:%d , u32Cnt:%d \n", __FUNCTION__, __LINE__, u32Cnt);
    int i;
    for(i = 0; i < u32Cnt; i++)
    {
        astAhdStatus[i] = i%2;
    }

    return HI_SUCCESS;
}


void app_init(void)
{
#ifdef LOSCFG_PLATFORM_HISI_AMP
    extern int _ipcm_vdd_init(void);
    dprintf("ipcm init ...\n");
    _ipcm_vdd_init();

#ifndef LOSCFG_PLATFORM_HI3559AV100_CORTEX_M7
    extern int sharefs_client_init(const char *path);
    dprintf("sharefs init ...\n");
    sharefs_client_init("/sharefs");
#endif

    extern int virt_tty_dev_init(void);
    dprintf("virt tty init ...\n");
    virt_tty_dev_init();
#endif

#ifdef LOSCFG_DRIVERS_UART
    dprintf("uart init ...\n");
    if (uart_dev_init() != 0) {
        PRINT_ERR("uart_dev_init failed");
    }
    if (virtual_serial_init(TTY_DEVICE) != 0) {
        PRINT_ERR("virtual_serial_init failed");
    }
    if (system_console_init(SERIAL) != 0) {
        PRINT_ERR("system_console_init failed\n");
    }

#endif

#ifdef LOSCFG_DRIVERS_RANDOM
    dprintf("random dev init ...\n");
    extern int ran_dev_register(void);
    ran_dev_register();
#endif
#ifdef LOSCFG_DRIVERS_MMC
    dprintf("MMC dev init ...");
    extern int SD_MMC_Host_init(void);
    SD_MMC_Host_init();
#endif

#ifdef LOSCFG_DRIVERS_MEM
    dprintf("mem dev init ...\n");
    extern int mem_dev_register(void);
    mem_dev_register();
#endif

#ifdef LOSCFG_FS_PROC
    dprintf("porc fs init ...\n");
    extern void proc_fs_init(void);
    proc_fs_init();
#endif

#ifdef LOSCFG_DRIVERS_SPI
    dprintf("spi bus init ...\n");
    extern int spi_dev_init(void);
    spi_dev_init();
#endif

#ifdef LOSCFG_DRIVERS_I2C
    dprintf("i2c bus init ...\n");
    extern int i2c_dev_init(void);
    i2c_dev_init();
#endif

#ifdef LOSCFG_DRIVERS_GPIO
    dprintf("gpio init ...\n");
    extern int gpio_dev_init(void);
    gpio_dev_init();
#endif

#ifdef LOSCFG_DRIVERS_HIEDMAC
    extern int hiedmac_init(void);
    hiedmac_init();
#endif
#ifdef LOSCFG_DRIVERS_HIDMAC
    dprintf("dmac init ...\n");
    extern int hi_dmac_init(void);
    hi_dmac_init();
#endif

#ifdef LOSCFG_DRIVERS_MTD_NAND
    dprintf("nand init ...\n");
    extern int nand_init(void);

    if (!nand_init()) {
        extern int add_mtd_partition(char *type, UINT32 start_addr, UINT32 length, UINT32 partition_num);
        add_mtd_partition("nand", 0x200000, 32 * 0x100000, 0);
        add_mtd_partition("nand", 0x200000 + 32 * 0x100000, 32 * 0x100000, 1);
        extern int mount(const char *source, const char *target,
                         const char *filesystemtype, unsigned long mountflags,
                         const void *data);
        mount("/dev/nandblk0", "/yaffs0", "yaffs", 0, NULL);
        // mount("/dev/nandblk1", "/yaffs1", "yaffs", 0, NULL);
    }

#endif

#ifdef LOSCFG_DRIVERS_MTD_SPI_NOR
    dprintf("spi nor flash init ...\n");
    extern int spinor_init(void);

    if (!spinor_init()) {
        extern int add_mtd_partition(char *type, UINT32 start_addr, UINT32 length, UINT32 partition_num);
        add_mtd_partition("spinor", 0x100000, 2 * 0x100000, 0);
        add_mtd_partition("spinor", 3 * 0x100000, 2 * 0x100000, 1);
        extern int mount(const char *source, const char *target,
                         const char *filesystemtype, unsigned long mountflags,
                         const void *data);
        mount("/dev/spinorblk0", "/jffs0", "jffs", 0, NULL);
        // mount("/dev/spinorblk1", "/jffs1", "jffs", 0, NULL);
    }
#endif

#if 0//def LOSCFG_DRIVERS_NETDEV
    dprintf("net init ...\n");
    net_init();
#endif

    dprintf("g_sys_mem_addr_end=0x%p,\n", g_sys_mem_addr_end);
    dprintf("Date:%s.\n", __DATE__);
    dprintf("Time:%s.\n", __TIME__);

    Load_sdk();

    /* for tmp debug */
    cmd_register();
    dprintf("cmd register ...\n");

    HI_MAPI_LOG_SetEnabledLevel(HI_MAPI_LOG_LEVEL_WARN);
    dprintf("set debug level ...\n");

#ifdef LOSCFG_PLATFORM_HISI_AMP
    if (0 != HI_MAPI_Sys_Init()) {
        dprintf("server mapi sys init fail ...\n");
    }

    dprintf("server mapi sys init complete ...\n");

    HI_HAL_AHD_DEV_S stAhdDev;
    stAhdDev.id = 0;
    stAhdDev.u32ChnMax = 2;
    memset(stAhdDev.bUsedChn, HI_FALSE, sizeof(HI_BOOL)*HI_HAL_AHD_CHN_MAX);
    stAhdDev.bUsedChn[0] = HI_TRUE;
    stAhdDev.pfnAhdInit = VCAP_MST_AhdInit;
    stAhdDev.pfnAhdDeinit = VCAP_MST_AhdDeinit;
    stAhdDev.pfnAhdStart = VCAP_MST_AhdStart;
    stAhdDev.pfnAhdStop = VCAP_MST_AhdStop;
    stAhdDev.pfnAhdStartChn = VCAP_MST_AhdStartChn;
    stAhdDev.pfnAhdStopChn = VCAP_MST_AhdStopChn;
    stAhdDev.pfnAhdSetAttr = VCAP_MST_AhdSetAttr;
    stAhdDev.pfnAhdGetAttr = VCAP_MST_AhdGetAttr;
    stAhdDev.pfnAhdGetStatus = VCAP_MST_AhdGetStatus;
    HI_MAPI_VCAP_RegAhdModule(1, &stAhdDev);
    dprintf("server mapi reg ahd module complete ...\n");

#else
    sample_command();
#endif

#ifndef __HI3556AV100__

    if (0 != LOS_PathAdd("/sharefs/so")) {
        printf("add path /sharefs/so failed \n");
        return;
    }


#endif

    CatLogShell();
    return;
}
