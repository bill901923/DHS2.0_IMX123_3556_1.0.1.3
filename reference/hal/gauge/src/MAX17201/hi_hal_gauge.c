/**
* @file    hi_hal_gauge.c
* @brief   hal gauge implemention
*
* Copyright (c) 2017 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date      2017/12/17
* @version   1.0

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "hi_hal_common_inner.h"

#include "hi_type.h"
#include "hi_hal_gauge.h"
#include "hi_appcomm_log.h"
#include "hi_appcomm_util.h"
#include "hi_hal_common.h"


#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif  /* End of #ifdef __cplusplus */


#define GAUGE_VOLTGE        _IOWR('r', 5, unsigned int)
#define GAUGE_CAPACITY      _IOWR('r', 6, unsigned int)
#define GAUGE_CONFIG_Read   _IOWR('r', 7, unsigned int)
#define GAUGE_ChargeState   _IOWR('r', 8, unsigned int)
#define GAUGE_CURRENT       _IOWR('r', 9, unsigned int)

/** macro define */
#define HAL_GAUGE_DEV    ("/dev/gauge")
static HI_S32 s_s32HALGAUGEfd = HAL_FD_INITIALIZATION_VAL;


HI_S32 HI_HAL_GAUGE_Init(HI_VOID)
{
    if (HAL_FD_INITIALIZATION_VAL == s_s32HALGAUGEfd)
    {
	    himm(0x114F000C ,0x603); /**< #pinout reuse as SDA*/
    	himm(0x114F0008  ,0x603); /**< #pinout reuse as SCL*/
        /**First Load KO*/
        HI_S32 s32Ret = HI_SUCCESS;
	
    	s32Ret = HI_insmod(HI_APPFS_KOMOD_PATH"/MAX_17201.ko",NULL);
        if(HI_SUCCESS != s32Ret)
        {
            MLOGE("insmod MAX17201:failed  %d\n",HAL_FD_INITIALIZATION_VAL);
            return HI_HAL_EINTER;
        }

        s_s32HALGAUGEfd = open(HAL_GAUGE_DEV, O_RDWR);
        if (HAL_FD_INITIALIZATION_VAL == s_s32HALGAUGEfd)
        {
            MLOGE("[Error] open gauge failed\n");
            return HI_HAL_EINVOKESYS;
        }

    }
    else
    {
        MLOGE("[Warning] gauge already be opened\n");
        return HI_HAL_EINITIALIZED;
    }
    return HI_SUCCESS;
}
HI_S32 HI_HAL_GAUGE_GetCURRENT(HI_S32* ps32CURRENT) /* percent*/
{
    HI_S32 s32Ret = 0;
    HI_S32 s32CURRENT = 0;
    if (NULL == ps32CURRENT)
    {
        MLOGE("ps32CURRENT is null\n");
        return HI_HAL_EINVAL;
    }

    /* init check */
    if (HAL_FD_INITIALIZATION_VAL == s_s32HALGAUGEfd)
    {
        MLOGE("gauge not initialized\n");
        return HI_HAL_ENOINIT;
    }

    s32Ret = ioctl(s_s32HALGAUGEfd, GAUGE_CURRENT, &s32CURRENT);

    if(HI_FAILURE == s32Ret)
    {
        MLOGE("get CURRENT error\n");
        return HI_HAL_EINTER;
    }

    *ps32CURRENT = s32CURRENT;

    return HI_SUCCESS;
}

HI_S32 HI_HAL_GAUGE_GetLevel(HI_S32* ps32Level) /* percent*/
{
    HI_S32 s32Ret = 0;
    HI_S32 s32Level = 0;
    //HI_S32 s32val = 0;
    /* parm ps32Level check */
    if (NULL == ps32Level)
    {
        MLOGE("ps32Level is null\n");
        return HI_HAL_EINVAL;
    }

    /* init check */
    if (HAL_FD_INITIALIZATION_VAL == s_s32HALGAUGEfd)
    {
        MLOGE("gauge not initialized\n");
        return HI_HAL_ENOINIT;
    }

  /*  s32Ret = ioctl(s_s32HALGAUGEfd, GAUGE_VOLTGE, &s32val);
   //s32Ret = ioctl(s_s32HALGAUGEfd, IOCTL_CMD_READ_CAPACITY, &s32Level);
    if(HI_FAILURE == s32Ret)
    {
        MLOGE("get VOLTGE error\n");
        return HI_HAL_EINTER;
    }*/

      //  MLOGE("BYE HI_HAL_GAUGE_VOLTGE %d\n",s32val);

    s32Ret = ioctl(s_s32HALGAUGEfd, GAUGE_CAPACITY, &s32Level);
   //s32Ret = ioctl(s_s32HALGAUGEfd, IOCTL_CMD_READ_CAPACITY, &s32Level);
    if(HI_FAILURE == s32Ret)
    {
        MLOGE("get Level error\n");
        return HI_HAL_EINTER;
    }

    //    MLOGE("BYE HI_HAL_GAUGE_GetLevel %d\n",s32Level);
    *ps32Level = s32Level;

    return HI_SUCCESS;
}

HI_S32 HI_HAL_GAUGE_GetChargeState(HI_BOOL* pbCharge)
{
    HI_S32 s32Ret = 0;
    HI_S32 s32val = 0;
    HI_BOOL bCharge = HI_FALSE;
    /* parm ps32Level check */
    if (NULL == pbCharge)
    {
        MLOGE("pbCharge is null\n");
        return HI_HAL_EINVAL;
    }

    /* init check */
    if (HAL_FD_INITIALIZATION_VAL == s_s32HALGAUGEfd)
    {
        MLOGE("gauge not initialized\n");
        return HI_HAL_ENOINIT;
    }

    s32Ret = ioctl(s_s32HALGAUGEfd, GAUGE_ChargeState, &s32val);
   //s32Ret = ioctl(s_s32HALGAUGEfd, IOCTL_CMD_READ_CAPACITY, &s32Level);
    if(HI_FAILURE == s32Ret)
    {
        MLOGE("get Charge State error\n");
        return HI_HAL_EINTER;
    }
    if(s32val)
    {
            bCharge = HI_TRUE;
    }
    else
    {
        bCharge = HI_FALSE;
    }
    
    *pbCharge = bCharge;

    return HI_SUCCESS;
}


HI_S32 HI_HAL_GAUGE_Deinit(HI_VOID)
{	
   HI_S32 s32Ret;

    if (HAL_FD_INITIALIZATION_VAL == s_s32HALGAUGEfd)
    {
        MLOGE("gauge not initialized,no need to close\n");
        return HI_HAL_ENOINIT;
    }
   close(s_s32HALGAUGEfd);
 
    s_s32HALGAUGEfd = HAL_FD_INITIALIZATION_VAL;
#ifndef __LITEOS__
    s32Ret = HI_rmmod(HI_APPFS_KOMOD_PATH"/MAX17201.ko");
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("HI_rmmod failed\n");
        return HI_FAILURE;
    }
#else
    //extern void gsesnor_exit(void);
    //gsesnor_exit();
#endif
    return HI_SUCCESS;

}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif  /* End of #ifdef __cplusplus */

