/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_cfg.h
 * @brief   sample cfg header
 * @author  HiMobileCam NDK develop team
 * @date  2019-4-18
 */

 /*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_common.h
 * @brief   aenc module
 * @author  HiMobileCam NDK develop team
 * @date  2019-4-16
 */
#ifndef __SAMPLE_CFG_H__
#define __SAMPLE_CFG_H__

#include "comm_define.h"
#include "sensor_interface_cfg_params.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define SAMPLE_MAPI_AI_CHN                        0 /** stereo sound mode */
#define SAMPLE_MAPI_AO_CHN                        0 /** stereo or left sound mode */
#define SAMPLE_MAPI_AI_IN_GAIN                    20
#define SAMPLE_MAPI_AENC_CHN                      0
#define SAMPLE_MAPI_ADEC_DEV                      0
#define SAMPLE_MAPI_AO_OUT_GAIN                   0
#define SAMPLE_MAPI_AUDIO_POINT_PER_FRAME         1024

#define SAMPLE_MAPI_SENSOR_CNT                    8

#define SAMPLE_MAPI_VCAP_DEV_0                    0
#define SAMPLE_MAPI_VCAP_DEV_1                    1
#define SAMPLE_MAPI_VCAP_DEV_2                    2

#define SAMPLE_MAPI_VCAP_PIPE_0                   0
#define SAMPLE_MAPI_VCAP_PIPE_1                   1
#define SAMPLE_MAPI_VCAP_PIPE_2                   2
#define SAMPLE_MAPI_VCAP_PIPE_3                   3
#define SAMPLE_MAPI_VCAP_PIPE_4                   4
#define SAMPLE_MAPI_VCAP_PIPE_5                   5
#define SAMPLE_MAPI_VCAP_PIPE_5                   5
#define SAMPLE_MAPI_VCAP_PIPE_6                   6
#define SAMPLE_MAPI_VCAP_PIPE_7                   7

#define SAMPLE_MAPI_PIPE_CHN_0                    0
#define SAMPLE_MAPI_PIPE_CHN_1                    1

#define SAMPLE_MAPI_VPROC_DUMP_BUF_LEN            4096

#define SAMPLE_MAPI_DISP_HANDLE                   0
#define SAMPLE_MAPI_WIND_HANDLE                   0
#define SAMPLE_MAPI_HDMI_HANDLE                   0

#define VB_SUPPLEMENT_JPEG_MASK                   0x1


#ifdef SUPPORT_PHOTO_POST_PROCESS
#define SAMPLE_MAPI_DSP_ONE_GROUP_INCLUDE_BIN_CNT 4
#define SAMPLE_MAPI_DSP_GROUP_CNT                 4
#endif

#define SAMPLE_CMD_MAX_LEN                        64

#define FILE_FULL_PATH_MAX_LEN                    128

#define SAMPLE_U32MAX_NUM 0xFFFFFFFF
#define SAMPLE_U64MAX_NUM 0xFFFFFFFFFFFFFFFF

/* in sensor 477 modes, the frame rate of 4k30 snsmode is 29.97fps, else it is 30.0fps */
#ifdef SENSOR_IMX477
#define SAMPLE_4K30FPS_ISPPUB_MAX_FRAMERATE      29.97f
#else
#define SAMPLE_4K30FPS_ISPPUB_MAX_FRAMERATE      30.0f
#endif

extern HI_BOOL g_bSnap_finished;
extern HI_S32 g_as32RawFrameCnt[HI_MAPI_VCAP_MAX_PIPE_NUM];

extern HI_CHAR g_aszDumpFrameName[HI_MAPI_VCAP_MAX_PIPE_NUM][FILE_FULL_PATH_MAX_LEN];

#ifndef SAMPLE_CHECK_GOTO
#define SAMPLE_CHECK_GOTO(express)                                                              \
    do {                                                                                        \
        s32Ret = express;                                                                       \
        if (s32Ret != HI_SUCCESS) {                                                             \
            printf("\nFailed at %s: LINE: %d  (ret:0x%#x!)\n", __FUNCTION__, __LINE__, s32Ret); \
            goto exit;                                                                          \
        }                                                                                       \
    } while (0)
