/**
* Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
*
* @file      adpt_aenc_aac.c
* @brief     adec aac decoder adapter module.
* @author    HiMobileCam ndk develop team
* @date       2019/06/19
*/

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "mapi_comm_inner.h"
#include "hi_mapi_log.h"
#include "hi_mapi_adec_define.h"
#include "mpi_audio.h"
#include "adpt_adec_aac.h"
#include "hi_comm_adec.h"
#include "aacdec.h"

#define HI_AUDIO_ASSERT(x) {if (HI_TRUE != (x))return -1;}

static HI_S32 s_AacDecHandle = HI_INVALID_HANDLE;


HI_S32 OpenAACDecoder(HI_VOID *pDecoderAttr, HI_VOID **ppDecoder)
{
    ADEC_AAC_DECODER_S *pstDecoder = NULL;
    ADEC_ATTR_AAC_S *pstAttr = NULL;
    HI_S32 s32Ret;

    HI_AUDIO_ASSERT(pDecoderAttr != NULL);
    HI_AUDIO_ASSERT(ppDecoder != NULL);

    pstAttr = (ADEC_ATTR_AAC_S *)pDecoderAttr;

    /* allocate memory for decoder */
    pstDecoder = (ADEC_AAC_DECODER_S *)malloc(sizeof(ADEC_AAC_DECODER_S));
    if(pstDecoder == NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "[Info]:%s\n", "no memory");
        return HI_FAILURE;
    }
    s32Ret = memset_s(pstDecoder, sizeof(ADEC_AAC_DECODER_S), 0, sizeof(ADEC_AAC_DECODER_S));
    MAPI_CHECK_RET(HI_MAPI_MOD_ADEC, "memset_s", s32Ret);

   *ppDecoder = (HI_VOID *)pstDecoder;

    /* create decoder */
    pstDecoder->pstAACState = AACInitDecoder((AACDECTransportType)pstAttr->enTransType);
    if (!pstDecoder->pstAACState) {
        free(pstDecoder);
        pstDecoder = NULL;
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "[Info]:%s\n", "AACInitDecoder failed");
        return HI_FAILURE;
    }

    s32Ret = memcpy_s(&pstDecoder->stAACAttr, sizeof(ADEC_ATTR_AAC_S), pstAttr, sizeof(ADEC_ATTR_AAC_S));
    MAPI_CHECK_RET(HI_MAPI_MOD_ADEC, "memcpy_s", s32Ret);

    return HI_SUCCESS;
}

HI_S32 DecodeAACFrm(HI_VOID *pDecoder, HI_U8 **pu8Inbuf,HI_S32 *ps32LeftByte,
                        HI_U16 *pu16Outbuf,HI_U32 *pu32OutLen,HI_U32 *pu32Chns)
{
    HI_S32 s32Ret = HI_SUCCESS;
    ADEC_AAC_DECODER_S *pstDecoder = NULL;
    HI_S32 s32Samples, s32FrmLen, s32SampleBytes;
    AACFrameInfo aacFrameInfo;

    HI_AUDIO_ASSERT(pDecoder != NULL);
    HI_AUDIO_ASSERT(pu8Inbuf != NULL);
    HI_AUDIO_ASSERT(ps32LeftByte != NULL);
    HI_AUDIO_ASSERT(pu16Outbuf != NULL);
    HI_AUDIO_ASSERT(pu32OutLen != NULL);
    HI_AUDIO_ASSERT(pu32Chns != NULL);

    *pu32Chns = 1;/*voice encoder only one channle */

    pstDecoder = (ADEC_AAC_DECODER_S *)pDecoder;

    s32FrmLen = AACDecodeFindSyncHeader(pstDecoder->pstAACState, pu8Inbuf, ps32LeftByte);
    if (s32FrmLen < 0) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "[Info]:%s\n", "AAC decoder can't find sync header");
        return HI_FAILURE;
    }

    if(pstDecoder->stAACAttr.enTransType == AAC_TRANS_TYPE_ADTS) {
        HI_S32 s32OldLeftBytes = *ps32LeftByte;
        if(s32OldLeftBytes <= 7) {
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC,"LeftBytes:%d, not enough for AAC ADTS header\n", s32OldLeftBytes);
            return HI_FAILURE;
        }
        HI_S32 s32PacketSize = ((*pu8Inbuf)[4] << 3) | (((*pu8Inbuf)[5] - 0x1F) >> 5);
        if(s32OldLeftBytes < s32PacketSize) {
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC,"AAC packet size %d, buffer left %d bytes, not enough for a frame\n", s32PacketSize, s32OldLeftBytes);
            return HI_FAILURE;
        }
    }

    /*Notes: pInbuf will updated*/
    s32Ret = AACDecodeFrame(pstDecoder->pstAACState, pu8Inbuf, ps32LeftByte, (HI_S16 *)pu16Outbuf);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "aac decoder failed!, s32Ret:0x%x\n", s32Ret);
        return s32Ret;
    }
    AACGetLastFrameInfo(pstDecoder->pstAACState, &aacFrameInfo);
    aacFrameInfo.nChans = ((aacFrameInfo.nChans != 0) ? aacFrameInfo.nChans : 1);
    /* samples per frame of one sound track*/
    s32Samples = aacFrameInfo.outputSamps/aacFrameInfo.nChans;

    s32SampleBytes = s32Samples * sizeof(HI_U16);
    *pu32Chns = aacFrameInfo.nChans;
    *pu32OutLen = s32SampleBytes;

    return s32Ret;
}

