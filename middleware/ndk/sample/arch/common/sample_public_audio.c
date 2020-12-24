/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_comm_audio.c
 * @brief   sample audio comm module
 * @author  HiMobileCam NDK develop team
 * @date  2019-4-16
 */

#include "sample_public_audio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

static HI_CHAR *g_pG711RawData = HI_NULL;
static FILE *g_pAEncAacFile = HI_NULL;

HI_VOID SAMPLE_AUDIO_ConfigACapAttr(HI_MAPI_ACAP_ATTR_S *pstACapAttr)
{
    pstACapAttr->enSampleRate = AUDIO_SAMPLE_RATE_48000;
    pstACapAttr->enBitwidth = AUDIO_BIT_WIDTH_16;
    pstACapAttr->enSoundMode = AUDIO_SOUND_MODE_STEREO;
    pstACapAttr->enWorkmode = AIO_MODE_I2S_MASTER;
    pstACapAttr->u32PtNumPerFrm = SAMPLE_MAPI_AUDIO_POINT_PER_FRAME;
    pstACapAttr->enTrackMode = AUDIO_TRACK_NORMAL;
    pstACapAttr->enMixerMicMode = ACODEC_MIXER_IN1;
    pstACapAttr->enResampleRate = AUDIO_SAMPLE_RATE_48000;
    pstACapAttr->enI2sType = AIO_I2STYPE_INNERCODEC;
    return;
}

HI_VOID SAMPLE_AUDIO_ConfigAEncAttr(HI_MAPI_AENC_ATTR_S *pstAEncAttr,
                                    HI_MAPI_AENC_ATTR_AAC_S *pstAacAencAttr)
{
    pstAacAencAttr->enAACType = AAC_TYPE_AACLC;
    pstAacAencAttr->enBitRate = AAC_BPS_48K;
    pstAacAencAttr->enBitWidth = AUDIO_BIT_WIDTH_16;
    pstAacAencAttr->enSmpRate = AUDIO_SAMPLE_RATE_48000;
    pstAacAencAttr->enSoundMode = AUDIO_SOUND_MODE_STEREO;
    pstAacAencAttr->enTransType = AAC_TRANS_TYPE_ADTS;
    pstAacAencAttr->s16BandWidth = 0;
    pstAEncAttr->enAencFormat = HI_MAPI_AUDIO_FORMAT_AACLC;
    pstAEncAttr->u32PtNumPerFrm = SAMPLE_MAPI_AUDIO_POINT_PER_FRAME;
    pstAEncAttr->pValue = pstAacAencAttr;
    pstAEncAttr->u32Len = sizeof(HI_MAPI_AENC_ATTR_AAC_S);
    return;
}

HI_VOID SAMPLE_AUDIO_ConfigAOAttr(HI_MAPI_AO_ATTR_S *pstAOAttr)
{
    pstAOAttr->enWorkmode = AIO_MODE_I2S_MASTER;
    pstAOAttr->enBitwidth = AUDIO_BIT_WIDTH_16;
    pstAOAttr->enSampleRate = AUDIO_SAMPLE_RATE_16000;
    pstAOAttr->enSoundMode = AUDIO_SOUND_MODE_STEREO;
    pstAOAttr->u32PtNumPerFrm = SAMPLE_MAPI_AUDIO_POINT_PER_FRAME;
    pstAOAttr->enTrackMode = AUDIO_TRACK_NORMAL;
    pstAOAttr->enInSampleRate = AUDIO_SAMPLE_RATE_16000;
    pstAOAttr->enI2sType = AIO_I2STYPE_INNERCODEC;
    return;
}

static char *SAMPLE_AUDIO_Pt2Str(HI_MAPI_AUDIO_FORMAT_E enAencFormat)
{
    if (enAencFormat == HI_MAPI_AUDIO_FORMAT_G711A) {
        return "g711a";
    } else if (enAencFormat == HI_MAPI_AUDIO_FORMAT_G711U) {
        return "g711u";
    } else if (enAencFormat == HI_MAPI_AUDIO_FORMAT_AACLC) {
        return "aac";
    } else {
        return "data";
    }
}

