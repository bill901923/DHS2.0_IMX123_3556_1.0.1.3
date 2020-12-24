/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_vio.c
 * @brief   sample vio function
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */

#include "sample_comm.h"
#include "sensor_interface_cfg_params.h"
#include "sample_public_audio.h"
#include "sample_public_vcap.h"
#include "sample_public_disp.h"
#if SUPPORT_GYRO
#include "hi_comm_motionfusion.h"
#include "hi_comm_motionsensor.h"
#include "mpi_motionfusion.h"
#include "motionsensor_chip_cmd.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

static HI_CHAR g_sampleId;

static HI_HANDLE g_acapHdl = SAMPLE_MAPI_AI_INNER_DEV;
static HI_HANDLE g_acapChnHdl = SAMPLE_MAPI_AI_CHN;
static HI_HANDLE g_aoHdl = SAMPLE_MAPI_AO_HDMI_DEV;
static HI_HANDLE g_aoChnHdl = SAMPLE_MAPI_AO_CHN;

static HI_BOOL g_bAoStartFlg = HI_FALSE;

#ifdef SUPPORT_HDMI
static HI_BOOL g_bDispInited = HI_FALSE;
static HI_BOOL g_bHdmiStarted = HI_FALSE;
static HI_BOOL g_bHdmiEventProFlg = HI_FALSE;
static pthread_t g_hdmiEventProThd = HI_NULL;
static pthread_t g_sendFrmAOChnThd = HI_NULL;
static HI_S32 g_hdmiEvent = 0;
#define HOTPLUGEVENT 1
#define NOPLUGEVENT  2
#endif
HI_VOID SendFrameToAOChnThread(HI_VOID *args)
{
    HI_S32 s32Ret;
    HI_S32 i = 0;
    HI_HANDLE hAcapHdl = g_acapHdl;
    HI_HANDLE hAcapChnHdl = g_acapChnHdl;
    HI_HANDLE hAoHdl = g_aoHdl;
    HI_HANDLE hAoChnHdl = g_aoChnHdl;
    AUDIO_FRAME_S stFrm;
    AEC_FRAME_S stAecFrm;

    prctl(PR_SET_NAME, (unsigned long)"Hi_SendFrameToAOChnThread", 0, 0, 0);

    while (g_bAoStartFlg) {
        s32Ret = HI_MAPI_ACAP_GetFrame(hAcapHdl, hAcapChnHdl, &stFrm, &stAecFrm);

        if (s32Ret != HI_SUCCESS) {
            usleep(1000 * 5);
            continue;
        }

        i = 0;

        while (i < 10) {
            i++;
            s32Ret = HI_MAPI_AO_SendFrame(hAoHdl, hAoChnHdl, &stFrm, 500);

            if (s32Ret == HI_SUCCESS) {
                break;
            }

            usleep(1000 * 5);
            continue;
        }

        if (i == 10) {
            printf("----------------drop a frame!!!\n");
        }

        s32Ret = HI_MAPI_ACAP_ReleaseFrame(hAcapHdl, hAcapChnHdl, &stFrm, &stAecFrm);
        if (s32Ret != HI_SUCCESS) {
            printf("------HI_MAPI_ACAP_ReleaseFrame error :0x%x!!!\n", s32Ret);
        }
        usleep(1000 * 5);
    }

    return;
}

