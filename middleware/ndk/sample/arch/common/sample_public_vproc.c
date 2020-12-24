/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_public_vproc.c
 * @brief   sample public vproc module
 * @author  HiMobileCam NDK develop team
 * @date  2019-5-16
 */

#include "sample_cfg.h"
#include "sample_public_vproc.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

HI_S32 SAMPLE_COMM_VPROC_SaveYuvFile(HI_HANDLE VProcHdl, HI_HANDLE VPortHdl, HI_MAPI_FRAME_DATA_S *pVPortYUV,
                               HI_VOID *pPrivateData)
{
    unsigned int w, h;
    char *pVBufVirtY;
    char *pVBufVirtC;
    char *pMemContent;
    HI_S32 s32Ret;

    // If this value is too small and the image is big, this memory may not be enough
    unsigned char aTmpBuff[SAMPLE_MAPI_VPROC_DUMP_BUF_LEN];
    HI_U8 *pUserPageAddr = NULL;
    PIXEL_FORMAT_E enPixelFormat = pVPortYUV->enPixelFormat;

    /* When the storage format is a planar format, this variable is used to keep the height of the UV component */
    HI_U32 u32UvHeight;
    HI_CHAR aFileName[FILE_FULL_PATH_MAX_LEN] = {0};
    FILE *pYuvFile = NULL;
    static HI_U64 u64OldPts = 0;

    if (pVPortYUV->enCompressMode != COMPRESS_MODE_NONE) {
        printf("Frame is compressed:%d\n", pVPortYUV->enCompressMode);
        return -1;
    }

    s32Ret = snprintf_s(aFileName, FILE_FULL_PATH_MAX_LEN, FILE_FULL_PATH_MAX_LEN, "%s%d_port%d_w%d_h%d_sp%d.yuv",
        (char *)pPrivateData, VProcHdl, VPortHdl, pVPortYUV->u32Width, pVPortYUV->u32Height,
        ((enPixelFormat - PIXEL_FORMAT_YVU_SEMIPLANAR_422) ? 420 : 422));
    if (s32Ret == HI_FAILURE){
        printf("[%s][%d] snprintf_s error:%#x\n", __FUNCTION__, __LINE__, s32Ret);
        return HI_FAILURE;
    }

    if ((pVPortYUV->u64pts - u64OldPts) / 20000000 > 0) {
        pYuvFile = fopen(aFileName, "w");
    } else {
        pYuvFile = fopen(aFileName, "a");
    }

    if (pYuvFile == NULL) {
        printf("fopen %s error.\n", aFileName);
        return -1;
    }

    if (PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat) {
        u32UvHeight = pVPortYUV->u32Height / 2;
    } else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat) {
        u32UvHeight = pVPortYUV->u32Height;
    } else {
        printf("enPixelFormat error.\n");
        fclose(pYuvFile);
        return -1;
    }

    pUserPageAddr = (HI_U8 *)(HI_UL)pVPortYUV->u64VirAddr[0];
    if (pUserPageAddr == HI_NULL) {
        printf("mmap error.\n");
        fclose(pYuvFile);
        return -1;
    }

    pVBufVirtY = (HI_CHAR *)pUserPageAddr;
    pVBufVirtC = pVBufVirtY + (pVPortYUV->u32Stride[0]) * (pVPortYUV->u32Height);

    /* save Y ---------------------------------------------------------------- */
    fprintf(stderr, "saving......Y......");
    for (h = 0; h < pVPortYUV->u32Height; h++) {
        pMemContent = pVBufVirtY + h * pVPortYUV->u32Stride[0];
        fwrite(pMemContent, pVPortYUV->u32Width, 1, pYuvFile);
    }
    fflush(pYuvFile);

    /* save U ---------------------------------------------------------------- */
    fprintf(stderr, "U......");
    for (h = 0; h < u32UvHeight; h++) {
        pMemContent = pVBufVirtC + h * pVPortYUV->u32Stride[1];

        pMemContent += 1;

        for (w = 0; w < pVPortYUV->u32Width / 2; w++) {
            aTmpBuff[w] = *pMemContent;
            pMemContent += 2;
        }
        fwrite(aTmpBuff, pVPortYUV->u32Width / 2, 1, pYuvFile);
    }
    fflush(pYuvFile);

    /* save V ---------------------------------------------------------------- */
    fprintf(stderr, "V......");
    for (h = 0; h < u32UvHeight; h++) {
        pMemContent = pVBufVirtC + h * pVPortYUV->u32Stride[1];

        for (w = 0; w < pVPortYUV->u32Width / 2; w++) {
            aTmpBuff[w] = *pMemContent;
            pMemContent += 2;
        }
        fwrite(aTmpBuff, pVPortYUV->u32Width / 2, 1, pYuvFile);
    }
    fflush(pYuvFile);

    if (fclose(pYuvFile)) {
        printf("fclose error\n");
    }

    fprintf(stderr, "done %llu!\n", pVPortYUV->u64pts);
    u64OldPts = pVPortYUV->u64pts;

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VPROC_ProcYuvData(HI_HANDLE VProcHdl, HI_HANDLE VPortHdl, HI_MAPI_FRAME_DATA_S *pVPortYUV,
    HI_VOID *pPrivateData)
{
    HI_S32 s32Ret;
    s32Ret = SAMPLE_COMM_VPROC_SaveYuvFile(VProcHdl, VPortHdl, pVPortYUV, pPrivateData);
    if (s32Ret != HI_SUCCESS) {
        printf("SAMPLE_COMM_VPROC_SaveYuvFile error s32Ret:%d\n", s32Ret);
    }

    return HI_SUCCESS;
}

