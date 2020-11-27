#ifndef _SAMPLE_SVP_USER_KERNEL_H_
#define _SAMPLE_SVP_USER_KERNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_comm_svp.h"
#include "hi_nnie.h"
#include "mpi_nnie.h"
#include "sample_comm_svp.h"
#include "sample_comm_nnie.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define SAMPLE_SVP_NNIE_ADDR_ALIGN_16          16  /*16 byte alignment*/
#define SAMPLE_SVP_NNIE_MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define SAMPLE_SVP_NNIE_MIN(a,b)    (((a) < (b)) ? (a) : (b))
#define SAMPLE_SVP_NNIE_SIGMOID(x)   (HI_FLOAT)(1.0f/(1+exp(-x)))

#define SAMPLE_SVP_NNIE_COORDI_NUM  4      /*coordinate numbers*/
#define SAMPLE_SVP_NNIE_PROPOSAL_WIDTH  6  /*the number of proposal values*/
#define SAMPLE_SVP_NNIE_QUANT_BASE 4096    /*the base value*/
#define SAMPLE_SVP_NNIE_SCORE_NUM  2       /*the num of RPN scores*/
#define SAMPLE_SVP_NNIE_HALF 0.5f          /*the half value*/


/*stack for sort*/
typedef struct hiSAMPLE_SVP_NNIE_STACK
{
    HI_S32 s32Min;
    HI_S32 s32Max;
}SAMPLE_SVP_NNIE_STACK_S;


/*FasterRcnn*/
HI_U32 SAMPLE_SVP_NNIE_RpnTmpBufSize(HI_U32 u32RatioAnchorsNum,
    HI_U32 u32ScaleAnchorsNum, HI_U32 u32ConvHeight, HI_U32 u32ConvWidth);

HI_U32 SAMPLE_SVP_NNIE_FasterRcnn_GetResultTmpBufSize(
    HI_U32 u32MaxRoiNum, HI_U32 u32ClassNum);

HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_Rpn(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftwareParam);

HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_GetResult(SAMPLE_SVP_NNIE_PARAM_S*pstNnieParam,
    SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftwareParam);

#ifdef __cplusplus
}
#endif

#endif /* _SAMPLE_SVP_USER_KERNEL_H_ */