HI_S32 SAMPLE_GetWindowSize(const VO_PUB_ATTR_S *pstPubAttr, HI_U32 *pu32Width, HI_U32 *pu32Height)
{
    HI_U32 u32Width = 0;
    HI_U32 u32Height = 0;

    switch (pstPubAttr->enIntfSync) {
        case VO_OUTPUT_PAL:
            u32Width = 720;
            u32Height = 576;
            break;

        case VO_OUTPUT_NTSC:
            u32Width = 720;
            u32Height = 480;
            break;

        case VO_OUTPUT_1080P24:
            u32Width = 1920;
            u32Height = 1080;
            break;

        case VO_OUTPUT_1080P25:
            u32Width = 1920;
            u32Height = 1080;
            break;

        case VO_OUTPUT_1080P30:
            u32Width = 1920;
            u32Height = 1080;
            break;

        case VO_OUTPUT_720P50:
            u32Width = 1280;
            u32Height = 720;
            break;

        case VO_OUTPUT_720P60:
            u32Width = 1280;
            u32Height = 720;
            break;

        case VO_OUTPUT_1080I50:
            u32Width = 1920;
            u32Height = 1080;
            break;

        case VO_OUTPUT_1080I60:
            u32Width = 1920;
            u32Height = 1080;
            break;

        case VO_OUTPUT_1080P50:
            u32Width = 1920;
            u32Height = 1080;
            break;

        case VO_OUTPUT_1080P60:
            u32Width = 1920;
            u32Height = 1080;
            break;

        case VO_OUTPUT_576P50:
            u32Width = 720;
            u32Height = 576;
            break;

        case VO_OUTPUT_480P60:
            u32Width = 720;
            u32Height = 480;
            break;

        case VO_OUTPUT_800x600_60:
            u32Width = 800;
            u32Height = 600;
            break;

        case VO_OUTPUT_1024x768_60:
            u32Width = 1024;
            u32Height = 768;
            break;

        case VO_OUTPUT_1280x1024_60:
            u32Width = 1280;
            u32Height = 1024;
            break;

        case VO_OUTPUT_1366x768_60:
            u32Width = 1366;
            u32Height = 768;
            break;

        case VO_OUTPUT_1440x900_60:
            u32Width = 1440;
            u32Height = 900;
            break;

        case VO_OUTPUT_1280x800_60:
            u32Width = 1280;
            u32Height = 800;
            break;

        case VO_OUTPUT_1600x1200_60:
            u32Width = 1600;
            u32Height = 1200;
            break;

        case VO_OUTPUT_1680x1050_60:
            u32Width = 1680;
            u32Height = 1050;
            break;

        case VO_OUTPUT_1920x1200_60:
            u32Width = 1920;
            u32Height = 1200;
            break;

        case VO_OUTPUT_640x480_60:
            u32Width = 640;
            u32Height = 480;
            break;

        case VO_OUTPUT_960H_PAL:
            u32Width = 960;
            u32Height = 576;
            break;

        case VO_OUTPUT_960H_NTSC:
            u32Width = 960;
            u32Height = 480;
            break;

        case VO_OUTPUT_1920x2160_30:
            u32Width = 1920;
            u32Height = 2160;
            break;

        case VO_OUTPUT_2560x1440_30:
            u32Width = 2560;
            u32Height = 1440;
            break;

        case VO_OUTPUT_2560x1600_60:
            u32Width = 2560;
            u32Height = 1600;
            break;

        case VO_OUTPUT_3840x2160_24:
            u32Width = 3840;
            u32Height = 2160;
            break;

        case VO_OUTPUT_3840x2160_25:
            u32Width = 3840;
            u32Height = 2160;
            break;

        case VO_OUTPUT_3840x2160_30:
            u32Width = 3840;
            u32Height = 2160;
            break;

        case VO_OUTPUT_3840x2160_50:
            u32Width = 3840;
            u32Height = 2160;
            break;

        case VO_OUTPUT_3840x2160_60:
            u32Width = 3840;
            u32Height = 2160;
            break;

        case VO_OUTPUT_4096x2160_24:
            u32Width = 4096;
            u32Height = 2160;
            break;

        case VO_OUTPUT_4096x2160_25:
            u32Width = 4096;
            u32Height = 2160;
            break;

        case VO_OUTPUT_4096x2160_30:
            u32Width = 4096;
            u32Height = 2160;
            break;

        case VO_OUTPUT_4096x2160_50:
            u32Width = 4096;
            u32Height = 2160;
            break;

        case VO_OUTPUT_4096x2160_60:
            u32Width = 4096;
            u32Height = 2160;
            break;

        case VO_OUTPUT_320x240_60:
            u32Width = 320;
            u32Height = 240;
            break;

        case VO_OUTPUT_320x240_50:
            u32Width = 320;
            u32Height = 240;
            break;

        case VO_OUTPUT_240x320_50:
            u32Width = 240;
            u32Height = 320;
            break;

        case VO_OUTPUT_240x320_60:
            u32Width = 240;
            u32Height = 320;
            break;

        case VO_OUTPUT_800x600_50:
            u32Width = 800;
            u32Height = 600;
            break;

        case VO_OUTPUT_720x1280_60:
            u32Width = 720;
            u32Height = 1280;
            break;

        case VO_OUTPUT_1080x1920_60:
            u32Width = 1080;
            u32Height = 1920;
            break;

        case VO_OUTPUT_7680x4320_30:
            u32Width = 7680;
            u32Height = 4320;
            break;

        case VO_OUTPUT_USER:
            u32Width = pstPubAttr->stSyncInfo.u16Hact;  // stSyncInfo.u16Hact;
            u32Height = (pstPubAttr->stSyncInfo.bIop) ? pstPubAttr->stSyncInfo.u16Vact :
                pstPubAttr->stSyncInfo.u16Vact * 2;
            break;

        default:
            printf("sample_GetwindowSize ERR\n");
            break;
    }

    *pu32Width = u32Width;
    *pu32Height = u32Height;

    return HI_SUCCESS;
}
HI_VOID HdmiEventProThread(HI_VOID *args)
{
#ifdef SUPPORT_HDMI

    HI_S32 s32Ret;
    HI_S32 i = 0;
    HI_HANDLE hDispHdl0 = 0;
    HI_HANDLE hWndHdl0 = 0;
    HI_MAPI_DISP_ATTR_S stDispAttr;
    HI_MAPI_DISP_VIDEOLAYER_ATTR_S stVideoLayerAttr;
    HI_MAPI_DISP_WINDOW_ATTR_S stWndAttr;
    HI_U32 u32WinWidth;
    HI_U32 u32WinHeight;

    HI_HANDLE hVpssHdl0 = 0;
    HI_HANDLE hVPortHdl0 = 0;
    HI_BOOL bStitch = HI_FALSE;

    HI_HANDLE hAcapHdl = SAMPLE_MAPI_AI_INNER_DEV;
    HI_HANDLE hAcapChnHdl = SAMPLE_MAPI_AI_CHN;
    HI_HANDLE hAoHdl = SAMPLE_MAPI_AO_HDMI_DEV;
    HI_HANDLE hAoChnHdl = SAMPLE_MAPI_AO_CHN;
    AUDIO_SAMPLE_RATE_E enHdmiSamRate = AUDIO_SAMPLE_RATE_BUTT;
    HI_MAPI_ACAP_ATTR_S stACapAttr;
    HI_MAPI_AO_ATTR_S stAoAttr;
    HI_S32 s32InGain;
    HI_S32 s32OutGain;

    HI_HDMI_ID_E enHdmi = HI_HDMI_ID_0;

    prctl(PR_SET_NAME, (unsigned long)"hdmi event pro Thread", 0, 0, 0);

    while (g_bHdmiEventProFlg) {
        if (g_hdmiEvent) {
            switch (g_hdmiEvent) {
                case HOTPLUGEVENT:

                    /* get sink capbility */
                
                    if (!(stsinkcapability.bConnected && stsinkcapability.bIsSinkPowerOn)) {
                        g_hdmiEvent = 0;
                        break;
                    }

                    /* If you have initialized disp you need to deinitialize */
                    if (g_bDispInited) {
                        SAMPLE_CHECK_GOTO(HI_MAPI_DISP_UnBind_VProc(hVpssHdl0, hVPortHdl0, hDispHdl0, hWndHdl0,
                            bStitch));
                        SAMPLE_CHECK_GOTO(HI_MAPI_DISP_StopWindow(hDispHdl0, hWndHdl0));
                        SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Stop(hDispHdl0));
                        SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Deinit(hDispHdl0));
                        g_bDispInited = HI_FALSE;
                    }

                    /* config disp */
                    if (stsinkcapability.enNativeIntfSync != VO_OUTPUT_BUTT) {
                        stDispAttr.stPubAttr.enIntfSync = stsinkcapability.enNativeIntfSync;
                    } else {
                        for (i = 0; i < VO_OUTPUT_BUTT; i++) {
                            if (stsinkcapability.abIntfSync[i] == HI_TRUE) {
                                stDispAttr.stPubAttr.enIntfSync = (VO_INTF_SYNC_E)i;
                                break;
                            }
                        }

                        if (i == VO_OUTPUT_BUTT) {
                            stDispAttr.stPubAttr.enIntfSync = VO_OUTPUT_1080P30;
                        }
                    }

                    stDispAttr.stPubAttr.u32BgColor = 0xFF;
                    stDispAttr.stPubAttr.enIntfType = VO_INTF_HDMI;
                    SAMPLE_CHECK_GOTO(memset_s(&stDispAttr.stPubAttr.stSyncInfo, sizeof(VO_SYNC_INFO_S), 0,
                        sizeof(VO_SYNC_INFO_S)));

                    SAMPLE_GetWindowSize(&stDispAttr.stPubAttr, &u32WinWidth, &u32WinHeight);
                    stWndAttr.stRect.s32X = 0;
                    stWndAttr.stRect.s32Y = 0;
                    stWndAttr.stRect.u32Width = u32WinWidth;
                    stWndAttr.stRect.u32Height = u32WinHeight;
                    stWndAttr.u32Priority = 0;
                    SAMPLE_CHECK_GOTO(memset_s(&stVideoLayerAttr, sizeof(HI_MAPI_DISP_VIDEOLAYER_ATTR_S), 0,
                        sizeof(HI_MAPI_DISP_VIDEOLAYER_ATTR_S)));
                    stVideoLayerAttr.u32BufLen = 4;

                    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Init(hDispHdl0, &stDispAttr));
                    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Start(hDispHdl0, &stVideoLayerAttr));
                    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_SetWindowAttr(hDispHdl0, hWndHdl0, &stWndAttr));
                    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_StartWindow(hDispHdl0, hWndHdl0));
                    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Bind_VProc(hVpssHdl0, hVPortHdl0, hDispHdl0, hWndHdl0, bStitch));
                    g_bDispInited = HI_TRUE;

                    /* configer audio */
                    if (stsinkcapability.bSupportHdmi && (stsinkcapability.aenSupportSampleRate[0] !=
                        AUDIO_SAMPLE_RATE_BUTT)) {
                        /* If you have initialized audio you need to deinitialize */
                        if (g_bAoStartFlg) {
                            g_bAoStartFlg = HI_FALSE;
                            pthread_join(g_sendFrmAOChnThd, HI_NULL);
                            SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Stop(hAcapHdl, hAcapChnHdl));
                            SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Deinit(hAcapHdl));
                            SAMPLE_CHECK_GOTO(HI_MAPI_AO_Stop(hAoHdl, hAoChnHdl));
                            SAMPLE_CHECK_GOTO(HI_MAPI_AO_Deinit(hAoHdl));
                        }

                        /* start acap */
                        enHdmiSamRate = stsinkcapability.aenSupportSampleRate[0];

                        for (i = 0; i < HI_MAPI_AO_MAX_SUPPORT_SAMPLERATE_NUM; i++) { //  prior choose 48000
                            if (AUDIO_SAMPLE_RATE_48000 == stsinkcapability.aenSupportSampleRate[i]) {
                                enHdmiSamRate = stsinkcapability.aenSupportSampleRate[i];
                                break;
                            }
                        }

                        SAMPLE_AUDIO_ConfigACapAttr(&stACapAttr);
                        stACapAttr.enSampleRate = enHdmiSamRate;
                        s32InGain = SAMPLE_MAPI_AI_IN_GAIN;

                        SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Init(hAcapHdl, &stACapAttr));
                        SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Start(hAcapHdl, hAcapChnHdl));
                        SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_SetVolume(hAcapHdl, s32InGain));

                        /* start ao */
                        SAMPLE_AUDIO_ConfigAOAttr(&stAoAttr);
                        stAoAttr.enSampleRate = enHdmiSamRate;
                        stAoAttr.enInSampleRate = AUDIO_SAMPLE_RATE_48000;
                        stAoAttr.enI2sType = AIO_I2STYPE_INNERHDMI;
                        s32OutGain = SAMPLE_MAPI_AO_OUT_GAIN;

                        SAMPLE_CHECK_GOTO(HI_MAPI_AO_Init(hAoHdl, &stAoAttr));
                        SAMPLE_CHECK_GOTO(HI_MAPI_AO_Start(hAoHdl, hAoChnHdl));
                        SAMPLE_CHECK_GOTO(HI_MAPI_AO_SetVolume(hAoHdl, s32OutGain));

                        /* acap send frame to ao */
                        g_bAoStartFlg = HI_TRUE;
                        s32Ret = pthread_create(&g_sendFrmAOChnThd, HI_NULL, (HI_VOID *)SendFrameToAOChnThread,
                            HI_NULL);
                        if (s32Ret != HI_SUCCESS) {
                            g_bAoStartFlg = HI_FALSE;
                            SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Stop(hAcapHdl, hAcapChnHdl));
                            SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Deinit(hAcapHdl));
                            SAMPLE_CHECK_GOTO(HI_MAPI_AO_Stop(hAoHdl, hAoChnHdl));
                            SAMPLE_CHECK_GOTO(HI_MAPI_AO_Deinit(hAoHdl));
                        }
                    }

                    /* config hdmi */
                    stHdmiAttr.bEnableHdmi = stsinkcapability.bSupportHdmi;

                    if (g_bAoStartFlg) {
                        stHdmiAttr.bEnableAudio = HI_TRUE;
                    } else {
                        stHdmiAttr.bEnableAudio = HI_FALSE;
                    }

                    stHdmiAttr.bEnableVideo = HI_TRUE;
                    stHdmiAttr.enHdmiSync = stDispAttr.stPubAttr.enIntfSync;
                    stHdmiAttr.enSampleRate = enHdmiSamRate;
                    stHdmiAttr.u32PixClk = 0;
                    g_bHdmiStarted = HI_TRUE;
                    g_hdmiEvent = 0;

                    break;

                case NOPLUGEVENT:

                    /* deinit ao */
                    if (g_bAoStartFlg) {
                        g_bAoStartFlg = HI_FALSE;
                        pthread_join(g_sendFrmAOChnThd, HI_NULL);
                        SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Stop(hAcapHdl, hAcapChnHdl));
                        SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Deinit(hAcapHdl));
                        SAMPLE_CHECK_GOTO(HI_MAPI_AO_Stop(hAoHdl, hAoChnHdl));
                        SAMPLE_CHECK_GOTO(HI_MAPI_AO_Deinit(hAoHdl));
                    }


                    /* stop DISP */
                    if (g_bDispInited) {
                        SAMPLE_CHECK_GOTO(HI_MAPI_DISP_UnBind_VProc(hVpssHdl0, hVPortHdl0, hDispHdl0, hWndHdl0,
                            bStitch));
                        SAMPLE_CHECK_GOTO(HI_MAPI_DISP_StopWindow(hDispHdl0, hWndHdl0));
                        SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Stop(hDispHdl0));
                        SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Deinit(hDispHdl0));
                        g_bDispInited = HI_FALSE;
                    }

                    /* config disp */
                    stDispAttr.stPubAttr.u32BgColor = 0xFF;
                    stDispAttr.stPubAttr.enIntfType = VO_INTF_BT1120;
                    stDispAttr.stPubAttr.enIntfSync = VO_OUTPUT_1080P30;
                    SAMPLE_CHECK_GOTO(memset_s(&stDispAttr.stPubAttr.stSyncInfo, sizeof(VO_SYNC_INFO_S), 0,
                        sizeof(VO_SYNC_INFO_S)));

                    stWndAttr.stRect.s32X = 0;
                    stWndAttr.stRect.s32Y = 0;
                    stWndAttr.stRect.u32Width = 1920;
                    stWndAttr.stRect.u32Height = 1080;
                    stWndAttr.u32Priority = 0;
                    HI_BOOL bStitch = HI_FALSE;
                    SAMPLE_CHECK_GOTO(memset_s(&stVideoLayerAttr, sizeof(HI_MAPI_DISP_VIDEOLAYER_ATTR_S), 0,
                        sizeof(HI_MAPI_DISP_VIDEOLAYER_ATTR_S)));
                    stVideoLayerAttr.u32BufLen = 4;

                    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Init(hDispHdl0, &stDispAttr));
                    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Start(hDispHdl0, &stVideoLayerAttr));
                    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_SetWindowAttr(hDispHdl0, hWndHdl0, &stWndAttr));
                    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_StartWindow(hDispHdl0, hWndHdl0));
                    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Bind_VProc(hVpssHdl0, hVPortHdl0, hDispHdl0, hWndHdl0, bStitch));
                    g_bDispInited = HI_TRUE;

                    g_hdmiEvent = 0;
                    break;

                default:
                    // wait HDMI plug event
                    usleep(SAMPLE_DISP_GET_HDMIEVENT_THREAD_USLEEP_TIME);
                    break;
            }

        }else {
            // wait HDMI event
            usleep(SAMPLE_DISP_GET_HDMIEVENT_THREAD_USLEEP_TIME);
        }
    }

