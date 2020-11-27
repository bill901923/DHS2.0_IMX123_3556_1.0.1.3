#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "math.h"
#include "sample_log.h"
#include "sample_memory_ops.h"
#include "sample_save_blob.h"
#include "sample_resize_roi.h"
#include "sample_data_utils.h"

#ifdef _WIN32
#include <Windows.h>
#define BILLION                             (1E9)

static BOOL g_first_time = 1;
static LARGE_INTEGER g_counts_per_sec;

int clock_gettime(int type, struct timespec* ct)
{
    LARGE_INTEGER count;

    if (g_first_time)
    {
        g_first_time = 0;

        if (0 == QueryPerformanceFrequency(&g_counts_per_sec))
        {
            g_counts_per_sec.QuadPart = 0;
        }
    }

    if ((NULL == ct) || (g_counts_per_sec.QuadPart <= 0) ||
        (0 == QueryPerformanceCounter(&count)))
    {
        return -1;
    }

    ct->tv_sec = count.QuadPart / g_counts_per_sec.QuadPart;
    ct->tv_nsec = (long)(((count.QuadPart % g_counts_per_sec.QuadPart) * BILLION) / g_counts_per_sec.QuadPart);

    return 0;
}
#endif

HI_VOID timeSpendMs(struct timespec* ptime1, struct timespec* ptime2, const char* des)
{
    long spend;
    spend = (((long)ptime2->tv_sec - (long)ptime1->tv_sec) * 1000 + (ptime2->tv_nsec - ptime1->tv_nsec) / 1000000);
    printf("[%s]===== TIME SPEND: %ld ms =====\n", des, spend);
}

HI_VOID timePrint(struct timespec* ptime, const char* des)
{
    printf("\n[%s]===== TIME NOW: %ld s, %ld us ===== ", des, (long)(ptime->tv_sec), (ptime->tv_nsec / 1000));
}

HI_VOID SAMPLE_DATA_GetStride(HI_RUNTIME_BLOB_TYPE_E type, HI_U32 width, HI_U32 align, HI_U32* pStride)
{
    HI_U32 u32Size = 0;

    if (HI_RUNTIME_BLOB_TYPE_U8 == type
            || HI_RUNTIME_BLOB_TYPE_YVU420SP == type
            || HI_RUNTIME_BLOB_TYPE_YVU422SP == type)
    {
        u32Size = sizeof(HI_U8);
    }
    else
    {
        u32Size = sizeof(HI_U32);
    }

#if 0

    if (HI_RUNTIME_BLOB_TYPE_SEQ_S32 == type)
    {
        if (ALIGN_16 == align)
        {
            *pStride = ALIGN16(u32Dim * u32Size);
        }
        else
        {
            *pStride = ALIGN32(u32Dim * u32Size);
        }

        total = step * *pStride;
    }
    else
#endif
    {
        if (ALIGN_16 == align)
        {
            *pStride = ALIGN16(width * u32Size);
        }
        else
        {
            *pStride = ALIGN32(width * u32Size);
        }
    }

    return;
}

HI_U32 SAMPLE_DATA_GetBlobSize(HI_U32 stride, HI_U32 num, HI_U32 height, HI_U32 chn)
{
    return num * stride * height * chn;
}

HI_VOID printDebugData(const HI_CHAR* pcName, HI_U64 u64VirAddr, HI_U32 u32PrintLine)
{
    HI_U8* pu8Tmp = HI_NULL;
    printf("============================== %s result print =============================\n", pcName);
    pu8Tmp = (HI_U8*)((uintptr_t)(u64VirAddr));

    for (HI_U32 i = 0; i < u32PrintLine; i++)
    {
        for (HI_U32 j = 0; j < 16; j++)
        {
            printf("%02x ", pu8Tmp[i * 16 + j]);
        }

        printf("\n");
    }

    printf("============================== %s result end =============================\n", pcName);
}

static HI_S32 Argswap(HI_S32* ps32Src1, HI_S32* ps32Src2)
{
    HI_U32 i = 0;
    HI_S32 tmp = 0;

    for (i = 0; i < SVP_WK_PROPOSAL_WIDTH; i++)
    {
        tmp = ps32Src1[i];
        ps32Src1[i] = ps32Src2[i];
        ps32Src2[i] = tmp;
    }

    return HI_SUCCESS;
}

