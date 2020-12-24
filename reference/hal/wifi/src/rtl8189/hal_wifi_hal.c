/**
* @file    hal_wifi_hal.c
* @brief   hal wifi hal implemention
*
* Copyright (c) 2017 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date      2017/12/25
* @version

*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#ifdef __LITEOS__
#include "rtw_intf.h"
#endif
#include "hi_hal_wifi_inner.h"
#include "hi_math.h"
#include "hi_type.h"
#include "hi_appcomm_util.h"
#include "hi_appcomm_log.h"
#include "hi_hal_wifi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif  /* End of #ifdef __cplusplus */

#ifdef __LITEOS__
static HI_VOID* liteos_rtw_wifi_open(HI_VOID* arg)
{
    struct netif *rtw_netif;
    HI_S32 s32Ret = rtw_wifi_open(&rtw_netif);
    if (s32Ret!=HI_SUCCESS||rtw_netif==NULL)
    {
        HI_LOG_PrintFuncErr(rtw_wifi_open,s32Ret);
    }
    return NULL;
}

static HI_VOID* liteos_rtw_wifi_close(HI_VOID* arg)
{
    rtw_wifi_close();
    return NULL;
}

HI_S32 liteos_priority_task(HI_S32 priority,HI_VOID *(*start_routine) (HI_VOID *), HI_VOID *arg)
{
    pthread_t thread;
    pthread_attr_t attr;
    (HI_VOID)pthread_attr_init(&attr);
    attr.inheritsched = PTHREAD_EXPLICIT_SCHED;
    attr.schedparam.sched_priority = priority;
    if(HI_SUCCESS != pthread_create(&thread, &attr, start_routine, arg))
    {
        MLOGE("pthread_create fail,errno=%d:%s",errno,strerror(errno));
        return HI_FAILURE;
    }
    (HI_VOID)pthread_attr_destroy(&attr);
    (HI_VOID)pthread_join(thread,NULL);
    return HI_SUCCESS;
}
#endif

HI_S32 HAL_WIFI_HAL_PowerOnReset(HI_VOID)
{
    return HI_SUCCESS;    /** 8189 need not power on reset*/
}

HI_VOID HAL_WIFI_HAL_SysPreInit(HI_VOID)
{
    static HI_BOOL bPinOutInit = HI_FALSE;
    if(HI_TRUE == bPinOutInit)
    {
        MLOGD("hal_system_preinit already init\n");
        return;
    }
    MLOGD("hal_system_preinit init register\n");
    himm(0x112F0008,0x0681);
    himm(0x112F000C,0x0581);
    himm(0x112F0010,0x0581);
    himm(0x112F0014,0x0581);
    himm(0x112F0018,0x0581);
    himm(0x112F001C,0x0581);
}

HI_S32 HAL_WIFI_HAL_LoadAPDriver(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;

  /*  s32Ret = HI_insmod(HI_APPFS_KOMOD_PATH"/cfg80211.ko","");
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE("insmod ap: cfg80211 failed, errno(%d)\n", errno);
        return HI_FAILURE;
    }
    sleep(1);*/
#ifndef __LITEOS__
        MLOGE("__LITEOS__ Wifi\n\n");
    #if defined HI3559V200
            s32Ret = HI_insmod(HI_APPFS_KOMOD_PATH"/8189fs.ko","sdio_slot=1");
    #else
        MLOGE("please point chip type,fail\n\n");
    #endif
#else
        MLOGE("liteos_priority_task\n\n");
    s32Ret = liteos_priority_task(5, liteos_rtw_wifi_open, NULL);
#endif
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE("insmod ap: 8189fs failed, errno(%d)\n", errno);
        return HI_FAILURE;
    }
    else
    {
        MLOGE("insmod ap: 8189fs pass\n");
    }
    
    sleep(1);
    return HI_SUCCESS;
}


HI_S32 HAL_WIFI_HAL_LoadSTADriver(HI_VOID)
{
    MLOGE("insmod sta mode driver not support,failed\n");
    return HI_FAILURE;
}

