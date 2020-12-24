/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_venc.c
 * @brief   sample venc function
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */

#include "sample_comm.h"
#include "sample_public_vcap.h"
#include "sample_public_venc.h"
#include "sample_public_disp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

HI_VOID SAMPLE_VENC_Usage()
{
    printf("Usage : \n");
    printf("\t 0) 4K@30fps H.264 record + 1080p@30fps H.265 record + 1080p HDMI Preview.\n");
    printf("\t 1) 4K@30fps JPEG single snap + 1080p HDMI Preview.\n");
    printf("\t 2) record adapt sensor config + 1080p HDMI Preview.\n");
    printf("\t 3) 4K@30fps JPEG single snap + 1080p HDMI Preview. bind extern vport.\n");
    printf("\t q/Q) quit\n");
    return;
}

/******************************************************************************
* function : record adapt sensor config .
******************************************************************************/
HI_S32 SAMPLE_VENC_Record(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8 u8SnsCnt = 1;
    HI_HANDLE VcapDev0 = SAMPLE_MAPI_VCAP_DEV_0;
    HI_HANDLE VcapPipe0 = SAMPLE_MAPI_VCAP_PIPE_0;
    HI_HANDLE PipeChn0 = SAMPLE_MAPI_PIPE_CHN_0;
    HI_MAPI_VCAP_ATTR_S stVcapAttr;
    HI_MAPI_SENSOR_ATTR_S stVcapSensorAttr;
    SAMPLE_COMM_INITATTR_S stSampleCommAttr;
    SAMPLE_SENSOR_SEQ_E enSensorSeq = SAMPLE_SENSOR_1080P30;

    HI_HANDLE VpssHdl0 = 0;
    HI_HANDLE VPortHdl0 = 0;
    HI_BOOL bStitch = HI_FALSE;
    HI_MAPI_VPSS_ATTR_S stVpssAttr;
    HI_MAPI_VPORT_ATTR_S stVPortAttr;

    HI_HANDLE VencHdl0 = 0;
    HI_S32 s32FrameCnt = -1;
    HI_MAPI_VENC_ATTR_S stVencAttr;
    HI_MAPI_VENC_CALLBACK_S stVencCB;
    SAMPLE_MODE_COMM_CONFIG_S stCommConfig;

    SAMPLE_CHECK_RET(memset_s(&stVencCB,sizeof(HI_MAPI_VENC_CALLBACK_S), 0, sizeof(HI_MAPI_VENC_CALLBACK_S)));

    /* sensor0 and media cfg */
    SAMPLE_CHECK_GOTO(SAMPLE_VENC_GetMediaMode(&enSensorSeq, &stVcapSensorAttr));
    SAMPLE_CHECK_GOTO(MAPI_SAMPLE_COMM_GetMediaCFG(enSensorSeq, &stCommConfig));
    if (enSensorSeq == SAMPLE_SENSOR_4K30) {
        printf("\n----4K@30fps H.264 record + 1080p HDMI preview----\n\n");
    } else {
        printf("\n----1080p@30fps H.265 record + 1080p HDMI preview----\n\n");
    }

    /**************************start vcap *******************************/
    /* dev0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.stVcapDevAttr, sizeof(HI_MAPI_VCAP_DEV_ATTR_S), stCommConfig.pstVcapDevAttr,
        sizeof(HI_MAPI_VCAP_DEV_ATTR_S)));
    SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(&stVcapSensorAttr, &stVcapAttr.stVcapDevAttr);

    stVcapAttr.u32PipeBindNum = 1;
    stVcapAttr.aPipeIdBind[0] = VcapPipe0;

    /* Pipe0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe0], sizeof(HI_MAPI_VCAP_PIPE_ATTR_S),
        stCommConfig.pstVcapPipeAttr, sizeof(HI_MAPI_VCAP_PIPE_ATTR_S)));
    stVcapAttr.astVcapPipeAttr[VcapPipe0].enPipeType = HI_MAPI_PIPE_TYPE_VIDEO;

    /* Chn0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe0].astPipeChnAttr[PipeChn0],
        sizeof(HI_MAPI_PIPE_CHN_ATTR_S), stCommConfig.pstPipeChnAttr, sizeof(HI_MAPI_PIPE_CHN_ATTR_S)));

    /* VB and media Attr */
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr, sizeof(SAMPLE_COMM_INITATTR_S), 0, sizeof(SAMPLE_COMM_INITATTR_S)));
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr.stViVpssMode, sizeof(VI_VPSS_MODE_S), 0, sizeof(VI_VPSS_MODE_S)));
    stSampleCommAttr.stResolution.u32Width = stVcapSensorAttr.stSize.u32Width;
    stSampleCommAttr.stResolution.u32Height = stVcapSensorAttr.stSize.u32Height;
    stSampleCommAttr.stViVpssMode.aenMode[VcapPipe0] = VI_OFFLINE_VPSS_OFFLINE;
    stSampleCommAttr.u8SnsCnt = u8SnsCnt;

    /* init VB and media */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_Init(stSampleCommAttr, VB_MODE_1));

    /* vcap */
    SAMPLE_CHECK_GOTO(SAMPLE_VCAP_CreateSingle(VcapDev0, VcapPipe0, PipeChn0, &stVcapSensorAttr, &stVcapAttr));
    /**************************start vproc *******************************/
    /* Vpss0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVpssAttr, sizeof(HI_MAPI_VPSS_ATTR_S), stCommConfig.pstVpssAttr,
        sizeof(HI_MAPI_VPSS_ATTR_S)));

    /* VPort0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S), stCommConfig.pstVPortAttr,
        sizeof(HI_MAPI_VPORT_ATTR_S)));

    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_InitVpss(VpssHdl0, &stVpssAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_BindVcap(VcapPipe0, PipeChn0, VpssHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetPortAttr(VpssHdl0, VPortHdl0, &stVPortAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StartPort(VpssHdl0, VPortHdl0));

    /**************************start preview *******************************/

    SAMPLE_CHECK_GOTO(SAMPLE_COMM_StartPreview(VpssHdl0, VPortHdl0, bStitch));

    /**************************start venc *******************************/
    SAMPLE_CHECK_GOTO(memcpy_s(&stVencAttr, sizeof(HI_MAPI_VENC_ATTR_S), stCommConfig.pstRecVencAttr,
        sizeof(HI_MAPI_VENC_ATTR_S)));
    stVencCB.pfnDataCB = SMAPLE_COMM_VENC_DataProc;
    stVencCB.pPrivateData = (HI_VOID *)__FUNCTION__;

    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Init(VencHdl0, &stVencAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_RegisterCallback(VencHdl0, &stVencCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_BindVProc(VpssHdl0, VPortHdl0, VencHdl0, bStitch));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Start(VencHdl0, s32FrameCnt));

    printf("Press Enter key to stop record...\n");
    (void)getchar();