HI_S32 NonRecursiveArgQuickSort(HI_S32* aResultArray,
                                       HI_S32 s32Low, HI_S32 s32High, NNIE_STACK_S* pstStack, HI_U32 u32MaxNum)
{
    HI_S32 i = s32Low;
    HI_S32 j = s32High;
    HI_S32 s32Top = 0;
    pstStack[s32Top].s32Min = s32Low;
    pstStack[s32Top].s32Max = s32High;

    HI_S32 s32KeyConfidence = aResultArray[SVP_WK_PROPOSAL_WIDTH * s32Low + 4];

    while (s32Top > -1)
    {
        s32Low = pstStack[s32Top].s32Min;
        s32High = pstStack[s32Top].s32Max;
        i = s32Low;
        j = s32High;
        s32Top--;

        s32KeyConfidence = aResultArray[SVP_WK_PROPOSAL_WIDTH * s32Low + 4];

        while (i < j)
        {
            while ((i < j) && (s32KeyConfidence > aResultArray[j * SVP_WK_PROPOSAL_WIDTH + 4]))
            {
                j--;
            }

            if (i < j)
            {
                Argswap(&aResultArray[i * SVP_WK_PROPOSAL_WIDTH], &aResultArray[j * SVP_WK_PROPOSAL_WIDTH]);
                i++;
            }

            while ((i < j) && (s32KeyConfidence < aResultArray[i * SVP_WK_PROPOSAL_WIDTH + 4]))
            {
                i++;
            }

            if (i < j)
            {
                Argswap(&aResultArray[i * SVP_WK_PROPOSAL_WIDTH], &aResultArray[j * SVP_WK_PROPOSAL_WIDTH]);
                j--;
            }
        }

        if (s32Low <= (HI_S32)u32MaxNum)
        {
            if (s32Low < i - 1)
            {
                s32Top++;
                pstStack[s32Top].s32Min = s32Low;
                pstStack[s32Top].s32Max = i - 1;
            }

            if (s32High > i + 1)
            {
                s32Top++;
                pstStack[s32Top].s32Min = i + 1;
                pstStack[s32Top].s32Max = s32High;
            }

            if (s32Top > MAX_STACK_DEPTH)
            {
                return HI_FAILURE;
            }
        }
    }

    return HI_SUCCESS;
}

static HI_S32 Overlap(HI_S32 s32XMin1, HI_S32 s32YMin1, HI_S32 s32XMax1, HI_S32 s32YMax1, HI_S32 s32XMin2,
                      HI_S32 s32YMin2, HI_S32 s32XMax2, HI_S32 s32YMax2, HI_S32* s32AreaSum, HI_S32* s32AreaInter)
{
    /*** Check the input, and change the Return value  ***/
    HI_S32 s32Inter = 0;
    HI_S32 s32Total = 0;
    HI_S32 s32XMin = 0;
    HI_S32 s32YMin = 0;
    HI_S32 s32XMax = 0;
    HI_S32 s32YMax = 0;
    HI_S32 s32Area1 = 0;
    HI_S32 s32Area2 = 0;
    HI_S32 s32InterWidth = 0;
    HI_S32 s32InterHeight = 0;

    s32XMin = max(s32XMin1, s32XMin2);
    s32YMin = max(s32YMin1, s32YMin2);
    s32XMax = min(s32XMax1, s32XMax2);
    s32YMax = min(s32YMax1, s32YMax2);

    s32InterWidth = s32XMax - s32XMin + 1;
    s32InterHeight = s32YMax - s32YMin + 1;

    s32InterWidth = (s32InterWidth >= 0) ? s32InterWidth : 0;
    s32InterHeight = (s32InterHeight >= 0) ? s32InterHeight : 0;

    s32Inter = s32InterWidth * s32InterHeight;
    s32Area1 = (s32XMax1 - s32XMin1 + 1) * (s32YMax1 - s32YMin1 + 1);
    s32Area2 = (s32XMax2 - s32XMin2 + 1) * (s32YMax2 - s32YMin2 + 1);

    s32Total = s32Area1 + s32Area2 - s32Inter;

    *s32AreaSum = s32Total;
    *s32AreaInter = s32Inter;

    return HI_SUCCESS;
}

