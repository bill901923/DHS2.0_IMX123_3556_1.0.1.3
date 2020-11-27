#include <stdint.h>
#include "sample_model_classification.h"
#include "sample_log.h"
#include "sample_memory_ops.h"
#include "sample_data_utils.h"


/*CNN Software parameter*/
typedef struct hiSAMPLE_RUNTIME_CNN_SOFTWARE_PARAM_S
{
    HI_U32 u32TopN;
    HI_RUNTIME_BLOB_S stGetTopN;
    HI_RUNTIME_MEM_S stAssistBuf;
} SAMPLE_RUNTIME_CNN_SOFTWARE_PARAM_S;

/*CNN GetTopN unit*/
typedef struct hiSAMPLE_RUNTIME_CNN_GETTOPN_UNIT_S
{
    HI_U32   u32ClassId;
    HI_U32   u32Confidence;
} SAMPLE_RUNTIME_CNN_GETTOPN_UNIT_S;

static SAMPLE_RUNTIME_CNN_SOFTWARE_PARAM_S s_stCnnSoftwareParam = { 0 };

static HI_S32 SAMPLE_RUNTIME_GetTopN(HI_S32* ps32Fc, HI_U32 u32FcStride,
                                     HI_U32 u32ClassNum, HI_U32 u32BatchNum, HI_U32 u32TopN, HI_S32* ps32TmpBuf,
                                     HI_U32 u32TopNStride, HI_S32* ps32GetTopN)
{
    HI_U32 i = 0, j = 0, n = 0;
    HI_U32 u32Id = 0;
    HI_S32* ps32Score = NULL;
    SAMPLE_RUNTIME_CNN_GETTOPN_UNIT_S stTmp = { 0 };
    SAMPLE_RUNTIME_CNN_GETTOPN_UNIT_S* pstTopN = NULL;
    SAMPLE_RUNTIME_CNN_GETTOPN_UNIT_S* pstTmpBuf = (SAMPLE_RUNTIME_CNN_GETTOPN_UNIT_S*)ps32TmpBuf;

    for (n = 0; n < u32BatchNum; n++)
    {
        ps32Score = (HI_S32*)((HI_U8*)ps32Fc + n * u32FcStride);
        pstTopN = (SAMPLE_RUNTIME_CNN_GETTOPN_UNIT_S*)((HI_U8*)ps32GetTopN + n * u32TopNStride);

        for (i = 0; i < u32ClassNum; i++)
        {
            pstTmpBuf[i].u32ClassId = i;
            pstTmpBuf[i].u32Confidence = (HI_U32)ps32Score[i];
        }

        for (i = 0; i < u32TopN; i++)
        {
            u32Id = i;
            pstTopN[i].u32ClassId = pstTmpBuf[i].u32ClassId;
            pstTopN[i].u32Confidence = pstTmpBuf[i].u32Confidence;

            for (j = i + 1; j < u32ClassNum; j++)
            {
                if (pstTmpBuf[u32Id].u32Confidence < pstTmpBuf[j].u32Confidence)
                {
                    u32Id = j;
                }
            }

            stTmp.u32ClassId = pstTmpBuf[u32Id].u32ClassId;
            stTmp.u32Confidence = pstTmpBuf[u32Id].u32Confidence;

            if (i != u32Id)
            {
                pstTmpBuf[u32Id].u32ClassId = pstTmpBuf[i].u32ClassId;
                pstTmpBuf[u32Id].u32Confidence = pstTmpBuf[i].u32Confidence;
                pstTmpBuf[i].u32ClassId = stTmp.u32ClassId;
                pstTmpBuf[i].u32Confidence = stTmp.u32Confidence;

                pstTopN[i].u32ClassId = stTmp.u32ClassId;
                pstTopN[i].u32Confidence = stTmp.u32Confidence;
            }
        }
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_RUNTIME_Cnn_PrintResult(HI_RUNTIME_BLOB_S* pstGetTopN, HI_U32 u32TopN)
{
    HI_U32 i = 0, j = 0;
    HI_U32* pu32Tmp = NULL;
    HI_U32 u32Stride = 0;
    float fProb = 0;
    SAMPLE_CHK_GOTO(NULL == pstGetTopN, FAIL_0, "Input error!\n");

    u32Stride = pstGetTopN->u32Stride;

    for (j = 0; j < pstGetTopN->u32Num; j++)
    {
        SAMPLE_LOG_INFO("==== The %dth image top %u output info====\n", j, u32TopN);
        pu32Tmp = (HI_U32*)((uintptr_t)(pstGetTopN->u64VirAddr + j * u32Stride));

        for (i = 0; i < u32TopN * 2; i += 2)
        {
            fProb = (float)pu32Tmp[i + 1] / 4096;
            SAMPLE_LOG_INFO("Index:%d, Probability: %f(%d)\n",
                                  pu32Tmp[i], fProb, pu32Tmp[i + 1]);
        }
        SAMPLE_LOG_INFO("==== The %dth image info end===\n", j);
    }

    return HI_SUCCESS;

FAIL_0:
    return HI_FAILURE;
}

HI_S32 SAMPLE_RUNTIME_Cnn_SoftwareParaInit(HI_RUNTIME_BLOB_S* pstDstBlob, SAMPLE_RUNTIME_CNN_SOFTWARE_PARAM_S* pstCnnSoftWarePara)
{
    HI_U32 u32GetTopNMemSize = 0;
    HI_U32 u32GetTopNAssistBufSize = 0;
    HI_U32 u32GetTopNPerFrameSize = 0;
    HI_U32 u32TotalSize = 0;
    HI_U32 u32ClassNum = pstDstBlob->unShape.stWhc.u32Width;
    HI_U8* pu8VirAddr = NULL;

    /*get mem size*/
    u32GetTopNPerFrameSize = pstCnnSoftWarePara->u32TopN * sizeof(SAMPLE_RUNTIME_CNN_GETTOPN_UNIT_S);
    u32GetTopNMemSize = ALIGN16(u32GetTopNPerFrameSize) * pstDstBlob->u32Num;
    u32GetTopNAssistBufSize = u32ClassNum * sizeof(SAMPLE_RUNTIME_CNN_GETTOPN_UNIT_S);
    u32TotalSize = u32GetTopNMemSize + u32GetTopNAssistBufSize;


    SAMPLE_CHK_GOTO(0 == u32TotalSize, FAIL_0, "u32TotalSize equals 0\n");
    pu8VirAddr = (HI_U8*)malloc(u32TotalSize);
    SAMPLE_CHK_GOTO(NULL == pu8VirAddr, FAIL_0, "Malloc memory failed, number = %u!\n", pstDstBlob->u32Num);
    memset(pu8VirAddr, 0, u32TotalSize);

    /*init GetTopn */
    pstCnnSoftWarePara->stGetTopN.u32Num = pstDstBlob->u32Num;
    pstCnnSoftWarePara->stGetTopN.unShape.stWhc.u32Chn = 1;
    pstCnnSoftWarePara->stGetTopN.unShape.stWhc.u32Height = 1;
    pstCnnSoftWarePara->stGetTopN.unShape.stWhc.u32Width = u32GetTopNPerFrameSize / sizeof(HI_U32);
    pstCnnSoftWarePara->stGetTopN.u32Stride = ALIGN16(u32GetTopNPerFrameSize);
    pstCnnSoftWarePara->stGetTopN.u64PhyAddr = 0;
    pstCnnSoftWarePara->stGetTopN.u64VirAddr = (HI_U64)((uintptr_t)pu8VirAddr);

    /*init AssistBuf */
    pstCnnSoftWarePara->stAssistBuf.u32Size = u32GetTopNAssistBufSize;
    pstCnnSoftWarePara->stAssistBuf.u64PhyAddr = 0;
    pstCnnSoftWarePara->stAssistBuf.u64VirAddr = (HI_U64)((uintptr_t)pu8VirAddr) + u32GetTopNMemSize;

    return HI_SUCCESS;

FAIL_0:
    return HI_FAILURE;
}

void SAMPLE_RUNTIME_Cnn_SoftwareParaDeInit(SAMPLE_RUNTIME_CNN_SOFTWARE_PARAM_S* pstCnnSoftWarePara)
{
    HI_U8* pu8VirAddr = (HI_U8*)((uintptr_t)pstCnnSoftWarePara->stGetTopN.u64VirAddr);

    if (NULL != pu8VirAddr)
    {
        SAMPLE_FREE(pu8VirAddr);
    }

    pstCnnSoftWarePara->stGetTopN.u64VirAddr = 0;
    pstCnnSoftWarePara->stAssistBuf.u64VirAddr = 0;

    return;
}

HI_S32 SAMPLE_RUNTIME_Cnn_GetTopN(HI_RUNTIME_BLOB_S* pstBlob,
                                  SAMPLE_RUNTIME_CNN_SOFTWARE_PARAM_S* pstCnnSoftwareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = SAMPLE_RUNTIME_GetTopN((HI_S32*)((uintptr_t)pstBlob->u64VirAddr),
                                    pstBlob->u32Stride,
                                    pstBlob->unShape.stWhc.u32Width,
                                    pstBlob->u32Num,
                                    pstCnnSoftwareParam->u32TopN,
                                    (HI_S32*)((uintptr_t)pstCnnSoftwareParam->stAssistBuf.u64VirAddr),
                                    pstCnnSoftwareParam->stGetTopN.u32Stride,
                                    (HI_S32*)((uintptr_t)pstCnnSoftwareParam->stGetTopN.u64VirAddr));
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "SAMPLE_RUNTIME_GetTopN failed!\n");

FAIL_0:
    return s32Ret;
}

HI_S32 SAMPLE_RUNTIME_Cnn_TopN_Output(HI_RUNTIME_BLOB_S* pstDst, HI_U32 u32TopN)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_RUNTIME_BLOB_S* pstDstBlob = NULL;

    pstDstBlob = (HI_RUNTIME_BLOB_S*) & (pstDst[0]);
    s_stCnnSoftwareParam.u32TopN = u32TopN;

    s32Ret = SAMPLE_RUNTIME_Cnn_SoftwareParaInit(pstDstBlob, &s_stCnnSoftwareParam);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "SAMPLE_RUNTIME_Cnn_SoftwareParaInit failed!\n");

    s32Ret = SAMPLE_RUNTIME_Cnn_GetTopN(pstDstBlob, &s_stCnnSoftwareParam);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "SAMPLE_RUNTIME_Cnn_GetTopN failed!\n");

    s32Ret = SAMPLE_RUNTIME_Cnn_PrintResult(&(s_stCnnSoftwareParam.stGetTopN),
                                            s_stCnnSoftwareParam.u32TopN);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "SAMPLE_RUNTIME_Cnn_PrintResult failed!\n");

FAIL_0:
    SAMPLE_RUNTIME_Cnn_SoftwareParaDeInit(&s_stCnnSoftwareParam);
    return s32Ret;
}
