/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_public_disp.h
 * @brief   sample public disp header
 * @author  HiMobileCam NDK develop team
 * @date  2019-4-16
 */
#ifndef __SAMPLE_PUBLIC_DISP_H__
#define __SAMPLE_PUBLIC_DISP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SAMPLE_DISP_GET_HDMIEVENT_THREAD_USLEEP_TIME         (10 * 1000)



HI_S32 SAMPLE_COMM_StartPreview(HI_HANDLE GrpHdl, HI_HANDLE PortHdl, HI_BOOL bStitch);
HI_VOID SAMPLE_COMM_StopPreview(HI_HANDLE GrpHdl, HI_HANDLE PortHdl, HI_BOOL bStitch);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SAMPLE_PUBLIC_DISP_H__ */
