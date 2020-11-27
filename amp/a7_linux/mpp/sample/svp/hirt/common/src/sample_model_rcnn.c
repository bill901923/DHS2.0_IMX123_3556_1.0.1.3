#include <stdint.h>
#include <math.h>
#include "sample_model_rcnn.h"
#include "sample_data_utils.h"
#include "sample_memory_ops.h"
#include "sample_log.h"

static HI_S32 SizeClip(HI_S32 s32inputSize, HI_S32 s32sizeMin, HI_S32 s32sizeMax)
{
    return max(min(s32inputSize, s32sizeMax), s32sizeMin);
}

static HI_S32 BboxClip(HI_S32* ps32Proposals, HI_U32 u32ImageW, HI_U32 u32ImageH)
{
    ps32Proposals[0] = SizeClip(ps32Proposals[0], 0, (HI_S32)u32ImageW - 1);
    ps32Proposals[1] = SizeClip(ps32Proposals[1], 0, (HI_S32)u32ImageH - 1);
    ps32Proposals[2] = SizeClip(ps32Proposals[2], 0, (HI_S32)u32ImageW - 1);
    ps32Proposals[3] = SizeClip(ps32Proposals[3], 0, (HI_S32)u32ImageH - 1);

    return HI_SUCCESS;
}

static HI_S32 BboxClip_N(HI_S32* ps32Proposals, HI_U32 u32ImageW, HI_U32 u32ImageH, HI_U32 u32Num)
{
    HI_S32 s32Ret = HI_FAILURE;

    for (HI_U32 i = 0; i < u32Num; i++)
    {
        s32Ret = BboxClip(&ps32Proposals[i * SVP_WK_PROPOSAL_WIDTH], u32ImageW, u32ImageH);
        SAMPLE_CHK_RET(HI_SUCCESS != s32Ret, HI_FAILURE, "BboxClip fail");
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_DATA_GetRoiResultFromOriginSize(SAMPLE_RUNTIME_MODEL_TYPE_E enType,
        HI_RUNTIME_BLOB_S* pstScoreBlob,
        HI_RUNTIME_BLOB_S* pstBBoxBlob,
        HI_RUNTIME_BLOB_S* pstProposalBlob,
        HI_U32 u32Width,
        HI_U32 u32Height,
        HI_S32* ps32ResultROI,
        HI_U32* pu32ResultROICnt)
{
    HI_U32 i, j;
    HI_U32 u32RoiCnt = pstProposalBlob->unShape.stWhc.u32Height;
    HI_U32 u32ScoreStride = pstScoreBlob->u32Stride;
    HI_U32 u32BBoxStride = ALIGN32(pstBBoxBlob->unShape.stWhc.u32Width * sizeof(HI_S32));
    HI_S32* ps32Score = (HI_S32*)((uintptr_t)pstScoreBlob->u64VirAddr);
    HI_U32 u32ClassNum = pstScoreBlob->unShape.stWhc.u32Width;
    HI_S32* ps32Proposal = (HI_S32*)((uintptr_t)pstProposalBlob->u64VirAddr);
    HI_S32* ps32BBox = (HI_S32*)((uintptr_t)pstBBoxBlob->u64VirAddr);
    HI_S32* ps32AllBoxes = HI_NULL;
    HI_S32* ps32AllBoxesTmp = HI_NULL;
    HI_FLOAT fProposalXMin, fProposalXMax, fProposalYMin, fProposalYMax, fProposalWidth, fProposalHeight, fProposalCenterX, fProposalCenterY;
    HI_FLOAT fBBoxCenterXDelta, fBBoxCenterYDelta, fBBoxWidthDelta, fBBoxHeightDelta;
    HI_FLOAT fPredWidth, fPredHeight, fPredCenterX, fPredCenterY;
    NNIE_STACK_S* pstStack = HI_NULL;
    HI_U32 u32ResultBoxNum = 0;

    SAMPLE_CHK_GOTO(0 == u32RoiCnt, FAIL_0, "u32RoiCnt equals 0\n");

    ps32AllBoxes = (HI_S32*)malloc(u32RoiCnt * SVP_WK_PROPOSAL_WIDTH * sizeof(HI_S32));
    SAMPLE_CHK_GOTO(HI_NULL == ps32AllBoxes, FAIL_0, "u32RoiCnt equals 0\n");

    pstStack = (NNIE_STACK_S*)malloc(sizeof(NNIE_STACK_S) * u32RoiCnt);
    SAMPLE_CHK_GOTO(HI_NULL == pstStack, FAIL_0, "u32RoiCnt equals 0\n");

    for (j = 0; j < u32ClassNum; j++)
    {
        for (i = 0; i < u32RoiCnt; i++)
        {
            fProposalXMin = (HI_FLOAT)(ps32Proposal[i * SVP_WK_COORDI_NUM] / 4096.0);
            fProposalYMin = (HI_FLOAT)(ps32Proposal[i * SVP_WK_COORDI_NUM + 1] / 4096.0);
            fProposalXMax = (HI_FLOAT)(ps32Proposal[i * SVP_WK_COORDI_NUM + 2] / 4096.0);
            fProposalYMax = (HI_FLOAT)(ps32Proposal[i * SVP_WK_COORDI_NUM + 3] / 4096.0);

            if (SAMPLE_RUNTIME_MODEL_TYPE_RFCN == enType)
            {
                fBBoxCenterXDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + SVP_WK_COORDI_NUM] / 4096.0);
                fBBoxCenterYDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + SVP_WK_COORDI_NUM + 1] / 4096.0);
                fBBoxWidthDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + SVP_WK_COORDI_NUM + 2] / 4096.0);
                fBBoxHeightDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + SVP_WK_COORDI_NUM + 3] / 4096.0);
            }
            else
            {
                fBBoxCenterXDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + j * SVP_WK_COORDI_NUM] / 4096.0);
                fBBoxCenterYDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + j * SVP_WK_COORDI_NUM + 1] / 4096.0);
                fBBoxWidthDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + j * SVP_WK_COORDI_NUM + 2] / 4096.0);
                fBBoxHeightDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + j * SVP_WK_COORDI_NUM + 3] / 4096.0);
            }

            fProposalWidth = fProposalXMax - fProposalXMin + 1;
            fProposalHeight = fProposalYMax - fProposalYMin + 1;
            fProposalCenterX = (HI_FLOAT)(fProposalXMin + fProposalWidth * 0.5);
            fProposalCenterY = (HI_FLOAT)(fProposalYMin + fProposalHeight * 0.5);
            fPredWidth = (HI_FLOAT)(fProposalWidth * exp(fBBoxWidthDelta));
            fPredHeight = (HI_FLOAT)(fProposalHeight * exp(fBBoxHeightDelta));
            fPredCenterX = fProposalCenterX + fProposalWidth * fBBoxCenterXDelta;
            fPredCenterY = fProposalCenterY + fProposalHeight * fBBoxCenterYDelta;
            ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH] = (HI_S32)(fPredCenterX - 0.5 * fPredWidth);
            ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 1] = (HI_S32)(fPredCenterY - 0.5 * fPredHeight);
            ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 2] = (HI_S32)(fPredCenterX + 0.5 * fPredWidth);
            ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 3] = (HI_S32)(fPredCenterY + 0.5 * fPredHeight);
            ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 4] = ps32Score[i * u32ScoreStride / sizeof(HI_S32) + j];
            ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 5] = RPN_SUPPRESS_FALSE; // RPN Suppress
        }

        BboxClip_N(ps32AllBoxes, u32Width, u32Height, u32RoiCnt);
        ps32AllBoxesTmp = ps32AllBoxes;
        NonRecursiveArgQuickSort(ps32AllBoxesTmp, 0, u32RoiCnt - 1, pstStack, u32RoiCnt);
        NonMaxSuppression(ps32AllBoxesTmp, u32RoiCnt, (HI_U32)(0.7 * SVP_WK_QUANT_BASE));
        u32ResultBoxNum = 0;

        for (i = 0; i < u32RoiCnt; i++)
        {
            if (RPN_SUPPRESS_FALSE == ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 5]
                && ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 4] > 0.8 * SVP_WK_QUANT_BASE)
            {
#if SAMPLE_DEBUG
                printf("%d %f %d %d %d %d\n", j, ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 4] / 4096.0,
                       ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 0], ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 1],
                       ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 2], ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 3]);
