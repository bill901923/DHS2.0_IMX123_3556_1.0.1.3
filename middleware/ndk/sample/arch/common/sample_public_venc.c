/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_public_venc.c
 * @brief   sample public venc module
 * @author  HiMobileCam NDK develop team
 * @date  2019-5-16
 */

#include "sample_public_venc.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

HI_CHAR *SAMPLE_VENC_Pt2Str(HI_MAPI_PAYLOAD_TYPE_E enPayloadType)
{
    if (enPayloadType == HI_MAPI_PAYLOAD_TYPE_H264) {
        return "h264";
    } else if (enPayloadType == HI_MAPI_PAYLOAD_TYPE_H265) {
        return "h265";
    } else if (enPayloadType == HI_MAPI_PAYLOAD_TYPE_JPEG) {
        return "jpg";
    } else if (enPayloadType == HI_MAPI_PAYLOAD_TYPE_MJPEG) {
        return "mjp";
    } else {
        return "data";
    }
}

HI_S32 SAMPLE_VENC_SaveJpegFile(HI_HANDLE VencHdl, HI_MAPI_VENC_DATA_S *pVStreamData,
    HI_VOID *pPrivateData)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR aFileName[FILE_FULL_PATH_MAX_LEN] = {0};
    FILE *pVencFile = NULL;
    static int u32count = 0;

    HI_CHAR *pDataPostfix = SAMPLE_VENC_Pt2Str(pVStreamData->astPack[0].stDataType.enPayloadType);

    if (pPrivateData != NULL) {
        s32Ret = snprintf_s(aFileName, FILE_FULL_PATH_MAX_LEN, FILE_FULL_PATH_MAX_LEN, "%s_chn%d_%d.%s",
            (char *)pPrivateData, VencHdl, u32count, pDataPostfix);
        if (s32Ret == HI_FAILURE){
            printf("[%s][%d] snprintf_s error:%#x\n", __FUNCTION__, __LINE__, s32Ret);
            return HI_FAILURE;
        }

    } else {
        s32Ret = snprintf_s(aFileName, FILE_FULL_PATH_MAX_LEN, FILE_FULL_PATH_MAX_LEN, "%s_chn%d_%d.%s", "null", VencHdl,
            u32count, pDataPostfix);
        if (s32Ret == HI_FAILURE){
            printf("[%s][%d] snprintf_s error:%#x\n", __FUNCTION__, __LINE__, s32Ret);
            return HI_FAILURE;
        }
    }

    pVencFile = fopen(aFileName, "w");

    if (pVencFile == NULL) {
        printf("fopen %s error.\n", aFileName);
        return -1;
    }

    HI_S32 i = 0;

    for (i = 0; i < pVStreamData->u32PackCount; i++) {
#if  defined (__DualSys__) || defined (__HuaweiLite__)
        HI_U64 u64PackPhyAddr = pVStreamData->astPack[i].au64PhyAddr[0];
        HI_U32 u32PackLen = pVStreamData->astPack[i].au32Len[0];
        HI_U32 u32PackOffset = pVStreamData->astPack[i].u32Offset;

        HI_U64 s_pDataVirtAddr = (HI_U64)(HI_UL)pVStreamData->astPack[i].apu8Addr[1];
        HI_U64 s_u64DataPhyAddr = pVStreamData->astPack[i].au64PhyAddr[1];
        HI_U32 s_u32DataLen = pVStreamData->astPack[i].au32Len[1];
        HI_VOID *pPackVirtAddr = pVStreamData->astPack[i].apu8Addr[0];

        if (u64PackPhyAddr + u32PackLen >= s_u64DataPhyAddr + s_u32DataLen) {
            /* physical address retrace in offset segment */
            if (u64PackPhyAddr + u32PackOffset >= s_u64DataPhyAddr + s_u32DataLen) {
                HI_VOID *pSrcVirtAddr = (HI_VOID *)(HI_UL)(s_pDataVirtAddr +
                    ((u64PackPhyAddr + u32PackOffset) - (s_u64DataPhyAddr + s_u32DataLen)));
                s32Ret = fwrite(pSrcVirtAddr, u32PackLen - u32PackOffset, 1, pVencFile);

                if (s32Ret < 0) {
                    printf("fwrite error %d\n", s32Ret);
                }
            } else {
                /* physical address retrace in data segment */
                HI_U32 u32Left = (s_u64DataPhyAddr + s_u32DataLen) - u64PackPhyAddr;
                s32Ret = fwrite((HI_VOID *)((HI_UL)pPackVirtAddr + u32PackOffset), u32Left - u32PackOffset, 1,
                    pVencFile);

                if (s32Ret < 0) {
                    printf("fwrite error %d\n", s32Ret);
                }

                s32Ret = fwrite((HI_VOID *)(HI_UL)s_pDataVirtAddr, u32PackLen - u32Left, 1, pVencFile);

                if (s32Ret < 0) {
                    printf("fwrite error %d\n", s32Ret);
                }
            }
        } else {
            /* physical address retrace does not happen */
            s32Ret = fwrite((HI_VOID *)((HI_UL)pPackVirtAddr + u32PackOffset), u32PackLen - u32PackOffset, 1,
                pVencFile);

            if (s32Ret < 0) {
                printf("fwrite error %d\n", s32Ret);
            }
        }

#else
        HI_U8* pPackVirtAddr = pVStreamData->astPack[i].apu8Addr[0];
        HI_U32 u32PackLen = pVStreamData->astPack[i].au32Len[0];
        HI_U32 u32PackOffset = pVStreamData->astPack[i].u32Offset;
        s32Ret = fwrite(pPackVirtAddr + u32PackOffset, u32PackLen - u32PackOffset, 1, pVencFile);
        if (s32Ret < 0)
        {
            printf("fwrite error %x\n", s32Ret);
        }
#endif

        if (fflush(pVencFile)) {
            perror("fflush file\n");
        }

    }

    if (fclose(pVencFile)) {
        printf("fclose error\n");
    }

    printf("save jpeg %d\n", u32count);
    u32count++;

    return HI_SUCCESS;
}

