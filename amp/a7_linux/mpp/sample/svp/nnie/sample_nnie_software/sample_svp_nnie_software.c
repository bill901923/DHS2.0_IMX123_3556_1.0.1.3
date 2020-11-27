#include"sample_svp_nnie_software.h"
#include <math.h>

#ifdef __cplusplus    // If used by C++ code,
extern "C" {          // we need to export the C interface
#endif



static HI_FLOAT s_af32ExpCoef[10][16] = {
    {1.0f, 1.00024f, 1.00049f, 1.00073f, 1.00098f, 1.00122f, 1.00147f, 1.00171f, 1.00196f, 1.0022f, 1.00244f, 1.00269f, 1.00293f, 1.00318f, 1.00342f, 1.00367f},
    {1.0f, 1.00391f, 1.00784f, 1.01179f, 1.01575f, 1.01972f, 1.02371f, 1.02772f, 1.03174f, 1.03578f, 1.03984f, 1.04391f, 1.04799f, 1.05209f, 1.05621f, 1.06034f},
    {1.0f, 1.06449f, 1.13315f, 1.20623f, 1.28403f, 1.36684f, 1.45499f, 1.54883f, 1.64872f, 1.75505f, 1.86825f, 1.98874f, 2.117f, 2.25353f, 2.39888f, 2.55359f},
    {1.0f, 2.71828f, 7.38906f, 20.0855f, 54.5981f, 148.413f, 403.429f, 1096.63f, 2980.96f, 8103.08f, 22026.5f, 59874.1f, 162755.0f, 442413.0f, 1.2026e+006f, 3.26902e+006f},
    {1.0f, 8.88611e+006f, 7.8963e+013f, 7.01674e+020f, 6.23515e+027f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f, 5.54062e+034f},
    {1.0f, 0.999756f, 0.999512f, 0.999268f, 0.999024f, 0.99878f, 0.998536f, 0.998292f, 0.998049f, 0.997805f, 0.997562f, 0.997318f, 0.997075f, 0.996831f, 0.996588f, 0.996345f},
    {1.0f, 0.996101f, 0.992218f, 0.98835f, 0.984496f, 0.980658f, 0.976835f, 0.973027f, 0.969233f, 0.965455f, 0.961691f, 0.957941f, 0.954207f, 0.950487f, 0.946781f, 0.94309f},
    {1.0f, 0.939413f, 0.882497f, 0.829029f, 0.778801f, 0.731616f, 0.687289f, 0.645649f, 0.606531f, 0.569783f, 0.535261f, 0.502832f, 0.472367f, 0.443747f, 0.416862f, 0.391606f},
    {1.0f, 0.367879f, 0.135335f, 0.0497871f, 0.0183156f, 0.00673795f, 0.00247875f, 0.000911882f, 0.000335463f, 0.00012341f, 4.53999e-005f, 1.67017e-005f, 6.14421e-006f, 2.26033e-006f, 8.31529e-007f, 3.05902e-007f},
    {1.0f, 1.12535e-007f, 1.26642e-014f, 1.42516e-021f, 1.60381e-028f, 1.80485e-035f, 2.03048e-042f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
};

/*****************************************************************************
* Prototype :   SVP_NNIE_QuickExp
* Description : this function is used to quickly get exp result
* Input :     HI_S32    s32Value           [IN]   input value
*
*
*
*
* Output :
* Return Value : HI_FLOAT: output value.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_FLOAT SVP_NNIE_QuickExp( HI_S32 s32Value )
{
    if( s32Value & 0x80000000 )
    {
        s32Value = ~s32Value + 0x00000001;
        return s_af32ExpCoef[5][s32Value & 0x0000000F] * s_af32ExpCoef[6][(s32Value>>4) & 0x0000000F] * s_af32ExpCoef[7][(s32Value>>8) & 0x0000000F] * s_af32ExpCoef[8][(s32Value>>12) & 0x0000000F] * s_af32ExpCoef[9][(s32Value>>16) & 0x0000000F ];
    }
    else
    {
        return s_af32ExpCoef[0][s32Value & 0x0000000F] * s_af32ExpCoef[1][(s32Value>>4) & 0x0000000F] * s_af32ExpCoef[2][(s32Value>>8) & 0x0000000F] * s_af32ExpCoef[3][(s32Value>>12) & 0x0000000F] * s_af32ExpCoef[4][(s32Value>>16) & 0x0000000F ];
    }
}

/*****************************************************************************
* Prototype :   SVP_NNIE_SoftMax
* Description : this function is used to do softmax
* Input :     HI_FLOAT*         pf32Src           [IN]   the pointer to source data
*             HI_U32             u32Num           [IN]   the num of source data
*
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_SoftMax( HI_FLOAT* pf32Src, HI_U32 u32Num)
{
    HI_FLOAT f32Max = 0;
    HI_FLOAT f32Sum = 0;
    HI_U32 i = 0;

    for(i = 0; i < u32Num; ++i)
    {
        if(f32Max < pf32Src[i])
        {
            f32Max = pf32Src[i];
        }
    }

    for(i = 0; i < u32Num; ++i)
    {
        pf32Src[i] = (HI_FLOAT)SVP_NNIE_QuickExp((HI_S32)((pf32Src[i] - f32Max)*SAMPLE_SVP_NNIE_QUANT_BASE));
        f32Sum += pf32Src[i];
    }

    for(i = 0; i < u32Num; ++i)
    {
        pf32Src[i] /= f32Sum;
    }
    return HI_SUCCESS;
}




/*****************************************************************************
* Prototype :   SVP_NNIE_Argswap
* Description : this function is used to exchange array data
* Input :       HI_FLOAT*           pf32Src1          [IN]   the pointer to the first array
*               HI_S32*             ps32Src2          [OUT]  the pointer to the second array
*
*
*
*
* Output :
* Return Value : void
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-03-10
* Author :
* Modification : Create
*
*****************************************************************************/
static void SVP_NNIE_Argswap(HI_S32* ps32Src1, HI_S32* ps32Src2)
{
    HI_U32 i = 0;
    HI_S32 u32Tmp = 0;
    for( i = 0; i < SAMPLE_SVP_NNIE_PROPOSAL_WIDTH; i++ )
    {
        u32Tmp = ps32Src1[i];
        ps32Src1[i] = ps32Src2[i];
        ps32Src2[i] = u32Tmp;
    }
}


/*****************************************************************************
* Prototype :   SVP_NNIE_NonRecursiveArgQuickSort
* Description : this function is used to do quick sort
* Input :       HI_S32*             ps32Array         [IN]   the array need to be sorted
*               HI_S32              s32Low            [IN]   the start position of quick sort
*               HI_S32              s32High           [IN]   the end position of quick sort
*               SAMPLE_SVP_NNIE_STACK_S *  pstStack   [IN]   the buffer used to store start positions and end positions
*
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-03-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_NonRecursiveArgQuickSort(HI_S32* ps32Array,
    HI_S32 s32Low, HI_S32 s32High, SAMPLE_SVP_NNIE_STACK_S *pstStack,HI_U32 u32MaxNum)
{
    HI_S32 i = s32Low;
    HI_S32 j = s32High;
    HI_S32 s32Top = 0;
    HI_S32 s32KeyConfidence = ps32Array[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * s32Low + 4];
    pstStack[s32Top].s32Min = s32Low;
    pstStack[s32Top].s32Max = s32High;

    while(s32Top > -1)
    {
        s32Low = pstStack[s32Top].s32Min;
        s32High = pstStack[s32Top].s32Max;
        i = s32Low;
        j = s32High;
        s32Top--;

        s32KeyConfidence = ps32Array[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * s32Low + 4];

        while(i < j)
        {
            while((i < j) && (s32KeyConfidence > ps32Array[j * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 4]))
            {
                j--;
            }
            if(i < j)
            {
                SVP_NNIE_Argswap(&ps32Array[i*SAMPLE_SVP_NNIE_PROPOSAL_WIDTH], &ps32Array[j*SAMPLE_SVP_NNIE_PROPOSAL_WIDTH]);
                i++;
            }

            while((i < j) && (s32KeyConfidence < ps32Array[i*SAMPLE_SVP_NNIE_PROPOSAL_WIDTH + 4]))
            {
                i++;
            }
            if(i < j)
            {
                SVP_NNIE_Argswap(&ps32Array[i*SAMPLE_SVP_NNIE_PROPOSAL_WIDTH], &ps32Array[j*SAMPLE_SVP_NNIE_PROPOSAL_WIDTH]);
                j--;
            }
        }

        if(s32Low <= u32MaxNum)
        {
                if(s32Low < i-1)
                {
                    s32Top++;
                    pstStack[s32Top].s32Min = s32Low;
                    pstStack[s32Top].s32Max = i-1;
                }

                if(s32High > i+1)
                {
                    s32Top++;
                    pstStack[s32Top].s32Min = i+1;
                    pstStack[s32Top].s32Max = s32High;
                }
        }
    }
    return HI_SUCCESS;
}


/*****************************************************************************
* Prototype :   SVP_NNIE_Overlap
* Description : this function is used to calculate the overlap ratio of two proposals
* Input :     HI_S32              s32XMin1          [IN]   first input proposal's minimum value of x coordinate
*               HI_S32              s32YMin1          [IN]   first input proposal's minimum value of y coordinate of first input proposal
*               HI_S32              s32XMax1          [IN]   first input proposal's maximum value of x coordinate of first input proposal
*               HI_S32              s32YMax1          [IN]   first input proposal's maximum value of y coordinate of first input proposal
*               HI_S32              s32XMin1          [IN]   second input proposal's minimum value of x coordinate
*               HI_S32              s32YMin1          [IN]   second input proposal's minimum value of y coordinate of first input proposal
*               HI_S32              s32XMax1          [IN]   second input proposal's maximum value of x coordinate of first input proposal
*               HI_S32              s32YMax1          [IN]   second input proposal's maximum value of y coordinate of first input proposal
*             HI_FLOAT            *pf32IoU          [INOUT]the pointer of the IoU value
*
*
* Output :
* Return Value : HI_FLOAT f32Iou.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-03-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_Overlap(HI_S32 s32XMin1, HI_S32 s32YMin1, HI_S32 s32XMax1, HI_S32 s32YMax1, HI_S32 s32XMin2,
    HI_S32 s32YMin2, HI_S32 s32XMax2, HI_S32 s32YMax2,  HI_S32* s32AreaSum, HI_S32* s32AreaInter)
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

    s32XMin = SAMPLE_SVP_NNIE_MAX(s32XMin1, s32XMin2);
    s32YMin = SAMPLE_SVP_NNIE_MAX(s32YMin1, s32YMin2);
    s32XMax = SAMPLE_SVP_NNIE_MIN(s32XMax1, s32XMax2);
    s32YMax = SAMPLE_SVP_NNIE_MIN(s32YMax1, s32YMax2);

    s32InterWidth = s32XMax - s32XMin + 1;
    s32InterHeight = s32YMax - s32YMin + 1;

    s32InterWidth = ( s32InterWidth >= 0 ) ? s32InterWidth : 0;
    s32InterHeight = ( s32InterHeight >= 0 ) ? s32InterHeight : 0;

    s32Inter = s32InterWidth * s32InterHeight;
    s32Area1 = (s32XMax1 - s32XMin1 + 1) * (s32YMax1 - s32YMin1 + 1);
    s32Area2 = (s32XMax2 - s32XMin2 + 1) * (s32YMax2 - s32YMin2 + 1);

    s32Total = s32Area1 + s32Area2 - s32Inter;

    *s32AreaSum = s32Total;
    *s32AreaInter = s32Inter;
    return HI_SUCCESS;
}

/*****************************************************************************
* Prototype :   SVP_NNIE_FilterLowScoreBbox
* Description : this function is used to remove low score bboxes, in order to speed-up Sort & RPN procedures.
* Input :      HI_S32*         ps32Proposals     [IN]   proposals
*              HI_U32          u32NumAnchors     [IN]   input anchors' num
*              HI_U32          u32FilterThresh   [IN]   rpn configuration
*              HI_U32*         u32NumAfterFilter [OUT]  output num of anchors after low score filtering
*
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-03-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_FilterLowScoreBbox(HI_S32* ps32Proposals, HI_U32 u32AnchorsNum,
    HI_U32 u32FilterThresh, HI_U32* u32NumAfterFilter)
{
    HI_U32 u32ProposalCnt = u32AnchorsNum;
    HI_U32 i = 0;

    if( u32FilterThresh > 0 )
    {
        for( i = 0; i < u32AnchorsNum; i++ )
        {
            if( ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 4 ] < (HI_S32)u32FilterThresh )
            {
                ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 5 ] = 1;
            }
        }

        u32ProposalCnt = 0;
        for( i = 0; i < u32AnchorsNum; i++ )
        {
            if( 0 == ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 5 ] )
            {
                ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32ProposalCnt ] = ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i ];
                ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32ProposalCnt + 1 ] = ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 1 ];
                ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32ProposalCnt + 2 ] = ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 2 ];
                ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32ProposalCnt + 3 ] = ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 3 ];
                ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32ProposalCnt + 4 ] = ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 4 ];
                ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32ProposalCnt + 5 ] = ps32Proposals[ SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i + 5 ];
                u32ProposalCnt++;
            }
        }
    }
    *u32NumAfterFilter = u32ProposalCnt;
    return HI_SUCCESS;
}
/*****************************************************************************
* Prototype :   SVP_NNIE_NonMaxSuppression
* Description : this function is used to do non maximum suppression
* Input :       HI_S32*           ps32Proposals     [IN]   proposals
*               HI_U32            u32AnchorsNum     [IN]   anchors num
*               HI_U32            u32NmsThresh      [IN]   non maximum suppression threshold
*               HI_U32            u32MaxRoiNum      [IN]  The max roi num for the roi pooling
*
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-03-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_NonMaxSuppression( HI_S32* ps32Proposals, HI_U32 u32AnchorsNum,
    HI_U32 u32NmsThresh,HI_U32 u32MaxRoiNum)
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
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 u32Num = 0;
    HI_BOOL bNoOverlap  = HI_TRUE;
    for (i = 0; i < u32AnchorsNum && u32Num < u32MaxRoiNum; i++)
    {
        if( ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i+5] == 0 )
        {
            u32Num++;
            s32XMin1 =  ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i];
            s32YMin1 =  ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i+1];
            s32XMax1 =  ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i+2];
            s32YMax1 =  ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i+3];
            for(j= i+1;j< u32AnchorsNum; j++)
            {
                if( ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j+5] == 0 )
                {
                    s32XMin2 = ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j];
                    s32YMin2 = ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j+1];
                    s32XMax2 = ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j+2];
                    s32YMax2 = ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j+3];
                    bNoOverlap = (s32XMin2>s32XMax1)||(s32XMax2<s32XMin1)||(s32YMin2>s32YMax1)||(s32YMax2<s32YMin1);
                    if(bNoOverlap)
                    {
                        continue;
                    }
                    (void)SVP_NNIE_Overlap(s32XMin1, s32YMin1, s32XMax1, s32YMax1, s32XMin2, s32YMin2, s32XMax2, s32YMax2, &s32AreaTotal, &s32AreaInter);
                    if(s32AreaInter*SAMPLE_SVP_NNIE_QUANT_BASE > ((HI_S32)u32NmsThresh*s32AreaTotal))
                    {
                        if( ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i+4] >= ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j+4] )
                        {
                            ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*j+5] = 1;
                        }
                        else
                        {
                            ps32Proposals[SAMPLE_SVP_NNIE_PROPOSAL_WIDTH*i+5] = 1;
                        }
                    }
                }
            }
        }
    }
    return HI_SUCCESS;
}

/*****************************************************************************
* Prototype :   SVP_NNIE_Rpn
* Description : this function is used to do RPN
* Input :     HI_S32** pps32Src              [IN] convolution data
*             HI_U32 u32NumRatioAnchors      [IN] Ratio anchor num
*             HI_U32 u32NumScaleAnchors      [IN] scale anchor num
*             HI_U32* au32Scales             [IN] scale value
*             HI_U32* au32Ratios             [IN] ratio value
*             HI_U32 u32OriImHeight          [IN] input image height
*             HI_U32 u32OriImWidth           [IN] input image width
*             HI_U32* pu32ConvHeight         [IN] convolution height
*             HI_U32* pu32ConvWidth          [IN] convolution width
*             HI_U32* pu32ConvChannel        [IN] convolution channel
*             HI_U32  u32ConvStride          [IN] convolution stride
*             HI_U32 u32MaxRois              [IN] max roi num
*             HI_U32 u32MinSize              [IN] min size
*             HI_U32 u32SpatialScale         [IN] spatial scale
*             HI_U32 u32NmsThresh            [IN] NMS thresh
*             HI_U32 u32FilterThresh         [IN] filter thresh
*             HI_U32 u32NumBeforeNms         [IN] num before doing NMS
*             HI_U32 *pu32MemPool            [IN] assist buffer
*             HI_S32 *ps32ProposalResult     [OUT] proposal result
*             HI_U32* pu32NumRois            [OUT] proposal num
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_Rpn(HI_S32** pps32Src,HI_U32 u32NumRatioAnchors,
    HI_U32 u32NumScaleAnchors,HI_U32* au32Scales,HI_U32* au32Ratios,HI_U32 u32OriImHeight,
    HI_U32 u32OriImWidth,HI_U32* pu32ConvHeight,HI_U32* pu32ConvWidth,HI_U32* pu32ConvChannel,
    HI_U32  u32ConvStride,HI_U32 u32MaxRois,HI_U32 u32MinSize,HI_U32 u32SpatialScale,
    HI_U32 u32NmsThresh,HI_U32 u32FilterThresh,HI_U32 u32NumBeforeNms,HI_U32 *pu32MemPool,
    HI_S32 *ps32ProposalResult,HI_U32* pu32NumRois)
{
    /******************** define parameters ****************/
    HI_U32 u32Size = 0;
    HI_S32* ps32Anchors = NULL;
    HI_S32* ps32BboxDelta = NULL;
    HI_S32* ps32Proposals = NULL;
    HI_U32* pu32Ptr = NULL;
    HI_S32* ps32Ptr = NULL;
    HI_U32 u32NumAfterFilter = 0;
    HI_U32 u32NumAnchors = 0;
    HI_FLOAT f32BaseW = 0;
    HI_FLOAT f32BaseH = 0;
    HI_FLOAT f32BaseXCtr = 0;
    HI_FLOAT f32BaseYCtr = 0;
    HI_FLOAT f32SizeRatios = 0;
    HI_FLOAT* pf32RatioAnchors = NULL;
    HI_FLOAT* pf32Ptr = NULL;
    HI_FLOAT *pf32Ptr2 = NULL;
    HI_FLOAT* pf32ScaleAnchors = NULL;
    HI_FLOAT* pf32Scores = NULL;
    HI_FLOAT f32Ratios = 0;
    HI_FLOAT f32Size = 0;
    HI_U32 u32PixelInterval = 0;
    HI_U32 u32SrcBboxIndex = 0;
    HI_U32 u32SrcFgProbIndex = 0;
    HI_U32 u32SrcBgProbIndex = 0;
    HI_U32 u32SrcBboxBias = 0;
    HI_U32 u32SrcProbBias = 0;
    HI_U32 u32DesBox = 0;
    HI_U32 u32BgBlobSize = 0;
    HI_U32 u32AnchorsPerPixel = 0;
    HI_U32 u32MapSize = 0;
    HI_U32 u32LineSize = 0;
    HI_S32* ps32Ptr2 = NULL;
    HI_S32* ps32Ptr3 = NULL;
    HI_S32 s32ProposalWidth = 0;
    HI_S32 s32ProposalHeight = 0;
    HI_S32 s32ProposalCenterX = 0;
    HI_S32 s32ProposalCenterY = 0;
    HI_S32 s32PredW = 0;
    HI_S32 s32PredH = 0;
    HI_S32 s32PredCenterX = 0;
    HI_S32 s32PredCenterY = 0;
    HI_U32 u32DesBboxDeltaIndex = 0;
    HI_U32 u32DesScoreIndex = 0;
    HI_U32 u32RoiCount = 0;
    SAMPLE_SVP_NNIE_STACK_S* pstStack = NULL;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 c = 0;
    HI_U32 h = 0;
    HI_U32 w = 0;
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 p = 0;
    HI_U32 q = 0;
    HI_U32 z = 0;
    HI_U32 au32BaseAnchor[4] = {0, 0, (u32MinSize -1), (u32MinSize -1)};

    /*********************************** Faster RCNN *********************************************/
    /********* calculate the start pointer of each part in MemPool *********/
    pu32Ptr = (HI_U32*)pu32MemPool;
    ps32Anchors = (HI_S32*)pu32Ptr;
    u32NumAnchors = u32NumRatioAnchors * u32NumScaleAnchors * ( pu32ConvHeight[0] * pu32ConvWidth[0] );
    u32Size = SAMPLE_SVP_NNIE_COORDI_NUM * u32NumAnchors;
    pu32Ptr += u32Size;

    ps32BboxDelta = (HI_S32*)pu32Ptr;
    pu32Ptr += u32Size;

    ps32Proposals = (HI_S32*)pu32Ptr;
    u32Size = SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32NumAnchors;
    pu32Ptr += u32Size;

    pf32RatioAnchors = (HI_FLOAT*)pu32Ptr;
    pf32Ptr = (HI_FLOAT*)pu32Ptr;
    u32Size = u32NumRatioAnchors * SAMPLE_SVP_NNIE_COORDI_NUM;
    pf32Ptr = pf32Ptr + u32Size;

    pf32ScaleAnchors = pf32Ptr;
    u32Size = u32NumScaleAnchors * u32NumRatioAnchors * SAMPLE_SVP_NNIE_COORDI_NUM;
    pf32Ptr = pf32Ptr + u32Size;

    pf32Scores = pf32Ptr;
    u32Size = u32NumAnchors * SAMPLE_SVP_NNIE_SCORE_NUM;
    pf32Ptr = pf32Ptr + u32Size;

    pstStack = (SAMPLE_SVP_NNIE_STACK_S*)pf32Ptr;

    /********************* Generate the base anchor ***********************/
    f32BaseW = (HI_FLOAT)(au32BaseAnchor[2] - au32BaseAnchor[0] + 1 );
    f32BaseH = (HI_FLOAT)(au32BaseAnchor[3] - au32BaseAnchor[1] + 1 );
    f32BaseXCtr = (HI_FLOAT)(au32BaseAnchor[0] + ( ( f32BaseW - 1 ) * 0.5 ) );
    f32BaseYCtr = (HI_FLOAT)(au32BaseAnchor[1] + ( ( f32BaseH - 1 ) * 0.5 ) );

    /*************** Generate Ratio Anchors for the base anchor ***********/
    pf32Ptr = pf32RatioAnchors;
    f32Size = f32BaseW * f32BaseH;
    for (i = 0; i < u32NumRatioAnchors; i++)
    {
        f32Ratios = (HI_FLOAT)au32Ratios[i]/SAMPLE_SVP_NNIE_QUANT_BASE;
        f32SizeRatios = f32Size / f32Ratios;
        f32BaseW = sqrt(f32SizeRatios);
        f32BaseW = (HI_FLOAT)(1.0 * ( (f32BaseW) >= 0 ? (HI_S32)(f32BaseW+SAMPLE_SVP_NNIE_HALF) : (HI_S32)(f32BaseW-SAMPLE_SVP_NNIE_HALF)));
        f32BaseH = f32BaseW * f32Ratios;
        f32BaseH = (HI_FLOAT)(1.0 * ( (f32BaseH) >= 0 ? (HI_S32)(f32BaseH+SAMPLE_SVP_NNIE_HALF) : (HI_S32)(f32BaseH-SAMPLE_SVP_NNIE_HALF)));

        *pf32Ptr++ = (HI_FLOAT)(f32BaseXCtr - ( ( f32BaseW - 1 ) * SAMPLE_SVP_NNIE_HALF ));
        *(pf32Ptr++) = (HI_FLOAT)(f32BaseYCtr - ( ( f32BaseH - 1 ) * SAMPLE_SVP_NNIE_HALF ));
        *(pf32Ptr++) = (HI_FLOAT)(f32BaseXCtr + ( ( f32BaseW - 1 ) * SAMPLE_SVP_NNIE_HALF ));
        *(pf32Ptr++) =  (HI_FLOAT)( f32BaseYCtr + ( ( f32BaseH - 1 ) * SAMPLE_SVP_NNIE_HALF ));
    }

    /********* Generate Scale Anchors for each Ratio Anchor **********/
    pf32Ptr = pf32RatioAnchors;
    pf32Ptr2 = pf32ScaleAnchors;
    /* Generate Scale Anchors for one pixel */
    for( i = 0; i < u32NumRatioAnchors; i++ )
    {
        for( j = 0; j < u32NumScaleAnchors; j++ )
        {
            f32BaseW = *( pf32Ptr + 2 ) - *( pf32Ptr ) + 1;
            f32BaseH = *( pf32Ptr + 3 ) - *( pf32Ptr + 1 ) + 1;
            f32BaseXCtr = (HI_FLOAT)( *( pf32Ptr ) + ( ( f32BaseW - 1 ) * SAMPLE_SVP_NNIE_HALF ));
            f32BaseYCtr = (HI_FLOAT)( *( pf32Ptr + 1 ) + ( ( f32BaseH - 1 ) * SAMPLE_SVP_NNIE_HALF ));

            *( pf32Ptr2++ ) = (HI_FLOAT) (f32BaseXCtr - ((f32BaseW * ((HI_FLOAT)au32Scales[j]/SAMPLE_SVP_NNIE_QUANT_BASE) - 1) * SAMPLE_SVP_NNIE_HALF));
            *( pf32Ptr2++ ) = (HI_FLOAT)(f32BaseYCtr - ((f32BaseH * ((HI_FLOAT)au32Scales[j]/SAMPLE_SVP_NNIE_QUANT_BASE) - 1) * SAMPLE_SVP_NNIE_HALF));
            *( pf32Ptr2++ ) = (HI_FLOAT)(f32BaseXCtr + ((f32BaseW * ((HI_FLOAT)au32Scales[j]/SAMPLE_SVP_NNIE_QUANT_BASE) - 1) * SAMPLE_SVP_NNIE_HALF));
            *( pf32Ptr2++ ) = (HI_FLOAT)(f32BaseYCtr + ((f32BaseH * ((HI_FLOAT)au32Scales[j]/SAMPLE_SVP_NNIE_QUANT_BASE) - 1) * SAMPLE_SVP_NNIE_HALF));
        }
            pf32Ptr += SAMPLE_SVP_NNIE_COORDI_NUM;
    }


    /******************* Copy the anchors to every pixel in the feature map ******************/
    ps32Ptr = ps32Anchors;
    u32PixelInterval = SAMPLE_SVP_NNIE_QUANT_BASE/ u32SpatialScale;

    for ( p = 0; p < pu32ConvHeight[0]; p++  )
    {
        for ( q = 0; q < pu32ConvWidth[0]; q++ )
        {
            pf32Ptr2 = pf32ScaleAnchors;
            for ( z = 0 ; z < u32NumScaleAnchors * u32NumRatioAnchors; z++ )
            {
                *(ps32Ptr++) = (HI_S32)(q * u32PixelInterval + *(pf32Ptr2++) );
                *(ps32Ptr++) = (HI_S32)( p * u32PixelInterval + *( pf32Ptr2++ ));
                *(ps32Ptr++) = (HI_S32)( q * u32PixelInterval + *( pf32Ptr2++ ));
                *(ps32Ptr++) = (HI_S32)( p * u32PixelInterval + *( pf32Ptr2++ ));
            }
        }
    }

    /********** do transpose, convert the blob from (M,C,H,W) to (M,H,W,C) **********/
    u32MapSize = pu32ConvHeight[1] * u32ConvStride / sizeof(HI_U32);
    u32AnchorsPerPixel = u32NumRatioAnchors * u32NumScaleAnchors;
    u32BgBlobSize = u32AnchorsPerPixel * u32MapSize;
    u32LineSize = u32ConvStride / sizeof(HI_U32);
    u32SrcProbBias = 0;
    u32SrcBboxBias = 0;

    for ( c = 0; c < pu32ConvChannel[1]; c++ )
    {
        for ( h = 0; h < pu32ConvHeight[1]; h++ )
        {
            for ( w = 0; w < pu32ConvWidth[1]; w++ )
            {
                u32SrcBboxIndex = u32SrcBboxBias + c * u32MapSize + h * u32LineSize + w;
                u32SrcBgProbIndex = u32SrcProbBias + (c/SAMPLE_SVP_NNIE_COORDI_NUM) * u32MapSize + h * u32LineSize + w;
                u32SrcFgProbIndex = u32BgBlobSize + u32SrcBgProbIndex;

                u32DesBox = ( u32AnchorsPerPixel ) * ( h * pu32ConvWidth[1] + w) + c/SAMPLE_SVP_NNIE_COORDI_NUM ;

                u32DesBboxDeltaIndex = SAMPLE_SVP_NNIE_COORDI_NUM * u32DesBox + c % SAMPLE_SVP_NNIE_COORDI_NUM;
                ps32BboxDelta[u32DesBboxDeltaIndex] = (HI_S32)pps32Src[1][u32SrcBboxIndex];

                u32DesScoreIndex = ( SAMPLE_SVP_NNIE_SCORE_NUM ) * u32DesBox;
                pf32Scores[u32DesScoreIndex] = (HI_FLOAT)((HI_S32)pps32Src[0][u32SrcBgProbIndex]) / SAMPLE_SVP_NNIE_QUANT_BASE;
                pf32Scores[u32DesScoreIndex + 1] = (HI_FLOAT)((HI_S32)pps32Src[0][u32SrcFgProbIndex]) / SAMPLE_SVP_NNIE_QUANT_BASE;
            }
        }
    }


    /************************* do softmax ****************************/
    pf32Ptr = pf32Scores;
    for( i = 0; i<u32NumAnchors; i++ )
    {
        s32Ret = SVP_NNIE_SoftMax(pf32Ptr, SAMPLE_SVP_NNIE_SCORE_NUM);
        pf32Ptr += SAMPLE_SVP_NNIE_SCORE_NUM;
    }


    /************************* BBox Transform *****************************/
    /* use parameters from Conv3 to adjust the coordinates of anchors */
    ps32Ptr = ps32Anchors;
    ps32Ptr2 = ps32Proposals;
    ps32Ptr3 = ps32BboxDelta;
    for( i = 0; i < u32NumAnchors; i++)
    {
        ps32Ptr = ps32Anchors;
        ps32Ptr = ps32Ptr + SAMPLE_SVP_NNIE_COORDI_NUM * i;
        ps32Ptr2 = ps32Proposals;
        ps32Ptr2 = ps32Ptr2 + SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i;
        ps32Ptr3 = ps32BboxDelta;
        ps32Ptr3 = ps32Ptr3 + SAMPLE_SVP_NNIE_COORDI_NUM * i;
        pf32Ptr = pf32Scores;
        pf32Ptr = pf32Ptr + i* ( SAMPLE_SVP_NNIE_SCORE_NUM );

        s32ProposalWidth = *(ps32Ptr+2) - *(ps32Ptr) + 1;
        s32ProposalHeight = *(ps32Ptr+3) - *(ps32Ptr+1) + 1;
        s32ProposalCenterX = *(ps32Ptr) + (HI_S32)( s32ProposalWidth * SAMPLE_SVP_NNIE_HALF );
        s32ProposalCenterY = *(ps32Ptr+1) + (HI_S32)( s32ProposalHeight * SAMPLE_SVP_NNIE_HALF);
        s32PredCenterX = (HI_S32)( ((HI_FLOAT)(*(ps32Ptr3))/SAMPLE_SVP_NNIE_QUANT_BASE) * s32ProposalWidth + s32ProposalCenterX );
        s32PredCenterY = (HI_S32)( ((HI_FLOAT)(*(ps32Ptr3 + 1))/SAMPLE_SVP_NNIE_QUANT_BASE) * s32ProposalHeight + s32ProposalCenterY);

        s32PredW = (HI_S32)(s32ProposalWidth * SVP_NNIE_QuickExp((HI_S32)( *(ps32Ptr3+2) )));
        s32PredH = (HI_S32)(s32ProposalHeight * SVP_NNIE_QuickExp((HI_S32)( *(ps32Ptr3+3) )));
        *(ps32Ptr2) = (HI_S32)( s32PredCenterX - SAMPLE_SVP_NNIE_HALF * s32PredW);
        *(ps32Ptr2+1) = (HI_S32)( s32PredCenterY - SAMPLE_SVP_NNIE_HALF * s32PredH );
        *(ps32Ptr2+2) = (HI_S32)( s32PredCenterX + SAMPLE_SVP_NNIE_HALF * s32PredW );
        *(ps32Ptr2+3) = (HI_S32)( s32PredCenterY + SAMPLE_SVP_NNIE_HALF * s32PredH );
        *(ps32Ptr2+4) = (HI_S32)(*(pf32Ptr+1) * SAMPLE_SVP_NNIE_QUANT_BASE);
        *(ps32Ptr2+5) = 0;
    }


    /************************ clip bbox *****************************/
    for( i = 0; i < u32NumAnchors; i++)
    {
        ps32Ptr = ps32Proposals;
        ps32Ptr = ps32Ptr + SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i;
        *ps32Ptr = SAMPLE_SVP_NNIE_MAX(SAMPLE_SVP_NNIE_MIN(*ps32Ptr, (HI_S32)u32OriImWidth-1),0 );
        *(ps32Ptr+1) = SAMPLE_SVP_NNIE_MAX(SAMPLE_SVP_NNIE_MIN(*(ps32Ptr+1), (HI_S32)u32OriImHeight-1),0 );
        *(ps32Ptr+2) = SAMPLE_SVP_NNIE_MAX(SAMPLE_SVP_NNIE_MIN(*(ps32Ptr+2), (HI_S32)u32OriImWidth-1),0 );
        *(ps32Ptr+3) = SAMPLE_SVP_NNIE_MAX(SAMPLE_SVP_NNIE_MIN(*(ps32Ptr+3), (HI_S32)u32OriImHeight-1),0 );
    }

    /************ remove the bboxes which are too small *************/
    for( i = 0; i< u32NumAnchors; i++)
    {
        ps32Ptr = ps32Proposals;
        ps32Ptr = ps32Ptr + SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i;
        s32ProposalWidth = *(ps32Ptr+2)-*(ps32Ptr) + 1;
        s32ProposalHeight = *(ps32Ptr+3)-*(ps32Ptr+1) + 1;
        if(s32ProposalWidth < (HI_S32)u32MinSize  || s32ProposalHeight < (HI_S32)u32MinSize  )
        {
            *(ps32Ptr+5) = 1;
        }
    }

    /********** remove low score bboxes ************/
    (void)SVP_NNIE_FilterLowScoreBbox( ps32Proposals, u32NumAnchors, u32FilterThresh, &u32NumAfterFilter );


    /********** sort ***********/
    (void)SVP_NNIE_NonRecursiveArgQuickSort( ps32Proposals, 0, u32NumAfterFilter - 1, pstStack ,u32NumBeforeNms);
    u32NumAfterFilter = ( u32NumAfterFilter < u32NumBeforeNms ) ? u32NumAfterFilter : u32NumBeforeNms;

    /* do nms to remove highly overlapped bbox */
    (void)SVP_NNIE_NonMaxSuppression(ps32Proposals, u32NumAfterFilter, u32NmsThresh, u32MaxRois);     /* function NMS */

    /************** write the final result to output ***************/
    u32RoiCount = 0;
    for( i = 0; i < u32NumAfterFilter; i++)
    {
        ps32Ptr = ps32Proposals;
        ps32Ptr = ps32Ptr + SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i;
        if( *(ps32Ptr+5) == 0)
        {
            /*In this sample,the output Roi coordinates will be input in hardware,
            so the type coordinates are convert to HI_S20Q12*/
            ps32ProposalResult[SAMPLE_SVP_NNIE_COORDI_NUM * u32RoiCount] = *ps32Ptr*SAMPLE_SVP_NNIE_QUANT_BASE;
            ps32ProposalResult[SAMPLE_SVP_NNIE_COORDI_NUM * u32RoiCount + 1] = *(ps32Ptr+1)*SAMPLE_SVP_NNIE_QUANT_BASE;
            ps32ProposalResult[SAMPLE_SVP_NNIE_COORDI_NUM * u32RoiCount + 2] = *(ps32Ptr+2)*SAMPLE_SVP_NNIE_QUANT_BASE;
            ps32ProposalResult[SAMPLE_SVP_NNIE_COORDI_NUM * u32RoiCount + 3] = *(ps32Ptr+3)*SAMPLE_SVP_NNIE_QUANT_BASE;
            u32RoiCount++;
        }
        if(u32RoiCount >= u32MaxRois)
        {
            break;
        }
    }

    *pu32NumRois = u32RoiCount;

    return s32Ret;

}

/*****************************************************************************
* Prototype :   SVP_NNIE_FasterRcnn_GetResult
* Description : this function is used to get FasterRcnn result
* Input :     HI_S32* ps32FcBbox             [IN] Bbox for Roi
*             HI_S32 *ps32FcScore            [IN] Score for roi
*             HI_S32 *ps32Proposals          [IN] proposal
*             HI_U32 u32RoiCnt               [IN] Roi num
*             HI_U32 *pu32ConfThresh         [IN] each class confidence thresh
*             HI_U32 u32NmsThresh            [IN] Nms thresh
*             HI_U32 u32MaxRoi               [IN] max roi
*             HI_U32 u32ClassNum             [IN] class num
*             HI_U32 u32OriImWidth           [IN] input image width
*             HI_U32 u32OriImHeight          [IN] input image height
*             HI_U32* pu32MemPool            [IN] assist buffer
*             HI_S32* ps32DstScore           [OUT] result of score
*             HI_S32* ps32DstRoi             [OUT] result of Bbox
*             HI_S32* ps32ClassRoiNum        [OUT] result of the roi num of each classs
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
static HI_S32 SVP_NNIE_FasterRcnn_GetResult(HI_S32*ps32FcBbox,HI_U32 u32BboxStride,
    HI_S32*ps32FcScore, HI_U32 u32ScoreStride,HI_S32* ps32Proposal,HI_U32 u32RoiCnt,
    HI_U32* pu32ConfThresh,HI_U32 u32NmsThresh,HI_U32 u32MaxRoi,HI_U32 u32ClassNum,
    HI_U32 u32OriImWidth,HI_U32 u32OriImHeight,HI_U32* pu32MemPool,HI_S32* ps32DstScore,
    HI_S32* ps32DstBbox,HI_S32* ps32ClassRoiNum)
{
    /************* define variables *****************/
    HI_U32 u32Size = 0;
    HI_U32 u32ClsScoreChannels = 0;
    HI_S32* ps32Proposals = NULL;
    HI_U32 u32FcScoreWidth = 0;
    HI_U32 u32FcBboxWidth = 0;
    HI_FLOAT f32ProposalWidth = 0.0;
    HI_FLOAT f32ProposalHeight = 0.0;
    HI_FLOAT f32ProposalCenterX = 0.0;
    HI_FLOAT f32ProposalCenterY = 0.0;
    HI_FLOAT f32PredW = 0.0;
    HI_FLOAT f32PredH = 0.0;
    HI_FLOAT f32PredCenterX = 0.0;
    HI_FLOAT f32PredCenterY = 0.0;
    HI_FLOAT* pf32FcScoresMemPool = NULL;
    HI_S32* ps32ProposalMemPool = NULL;
    HI_S32* ps32ProposalTmp = NULL;
    HI_U32 u32FcBboxIndex = 0;
    HI_U32 u32ProposalMemPoolIndex = 0;
    HI_FLOAT* pf32Ptr = NULL;
    HI_S32* ps32Ptr = NULL;
    HI_S32* ps32Score = NULL;
    HI_S32* ps32Bbox = NULL;
    HI_S32* ps32RoiCnt = NULL;
    HI_U32 u32RoiOutCnt = 0;
    HI_U32 u32SrcIndex = 0;
    HI_U32 u32DstIndex = 0;
    HI_U32 i = 0;
    HI_U32 j = 0;
    HI_U32 k = 0;
    SAMPLE_SVP_NNIE_STACK_S* pstStack=NULL;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32OffSet = 0;

  /******************* Get or calculate parameters **********************/
    u32ClsScoreChannels = u32ClassNum;   /*channel num is equal to class size, cls_score class*/
    u32FcScoreWidth = u32ScoreStride / sizeof(HI_U32);
    u32FcBboxWidth = u32BboxStride / sizeof(HI_U32);

    /*************** Get Start Pointer of MemPool ******************/
    pf32FcScoresMemPool = (HI_FLOAT*)pu32MemPool;
    pf32Ptr = pf32FcScoresMemPool;
    u32Size = u32MaxRoi * u32ClsScoreChannels;
    pf32Ptr += u32Size;

    ps32ProposalMemPool = (HI_S32*)pf32Ptr;
    ps32Ptr = ps32ProposalMemPool;
    u32Size = u32MaxRoi * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH ;
    ps32Ptr += u32Size;
    pstStack = (SAMPLE_SVP_NNIE_STACK_S* )ps32Ptr;

    u32DstIndex = 0;

    for( i = 0; i < u32RoiCnt; i++ )
    {
        for( k = 0; k < u32ClsScoreChannels; k++ )
        {
            u32SrcIndex = i * u32FcScoreWidth + k;
            pf32FcScoresMemPool[u32DstIndex++] = (HI_FLOAT)((HI_S32)ps32FcScore[u32SrcIndex]) / SAMPLE_SVP_NNIE_QUANT_BASE;
        }
    }
    ps32Proposals = (HI_S32*)ps32Proposal;

    /************** bbox tranform ************/
    for(j = 0; j < u32ClsScoreChannels; j++)
    {
        for(i = 0; i < u32RoiCnt; i++)
        {
            f32ProposalWidth   = (HI_FLOAT)(ps32Proposals[SAMPLE_SVP_NNIE_COORDI_NUM*i + 2] - ps32Proposals[SAMPLE_SVP_NNIE_COORDI_NUM*i] + 1);
            f32ProposalHeight  = (HI_FLOAT)(ps32Proposals[SAMPLE_SVP_NNIE_COORDI_NUM*i + 3] - ps32Proposals[SAMPLE_SVP_NNIE_COORDI_NUM*i + 1] + 1);
            f32ProposalCenterX = (HI_FLOAT)(ps32Proposals[SAMPLE_SVP_NNIE_COORDI_NUM*i] + SAMPLE_SVP_NNIE_HALF * f32ProposalWidth);
            f32ProposalCenterY = (HI_FLOAT)(ps32Proposals[SAMPLE_SVP_NNIE_COORDI_NUM*i + 1] + SAMPLE_SVP_NNIE_HALF * f32ProposalHeight);

            u32FcBboxIndex = u32FcBboxWidth * i + SAMPLE_SVP_NNIE_COORDI_NUM * j;
            f32PredCenterX = ((HI_FLOAT)ps32FcBbox[u32FcBboxIndex]/SAMPLE_SVP_NNIE_QUANT_BASE) * f32ProposalWidth  + f32ProposalCenterX;
            f32PredCenterY = ((HI_FLOAT)ps32FcBbox[u32FcBboxIndex + 1]/SAMPLE_SVP_NNIE_QUANT_BASE) * f32ProposalHeight + f32ProposalCenterY;
            f32PredW = f32ProposalWidth * SVP_NNIE_QuickExp((HI_S32)( ps32FcBbox[u32FcBboxIndex+2] ));
            f32PredH = f32ProposalHeight * SVP_NNIE_QuickExp((HI_S32)( ps32FcBbox[u32FcBboxIndex+3] ));

            u32ProposalMemPoolIndex =  SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i;
            ps32ProposalMemPool[u32ProposalMemPoolIndex] = (HI_S32)(f32PredCenterX - SAMPLE_SVP_NNIE_HALF * f32PredW);
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 1] = (HI_S32)(f32PredCenterY - SAMPLE_SVP_NNIE_HALF * f32PredH);
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 2] = (HI_S32)(f32PredCenterX + SAMPLE_SVP_NNIE_HALF * f32PredW);
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 3] = (HI_S32)(f32PredCenterY + SAMPLE_SVP_NNIE_HALF * f32PredH);
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 4] = (HI_S32)( pf32FcScoresMemPool[u32ClsScoreChannels*i+j] * SAMPLE_SVP_NNIE_QUANT_BASE );
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 5] = 0;   /* suprressed flag */

        }

        /* clip bbox */
       for(i = 0; i < u32RoiCnt; i++)
        {
            u32ProposalMemPoolIndex = SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i;
            ps32ProposalMemPool[u32ProposalMemPoolIndex] = ( (ps32ProposalMemPool[u32ProposalMemPoolIndex]) > ((HI_S32)u32OriImWidth - 1) ? ((HI_S32)u32OriImWidth - 1):( ps32ProposalMemPool[u32ProposalMemPoolIndex] ) )>0?( (ps32ProposalMemPool[u32ProposalMemPoolIndex])>((HI_S32)u32OriImWidth)? (u32OriImWidth - 1):( ps32ProposalMemPool[u32ProposalMemPoolIndex] ) ):0;
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 1] = ( (ps32ProposalMemPool[u32ProposalMemPoolIndex + 1]) > ((HI_S32)u32OriImHeight - 1) ? ((HI_S32)u32OriImHeight - 1):( ps32ProposalMemPool[u32ProposalMemPoolIndex + 1] ) )>0?( (ps32ProposalMemPool[u32ProposalMemPoolIndex + 1])>((HI_S32)u32OriImHeight)? (u32OriImHeight - 1):(ps32ProposalMemPool[u32ProposalMemPoolIndex + 1] ) ):0;
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 2] = ( (ps32ProposalMemPool[u32ProposalMemPoolIndex + 2]) > ((HI_S32)u32OriImWidth - 1) ? ((HI_S32)u32OriImWidth - 1):( ps32ProposalMemPool[u32ProposalMemPoolIndex + 2] ) )>0?( (ps32ProposalMemPool[u32ProposalMemPoolIndex + 2])>((HI_S32)u32OriImWidth)? (u32OriImWidth - 1):( ps32ProposalMemPool[u32ProposalMemPoolIndex + 2] ) ):0;
            ps32ProposalMemPool[u32ProposalMemPoolIndex + 3] = ( (ps32ProposalMemPool[u32ProposalMemPoolIndex + 3]) > ((HI_S32)u32OriImHeight - 1) ? ((HI_S32)u32OriImHeight - 1):( ps32ProposalMemPool[u32ProposalMemPoolIndex + 3] ) )>0?( (ps32ProposalMemPool[u32ProposalMemPoolIndex + 3])>((HI_S32)u32OriImHeight)? (u32OriImHeight - 1):(ps32ProposalMemPool[u32ProposalMemPoolIndex + 3] ) ):0;
       }

        ps32ProposalTmp = ps32ProposalMemPool;

        (void)SVP_NNIE_NonRecursiveArgQuickSort( ps32ProposalTmp, 0, u32RoiCnt-1, pstStack,u32RoiCnt);

        (void)SVP_NNIE_NonMaxSuppression(ps32ProposalTmp, u32RoiCnt, u32NmsThresh, u32RoiCnt);

        ps32Score = (HI_S32*)ps32DstScore;
        ps32Bbox = (HI_S32*)ps32DstBbox;
        ps32RoiCnt = (HI_S32*)ps32ClassRoiNum;

        ps32Score += (HI_S32)(u32OffSet);
        ps32Bbox += (HI_S32)(SAMPLE_SVP_NNIE_COORDI_NUM * u32OffSet);

        u32RoiOutCnt = 0;
        for(i = 0; i < u32RoiCnt; i++)
        {
            u32ProposalMemPoolIndex = SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * i;
            if( 0 == ps32ProposalMemPool[u32ProposalMemPoolIndex + 5]  && ps32ProposalMemPool[u32ProposalMemPoolIndex + 4] > (HI_S32)pu32ConfThresh[j] ) //Suppression = 0; CONF_THRESH == 0.8
            {
                ps32Score[u32RoiOutCnt] = ps32ProposalMemPool[u32ProposalMemPoolIndex + 4];
                ps32Bbox[u32RoiOutCnt * SAMPLE_SVP_NNIE_COORDI_NUM ] = ps32ProposalMemPool[u32ProposalMemPoolIndex];
                ps32Bbox[u32RoiOutCnt * SAMPLE_SVP_NNIE_COORDI_NUM + 1 ] = ps32ProposalMemPool[u32ProposalMemPoolIndex + 1];
                ps32Bbox[u32RoiOutCnt * SAMPLE_SVP_NNIE_COORDI_NUM + 2 ] = ps32ProposalMemPool[u32ProposalMemPoolIndex + 2];
                ps32Bbox[u32RoiOutCnt * SAMPLE_SVP_NNIE_COORDI_NUM + 3 ] = ps32ProposalMemPool[u32ProposalMemPoolIndex + 3];
                u32RoiOutCnt++;
            }
            if(u32RoiOutCnt >= u32RoiCnt)break;
        }
        ps32RoiCnt[j] = (HI_S32)u32RoiOutCnt;
        u32OffSet += u32RoiOutCnt;

    }
    return s32Ret;
}



