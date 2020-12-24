/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_switch.c
 * @brief   sample switch function
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */

#include "sample_comm.h"
#include "sample_public_vcap.h"
#include "sample_public_venc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define SAMPLE_SLEEP_TIME 3

HI_S32 SAMPLE_SwitchSensorMode(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;

    /* start vcap 0 */
    HI_HANDLE hVcapDev0 = SAMPLE_MAPI_VCAP_DEV_0;
    HI_HANDLE hVcapPipe0 = SAMPLE_MAPI_VCAP_PIPE_0;
    HI_HANDLE hPipeChn0 = SAMPLE_MAPI_PIPE_CHN_0;
    /* start vproc 0 */
    HI_HANDLE hVpssHdl0 = 0;
    HI_HANDLE hVportHdl0 = 0;
    /* start venc 0 */
    HI_HANDLE hVencHdl0 = 0;

    HI_MAPI_VCAP_ATTR_S stVcapAttr;
    HI_MAPI_SENSOR_ATTR_S stVcapSensorAttr;
    SAMPLE_COMM_INITATTR_S stSampleCommAttr;
    HI_MAPI_VPSS_ATTR_S stVpssAttr;
    HI_MAPI_VPORT_ATTR_S stVPortAttr;
    HI_MAPI_VENC_ATTR_S stVencAttr;
    HI_MAPI_VENC_CALLBACK_S stVencCB = { HI_NULL, HI_NULL };
    HI_CHAR aszSaveName[FILE_FULL_PATH_MAX_LEN] = {0};

    printf("\n\r\033[1;34m--->>>59v200 current SnsMode is 4K\033[0;39m\n\n");

    /* 4k */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_4K30, &stVcapSensorAttr));

    /* dev0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.stVcapDevAttr, sizeof(HI_MAPI_VCAP_DEV_ATTR_S), &g_stDev4K,
                               sizeof(HI_MAPI_VCAP_DEV_ATTR_S)));
    SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(&stVcapSensorAttr, &stVcapAttr.stVcapDevAttr);
    stVcapAttr.u32PipeBindNum = 1;
    stVcapAttr.aPipeIdBind[0] = hVcapPipe0;

    /* Pipe0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[hVcapPipe0], sizeof(HI_MAPI_VCAP_PIPE_ATTR_S),
                               &g_stPipe4K30Fps,
                               sizeof(HI_MAPI_VCAP_PIPE_ATTR_S)));
    stVcapAttr.astVcapPipeAttr[hVcapPipe0].enPipeType = HI_MAPI_PIPE_TYPE_VIDEO;

    /* pipe0-Chn0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[hVcapPipe0].astPipeChnAttr[hPipeChn0],
                               sizeof(HI_MAPI_PIPE_CHN_ATTR_S), &g_stChn4K30Fps,
                               sizeof(HI_MAPI_PIPE_CHN_ATTR_S)));

    /* VB and media Attr */
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr, sizeof(SAMPLE_COMM_INITATTR_S), 0, sizeof(SAMPLE_COMM_INITATTR_S)));
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr.stViVpssMode, sizeof(VI_VPSS_MODE_S), 0, sizeof(VI_VPSS_MODE_S)));
    stSampleCommAttr.stResolution.u32Width = 4000;
    stSampleCommAttr.stResolution.u32Height = 3000;
    stSampleCommAttr.stViVpssMode.aenMode[hVcapPipe0] = VI_OFFLINE_VPSS_OFFLINE;
    stSampleCommAttr.u8SnsCnt = 1;

    /* init VB and media */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_Init(stSampleCommAttr, VB_MODE_1));
    SAMPLE_CHECK_GOTO(SAMPLE_VCAP_CreateSingle(hVcapDev0, hVcapPipe0, hPipeChn0, &stVcapSensorAttr, &stVcapAttr));

    SAMPLE_CHECK_GOTO(memcpy_s(&stVpssAttr, sizeof(HI_MAPI_VPSS_ATTR_S), &g_stVpss4K30Fps,
                               sizeof(HI_MAPI_VPSS_ATTR_S)));
    SAMPLE_CHECK_GOTO(memcpy_s(&stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S), &g_stVport4K30Fps,
                               sizeof(HI_MAPI_VPORT_ATTR_S)));

    /* set vport0 attr */
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_InitVpss(hVpssHdl0, &stVpssAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_BindVcap(hVcapPipe0, hPipeChn0, hVpssHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetPortAttr(hVpssHdl0, hVportHdl0, &stVPortAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StartPort(hVpssHdl0, hVportHdl0));

    SAMPLE_CHECK_GOTO(memcpy_s(&stVencAttr, sizeof(HI_MAPI_VENC_ATTR_S), &g_stVenc4K30FpsVideoH264,
                               sizeof(HI_MAPI_VENC_ATTR_S)));

    /* venc callback */
    snprintf(aszSaveName, FILE_FULL_PATH_MAX_LEN, "%s_4k30", __FUNCTION__);
    stVencCB.pfnDataCB = SMAPLE_COMM_VENC_DataProc;
    stVencCB.pPrivateData = (HI_VOID *)aszSaveName;

    /* venc0 */
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Init(hVencHdl0, &stVencAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_RegisterCallback(hVencHdl0, &stVencCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_BindVProc(hVpssHdl0, hVportHdl0, hVencHdl0, HI_FALSE));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Start(hVencHdl0, -1));

    sleep(SAMPLE_SLEEP_TIME);
    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_SetAttrEx(hVcapPipe0, hPipeChn0, HI_MAPI_VCAP_CMD_ENTER_STANDBY, HI_NULL, 0));

    /* stop venc 0 */
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Stop(hVencHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_UnBindVProc(hVpssHdl0, hVportHdl0, hVencHdl0, HI_FALSE));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_UnRegisterCallback(hVencHdl0, &stVencCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Deinit(hVencHdl0));

    /* stop vproc 0 */
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StopPort(hVpssHdl0, hVportHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_UnBindVCap(hVcapPipe0, hPipeChn0, hVpssHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_DeinitVpss(hVpssHdl0));

    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_StopPipe(hVcapPipe0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_StopChn(hVcapPipe0, hPipeChn0));
    /* stop vcap 0 */
    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_StopDev(hVcapDev0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_DeinitSensor(hVcapDev0));

    /* 12M Mode */
    printf("\n\r\033[1;34m--->>>59v200 current SnsMode is 12M\033[0;39m\n\n");

    /* sensor0 attr */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_12M, &stVcapSensorAttr));

    /* dev0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.stVcapDevAttr, sizeof(HI_MAPI_VCAP_DEV_ATTR_S), &g_stDev12M,
                               sizeof(HI_MAPI_VCAP_DEV_ATTR_S)));
    SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(&stVcapSensorAttr, &stVcapAttr.stVcapDevAttr);
    stVcapAttr.u32PipeBindNum = 1;
    stVcapAttr.aPipeIdBind[0] = hVcapPipe0;

    /* Pipe0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[hVcapPipe0], sizeof(HI_MAPI_VCAP_PIPE_ATTR_S),
                               &g_stPipe12M30Fps,
                               sizeof(HI_MAPI_VCAP_PIPE_ATTR_S)));
    stVcapAttr.astVcapPipeAttr[hVcapPipe0].stIspPubAttr.f32FrameRate = 19.0f;
    stVcapAttr.astVcapPipeAttr[hVcapPipe0].enPipeType = HI_MAPI_PIPE_TYPE_VIDEO;

    /* Chn0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[hVcapPipe0].astPipeChnAttr[hPipeChn0],
                               sizeof(HI_MAPI_PIPE_CHN_ATTR_S), &g_stChn12M30Fps,
                               sizeof(HI_MAPI_PIPE_CHN_ATTR_S)));

    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_InitSensor(hVcapDev0, &stVcapSensorAttr, HI_TRUE));
    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_SetAttr(hVcapDev0, &stVcapAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_StartDev(hVcapDev0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_StartChn(hVcapPipe0, hPipeChn0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_StartPipe(hVcapPipe0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VCAP_SetAttrEx(hVcapPipe0, hPipeChn0, HI_MAPI_VCAP_CMD_EXIT_STANDBY, HI_NULL, 0));

    SAMPLE_CHECK_GOTO(memcpy_s(&stVpssAttr, sizeof(HI_MAPI_VPSS_ATTR_S), &g_stVpss12M30Fps,
                               sizeof(HI_MAPI_VPSS_ATTR_S)));
    SAMPLE_CHECK_GOTO(memcpy_s(&stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S), &g_stVport12M30Fps,
                               sizeof(HI_MAPI_VPORT_ATTR_S)));

    /* set vport0 attr */
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_InitVpss(hVpssHdl0, &stVpssAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_BindVcap(hVcapPipe0, hPipeChn0, hVpssHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetPortAttr(hVpssHdl0, hVportHdl0, &stVPortAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StartPort(hVpssHdl0, hVportHdl0));

    /* venc callback */
    snprintf(aszSaveName, FILE_FULL_PATH_MAX_LEN, "%s_12M", __FUNCTION__);
    stVencCB.pfnDataCB = SMAPLE_COMM_VENC_DataProc;
    stVencCB.pPrivateData = (HI_VOID *)aszSaveName;
    SAMPLE_CHECK_GOTO(memcpy_s(&stVencAttr, sizeof(HI_MAPI_VENC_ATTR_S), &g_stVenc4K30FpsVideoH264,
                               sizeof(HI_MAPI_VENC_ATTR_S)));

    /* venc0 */
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Init(hVencHdl0, &stVencAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_RegisterCallback(hVencHdl0, &stVencCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_BindVProc(hVpssHdl0, hVportHdl0, hVencHdl0, HI_FALSE));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Start(hVencHdl0, -1));
    sleep(SAMPLE_SLEEP_TIME);

exit:
    /* stop venc 0 */
    HI_MAPI_VENC_Stop(hVencHdl0);
    HI_MAPI_VENC_UnBindVProc(hVpssHdl0, hVportHdl0, hVencHdl0, HI_FALSE);
    HI_MAPI_VENC_UnRegisterCallback(hVencHdl0, &stVencCB);
    HI_MAPI_VENC_Deinit(hVencHdl0);

    /* stop vproc 0 */
    HI_MAPI_VPROC_StopPort(hVpssHdl0, hVportHdl0);
    HI_MAPI_VPROC_UnBindVCap(hVcapPipe0, hPipeChn0, hVpssHdl0);
    HI_MAPI_VPROC_DeinitVpss(hVpssHdl0);

    /* stop vcap 0 */
    SAMPLE_VCAP_DestorySingle(hVcapDev0, hVcapPipe0, hPipeChn0);

    /* stop media */
    SAMPLE_COMM_Deinit();

    return s32Ret;
}

HI_S32 SAMPLE_SwitchViVpssMode(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;

    /* start vcap 0 */
    HI_HANDLE hVcapDev0 = SAMPLE_MAPI_VCAP_DEV_0;
    HI_HANDLE hVcapPipe0 = SAMPLE_MAPI_VCAP_PIPE_0;
    HI_HANDLE hPipeChn0 = SAMPLE_MAPI_PIPE_CHN_0;
    /* start vproc 0 */
    HI_HANDLE hVpssHdl0 = 0;
    HI_HANDLE hVportHdl0 = 0;
    /* start venc 0 */
    HI_HANDLE hVencHdl0 = 0;

    HI_MAPI_VCAP_ATTR_S stVcapAttr;
    HI_MAPI_SENSOR_ATTR_S stVcapSensorAttr;
    SAMPLE_COMM_INITATTR_S stSampleCommAttr;
    HI_MAPI_VPSS_ATTR_S stVpssAttr;
    HI_MAPI_VPORT_ATTR_S stVPortAttr;
    HI_MAPI_VENC_ATTR_S stVencAttr;
    HI_MAPI_VENC_CALLBACK_S stVencCB = { HI_NULL, HI_NULL };
    HI_CHAR aszSaveName[FILE_FULL_PATH_MAX_LEN] = {0};

    printf("\n\r\033[1;34m--->>>59v200 current workMode is vi-off-vpss-off\033[0;39m\n\n");

    /* 4k */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_4K30, &stVcapSensorAttr));

    /* dev0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.stVcapDevAttr, sizeof(HI_MAPI_VCAP_DEV_ATTR_S), &g_stDev4K,
                               sizeof(HI_MAPI_VCAP_DEV_ATTR_S)));
    SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(&stVcapSensorAttr, &stVcapAttr.stVcapDevAttr);
    stVcapAttr.u32PipeBindNum = 1;
    stVcapAttr.aPipeIdBind[0] = hVcapPipe0;

    /* Pipe0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[hVcapPipe0], sizeof(HI_MAPI_VCAP_PIPE_ATTR_S),
                               &g_stPipe4K30Fps,
                               sizeof(HI_MAPI_VCAP_PIPE_ATTR_S)));
    stVcapAttr.astVcapPipeAttr[hVcapPipe0].enPipeType = HI_MAPI_PIPE_TYPE_VIDEO;

    /* pipe0-Chn0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[hVcapPipe0].astPipeChnAttr[hPipeChn0],
                               sizeof(HI_MAPI_PIPE_CHN_ATTR_S), &g_stChn4K30Fps,
                               sizeof(HI_MAPI_PIPE_CHN_ATTR_S)));

    /* VB and media Attr */
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr, sizeof(SAMPLE_COMM_INITATTR_S), 0, sizeof(SAMPLE_COMM_INITATTR_S)));
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr.stViVpssMode, sizeof(VI_VPSS_MODE_S), 0, sizeof(VI_VPSS_MODE_S)));
    stSampleCommAttr.stResolution.u32Width = stVcapSensorAttr.stSize.u32Width;
    stSampleCommAttr.stResolution.u32Height = stVcapSensorAttr.stSize.u32Height;
    stSampleCommAttr.stViVpssMode.aenMode[hVcapPipe0] = VI_OFFLINE_VPSS_OFFLINE;
    stSampleCommAttr.u8SnsCnt = 1;

    /* init VB and media */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_Init(stSampleCommAttr, VB_MODE_1));
    SAMPLE_CHECK_GOTO(SAMPLE_VCAP_CreateSingle(hVcapDev0, hVcapPipe0, hPipeChn0, &stVcapSensorAttr, &stVcapAttr));

    SAMPLE_CHECK_GOTO(memcpy_s(&stVpssAttr, sizeof(HI_MAPI_VPSS_ATTR_S), &g_stVpss4K30Fps,
                               sizeof(HI_MAPI_VPSS_ATTR_S)));
    SAMPLE_CHECK_GOTO(memcpy_s(&stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S), &g_stVport4K30Fps,
                               sizeof(HI_MAPI_VPORT_ATTR_S)));

    /* set vport0 attr */
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_InitVpss(hVpssHdl0, &stVpssAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_BindVcap(hVcapPipe0, hPipeChn0, hVpssHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetPortAttr(hVpssHdl0, hVportHdl0, &stVPortAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StartPort(hVpssHdl0, hVportHdl0));

    SAMPLE_CHECK_GOTO(memcpy_s(&stVencAttr, sizeof(HI_MAPI_VENC_ATTR_S), &g_stVenc4K30FpsVideoH264,
                               sizeof(HI_MAPI_VENC_ATTR_S)));

    /* venc callback */
    snprintf(aszSaveName, FILE_FULL_PATH_MAX_LEN, "%s_4k30", __FUNCTION__);
    stVencCB.pfnDataCB = SMAPLE_COMM_VENC_DataProc;
    stVencCB.pPrivateData = (HI_VOID *)aszSaveName;

    /* venc0 */
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Init(hVencHdl0, &stVencAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_RegisterCallback(hVencHdl0, &stVencCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_BindVProc(hVpssHdl0, hVportHdl0, hVencHdl0, HI_FALSE));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Start(hVencHdl0, -1));

    sleep(SAMPLE_SLEEP_TIME);

    /* stop venc 0 */
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Stop(hVencHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_UnBindVProc(hVpssHdl0, hVportHdl0, hVencHdl0, HI_FALSE));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_UnRegisterCallback(hVencHdl0, &stVencCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Deinit(hVencHdl0));

    /* stop vproc 0 */
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StopPort(hVpssHdl0, hVportHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_UnBindVCap(hVcapPipe0, hPipeChn0, hVpssHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_DeinitVpss(hVpssHdl0));

    SAMPLE_CHECK_GOTO(SAMPLE_VCAP_DestorySingle(hVcapDev0, hVcapPipe0, hPipeChn0));

    SAMPLE_CHECK_GOTO(SAMPLE_COMM_Deinit());

    /* 12M Mode */
    printf("\n\r\033[1;34m--->>>59v200 current workMode is vi-on-vpss-off\033[0;39m\n\n");

    /* sensor0 attr */
    SAMPLE_CHECK_GOTO(SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_4K30, &stVcapSensorAttr));

    /* dev0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.stVcapDevAttr, sizeof(HI_MAPI_VCAP_DEV_ATTR_S), &g_stDev4K,
                               sizeof(HI_MAPI_VCAP_DEV_ATTR_S)));
    SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(&stVcapSensorAttr, &stVcapAttr.stVcapDevAttr);
    stVcapAttr.u32PipeBindNum = 1;
    stVcapAttr.aPipeIdBind[0] = hVcapPipe0;

    /* Pipe0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[hVcapPipe0], sizeof(HI_MAPI_VCAP_PIPE_ATTR_S),
                               &g_stPipe4K30Fps,
                               sizeof(HI_MAPI_VCAP_PIPE_ATTR_S)));
    stVcapAttr.astVcapPipeAttr[hVcapPipe0].enPipeType = HI_MAPI_PIPE_TYPE_VIDEO;

    /* Chn0 attr */
    SAMPLE_CHECK_GOTO(memcpy_s(&stVcapAttr.astVcapPipeAttr[hVcapPipe0].astPipeChnAttr[hPipeChn0],
                               sizeof(HI_MAPI_PIPE_CHN_ATTR_S), &g_stChn4K30Fps,
                               sizeof(HI_MAPI_PIPE_CHN_ATTR_S)));
    /* VB and media Attr */
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr, sizeof(SAMPLE_COMM_INITATTR_S), 0, sizeof(SAMPLE_COMM_INITATTR_S)));
    SAMPLE_CHECK_GOTO(memset_s(&stSampleCommAttr.stViVpssMode, sizeof(VI_VPSS_MODE_S), 0, sizeof(VI_VPSS_MODE_S)));
    stSampleCommAttr.stResolution.u32Width = stVcapSensorAttr.stSize.u32Width;
    stSampleCommAttr.stResolution.u32Height = stVcapSensorAttr.stSize.u32Height;
    stSampleCommAttr.stViVpssMode.aenMode[hVcapPipe0] = VI_ONLINE_VPSS_OFFLINE;
    stSampleCommAttr.u8SnsCnt = 1;

    SAMPLE_CHECK_GOTO(SAMPLE_COMM_Init(stSampleCommAttr, VB_MODE_1));
    SAMPLE_CHECK_GOTO(SAMPLE_VCAP_CreateSingle(hVcapDev0, hVcapPipe0, hPipeChn0, &stVcapSensorAttr, &stVcapAttr));

    SAMPLE_CHECK_GOTO(memcpy_s(&stVpssAttr, sizeof(HI_MAPI_VPSS_ATTR_S), &g_stVpss4K30Fps,
                               sizeof(HI_MAPI_VPSS_ATTR_S)));
    SAMPLE_CHECK_GOTO(memcpy_s(&stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S), &g_stVport4K30Fps,
                               sizeof(HI_MAPI_VPORT_ATTR_S)));

    /* set vport0 attr */
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_InitVpss(hVpssHdl0, &stVpssAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_BindVcap(hVcapPipe0, hPipeChn0, hVpssHdl0));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_SetPortAttr(hVpssHdl0, hVportHdl0, &stVPortAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VPROC_StartPort(hVpssHdl0, hVportHdl0));

    /* venc callback */
    snprintf(aszSaveName, FILE_FULL_PATH_MAX_LEN, "%s_4k", __FUNCTION__);
    stVencCB.pfnDataCB = SMAPLE_COMM_VENC_DataProc;
    stVencCB.pPrivateData = (HI_VOID *)aszSaveName;
    SAMPLE_CHECK_GOTO(memcpy_s(&stVencAttr, sizeof(HI_MAPI_VENC_ATTR_S), &g_stVenc4K30FpsVideoH264,
                               sizeof(HI_MAPI_VENC_ATTR_S)));

    /* venc0 */
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Init(hVencHdl0, &stVencAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_RegisterCallback(hVencHdl0, &stVencCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_BindVProc(hVpssHdl0, hVportHdl0, hVencHdl0, HI_FALSE));
    SAMPLE_CHECK_GOTO(HI_MAPI_VENC_Start(hVencHdl0, -1));
    sleep(SAMPLE_SLEEP_TIME);

exit:
    /* stop venc 0 */
    HI_MAPI_VENC_Stop(hVencHdl0);
    HI_MAPI_VENC_UnBindVProc(hVpssHdl0, hVportHdl0, hVencHdl0, HI_FALSE);
    HI_MAPI_VENC_UnRegisterCallback(hVencHdl0, &stVencCB);
    HI_MAPI_VENC_Deinit(hVencHdl0);

    /* stop vproc 0 */
    HI_MAPI_VPROC_StopPort(hVpssHdl0, hVportHdl0);
    HI_MAPI_VPROC_UnBindVCap(hVcapPipe0, hPipeChn0, hVpssHdl0);
    HI_MAPI_VPROC_DeinitVpss(hVpssHdl0);

    SAMPLE_VCAP_DestorySingle(hVcapDev0, hVcapPipe0, hPipeChn0);

    /* stop media */
    SAMPLE_COMM_Deinit();

    return s32Ret;
}

HI_VOID SAMPLE_StandbySceneUsage()
{
    printf("Usage : 59v200 don't use double pipe\n");
    printf("\t 0) 59v200 sensor switch mode.\n");
    printf("\t 1) 59v200 change VI-VPSS Mode, Need to stop ISP.\n");
    printf("\t q/Q) quit\n");
    return;
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

    SAMPLE_StandbySceneUsage();
    ch = (char)getchar();
    (void)getchar();

    switch (ch) {
        case '0':
            s32Ret = SAMPLE_SwitchSensorMode();

            if (s32Ret != HI_SUCCESS) {
                printf("SAMPLE_SwitchSensorMode fail s32Ret:%x\n", s32Ret);
                return HI_FAILURE;
            }
            break;

        case '1':
            s32Ret = SAMPLE_SwitchViVpssMode();

            if (s32Ret != HI_SUCCESS) {
                printf("SAMPLE_SwitchViVpssMode fail s32Ret:%x\n", s32Ret);
                return HI_FAILURE;
            }
            break;

        default:
            printf("the index is invaild!\n");
            SAMPLE_StandbySceneUsage();
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
