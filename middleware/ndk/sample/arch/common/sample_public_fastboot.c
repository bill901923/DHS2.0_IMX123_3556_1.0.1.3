/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_comm_audio.c
 * @brief   sample audio comm module
 * @author  HiMobileCam NDK develop team
 * @date  2019-4-16
 */
#include "sample_cfg.h"
#include "sample_public_vcap.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

HI_S32 Fastboot_Start_Audio_Aenc(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;

    HI_HANDLE hAcapHdl = SAMPLE_MAPI_AI_INNER_DEV;
    HI_HANDLE hAcapChnHdl = SAMPLE_MAPI_AI_CHN;
    HI_HANDLE hAencHdl = SAMPLE_MAPI_AENC_CHN;
    HI_MAPI_ACAP_ATTR_S stACapAttr;
    HI_S32 s32InGain;

    /* init and start acap */
    stACapAttr.enWorkmode = AIO_MODE_I2S_MASTER;
    stACapAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
    stACapAttr.enSampleRate = AUDIO_SAMPLE_RATE_48000;
    stACapAttr.enSoundMode = AUDIO_SOUND_MODE_STEREO;
    stACapAttr.enTrackMode = AUDIO_TRACK_NORMAL;
    stACapAttr.u32PtNumPerFrm = SAMPLE_MAPI_AUDIO_POINT_PER_FRAME;
    stACapAttr.enMixerMicMode = ACODEC_MIXER_IN0;
    stACapAttr.enResampleRate = AUDIO_SAMPLE_RATE_48000;
    stACapAttr.enI2sType = AIO_I2STYPE_INNERCODEC;
    s32InGain = SAMPLE_MAPI_AI_IN_GAIN;

    SAMPLE_CHECK_RET(HI_MAPI_ACAP_Init(hAcapHdl, &stACapAttr));
    SAMPLE_CHECK_RET(HI_MAPI_ACAP_Start(hAcapHdl, hAcapChnHdl));
    SAMPLE_CHECK_RET(HI_MAPI_ACAP_SetVolume(hAcapHdl, s32InGain));
    /* init and start aenc */
    SAMPLE_CHECK_RET(HI_MAPI_AENC_RegisterAudioEncoder(HI_MAPI_AUDIO_FORMAT_AACLC));

    HI_MAPI_AENC_ATTR_S stAencAttr = {0};
    HI_MAPI_AENC_ATTR_AAC_S stAacAencAttr = {0};

    stAacAencAttr.enAACType = AAC_TYPE_AACLC;
    stAacAencAttr.enBitRate = AAC_BPS_48K;
    stAacAencAttr.enBitWidth = AUDIO_BIT_WIDTH_16;
    stAacAencAttr.enSmpRate = AUDIO_SAMPLE_RATE_48000;
    stAacAencAttr.enSoundMode = AUDIO_SOUND_MODE_STEREO;
    stAacAencAttr.enTransType = AAC_TRANS_TYPE_ADTS;
    stAacAencAttr.s16BandWidth = 0;

    stAencAttr.enAencFormat = HI_MAPI_AUDIO_FORMAT_AACLC;
    stAencAttr.u32PtNumPerFrm = SAMPLE_MAPI_AUDIO_POINT_PER_FRAME;
    stAencAttr.pValue = &stAacAencAttr;
    stAencAttr.u32Len = sizeof(HI_MAPI_AENC_ATTR_AAC_S);

    SAMPLE_CHECK_RET(HI_MAPI_AENC_Init(hAencHdl, &stAencAttr));
    SAMPLE_CHECK_RET(HI_MAPI_AENC_Start(hAencHdl));
    printf("init and start aenc complete\n");

    /* aenc bind acap */
    SAMPLE_CHECK_RET(HI_MAPI_AENC_BindACap(hAcapHdl, hAcapChnHdl, hAencHdl));
    return HI_SUCCESS;
}

