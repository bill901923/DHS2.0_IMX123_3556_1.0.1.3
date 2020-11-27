#include <math.h>
#include "hi_runtime_comm.h"
#include "sample_data_utils.h"
#include "sample_memory_ops.h"
#include "sample_log.h"

#define SAMPLE_SSD_REPORT_NODE_NUM       12
#define SAMPLE_SSD_PRIORBOX_NUM          6
#define SAMPLE_SSD_SOFTMAX_NUM           6
#define SAMPLE_SSD_ASPECT_RATIO_NUM      6

/*SSD software parameter*/
typedef struct hiSAMPLE_SSD_PARAM_S
{
    /*----------------- Model Parameters ---------------*/
    HI_U32 au32ConvHeight[12];
    HI_U32 au32ConvWidth[12];
    HI_U32 au32ConvChannel[12];
    /*----------------- PriorBox Parameters ---------------*/
    HI_U32 au32PriorBoxWidth[6];
    HI_U32 au32PriorBoxHeight[6];
    HI_FLOAT af32PriorBoxMinSize[6][1];
    HI_FLOAT af32PriorBoxMaxSize[6][1];
    HI_U32 u32MinSizeNum;
    HI_U32 u32MaxSizeNum;
    HI_U32 u32OriImHeight;
    HI_U32 u32OriImWidth;
    HI_U32 au32InputAspectRatioNum[6];
    HI_FLOAT af32PriorBoxAspectRatio[6][2];
    HI_FLOAT af32PriorBoxStepWidth[6];
    HI_FLOAT af32PriorBoxStepHeight[6];
    HI_FLOAT f32Offset;
    HI_BOOL bFlip;
    HI_BOOL bClip;
    HI_S32 as32PriorBoxVar[4];
    /*----------------- Softmax Parameters ---------------*/
    HI_U32 au32SoftMaxInChn[6];
    HI_U32 u32SoftMaxInHeight;
    HI_U32 u32ConcatNum;
    HI_U32 u32SoftMaxOutWidth;
    HI_U32 u32SoftMaxOutHeight;
    HI_U32 u32SoftMaxOutChn;
    /*----------------- DetectionOut Parameters ---------------*/
    HI_U32 u32ClassNum;
    HI_U32 u32TopK;
    HI_U32 u32KeepTopK;
    HI_U32 u32NmsThresh;
    HI_U32 u32ConfThresh;
    HI_U32 au32DetectInputChn[6];
    HI_U32 au32ConvStride[6];
    HI_RUNTIME_MEM_S stPriorBoxTmpBuf;
    HI_RUNTIME_MEM_S stSoftMaxTmpBuf;
    HI_RUNTIME_MEM_S stGetResultTmpBuf;
    HI_RUNTIME_MEM_S stClassRoiNum;
    HI_RUNTIME_MEM_S stDstRoi;
    HI_RUNTIME_MEM_S stDstScore;
} HI_SAMPLE_SSD_PARAM_S;

static HI_U32 SAMPLE_Ssd_GetResultTmpBuf(HI_SAMPLE_SSD_PARAM_S* pstSsdParam)
{
    HI_U32 u32PriorBoxSize = 0;
    HI_U32 u32SoftMaxSize = 0;
    HI_U32 u32DetectionSize = 0;
    HI_U32 u32TotalSize = 0;
    HI_U32 u32PriorNum = 0;
    HI_U32 i = 0;

    /*priorbox size*/
    for(i = 0; i < 6; i++)
    {
        u32PriorBoxSize += pstSsdParam->au32PriorBoxHeight[i]*pstSsdParam->au32PriorBoxWidth[i]*
            SVP_WK_COORDI_NUM*2*(pstSsdParam->u32MaxSizeNum+pstSsdParam->u32MinSizeNum+
            pstSsdParam->au32InputAspectRatioNum[i]*2*pstSsdParam->u32MinSizeNum)*sizeof(HI_U32);
    }
    pstSsdParam->stPriorBoxTmpBuf.u32Size = u32PriorBoxSize;
    u32TotalSize+=u32PriorBoxSize;
    /*softmax size*/
    for(i = 0; i < pstSsdParam->u32ConcatNum; i++)
    {
        u32SoftMaxSize += pstSsdParam->au32SoftMaxInChn[i]*sizeof(HI_U32);
    }
    pstSsdParam->stSoftMaxTmpBuf.u32Size = u32SoftMaxSize;
    u32TotalSize+=u32SoftMaxSize;

    /*detection size*/
    for(i = 0; i < pstSsdParam->u32ConcatNum; i++)
    {
        u32PriorNum+=pstSsdParam->au32DetectInputChn[i]/SVP_WK_COORDI_NUM;
    }
    u32DetectionSize+=u32PriorNum*SVP_WK_COORDI_NUM*sizeof(HI_U32);
    u32DetectionSize+=u32PriorNum*SVP_WK_PROPOSAL_WIDTH*sizeof(HI_U32)*2;
    u32DetectionSize+=u32PriorNum*2*sizeof(HI_U32);
    pstSsdParam->stGetResultTmpBuf.u32Size = u32DetectionSize;
    u32TotalSize+=u32DetectionSize;

    return u32TotalSize;
}

