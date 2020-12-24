/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_public_fastboot.h
 * @brief   sample public fastboot header
 * @author  HiMobileCam NDK develop team
 * @date  2019-4-16
 */
#ifndef __SAMPLE_PUBLIC_FASTBOOT_H__
#define __SAMPLE_PUBLIC_FASTBOOT_H__

#include "hi_mapi_aenc_define.h"
#include "sample_cfg.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

HI_S32 FASTBOOT_StartAudioAenc(HI_VOID);
HI_S32 FASTBOOT_StartVcap(HI_HANDLE VcapDev, HI_HANDLE VcapPipe, HI_HANDLE PipeChn);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SAMPLE_PUBLIC_FASTBOOT_H__ */