exit:

    /* stop venc 0 */
    HI_MAPI_VENC_Stop(VencHdl0);
    HI_MAPI_VENC_UnBindVProc(VpssHdl0, VPortHdl0, VencHdl0, bStitch);
    HI_MAPI_VENC_UnRegisterCallback(VencHdl0, &stVencCB);
    HI_MAPI_VENC_Deinit(VencHdl0);

    /* stop preview */
    SAMPLE_COMM_StopPreview(VpssHdl0, VPortHdl0, bStitch);

    /* stop vproc 0 */
    HI_MAPI_VPROC_StopPort(VpssHdl0, VPortHdl0);
    HI_MAPI_VPROC_UnBindVCap(VcapPipe0, PipeChn0, VpssHdl0);
    HI_MAPI_VPROC_DeinitVpss(VpssHdl0);

    /* stop vcap 0 */
    SAMPLE_VCAP_DestorySingle(VcapDev0, VcapPipe0, PipeChn0);

    /* stop media */
    SAMPLE_COMM_Deinit();

    return s32Ret;
}

/******************************************************************************
* function : 4K@30fps H.264 record + 1080p@30fps H.265 record + 1080p HDMI preview.
******************************************************************************/
HI_S32 SAMPLE_VENC_TwoRecord(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8 u8SnsCnt = 1;
    HI_HANDLE VcapDev0 = SAMPLE_MAPI_VCAP_DEV_0;
    HI_HANDLE VcapPipe0 = SAMPLE_MAPI_VCAP_PIPE_0;
    HI_HANDLE PipeChn0 = SAMPLE_MAPI_PIPE_CHN_0;
    HI_MAPI_VCAP_ATTR_S stVcapAttr;
    HI_MAPI_SENSOR_ATTR_S stVcapSensorAttr;
    SAMPLE_COMM_INITATTR_S stSampleCommAttr;

    HI_HANDLE VpssHdl0 = 0;
    HI_HANDLE VPortHdl0 = 0;
    HI_BOOL bStitch = HI_FALSE;
    HI_MAPI_VPSS_ATTR_S stVpssAttr;
    HI_MAPI_VPORT_ATTR_S stVPortAttr;

    HI_HANDLE VencHdl0 = 0;
    HI_HANDLE VencHdl1 = 1;
    HI_S32 s32FrameCnt = -1;
    HI_MAPI_VENC_ATTR_S stVencAttr;
    HI_MAPI_VENC_CALLBACK_S stVencCB;

    SAMPLE_CHECK_GOTO(memset_s(&stVencCB, sizeof(HI_MAPI_VENC_CALLBACK_S), 0, sizeof(HI_MAPI_VENC_CALLBACK_S)));

    printf("\n----4K@30fps H.264 record + 1080p@30fps H.265 record + 1080p HDMI preview----\n\n");

    /**************************start vcap *******************************/
    /* sensor0 attr */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_4K30, &stVcapSensorAttr));

    /* dev0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.stVcapDevAttr, sizeof(HI_MAPI_VCAP_DEV_ATTR_S), &g_stDev4K,
        sizeof(HI_MAPI_VCAP_DEV_ATTR_S)));
    SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(&stVcapSensorAttr, &stVcapAttr.stVcapDevAttr);
    stVcapAttr.u32PipeBindNum = 1;
    stVcapAttr.aPipeIdBind[0] = VcapPipe0;

    /* Pipe0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe0], sizeof(HI_MAPI_VCAP_PIPE_ATTR_S),
        &g_stPipe4K30Fps, sizeof(HI_MAPI_VCAP_PIPE_ATTR_S)));
    stVcapAttr.astVcapPipeAttr[VcapPipe0].enPipeType = HI_MAPI_PIPE_TYPE_VIDEO;

    /* Chn0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe0].astPipeChnAttr[PipeChn0],
        sizeof(HI_MAPI_PIPE_CHN_ATTR_S), &g_stChn4K30Fps, sizeof(HI_MAPI_PIPE_CHN_ATTR_S)));

    /* VB and media Attr */
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr, sizeof(SAMPLE_COMM_INITATTR_S), 0, sizeof(SAMPLE_COMM_INITATTR_S)));
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr.stViVpssMode, sizeof(VI_VPSS_MODE_S), 0, sizeof(VI_VPSS_MODE_S)));
    stSampleCommAttr.stResolution.u32Width = stVcapSensorAttr.stSize.u32Width;
    stSampleCommAttr.stResolution.u32Height = stVcapSensorAttr.stSize.u32Height;
    stSampleCommAttr.stViVpssMode.aenMode[VcapPipe0] = VI_OFFLINE_VPSS_OFFLINE;
    stSampleCommAttr.u8SnsCnt = u8SnsCnt;

    /* init VB and media */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_Init(stSampleCommAttr, VB_MODE_1));

    /* vcap */
    SAMPLE_CHECK_GOTO(SAMPLE_VCAP_CreateSingle(VcapDev0, VcapPipe0, PipeChn0, &stVcapSensorAttr, &stVcapAttr));

    /**************************start vproc *******************************/
    /* Vpss0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVpssAttr, sizeof(HI_MAPI_VPSS_ATTR_S), &g_stVpss4K30Fps,
        sizeof(HI_MAPI_VPSS_ATTR_S)));

    /* VPort0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S), &g_stVport4K30Fps,
        sizeof(HI_MAPI_VPORT_ATTR_S)));

    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_InitVpss(VpssHdl0, &stVpssAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_BindVcap(VcapPipe0, PipeChn0, VpssHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetPortAttr(VpssHdl0, VPortHdl0, &stVPortAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StartPort(VpssHdl0, VPortHdl0));

    /**************************start preview *******************************/

    SAMPLE_CHECK_GOTO(SAMPLE_COMM_StartPreview(VpssHdl0, VPortHdl0, bStitch));

    /**************************start venc *******************************/
    /* venc 0 H264 big stream */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVencAttr,sizeof(HI_MAPI_VENC_ATTR_S), &g_stVenc4K30FpsVideoH264,
        sizeof(HI_MAPI_VENC_ATTR_S)));
    stVencCB.pfnDataCB = SMAPLE_COMM_VENC_DataProc;
    stVencCB.pPrivateData = (HI_VOID *)__FUNCTION__;

    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Init(VencHdl0, &stVencAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_RegisterCallback(VencHdl0, &stVencCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_BindVProc(VpssHdl0, VPortHdl0, VencHdl0, bStitch));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Start(VencHdl0, s32FrameCnt));

    /* venc 1 H265 little stream */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVencAttr, sizeof(HI_MAPI_VENC_ATTR_S), &g_stVenc1080P30FpsVideoH265,
        sizeof(HI_MAPI_VENC_ATTR_S)));
    stVencCB.pfnDataCB = SMAPLE_COMM_VENC_DataProc;
    stVencCB.pPrivateData = (HI_VOID *)__FUNCTION__;

    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Init(VencHdl1, &stVencAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_RegisterCallback(VencHdl1, &stVencCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_BindVProc(VpssHdl0, VPortHdl0, VencHdl1, bStitch));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Start(VencHdl1, s32FrameCnt));

    printf("Press Enter key to stop record...\n");
    (void)getchar();

