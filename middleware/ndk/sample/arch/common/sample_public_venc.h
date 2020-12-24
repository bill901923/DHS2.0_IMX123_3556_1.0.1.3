/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_public_venc.h
 * @brief   sample public venc header
 * @author  HiMobileCam NDK develop team
 * @date  2019-4-16
 */
#ifndef __SAMPLE_PUBLIC_VENC_H__
#define __SAMPLE_PUBLIC_VENC_H__

#include "sample_cfg.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

HI_CHAR *SAMPLE_VENC_Pt2Str(HI_MAPI_PAYLOAD_TYPE_E enPayloadType);
HI_S32 SAMPLE_VENC_SaveJpegFile(HI_HANDLE VencHdl, HI_MAPI_VENC_DATA_S *pVStreamData,
    HI_VOID *pPrivateData);
HI_S32 SAMPLE_VENC_SaveStreamFile(HI_HANDLE VencHdl, HI_MAPI_VENC_DATA_S *pVStreamData,
    HI_VOID *pPrivateData);
HI_S32 SMAPLE_COMM_VENC_DataProc(HI_HANDLE VencHdl, HI_MAPI_VENC_DATA_S *pVStreamData,
    HI_VOID *pPrivateData);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SAMPLE_PUBLIC_VENC_H__ */
