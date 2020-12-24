/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: Definiton error code data structure of hitimer
 * Author: HiMobileCam middleware develop team
 * Create: 2012-12-22
 */
#ifndef __HI_TIMER_ERR_H__
#define __HI_TIMER_ERR_H__

#include "hi_error_def.h"  // ����������ɹ���
#include "hi_defs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* ��0x40~0xFF */
typedef enum hiAPP_TIMER_ERR_CODE_E {
    APP_TIMER_ERR_CANCEL = 0xd4,  /* cancel timer error */
    APP_TIMER_ERR_CLEANUP = 0xd5, /* clean up timer error */
    APP_TIMER_ERR_RESET = 0xd6,   /* reset timer error */
    APP_TIMER_ERR_BUTT = 0xFF
} APP_TIMER_ERR_CODE_E;

#define HI_ERR_TIMER_NULL_PTR HI_APP_DEF_ERR(HI_APPID_TIMER, APP_ERR_LEVEL_ERROR, APP_ERR_NULL_PTR)
#define HI_ERR_TIMER_CANCEL   HI_APP_DEF_ERR(HI_APPID_TIMER, APP_ERR_LEVEL_ERROR, APP_TIMER_ERR_CANCEL)
#define HI_ERR_TIMER_CLEANUP  HI_APP_DEF_ERR(HI_APPID_TIMER, APP_ERR_LEVEL_ERROR, APP_TIMER_ERR_CLEANUP)
#define HI_ERR_TIMER_RESET    HI_APP_DEF_ERR(HI_APPID_TIMER, APP_ERR_LEVEL_ERROR, APP_TIMER_ERR_RESET)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_TIMER_ERR_H__ */