exit:

    if (g_bAoStartFlg) {
        g_bAoStartFlg = HI_FALSE;
        pthread_join(g_sendFrmAOChnThd, HI_NULL);
        HI_MAPI_ACAP_Stop(hAcapHdl, hAcapChnHdl);
        HI_MAPI_ACAP_Deinit(hAcapHdl);
        HI_MAPI_AO_Stop(hAoHdl, hAoChnHdl);
        HI_MAPI_AO_Deinit(hAoHdl);
    }
#endif
    return;
}


HI_VOID SAMPLE_VIO_Usage()
{
    printf("Usage : \n");
    printf("\t 0) VIO HDMI Vieo preview.\n");
#ifdef SUPPORT_HDMI
    printf("\t 1) VIO HMDI Vieo+Audio.\n");
    printf("\t 2) VIO HMDI AutoAdapt.\n");
#endif
#ifdef SUPPORT_GYRO
    printf("\t 3) VIO HDMI preview in gyro mode.\n");
#endif
    printf("\t 4) VIO HDMI preview with ahd.\n");
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

#ifdef __DualSys__
    s32Ret = HI_MAPI_Sys_Init();

    if (s32Ret != HI_SUCCESS) {
        printf("HI_MAPI_Sys_Init fail s32Ret:%x\n", s32Ret);
        return HI_FAILURE;
    }

#endif

    SAMPLE_VIO_Usage();
    g_sampleId = (char)getchar();
    (void)getchar();

    switch (g_sampleId) {
        case '0':
            break;

#ifdef SUPPORT_HDMI
        case '1':
            s32Ret = SAMPLE_HDMI_AudioOutput();
            break;

        case '2':
            s32Ret = SAMPLE_HDMI_AutoAdapt();
            break;
#endif

#ifdef SUPPORT_GYRO
        case '3':
            s32Ret = SAMPLE__Gyro();
            break;
#endif
        case '4':
            break;
        default:
            printf("the index is invaild!\n");
            return HI_FAILURE;
    }

#ifdef __DualSys__
    s32Ret = HI_MAPI_Sys_Deinit();
#endif

    if (s32Ret == HI_SUCCESS) {
        printf("\n The program exited successfully!\n");
    } else {
        printf("\n The program exits abnormally!\n");
    }

    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
