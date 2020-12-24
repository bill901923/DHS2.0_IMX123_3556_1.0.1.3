/**
 * @file      hi_product_usbctrl.h
 * @brief     usb control interface
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/26
 * @version   1.0

 */

#ifndef __HI_PRODUCT_USBCTRL_H__
#define __HI_PRODUCT_USBCTRL_H__

#include "hi_usb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define HI_PDT_USBCTRL_EBUSY         HI_APPCOMM_ERR_ID(HI_APP_MOD_USBCTRL, HI_EBUSY)

/** \addtogroup     USBCTRL */
/** @{ */  /** <!-- [USBCTRL] */

/**
 * @brief     register usb event
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/26
 */
HI_S32 HI_PDT_USBCTRL_RegisterEvent(HI_VOID);

/**
 * @brief     module init
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/26
 */
HI_S32 HI_PDT_USBCTRL_Init(HI_VOID);

/**
 * @brief     module deinit
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/1/26
 */
HI_S32 HI_PDT_USBCTRL_Deinit(HI_VOID);

/**
 * @brief     Pause USB
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/06/28
 */
HI_S32 HI_PDT_USBCTRL_Pause(HI_BOOL enable);

/** @}*/  /** <!-- ==== USBCTRL End ====*/

/**
 * @brief     USBCTRL set mode
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/06/28
 */
HI_S32 HI_PDT_USBCTRL_SetMode(HI_USB_MODE_E mode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __HI_PRODUCT_USBCTRL_H__ */

