/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_dump.c
 * @brief   sample dump function
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */

#include "sample_comm.h"
#include "sample_public_vcap.h"
#include "sample_public_venc.h"
#include "sample_public_vproc.h"
#include "sample_public_disp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


HI_VOID SAMPLE_VENC_Usage()
{
    printf("Usage : \n");
    printf("\t 0) vcap dump raw.\n");
    printf("\t 1) vpss dump YUV.\n");
    printf("\t q/Q) quit\n");
    return;
}

HI_S32 SAMPLE_DUMP_Raw(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8 u8SnsCnt = 1;

    printf("\n----vcap dump raw.----\n\n");

    /* ************************start vcap ***************************** */
    HI_HANDLE VcapDev0 = SAMPLE_MAPI_VCAP_DEV_0;
    HI_HANDLE VcapPipe0 = SAMPLE_MAPI_VCAP_PIPE_0;
    HI_HANDLE PipeChn0 = SAMPLE_MAPI_PIPE_CHN_0;

    HI_MAPI_VCAP_ATTR_S stVcapAttr;
    HI_MAPI_SENSOR_ATTR_S stVcapSensorAttr;
    SAMPLE_COMM_INITATTR_S stSampleCommAttr;
    HI_MAPI_VCAP_SNAP_ATTR_S stSnapAttr;
    HI_MAPI_DUMP_ATTR_S stDumpAttr;
    HI_MAPI_VCAP_RAW_DATA_S stVCapRawData;

    HI_HANDLE VpssHdl0 = 0;
    HI_HANDLE VPortHdl0 = 0;
    HI_BOOL bStitch = HI_FALSE;
    HI_MAPI_VPSS_ATTR_S stVpssAttr;
    HI_MAPI_VPORT_ATTR_S stVPortAttr;

    HI_HANDLE VencHdl0 = 0;
    HI_S32 s32FrameCnt = 3;
    HI_MAPI_VENC_ATTR_S stVencAttr;
    HI_MAPI_VENC_CALLBACK_S stVencCB = { HI_NULL, HI_NULL };

    /* sensor0 attr */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_4K30, &stVcapSensorAttr));

    /* dev0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.stVcapDevAttr, sizeof(HI_MAPI_VCAP_DEV_ATTR_S), &g_stDev4K, sizeof(HI_MAPI_VCAP_DEV_ATTR_S)));
    SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(&stVcapSensorAttr, &stVcapAttr.stVcapDevAttr);
    stVcapAttr.u32PipeBindNum = 1;
    stVcapAttr.aPipeIdBind[0] = VcapPipe0;

    /* Pipe0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe0], sizeof(HI_MAPI_VCAP_PIPE_ATTR_S), &g_stPipe4K30Fps, sizeof(HI_MAPI_VCAP_PIPE_ATTR_S)));
    stVcapAttr.astVcapPipeAttr[VcapPipe0].enPipeType = HI_MAPI_PIPE_TYPE_VIDEO;

    /* Chn0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe0].astPipeChnAttr[PipeChn0], sizeof(HI_MAPI_PIPE_CHN_ATTR_S), &g_stChn4K30Fps,
           sizeof(HI_MAPI_PIPE_CHN_ATTR_S)));

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

    /* ************************start vproc ***************************** */
    /* Vpss0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVpssAttr, sizeof(HI_MAPI_VPSS_ATTR_S), &g_stVpss4K30Fps, sizeof(HI_MAPI_VPSS_ATTR_S)));

    /* VPort0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S), &g_stVport4K30Fps, sizeof(HI_MAPI_VPORT_ATTR_S)));

    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_InitVpss(VpssHdl0, &stVpssAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_BindVcap(VcapPipe0, PipeChn0, VpssHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetPortAttr(VpssHdl0, VPortHdl0, &stVPortAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StartPort(VpssHdl0, VPortHdl0));

    /* ************************start preview ***************************** */

    SAMPLE_CHECK_GOTO(SAMPLE_COMM_StartPreview(VpssHdl0, VPortHdl0, bStitch));

    /* ************************start venc ***************************** */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVencAttr, sizeof(HI_MAPI_VENC_ATTR_S), &g_stVenc4K30FpsJpeg, sizeof(HI_MAPI_VENC_ATTR_S)));
    stVencCB.pfnDataCB = SMAPLE_COMM_VENC_DataProc;
    stVencCB.pPrivateData = (HI_VOID *)__FUNCTION__;

    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Init(VencHdl0, &stVencAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_RegisterCallback(VencHdl0, &stVencCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_BindVProc(VpssHdl0, VPortHdl0, VencHdl0, bStitch));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Start(VencHdl0, s32FrameCnt));

    /* ************************start trigger ***************************** */
    stSnapAttr.bLoadCCM = HI_FALSE;
    stSnapAttr.enSnapType = SNAP_TYPE_NORMAL;
    stSnapAttr.stNormalAttr.u32FrameCnt = s32FrameCnt;
    stSnapAttr.stNormalAttr.u32RepeatSendTimes = 1;

    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_SetSnapAttr(VcapPipe0, &stSnapAttr));

    /* ************************start dump raw ***************************** */
    SAMPLE_CHECK_GOTO(memset_s(&stDumpAttr, sizeof(HI_MAPI_DUMP_ATTR_S), 0, sizeof(HI_MAPI_DUMP_ATTR_S)));
    stDumpAttr.stDumpAttr.bEnable = HI_TRUE;
    stDumpAttr.stDumpBNRAttr.bEnable = HI_FALSE;
    stDumpAttr.stDumpAttr.u32Depth = s32FrameCnt;
    stDumpAttr.enPixFmt = PIXEL_FORMAT_RGB_BAYER_12BPP;
    snprintf(g_aszDumpFrameName[VcapPipe0], FILE_FULL_PATH_MAX_LEN, "%s_%s_D%d_", __FUNCTION__, "SNAP",
        stDumpAttr.stDumpAttr.u32Depth);
    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_SetDumpRawAttr(VcapPipe0, &stDumpAttr));

    stVCapRawData.pfn_VCAP_RawDataProc = SAMPLE_COMM_VPROC_ProcRawData;

    printf("Press Enter key to dump raw, press 'q' to exit\n");

    while (getchar() != 'q') {
        SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Stop(VencHdl0));
        SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Start(VencHdl0, stSnapAttr.stNormalAttr.u32FrameCnt));
        SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_StartTrigger(VcapPipe0));
        SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_StartDumpRaw(VcapPipe0, stDumpAttr.stDumpAttr.u32Depth, &stVCapRawData));
        SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_StopDumpRaw(VcapPipe0));
        SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_StopTrigger(VcapPipe0));
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

