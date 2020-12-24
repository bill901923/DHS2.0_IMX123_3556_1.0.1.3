/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    mapi_venc_os.h
 * @brief   venc os module header
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */
#ifndef __MAPI_VENC_OS_H__
#define __MAPI_VENC_OS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

HI_VOID MAPI_VENC_OSInit(HI_VOID);

HI_VOID MAPI_VENC_OSDeInit(HI_VOID);

HI_S32 MAPI_VENC_GetVStream(HI_HANDLE VencHdl, MAPI_VENC_CHN_S *pstVencChn);

HI_VOID MAPI_VENC_ClearDatafifoData(HI_HANDLE VencHdl);


/*  used for hisilink */
HI_S32 MAPI_VENC_StartDataInfo(HI_HANDLE VencHdl);

HI_BOOL MAPI_VENC_StopDataInfo(HI_HANDLE VencHdl);

HI_S32 MAPI_VENC_ClearDatafifoFrame(HI_HANDLE VencHdl);






#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MAPI_VENC_OS_H__ */