static HI_S32 SAMPLE_SnapConvertBitPixel(HI_U8 *pu8Data, HI_U32 u32DataNum, PIXEL_FORMAT_E enPixelFormat,
                                         HI_U16 *pu16OutData)
{
    HI_S32 i, u32Tmp, s32OutCnt;
    HI_U32 u32Val;
    HI_U64 u64Val;
    HI_U8 *pu8Tmp = pu8Data;

    s32OutCnt = 0;

    switch (enPixelFormat) {
        case PIXEL_FORMAT_RGB_BAYER_10BPP: {
            /* 4 pixels consist of 5 bytes  */
            u32Tmp = u32DataNum / 4;

            for (i = 0; i < u32Tmp; i++) {
                /* byte4 byte3 byte2 byte1 byte0 */
                pu8Tmp = pu8Data + 5 * i;
                u64Val = pu8Tmp[0] + ((HI_U32)pu8Tmp[1] << 8) + ((HI_U32)pu8Tmp[2] << 16) +
                         ((HI_U32)pu8Tmp[3] << 24) + ((HI_U64)pu8Tmp[4] << 32);

                pu16OutData[s32OutCnt++] = u64Val & 0x3ff;
                pu16OutData[s32OutCnt++] = (u64Val >> 10) & 0x3ff;
                pu16OutData[s32OutCnt++] = (u64Val >> 20) & 0x3ff;
                pu16OutData[s32OutCnt++] = (u64Val >> 30) & 0x3ff;
            }
        }
        break;

        case PIXEL_FORMAT_RGB_BAYER_12BPP: {
            /* 2 pixels consist of 3 bytes  */
            u32Tmp = u32DataNum / 2;

            for (i = 0; i < u32Tmp; i++) {
                /* byte2 byte1 byte0 */
                pu8Tmp = pu8Data + 3 * i;
                u32Val = pu8Tmp[0] + (pu8Tmp[1] << 8) + (pu8Tmp[2] << 16);
                pu16OutData[s32OutCnt++] = u32Val & 0xfff;
                pu16OutData[s32OutCnt++] = (u32Val >> 12) & 0xfff;
            }
        }
        break;

        case PIXEL_FORMAT_RGB_BAYER_14BPP: {
            /* 4 pixels consist of 7 bytes  */
            u32Tmp = u32DataNum / 4;

            for (i = 0; i < u32Tmp; i++) {
                pu8Tmp = pu8Data + 7 * i;
                u64Val = pu8Tmp[0] + ((HI_U32)pu8Tmp[1] << 8) + ((HI_U32)pu8Tmp[2] << 16) +
                         ((HI_U32)pu8Tmp[3] << 24) + ((HI_U64)pu8Tmp[4] << 32) +
                         ((HI_U64)pu8Tmp[5] << 40) + ((HI_U64)pu8Tmp[6] << 48);

                pu16OutData[s32OutCnt++] = u64Val & 0x3fff;
                pu16OutData[s32OutCnt++] = (u64Val >> 14) & 0x3fff;
                pu16OutData[s32OutCnt++] = (u64Val >> 28) & 0x3fff;
                pu16OutData[s32OutCnt++] = (u64Val >> 42) & 0x3fff;
            }
        }
        break;

        default:
            fprintf(stderr, "unsuport enPixelFormat: %d\n", enPixelFormat);
            return -1;
            break;
    }

    return s32OutCnt;
}