HI_S32 NonMaxSuppression(HI_S32* pu32Proposals, HI_U32 u32NumAnchors, HI_U32 u32NmsThresh)
{
    /****** define variables *******/
    HI_S32 s32XMin1 = 0;
    HI_S32 s32YMin1 = 0;
    HI_S32 s32XMax1 = 0;
    HI_S32 s32YMax1 = 0;
    HI_S32 s32XMin2 = 0;
    HI_S32 s32YMin2 = 0;
    HI_S32 s32XMax2 = 0;
    HI_S32 s32YMax2 = 0;
    HI_S32 s32AreaTotal = 0;
    HI_S32 s32AreaInter = 0;

    for (HI_U32 i = 0; i < u32NumAnchors; i++)
    {
        if (RPN_SUPPRESS_FALSE == pu32Proposals[SVP_WK_PROPOSAL_WIDTH * i + 5])
        {
            s32XMin1 = pu32Proposals[SVP_WK_PROPOSAL_WIDTH * i];
            s32YMin1 = pu32Proposals[SVP_WK_PROPOSAL_WIDTH * i + 1];
            s32XMax1 = pu32Proposals[SVP_WK_PROPOSAL_WIDTH * i + 2];
            s32YMax1 = pu32Proposals[SVP_WK_PROPOSAL_WIDTH * i + 3];

            for (HI_U32 j = i + 1; j < u32NumAnchors; j++)
            {
                if (RPN_SUPPRESS_FALSE == pu32Proposals[SVP_WK_PROPOSAL_WIDTH * j + 5])
                {
                    s32XMin2 = pu32Proposals[SVP_WK_PROPOSAL_WIDTH * j];
                    s32YMin2 = pu32Proposals[SVP_WK_PROPOSAL_WIDTH * j + 1];
                    s32XMax2 = pu32Proposals[SVP_WK_PROPOSAL_WIDTH * j + 2];
                    s32YMax2 = pu32Proposals[SVP_WK_PROPOSAL_WIDTH * j + 3];

                    if((s32XMin2>s32XMax1) || (s32XMax2<s32XMin1) || (s32YMin2>s32YMax1) || (s32YMax2<s32YMin1))
                    {
                        continue;
                    }

                    Overlap(s32XMin1, s32YMin1, s32XMax1, s32YMax1, s32XMin2, s32YMin2, s32XMax2, s32YMax2, &s32AreaTotal, &s32AreaInter);

                    if (s32AreaInter * SVP_WK_QUANT_BASE > (HI_S32)u32NmsThresh * s32AreaTotal)
                    {
                        if (pu32Proposals[SVP_WK_PROPOSAL_WIDTH * i + 4] >= pu32Proposals[SVP_WK_PROPOSAL_WIDTH * j + 4])
                        {
                            pu32Proposals[SVP_WK_PROPOSAL_WIDTH * j + 5] = RPN_SUPPRESS_TRUE;
                        }

                        else
                        {
                            pu32Proposals[SVP_WK_PROPOSAL_WIDTH * i + 5] = RPN_SUPPRESS_TRUE;
                        }
                    }
                }
            }
        }
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_RUNTIME_HiMemAlloc(HI_RUNTIME_MEM_S* pstMem, HI_BOOL bCached)
{
    HI_S32 s32Ret = HI_SUCCESS;

    pstMem->u64VirAddr = 0;
    s32Ret = SAMPLE_AllocMem(pstMem, bCached);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "SAMPLE_Utils_AllocMem failed!\n");

    if (HI_FALSE == bCached)
    {
        return s32Ret;
    }

    s32Ret = SAMPLE_FlushCache(pstMem);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "SAMPLE_Utils_FlushCache failed!\n");

    return s32Ret;
FAIL_0:
    SAMPLE_FreeMem(pstMem);
    return HI_FAILURE;
}

HI_S32 SAMPLE_RUNTIME_HiBlobAlloc(HI_RUNTIME_BLOB_S* pstBlob, HI_U32 u32BlobSize, HI_BOOL bCached)
{
    HI_S32 s32Ret = HI_SUCCESS;

    HI_RUNTIME_MEM_S stMem;
    memset(&stMem, 0, sizeof(stMem));
    stMem.u32Size = u32BlobSize;
    s32Ret = SAMPLE_AllocMem(&stMem, bCached);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "SAMPLE_Utils_AllocMem failed!\n");
    pstBlob->u64PhyAddr = stMem.u64PhyAddr;
    pstBlob->u64VirAddr = stMem.u64VirAddr;

    if (HI_FALSE == bCached)
    {
        return s32Ret;
    }

    memset((HI_VOID*)((uintptr_t)pstBlob->u64VirAddr), 0, u32BlobSize);

    s32Ret = SAMPLE_FlushCache(&stMem);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "SAMPLE_Utils_FlushCache failed!\n");

    return s32Ret;
