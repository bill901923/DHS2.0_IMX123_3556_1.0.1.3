/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_audio.c
 * @brief   sample audio module
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */

#include "sample_comm.h"
#include "sample_public_audio.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


static FILE *g_pACapRawFile[2] = {HI_NULL};

static HI_BOOL g_bACapDumpThdFlg = HI_FALSE;
static pthread_t g_acapDumpThd = HI_NULL;

static HI_BOOL g_bSendFrmToAOChnThdFlg = HI_FALSE;
static pthread_t g_sendFrmAOChnThd = HI_NULL;

static HI_HANDLE g_acapHdl = SAMPLE_MAPI_AI_INNER_DEV;
static HI_HANDLE g_acapChnHdl = SAMPLE_MAPI_AI_CHN;
static HI_HANDLE g_aencHdl = SAMPLE_MAPI_AENC_CHN;
static HI_HANDLE g_aoHdl = SAMPLE_MAPI_AO_INNER_DEV;
static HI_HANDLE g_aoChnHdl = SAMPLE_MAPI_AO_CHN;

HI_VOID SAMPLE_AUDIO_Usage(HI_VOID)
{
    printf("Usage : \n\t\033[33m[ao sample send frame from linux (big-little) only for demo.\n\t" \
        "pls send frame to ao on liteos for general use]\033[0m\n");
    printf("\t 0) ACap bind AEnc and dump raw.\n");
    printf("\t 1) ACap send frame to Ao normal chn\n");
    printf("\t 2) ACap bind AEnc, dalete Hisi head with g711\n");
    printf("\t q/Q) quit\n");
    return;
}


HI_VOID ACapDumpThread(HI_VOID *args)
{
    HI_S32 s32Ret;
    HI_HANDLE acapHdl = g_acapHdl;
    HI_HANDLE acapChnHdl = g_acapChnHdl;
    AUDIO_FRAME_S stFrm;
    AEC_FRAME_S stAecFrm;

    prctl(PR_SET_NAME, (unsigned long)"Hi_ACapDumpThread", 0, 0, 0);
    while (g_bACapDumpThdFlg) {
        s32Ret = HI_MAPI_ACAP_GetFrame(acapHdl, acapChnHdl, &stFrm, &stAecFrm);
        if (s32Ret != HI_SUCCESS) {
            printf("HI_MAPI_ACAP_GetFrame error:0x%x\n", s32Ret);
            break;
        }

        /* only dump file 0 for mono(left or right channel),  dump file 0 and 1 for stereo */
        fwrite(stFrm.u64VirAddr[0], 1, stFrm.u32Len, g_pACapRawFile[0]);
        fwrite(stFrm.u64VirAddr[1], 1, stFrm.u32Len, g_pACapRawFile[1]);
        s32Ret = HI_MAPI_ACAP_ReleaseFrame(acapHdl, acapChnHdl, &stFrm, &stAecFrm);
        if (s32Ret != HI_SUCCESS) {
            printf("HI_MAPI_ACAP_ReleaseFrame error:0x%x\n", s32Ret);
        }
    }
    return;
}

HI_VOID SendFrameToAOChnThread(HI_VOID *args)
{
    HI_S32 s32Ret;
    HI_HANDLE acapHdl = g_acapHdl;
    HI_HANDLE acapChnHdl = g_acapChnHdl;
    HI_HANDLE hAoHdl = g_aoHdl;
    HI_HANDLE hAoChnHdl = g_aoChnHdl;
    AUDIO_FRAME_S stFrm;
    AEC_FRAME_S stAecFrm;

    prctl(PR_SET_NAME, (unsigned long)"Hi_SendFrameToAOChnThread", 0, 0, 0);
    while (g_bSendFrmToAOChnThdFlg) {
        s32Ret = HI_MAPI_ACAP_GetFrame(acapHdl, acapChnHdl, &stFrm, &stAecFrm);
        if (s32Ret != HI_SUCCESS) {
            printf("HI_MAPI_ACAP_GetFrame error:0x%x\n", s32Ret);
            break;
        }

        s32Ret = HI_MAPI_AO_SendFrame(hAoHdl, hAoChnHdl, &stFrm, 500);
        if (s32Ret != HI_SUCCESS) {
            printf("HI_MAPI_AO_SendFrame error:0x%x\n", s32Ret);
        }

        s32Ret = HI_MAPI_ACAP_ReleaseFrame(acapHdl, acapChnHdl, &stFrm, &stAecFrm);
        if (s32Ret != HI_SUCCESS) {
            printf("HI_MAPI_ACAP_ReleaseFrame error:0x%x\n", s32Ret);
        }
    }
    return;
}