HI_S32 FASTBOOT_StartVcap(HI_HANDLE VcapDev, HI_HANDLE VcapPipe, HI_HANDLE PipeChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_MAPI_VCAP_ATTR_S stVcapAttr;
    HI_MAPI_SENSOR_ATTR_S stVcapSensorAttr;
    HI_U8 u8SnsCnt = 1;

    SAMPLE_CHECK_RET(SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_4K30, &stVcapSensorAttr));

    SAMPLE_CHECK_RET(memcpy_s(&stVcapAttr.stVcapDevAttr, sizeof(HI_MAPI_VCAP_DEV_ATTR_S), &g_stDev4K,
        sizeof(HI_MAPI_VCAP_DEV_ATTR_S)));
    stVcapAttr.u32PipeBindNum = 1;
    stVcapAttr.aPipeIdBind[0] = VcapPipe;

    SAMPLE_CHECK_RET(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe], sizeof(HI_MAPI_VCAP_PIPE_ATTR_S), &g_stPipe4K30Fps,
        sizeof(HI_MAPI_VCAP_PIPE_ATTR_S)));
    SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(&stVcapSensorAttr, &stVcapAttr.stVcapDevAttr);
    stVcapAttr.astVcapPipeAttr[VcapPipe].enPipeType = HI_MAPI_PIPE_TYPE_VIDEO;

    SAMPLE_CHECK_RET(memcpy_s(&stVcapAttr.astVcapPipeAttr[VcapPipe].astPipeChnAttr[PipeChn],
        sizeof(HI_MAPI_PIPE_CHN_ATTR_S), &g_stChn4K30Fps, sizeof(HI_MAPI_PIPE_CHN_ATTR_S)));

    /* VB and media Attr */
    SAMPLE_COMM_INITATTR_S stSampleCommAttr;
    SAMPLE_CHECK_RET(memset_s(&stSampleCommAttr, sizeof(SAMPLE_COMM_INITATTR_S), 0, sizeof(SAMPLE_COMM_INITATTR_S)));
    SAMPLE_CHECK_RET(memset_s(&stSampleCommAttr.stViVpssMode, sizeof(VI_VPSS_MODE_S), 0, sizeof(VI_VPSS_MODE_S)));
    stSampleCommAttr.stResolution.u32Width = stVcapSensorAttr.stSize.u32Width;
    stSampleCommAttr.stResolution.u32Height = stVcapSensorAttr.stSize.u32Height;
    stSampleCommAttr.stViVpssMode.aenMode[VcapPipe] = VI_OFFLINE_VPSS_OFFLINE;
    stSampleCommAttr.u8SnsCnt = u8SnsCnt;
    SAMPLE_CHECK_RET(SAMPLE_COMM_Init(stSampleCommAttr, VB_MODE_1));

    SAMPLE_CHECK_RET(HI_MAPI_VCAP_InitSensor(VcapDev, &stVcapSensorAttr, HI_TRUE));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_SetAttr(VcapDev, &stVcapAttr));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartDev(VcapDev));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartChn(VcapPipe, PipeChn));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartISP(VcapPipe));

    return HI_SUCCESS;
}



// audio + vcap + vproc + venc + disp + hdmi
HI_S32 SAMPLE_COMM_FASTBOOT_All(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_HANDLE hVcapDev0 = 0;
    HI_HANDLE hVcapPipe0 = 0;
    HI_HANDLE hPipeChn0 = 0;

    HI_HANDLE hVpssHdl0 = 0;
    HI_HANDLE hVportHdl0 = 0;
    HI_HANDLE hVportHdl1 = 1;
    HI_BOOL bStitch = HI_FALSE;
    HI_MAPI_VPSS_ATTR_S stVpssAttr;
    HI_MAPI_VPORT_ATTR_S stVPortAttr;

    HI_HANDLE hVencHdl0 = 0;
    HI_HANDLE hVencHdl1 = 1;
    HI_S32 s32FrameCnt = -1;
    HI_MAPI_VENC_ATTR_S stVencAttr;

    HI_HANDLE hDispHdl0 = 0;
    HI_HANDLE hWndHdl0 = 0;

    printf("\n----4K@30fps H.265 record + 720p@30fps H.264 record----\n");

    /**************************start vcap *******************************/
    SAMPLE_CHECK_RET(FASTBOOT_StartVcap(hVcapDev0, hVcapPipe0, hPipeChn0));

    /**************************start audio****************************/
    SAMPLE_CHECK_RET(Fastboot_Start_Audio_Aenc());

    /**************************start vproc *******************************/
    SAMPLE_CHECK_RET(memcpy_s(&stVpssAttr, sizeof(HI_MAPI_VPSS_ATTR_S), &g_stVpss4K30Fps,
        sizeof(HI_MAPI_VPSS_ATTR_S)));
    SAMPLE_CHECK_RET(memcpy_s(&stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S), &g_stVport4K30Fps,
        sizeof(HI_MAPI_VPORT_ATTR_S)));

    SAMPLE_CHECK_RET(HI_MAPI_VPROC_InitVpss(hVpssHdl0, &stVpssAttr));
    SAMPLE_CHECK_RET(HI_MAPI_VPROC_BindVcap(hVcapPipe0, hPipeChn0, hVpssHdl0));
    SAMPLE_CHECK_RET(HI_MAPI_VPROC_SetPortAttr(hVpssHdl0, hVportHdl0, &stVPortAttr));
    SAMPLE_CHECK_RET(HI_MAPI_VPROC_StartPort(hVpssHdl0, hVportHdl0));

    SAMPLE_CHECK_RET(memcpy_s(&stVPortAttr, sizeof(HI_MAPI_VPORT_ATTR_S), &g_stVport4K30Fps,
        sizeof(HI_MAPI_VPORT_ATTR_S)));
    SAMPLE_CHECK_RET(HI_MAPI_VPROC_SetPortAttr(hVpssHdl0, hVportHdl1, &stVPortAttr));
    SAMPLE_CHECK_RET(HI_MAPI_VPROC_StartPort(hVpssHdl0, hVportHdl1));

    /**************************start venc *******************************/
    SAMPLE_CHECK_RET(memcpy_s(&stVencAttr, sizeof(HI_MAPI_VENC_ATTR_S), &g_stVenc4K30FpsVideoH265,
        sizeof(HI_MAPI_VENC_ATTR_S)));
    SAMPLE_CHECK_RET(HI_MAPI_VENC_Init(hVencHdl0, &stVencAttr));
    SAMPLE_CHECK_RET(HI_MAPI_VENC_BindVProc(hVpssHdl0, hVportHdl0, hVencHdl0, bStitch));
    SAMPLE_CHECK_RET(HI_MAPI_VENC_Start(hVencHdl0, s32FrameCnt));

    SAMPLE_CHECK_RET(memcpy_s(&stVencAttr, sizeof(HI_MAPI_VENC_ATTR_S), &g_stVenc720P30FpsH264,
        sizeof(HI_MAPI_VENC_ATTR_S)));
    SAMPLE_CHECK_RET(HI_MAPI_VENC_Init(hVencHdl1, &stVencAttr));
    SAMPLE_CHECK_RET(HI_MAPI_VENC_BindVProc(hVpssHdl0, hVportHdl1, hVencHdl1, bStitch));
    SAMPLE_CHECK_RET(HI_MAPI_VENC_Start(hVencHdl1, s32FrameCnt));

    /**************************start disp *******************************/
    SAMPLE_CHECK_RET(HI_MAPI_DISP_Bind_VProc(hVpssHdl0, hVportHdl0, hDispHdl0, hWndHdl0, bStitch));

    printf("media start ok.\n");
    return HI_SUCCESS;
}