FAIL_0:
    (HI_VOID)SAMPLE_FreeMem(&stMem);
    return HI_FAILURE;
}

HI_S32 SAMPLE_RUNTIME_LoadModelFile(const HI_CHAR* pcModelFile, HI_RUNTIME_MEM_S* pstMemInfo)
{
    FILE* fp = HI_NULL;
    HI_S32  s32RuntimeWkLen = 0;
    HI_S32 s32Ret = HI_FAILURE;
    HI_CHAR acCanonicalPath[PATH_MAX+1] = {0};

#ifdef _WIN32
    SAMPLE_CHK_RET(strlen(pcModelFile) > PATH_MAX || HI_NULL == _fullpath(acCanonicalPath, pcModelFile, PATH_MAX), HI_FAILURE, "fullpath fail %s", pcModelFile);
#else
    SAMPLE_CHK_RET(strlen(pcModelFile) > PATH_MAX || HI_NULL == realpath(pcModelFile, acCanonicalPath), HI_FAILURE, "realpath fail %s", pcModelFile);
#endif
    fp = fopen(acCanonicalPath, "rb");
    SAMPLE_CHK_RET(NULL == fp, HI_FAILURE, "Open model file  %s failed!\n", pcModelFile);

    s32Ret = fseek(fp, 0L, SEEK_END);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, CLOSE_FILE, "SAMPLE_RUNTIME_MemAlloc failed!\n");

    s32RuntimeWkLen = ftell(fp);
    SAMPLE_CHK_GOTO(0 != s32RuntimeWkLen % 16, CLOSE_FILE, "Runtime WK Len %% 16 != 0 \n");

    SAMPLE_LOG_INFO("Runtime WK Len: %d\n", s32RuntimeWkLen);

    SAMPLE_CHK_GOTO(0 != fseek(fp, 0L, SEEK_SET), CLOSE_FILE, "fseek fail");

    pstMemInfo->u32Size = s32RuntimeWkLen;
    s32Ret = SAMPLE_RUNTIME_HiMemAlloc(pstMemInfo, HI_FALSE);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, CLOSE_FILE, "SAMPLE_RUNTIME_MemAlloc failed!\n");

    s32Ret = (HI_S32)fread((HI_VOID*)((uintptr_t)pstMemInfo->u64VirAddr), s32RuntimeWkLen, 1, fp);
    SAMPLE_CHK_GOTO(1 != s32Ret, FREE_MEM, "Read runtime WK failed!\n");

    fclose(fp);
    return HI_SUCCESS;
FREE_MEM:
    SAMPLE_FreeMem(pstMemInfo);
CLOSE_FILE:
    fclose(fp);
    return HI_FAILURE;
}

HI_S32 SAMPLE_RUNTIME_SetBlob(HI_RUNTIME_BLOB_S* pstBlob,
        HI_RUNTIME_BLOB_TYPE_E enType,
        HI_U32 u32Num,
        HI_U32 u32Width,
        HI_U32 u32Height,
        HI_U32 u32Chn,
        HI_U32 u32Align)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 u32BlobSize = 0;
    HI_U64 u64Size = 0;

    pstBlob->enBlobType = enType;
    pstBlob->u32Num = u32Num;
    if (enType == HI_RUNTIME_BLOB_TYPE_VEC_S32)
    {
        pstBlob->unShape.stWhc.u32Width = u32Chn;
        pstBlob->unShape.stWhc.u32Chn = u32Width;
    }
    else
    {
        pstBlob->unShape.stWhc.u32Width = u32Width;
        pstBlob->unShape.stWhc.u32Chn = u32Chn;
    }

    pstBlob->unShape.stWhc.u32Height = u32Height;
    SAMPLE_DATA_GetStride(enType, pstBlob->unShape.stWhc.u32Width, u32Align, &(pstBlob->u32Stride));
    u64Size = pstBlob->u32Num * pstBlob->u32Stride * pstBlob->unShape.stWhc.u32Height * pstBlob->unShape.stWhc.u32Chn;
    SAMPLE_CHK_GOTO((u64Size > (HI_U32)-1), FAIL_0, "the blobsize is too large [%llu]\n", u64Size);
    u32BlobSize = SAMPLE_DATA_GetBlobSize(pstBlob->u32Stride, u32Num, u32Height, pstBlob->unShape.stWhc.u32Chn);

    s32Ret = SAMPLE_RUNTIME_HiBlobAlloc(pstBlob, u32BlobSize, HI_TRUE);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "SAMPLE_Utils_AllocMem failed!\n");