static HI_S32 PixelFormatToNumber(PIXEL_FORMAT_E enPixelFormat)
{
    switch (enPixelFormat) {
        case PIXEL_FORMAT_RGB_BAYER_8BPP:
            return 8;

        case PIXEL_FORMAT_RGB_BAYER_10BPP:
            return 10;

        case PIXEL_FORMAT_RGB_BAYER_12BPP:
            return 12;

        case PIXEL_FORMAT_RGB_BAYER_14BPP:
            return 14;

        case PIXEL_FORMAT_RGB_BAYER_16BPP:
            return 16;

        default:
            return 0;
    }
}

static int RawDataSave(HI_MAPI_FRAME_DATA_S *pVBuf, FILE *pfd, HI_U32 u32FrameCnt)
{
    HI_U32 u32H = 0;
    HI_S32 i = 0;
    HI_U16 *apu16Data[HI_VCAP_RAW_EACHGROUP_MAX_NUM * HI_VCAP_RAWGROUP_MAX_NUM] = {
        NULL,
    };

    HI_U8 *apUserPageAddr[HI_VCAP_RAW_EACHGROUP_MAX_NUM * HI_VCAP_RAWGROUP_MAX_NUM] = {
        HI_NULL,
    };
    HI_U8 *pu8Data = HI_NULL;
    HI_S32 s32Ret = HI_SUCCESS;

    if (u32FrameCnt > HI_VCAP_RAW_EACHGROUP_MAX_NUM * HI_VCAP_RAWGROUP_MAX_NUM) {
        fprintf(stderr, "u32FrameCnt max is :%d\n", HI_VCAP_RAW_EACHGROUP_MAX_NUM * HI_VCAP_RAWGROUP_MAX_NUM);
        return HI_FAILURE;
    }

    for (i = 0; i < u32FrameCnt; i++) {
        apUserPageAddr[i] = (HI_U8 *)(HI_UL)(pVBuf + i)->u64VirAddr[0];

        if (apUserPageAddr[i] == NULL) {
            fprintf(stderr, "alloc memory failed\n");
            s32Ret = HI_FAILURE;
            goto exit;
        }

        pu8Data = apUserPageAddr[i];
        if ((PIXEL_FORMAT_RGB_BAYER_8BPP != (pVBuf + i)->enPixelFormat) &&
            (PIXEL_FORMAT_RGB_BAYER_16BPP != (pVBuf + i)->enPixelFormat)) {
            if ((pVBuf + i)->u32Width > 0) {
                SAMPLE_CHECK_U32PRODUCT_OVERFLOW_RET((pVBuf + i)->u32Width, 2);
                apu16Data[i] = (HI_U16 *)malloc((pVBuf + i)->u32Width * 2);
                if (apu16Data[i] == NULL) {
                    fprintf(stderr, "alloc memory failed\n");
                    s32Ret = HI_FAILURE;
                    goto exit;
                }
                SAMPLE_CHECK_RET(memset_s(apu16Data[i], (pVBuf + i)->u32Width * 2, 0, (pVBuf + i)->u32Width * 2));
            } else {
                printf("error u32Width :0!\n");
                s32Ret = HI_FAILURE;
                goto exit;
            }
        }

        fprintf(stderr, "saving(UNCMP)......dump data......u32Stride[0]: %d, width: %d\n", (pVBuf + i)->u32Stride[0],
                (pVBuf + i)->u32Width);

        for (u32H = 0; u32H < (pVBuf + i)->u32Height; u32H++) {
            if (PIXEL_FORMAT_RGB_BAYER_8BPP == (pVBuf + i)->enPixelFormat) {
                fwrite(pu8Data, (pVBuf + i)->u32Width, 1, pfd);
                fflush(pfd);
            } else if (PIXEL_FORMAT_RGB_BAYER_16BPP == (pVBuf + i)->enPixelFormat) {
                fwrite(pu8Data, (pVBuf + i)->u32Width, 2, pfd);
                fflush(pfd);
            } else {
                SAMPLE_SnapConvertBitPixel(pu8Data, (pVBuf + i)->u32Width, (pVBuf + i)->enPixelFormat, apu16Data[i]);
                fwrite(apu16Data[i], (pVBuf + i)->u32Width, 2, pfd);
            }
            pu8Data += (pVBuf + i)->u32Stride[0];
        }

        fflush(pfd);
        printf("\n************saving(UNCMP)...No%d...done!****************************\n", i);
    }
exit:
    for (i = 0; i < u32FrameCnt; i++) {
        if (apu16Data[i] != HI_NULL) {
            free(apu16Data[i]);
            apu16Data[i] = HI_NULL;
        }
    }

    return s32Ret;
}