exit:

    /* stop venc 1 */
    HI_MAPI_VENC_Stop(VencHdl1);
    HI_MAPI_VENC_UnBindVProc(VpssHdl0, VPortHdl0, VencHdl1, bStitch);
    HI_MAPI_VENC_UnRegisterCallback(VencHdl1, &stVencCB);
    HI_MAPI_VENC_Deinit(VencHdl1);

    /* stop venc 0 */
    HI_MAPI_VENC_Stop(VencHdl0);
    HI_MAPI_VENC_UnBindVProc(VpssHdl0, VPortHdl0, VencHdl0, bStitch);
    HI_MAPI_VENC_UnRegisterCallback(VencHdl0, &stVencCB);
    HI_MAPI_VENC_Deinit(VencHdl0);

    /* stop preview */
    SAMPLE_COMM_StopPreview(VpssHdl0, VPortHdl0, bStitch);

    /* stop vproc 0 */
    HI_MAPI_VPROC_StopPort(VpssHdl0, VPortHdl0);
    HI_MAPI_VPROC_UnBindVCap(VcapPipe0, PipeChn0, VpssHdl0);
    HI_MAPI_VPROC_DeinitVpss(VpssHdl0);

    /* stop vcap 0 */
    SAMPLE_VCAP_DestorySingle(VcapDev0, VcapPipe0, PipeChn0);

    /* stop media */
    SAMPLE_COMM_Deinit();

    return s32Ret;
}