HI_S32 SAMPLE_AUDIO_ACapBindAEncWithResampleDump(HI_VOID)
{
    HI_S32 s32Ret;
    HI_CHAR aAacFilePath[FILE_FULL_PATH_MAX_LEN] = "./audio_stream";
    SAMPLE_AUDIO_ATTR_S stAudioAttr;

    /* init sys */
    HI_MAPI_MEDIA_ATTR_S stMediaAttr;
    SAMPLE_CHECK_RET(memset_s(&stMediaAttr, sizeof(HI_MAPI_MEDIA_ATTR_S), 0, sizeof(HI_MAPI_MEDIA_ATTR_S)));

    SAMPLE_CHECK_GOTO(HI_MAPI_Media_Init(&stMediaAttr));

    /* init and start acap */
    g_acapHdl = SAMPLE_MAPI_AI_INNER_DEV;
    g_acapChnHdl = SAMPLE_MAPI_AI_CHN;
    HI_MAPI_ACAP_ATTR_S stACapAttr;
    SAMPLE_AUDIO_ConfigACapAttr(&stACapAttr);
    stACapAttr.enResampleRate = AUDIO_SAMPLE_RATE_16000; /* resample */
    SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Init(g_acapHdl, &stACapAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Start(g_acapHdl, g_acapChnHdl));

    HI_S32 s32Gain = SAMPLE_MAPI_AI_IN_GAIN;
    SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_SetVolume(g_acapHdl, s32Gain));
    printf("init and start acap complete\n");

    /* init and start aenc */
    SAMPLE_CHECK_GOTO(HI_MAPI_AENC_RegisterAudioEncoder(HI_MAPI_AUDIO_FORMAT_AACLC));

    g_aencHdl = SAMPLE_MAPI_AENC_CHN;
    HI_MAPI_AENC_ATTR_S stAencAttr;
    HI_MAPI_AENC_ATTR_AAC_S stAacAencAttr;
    SAMPLE_AUDIO_ConfigAEncAttr(&stAencAttr, &stAacAencAttr);
    SAMPLE_CHECK_GOTO(HI_MAPI_AENC_Init(g_aencHdl, &stAencAttr));

    HI_MAPI_AENC_CALLBACK_S stAEncCB = { HI_NULL, HI_NULL };
    SAMPLE_CHECK_GOTO(memset_s(&stAEncCB, sizeof(HI_MAPI_AENC_CALLBACK_S), 0, sizeof(HI_MAPI_AENC_CALLBACK_S)));
    stAEncCB.pfnDataCB = SAMPLE_AUDIO_DataProc;

    s32Ret = snprintf_s(stAudioAttr.aFilePath, FILE_FULL_PATH_MAX_LEN, FILE_FULL_PATH_MAX_LEN, aAacFilePath);
    if (s32Ret == HI_FAILURE){
        printf("[%s][%d] snprintf_s error:%#x\n", __FUNCTION__, __LINE__, s32Ret);
        return HI_FAILURE;
    }

    stAudioAttr.enAencFormat = HI_MAPI_AUDIO_FORMAT_AACLC;
    stAEncCB.pPrivateData = &stAudioAttr;
    SAMPLE_CHECK_GOTO(HI_MAPI_AENC_RegisterCallback(g_aencHdl, &stAEncCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_AENC_Start(g_aencHdl));
    printf("init and start aenc complete\n");

    /* aenc bind acap */
    SAMPLE_CHECK_GOTO(HI_MAPI_AENC_BindACap(g_acapHdl, g_acapChnHdl, g_aencHdl));
    printf("acap bind aenc complete\n");

    /* acap dump raw */
    g_pACapRawFile[0] = fopen("ai_dump_ch0.raw", "wb");
    if (!g_pACapRawFile[0]) {
        printf("open file ai_dump_ch0.raw error!\n");
        goto exit;
    }

#if 1
    g_pACapRawFile[1] = fopen("ai_dump_ch1.raw", "wb");
    if (!g_pACapRawFile[1]) {
        printf("open file ai_dump_ch1.raw error!\n");
        goto exit;
    }
#endif

    g_bACapDumpThdFlg = HI_TRUE;
    s32Ret = pthread_create(&g_acapDumpThd, HI_NULL, (HI_VOID *)ACapDumpThread, HI_NULL);
    if (s32Ret != HI_SUCCESS) {
        printf("pthread_create error!...\n");
        g_bACapDumpThdFlg = HI_FALSE;
    }
    printf("acap dump raw ...\n");

    printf("Press Enter key to stop audio record...\n");
    (void)getchar();
    if (g_bACapDumpThdFlg) {
        g_bACapDumpThdFlg = HI_FALSE;
        pthread_join(g_acapDumpThd, HI_NULL);
    }
    printf("ACapDumpThread stop\n");

exit:

    if (g_pACapRawFile[0]) {
        fclose(g_pACapRawFile[0]);
        g_pACapRawFile[0] = HI_NULL;
    }

#if 1
    if (g_pACapRawFile[1]) {
        fclose(g_pACapRawFile[1]);
        g_pACapRawFile[1] = HI_NULL;
    }
#endif

    HI_MAPI_AENC_UnregisterCallback(g_aencHdl, &stAEncCB);
    HI_MAPI_AENC_Stop(g_aencHdl);
    HI_MAPI_AENC_Deinit(g_aencHdl);
    HI_MAPI_ACAP_Stop(g_acapHdl, g_acapChnHdl);
    HI_MAPI_ACAP_Deinit(g_acapHdl);
    HI_MAPI_Media_Deinit();

    return s32Ret;
}