static HI_S32 SAMPLE_AUDIO_GetRawStreamWithG711(HI_S16 *ps16HisiVoiceData, HI_S16 *ps16OutData,
                                                HI_S32 s32HisiSampleLen)
{
    HI_S32 s32Len = 0;
    HI_S32 s32OutLen = 0;
    HI_S16 *ps16copyHisiData, *ps16CopyOutData;
    HI_S32 s32CopySampleLen;
    HI_S32 s32Ret;

    // G711 data size align at HI_S16 , GetAudioFrame size align at HI_CHAR
    s32CopySampleLen = s32HisiSampleLen / (sizeof(HI_S16) / sizeof(HI_CHAR));
    if (s32CopySampleLen % (sizeof(HI_S16) / sizeof(HI_CHAR)) == 1) {
        s32CopySampleLen = s32CopySampleLen + 1;
    }
    ps16copyHisiData = ps16HisiVoiceData;
    ps16CopyOutData = ps16OutData;

    // hisi head data size is 2 * sizeof(HI_S16)
    while (s32CopySampleLen > 2) {
        // get cleanly data with a frame
        s32Len = ps16copyHisiData[1] & 0x00ff;

        // clean hisi head data whit a frame
        s32CopySampleLen -= 2;
        ps16copyHisiData += 2;
        if (s32CopySampleLen < s32Len) {
            break;
        }
        SAMPLE_CHECK_RET(memcpy_s(ps16CopyOutData, s32CopySampleLen * sizeof(HI_S16), ps16copyHisiData, s32Len * sizeof(HI_S16)));
        ps16CopyOutData += s32Len;
        ps16copyHisiData += s32Len;
        s32CopySampleLen -= s32Len;

        // return cleanly data size align at HI_CHAR
        s32OutLen += s32Len * (sizeof(HI_S16) / sizeof(HI_CHAR));
    }
    return s32OutLen;
}

HI_S32 SAMPLE_AUDIO_DataProc(HI_HANDLE AencHdl, const AUDIO_STREAM_S *pAStreamData, HI_VOID *pPrivateData)
{
    HI_CHAR au8TempValueStr[FILE_FULL_PATH_MAX_LEN] = { 0 };
    SAMPLE_AUDIO_ATTR_S *pstAudioAttr = NULL;
    HI_S32 s32Ret;

    if (pAStreamData == HI_NULL) {
        return HI_FAILURE;
    }

    if (pPrivateData != NULL) {
        pstAudioAttr = (SAMPLE_AUDIO_ATTR_S *)pPrivateData;
        HI_CHAR *pDataPostfix = SAMPLE_AUDIO_Pt2Str(pstAudioAttr->enAencFormat);

        s32Ret = snprintf_s(au8TempValueStr, FILE_FULL_PATH_MAX_LEN, FILE_FULL_PATH_MAX_LEN, "%s_chn%d.%s",
            pstAudioAttr->aFilePath, AencHdl, pDataPostfix);
        if (s32Ret == HI_FAILURE){
            printf("[%s][%d] snprintf_s error:%#x\n", __FUNCTION__, __LINE__, s32Ret);
            return HI_FAILURE;
        }

    } else {
        s32Ret = snprintf_s(au8TempValueStr, FILE_FULL_PATH_MAX_LEN, FILE_FULL_PATH_MAX_LEN, "%s_chn%d.%s", "null",
            AencHdl, "data");
        if (s32Ret == HI_FAILURE){
            printf("[%s][%d] snprintf_s error:%#x\n", __FUNCTION__, __LINE__, s32Ret);
            return HI_FAILURE;
        }
    }

    if (g_pAEncAacFile == HI_NULL) {
        g_pAEncAacFile = fopen(au8TempValueStr, "a+");
        if (!g_pAEncAacFile) {
            g_pAEncAacFile = fopen(au8TempValueStr, "wb");
            printf("[%s][%d] fopen wb\n", __FUNCTION__, __LINE__);
        }
        if (!g_pAEncAacFile) {
            printf("open file failed!\n");
        }
    }

    if (g_pAEncAacFile != HI_NULL) {
        if (((pPrivateData != NULL) && (pstAudioAttr->bDeleteHisiHead == HI_TRUE)) &&
            ((pstAudioAttr->enAencFormat == HI_MAPI_AUDIO_FORMAT_G711A) ||
             (pstAudioAttr->enAencFormat == HI_MAPI_AUDIO_FORMAT_G711U))) {
            if (g_pG711RawData == NULL) {
                // obligate buf for data head
                g_pG711RawData = (HI_CHAR *)malloc(SAMPLE_AIO_MAX_POINT_PER_FRAME_G711 * 2);
            }

            HI_S32 s32DataLen = SAMPLE_AUDIO_GetRawStreamWithG711((HI_S16 *)pAStreamData->pStream,
                                                                  (HI_S16 *)g_pG711RawData, pAStreamData->u32Len);
            if (s32DataLen <= 0) {
                printf("error datalen %d\n", s32DataLen);
                return HI_FAILURE;
            }
            fwrite((const void *)g_pG711RawData, 1, s32DataLen, g_pAEncAacFile);
        } else {
            fwrite((const void *)pAStreamData->pStream, 1, pAStreamData->u32Len, g_pAEncAacFile);
        }

        if (fflush(g_pAEncAacFile)) {
            perror("fflush file");
        }

        if (fclose(g_pAEncAacFile)) {
            perror("close file");
        }
        g_pAEncAacFile = HI_NULL;
    }
    return HI_SUCCESS;
}

static HI_VOID SAMPLE_AUDIO_FreeG711DataBuf(HI_VOID)
{
    if (g_pG711RawData != NULL) {
        free(g_pG711RawData);
        g_pG711RawData = NULL;
    }

    return;
}