#endif
                u32ResultBoxNum++;

                if (0 != j) // not background
                {
                    memcpy(&ps32ResultROI[*pu32ResultROICnt * SVP_WK_PROPOSAL_WIDTH],
                           &ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH], sizeof(HI_S32)*SVP_WK_PROPOSAL_WIDTH);
                    *pu32ResultROICnt = *pu32ResultROICnt + 1;
                }
            }
        }

#if SAMPLE_DEBUG
        printf("class %d has %d boxes\n", j, u32ResultBoxNum);
#endif
    }

    SAMPLE_FREE(pstStack);
    SAMPLE_FREE(ps32AllBoxes);
    return HI_SUCCESS;
FAIL_0:
    SAMPLE_FREE(pstStack);
    SAMPLE_FREE(ps32AllBoxes);
    return HI_FAILURE;
}

HI_S32 SAMPLE_DATA_GetRoiResult(SAMPLE_RUNTIME_MODEL_TYPE_E enType,
        HI_RUNTIME_BLOB_S* pstScoreBlob,
        HI_RUNTIME_BLOB_S* pstBBoxBlob,
        HI_RUNTIME_BLOB_S* pstProposalBlob,
        HI_RUNTIME_BLOB_S* pstDataBlob,
        HI_S32* ps32ResultROI,
        HI_U32* pu32ResultROICnt)
{
    HI_U32 i, j;
    HI_U32 u32RoiCnt = pstProposalBlob->unShape.stWhc.u32Height;
    HI_U32 u32ScoreStride = pstScoreBlob->u32Stride;
    HI_U32 u32BBoxStride = ALIGN32(pstBBoxBlob->unShape.stWhc.u32Width * sizeof(HI_S32));
    HI_S32* ps32Score = (HI_S32*)((uintptr_t)pstScoreBlob->u64VirAddr);
    HI_U32 u32ClassNum = pstScoreBlob->unShape.stWhc.u32Width;
    HI_S32* ps32Proposal = (HI_S32*)((uintptr_t)pstProposalBlob->u64VirAddr);
    HI_S32* ps32BBox = (HI_S32*)((uintptr_t)pstBBoxBlob->u64VirAddr);
    HI_S32* ps32AllBoxes = HI_NULL;
    HI_S32* ps32AllBoxesTmp = HI_NULL;
    HI_FLOAT fProposalXMin, fProposalXMax, fProposalYMin, fProposalYMax, fProposalWidth, fProposalHeight, fProposalCenterX, fProposalCenterY;
    HI_FLOAT fBBoxCenterXDelta, fBBoxCenterYDelta, fBBoxWidthDelta, fBBoxHeightDelta;
    HI_FLOAT fPredWidth, fPredHeight, fPredCenterX, fPredCenterY;
    NNIE_STACK_S* pstStack = HI_NULL;
    HI_U32 u32ResultBoxNum = 0;

    SAMPLE_CHK_GOTO(0 == u32RoiCnt, FAIL_0, "u32RoiCnt equals 0\n");

    ps32AllBoxes = (HI_S32*)malloc(u32RoiCnt * SVP_WK_PROPOSAL_WIDTH * sizeof(HI_S32));
    SAMPLE_CHK_GOTO(HI_NULL == ps32AllBoxes, FAIL_0, "u32RoiCnt equals 0\n");

    pstStack = (NNIE_STACK_S*)malloc(sizeof(NNIE_STACK_S) * u32RoiCnt);
    SAMPLE_CHK_GOTO(HI_NULL == pstStack, FAIL_0, "u32RoiCnt equals 0\n");

    for (j = 0; j < u32ClassNum; j++)
    {
        for (i = 0; i < u32RoiCnt; i++)
        {
            fProposalXMin = (HI_FLOAT)(ps32Proposal[i * SVP_WK_COORDI_NUM] / 4096.0);
            fProposalYMin = (HI_FLOAT)(ps32Proposal[i * SVP_WK_COORDI_NUM + 1] / 4096.0);
            fProposalXMax = (HI_FLOAT)(ps32Proposal[i * SVP_WK_COORDI_NUM + 2] / 4096.0);
            fProposalYMax = (HI_FLOAT)(ps32Proposal[i * SVP_WK_COORDI_NUM + 3] / 4096.0);

            if (SAMPLE_RUNTIME_MODEL_TYPE_RFCN == enType)
            {
                fBBoxCenterXDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + SVP_WK_COORDI_NUM] / 4096.0);
                fBBoxCenterYDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + SVP_WK_COORDI_NUM + 1] / 4096.0);
                fBBoxWidthDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + SVP_WK_COORDI_NUM + 2] / 4096.0);
                fBBoxHeightDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + SVP_WK_COORDI_NUM + 3] / 4096.0);
            }
            else
            {
                fBBoxCenterXDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + j * SVP_WK_COORDI_NUM] / 4096.0);
                fBBoxCenterYDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + j * SVP_WK_COORDI_NUM + 1] / 4096.0);
                fBBoxWidthDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + j * SVP_WK_COORDI_NUM + 2] / 4096.0);
                fBBoxHeightDelta = (HI_FLOAT)(ps32BBox[i * u32BBoxStride / sizeof(HI_S32) + j * SVP_WK_COORDI_NUM + 3] / 4096.0);
            }

            fProposalWidth = fProposalXMax - fProposalXMin + 1;
            fProposalHeight = fProposalYMax - fProposalYMin + 1;
            fProposalCenterX = (HI_FLOAT)(fProposalXMin + fProposalWidth * 0.5);
            fProposalCenterY = (HI_FLOAT)(fProposalYMin + fProposalHeight * 0.5);
            fPredWidth = (HI_FLOAT)(fProposalWidth * exp(fBBoxWidthDelta));
            fPredHeight = (HI_FLOAT)(fProposalHeight * exp(fBBoxHeightDelta));
            fPredCenterX = fProposalCenterX + fProposalWidth * fBBoxCenterXDelta;
            fPredCenterY = fProposalCenterY + fProposalHeight * fBBoxCenterYDelta;
            ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH] = (HI_S32)(fPredCenterX - 0.5 * fPredWidth);
            ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 1] = (HI_S32)(fPredCenterY - 0.5 * fPredHeight);
            ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 2] = (HI_S32)(fPredCenterX + 0.5 * fPredWidth);
            ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 3] = (HI_S32)(fPredCenterY + 0.5 * fPredHeight);
            ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 4] = ps32Score[i * u32ScoreStride / sizeof(HI_S32) + j];
            ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 5] = RPN_SUPPRESS_FALSE; // RPN Suppress
        }

        BboxClip_N(ps32AllBoxes, pstDataBlob->unShape.stWhc.u32Width, pstDataBlob->unShape.stWhc.u32Height, u32RoiCnt);
        ps32AllBoxesTmp = ps32AllBoxes;
        NonRecursiveArgQuickSort(ps32AllBoxesTmp, 0, u32RoiCnt - 1, pstStack, u32RoiCnt);
        NonMaxSuppression(ps32AllBoxesTmp, u32RoiCnt, (HI_U32)(0.7 * SVP_WK_QUANT_BASE)); //0.7 is for rfcn to use depends on your config
        u32ResultBoxNum = 0;

        for (i = 0; i < u32RoiCnt; i++)
        {
            if (RPN_SUPPRESS_FALSE == ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 5]
                && ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 4] > 0.3 * SVP_WK_QUANT_BASE)   //0.3 is for rfcn to use depends on your config
            {
#if SAMPLE_DEBUG
                printf("%d %f %d %d %d %d\n", j, ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 4] / 4096.0,
                       ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 0], ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 1],
                       ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 2], ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH + 3]);
#endif
                u32ResultBoxNum++;

                if (0 != j) // not background
                {
                    memcpy(&ps32ResultROI[*pu32ResultROICnt * SVP_WK_PROPOSAL_WIDTH],
                           &ps32AllBoxes[i * SVP_WK_PROPOSAL_WIDTH], sizeof(HI_S32)*SVP_WK_PROPOSAL_WIDTH);
                    *pu32ResultROICnt = *pu32ResultROICnt + 1;
                }
            }
        }

#if SAMPLE_DEBUG
        printf("class %d has %d boxes\n", j, u32ResultBoxNum);
#endif
    }

    SAMPLE_FREE(pstStack);
    SAMPLE_FREE(ps32AllBoxes);
    return HI_SUCCESS;
FAIL_0:
    SAMPLE_FREE(pstStack);
    SAMPLE_FREE(ps32AllBoxes);
    return HI_FAILURE;
}
