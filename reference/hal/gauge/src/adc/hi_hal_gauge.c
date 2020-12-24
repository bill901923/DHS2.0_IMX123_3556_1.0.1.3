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

#include "hi_type.h"
#include "hi_hal_gauge.h"
#include "hi_appcomm_log.h"
#include "hi_appcomm_util.h"
#include "hi_hal_common.h"
#include "hi_hal_adc_inner.h"


#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif  /* End of #ifdef __cplusplus */


#if defined(HI3518EV300)

#define ADC_CH1_IOCFG_REG                (0x120c0004)
#define HAL_GAUGE_ADC_CHN                (1)

#elif defined(HI3559V200)

#define ADC_CH1_IOCFG_REG                (0x111F0034)
#define HAL_GAUGE_ADC_CHN                (1)

#endif

/**Attention!!*/
/**Attention!!*/
/**The value just be a sample. You must  calibrate it by the battery*/
/**HAL_GAUGE_VALUE_MIN: The value can be get by Get ADC value when machine power off*/
/**HAL_GAUGE_VALUE_MAX:The value can be get by the ADC value can not be up when machine charge in*/
#define HAL_GAUGE_VALUE_MAX              (830)
#define HAL_GAUGE_VALUE_MIN              (730)
#define HAL_GAUGE_DIV_0TO1(a)            ((0 == (a)) ? 1 : (a))

static HI_BOOL s_bGaugeInitFlag = HI_FALSE;
/** macro define */
HI_S32 HI_HAL_GAUGE_Init(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    if (HI_TRUE == s_bGaugeInitFlag)
    {
        MLOGD("GAUGE has Already Init.\n");
        return HI_HAL_EINITIALIZED;
    }

    /**Set ADC Piuout Mux*/
#if defined(HI3518EV300)

    writereg(0x1, 0xf, ADC_CH1_IOCFG_REG);/*LSADC_CH1*/
#if defined(BOARD_DASHCAM_REFB)
    /**Set GPIO1_6 Pin input, Charge State Check*/
    himm(0X120C0018, 0x1C02);//set gpio1_6
#endif

#elif defined(HI3559V200)

    writereg(0x0, 0xf, ADC_CH1_IOCFG_REG);/*LSADC_CH1*/

#endif

    s32Ret = HAL_ADC_Init(HAL_GAUGE_ADC_CHN);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("[Error]ADC[%d] Init Failed.\n\n",HAL_GAUGE_ADC_CHN);
        return HI_HAL_EINTER;
    }

    s_bGaugeInitFlag = HI_TRUE;
    return HI_SUCCESS;
}

HI_S32 HI_HAL_GAUGE_GetLevel(HI_S32* ps32Level) /* percent*/
{
    HI_S32 s32Ret = 0;
    HI_S32 s32Value = 0;
    /* parm ps32Level check */
    if (NULL == ps32Level)
    {
        MLOGE("ps32Level is null\n");
        return HI_HAL_EINVAL;
    }

    /* init check */
    if (HI_TRUE != s_bGaugeInitFlag)
    {
        MLOGE("gauge not initialized\n");
        return HI_HAL_ENOINIT;
    }

    s32Ret = HAL_ADC_GetVal(HAL_GAUGE_ADC_CHN, &s32Value);
    if(HI_FAILURE == s32Ret)
    {
        MLOGE("get value error\n");
        return HI_HAL_EINTER;
    }

    if (s32Value > HAL_GAUGE_VALUE_MAX)
    {
        s32Value = HAL_GAUGE_VALUE_MAX;
    }
    else if (s32Value < HAL_GAUGE_VALUE_MIN)
    {
        s32Value = HAL_GAUGE_VALUE_MIN;
    }

    /**change  voltage  to guage percent */
    /**The Following fuction just a sample*/
    *ps32Level = (s32Value - HAL_GAUGE_VALUE_MIN) * 100 / HAL_GAUGE_DIV_0TO1(HAL_GAUGE_VALUE_MAX - HAL_GAUGE_VALUE_MIN);

    return HI_SUCCESS;
}

HI_S32 HI_HAL_GAUGE_GetChargeState(HI_BOOL* pbCharge)
{
    HI_BOOL bCharge = HI_FALSE;

    /* parm ps32Level check */
    if (NULL == pbCharge)
    {
        MLOGE("pbCharge is null\n");
        return HI_HAL_EINVAL;
    }

    /* init check */
    if (HI_TRUE != s_bGaugeInitFlag)
    {
        MLOGE("gauge not initialized\n");
        return HI_HAL_ENOINIT;
    }

#if defined(HI3518EV300) && defined(BOARD_DASHCAM_REFB)
    HI_S32 s32Value = 0;
    himd(0x120B1100, &s32Value);
    if ((1 << 6) & s32Value)
    {
        bCharge = HI_TRUE;
    }
#endif
    *pbCharge = bCharge;

    return HI_SUCCESS;
}

HI_S32 HI_HAL_GAUGE_Deinit(HI_VOID)
{
    /* Close gauge Device */
    if (HI_TRUE != s_bGaugeInitFlag)
    {
        MLOGW("gauge not initialized, no need to close\n");
        return HI_HAL_ENOINIT;
    }
    HAL_ADC_Deinit(HAL_GAUGE_ADC_CHN);
    s_bGaugeInitFlag = HI_FALSE;

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif  /* End of #ifdef __cplusplus */