#endif

#ifndef SAMPLE_CHECK_RET
#define SAMPLE_CHECK_RET(express)                                                               \
    do {                                                                                        \
        s32Ret = express;                                                                       \
        if (s32Ret != HI_SUCCESS) {                                                             \
            printf("\nFailed at %s: LINE: %d  (ret:0x%#x!)\n", __FUNCTION__, __LINE__, s32Ret); \
            return s32Ret;                                                                      \
        }                                                                                       \
    } while (0)
#endif

#define SAMPLE_CHECK_U32PRODUCT_OVERFLOW_RET(value1, value2)         \
    do {                                                             \
        if ((HI_U32)SAMPLE_U32MAX_NUM / (value1) < (value2)) {       \
            printf("\n%u * %u product owerflow!\n", value1, value2); \
            return HI_FAILURE;                                       \
        }                                                            \
    } while (0)

#define SAMPLE_CHECK_U64SUM_OVERFLOW_RET(value1, value2)             \
    do {                                                             \
        if ((HI_U64)SAMPLE_U64MAX_NUM - (value1) < (value2)) {       \
            printf("\n%llu + %llu sum owerflow!\n", value1, value2); \
            return HI_FAILURE;                                       \
        }                                                            \
    } while (0)

extern const HI_MAPI_SENSOR_ATTR_S g_stSensor1080PMode0;
extern const HI_MAPI_SENSOR_ATTR_S g_stSensor4MMode0;
extern const HI_MAPI_SENSOR_ATTR_S g_stSensor4KMode0;
extern const HI_MAPI_SENSOR_ATTR_S g_stSensor4KMode1;
extern const HI_MAPI_SENSOR_ATTR_S g_stSensor12MMode0;
extern const HI_MAPI_SENSOR_ATTR_S g_stSensor12MMode1;

extern const HI_MAPI_VCAP_PIPE_ATTR_S g_stPipe1080P30Fps;
extern const HI_MAPI_VCAP_PIPE_ATTR_S g_stPipe4M30Fps;
extern const HI_MAPI_VCAP_PIPE_ATTR_S g_stPipe4K30Fps;
extern const HI_MAPI_VCAP_PIPE_ATTR_S g_stPipe12M30Fps;

extern const HI_MAPI_VCAP_DEV_ATTR_S g_stDev1080P;
extern const HI_MAPI_VCAP_DEV_ATTR_S g_stDev4M;
extern const HI_MAPI_VCAP_DEV_ATTR_S g_stDev4K;
extern const HI_MAPI_VCAP_DEV_ATTR_S g_stDev12M;

extern const HI_MAPI_PIPE_CHN_ATTR_S g_stChn1080P30Fps;
extern const HI_MAPI_PIPE_CHN_ATTR_S g_stChn4M30Fps;
extern const HI_MAPI_PIPE_CHN_ATTR_S g_stChn4K30Fps;
extern const HI_MAPI_PIPE_CHN_ATTR_S g_stChn12M30Fps;

extern const HI_MAPI_VPSS_ATTR_S g_stVpss1080P30Fps;
extern const HI_MAPI_VPSS_ATTR_S g_stVpss4M30Fps;
extern const HI_MAPI_VPSS_ATTR_S g_stVpss4K30Fps;
extern const HI_MAPI_VPSS_ATTR_S g_stVpss12M30Fps;

extern const HI_MAPI_VPORT_ATTR_S g_stVport320x180P30Fps;
extern const HI_MAPI_VPORT_ATTR_S g_stVport640x480P30Fps;
extern const HI_MAPI_VPORT_ATTR_S g_stVport1080P30Fps;
extern const HI_MAPI_VPORT_ATTR_S g_stVport4M30Fps;
extern const HI_MAPI_VPORT_ATTR_S g_stVport4K30Fps;
extern const HI_MAPI_VPORT_ATTR_S g_stVport1080P30Fps16Bpp;
extern const HI_MAPI_VPORT_ATTR_S g_stVport4K30Fps16Bpp;
extern const HI_MAPI_VPORT_ATTR_S g_stVport12M30Fps;