HI_S32 GetAACFrmInfo(HI_VOID *pDecoder, HI_VOID *pInfo)
{
    ADEC_AAC_DECODER_S *pstDecoder = NULL;
    AACFrameInfo aacFrameInfo;
    AAC_FRAME_INFO_S *pstAacFrm = NULL;

    HI_AUDIO_ASSERT(pDecoder != NULL);
    HI_AUDIO_ASSERT(pInfo != NULL);

    pstDecoder = (ADEC_AAC_DECODER_S *)pDecoder;
    pstAacFrm = (AAC_FRAME_INFO_S *)pInfo;

    AACGetLastFrameInfo(pstDecoder->pstAACState, &aacFrameInfo);

    pstAacFrm->s32Samplerate = aacFrameInfo.sampRateOut;
    pstAacFrm->s32BitRate = aacFrameInfo.bitRate;
    pstAacFrm->s32Profile = aacFrameInfo.profile;
    pstAacFrm->s32TnsUsed = aacFrameInfo.tnsUsed;
    pstAacFrm->s32PnsUsed = aacFrameInfo.pnsUsed;

    return HI_SUCCESS;
}


HI_S32 CloseAACDecoder(HI_VOID *pDecoder)
{
    ADEC_AAC_DECODER_S *pstDecoder = NULL;

    HI_AUDIO_ASSERT(pDecoder != NULL);
    pstDecoder = (ADEC_AAC_DECODER_S *)pDecoder;

    AACFreeDecoder(pstDecoder->pstAACState);

    free(pstDecoder);

    return HI_SUCCESS;
}

HI_S32 ResetAACDecoder(HI_VOID *pDecoder)
{
    ADEC_AAC_DECODER_S *pstDecoder = NULL;

    HI_AUDIO_ASSERT(pDecoder != NULL);
    pstDecoder = (ADEC_AAC_DECODER_S *)pDecoder;

    AACFreeDecoder(pstDecoder->pstAACState);

    /* create decoder */
    pstDecoder->pstAACState = AACInitDecoder((AACDECTransportType)pstDecoder->stAACAttr.enTransType);
    if (!pstDecoder->pstAACState) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "[Info]:%s\n", "AACResetDecoder failed");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


HI_S32 MAPI_ADEC_RegisterAac(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Handle = HI_INVALID_HANDLE;
    ADEC_DECODER_S stAac;

    if(s_AacDecHandle != HI_INVALID_HANDLE) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "aacDec have inited, return successful \n");
        return HI_SUCCESS;
    }

    stAac.enType = PT_AAC;
    s32Ret = snprintf_s(stAac.aszName, sizeof(stAac.aszName), sizeof(stAac.aszName), "Aac");
    if (s32Ret == HI_FAILURE)
    {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "snprintf_s error:%#x \n", s32Ret);
        return HI_MAPI_SYS_ESAFEFUNC_OPERATE_FAIL;
    }

    stAac.pfnOpenDecoder = OpenAACDecoder;
    stAac.pfnDecodeFrm = DecodeAACFrm;
    stAac.pfnGetFrmInfo = GetAACFrmInfo;
    stAac.pfnCloseDecoder = CloseAACDecoder;
    stAac.pfnResetDecoder = ResetAACDecoder;
    s32Ret = HI_MPI_ADEC_RegisterDecoder(&s32Handle, &stAac);
    if(HI_SUCCESS != s32Ret) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MPI_ADEC_RegisterDecoder return ERR, %x", s32Ret);
        return s32Ret;
    }

    s_AacDecHandle = s32Handle;

    return HI_SUCCESS;
}

HI_S32 MAPI_ADEC_UnRegisterAac(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    if(s_AacDecHandle == HI_INVALID_HANDLE) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "aacDec have Deinited, return successful \n");
        return HI_SUCCESS;
    }

    s32Ret = HI_MPI_ADEC_UnRegisterDecoder(s_AacDecHandle);
    if (HI_SUCCESS != s32Ret) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC, "HI_MPI_ADEC_UnRegisterDecoder fail,s32Ret:%x\n", s32Ret);
        return s32Ret;
    }
    s_AacDecHandle = HI_INVALID_HANDLE;

    return HI_SUCCESS;
}