static int SaveCompressParam(HI_HANDLE hVcapPipeHdl, FILE *pfd)
{
    VI_CMP_PARAM_S stCmpParam = {
        0,
    };
    HI_S32 s32Ret;

    s32Ret = HI_MAPI_VCAP_GetAttrEx(hVcapPipeHdl, 0, HI_MAPI_VCAP_CMD_PIPE_GetCmpParam, &stCmpParam,
                                    sizeof(VI_CMP_PARAM_S));

    fwrite(&stCmpParam, sizeof(VI_CMP_PARAM_S), 1, pfd);
    fflush(pfd);
    return s32Ret;
}

static int RawDataSaveCompress(HI_MAPI_FRAME_DATA_S *pVBuf, FILE *pfd, HI_U32 u32FrameCnt)
{
    HI_U32 u32H = 0;
    HI_S32 i = 0;
    HI_U8 *apUserPageAddr[HI_VCAP_RAW_EACHGROUP_MAX_NUM * HI_VCAP_RAWGROUP_MAX_NUM] = { HI_NULL };
    HI_U8 *pu8Data = HI_NULL;
    HI_U32 u32DataSize;
    HI_U16 u16HeadData = 0x0;

    if (u32FrameCnt > HI_VCAP_RAW_EACHGROUP_MAX_NUM * HI_VCAP_RAWGROUP_MAX_NUM) {
        fprintf(stderr, "u32FrameCnt max is :%d\n", HI_VCAP_RAW_EACHGROUP_MAX_NUM * HI_VCAP_RAWGROUP_MAX_NUM);
        return HI_FAILURE;
    }

    for (i = 0; i < u32FrameCnt; i++) {
        fprintf(stderr, "saving(CMP)......dump data......u32Stride[0]: %d, width: %d\n", (pVBuf + i)->u32Stride[0],
                (pVBuf + i)->u32Width);
        fflush(stderr);

        apUserPageAddr[i] = (HI_U8 *)(HI_UL)(pVBuf + i)->u64VirAddr[0];
        pu8Data = apUserPageAddr[i];

        for (u32H = 0; u32H < (pVBuf + i)->u32Height; u32H++) {
            u16HeadData = *(HI_U16 *)pu8Data;
            u32DataSize = (u16HeadData + 1) * 16;

            fwrite(pu8Data, u32DataSize, 1, pfd);
            pu8Data += (pVBuf + i)->u32Stride[0];
        }

        fflush(pfd);
        printf("\n************saving(CMP)...No%d...done!****************************\n", i);
        fflush(stderr);

        apUserPageAddr[i] = HI_NULL;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VPROC_ProcRawData(HI_HANDLE VcapPipeHdl, HI_MAPI_FRAME_DATA_S *pVCapRawData, HI_S32 s32DataNum,
                    HI_VOID *pPrivateData)
{
    FILE *pfd = NULL;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR szRawDataName[FILE_FULL_PATH_MAX_LEN] = {0};
    HI_S32 s32PixBit;

    s32PixBit = PixelFormatToNumber(pVCapRawData->enPixelFormat);

    printf("++nbit:%d\n", s32PixBit);
    printf("begin dump...\n");

    s32Ret = snprintf_s(szRawDataName, FILE_FULL_PATH_MAX_LEN, FILE_FULL_PATH_MAX_LEN, "./%s_vcap%d_%d_%d_%dbit_%d.raw",
        g_aszDumpFrameName[VcapPipeHdl], VcapPipeHdl, pVCapRawData->u32Width, pVCapRawData->u32Height, s32PixBit,
        g_as32RawFrameCnt[VcapPipeHdl]++);
    if (s32Ret == HI_FAILURE){
        printf("[%s][%d] snprintf_s error:%#x\n", __FUNCTION__, __LINE__, s32Ret);
        return HI_FAILURE;
    }

    pfd = fopen(szRawDataName, "wb");

    if (pfd != NULL) {
        if (pVCapRawData->enCompressMode == COMPRESS_MODE_NONE) {
            s32Ret = RawDataSave(pVCapRawData, pfd, s32DataNum);

            if (s32Ret != HI_SUCCESS) {
                fclose(pfd);
                printf("RawDataSave fail\n");
                return HI_FAILURE;
            }
        } else {
            s32Ret = SaveCompressParam(VcapPipeHdl, pfd);
            if (s32Ret != HI_SUCCESS) {
                fclose(pfd);
                printf("SaveCompressParam fail\n");
                return HI_FAILURE;
            }

            s32Ret = RawDataSaveCompress(pVCapRawData, pfd, s32DataNum);

            if (s32Ret != HI_SUCCESS) {
                fclose(pfd);
                printf("RawDataSaveCompress fail\n");
                return HI_FAILURE;
            }
        }

        fclose(pfd);
    } else {
        printf("open file fail\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VPROC_ProcBnrRawData(HI_HANDLE ProcHdl, HI_HANDLE PortHdl, HI_MAPI_FRAME_DATA_S *pBNRRaw,
    HI_VOID *pPrivateData)
{
    FILE *pfd = NULL;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR szRawDataName[FILE_FULL_PATH_MAX_LEN] = {0};

    printf("begin dump...\n");

    s32Ret = snprintf_s(szRawDataName, FILE_FULL_PATH_MAX_LEN, FILE_FULL_PATH_MAX_LEN, "./%s_proc%d_%d_%d.raw",
        (HI_S8 *)pPrivateData, ProcHdl, pBNRRaw->u32Width, pBNRRaw->u32Height);
    if (s32Ret == HI_FAILURE){
        printf("[%s][%d] snprintf_s error:%#x\n", __FUNCTION__, __LINE__, s32Ret);
        return HI_FAILURE;
    }

    pfd = fopen(szRawDataName, "wb");

    if (pfd > 0) {
        if (pBNRRaw->enCompressMode == COMPRESS_MODE_NONE) {
            s32Ret = RawDataSave(pBNRRaw, pfd, 1);

            if (s32Ret != HI_SUCCESS) {
                fclose(pfd);
                printf("RawDataSave fail\n");
                return HI_FAILURE;
            }
        } else {
            printf("not support compress\n");
        }
    } else {
        printf("open file fail\n");
    }

    if (fclose(pfd)) {
        printf("fclose error\n");
    }
    printf("fclose ok\n");
    return HI_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
