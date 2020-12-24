/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    hi_mapi_adec.h
 * @brief   adec module header
 * @author  HiMobileCam NDK develop team
 * @date  2019/06/19
 */

#ifndef __HI_MAPI_ADEC_H__
#define __HI_MAPI_ADEC_H__

#include "hi_mapi_adec_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** \addtogroup     ADEC */
/** @{ */  /** <!-- [ADEC] */

/* Interface */
/**
 * @brief    init adec with attr
 * @param[in] AdecHdl HI_HANDLE:handle of audio Decode module.
 * @param[in] pstAdecAttr HI_MAPI_ADEC_ATTR_S:audio Decode module attribute.
 * @return 0 success,non-zero error code.
 */
HI_S32 HI_MAPI_ADEC_Init(HI_HANDLE AdecHdl, const HI_MAPI_ADEC_ATTR_S* pstAdecAttr);

/* Interface */
/**
 * @brief    deinit adec with attr
 * @param[in] AdecHdl HI_HANDLE:handle of audio Decode module.
 * @return 0 success,non-zero error code.
 */
HI_S32 HI_MAPI_ADEC_Deinit(HI_HANDLE AdecHdl);

/**
 * @brief    send audio frame to adec
 * @param[in] AdecHdl HI_HANDLE:handle of audio Decode module.
 * @param[in] pstAdecStream AUDIO_STREAM_S: audio frame data info
 * @param[in] bBlock HI_BOOL: Block sign
 * @return 0 success,non-zero error code.
 */
HI_S32 HI_MAPI_ADEC_SendStream(HI_HANDLE AdecHdl, const AUDIO_STREAM_S* pstAdecStream, HI_BOOL bBlock);

/**
 * @brief    get pcm raw audio frame
 * @param[in] AdecHdl HI_HANDLE:handle of audio Decode module.
 * @param[in] pstAdecStream pstAudioFrameInfo: audio frame info
 * @param[in] bBlock HI_BOOL: Block sign
 * @return 0 success,non-zero error code.
 */
HI_S32 HI_MAPI_ADEC_GetFrame(HI_HANDLE AdecHdl, HI_MAPI_AUDIO_FRAME_INFO_S* pstAudioFrameInfo, HI_BOOL bBlock);

/**
 * @brief    release pcm raw audio frame
 * @param[in] AdecHdl HI_HANDLE:handle of audio Decode module.
 * @param[in] pstAdecStream pstAudioFrameInfo: audio frame info
 * @return 0 success,non-zero error code.
 */
HI_S32 HI_MAPI_ADEC_ReleaseFrame(HI_HANDLE AdecHdl, const HI_MAPI_AUDIO_FRAME_INFO_S* pstAudioFrameInfo);

/**
 * @brief    clean audio frame buf
 * @param[in] AdecHdl HI_HANDLE:handle of audio Decode module.
 * @return 0 success,non-zero error code.
 */
HI_S32 HI_MAPI_ADEC_SendEndOfStream(HI_HANDLE AdecHdl);


/** @}*/  /** <!-- ==== ADEC End ====*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_MAPI_ADEC_H__ */