static HI_S32 SAMPLE_Ssd_InitParam(HI_RUNTIME_BLOB_S* pstSrcBlob, HI_RUNTIME_BLOB_S* pstDstBlob, HI_SAMPLE_SSD_PARAM_S* pstSsdParam) {
    HI_U32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32ClassNum = 0;
    HI_U32 u32TotalSize = 0;
    HI_U32 u32DstRoiSize = 0;
    HI_U32 u32DstScoreSize = 0;
    HI_U32 u32ClassRoiNumSize = 0;
    HI_U32 u32TmpBufTotalSize = 0;
    HI_U64 u64PhyAddr = 0;
    HI_U8* pu8VirAddr = NULL;
    HI_RUNTIME_MEM_S stMem;
    memset(&stMem, 0, sizeof(stMem));

    for(i = 0; i < 12; i++)
    {
        pstSsdParam->au32ConvHeight[i] = pstDstBlob[i].unShape.stWhc.u32Chn;
        pstSsdParam->au32ConvWidth[i] = pstDstBlob[i].unShape.stWhc.u32Height;
        pstSsdParam->au32ConvChannel[i] = pstDstBlob[i].unShape.stWhc.u32Width;
        if(i%2==1)
        {
            pstSsdParam->au32ConvStride[i/2] = ALIGN16(pstSsdParam->au32ConvChannel[i]*sizeof(HI_U32))/sizeof(HI_U32);
        }
    }

    /*Set PriorBox Parameters*/
    pstSsdParam->au32PriorBoxWidth[0] = 38;
    pstSsdParam->au32PriorBoxWidth[1] = 19;
    pstSsdParam->au32PriorBoxWidth[2] = 10;
    pstSsdParam->au32PriorBoxWidth[3] = 5;
    pstSsdParam->au32PriorBoxWidth[4] = 3;
    pstSsdParam->au32PriorBoxWidth[5] = 1;

    pstSsdParam->au32PriorBoxHeight[0] = 38;
    pstSsdParam->au32PriorBoxHeight[1] = 19;
    pstSsdParam->au32PriorBoxHeight[2] = 10;
    pstSsdParam->au32PriorBoxHeight[3] = 5;
    pstSsdParam->au32PriorBoxHeight[4] = 3;
    pstSsdParam->au32PriorBoxHeight[5] = 1;

    pstSsdParam->u32OriImHeight = 300;
    pstSsdParam->u32OriImWidth = 300;

    pstSsdParam->af32PriorBoxMinSize[0][0] = 30.0f;
    pstSsdParam->af32PriorBoxMinSize[1][0] = 60.0f;
    pstSsdParam->af32PriorBoxMinSize[2][0] = 111.0f;
    pstSsdParam->af32PriorBoxMinSize[3][0] = 162.0f;
    pstSsdParam->af32PriorBoxMinSize[4][0] = 213.0f;
    pstSsdParam->af32PriorBoxMinSize[5][0] = 264.0f;

    pstSsdParam->af32PriorBoxMaxSize[0][0] = 60.0f;
    pstSsdParam->af32PriorBoxMaxSize[1][0] = 111.0f;
    pstSsdParam->af32PriorBoxMaxSize[2][0] = 162.0f;
    pstSsdParam->af32PriorBoxMaxSize[3][0] = 213.0f;
    pstSsdParam->af32PriorBoxMaxSize[4][0] = 264.0f;
    pstSsdParam->af32PriorBoxMaxSize[5][0] = 315.0f;

    pstSsdParam->u32MinSizeNum = 1;
    pstSsdParam->u32MaxSizeNum = 1;
    pstSsdParam->bFlip= HI_TRUE;
    pstSsdParam->bClip= HI_FALSE;

    pstSsdParam->au32InputAspectRatioNum[0] = 1;
    pstSsdParam->au32InputAspectRatioNum[1] = 2;
    pstSsdParam->au32InputAspectRatioNum[2] = 2;
    pstSsdParam->au32InputAspectRatioNum[3] = 2;
    pstSsdParam->au32InputAspectRatioNum[4] = 1;
    pstSsdParam->au32InputAspectRatioNum[5] = 1;

    pstSsdParam->af32PriorBoxAspectRatio[0][0] = 2;
    pstSsdParam->af32PriorBoxAspectRatio[0][1] = 0;
    pstSsdParam->af32PriorBoxAspectRatio[1][0] = 2;
    pstSsdParam->af32PriorBoxAspectRatio[1][1] = 3;
    pstSsdParam->af32PriorBoxAspectRatio[2][0] = 2;
    pstSsdParam->af32PriorBoxAspectRatio[2][1] = 3;
    pstSsdParam->af32PriorBoxAspectRatio[3][0] = 2;
    pstSsdParam->af32PriorBoxAspectRatio[3][1] = 3;
    pstSsdParam->af32PriorBoxAspectRatio[4][0] = 2;
    pstSsdParam->af32PriorBoxAspectRatio[4][1] = 0;
    pstSsdParam->af32PriorBoxAspectRatio[5][0] = 2;
    pstSsdParam->af32PriorBoxAspectRatio[5][1] = 0;

    pstSsdParam->af32PriorBoxStepWidth[0] = 8;
    pstSsdParam->af32PriorBoxStepWidth[1] = 16;
    pstSsdParam->af32PriorBoxStepWidth[2] = 32;
    pstSsdParam->af32PriorBoxStepWidth[3] = 64;
    pstSsdParam->af32PriorBoxStepWidth[4] = 100;
    pstSsdParam->af32PriorBoxStepWidth[5] = 300;

    pstSsdParam->af32PriorBoxStepHeight[0] = 8;
    pstSsdParam->af32PriorBoxStepHeight[1] = 16;
    pstSsdParam->af32PriorBoxStepHeight[2] = 32;
    pstSsdParam->af32PriorBoxStepHeight[3] = 64;
    pstSsdParam->af32PriorBoxStepHeight[4] = 100;
    pstSsdParam->af32PriorBoxStepHeight[5] = 300;

    pstSsdParam->f32Offset = 0.5f;

    pstSsdParam->as32PriorBoxVar[0] = (HI_S32)(0.1f*SVP_WK_QUANT_BASE);
    pstSsdParam->as32PriorBoxVar[1] = (HI_S32)(0.1f*SVP_WK_QUANT_BASE);
    pstSsdParam->as32PriorBoxVar[2] = (HI_S32)(0.2f*SVP_WK_QUANT_BASE);
    pstSsdParam->as32PriorBoxVar[3] = (HI_S32)(0.2f*SVP_WK_QUANT_BASE);

    /*Set Softmax Parameters*/
    pstSsdParam->u32SoftMaxInHeight = 21;
    pstSsdParam->au32SoftMaxInChn[0] = 121296;
    pstSsdParam->au32SoftMaxInChn[1] = 45486;
    pstSsdParam->au32SoftMaxInChn[2] = 12600;
    pstSsdParam->au32SoftMaxInChn[3] = 3150;
    pstSsdParam->au32SoftMaxInChn[4] = 756;
    pstSsdParam->au32SoftMaxInChn[5] = 84;

    pstSsdParam->u32ConcatNum = 6;
    pstSsdParam->u32SoftMaxOutWidth = 1;
    pstSsdParam->u32SoftMaxOutHeight = 21;
    pstSsdParam->u32SoftMaxOutChn = 8732;

    /*Set DetectionOut Parameters*/
    pstSsdParam->u32ClassNum = 21;
    pstSsdParam->u32TopK = 400;
    pstSsdParam->u32KeepTopK = 200;
    pstSsdParam->u32NmsThresh = (HI_U32)(0.3f*SVP_WK_QUANT_BASE);
    pstSsdParam->u32ConfThresh = 1;
    pstSsdParam->au32DetectInputChn[0] = 23104;
    pstSsdParam->au32DetectInputChn[1] = 8664;
    pstSsdParam->au32DetectInputChn[2] = 2400;
    pstSsdParam->au32DetectInputChn[3] = 600;
    pstSsdParam->au32DetectInputChn[4] = 144;
    pstSsdParam->au32DetectInputChn[5] = 16;

    /*Malloc assist buffer memory*/
    u32ClassNum = pstSsdParam->u32ClassNum;
    u32TotalSize = SAMPLE_Ssd_GetResultTmpBuf(pstSsdParam);
    u32DstRoiSize = ALIGN16(u32ClassNum*pstSsdParam->u32TopK*sizeof(HI_U32)*SVP_WK_COORDI_NUM);
    u32DstScoreSize = ALIGN16(u32ClassNum*pstSsdParam->u32TopK*sizeof(HI_U32));
    u32ClassRoiNumSize = ALIGN16(u32ClassNum*sizeof(HI_U32));
    u32TotalSize = u32TotalSize+u32DstRoiSize+u32DstScoreSize+u32ClassRoiNumSize;

    stMem.u32Size = u32TotalSize;
    s32Ret = SAMPLE_AllocMem(&stMem, HI_TRUE);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_Utils_AllocMem failed!\n");
    u64PhyAddr = stMem.u64PhyAddr;
    pu8VirAddr = (HI_U8*)((uintptr_t)stMem.u64VirAddr);
    memset(pu8VirAddr, 0, u32TotalSize);
    s32Ret = SAMPLE_FlushCache(&stMem);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_Utils_FlushCache failed!\n");

   /*set each tmp buffer addr*/
    pstSsdParam->stPriorBoxTmpBuf.u64PhyAddr = u64PhyAddr;
    pstSsdParam->stPriorBoxTmpBuf.u64VirAddr = (HI_U64)((uintptr_t)pu8VirAddr);

    pstSsdParam->stSoftMaxTmpBuf.u64PhyAddr = u64PhyAddr+
        pstSsdParam->stPriorBoxTmpBuf.u32Size;
    pstSsdParam->stSoftMaxTmpBuf.u64VirAddr = (HI_U64)((uintptr_t)(pu8VirAddr+
        pstSsdParam->stPriorBoxTmpBuf.u32Size));

    pstSsdParam->stGetResultTmpBuf.u64PhyAddr = u64PhyAddr+
        pstSsdParam->stPriorBoxTmpBuf.u32Size+pstSsdParam->stSoftMaxTmpBuf.u32Size;
    pstSsdParam->stGetResultTmpBuf.u64VirAddr = (HI_U64)((uintptr_t)(pu8VirAddr+
        pstSsdParam->stPriorBoxTmpBuf.u32Size+ pstSsdParam->stSoftMaxTmpBuf.u32Size));

    u32TmpBufTotalSize = pstSsdParam->stPriorBoxTmpBuf.u32Size+
        pstSsdParam->stSoftMaxTmpBuf.u32Size + pstSsdParam->stGetResultTmpBuf.u32Size;

    /*set result blob*/
    pstSsdParam->stDstRoi.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize;
    pstSsdParam->stDstRoi.u64VirAddr = (HI_U64)((uintptr_t)(pu8VirAddr+u32TmpBufTotalSize));

    pstSsdParam->stDstScore.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize+u32DstRoiSize;
    pstSsdParam->stDstScore.u64VirAddr = (HI_U64)((uintptr_t)(pu8VirAddr+u32TmpBufTotalSize+u32DstRoiSize));

    pstSsdParam->stClassRoiNum.u64PhyAddr = u64PhyAddr+u32TmpBufTotalSize+
        u32DstRoiSize+u32DstScoreSize;
    pstSsdParam->stClassRoiNum.u64VirAddr = (HI_U64)((uintptr_t)(pu8VirAddr+u32TmpBufTotalSize+
        u32DstRoiSize+u32DstScoreSize));

    return s32Ret;
FAIL_0:
    SAMPLE_FreeMem(&stMem);
    return s32Ret;
}