/* 4K@30fps single PIPE Snap + 1080p HDMI preview */
HI_S32 SAMPLE_VENC_SinglePipeSnap(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8 u8SnsCnt = 1;
    HI_U32 u32FrameCnt = 2;
    HI_HANDLE VcapDev0 = SAMPLE_MAPI_VCAP_DEV_0;
    HI_HANDLE VcapPipe0 = SAMPLE_MAPI_VCAP_PIPE_0;
    HI_HANDLE PipeChn0 = SAMPLE_MAPI_PIPE_CHN_0;
    HI_MAPI_VCAP_ATTR_S stVcapAttr;
    HI_MAPI_SENSOR_ATTR_S stVcapSensorAttr;
    SAMPLE_COMM_INITATTR_S stSampleCommAttr;
    HI_MAPI_VCAP_SNAP_ATTR_S stSnapAttr;

    HI_HANDLE VpssHdl0 = 0;
    HI_HANDLE VPortHdl0 = 0;
    HI_BOOL bStitch = HI_FALSE;
    HI_MAPI_VPSS_ATTR_S stVpssAttr;
    HI_MAPI_VPORT_ATTR_S stVPortAttr;

    HI_HANDLE VencHdl0 = 0;
    HI_S32 s32FrameCnt = u32FrameCnt;
    HI_MAPI_VENC_ATTR_S stVencAttr;
    HI_MAPI_VENC_CALLBACK_S stVencCB;

    SAMPLE_CHECK_GOTO(memset_s(&stVencCB,sizeof(HI_MAPI_VENC_CALLBACK_S), 0, sizeof(HI_MAPI_VENC_CALLBACK_S)));

    printf("\n----4K@30fps single PIPE Snap + 1080p HDMI preview----\n\n");

    /**************************start vcap *******************************/
    /* sensor0 attr */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_4K30, &stVcapSensorAttr));

    /* dev0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.stVcapDevAttr, sizeof(HI_MAPI_VCAP_DEV_ATTR_S), &g_stDev4K,
        sizeof(HI_MAPI_VCAP_DEV_ATTR_S)));
    SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(&stVcapSensorAttr, &stVcapAttr.stVcapDevAttr);
    stVcapAttr.u32PipeBindNum = 1;
    stVcapAttr.aPipeIdBind[0] = VcapPipe0;

    /* Pipe0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe0], sizeof(HI_MAPI_VCAP_PIPE_ATTR_S),
        &g_stPipe4K30Fps, sizeof(HI_MAPI_VCAP_PIPE_ATTR_S)));
    stVcapAttr.astVcapPipeAttr[VcapPipe0].enPipeType = HI_MAPI_PIPE_TYPE_VIDEO;

    /* Chn0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe0].astPipeChnAttr[PipeChn0],
        sizeof(HI_MAPI_PIPE_CHN_ATTR_S), &g_stChn4K30Fps, sizeof(HI_MAPI_PIPE_CHN_ATTR_S)));

    /* VB and media Attr */
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr, sizeof(SAMPLE_COMM_INITATTR_S), 0, sizeof(SAMPLE_COMM_INITATTR_S)));
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr.stViVpssMode, sizeof(VI_VPSS_MODE_S), 0, sizeof(VI_VPSS_MODE_S)));
    stSampleCommAttr.stResolution.u32Width = stVcapSensorAttr.stSize.u32Width;
    stSampleCommAttr.stResolution.u32Height = stVcapSensorAttr.stSize.u32Height;
    stSampleCommAttr.stViVpssMode.aenMode[VcapPipe0] = VI_ONLINE_VPSS_OFFLINE;
    stSampleCommAttr.u8SnsCnt = u8SnsCnt;

    /* init VB and media */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_Init(stSampleCommAttr, VB_MODE_1));

    /* vcap */
    SAMPLE_CHECK_GOTO(SAMPLE_VCAP_CreateSingle(VcapDev0, VcapPipe0, PipeChn0, &stVcapSensorAttr, &stVcapAttr));

    /**************************start vproc *******************************/
    /* Vpss0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVpssAttr, sizeof(HI_MAPI_VPSS_ATTR_S), &g_stVpss4K30Fps,
        sizeof(HI_MAPI_VPSS_ATTR_S)));

    /* VPort0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S), &g_stVport4K30Fps,
        sizeof(HI_MAPI_VPORT_ATTR_S)));

    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_InitVpss(VpssHdl0, &stVpssAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_BindVcap(VcapPipe0, PipeChn0, VpssHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetPortAttr(VpssHdl0, VPortHdl0, &stVPortAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StartPort(VpssHdl0, VPortHdl0));

    /**************************start preview *******************************/

    SAMPLE_CHECK_GOTO(SAMPLE_COMM_StartPreview(VpssHdl0, VPortHdl0, bStitch));

    /**************************start venc *******************************/
    SAMPLE_CHECK_GOTO(memcpy_s(&stVencAttr, sizeof(HI_MAPI_VENC_ATTR_S), &g_stVenc4K30FpsJpeg,
        sizeof(HI_MAPI_VENC_ATTR_S)));
    stVencCB.pfnDataCB = SMAPLE_COMM_VENC_DataProc;
    stVencCB.pPrivateData = (HI_VOID *)__FUNCTION__;

    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Init(VencHdl0, &stVencAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_RegisterCallback(VencHdl0, &stVencCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_BindVProc(VpssHdl0, VPortHdl0, VencHdl0, bStitch));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Start(VencHdl0, s32FrameCnt));

    /**************************start trigger *******************************/
    stSnapAttr.bLoadCCM = HI_FALSE;
    stSnapAttr.enSnapType = SNAP_TYPE_NORMAL;
    stSnapAttr.stNormalAttr.u32FrameCnt = u32FrameCnt;
    stSnapAttr.stNormalAttr.u32RepeatSendTimes = 1;

    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_SetSnapAttr(VcapPipe0, &stSnapAttr));

    printf("Press Enter key to snap,press 'q' exit snap ...\n");

    while (getchar() != 'q') {
        SAMPLE_CHECK_GOTO(SAMPLE_COMM_VCAP_SnapTrigger(VcapPipe0, VencHdl0, stSnapAttr.stNormalAttr.u32FrameCnt));
    }

