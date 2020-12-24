/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_comm.c
 * @brief   sample comm function
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */

#include <stdio.h>
#include "sample_comm.h"
#include "sensor_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define DATA_BITWIDTH DATA_BITWIDTH_8
#define TEST_AI_DEV   0 /** dev for inner acodec */

HI_CHAR g_aszDumpFrameName[HI_MAPI_VCAP_MAX_PIPE_NUM][FILE_FULL_PATH_MAX_LEN] = {0};
HI_S32 g_as32RawFrameCnt[HI_MAPI_VCAP_MAX_PIPE_NUM];

HI_S32 SAMPLE_COMM_Init(SAMPLE_COMM_INITATTR_S stSampleCommAttr, SAMPLE_VB_CFG_MODE_E enVbMode)
{
    HI_S32 s32Ret;

    /* start media */
    HI_MAPI_MEDIA_ATTR_S stMediaAttr;
    HI_U32 u32BlkSize;

    SAMPLE_CHECK_RET(memset_s(&stMediaAttr, sizeof(HI_MAPI_MEDIA_ATTR_S), 0, sizeof(HI_MAPI_MEDIA_ATTR_S)));
    SAMPLE_CHECK_RET(memset_s(&stMediaAttr.stMediaConfig.stVIVPSSMode, sizeof(VI_VPSS_MODE_S), 0,
        sizeof(VI_VPSS_MODE_S)));

    SAMPLE_CHECK_RET(memcpy_s(&stMediaAttr.stMediaConfig.stVIVPSSMode, sizeof(VI_VPSS_MODE_S),
        &stSampleCommAttr.stViVpssMode, sizeof(VI_VPSS_MODE_S)));
    stMediaAttr.stMediaConfig.stVbConfig.u32MaxPoolCnt = 16;
    if (VB_MODE_1 == enVbMode) {
        u32BlkSize = COMMON_GetPicBufferSize(stSampleCommAttr.stResolution.u32Width,
                                             stSampleCommAttr.stResolution.u32Height, PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                                             DATA_BITWIDTH, COMPRESS_MODE_NONE, DEFAULT_ALIGN);

        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[0].u64BlkSize = u32BlkSize;
        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[0].u32BlkCnt = 8;

        u32BlkSize = COMMON_GetPicBufferSize(1920, 1080, PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                                             DATA_BITWIDTH, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[1].u64BlkSize = u32BlkSize;
        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[1].u32BlkCnt = 6;

        u32BlkSize = VI_GetRawBufferSize(stSampleCommAttr.stResolution.u32Width,
            stSampleCommAttr.stResolution.u32Height, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[2].u64BlkSize = u32BlkSize;
        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[2].u32BlkCnt = 2;

    } else if (VB_MODE_2 == enVbMode) {
        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[0].u64BlkSize = 18481152;  // VPSS
        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[0].u32BlkCnt = 15;

        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[1].u64BlkSize = 3750016;  // vi
        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[1].u32BlkCnt = 22;

        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[2].u64BlkSize = 6220800;  // avs
        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[2].u32BlkCnt = 4;

        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[2].u64BlkSize = 3110400;  // avs_chn1
        stMediaAttr.stMediaConfig.stVbConfig.astCommPool[2].u32BlkCnt = 4;
    }
    stMediaAttr.stVencModPara.u32H264eLowPowerMode = 1;
    stMediaAttr.stVencModPara.u32H265eLowPowerMode = 1;
    s32Ret = HI_MAPI_Media_Init(&stMediaAttr);

    if (s32Ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_Deinit(HI_VOID)
{
    HI_S32 s32Ret;
    s32Ret = HI_MAPI_Media_Deinit();

    if (s32Ret != HI_SUCCESS) {
        printf("HI_MAPI_Media_DeInit fail\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