static HI_S32 SAMPLE_Ssd_PriorBoxForward(HI_U32 u32PriorBoxWidth,
    HI_U32 u32PriorBoxHeight, HI_U32 u32OriImWidth, HI_U32 u32OriImHeight,
    HI_FLOAT* pf32PriorBoxMinSize, HI_U32 u32MinSizeNum, HI_FLOAT* pf32PriorBoxMaxSize,
    HI_U32 u32MaxSizeNum, HI_BOOL bFlip, HI_BOOL bClip, HI_U32 u32InputAspectRatioNum,
    HI_FLOAT af32PriorBoxAspectRatio[],HI_FLOAT f32PriorBoxStepWidth,
    HI_FLOAT f32PriorBoxStepHeight,HI_FLOAT f32Offset,HI_S32 as32PriorBoxVar[],
    HI_S32* ps32PriorboxOutputData, const HI_U32 u32Length)
{
    HI_U32 u32AspectRatioNum = 0;
    HI_U32 u32Index = 0;
    HI_FLOAT af32AspectRatio[SAMPLE_SSD_ASPECT_RATIO_NUM] = { 0 };
    HI_U32 u32NumPrior = 0;
    HI_FLOAT f32CenterX = 0;
    HI_FLOAT f32CenterY = 0;
    HI_FLOAT f32BoxHeight = 0;
    HI_FLOAT f32BoxWidth = 0;
    HI_FLOAT f32MaxBoxWidth = 0;
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 n = 0;
    HI_U32 h = 0;
    HI_U32 w = 0;
    SAMPLE_CHK_RET((HI_TRUE == bFlip && u32InputAspectRatioNum >
        (SAMPLE_SSD_ASPECT_RATIO_NUM-1)/2), HI_FAILURE,
        "Error,when bFlip is true, u32InputAspectRatioNum(%d) can't be greater than %d!\n",
        u32InputAspectRatioNum, (SAMPLE_SSD_ASPECT_RATIO_NUM-1)/2);
    SAMPLE_CHK_RET((HI_FALSE == bFlip && u32InputAspectRatioNum >
        (SAMPLE_SSD_ASPECT_RATIO_NUM-1)), HI_FAILURE,
        "Error,when bFlip is false, u32InputAspectRatioNum(%d) can't be greater than %d!\n",
        u32InputAspectRatioNum, (SAMPLE_SSD_ASPECT_RATIO_NUM-1));

    // generate aspect_ratios
    u32AspectRatioNum = 0;
    af32AspectRatio[0] = 1;
    u32AspectRatioNum++;
    for (i = 0; i < u32InputAspectRatioNum; i++)
    {
        af32AspectRatio[u32AspectRatioNum++] = af32PriorBoxAspectRatio[i];
        if (bFlip)
        {
            af32AspectRatio[u32AspectRatioNum++] = 1.0f / af32PriorBoxAspectRatio[i];
        }
    }
    u32NumPrior = u32MinSizeNum * u32AspectRatioNum + u32MaxSizeNum;

    u32Index = 0;
    for (h = 0; h < u32PriorBoxHeight; h++)
    {
        for (w = 0; w < u32PriorBoxWidth; w++)
        {
            f32CenterX = (w + f32Offset) * f32PriorBoxStepWidth;
            f32CenterY = (h + f32Offset) * f32PriorBoxStepHeight;
            for (n = 0; n < u32MinSizeNum; n++)
            {
                /*** first prior ***/
                f32BoxHeight = pf32PriorBoxMinSize[n];
                f32BoxWidth = pf32PriorBoxMinSize[n];
                ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX - f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterY - f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX + f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterY + f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                /*** second prior ***/
                if(u32MaxSizeNum>0)
                {
                    f32MaxBoxWidth = (HI_FLOAT)sqrt(pf32PriorBoxMinSize[n] * pf32PriorBoxMaxSize[n]);
                    f32BoxHeight = f32MaxBoxWidth;
                    f32BoxWidth = f32MaxBoxWidth;
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX - f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterY - f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX + f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterY + f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                }
                /**** rest of priors, skip AspectRatio == 1 ****/
                for (i = 1; i < u32AspectRatioNum; i++)
                {
                    f32BoxWidth = (HI_FLOAT)(pf32PriorBoxMinSize[n] * sqrt( af32AspectRatio[i] ));
                    f32BoxHeight = (HI_FLOAT)(pf32PriorBoxMinSize[n]/sqrt( af32AspectRatio[i] ));
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX - f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterY - f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterX + f32BoxWidth * SAMPLE_SVP_NNIE_HALF);
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)(f32CenterY + f32BoxHeight * SAMPLE_SVP_NNIE_HALF);
                }
            }
        }
    }
    /************ clip the priors' coordidates, within [0, u32ImgWidth] & [0, u32ImgHeight] *************/
    if (bClip)
    {
        for (i = 0; i < (HI_U32)(u32PriorBoxWidth * u32PriorBoxHeight * SVP_WK_COORDI_NUM*u32NumPrior / 2); i++)
        {
            ps32PriorboxOutputData[2 * i] = min((HI_U32)max(ps32PriorboxOutputData[2 * i], 0), u32OriImWidth);
            ps32PriorboxOutputData[2 * i + 1] = min((HI_U32)max(ps32PriorboxOutputData[2 * i + 1], 0), u32OriImHeight);
        }
    }
    /*********************** get var **********************/
    if(0 == u32Length)
    {
        SAMPLE_LOG_INFO("as32PriorBoxVar is zero length!");
        return HI_FAILURE;
    }
    for (h = 0; h < u32PriorBoxHeight; h++)
    {
        for (w = 0; w < u32PriorBoxWidth; w++)
        {
            for (i = 0; i < u32NumPrior; i++)
            {
                for (j = 0; j < SVP_WK_COORDI_NUM; j++)
                {
                    ps32PriorboxOutputData[u32Index++] = (HI_S32)as32PriorBoxVar[j];
                }
            }
        }
    }
    return HI_SUCCESS;
}