// audio + vcap + venc + disp + hdmi
HI_S32 SAMPLE_COMM_FASTBOOT_NoVproc(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_HANDLE hVcapDev0 = 0;
    HI_HANDLE hVcapPipe0 = 0;
    HI_HANDLE hPipeChn0 = 0;

    HI_HANDLE hVencHdl0 = 0;
    HI_S32 s32FrameCnt = -1;
    HI_MAPI_VENC_ATTR_S stVencAttr;

    HI_HANDLE hDispHdl0 = 0;
    HI_HANDLE hWndHdl0 = 0;

    printf("\n----4K@30fps H.265 record \n");

    /**************************start vcap *******************************/
    SAMPLE_CHECK_RET(FASTBOOT_StartVcap(hVcapDev0, hVcapPipe0, hPipeChn0));

    /**************************start audio****************************/
    SAMPLE_CHECK_RET(Fastboot_Start_Audio_Aenc());

    /**************************start venc *******************************/
    SAMPLE_CHECK_RET(memcpy_s(&stVencAttr, sizeof(HI_MAPI_VENC_ATTR_S), &g_stVenc4K30FpsVideoH264,
        sizeof(HI_MAPI_VENC_ATTR_S)));
    SAMPLE_CHECK_RET(HI_MAPI_VENC_Init(hVencHdl0, &stVencAttr));
    SAMPLE_CHECK_RET(HI_MAPI_VENC_BindVCap(hVcapPipe0, hPipeChn0, hVencHdl0));
    SAMPLE_CHECK_RET(HI_MAPI_VENC_Start(hVencHdl0, s32FrameCnt));

    /**************************start disp *******************************/
    SAMPLE_CHECK_RET(HI_MAPI_DISP_Bind_VCap(hVcapPipe0, hPipeChn0, hDispHdl0, hWndHdl0));

    printf("media start ok.\n");
    return HI_SUCCESS;
}

HI_VOID FASTBOOT_TestUsage(HI_VOID)
{
    printf("\n----1.audio + vcap + vproc + venc + disp + hdmi .\n");
    printf("\n----2.audio + vcap + venc + disp + hdmi.\n");
    printf("\n----pleale input:\n");
    return;
}

HI_VOID FASTBOOT_Sample(HI_VOID)
{
    HI_CHAR cmd[SAMPLE_CMD_MAX_LEN] = { 0 };
    FASTBOOT_TestUsage();
    fgets(cmd, SAMPLE_CMD_MAX_LEN, stdin);
    cmd[SAMPLE_CMD_MAX_LEN - 1] = '\0';

    if (strncmp(cmd, "1", 1) == 0) {
        (void)SAMPLE_COMM_FASTBOOT_All();  // audio + vcap + vproc + venc + disp + hdmi
    } else if (strncmp(cmd, "2", 1) == 0) {
        (void)SAMPLE_COMM_FASTBOOT_NoVproc();  // audio + vcap + venc + disp + hdmi
    } else {
        printf("the index is invaild!\n");
        FASTBOOT_TestUsage();
    }
}


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