HI_S32 SAMPLE_AUDIO_DeleteHisiHeadWithG711(HI_HANDLE acapHdl, HI_CHAR *pAacFilePath, HI_BOOL bDeleteHisiHead)
{
    HI_S32 s32Ret;

    SAMPLE_AUDIO_ATTR_S stAudioAttr;

    /* init sys */
    HI_MAPI_MEDIA_ATTR_S stMediaAttr;
    SAMPLE_CHECK_RET(memset_s(&stMediaAttr, sizeof(HI_MAPI_MEDIA_ATTR_S), 0, sizeof(HI_MAPI_MEDIA_ATTR_S)));

    SAMPLE_CHECK_GOTO(HI_MAPI_Media_Init(&stMediaAttr));

    /* init and start acap */
    HI_HANDLE acapChnHdl = SAMPLE_MAPI_AI_CHN;
    HI_HANDLE aencHdl = SAMPLE_MAPI_AENC_CHN;
    HI_MAPI_ACAP_ATTR_S stACapAttr;
    SAMPLE_AUDIO_ConfigACapAttr(&stACapAttr);
    stACapAttr.u32PtNumPerFrm = SAMPLE_AIO_MAX_POINT_PER_FRAME_G711;
    stACapAttr.enSampleRate = AUDIO_SAMPLE_RATE_8000;
    stACapAttr.enResampleRate = AUDIO_SAMPLE_RATE_8000;
    SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Init(acapHdl, &stACapAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_Start(acapHdl, acapChnHdl));

    HI_S32 s32Gain = SAMPLE_MAPI_AI_IN_GAIN;
    SAMPLE_CHECK_GOTO(HI_MAPI_ACAP_SetVolume(acapHdl, s32Gain));
    printf("init and start acap complete\n");

    /* init and start aenc */
    HI_MAPI_AENC_ATTR_S stAencAttr;
   // HI_MAPI_AENC_ATTR_AAC_S stAacAencAttr;

    AENC_ATTR_G711_S stAencG711;
    stAencAttr.enAencFormat = HI_MAPI_AUDIO_FORMAT_G711A;
    stAencAttr.u32PtNumPerFrm = SAMPLE_AIO_MAX_POINT_PER_FRAME_G711;
    stAencAttr.pValue = &stAencG711;
    stAencAttr.u32Len = sizeof(AENC_ATTR_G711_S);

    SAMPLE_CHECK_GOTO(HI_MAPI_AENC_Init(aencHdl, &stAencAttr));

    HI_MAPI_AENC_CALLBACK_S stAEncCB = { HI_NULL, HI_NULL };
    SAMPLE_CHECK_GOTO(memset_s(&stAEncCB, sizeof(HI_MAPI_AENC_CALLBACK_S), 0, sizeof(HI_MAPI_AENC_CALLBACK_S)));
    stAEncCB.pfnDataCB = SAMPLE_AUDIO_DataProc;

    s32Ret = snprintf_s(stAudioAttr.aFilePath, FILE_FULL_PATH_MAX_LEN, FILE_FULL_PATH_MAX_LEN, pAacFilePath);
    if (s32Ret == HI_FAILURE){
        printf("[%s][%d] snprintf_s error:%#x\n", __FUNCTION__, __LINE__, s32Ret);
        return HI_FAILURE;
    }

    stAudioAttr.bDeleteHisiHead = bDeleteHisiHead;
    stAudioAttr.enAencFormat = HI_MAPI_AUDIO_FORMAT_G711A;
    stAEncCB.pPrivateData = &stAudioAttr;
    SAMPLE_CHECK_GOTO(HI_MAPI_AENC_RegisterCallback(aencHdl, &stAEncCB));
    SAMPLE_CHECK_GOTO(HI_MAPI_AENC_Start(aencHdl));
    printf("init and start aenc complete\n");

    /* aenc bind acap */
    SAMPLE_CHECK_GOTO(HI_MAPI_AENC_BindACap(acapHdl, acapChnHdl, aencHdl));
    printf("acap bind aenc complete\n");

    printf("Press Enter key to stop audio record...\n");
    HI_CHAR cmd[SAMPLE_CMD_MAX_LEN];
    (void)fgets(cmd, SAMPLE_CMD_MAX_LEN, stdin);
    printf("ACapDumpThread stop\n");

exit:

    HI_MAPI_AENC_UnregisterCallback(aencHdl, &stAEncCB);
    HI_MAPI_AENC_Stop(aencHdl);
    HI_MAPI_AENC_Deinit(aencHdl);
    HI_MAPI_ACAP_Stop(acapHdl, acapChnHdl);
    HI_MAPI_ACAP_Deinit(acapHdl);
    SAMPLE_AUDIO_FreeG711DataBuf();
    HI_MAPI_Media_Deinit();

    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