static HI_S32 SAMPLE_SSD_SoftMax(HI_S32* ps32Src, HI_S32 s32ArraySize, HI_S32* ps32Dst)
{
    /***** define parameters ****/
    HI_S32 s32Max = 0;
    HI_S32 s32Sum = 0;
    HI_S32 i = 0;
    for (i = 0; i < s32ArraySize; ++i)
    {
        if (s32Max < ps32Src[i])
        {
            s32Max = ps32Src[i];
        }
    }
    for (i = 0; i < s32ArraySize; ++i)
    {
        ps32Dst[i] = (HI_S32)(SVP_WK_QUANT_BASE* exp((HI_FLOAT)(ps32Src[i] - s32Max) / SVP_WK_QUANT_BASE));
        s32Sum += ps32Dst[i];
    }
    for (i = 0; i < s32ArraySize; ++i)
    {
        ps32Dst[i] = (HI_S32)(((HI_FLOAT)ps32Dst[i] / (HI_FLOAT)s32Sum) * SVP_WK_QUANT_BASE);
    }
    return HI_SUCCESS;
}

static HI_S32 SAMPLE_Ssd_SoftmaxForward(HI_U32 u32SoftMaxInHeight,
    HI_U32 au32SoftMaxInChn[], HI_U32 u32ConcatNum, HI_U32 au32ConvStride[],
    HI_U32 u32SoftMaxOutWidth, HI_U32 u32SoftMaxOutHeight, HI_U32 u32SoftMaxOutChn,
    HI_S32* aps32SoftMaxInputData[], HI_S32* ps32SoftMaxOutputData)
{
    HI_S32* ps32InputData = NULL;
    HI_S32* ps32OutputTmp = NULL;
    HI_U32 u32OuterNum = 0;
    HI_U32 u32InnerNum = 0;
    HI_U32 u32InputChannel = 0;
    HI_U32 i = 0;
    HI_U32 u32ConcatCnt = 0;
    HI_S32 s32Ret = 0;
    HI_U32 u32Stride = 0;
    HI_U32 u32Skip = 0;
    HI_U32 u32Left = 0;
    ps32OutputTmp = ps32SoftMaxOutputData;
    for (u32ConcatCnt = 0; u32ConcatCnt < u32ConcatNum; u32ConcatCnt++)
    {
        ps32InputData = aps32SoftMaxInputData[u32ConcatCnt];
        u32Stride = au32ConvStride[u32ConcatCnt];
        u32InputChannel = au32SoftMaxInChn[u32ConcatCnt];
        u32OuterNum = u32InputChannel / u32SoftMaxInHeight;
        u32InnerNum = u32SoftMaxInHeight;
        u32Skip = u32Stride / u32InnerNum;
        u32Left = u32Stride % u32InnerNum;        // do softmax
        for (i = 0; i < u32OuterNum; i++)
        {
            s32Ret = SAMPLE_SSD_SoftMax(ps32InputData, (HI_S32)u32InnerNum,ps32OutputTmp);
            if ((i + 1) % u32Skip == 0)
            {
                ps32InputData += u32Left;
            }
            ps32InputData += u32InnerNum;
            ps32OutputTmp += u32InnerNum;
        }
    }
    return s32Ret;
}

