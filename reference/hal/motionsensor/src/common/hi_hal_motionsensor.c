/**
* @file    hal_motionsensor_hal.c
* @brief   hal motionsensor hal implemention
*
* Copyright (c) 2017 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date      2017/12/25
* @version

*/
#include "hi_hal_motionsensor.h"
#include "hal_motionsensor_inner.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif  /* End of #ifdef __cplusplus */

HI_S32 HI_HAL_MOTIONSENSOR_Init(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;

    //mng
    extern int motionsensor_init(void);
    motionsensor_init();
    HI_APPCOMM_CHECK_EXPR_WITHOUT_RETURN((HI_SUCCESS == s32Ret), "motionsensor_init");

    //chip
    extern int MotionSensorInit(void);
    s32Ret = MotionSensorInit();
    HI_APPCOMM_CHECK_EXPR_WITHOUT_RETURN((HI_SUCCESS == s32Ret), "MotionSensorInit");

    s32Ret = HAL_MOTIONSENSOR_PreInit();
    if (HI_SUCCESS != s32Ret) {
        MLOGE("HAL_MOTIONSENSOR_PreInit Failed!\n");
        return HI_FAILURE;
    }

    return s32Ret;
}

HI_S32 HI_HAL_MOTIONSENSOR_DeInit(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;

    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* End of #ifdef __cplusplus */
