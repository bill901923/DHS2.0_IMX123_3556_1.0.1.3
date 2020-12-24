/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    comm_define.c
 * @brief   sample define function
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */

#ifndef __COMM_DEFINE_H__
#define __COMM_DEFINE_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "hi_mapi_vcap_define.h"
#include "hi_mapi_vproc_define.h"
#include "hi_mapi_venc_define.h"
#include "hi_mapi_comm_define.h"
#include "hi_mapi_sys.h"
#include "hi_mapi_vcap.h"
#include "hi_mapi_vproc.h"
#include "hi_mapi_venc.h"
#include "hi_buffer.h"
#include "hi_comm_vb.h"
#include "hi_comm_video.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "hi_comm_vo.h"
#include "hi_mapi_disp_define.h"
#include "hi_mapi_disp.h"


#include "hi_mapi_acap_define.h"
#include "hi_mapi_acap.h"
#include "hi_mapi_ao_define.h"
#include "hi_mapi_ao.h"
#include "hi_mapi_aenc_adpt.h"
#include "hi_mapi_aenc_define.h"
#include "hi_mapi_aenc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define SAMPLE_MAPI_VPORT_16BPP (0)

#define SAMPLE_MAPI_AI_EXTERN_DEV 0 /** dev for extern acodec */
#define SAMPLE_MAPI_AI_INNER_DEV  0 /** dev for inner acodec */
#define SAMPLE_MAPI_AO_EXTERN_DEV 0 /** ao extern dev id */
#define SAMPLE_MAPI_AO_INNER_DEV  0 /** ao inner acodec dev id */
#define SAMPLE_MAPI_AO_HDMI_DEV   1 /** ao hdmi dev id */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