HI_S32 SAMPLE_VENC_SaveStreamFile(HI_HANDLE VencHdl, HI_MAPI_VENC_DATA_S *pVStreamData,
    HI_VOID *pPrivateData)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR aFileName[FILE_FULL_PATH_MAX_LEN] = {0};
    FILE *pVencFile = NULL;

    HI_CHAR *pDataPostfix = SAMPLE_VENC_Pt2Str(pVStreamData->astPack[0].stDataType.enPayloadType);

    if (pPrivateData != NULL) {
        s32Ret = snprintf_s(aFileName, FILE_FULL_PATH_MAX_LEN, FILE_FULL_PATH_MAX_LEN, "%s_chn%d.%s",
            (char *)pPrivateData, VencHdl, pDataPostfix);
        if (s32Ret == HI_FAILURE){
            printf("[%s][%d] snprintf_s error:%#x\n", __FUNCTION__, __LINE__, s32Ret);
            return HI_FAILURE;
        }

    } else {
        s32Ret = snprintf_s(aFileName, FILE_FULL_PATH_MAX_LEN, FILE_FULL_PATH_MAX_LEN, "%s_chn%d.%s", "null", VencHdl,
            pDataPostfix);
        if (s32Ret == HI_FAILURE){
            printf("[%s][%d] snprintf_s error:%#x\n", __FUNCTION__, __LINE__, s32Ret);
            return HI_FAILURE;
        }
    }

    if (pVStreamData->u32Seq == 0) {
        pVencFile = fopen(aFileName, "w");
    } else {
        pVencFile = fopen(aFileName, "a");
    }

    if (pVencFile == NULL) {
        printf("fopen %s error.\n", aFileName);
        return HI_FAILURE;
    }

    HI_S32 i = 0;

    for (i = 0; i < pVStreamData->u32PackCount; i++) {
#if  defined (__DualSys__) || defined (__HuaweiLite__)
        HI_U64 u64PackPhyAddr = pVStreamData->astPack[i].au64PhyAddr[0];
        HI_U32 u32PackLen = pVStreamData->astPack[i].au32Len[0];
        HI_U32 u32PackOffset = pVStreamData->astPack[i].u32Offset;

        HI_U64 s_pDataVirtAddr = (HI_U64)(HI_UL)pVStreamData->astPack[i].apu8Addr[1];
        HI_U64 s_u64DataPhyAddr = pVStreamData->astPack[i].au64PhyAddr[1];
        HI_U32 s_u32DataLen = pVStreamData->astPack[i].au32Len[1];
        HI_VOID *pPackVirtAddr = pVStreamData->astPack[i].apu8Addr[0];

        if (u64PackPhyAddr + u32PackLen >= s_u64DataPhyAddr + s_u32DataLen) {
            /* physical address retrace in offset segment */
            if (u64PackPhyAddr + u32PackOffset >= s_u64DataPhyAddr + s_u32DataLen) {
                HI_VOID *pSrcVirtAddr = (HI_VOID *)(HI_UL)(s_pDataVirtAddr +
                    ((u64PackPhyAddr + u32PackOffset) - (s_u64DataPhyAddr + s_u32DataLen)));
                s32Ret = fwrite(pSrcVirtAddr, u32PackLen - u32PackOffset, 1, pVencFile);

                if (s32Ret < 0) {
                    printf("fwrite error %d\n", s32Ret);
                }
            } else {
                /* physical address retrace in data segment */
                HI_U32 u32Left = (s_u64DataPhyAddr + s_u32DataLen) - u64PackPhyAddr;
                s32Ret = fwrite((HI_VOID *)((HI_UL)pPackVirtAddr + u32PackOffset), u32Left - u32PackOffset, 1,
                    pVencFile);

                if (s32Ret < 0) {
                    printf("fwrite error %d\n", s32Ret);
                }

                s32Ret = fwrite((HI_VOID *)(HI_UL)s_pDataVirtAddr, u32PackLen - u32Left, 1, pVencFile);

                if (s32Ret < 0) {
                    printf("fwrite error %d\n", s32Ret);
                }
            }
        } else {
            /* physical address retrace does not happen */
            s32Ret = fwrite((HI_VOID *)((HI_UL)pPackVirtAddr + u32PackOffset), u32PackLen - u32PackOffset, 1,
                pVencFile);

            if (s32Ret < 0) {
                printf("fwrite error %d\n", s32Ret);
            }
        }

#else
        HI_U8* pPackVirtAddr = pVStreamData->astPack[i].apu8Addr[0];
        HI_U32 u32PackLen = pVStreamData->astPack[i].au32Len[0];
        HI_U32 u32PackOffset = pVStreamData->astPack[i].u32Offset;
        s32Ret = fwrite(pPackVirtAddr + u32PackOffset, u32PackLen - u32PackOffset, 1, pVencFile);

        if (s32Ret < 0)
        {
            printf("fwrite error %x\n", s32Ret);
        }

#endif

        if (fflush(pVencFile)) {
            perror("fflush file\n");
        }
    }

    if (fclose(pVencFile)) {
        printf("fclose error\n");
    }

    return HI_SUCCESS;
}

HI_S32 SMAPLE_COMM_VENC_DataProc(HI_HANDLE VencHdl, HI_MAPI_VENC_DATA_S *pVStreamData,
    HI_VOID *pPrivateData)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if (pVStreamData->astPack[0].stDataType.enPayloadType == HI_MAPI_PAYLOAD_TYPE_JPEG) {
        s32Ret = SAMPLE_VENC_SaveJpegFile(VencHdl, pVStreamData, pPrivateData);
    } else {
        s32Ret = SAMPLE_VENC_SaveStreamFile(VencHdl, pVStreamData, pPrivateData);
    }

    if (s32Ret != HI_SUCCESS) {
        printf("VENC_SaveFile error s32Ret:%d\n", s32Ret);
    }

    return HI_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