static HI_S32 SAMPLE_Ssd_DetectionOutForward(HI_U32 u32ConcatNum,
    HI_U32 u32ConfThresh,HI_U32 u32ClassNum, HI_U32 u32TopK, HI_U32 u32KeepTopK, HI_U32 u32NmsThresh,
    HI_U32 au32DetectInputChn[], HI_S32* aps32AllLocPreds[], HI_S32* aps32AllPriorBoxes[],
    HI_S32* ps32ConfScores, HI_S32* ps32AssistMemPool, HI_S32* ps32DstScoreSrc,
    HI_S32* ps32DstBboxSrc, HI_S32* ps32RoiOutCntSrc)
{
    /************* check input parameters ****************/
    /******** define variables **********/
    HI_S32* ps32LocPreds = NULL;
    HI_S32* ps32PriorBoxes = NULL;
    HI_S32* ps32PriorVar = NULL;
    HI_S32* ps32AllDecodeBoxes = NULL;
    HI_S32* ps32DstScore = NULL;
    HI_S32* ps32DstBbox = NULL;
    HI_S32* ps32ClassRoiNum = NULL;
    HI_U32 u32RoiOutCnt = 0;
    HI_S32* ps32SingleProposal = NULL;
    HI_S32* ps32AfterTopK = NULL;
    NNIE_STACK_S* pstStack = NULL;
    HI_U32 u32PriorNum = 0;
    HI_U32 u32NumPredsPerClass = 0;
    HI_FLOAT f32PriorWidth = 0;
    HI_FLOAT f32PriorHeight = 0;
    HI_FLOAT f32PriorCenterX = 0;
    HI_FLOAT f32PriorCenterY = 0;
    HI_FLOAT f32DecodeBoxCenterX = 0;
    HI_FLOAT f32DecodeBoxCenterY = 0;
    HI_FLOAT f32DecodeBoxWidth = 0;
    HI_FLOAT f32DecodeBoxHeight = 0;
    HI_U32 u32SrcIdx = 0;
    HI_U32 u32AfterFilter = 0;
    HI_U32 u32AfterTopK = 0;
    HI_U32 u32KeepCnt = 0;
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 u32Offset = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    u32PriorNum = 0;
    for (i = 0; i < u32ConcatNum; i++)
    {
        u32PriorNum += au32DetectInputChn[i] / SVP_WK_COORDI_NUM;
    }
    //prepare for Assist MemPool
    ps32AllDecodeBoxes = ps32AssistMemPool;
    ps32SingleProposal = ps32AllDecodeBoxes + u32PriorNum * SVP_WK_COORDI_NUM;
    ps32AfterTopK = ps32SingleProposal + SVP_WK_PROPOSAL_WIDTH * u32PriorNum;
    pstStack = (NNIE_STACK_S*)(ps32AfterTopK + u32PriorNum * SVP_WK_PROPOSAL_WIDTH);
    u32SrcIdx = 0;
    for (i = 0; i < u32ConcatNum; i++)
    {
        /********** get loc predictions ************/
        ps32LocPreds = aps32AllLocPreds[i];
        u32NumPredsPerClass = au32DetectInputChn[i] / SVP_WK_COORDI_NUM;
        /********** get Prior Bboxes ************/
        ps32PriorBoxes = aps32AllPriorBoxes[i];
        ps32PriorVar = ps32PriorBoxes + u32NumPredsPerClass*SVP_WK_COORDI_NUM;
        for (j = 0; j < u32NumPredsPerClass; j++)
        {
            //printf("ps32PriorBoxes start***************\n");
            f32PriorWidth = (HI_FLOAT)(ps32PriorBoxes[j*SVP_WK_COORDI_NUM+2] - ps32PriorBoxes[j*SVP_WK_COORDI_NUM]);
            f32PriorHeight = (HI_FLOAT)(ps32PriorBoxes[j*SVP_WK_COORDI_NUM+3] - ps32PriorBoxes[j*SVP_WK_COORDI_NUM + 1]);
            f32PriorCenterX = (ps32PriorBoxes[j*SVP_WK_COORDI_NUM+2] + ps32PriorBoxes[j*SVP_WK_COORDI_NUM])*SAMPLE_SVP_NNIE_HALF;
            f32PriorCenterY = (ps32PriorBoxes[j*SVP_WK_COORDI_NUM+3] + ps32PriorBoxes[j*SVP_WK_COORDI_NUM+1])*SAMPLE_SVP_NNIE_HALF;

            f32DecodeBoxCenterX = ((HI_FLOAT)ps32PriorVar[j*SVP_WK_COORDI_NUM]/SVP_WK_QUANT_BASE)*
                ((HI_FLOAT)ps32LocPreds[j*SVP_WK_COORDI_NUM]/SVP_WK_QUANT_BASE)*f32PriorWidth+f32PriorCenterX;

            f32DecodeBoxCenterY = ((HI_FLOAT)ps32PriorVar[j*SVP_WK_COORDI_NUM+1]/SVP_WK_QUANT_BASE)*
                ((HI_FLOAT)ps32LocPreds[j*SVP_WK_COORDI_NUM+1]/SVP_WK_QUANT_BASE)*f32PriorHeight+f32PriorCenterY;

            f32DecodeBoxWidth = (HI_FLOAT)exp(((HI_FLOAT)ps32PriorVar[j*SVP_WK_COORDI_NUM+2]/SVP_WK_QUANT_BASE)*
                ((HI_FLOAT)ps32LocPreds[j*SVP_WK_COORDI_NUM+2]/SVP_WK_QUANT_BASE))*f32PriorWidth;

            f32DecodeBoxHeight = (HI_FLOAT)exp(((HI_FLOAT)ps32PriorVar[j*SVP_WK_COORDI_NUM+3]/SVP_WK_QUANT_BASE)*
                ((HI_FLOAT)ps32LocPreds[j*SVP_WK_COORDI_NUM+3]/SVP_WK_QUANT_BASE))*f32PriorHeight;

            //printf("ps32PriorBoxes end***************\n");

            ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(f32DecodeBoxCenterX - f32DecodeBoxWidth * SAMPLE_SVP_NNIE_HALF);
            ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(f32DecodeBoxCenterY - f32DecodeBoxHeight * SAMPLE_SVP_NNIE_HALF);
            ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(f32DecodeBoxCenterX + f32DecodeBoxWidth * SAMPLE_SVP_NNIE_HALF);
            ps32AllDecodeBoxes[u32SrcIdx++] = (HI_S32)(f32DecodeBoxCenterY + f32DecodeBoxHeight * SAMPLE_SVP_NNIE_HALF);
        }
    }
    // do NMS for each class
    u32AfterTopK = 0;
    for (i = 0; i < u32ClassNum; i++)
    {
        for (j = 0; j < u32PriorNum; j++)
        {
            ps32SingleProposal[j * SVP_WK_PROPOSAL_WIDTH] = ps32AllDecodeBoxes[j * SVP_WK_COORDI_NUM];
            ps32SingleProposal[j * SVP_WK_PROPOSAL_WIDTH + 1] = ps32AllDecodeBoxes[j * SVP_WK_COORDI_NUM + 1];
            ps32SingleProposal[j * SVP_WK_PROPOSAL_WIDTH + 2] = ps32AllDecodeBoxes[j * SVP_WK_COORDI_NUM + 2];
            ps32SingleProposal[j * SVP_WK_PROPOSAL_WIDTH + 3] = ps32AllDecodeBoxes[j * SVP_WK_COORDI_NUM + 3];
            ps32SingleProposal[j * SVP_WK_PROPOSAL_WIDTH + 4] = ps32ConfScores[j*u32ClassNum + i];
            ps32SingleProposal[j * SVP_WK_PROPOSAL_WIDTH + 5] = 0;
        }
        s32Ret = NonRecursiveArgQuickSort(ps32SingleProposal, 0, u32PriorNum - 1, pstStack, u32TopK);
        u32AfterFilter = (u32PriorNum < u32TopK) ? u32PriorNum : u32TopK;
        s32Ret = NonMaxSuppression(ps32SingleProposal, u32AfterFilter, u32NmsThresh);
        u32RoiOutCnt = 0;
        ps32DstScore = (HI_S32*)ps32DstScoreSrc;
        ps32DstBbox = (HI_S32*)ps32DstBboxSrc;
        ps32ClassRoiNum = (HI_S32*)ps32RoiOutCntSrc;
        ps32DstScore += (HI_S32)u32AfterTopK;
        ps32DstBbox += (HI_S32)(u32AfterTopK * SVP_WK_COORDI_NUM);
        for (j = 0; j < u32TopK; j++)
        {
            if (ps32SingleProposal[j * SVP_WK_PROPOSAL_WIDTH + 5] == 0 &&
                ps32SingleProposal[j * SVP_WK_PROPOSAL_WIDTH + 4] > (HI_S32)u32NmsThresh)
            {
                ps32DstScore[u32RoiOutCnt] = ps32SingleProposal[j * 6 + 4];
                ps32DstBbox[u32RoiOutCnt * SVP_WK_COORDI_NUM] = ps32SingleProposal[j * SVP_WK_PROPOSAL_WIDTH];
                ps32DstBbox[u32RoiOutCnt * SVP_WK_COORDI_NUM + 1] = ps32SingleProposal[j * SVP_WK_PROPOSAL_WIDTH + 1];
                ps32DstBbox[u32RoiOutCnt * SVP_WK_COORDI_NUM + 2] = ps32SingleProposal[j * SVP_WK_PROPOSAL_WIDTH + 2];
                ps32DstBbox[u32RoiOutCnt * SVP_WK_COORDI_NUM + 3] = ps32SingleProposal[j * SVP_WK_PROPOSAL_WIDTH + 3];
                u32RoiOutCnt++;
            }
        }
        ps32ClassRoiNum[i] = (HI_S32)u32RoiOutCnt;
        u32AfterTopK += u32RoiOutCnt;
    }

    u32KeepCnt = 0;
    u32Offset = 0;
    if (u32AfterTopK > u32KeepTopK)
    {
        SAMPLE_CHK_RET(HI_NULL == ps32ClassRoiNum, HI_FAILURE, "ps32ClassRoiNum is null");
        u32Offset = ps32ClassRoiNum[0];
        for (i = 1; i < u32ClassNum; i++)
        {
            ps32DstScore = (HI_S32*)ps32DstScoreSrc;
            ps32DstBbox = (HI_S32*)ps32DstBboxSrc;
            ps32ClassRoiNum = (HI_S32*)ps32RoiOutCntSrc;
            ps32DstScore += (HI_S32)(u32Offset);
            ps32DstBbox += (HI_S32)(u32Offset * SVP_WK_COORDI_NUM);
            for (j = 0; j < (HI_U32)ps32ClassRoiNum[i]; j++)
            {
                ps32AfterTopK[u32KeepCnt * SVP_WK_PROPOSAL_WIDTH] = ps32DstBbox[j * SVP_WK_COORDI_NUM];
                ps32AfterTopK[u32KeepCnt * SVP_WK_PROPOSAL_WIDTH + 1] = ps32DstBbox[j * SVP_WK_COORDI_NUM + 1];
                ps32AfterTopK[u32KeepCnt * SVP_WK_PROPOSAL_WIDTH + 2] = ps32DstBbox[j * SVP_WK_COORDI_NUM + 2];
                ps32AfterTopK[u32KeepCnt * SVP_WK_PROPOSAL_WIDTH + 3] = ps32DstBbox[j * SVP_WK_COORDI_NUM + 3];
                ps32AfterTopK[u32KeepCnt * SVP_WK_PROPOSAL_WIDTH + 4] = ps32DstScore[j];
                ps32AfterTopK[u32KeepCnt * SVP_WK_PROPOSAL_WIDTH + 5] = i;
                u32KeepCnt++;
            }
            u32Offset = u32Offset + ps32ClassRoiNum[i];
        }
        s32Ret = NonRecursiveArgQuickSort(ps32AfterTopK, 0, u32KeepCnt - 1, pstStack, u32KeepCnt);

        u32Offset = 0;
        u32Offset = ps32ClassRoiNum[0];
        for (i = 1; i < u32ClassNum; i++)
        {
            u32RoiOutCnt = 0;
            ps32DstScore = (HI_S32*)ps32DstScoreSrc;
            ps32DstBbox = (HI_S32*)ps32DstBboxSrc;
            ps32ClassRoiNum = (HI_S32*)ps32RoiOutCntSrc;
            ps32DstScore += (HI_S32)(u32Offset);
            ps32DstBbox += (HI_S32)(u32Offset * SVP_WK_COORDI_NUM);
            for (j = 0; j < u32KeepTopK; j++)
            {
                if (ps32AfterTopK[j * SVP_WK_PROPOSAL_WIDTH + 5] == i)
                {
                    ps32DstScore[u32RoiOutCnt] = ps32AfterTopK[j * SVP_WK_PROPOSAL_WIDTH + 4];
                    ps32DstBbox[u32RoiOutCnt * SVP_WK_COORDI_NUM] = ps32AfterTopK[j * SVP_WK_PROPOSAL_WIDTH];
                    ps32DstBbox[u32RoiOutCnt * SVP_WK_COORDI_NUM + 1] = ps32AfterTopK[j * SVP_WK_PROPOSAL_WIDTH + 1];
                    ps32DstBbox[u32RoiOutCnt * SVP_WK_COORDI_NUM + 2] = ps32AfterTopK[j * SVP_WK_PROPOSAL_WIDTH + 2];
                    ps32DstBbox[u32RoiOutCnt * SVP_WK_COORDI_NUM + 3] = ps32AfterTopK[j * SVP_WK_PROPOSAL_WIDTH + 3];
                    u32RoiOutCnt++;
                }
            }
            ps32ClassRoiNum[i] = (HI_S32)u32RoiOutCnt;
            u32Offset += u32RoiOutCnt;
        }
    }
    return s32Ret;
}