HI_S32 SAMPLE_AUDIO_ACapSendFrameToAO(HI_VOID)
{
    HI_S32 s32Ret;

    /* init sys */
    HI_MAPI_MEDIA_ATTR_S stMediaAttr;
    SAMPLE_CHECK_RET(memset_s(&stMediaAttr, sizeof(HI_MAPI_MEDIA_ATTR_S), 0, sizeof(HI_MAPI_MEDIA_ATTR_S)));
    SAMPLE_CHECK_GOTO(HI_MAPI_Media_Init(&stMediaAttr));

    /* init and start acap */
    g_acapHdl = SAMPLE_MAPI_AI_INNER_DEV;
    g_acapChnHdl = SAMPLE_MAPI_AI_CHN;
    g_aoChnHdl = SAMPLE_MAPI_AO_CHN;
    HI_MAPI_ACAP_ATTR_S stACapAttr;
    SAMPLE_AUDIO_ConfigACapAttr(&stACapAttr);
    stACapAttr.enSampleRate = AUDIO_SAMPLE_RATE_16000;
    stACapAttr.enResampleRate = AUDIO_SAMPLE_RATE_16000;
    SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Init(g_acapHdl, &stACapAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Start(g_acapHdl, g_acapChnHdl));

    HI_S32 s32InGain = SAMPLE_MAPI_AI_IN_GAIN;
    SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_SetVolume(g_acapHdl, s32InGain));
    printf("init and start acap complete\n");

    /* init and start ao */
    g_aoHdl = SAMPLE_MAPI_AO_INNER_DEV;
    HI_MAPI_AO_ATTR_S stAoAttr;
    SAMPLE_AUDIO_ConfigAOAttr(&stAoAttr);
    stAoAttr.enI2sType = AIO_I2STYPE_INNERCODEC;

    SAMPLE_CHECK_GOTO(HI_MAPI_AO_Init(g_aoHdl, &stAoAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_AO_Start(g_aoHdl, g_aoChnHdl));

    HI_S32 s32OutGain = SAMPLE_MAPI_AO_OUT_GAIN;
    SAMPLE_CHECK_GOTO(HI_MAPI_AO_SetVolume(g_aoHdl, s32OutGain));
    printf("init and start ao complete\n");

    /* acap send frame to ao */
    g_bSendFrmToAOChnThdFlg = HI_TRUE;
    s32Ret = pthread_create(&g_sendFrmAOChnThd, HI_NULL, (HI_VOID *)SendFrameToAOChnThread, HI_NULL);
    if (s32Ret != HI_SUCCESS) {
        printf("pthread_create error!...\n");
        g_bSendFrmToAOChnThdFlg = HI_FALSE;
    }
    printf("Press Enter key to stop audio record...\n");
    (void)getchar();

    if (g_bSendFrmToAOChnThdFlg) {
        g_bSendFrmToAOChnThdFlg = HI_FALSE;
        pthread_join(g_sendFrmAOChnThd, HI_NULL);
    }

    printf("SendFrameThread stop\n");

exit:

    HI_MAPI_AO_Stop(g_aoHdl, g_aoChnHdl);
    HI_MAPI_AO_Deinit(g_aoHdl);
    HI_MAPI_ACAP_Stop(g_acapHdl, g_acapChnHdl);
    HI_MAPI_ACAP_Deinit(g_acapHdl);
    HI_MAPI_Media_Deinit();

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

    HI_MAPI_LOG_SetEnabledLevel(HI_MAPI_LOG_LEVEL_WARN);

#ifdef __DualSys__
    s32Ret = HI_MAPI_Sys_Init();
    if (s32Ret != HI_SUCCESS) {
        printf("HI_MAPI_Sys_Init fail s32Ret:%x\n", s32Ret);
        return HI_FAILURE;
    }
#endif

    SAMPLE_AUDIO_Usage();
    ch = (char)getchar();
    (void)getchar();
    switch (ch) {
        case '0':
            s32Ret = SAMPLE_AUDIO_ACapBindAEncWithResampleDump();
            break;
        case '1':
            s32Ret = SAMPLE_AUDIO_ACapSendFrameToAO();
            break;
        case '2':
            s32Ret = SAMPLE_AUDIO_DeleteHisiHeadWithG711(SAMPLE_MAPI_AI_INNER_DEV, "./audio_stream", HI_TRUE);
            break;
        default:
            printf("the index is invaild!\n");
            SAMPLE_AUDIO_Usage();
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