/*****************************************************************************
* Prototype :   SAMPLE_SVP_NNIE_RpnTmpBufSize
* Description : this function is used to get RPN func's assist buffer size
* Input :      HI_U32 u32NumRatioAnchors     [IN]  ratio anchor num
*              HI_U32 u32NumScaleAnchors     [IN]  scale anchor num
*              HI_U32 u32ConvHeight          [IN]  convolution height
*              HI_U32 u32ConvWidth           [IN]  convolution width
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
****************************************************************************/
HI_U32 SAMPLE_SVP_NNIE_RpnTmpBufSize(HI_U32 u32NumRatioAnchors,
    HI_U32 u32NumScaleAnchors, HI_U32 u32ConvHeight, HI_U32 u32ConvWidth)
{
    HI_U32 u32AnchorsNum = u32NumRatioAnchors * u32NumScaleAnchors * u32ConvHeight * u32ConvWidth;
    HI_U32 u32AnchorsSize = sizeof(HI_U32) * SAMPLE_SVP_NNIE_COORDI_NUM * u32AnchorsNum;
    HI_U32 u32BboxDeltaSize = u32AnchorsSize;
    HI_U32 u32ProposalSize = sizeof(HI_U32) * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH * u32AnchorsNum;
    HI_U32 u32RatioAnchorsSize = sizeof(HI_FLOAT) * u32NumRatioAnchors * SAMPLE_SVP_NNIE_COORDI_NUM;
    HI_U32 u32ScaleAnchorsSize = sizeof(HI_FLOAT) * u32NumRatioAnchors * u32NumScaleAnchors * SAMPLE_SVP_NNIE_COORDI_NUM;
    HI_U32 u32ScoreSize = sizeof(HI_FLOAT) * u32AnchorsNum * 2;
    HI_U32 u32StackSize = sizeof( SAMPLE_SVP_NNIE_STACK_S ) * u32AnchorsNum;
    HI_U32 u32TotalSize = u32AnchorsSize + u32BboxDeltaSize + u32ProposalSize + u32RatioAnchorsSize + u32ScaleAnchorsSize + u32ScoreSize + u32StackSize;
    return u32TotalSize;
}