HI_S32 SAMPLE_VPSS_DUMP_YUV(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8 u8SnsCnt = 1;

    printf("\n----vpss dump YUV.----\n\n");

    /* ************************start vcap ***************************** */
    HI_HANDLE VcapDev0 = SAMPLE_MAPI_VCAP_DEV_0;
    HI_HANDLE VcapPipe0 = SAMPLE_MAPI_VCAP_PIPE_0;
    HI_HANDLE PipeChn0 = SAMPLE_MAPI_PIPE_CHN_0;

    HI_MAPI_VCAP_ATTR_S stVcapAttr;
    HI_MAPI_SENSOR_ATTR_S stVcapSensorAttr;
    SAMPLE_COMM_INITATTR_S stSampleCommAttr;

    /* sensor0 attr */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_4K30, &stVcapSensorAttr));

    /* dev0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.stVcapDevAttr, sizeof(HI_MAPI_VCAP_DEV_ATTR_S), &g_stDev4K, sizeof(HI_MAPI_VCAP_DEV_ATTR_S)));
    SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(&stVcapSensorAttr, &stVcapAttr.stVcapDevAttr);
    stVcapAttr.u32PipeBindNum = 1;
    stVcapAttr.aPipeIdBind[0] = VcapPipe0;

    /* Pipe0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe0], sizeof(HI_MAPI_VCAP_PIPE_ATTR_S), &g_stPipe4K30Fps, sizeof(HI_MAPI_VCAP_PIPE_ATTR_S)));
    stVcapAttr.astVcapPipeAttr[VcapPipe0].enPipeType = HI_MAPI_PIPE_TYPE_VIDEO;

    /* Chn0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe0].astPipeChnAttr[PipeChn0], sizeof(HI_MAPI_PIPE_CHN_ATTR_S), &g_stChn4K30Fps,
           sizeof(HI_MAPI_PIPE_CHN_ATTR_S)));

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

    /* ************************start vproc ***************************** */
    HI_HANDLE VpssHdl0 = 0;
    HI_HANDLE VPortHdl0 = 0;
    HI_MAPI_VPSS_ATTR_S stVpssAttr;
    HI_MAPI_VPORT_ATTR_S stVPortAttr;

    /* Vpss0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVpssAttr, sizeof(HI_MAPI_VPSS_ATTR_S), &g_stVpss4K30Fps, sizeof(HI_MAPI_VPSS_ATTR_S)));

    /* VPort0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S), &g_stVport4K30Fps, sizeof(HI_MAPI_VPORT_ATTR_S)));

    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_InitVpss(VpssHdl0, &stVpssAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_BindVcap(VcapPipe0, PipeChn0, VpssHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetPortAttr(VpssHdl0, VPortHdl0, &stVPortAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StartPort(VpssHdl0, VPortHdl0));

    // dump YUV
    HI_MAPI_DUMP_YUV_ATTR_S stDumpYUVAttr;
    stDumpYUVAttr.bEnable = HI_TRUE;
    stDumpYUVAttr.u32Depth = 5;
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetVpssDumpYUVAttr(VpssHdl0, VPortHdl0, &stDumpYUVAttr));

    HI_S32 s32Count = 5;
    HI_DUMP_YUV_CALLBACK_FUNC_S pfunVProcYUVProc;
    pfunVProcYUVProc.pfunVProcYUVProc = SAMPLE_COMM_VPROC_ProcYuvData;
    pfunVProcYUVProc.pPrivateData = "vpss";

    printf("Press Enter key to dump YUV, press 'q' exit ...\n");

    while (getchar() != 'q') {
        SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StartVpssDumpYUV(VpssHdl0, VPortHdl0, s32Count, &pfunVProcYUVProc));
        SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StopVpssDumpYUV(VpssHdl0, VPortHdl0));
        printf("Press Enter key to dump YUV, press 'q' exit ...\n");
    }

    stDumpYUVAttr.bEnable = HI_FALSE;
    stDumpYUVAttr.u32Depth = 0;
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetVpssDumpYUVAttr(VpssHdl0, VPortHdl0, &stDumpYUVAttr));

exit:

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

#ifdef __HuaweiLite__
int app_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    HI_S32 s32Ret;
    HI_CHAR ch;

#ifdef __DualSys__
    s32Ret = HI_MAPI_Sys_Init();

    if (s32Ret != HI_SUCCESS) {
        printf("HI_MAPI_Sys_Init fail s32Ret:%x\n", s32Ret);
        return HI_FAILURE;
    }

#endif

    SAMPLE_VENC_Usage();
    ch = (char)getchar();
    (void)getchar();

    switch (ch) {
        case '0':
            s32Ret = SAMPLE_DUMP_Raw();
            break;

        case '1':
            s32Ret = SAMPLE_VPSS_DUMP_YUV();
            break;

        default:
            printf("the index is invaild!\n");
            SAMPLE_VENC_Usage();
            return HI_FAILURE;
    }

#ifdef __DualSys__
    s32Ret = HI_MAPI_Sys_Deinit();
#endif

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
