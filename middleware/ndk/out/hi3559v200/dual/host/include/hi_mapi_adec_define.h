/**
* Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
*
* @file         hi_mapi_adec_define.h
* @brief       adec mapi struct declaration
* @author    HiMobileCam ndk develop team
* @date       2019/06/19
*/
#ifndef __HI_MAPI_ADEC_DEFINE_H__
#define __HI_MAPI_ADEC_DEFINE_H__

#include "hi_mapi_errno.h"
#include "hi_mapi_comm_define.h"
#include "hi_comm_adec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/** \addtogroup     ADEC */
/** @{ */  /** <!-- [ADEC] */

/** error code
      invlalid Handle error for AO */
#define HI_MAPI_ADEC_EINVALIDHDL     HI_MAPI_DEF_ERR(HI_MAPI_MOD_ADEC,MAPI_EN_ERR_LEVEL_ERROR,MAPI_EN_ERR_INVALID_CHNID)     /** invalid handle error for adec */
/* NULL point error for AO */
#define HI_MAPI_ADEC_ENULLPTR        HI_MAPI_DEF_ERR(HI_MAPI_MOD_ADEC,MAPI_EN_ERR_LEVEL_ERROR,MAPI_EN_ERR_NULL_PTR)          /** null pointer error for adec */
/* media not inited */
#define HI_MAPI_ADEC_ENOTINITED      HI_MAPI_DEF_ERR(HI_MAPI_MOD_ADEC,MAPI_EN_ERR_LEVEL_ERROR,MAPI_EN_ERR_SYS_NOTREADY)      /** not inited error for adec */
/* state error for AO */
#define HI_MAPI_ADEC_ESTATEERR       HI_MAPI_DEF_ERR(HI_MAPI_MOD_ADEC,MAPI_EN_ERR_LEVEL_ERROR,MAPI_EN_ERR_NOT_PERM)          /** state error for adec */
/* at lease one parameter is illagal for ADEC */
#define HI_MAPI_ADEC_EILLPARAM       HI_MAPI_DEF_ERR(HI_MAPI_MOD_ADEC,MAPI_EN_ERR_LEVEL_ERROR,MAPI_EN_ERR_ILLEGAL_PARAM)     /** illegal parameter error for adec */

typedef struct hiMAPI_ADEC_ATTR_S
{
    HI_MAPI_AUDIO_FORMAT_E          enAdecFormat;          /* Audio decoding Format*/
    ADEC_MODE_E                     enMode;
} HI_MAPI_ADEC_ATTR_S;


typedef struct hiMAPI_AUDIO_FRAME_INFO_S
{
    HI_VOID *pstFrame;
    AUDIO_FRAME_S stAudioFrame;
    // Audio frame index, this index will be used as uniquely identify when release data.
    HI_U32 u32Id;
} HI_MAPI_AUDIO_FRAME_INFO_S;


/** @}*/  /** <!-- ==== ADEC End ====*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_MAPI_ADEC_DEFINE_H__ */