extern const HI_MAPI_VENC_ATTR_S g_stVenc320x180Jpeg;
extern const HI_MAPI_VENC_ATTR_S g_stVenc1080P30FpsJpeg;
extern const HI_MAPI_VENC_ATTR_S g_stVenc4K30FpsJpeg;
extern const HI_MAPI_VENC_ATTR_S g_stVenc720P30FpsH264;
extern const HI_MAPI_VENC_ATTR_S g_stVenc1080P30FpsVideoH264;
extern const HI_MAPI_VENC_ATTR_S g_stVenc4K30FpsVideoH264;
extern const HI_MAPI_VENC_ATTR_S g_stVenc12M15FpsVideoH264;
extern const HI_MAPI_VENC_ATTR_S g_stVenc640x480P30FpsVideoH265;
extern const HI_MAPI_VENC_ATTR_S g_stVenc1080P30FpsVideoH265;
extern const HI_MAPI_VENC_ATTR_S g_stVenc4K30FpsVideoH265;

extern const VO_PUB_ATTR_S g_stDispHdmi1080P30;
extern const HI_MAPI_DISP_WINDOW_ATTR_S g_stWnd1920x1080;


typedef enum SAMPLE_SENSOR_SEQ_E {
    SAMPLE_SENSOR_1080P30,
    SAMPLE_SENSOR_4M30,
    SAMPLE_SENSOR_4K30,
    SAMPLE_SENSOR_4K60,
    SAMPLE_SENSOR_12M,
    SAMPLE_SENSOR_DEFAULT, /* it is the typical resolution in every chip */
} SAMPLE_SENSOR_SEQ_E;

typedef struct tagSAMPLE_COMM_INITATTR_S {
    VI_VPSS_MODE_S stViVpssMode;
    SIZE_S stResolution;
    HI_U8 u8SnsCnt;
} SAMPLE_COMM_INITATTR_S;

typedef struct tagSAMPLE_LUT_BUFFER_S {
    HI_U64 u64PhyAddr;
    HI_VOID *VirAddr;
    HI_U32 u32BufLen;
} SAMPLE_LUT_BUFFER_S;

typedef enum tagSAMPLE_VB_CFG_MODE_E {
    VB_MODE_1 = 0, /* not stitch */
    VB_MODE_2,     /* stitch */
} SAMPLE_VB_CFG_MODE_E;

typedef struct hiSAMPLE_MODE_COMM_CONFIG_S {
    const HI_MAPI_SENSOR_ATTR_S *pstVcapSensorAttr;
    const HI_MAPI_VCAP_DEV_ATTR_S *pstVcapDevAttr;
    const HI_MAPI_VCAP_PIPE_ATTR_S *pstVcapPipeAttr;
    const HI_MAPI_PIPE_CHN_ATTR_S *pstPipeChnAttr;
    const HI_MAPI_VPSS_ATTR_S *pstVpssAttr;
    const HI_MAPI_VPORT_ATTR_S *pstVPortAttr;
    const HI_MAPI_VENC_ATTR_S *pstRecVencAttr;
    const HI_MAPI_VENC_ATTR_S *pstSnapVencAttr;
} SAMPLE_MODE_COMM_CONFIG_S;

HI_S32 SAMPLE_COMM_Init(SAMPLE_COMM_INITATTR_S stSampleCommAttr, SAMPLE_VB_CFG_MODE_E enVbMode);

HI_S32 SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_SEQ_E enSensorSeq, HI_MAPI_SENSOR_ATTR_S *pstSensorAttr);
HI_S32 SAMPLE_VENC_GetMediaMode(SAMPLE_SENSOR_SEQ_E *penSensorSeq,
    HI_MAPI_SENSOR_ATTR_S *pstVcapSensorAttr);
HI_S32 MAPI_SAMPLE_COMM_GetMediaCFG(SAMPLE_SENSOR_SEQ_E enSensorSeq,
    SAMPLE_MODE_COMM_CONFIG_S *pstCommConfig);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* ___SAMPLE_CFG_H__ */