HI_S32 HAL_WIFI_HAL_RemoveDriver(HI_VOID)
{
    HI_S32 s32Ret = 0;
#ifndef __LITEOS__
    s32Ret = HI_rmmod(HI_APPFS_KOMOD_PATH"/8189fs.ko");
#else
    s32Ret = liteos_priority_task(5, liteos_rtw_wifi_close, NULL);
#endif
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE("insmod ap: failed, errno(%d)\n", errno);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
HI_S32 HAL_WIFI_HAL_UpdateApConfig(HI_CHAR *ifname,  const HI_HAL_WIFI_APMODE_CFG_S *pstApCfg, const HI_CHAR *pszConfigFile)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32fd;
    HI_CHAR* szWbuf = NULL;
    /** open configure file, if not exist, create it */
    s32fd = open(pszConfigFile, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(s32fd < 0)
    {
        MLOGE("WiFi: Cann't open configure file '%s',errno(%d)", pszConfigFile,errno);
        return HI_FAILURE;
    }

    asprintf(&szWbuf, "interface=%s\n"
                    "driver=%s\n"
                    "ctrl_interface=/dev/wifi/hostapd\n"
                    "ssid=%s\n"
                    "channel=%d\n"
                    "ignore_broadcast_ssid=%d\n"
                    "hw_mode=g\n"
                    "ieee80211n=1\n"
                    "ht_capab=[SHORT-GI-20][SHORT-GI-40]%s\n",
                    ifname, "nl80211", pstApCfg->stCfg.szWiFiSSID, pstApCfg->s32Channel,
                    pstApCfg->bHideSSID? 1 : 0,
                    (pstApCfg->s32Channel > 4) ? "[HT40-]" : "[HT40+]");

    if (write(s32fd, szWbuf, strlen(szWbuf)) < 0) {
        MLOGE("WiFi: Cann't write configuration to '%s',errno(%d)\n", pszConfigFile,errno);
        s32Ret = HI_FAILURE;
    }
    close(s32fd);
    free(szWbuf);
    return s32Ret;
}
HI_S32 HAL_WIFI_HAL_UpdateStaConfig(HI_CHAR *ifname,  const HI_HAL_WIFI_APMODE_CFG_S *pstStaCfg, const HI_CHAR *pszConfigFile)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32fd;
    HI_CHAR* szWbuf = NULL;
    s32fd = open(pszConfigFile, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(s32fd < 0)
    {
        MLOGE("WiFi: Cann't open configure file '%s',errno(%d)", pszConfigFile,errno);
        return HI_FAILURE;
    }

   /* asprintf(&szWbuf, "interface=%s\n"
                    "driver=%s\n"
                    "ctrl_interface=/var/run/wpa_supplicant\n"
                    "ssid=%s\n"
                    "channel=%d\n"
                    "ignore_broadcast_ssid=%d\n"
                    "hw_mode=g\n"
                    "ieee80211n=1\n"
                    "ht_capab=[SHORT-GI-20][SHORT-GI-40]%s\n",
                    ifname, "nl80211", pstApCfg->stCfg.szWiFiSSID, pstApCfg->s32Channel,
                    pstApCfg->bHideSSID? 1 : 0,
                    (pstApCfg->s32Channel > 4) ? "[HT40-]" : "[HT40+]");*/
     asprintf(&szWbuf,"interface=%s\n"
                    "ctrl_interface=/var/run/wpa_supplicant\n"
                    "#update_config=1"
                    "ap_scan=1"
                    "device_name=DHS2.0"
                    "device_type=1-0050F204-1"
                    "p2p_go_intent=0"
                    "ssid=%s\n"
                    "channel=%d\n"
                    "ignore_broadcast_ssid=%d\n"
                    "hw_mode=g\n"
                    "ieee80211n=1\n",
                    ifname,pstStaCfg->stCfg.szWiFiSSID, pstStaCfg->s32Channel,
                    pstStaCfg->bHideSSID? 1 : 0);

    if (write(s32fd, szWbuf, strlen(szWbuf)) < 0) {
        MLOGE("WiFi: Cann't write configuration to '%s',errno(%d)\n", pszConfigFile,errno);
        s32Ret = HI_FAILURE;
    }
    close(s32fd);
    free(szWbuf);
    return s32Ret;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* End of #ifdef __cplusplus */