static HI_S32 SAMPLE_SSD_Detection_PrintResult(HI_RUNTIME_MEM_S *pstDstScore,
                    HI_RUNTIME_MEM_S *pstDstRoi, HI_RUNTIME_MEM_S *pstClassRoiNum,
                    HI_FLOAT f32PrintResultThresh, HI_S32* ps32ResultROI, HI_U32* pu32ResultROICnt)
{
    HI_U32 i = 0, j = 0;
    HI_U32 u32RoiNumBias = 0;
    HI_U32 u32ScoreBias = 0;
    HI_U32 u32BboxBias = 0;
    HI_FLOAT f32Score = 0.0f;
    HI_S32* ps32Score = (HI_S32*)((uintptr_t)pstDstScore->u64VirAddr);
    HI_S32* ps32Roi = (HI_S32*)((uintptr_t)pstDstRoi->u64VirAddr);
    HI_S32* ps32ClassRoiNum = (HI_S32*)((uintptr_t)pstClassRoiNum->u64VirAddr);
    HI_U32 u32ClassNum = 21;
    HI_S32 s32XMin = 0,s32YMin= 0,s32XMax = 0,s32YMax = 0;

    u32RoiNumBias += ps32ClassRoiNum[0];
    for (i = 1; i < u32ClassNum; i++)
    {
        u32ScoreBias = u32RoiNumBias;
        u32BboxBias = u32RoiNumBias * SVP_WK_COORDI_NUM;
        // if the confidence score greater than result threshold, the result will be printed
        if((HI_FLOAT)ps32Score[u32ScoreBias] / SVP_WK_QUANT_BASE >=
            f32PrintResultThresh && ps32ClassRoiNum[i]!=0)
        {
            printf("==== The %dth class box info====\n", i);
        }
        for (j = 0; j < (HI_U32)ps32ClassRoiNum[i]; j++)
        {
            f32Score = (HI_FLOAT)ps32Score[u32ScoreBias + j] / SVP_WK_QUANT_BASE;
            if (f32Score < f32PrintResultThresh)
            {
                break;
            }
            s32XMin = ps32Roi[u32BboxBias + j*SVP_WK_COORDI_NUM];
            s32YMin = ps32Roi[u32BboxBias + j*SVP_WK_COORDI_NUM + 1];
            s32XMax = ps32Roi[u32BboxBias + j*SVP_WK_COORDI_NUM + 2];
            s32YMax = ps32Roi[u32BboxBias + j*SVP_WK_COORDI_NUM + 3];
            printf("%d %d %d %d %f\n", s32XMin, s32YMin, s32XMax, s32YMax, f32Score);

            memcpy(&ps32ResultROI[*pu32ResultROICnt * SVP_WK_PROPOSAL_WIDTH],
                   &ps32Roi[u32BboxBias + j * SVP_WK_COORDI_NUM], sizeof(HI_S32)*SVP_WK_COORDI_NUM);
            ps32ResultROI[*pu32ResultROICnt * SVP_WK_PROPOSAL_WIDTH + 4] = ps32Score[u32ScoreBias + j];
            ps32ResultROI[*pu32ResultROICnt * SVP_WK_PROPOSAL_WIDTH + 5] = RPN_SUPPRESS_FALSE;
            *pu32ResultROICnt = *pu32ResultROICnt + 1;
        }
        u32RoiNumBias += ps32ClassRoiNum[i];
    }
    return HI_SUCCESS;
}