FAIL_0:
    return s32Ret;
}

HI_S32 SAMPLE_RUNTIME_ReadSrcFile(const HI_CHAR* pcSrcFile, HI_RUNTIME_BLOB_S* pstSrcBlob, const HI_U32 u32Length)
{
    HI_U32 c, h;
    HI_U8* pu8Ptr = NULL;
    FILE* imgFp = NULL;
    HI_U32 s32Ret = HI_FAILURE;
    HI_U32 u32BlobSize = 0;
    HI_RUNTIME_MEM_S stMem;
    HI_CHAR acCanonicalPath[PATH_MAX+1] = {0};
    if(0 == u32Length)
    {
        SAMPLE_LOG_INFO("File name is empty!");
        return HI_FAILURE;
    }
#ifdef _WIN32
    SAMPLE_CHK_RET(strlen(pcSrcFile) > PATH_MAX || HI_NULL == _fullpath(acCanonicalPath, pcSrcFile, PATH_MAX), HI_FAILURE, "fullpath fail %s", pcSrcFile);
#else
    SAMPLE_CHK_RET(strlen(pcSrcFile) > PATH_MAX || HI_NULL == realpath(pcSrcFile, acCanonicalPath), HI_FAILURE, "realpath fail %s", pcSrcFile);
#endif
    pu8Ptr = (HI_U8*)((uintptr_t)(pstSrcBlob->u64VirAddr));
    imgFp = fopen(acCanonicalPath, "rb");
    SAMPLE_CHK_GOTO(NULL == imgFp, FAIL_0, "open img fp error[%s]\n", pcSrcFile);

    for (c = 0; c < pstSrcBlob->unShape.stWhc.u32Chn; c++)
    {
        for (h = 0; h < pstSrcBlob->unShape.stWhc.u32Height; h++)
        {
            s32Ret = (HI_S32)fread(pu8Ptr, pstSrcBlob->unShape.stWhc.u32Width * sizeof(HI_U8), 1, imgFp);
            SAMPLE_CHK_GOTO(1 != s32Ret, FAIL_0, "fread failed, (c,h)=(%d,%d)!", c, h);

            pu8Ptr += pstSrcBlob->u32Stride;
        }
    }

    fclose(imgFp);
    imgFp = HI_NULL;

    u32BlobSize = SAMPLE_DATA_GetBlobSize(pstSrcBlob->u32Stride, pstSrcBlob->u32Num, pstSrcBlob->unShape.stWhc.u32Height, pstSrcBlob->unShape.stWhc.u32Chn);
    stMem.u64PhyAddr = pstSrcBlob->u64PhyAddr;
    stMem.u64VirAddr = pstSrcBlob->u64VirAddr;
    stMem.u32Size = u32BlobSize;
    s32Ret = SAMPLE_FlushCache(&stMem);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "SAMPLE_Utils_FlushCache failed!\n");

    return HI_SUCCESS;
FAIL_0:

    if (HI_NULL != imgFp)
    { fclose(imgFp); }

    return HI_FAILURE;
}

