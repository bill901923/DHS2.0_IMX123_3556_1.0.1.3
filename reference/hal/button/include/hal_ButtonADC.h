
/**
* @file    hi_hal_gauge.h
* @brief   product hal gauge struct and interface
*
* Copyright (c) 2017 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date      2017/12/12
* @version   1.0

*/
#ifndef __HI_HAL_GAUGE_H__
#define __HI_HAL_GAUGE_H__
#include "hi_type.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
HI_S32 Button_ADC_Init(HI_HANDLE AdcHdl);
HI_S32 Button_ADC_VAl(HI_HANDLE AdcHdl,HI_S32* ps32vel);
HI_S32 Button_ADC_GetKey(HI_HANDLE AdcHdl,HI_S32* pbkeyNum);

HI_S32 Button_ADC_Deinit(HI_HANDLE AdcHdl);

/** @}*/  /** <!-- ==== HAL_GAUGE End ====*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of __HI_HAL_GAUGE_H__*/