/*****************************************************************************
* Prototype :   SAMPLE_SVP_NNIE_FasterRcnn_Rpn
* Description : this function is used to do RPN
* Input :     SAMPLE_SVP_NNIE_PARAM_S*                     pstNnieParam     [IN]  the pointer to FasterRcnn NNIE parameter
*              SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S*  pstSoftwareParam [IN]  the pointer to FasterRcnn software parameter
*
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_Rpn(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftwareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = SVP_NNIE_Rpn(pstSoftwareParam->aps32Conv,pstSoftwareParam->u32NumRatioAnchors,
        pstSoftwareParam->u32NumScaleAnchors,pstSoftwareParam->au32Scales,
        pstSoftwareParam->au32Ratios,pstSoftwareParam->u32OriImHeight,
        pstSoftwareParam->u32OriImWidth,pstSoftwareParam->au32ConvHeight,
        pstSoftwareParam->au32ConvWidth,pstSoftwareParam->au32ConvChannel,
        pstSoftwareParam->u32ConvStride,pstSoftwareParam->u32MaxRoiNum,
        pstSoftwareParam->u32MinSize,pstSoftwareParam->u32SpatialScale,
        pstSoftwareParam->u32NmsThresh,pstSoftwareParam->u32FilterThresh,
        pstSoftwareParam->u32NumBeforeNms,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstSoftwareParam->stRpnTmpBuf.u64VirAddr),
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstSoftwareParam->stRpnBbox.u64VirAddr),
        &pstSoftwareParam->stRpnBbox.unShape.stWhc.u32Height);
    SAMPLE_COMM_SVP_FlushCache(pstSoftwareParam->stRpnBbox.u64PhyAddr,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstSoftwareParam->stRpnBbox.u64VirAddr),
        pstSoftwareParam->stRpnBbox.u32Num*
        pstSoftwareParam->stRpnBbox.unShape.stWhc.u32Chn*
        pstSoftwareParam->stRpnBbox.unShape.stWhc.u32Height*
        pstSoftwareParam->stRpnBbox.u32Stride);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SVP_NNIE_Rpn failed!\n");
    return s32Ret;
}


/*****************************************************************************
* Prototype :   SAMPLE_SVP_NNIE_FasterRcnn_GetResultTmpBufSize
* Description : this function is used to get tmp buffer size for FasterRcnn_GetResult func
* Input :     HI_U32 u32MaxRoiNum     [IN]  max roi num
*              HI_U32 u32ClassNum      [IN]  class num
*
*
*
*
* Output :
* Return Value : HI_U32: tmp buffer size
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
HI_U32 SAMPLE_SVP_NNIE_FasterRcnn_GetResultTmpBufSize(HI_U32 u32MaxRoiNum, HI_U32 u32ClassNum)
{
    HI_U32 u32ScoreSize = sizeof(HI_FLOAT) * u32MaxRoiNum * u32ClassNum;
    HI_U32 u32ProposalSize = sizeof(HI_U32) * u32MaxRoiNum * SAMPLE_SVP_NNIE_PROPOSAL_WIDTH;
    HI_U32 u32StackSize = sizeof(SAMPLE_SVP_NNIE_STACK_S) * u32MaxRoiNum;
    HI_U32 u32TotalSize = u32ScoreSize + u32ProposalSize + u32StackSize;
    return u32TotalSize;
}


/*****************************************************************************
* Prototype :   SAMPLE_SVP_NNIE_FasterRcnn_GetResult
* Description : this function is used to get FasterRcnn result
* Input :     SAMPLE_SVP_NNIE_PARAM_S*                     pstNnieParam     [IN]  the pointer to FasterRcnn NNIE parameter
*              SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S*  pstSoftwareParam [IN]  the pointer to FasterRcnn software parameter
*
*
*
*
* Output :
* Return Value : HI_SUCCESS: Success;Error codes: Failure.
* Spec :
* Calls :
* Called By :
* History:
*
* 1. Date : 2017-11-10
* Author :
* Modification : Create
*
*****************************************************************************/
HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftwareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0;
    HI_S32* ps32Proposal = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstSoftwareParam->stRpnBbox.u64VirAddr);
    SAMPLE_SVP_CHECK_EXPR_RET(0 == pstSoftwareParam->stRpnBbox.u64VirAddr,HI_INVALID_VALUE,
        SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,pstSoftwareParam->stRpnBbox.u64VirAddr can't be 0!\n");

    for(i = 0; i < pstSoftwareParam->stRpnBbox.unShape.stWhc.u32Height; i++)
    {
        *(ps32Proposal+SAMPLE_SVP_NNIE_COORDI_NUM*i) /= SAMPLE_SVP_NNIE_QUANT_BASE;
        *(ps32Proposal+SAMPLE_SVP_NNIE_COORDI_NUM*i+1) /= SAMPLE_SVP_NNIE_QUANT_BASE;
        *(ps32Proposal+SAMPLE_SVP_NNIE_COORDI_NUM*i+2) /= SAMPLE_SVP_NNIE_QUANT_BASE;
        *(ps32Proposal+SAMPLE_SVP_NNIE_COORDI_NUM*i+3) /= SAMPLE_SVP_NNIE_QUANT_BASE;
    }
    s32Ret = SVP_NNIE_FasterRcnn_GetResult(
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstNnieParam->astSegData[1].astDst[0].u64VirAddr),
        pstNnieParam->astSegData[1].astDst[0].u32Stride,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstNnieParam->astSegData[1].astDst[1].u64VirAddr),
        pstNnieParam->astSegData[1].astDst[1].u32Stride,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstSoftwareParam->stRpnBbox.u64VirAddr),
        pstSoftwareParam->stRpnBbox.unShape.stWhc.u32Height,
        pstSoftwareParam->au32ConfThresh,pstSoftwareParam->u32ValidNmsThresh,
        pstSoftwareParam->u32MaxRoiNum,pstSoftwareParam->u32ClassNum,
        pstSoftwareParam->u32OriImWidth,pstSoftwareParam->u32OriImHeight,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstSoftwareParam->stGetResultTmpBuf.u64VirAddr),
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstSoftwareParam->stDstScore.u64VirAddr),
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstSoftwareParam->stDstRoi.u64VirAddr),
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstSoftwareParam->stClassRoiNum.u64VirAddr));

    return s32Ret;
}

#ifdef __cplusplus
}
#endif
