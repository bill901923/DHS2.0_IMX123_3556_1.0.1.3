/**
* Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
*
* @file      mapi_aenc_inner.h
* @brief     aenc inner head file.
* @author    HiMobileCam ndk develop team
* @date      2019/06/19
*/

#ifndef __MAPI_ADEC_INNER_H__
#define __MAPI_ADEC_INNER_H__

#include <pthread.h>
#include "hi_mapi_comm_define.h"
#include "hi_mapi_log.h"
#include "hi_mapi_adec_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define CHECK_MAPI_ADEC_NULL_PTR_RET(ptr)\
    do{\
        if(ptr == HI_NULL)\
        {\
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC,"ADEC NULL pointer\n");\
            return HI_MAPI_ADEC_ENULLPTR;\
        }\
    }while(0)

#define CHECK_MAPI_ADEC_CHECK_INIT_RET(state)\
    do{\
        if(state != HI_TRUE)\
        {\
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC,"MEDIA has not been pre inited \n");\
            return HI_MAPI_ADEC_ENOTINITED;\
        }\
    }while(0)

#define CHECK_MAPI_ADEC_HANDLE_RANGE_RET(handle)\
    do{\
        if(handle >= HI_MAPI_ADEC_CHN_MAX_NUM)\
        {\
            MAPI_ERR_TRACE(HI_MAPI_MOD_ADEC,"ADEC handle out of range\n");\
            return HI_MAPI_ADEC_EINVALIDHDL;\
        }\
    }while(0)




typedef struct tagMAPI_ADEC_CHN_S
{
    HI_BOOL                 bChnInited;
    pthread_mutex_t         adecFuncLock;
    HI_MAPI_AUDIO_FORMAT_E  enAdecFormat;
}MAPI_ADEC_CHN_S;

/** context for adec */
typedef struct tagMAPI_ADEC_CONTEXT_S
{
    HI_BOOL         bAdecInited; /**< module inited flag */
    MAPI_ADEC_CHN_S astAdecChn[HI_MAPI_ADEC_CHN_MAX_NUM];
}MAPI_ADEC_CONTEXT_S;

HI_S32 MAPI_ADEC_Init(HI_VOID);
HI_S32 MAPI_ADEC_Deinit(HI_VOID);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_MAPI_AENC_INNER_H__ */