HI_S32 SAMPLE_Ssd_GetResult(HI_RUNTIME_BLOB_S* pstSrcBlob, HI_RUNTIME_BLOB_S* pstDstBlob,
                        HI_S32* ps32ResultROI, HI_U32* pu32ResultROICnt, const HI_U32 u32Length) {
    if(0 == u32Length)
    {
        SAMPLE_LOG_INFO("pstDstBlob is empty! ");
        return HI_FAILURE;
    }
    HI_SAMPLE_SSD_PARAM_S ssdParam;
    HI_S32* aps32PermuteResult[SAMPLE_SSD_REPORT_NODE_NUM];
    HI_S32* aps32PriorboxOutputData[SAMPLE_SSD_PRIORBOX_NUM];
    HI_S32* aps32SoftMaxInputData[SAMPLE_SSD_SOFTMAX_NUM];
    HI_S32* aps32DetectionLocData[SAMPLE_SSD_SOFTMAX_NUM];
    HI_S32* ps32SoftMaxOutputData = NULL;
    HI_S32* ps32DetectionOutTmpBuf = NULL;
    HI_U32 u32Size = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0;
    HI_FLOAT f32PrintResultThresh = 0.8f;

    memset(&ssdParam, 0, sizeof(ssdParam));

    s32Ret = SAMPLE_Ssd_InitParam(pstSrcBlob, pstDstBlob, &ssdParam);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_Ssd_InitParam failed!\n");
    /*get permut result*/
    for(i = 0; i < SAMPLE_SSD_REPORT_NODE_NUM; i++)
    {
        aps32PermuteResult[i] = (HI_S32*)((uintptr_t)pstDstBlob[i].u64VirAddr);
    }

    /*priorbox*/
    aps32PriorboxOutputData[0] = (HI_S32*)((uintptr_t)ssdParam.stPriorBoxTmpBuf.u64VirAddr);
    for (i = 1; i < SAMPLE_SSD_PRIORBOX_NUM; i++)
    {
        u32Size = ssdParam.au32PriorBoxHeight[i-1]*ssdParam.au32PriorBoxWidth[i-1]*
            SVP_WK_COORDI_NUM*2*(ssdParam.u32MaxSizeNum+ssdParam.u32MinSizeNum+
            ssdParam.au32InputAspectRatioNum[i-1]*2*ssdParam.u32MinSizeNum);
        aps32PriorboxOutputData[i] = aps32PriorboxOutputData[i - 1] + u32Size;
    }

    for (i = 0; i < SAMPLE_SSD_PRIORBOX_NUM; i++)
    {
        s32Ret = SAMPLE_Ssd_PriorBoxForward(ssdParam.au32PriorBoxWidth[i],
            ssdParam.au32PriorBoxHeight[i], ssdParam.u32OriImWidth,
            ssdParam.u32OriImHeight, ssdParam.af32PriorBoxMinSize[i],
            ssdParam.u32MinSizeNum,ssdParam.af32PriorBoxMaxSize[i],
            ssdParam.u32MaxSizeNum, ssdParam.bFlip, ssdParam.bClip,
            ssdParam.au32InputAspectRatioNum[i],ssdParam.af32PriorBoxAspectRatio[i],
            ssdParam.af32PriorBoxStepWidth[i],ssdParam.af32PriorBoxStepHeight[i],
            ssdParam.f32Offset,ssdParam.as32PriorBoxVar,
            aps32PriorboxOutputData[i],
            sizeof(ssdParam.as32PriorBoxVar));
        SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "Error,SAMPLE_Ssd_PriorBoxForward failed!\n");
    }

    /*softmax*/
    ps32SoftMaxOutputData = (HI_S32*)((uintptr_t)ssdParam.stSoftMaxTmpBuf.u64VirAddr);
    for(i = 0; i < SAMPLE_SSD_SOFTMAX_NUM; i++)
    {
        aps32SoftMaxInputData[i] = aps32PermuteResult[i*2+1];
    }

    (void)SAMPLE_Ssd_SoftmaxForward(ssdParam.u32SoftMaxInHeight,
        ssdParam.au32SoftMaxInChn, ssdParam.u32ConcatNum,
        ssdParam.au32ConvStride, ssdParam.u32SoftMaxOutWidth,
        ssdParam.u32SoftMaxOutHeight, ssdParam.u32SoftMaxOutChn,
        aps32SoftMaxInputData, ps32SoftMaxOutputData);

    /*detection*/
    ps32DetectionOutTmpBuf = (HI_S32*)((uintptr_t)ssdParam.stGetResultTmpBuf.u64VirAddr);
    for(i = 0; i < SAMPLE_SSD_PRIORBOX_NUM; i++)
    {
        aps32DetectionLocData[i] = aps32PermuteResult[i*2];
    }

    (void)SAMPLE_Ssd_DetectionOutForward(ssdParam.u32ConcatNum,
        ssdParam.u32ConfThresh,ssdParam.u32ClassNum, ssdParam.u32TopK,
        ssdParam.u32KeepTopK, ssdParam.u32NmsThresh,ssdParam.au32DetectInputChn,
        aps32DetectionLocData, aps32PriorboxOutputData, ps32SoftMaxOutputData,
        ps32DetectionOutTmpBuf, (HI_S32*)((uintptr_t)ssdParam.stDstScore.u64VirAddr),
        (HI_S32*)((uintptr_t)ssdParam.stDstRoi.u64VirAddr),
        (HI_S32*)((uintptr_t)ssdParam.stClassRoiNum.u64VirAddr));

    (void)SAMPLE_SSD_Detection_PrintResult(&ssdParam.stDstScore,
        &ssdParam.stDstRoi, &ssdParam.stClassRoiNum,f32PrintResultThresh, ps32ResultROI, pu32ResultROICnt);

FAIL_0:
    SAMPLE_FreeMem(&ssdParam.stPriorBoxTmpBuf);
    return s32Ret;
}
