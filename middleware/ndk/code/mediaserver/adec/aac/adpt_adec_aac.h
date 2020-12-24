/**
* Copyright (C), 2017-2018, Hisilicon Tech. Co., Ltd.
* All rights reserved.
*
* @file      adpt_adec_aac.h
* @brief     adec aac decoder adapter head file.
* @author    HiMobileCam ndk develop team
* @date      2019/06/19
*/

#ifndef __ADPT_ADEC_AAC_H__
#define __ADPT_ADEC_AAC_H__

#include "aacdec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */



typedef enum hiAAC_TRANS_TYPE_E
{
    AAC_TRANS_TYPE_ADTS = 0,
    AAC_TRANS_TYPE_LOAS= 1,
    AAC_TRANS_TYPE_LATM_MCP1 = 2,
    AAC_TRANS_TYPE_BUTT
}AAC_TRANS_TYPE_E;

typedef struct hiADEC_ATTR_AAC_S
{
    AAC_TRANS_TYPE_E enTransType;
}ADEC_ATTR_AAC_S;

typedef struct hiADEC_AAC_DECODER_S
{
    HAACDecoder         pstAACState;
    ADEC_ATTR_AAC_S     stAACAttr;
} ADEC_AAC_DECODER_S;

typedef struct hiAAC_FRAME_INFO_S
{
    HI_S32 s32Samplerate;   /* sample rate*/
    HI_S32 s32BitRate;                  /* bitrate */
    HI_S32 s32Profile;                  /* profile*/
    HI_S32 s32TnsUsed;                  /* TNS Tools*/
    HI_S32 s32PnsUsed;                  /* PNS Tools*/
} AAC_FRAME_INFO_S;



HI_S32 MAPI_ADEC_RegisterAac(HI_VOID);
HI_S32 MAPI_ADEC_UnRegisterAac(HI_VOID);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