exit:

    /* stop venc 0 */
    HI_MAPI_VCAP_StopTrigger(VcapPipe0);
    HI_MAPI_VENC_Stop(VencHdl0);
    HI_MAPI_VENC_UnBindVProc(VpssHdl0, VPortHdl0, VencHdl0, bStitch);
    HI_MAPI_VENC_UnRegisterCallback(VencHdl0, &stVencCB);
    HI_MAPI_VENC_Deinit(VencHdl0);

    /* stop preview */
    SAMPLE_COMM_StopPreview(VpssHdl0, VPortHdl0, bStitch);

    /* stop vproc 0 */
    HI_MAPI_VPROC_StopPort(VpssHdl0, VPortHdl0);
    HI_MAPI_VPROC_UnBindVCap(VcapPipe0, PipeChn0, VpssHdl0);
    HI_MAPI_VPROC_DeinitVpss(VpssHdl0);

    /* stop vcap 0 */
    SAMPLE_VCAP_DestorySingle(VcapDev0, VcapPipe0, PipeChn0);

    /* stop media */
    SAMPLE_COMM_Deinit();

    return s32Ret;
}

/* vcap->venc:double pipe 4K@30fps H.264 record + 4K@30fps H.265 record */
HI_S32 SAMPLE_VENC_BindExtVportSnap(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8 u8SnsCnt = 1;
    HI_U32 u32FrameCnt = 2;
    HI_HANDLE VcapDev0 = SAMPLE_MAPI_VCAP_DEV_0;
    HI_HANDLE VcapPipe0 = SAMPLE_MAPI_VCAP_PIPE_0;
    HI_HANDLE PipeChn0 = SAMPLE_MAPI_PIPE_CHN_0;
    HI_MAPI_VCAP_ATTR_S stVcapAttr;
    HI_MAPI_SENSOR_ATTR_S stVcapSensorAttr;
    SAMPLE_COMM_INITATTR_S stSampleCommAttr;
    HI_MAPI_VCAP_SNAP_ATTR_S stSnapAttr;

    HI_HANDLE VpssHdl0 = 0;
    HI_HANDLE VPortHdl0 = 0;
    HI_HANDLE ExtVPortHdl = 3;
    HI_BOOL bStitch = HI_FALSE;
    HI_MAPI_VPSS_ATTR_S stVpssAttr;
    HI_MAPI_VPORT_ATTR_S stVPortAttr;
    HI_MAPI_EXT_VPORT_ATTR_S stExtVPortAttr;

    HI_HANDLE VencHdl0 = 0;
    HI_S32 s32FrameCnt = u32FrameCnt;
    HI_MAPI_VENC_ATTR_S stVencAttr;
    HI_MAPI_VENC_CALLBACK_S stVencCB;

    SAMPLE_CHECK_GOTO(memset_s(&stVencCB, sizeof(HI_MAPI_VENC_CALLBACK_S), 0, sizeof(HI_MAPI_VENC_CALLBACK_S)));

    printf("\n----4K@30fps single PIPE Snap + 1080p HDMI preview in ExtVproc----\n\n");

    /**************************start vcap *******************************/
    /* sensor0 attr */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_4K30, &stVcapSensorAttr));

    /* dev0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.stVcapDevAttr, sizeof(HI_MAPI_VCAP_DEV_ATTR_S), &g_stDev4K,
        sizeof(HI_MAPI_VCAP_DEV_ATTR_S)));
    SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(&stVcapSensorAttr, &stVcapAttr.stVcapDevAttr);
    stVcapAttr.u32PipeBindNum = 1;
    stVcapAttr.aPipeIdBind[0] = VcapPipe0;

    /* Pipe0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe0], sizeof(HI_MAPI_VCAP_PIPE_ATTR_S),
        &g_stPipe4K30Fps, sizeof(HI_MAPI_VCAP_PIPE_ATTR_S)));
    stVcapAttr.astVcapPipeAttr[VcapPipe0].enPipeType = HI_MAPI_PIPE_TYPE_VIDEO;

    /* Chn0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe0].astPipeChnAttr[PipeChn0],
        sizeof(HI_MAPI_PIPE_CHN_ATTR_S), &g_stChn4K30Fps, sizeof(HI_MAPI_PIPE_CHN_ATTR_S)));

    /* VB and media Attr */
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr, sizeof(SAMPLE_COMM_INITATTR_S), 0, sizeof(SAMPLE_COMM_INITATTR_S)));
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr.stViVpssMode, sizeof(VI_VPSS_MODE_S), 0, sizeof(VI_VPSS_MODE_S)));
    stSampleCommAttr.stResolution.u32Width = stVcapSensorAttr.stSize.u32Width;
    stSampleCommAttr.stResolution.u32Height = stVcapSensorAttr.stSize.u32Height;
    stSampleCommAttr.stViVpssMode.aenMode[VcapPipe0] = VI_OFFLINE_VPSS_OFFLINE;
    stSampleCommAttr.u8SnsCnt = u8SnsCnt;

    /* init VB and media */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_Init(stSampleCommAttr, VB_MODE_1));

    /* vcap */
    SAMPLE_CHECK_GOTO(SAMPLE_VCAP_CreateSingle(VcapDev0, VcapPipe0, PipeChn0, &stVcapSensorAttr, &stVcapAttr));

    /**************************start vproc *******************************/
    /* Vpss0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVpssAttr, sizeof(HI_MAPI_VPSS_ATTR_S), &g_stVpss4K30Fps,
        sizeof(HI_MAPI_VPSS_ATTR_S)));

    /* VPort0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S), &g_stVport4K30Fps,
        sizeof(HI_MAPI_VPORT_ATTR_S)));

    /* extvprot */
    SAMPLE_CHECK_GOTO(memset_s(&stExtVPortAttr, sizeof(HI_MAPI_EXT_VPORT_ATTR_S), 0x00,
        sizeof(HI_MAPI_EXT_VPORT_ATTR_S)));

    stExtVPortAttr.BindPortHdl = VPortHdl0;
    stExtVPortAttr.u32Width = stVPortAttr.u32Width;
    stExtVPortAttr.u32Height = stVPortAttr.u32Height;
    stExtVPortAttr.enVideoFormat = stVPortAttr.enVideoFormat;
    stExtVPortAttr.enPixelFormat = stVPortAttr.enPixelFormat;
    stExtVPortAttr.enCompressMode = stVPortAttr.enCompressMode;
    stExtVPortAttr.stFrameRate.s32SrcFrameRate = stVPortAttr.stFrameRate.s32SrcFrameRate;
    stExtVPortAttr.stFrameRate.s32DstFrameRate = stVPortAttr.stFrameRate.s32DstFrameRate;

    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_InitVpss(VpssHdl0, &stVpssAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_BindVcap(VcapPipe0, PipeChn0, VpssHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetPortAttr(VpssHdl0, VPortHdl0, &stVPortAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StartPort(VpssHdl0, VPortHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetExtPortAttr(VpssHdl0, ExtVPortHdl, &stExtVPortAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StartPort(VpssHdl0, ExtVPortHdl));

    /**************************start preview *******************************/

    SAMPLE_CHECK_GOTO(SAMPLE_COMM_StartPreview(VpssHdl0, ExtVPortHdl, bStitch));

    /**************************start venc *******************************/
    SAMPLE_CHECK_GOTO(memcpy_s(&stVencAttr, sizeof(HI_MAPI_VENC_ATTR_S), &g_stVenc4K30FpsJpeg,
        sizeof(HI_MAPI_VENC_ATTR_S)));
    stVencCB.pfnDataCB = SMAPLE_COMM_VENC_DataProc;
    stVencCB.pPrivateData = (HI_VOID *)__FUNCTION__;

    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Init(VencHdl0, &stVencAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_RegisterCallback(VencHdl0, &stVencCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_BindVProc(VpssHdl0, ExtVPortHdl, VencHdl0, bStitch));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Start(VencHdl0, s32FrameCnt));

    /**************************start trigger *******************************/
    stSnapAttr.bLoadCCM = HI_FALSE;
    stSnapAttr.enSnapType = SNAP_TYPE_NORMAL;
    stSnapAttr.stNormalAttr.u32FrameCnt = u32FrameCnt;
    stSnapAttr.stNormalAttr.u32RepeatSendTimes = 1;

    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_SetSnapAttr(VcapPipe0, &stSnapAttr));

    printf("Press Enter key to snap,press 'q' exit snap ...\n");

    while (getchar() != 'q') {
        SAMPLE_CHECK_GOTO(SAMPLE_COMM_VCAP_SnapTrigger(VcapPipe0, VencHdl0, stSnapAttr.stNormalAttr.u32FrameCnt));
    }

exit:

    /* stop venc 0 */
    HI_MAPI_VCAP_StopTrigger(VcapPipe0);
    HI_MAPI_VENC_Stop(VencHdl0);
    HI_MAPI_VENC_UnBindVProc(VpssHdl0, ExtVPortHdl, VencHdl0, bStitch);
    HI_MAPI_VENC_UnRegisterCallback(VencHdl0, &stVencCB);
    HI_MAPI_VENC_Deinit(VencHdl0);

    /* stop preview */
    SAMPLE_COMM_StopPreview(VpssHdl0, ExtVPortHdl, bStitch);

    /* stop vproc 0 */
    HI_MAPI_VPROC_StopPort(VpssHdl0, ExtVPortHdl);
    HI_MAPI_VPROC_StopPort(VpssHdl0, VPortHdl0);
    HI_MAPI_VPROC_UnBindVCap(VcapPipe0, PipeChn0, VpssHdl0);
    HI_MAPI_VPROC_DeinitVpss(VpssHdl0);

    /* stop vcap 0 */
    SAMPLE_VCAP_DestorySingle(VcapDev0, VcapPipe0, PipeChn0);

    /* stop media */
    SAMPLE_COMM_Deinit();

    return s32Ret;

}

#ifdef __HuaweiLite__
int app_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    HI_S32 s32Ret;
    HI_CHAR ch;

    s32Ret = HI_MAPI_Sys_Init();

    if (s32Ret != HI_SUCCESS) {
        printf("HI_MAPI_Sys_Init fail s32Ret:%x\n", s32Ret);
        return HI_FAILURE;
    }

    SAMPLE_VENC_Usage();
    ch = (char)getchar();
    (void)getchar();

    switch (ch) {
        case '0':
            s32Ret = SAMPLE_VENC_TwoRecord();
            break;

        case '1':
            s32Ret = SAMPLE_VENC_SinglePipeSnap();
            break;

        case '2':
            s32Ret = SAMPLE_VENC_Record();
            break;
        case '3':
            s32Ret = SAMPLE_VENC_BindExtVportSnap();

        default: // input error, return HI_FAILURE.
            printf("the index is invaild!\n");
            SAMPLE_VENC_Usage();
            return HI_FAILURE;
    }

    s32Ret = HI_MAPI_Sys_Deinit();
    if (s32Ret == HI_SUCCESS) {
        printf("The program exited successfully!\n");
    } else {
        printf("The program exits abnormally!\n");
    }

    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

