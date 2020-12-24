/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_public_vproc.h
 * @brief   sample public vproc header
 * @author  HiMobileCam NDK develop team
 * @date  2019-4-16
 */
#ifndef __SAMPLE_PUBLIC_VPROC_H__
#define __SAMPLE_PUBLIC_VPROC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


HI_S32 SAMPLE_COMM_VPROC_ProcYuvData(HI_HANDLE VProcHdl, HI_HANDLE VPortHdl, HI_MAPI_FRAME_DATA_S *pVPortYUV,
    HI_VOID *pPrivateData);
HI_S32 SAMPLE_COMM_VPROC_ProcRawData(HI_HANDLE VcapPipeHdl, HI_MAPI_FRAME_DATA_S *pVCapRawData, HI_S32 s32DataNum,
    HI_VOID *pPrivateData);
HI_S32 SAMPLE_COMM_VPROC_ProcBnrRawData(HI_HANDLE ProcHdl, HI_HANDLE PortHdl, HI_MAPI_FRAME_DATA_S *pBNRRaw,
    HI_VOID *pPrivateData);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SAMPLE_PUBLIC_VPROC_H__ */
