/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_public_audio.h
 * @brief   sample public audio header
 * @author  HiMobileCam NDK develop team
 * @date  2019-4-16
 */
#ifndef __SAMPLE_PUBLIC_AUDIO_H__
#define __SAMPLE_PUBLIC_AUDIO_H__

#include "sample_cfg.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define SAMPLE_SAVE_TYPE_POSTFIX            8
#define SAMPLE_AIO_MAX_POINT_PER_FRAME_G711 480


typedef struct hiSAMPLE_AUDIO_ATTR_S
{
    HI_CHAR aFilePath[FILE_FULL_PATH_MAX_LEN];
    HI_MAPI_AUDIO_FORMAT_E enAencFormat;
    HI_BOOL bDeleteHisiHead;
}SAMPLE_AUDIO_ATTR_S;

HI_VOID SAMPLE_AUDIO_ConfigACapAttr(HI_MAPI_ACAP_ATTR_S *pstACapAttr);
HI_VOID SAMPLE_AUDIO_ConfigAEncAttr(HI_MAPI_AENC_ATTR_S *pstAEncAttr,
    HI_MAPI_AENC_ATTR_AAC_S *pstAacAencAttr);
HI_VOID SAMPLE_AUDIO_ConfigAOAttr(HI_MAPI_AO_ATTR_S *pstAOAttr);
HI_S32 SAMPLE_AUDIO_DataProc(HI_HANDLE AencHdl, const AUDIO_STREAM_S *pAStreamData, HI_VOID *pPrivateData);
HI_S32 SAMPLE_AUDIO_DeleteHisiHeadWithG711(HI_HANDLE acapHdl, HI_CHAR *pAacFilePath, HI_BOOL bDeleteHisiHead);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SAMPLE_PUBLIC_AUDIO_H__ */