HI_S32 SAMPLE_RUNTIME_ReadConfig(const HI_CHAR* pcConfigFile, HI_CHAR **acBuff)
{
    HI_CHAR acCanonicalPath[PATH_MAX+1] = {0};
    FILE *f = NULL;
    size_t readSize  = 0;
    HI_S64 s64FileSize = 0;
    HI_CHAR *tempBuff = NULL;
    HI_S32 s32Ret = HI_FAILURE;

#ifdef _WIN32
    SAMPLE_CHK_RET(strlen(pcConfigFile) > PATH_MAX || HI_NULL == _fullpath(acCanonicalPath, pcConfigFile, PATH_MAX), HI_FAILURE, "fullpath fail %s", pcConfigFile);
#else
    SAMPLE_CHK_RET(strlen(pcConfigFile) > PATH_MAX || HI_NULL == realpath(pcConfigFile, acCanonicalPath), HI_FAILURE, "realpath fail %s", pcConfigFile);
#endif
    f = fopen(acCanonicalPath, "r");
    SAMPLE_CHK_RET(NULL == f, HI_FAILURE, "config file %s not exists\n", acCanonicalPath);

    s32Ret = fseek(f, 0L, SEEK_END);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, CLOSE_FILE, "SAMPLE_RUNTIME_ReadConfig Failed");

    s64FileSize = ftell(f);
    SAMPLE_CHK_GOTO(-1 == s64FileSize, CLOSE_FILE, "SAMPLE_RUNTIME_ReadConfig Failed");

    tempBuff = (HI_CHAR*)malloc(s64FileSize + 1);
    SAMPLE_CHK_GOTO(NULL == tempBuff, CLOSE_FILE, "SAMPLE_RUNTIME_ReadConfig Failed");

    rewind(f);
    readSize = fread(tempBuff, 1, s64FileSize, f);
    if ((HI_S64)readSize != s64FileSize && !feof(f))
    {
        SAMPLE_FREE(tempBuff);
        *acBuff = NULL;
        fclose(f);
        return HI_FAILURE;
    }
    tempBuff[readSize] = '\0';
    *acBuff = tempBuff;
    fclose(f);
    return HI_SUCCESS;

CLOSE_FILE:
    fclose(f);
    return HI_FAILURE;
}

HI_DOUBLE compute_output_w(HI_DOUBLE x1, HI_DOUBLE x2)
{
    const HI_DOUBLE bbox_width = (x2 - x1);
    const HI_DOUBLE output_width = 2 * bbox_width;

    return max(1.0, output_width);
}

HI_DOUBLE compute_output_h(HI_DOUBLE y1, HI_DOUBLE y2)
{
    const HI_DOUBLE bbox_height = (y2 - y1);
    const HI_DOUBLE output_height = 2 * bbox_height;

    return max(1.0, output_height);
}

HI_VOID computeCropLocation(const BondingBox_s *pstTightBbox, HI_DOUBLE dWidth, HI_DOUBLE dHeight, BondingBox_s* pstLocationBbox)
{
    const HI_DOUBLE centerx = (pstTightBbox->x1 + pstTightBbox->x2) / 2;
    const HI_DOUBLE centery = (pstTightBbox->y1 + pstTightBbox->y2) / 2;

    const HI_DOUBLE width = dWidth;
    const HI_DOUBLE height = dHeight;

    const HI_DOUBLE output_width = compute_output_w(pstTightBbox->x1, pstTightBbox->x2);
    const HI_DOUBLE output_height = compute_output_h(pstTightBbox->y1, pstTightBbox->y2);

    const HI_DOUBLE roi_left = max(0.0, centerx - output_width / 2);
    const HI_DOUBLE roi_bottom = max(0.0, centery - output_height / 2);


    const HI_DOUBLE left_half = min(output_width / 2, centerx);

    const HI_DOUBLE right_half = min(output_width / 2, width - centerx);

    const HI_DOUBLE roi_width = max(1.0, left_half + right_half);

    const HI_DOUBLE top_half = min(output_height / 2, centery);

    const HI_DOUBLE bottom_half = min(output_height / 2, height - centery);

    const HI_DOUBLE roi_height = max(1.0, top_half + bottom_half);

    pstLocationBbox->x1 = roi_left;
    pstLocationBbox->y1 = roi_bottom;
    pstLocationBbox->x2 = roi_left + roi_width;
    pstLocationBbox->y2 = roi_bottom + roi_height;

}

HI_DOUBLE compute_edge_x(HI_DOUBLE x1, HI_DOUBLE x2)
{
    const HI_DOUBLE output_width = compute_output_w(x1, x2);
    const HI_DOUBLE centerx = (x1 + x2) / 2;

    return max(0.0, output_width / 2 - centerx);
}

HI_DOUBLE compute_edge_y(HI_DOUBLE y1, HI_DOUBLE y2)
{
    const HI_DOUBLE output_height = compute_output_h(y1, y2);
    const HI_DOUBLE centery = (y1 + y2) / 2;

    return max(0.0, output_height / 2 - centery);
}
