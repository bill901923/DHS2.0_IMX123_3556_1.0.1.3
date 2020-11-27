#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <math.h>

#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_svp.h"
//#include "sample_comm.h"
#include "mpi_sys.h"
#include "sample_comm_svp.h"
#include "sample_comm_nnie.h"
#include "sample_nnie_main.h"
#include "sample_svp_nnie_software.h"


/*fasterrcnn para*/
static SAMPLE_SVP_NNIE_MODEL_S s_stFasterRcnnModel = {0};
static SAMPLE_SVP_NNIE_PARAM_S s_stFasterRcnnNnieParam = {0};
static SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S s_stFasterRcnnSoftwareParam = {0};
static SAMPLE_SVP_NNIE_NET_TYPE_E s_enNetType;

/******************************************************************************
* function : NNIE Forward
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Forward(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S* pstInputDataIdx,
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S* pstProcSegIdx,HI_BOOL bInstant)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0, j = 0;
    HI_BOOL bFinish = HI_FALSE;
    SVP_NNIE_HANDLE hSvpNnieHandle = 0;
    HI_U32 u32TotalStepNum = 0;


    SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64PhyAddr,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64VirAddr),
        pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u32Size);

    for(i = 0; i < pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].u32DstNum; i++)
    {
        if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].enType)
        {
            for(j = 0; j < pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num; j++)
            {
                u32TotalStepNum += *(SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stSeq.u64VirAddrStep)+j);
            }
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                u32TotalStepNum*pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);

        }
        else
        {
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Chn*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Height*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
    }

    /*set input blob according to node name*/
    if(pstInputDataIdx->u32SegIdx != pstProcSegIdx->u32SegIdx)
    {
        for(i = 0; i < pstNnieParam->pstModel->astSeg[pstProcSegIdx->u32SegIdx].u16SrcNum; i++)
        {
            for(j = 0; j < pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].u16DstNum; j++)
            {
                if(0 == strncmp(pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].astDstNode[j].szName,
                    pstNnieParam->pstModel->astSeg[pstProcSegIdx->u32SegIdx].astSrcNode[i].szName,
                    SVP_NNIE_NODE_NAME_LEN))
                {
                    pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astSrc[i] =
                        pstNnieParam->astSegData[pstInputDataIdx->u32SegIdx].astDst[j];
                    break;
                }
            }
            SAMPLE_SVP_CHECK_EXPR_RET((j == pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].u16DstNum),
                HI_FAILURE,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,can't find %d-th seg's %d-th src blob!\n",
                pstProcSegIdx->u32SegIdx,i);
        }
    }

    /*NNIE_Forward*/
    s32Ret = HI_MPI_SVP_NNIE_Forward(&hSvpNnieHandle,
        pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astSrc,
        pstNnieParam->pstModel, pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst,
        &pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx], bInstant);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,HI_MPI_SVP_NNIE_Forward failed!\n");

    if(bInstant)
    {
        /*Wait NNIE finish*/
        while(HI_ERR_SVP_NNIE_QUERY_TIMEOUT == (s32Ret = HI_MPI_SVP_NNIE_Query(pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].enNnieId,
            hSvpNnieHandle, &bFinish, HI_TRUE)))
        {
            usleep(100);
            SAMPLE_SVP_TRACE(SAMPLE_SVP_ERR_LEVEL_INFO,
                "HI_MPI_SVP_NNIE_Query Query timeout!\n");
        }
    }
    u32TotalStepNum = 0;

    for(i = 0; i < pstNnieParam->astForwardCtrl[pstProcSegIdx->u32SegIdx].u32DstNum; i++)
    {
        if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].enType)
        {
            for(j = 0; j < pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num; j++)
            {
                u32TotalStepNum += *(SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stSeq.u64VirAddrStep)+j);
            }
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                u32TotalStepNum*pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);

        }
        else
        {
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Chn*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Height*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
    }

    return s32Ret;
}

/******************************************************************************
* function : NNIE ForwardWithBbox
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_ForwardWithBbox(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S* pstInputDataIdx,SVP_SRC_BLOB_S astBbox[],
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S* pstProcSegIdx,HI_BOOL bInstant)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_BOOL bFinish = HI_FALSE;
    SVP_NNIE_HANDLE hSvpNnieHandle = 0;
    HI_U32 u32TotalStepNum = 0;
    HI_U32 i, j;


    SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64PhyAddr,
        SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u64VirAddr),
        pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].stTskBuf.u32Size);


    for(i = 0; i < pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].u32DstNum; i++)
    {
        if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].enType)
        {
            for(j = 0; j < pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num; j++)
            {
                u32TotalStepNum += *(SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stSeq.u64VirAddrStep)+j);
            }
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                u32TotalStepNum*pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
        else
        {
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Chn*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Height*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
    }

    /*set input blob according to node name*/
    if(pstInputDataIdx->u32SegIdx != pstProcSegIdx->u32SegIdx)
    {
        for(i = 0; i < pstNnieParam->pstModel->astSeg[pstProcSegIdx->u32SegIdx].u16SrcNum; i++)
        {
            for(j = 0; j < pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].u16DstNum; j++)
            {
                if(0 == strncmp(pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].astDstNode[j].szName,
                    pstNnieParam->pstModel->astSeg[pstProcSegIdx->u32SegIdx].astSrcNode[i].szName,
                    SVP_NNIE_NODE_NAME_LEN))
                {
                    pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astSrc[i] =
                        pstNnieParam->astSegData[pstInputDataIdx->u32SegIdx].astDst[j];
                    break;
                }
            }
            SAMPLE_SVP_CHECK_EXPR_RET((j == pstNnieParam->pstModel->astSeg[pstInputDataIdx->u32SegIdx].u16DstNum),
                HI_FAILURE,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,can't find %d-th seg's %d-th src blob!\n",
                pstProcSegIdx->u32SegIdx,i);
        }
    }
    /*NNIE_ForwardWithBbox*/
    s32Ret = HI_MPI_SVP_NNIE_ForwardWithBbox(&hSvpNnieHandle,
        pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astSrc,astBbox,
        pstNnieParam->pstModel, pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst,
        &pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx], bInstant);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,HI_MPI_SVP_NNIE_ForwardWithBbox failed!\n");

    if(bInstant)
    {
        /*Wait NNIE finish*/
        while(HI_ERR_SVP_NNIE_QUERY_TIMEOUT == (s32Ret = HI_MPI_SVP_NNIE_Query(pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].enNnieId,
            hSvpNnieHandle, &bFinish, HI_TRUE)))
        {
            usleep(100);
            SAMPLE_SVP_TRACE(SAMPLE_SVP_ERR_LEVEL_INFO,
                "HI_MPI_SVP_NNIE_Query Query timeout!\n");
        }
    }

    u32TotalStepNum = 0;

    for(i = 0; i < pstNnieParam->astForwardWithBboxCtrl[pstProcSegIdx->u32SegIdx].u32DstNum; i++)
    {
        if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].enType)
        {
            for(j = 0; j < pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num; j++)
            {
                u32TotalStepNum += *(SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stSeq.u64VirAddrStep)+j);
            }
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                u32TotalStepNum*pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
        else
        {
            SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64PhyAddr,
                SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u64VirAddr),
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Num*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Chn*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].unShape.stWhc.u32Height*
                pstNnieParam->astSegData[pstProcSegIdx->u32SegIdx].astDst[i].u32Stride);
        }
    }

    return s32Ret;
}


/******************************************************************************
* function : Fill Src Data
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_FillSrcData(SAMPLE_SVP_NNIE_CFG_S* pstNnieCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S* pstInputDataIdx)
{
    FILE* fp = NULL;
    HI_U32 i =0, j = 0, n = 0;
    HI_U32 u32Height = 0, u32Width = 0, u32Chn = 0, u32Stride = 0, u32Dim = 0;
    HI_U32 u32VarSize = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U8*pu8PicAddr = NULL;
    HI_U32*pu32StepAddr = NULL;
    HI_U32 u32SegIdx = pstInputDataIdx->u32SegIdx;
    HI_U32 u32NodeIdx = pstInputDataIdx->u32NodeIdx;
    HI_U32 u32TotalStepNum = 0;

    /*open file*/
    if (NULL != pstNnieCfg->pszPic)
    {
        fp = fopen(pstNnieCfg->pszPic,"rb");
        SAMPLE_SVP_CHECK_EXPR_RET(NULL == fp,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error, open file failed!\n");
    }

    /*get data size*/
    if(SVP_BLOB_TYPE_U8 <= pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType &&
        SVP_BLOB_TYPE_YVU422SP >= pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
    {
        u32VarSize = sizeof(HI_U8);
    }
    else
    {
        u32VarSize = sizeof(HI_U32);
    }

    /*fill src data*/
    if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
    {
        u32Dim = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stSeq.u32Dim;
        u32Stride = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Stride;
        pu32StepAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stSeq.u64VirAddrStep);
        pu8PicAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U8,pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr);
        for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
        {
            for(i = 0;i < *(pu32StepAddr+n); i++)
            {
                s32Ret = fread(pu8PicAddr,u32Dim*u32VarSize,1,fp);
                SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                pu8PicAddr += u32Stride;
            }
            u32TotalStepNum += *(pu32StepAddr+n);
        }
        SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64PhyAddr,
            SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr),
            u32TotalStepNum*u32Stride);
    }
    else
    {
        u32Height = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Height;
        u32Width = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Width;
        u32Chn = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].unShape.stWhc.u32Chn;
        u32Stride = pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Stride;
        pu8PicAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U8,pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr);
        if(SVP_BLOB_TYPE_YVU420SP== pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
        {
            for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for(i = 0; i < u32Chn*u32Height/2; i++)
                {
                    s32Ret = fread(pu8PicAddr,u32Width*u32VarSize,1,fp);
                    SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                    pu8PicAddr += u32Stride;
                }
            }
        }
        else if(SVP_BLOB_TYPE_YVU422SP== pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].enType)
        {
            for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for(i = 0; i < u32Height*2; i++)
                {
                    s32Ret = fread(pu8PicAddr,u32Width*u32VarSize,1,fp);
                    SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                    pu8PicAddr += u32Stride;
                }
            }
        }
        else
        {
            for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num; n++)
            {
                for(i = 0;i < u32Chn; i++)
                {
                    for(j = 0; j < u32Height; j++)
                    {
                        s32Ret = fread(pu8PicAddr,u32Width*u32VarSize,1,fp);
                        SAMPLE_SVP_CHECK_EXPR_GOTO(1 != s32Ret,FAIL,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,Read image file failed!\n");
                        pu8PicAddr += u32Stride;
                    }
                }
            }
        }
        SAMPLE_COMM_SVP_FlushCache(pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64PhyAddr,
            SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_VOID,pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u64VirAddr),
            pstNnieParam->astSegData[u32SegIdx].astSrc[u32NodeIdx].u32Num*u32Chn*u32Height*u32Stride);
    }

    fclose(fp);
    return HI_SUCCESS;
FAIL:

    fclose(fp);
    return HI_FAILURE;
}

/******************************************************************************
* function : print report result
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_PrintReportResult(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam)
{
    HI_U32 u32SegNum = pstNnieParam->pstModel->u32NetSegNum;
    HI_U32 i = 0, j = 0, k = 0, n = 0;
    HI_U32 u32SegIdx = 0, u32NodeIdx = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR acReportFileName[SAMPLE_SVP_NNIE_REPORT_NAME_LENGTH] = {'\0'};
    FILE* fp = NULL;
    HI_U32*pu32StepAddr = NULL;
    HI_S32*ps32ResultAddr = NULL;
    HI_U32 u32Height = 0, u32Width = 0, u32Chn = 0, u32Stride = 0, u32Dim = 0;

    for(u32SegIdx = 0; u32SegIdx < u32SegNum; u32SegIdx++)
    {
        for(u32NodeIdx = 0; u32NodeIdx < pstNnieParam->pstModel->astSeg[u32SegIdx].u16DstNum; u32NodeIdx++)
        {
            s32Ret = snprintf(acReportFileName,SAMPLE_SVP_NNIE_REPORT_NAME_LENGTH,
                "seg%d_layer%d_output%d_inst.linear.hex",u32SegIdx,
                pstNnieParam->pstModel->astSeg[u32SegIdx].astDstNode[u32NodeIdx].u32NodeId,0);
            SAMPLE_SVP_CHECK_EXPR_RET(s32Ret < 0,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error,create file name failed!\n");

            fp = fopen(acReportFileName,"w");
            SAMPLE_SVP_CHECK_EXPR_RET(NULL == fp,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
                "Error,open file failed!\n");

            if(SVP_BLOB_TYPE_SEQ_S32 == pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].enType)
            {
                u32Dim = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stSeq.u32Dim;
                u32Stride = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Stride;
                pu32StepAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_U32,pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stSeq.u64VirAddrStep);
                ps32ResultAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u64VirAddr);

                for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Num; n++)
                {
                    for(i = 0;i < *(pu32StepAddr+n); i++)
                    {
                        for(j = 0; j < u32Dim; j++)
                        {
                            s32Ret = fprintf(fp ,"%08x\n",*(ps32ResultAddr+j));
                            SAMPLE_SVP_CHECK_EXPR_GOTO(s32Ret < 0,PRINT_FAIL,
                                SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,write report result file failed!\n");
                        }
                        ps32ResultAddr += u32Stride/sizeof(HI_U32);
                    }
                }
            }
            else
            {
                u32Height = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stWhc.u32Height;
                u32Width = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stWhc.u32Width;
                u32Chn = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].unShape.stWhc.u32Chn;
                u32Stride = pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Stride;
                ps32ResultAddr = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u64VirAddr);
                for(n = 0; n < pstNnieParam->astSegData[u32SegIdx].astDst[u32NodeIdx].u32Num; n++)
                {
                    for(i = 0;i < u32Chn; i++)
                    {
                        for(j = 0; j < u32Height; j++)
                        {
                            for(k = 0; k < u32Width; k++)
                            {
                                s32Ret = fprintf(fp,"%08x\n",*(ps32ResultAddr+k));
                                SAMPLE_SVP_CHECK_EXPR_GOTO(s32Ret < 0,PRINT_FAIL,
                                    SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,write report result file failed!\n");
                            }
                            ps32ResultAddr += u32Stride/sizeof(HI_U32);
                        }
                    }
                }
            }
            fclose(fp);
        }
    }
    return HI_SUCCESS;

PRINT_FAIL:
    fclose(fp);
    return HI_FAILURE;
}

/******************************************************************************
* function : print detection result
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_Detection_PrintResult(SVP_BLOB_S *pstDstScore,
    SVP_BLOB_S *pstDstRoi, SVP_BLOB_S *pstClassRoiNum, HI_FLOAT f32PrintResultThresh)
{
    HI_U32 i = 0, j = 0;
    HI_U32 u32RoiNumBias = 0;
    HI_U32 u32ScoreBias = 0;
    HI_U32 u32BboxBias = 0;
    HI_FLOAT f32Score = 0.0f;
    HI_S32* ps32Score = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstDstScore->u64VirAddr);
    HI_S32* ps32Roi = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstDstRoi->u64VirAddr);
    HI_S32* ps32ClassRoiNum = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstClassRoiNum->u64VirAddr);
    HI_U32 u32ClassNum = pstClassRoiNum->unShape.stWhc.u32Width;
    HI_S32 s32XMin = 0,s32YMin= 0,s32XMax = 0,s32YMax = 0;

    u32RoiNumBias += ps32ClassRoiNum[0];
    for (i = 1; i < u32ClassNum; i++)
    {
        u32ScoreBias = u32RoiNumBias;
        u32BboxBias = u32RoiNumBias * SAMPLE_SVP_NNIE_COORDI_NUM;
        /*if the confidence score greater than result threshold, the result will be printed*/
        if((HI_FLOAT)ps32Score[u32ScoreBias] / SAMPLE_SVP_NNIE_QUANT_BASE >=
            f32PrintResultThresh && ps32ClassRoiNum[i]!=0)
        {
            SAMPLE_SVP_TRACE_INFO("==== The %dth class box info====\n", i);
        }
        for (j = 0; j < (HI_U32)ps32ClassRoiNum[i]; j++)
        {
            f32Score = (HI_FLOAT)ps32Score[u32ScoreBias + j] / SAMPLE_SVP_NNIE_QUANT_BASE;
            if (f32Score < f32PrintResultThresh)
            {
                break;
            }
            s32XMin = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM];
            s32YMin = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 1];
            s32XMax = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 2];
            s32YMax = ps32Roi[u32BboxBias + j*SAMPLE_SVP_NNIE_COORDI_NUM + 3];
            SAMPLE_SVP_TRACE_INFO("%d %d %d %d %f\n", s32XMin, s32YMin, s32XMax, s32YMax, f32Score);
        }
        u32RoiNumBias += ps32ClassRoiNum[i];
    }
    return HI_SUCCESS;
}


/******************************************************************************
* function : FasterRcnn software deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_SoftwareDeinit(SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_SVP_CHECK_EXPR_RET(NULL == pstSoftWareParam,HI_INVALID_VALUE,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error, pstSoftWareParam can't be NULL!\n");
    if(0!=pstSoftWareParam->stRpnTmpBuf.u64PhyAddr && 0!=pstSoftWareParam->stRpnTmpBuf.u64VirAddr)
    {
        SAMPLE_SVP_MMZ_FREE(pstSoftWareParam->stRpnTmpBuf.u64PhyAddr,
            pstSoftWareParam->stRpnTmpBuf.u64VirAddr);
        pstSoftWareParam->stRpnTmpBuf.u64PhyAddr = 0;
        pstSoftWareParam->stRpnTmpBuf.u64VirAddr = 0;
    }
    return s32Ret;
}


/******************************************************************************
* function : FasterRcnn Deinit
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_Deinit(SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam,
    SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftWareParam,SAMPLE_SVP_NNIE_MODEL_S* pstNnieModel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*hardware deinit*/
    if(pstNnieParam!=NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_ParamDeinit(pstNnieParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_COMM_SVP_NNIE_ParamDeinit failed!\n");
    }
    /*software deinit*/
    if(pstSoftWareParam!=NULL)
    {
        s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_SoftwareDeinit(pstSoftWareParam);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_FasterRcnn_SoftwareDeinit failed!\n");
    }
    /*model deinit*/
    if(pstNnieModel!=NULL)
    {
        s32Ret = SAMPLE_COMM_SVP_NNIE_UnloadModel(pstNnieModel);
        SAMPLE_SVP_CHECK_EXPR_TRACE(HI_SUCCESS != s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_COMM_SVP_NNIE_UnloadModel failed!\n");
    }
    return s32Ret;
}


/******************************************************************************
* function : FasterRcnn software para init
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_SoftwareInit(SAMPLE_SVP_NNIE_CFG_S* pstCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_U32 i = 0, j = 0;
    HI_U32 u32RpnTmpBufSize = 0;
    HI_U32 u32RpnBboxBufSize = 0;
    HI_U32 u32GetResultTmpBufSize = 0;
    HI_U32 u32DstRoiSize = 0;
    HI_U32 u32DstScoreSize = 0;
    HI_U32 u32ClassRoiNumSize = 0;
    HI_U32 u32ClassNum = 0;
    HI_U32 u32TotalSize = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U64 u64PhyAddr = 0;
    HI_U8* pu8VirAddr = NULL;

    /*RPN parameter init*/
    pstSoftWareParam->u32MaxRoiNum = pstCfg->u32MaxRoiNum;
    if(SAMPLE_SVP_NNIE_VGG16_FASTER_RCNN == s_enNetType)
    {
        pstSoftWareParam->u32ClassNum = 4;
        pstSoftWareParam->u32NumRatioAnchors = 3;
        pstSoftWareParam->u32NumScaleAnchors = 3;
        pstSoftWareParam->au32Scales[0] = 8 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[1] = 16 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[2] = 32 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Ratios[0] = 0.5 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Ratios[1] = 1 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Ratios[2] = 2 * SAMPLE_SVP_QUANT_BASE;
    }
    else
    {
        pstSoftWareParam->u32ClassNum = 2;
        pstSoftWareParam->u32NumRatioAnchors = 1;
        pstSoftWareParam->u32NumScaleAnchors = 9;
        pstSoftWareParam->au32Scales[0] = 1.5 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[1] = 2.1 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[2] = 2.9 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[3] = 4.1 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[4] = 5.8 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[5] = 8.0 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[6] = 11.3 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[7] = 15.8 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Scales[8] = 22.1 * SAMPLE_SVP_QUANT_BASE;
        pstSoftWareParam->au32Ratios[0] = 2.44 * SAMPLE_SVP_QUANT_BASE;
    }

    pstSoftWareParam->u32OriImHeight = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Height;
    pstSoftWareParam->u32OriImWidth = pstNnieParam->astSegData[0].astSrc[0].unShape.stWhc.u32Width;
    pstSoftWareParam->u32MinSize = 16;
    pstSoftWareParam->u32FilterThresh = 16;
    pstSoftWareParam->u32SpatialScale = (HI_U32)(0.0625 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->u32NmsThresh = (HI_U32)(0.7 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->u32FilterThresh = 0;
    pstSoftWareParam->u32NumBeforeNms = 6000;
    for(i = 0; i < pstSoftWareParam->u32ClassNum; i++)
    {
        pstSoftWareParam->au32ConfThresh[i] = 1;
    }
    pstSoftWareParam->u32ValidNmsThresh = (HI_U32)(0.3 * SAMPLE_SVP_QUANT_BASE);
    pstSoftWareParam->stRpnBbox.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Height = pstCfg->u32MaxRoiNum;
    pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Width = SAMPLE_SVP_COORDI_NUM;
    pstSoftWareParam->stRpnBbox.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(SAMPLE_SVP_COORDI_NUM*sizeof(HI_U32));
    pstSoftWareParam->stRpnBbox.u32Num = 1;
    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < pstNnieParam->pstModel->astSeg[0].u16DstNum; j++)
        {
            if(0 == strncmp(pstNnieParam->pstModel->astSeg[0].astDstNode[j].szName,
                    pstSoftWareParam->apcRpnDataLayerName[i],
                    SVP_NNIE_NODE_NAME_LEN))
            {
                pstSoftWareParam->aps32Conv[i] = SAMPLE_SVP_NNIE_CONVERT_64BIT_ADDR(HI_S32,pstNnieParam->astSegData[0].astDst[j].u64VirAddr);
                pstSoftWareParam->au32ConvHeight[i] = pstNnieParam->pstModel->astSeg[0].astDstNode[j].unShape.stWhc.u32Height;
                pstSoftWareParam->au32ConvWidth[i] = pstNnieParam->pstModel->astSeg[0].astDstNode[j].unShape.stWhc.u32Width;
                pstSoftWareParam->au32ConvChannel[i] = pstNnieParam->pstModel->astSeg[0].astDstNode[j].unShape.stWhc.u32Chn;
                break;
            }
        }
        SAMPLE_SVP_CHECK_EXPR_RET((j == pstNnieParam->pstModel->astSeg[0].u16DstNum),
            HI_FAILURE,SAMPLE_SVP_ERR_LEVEL_ERROR,"Error,failed to find report node %s!\n",
            pstSoftWareParam->apcRpnDataLayerName[i]);
        if(0 == i)
        {
            pstSoftWareParam->u32ConvStride = pstNnieParam->astSegData[0].astDst[j].u32Stride;
        }
    }

    /*calculate software mem size*/
    u32ClassNum = pstSoftWareParam->u32ClassNum;
    u32RpnTmpBufSize = SAMPLE_SVP_NNIE_RpnTmpBufSize(pstSoftWareParam->u32NumRatioAnchors,
        pstSoftWareParam->u32NumScaleAnchors,pstSoftWareParam->au32ConvHeight[0],
        pstSoftWareParam->au32ConvWidth[0]);
    u32RpnTmpBufSize = SAMPLE_SVP_NNIE_ALIGN16(u32RpnTmpBufSize);
    u32RpnBboxBufSize = pstSoftWareParam->stRpnBbox.u32Num*
        pstSoftWareParam->stRpnBbox.unShape.stWhc.u32Height*pstSoftWareParam->stRpnBbox.u32Stride;
    u32GetResultTmpBufSize = SAMPLE_SVP_NNIE_FasterRcnn_GetResultTmpBufSize(pstCfg->u32MaxRoiNum,u32ClassNum);
    u32GetResultTmpBufSize = SAMPLE_SVP_NNIE_ALIGN16(u32GetResultTmpBufSize);
    u32DstRoiSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstCfg->u32MaxRoiNum*sizeof(HI_U32)*SAMPLE_SVP_COORDI_NUM);
    u32DstScoreSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstCfg->u32MaxRoiNum*sizeof(HI_U32));
    u32ClassRoiNumSize = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    u32TotalSize = u32RpnTmpBufSize + u32RpnBboxBufSize + u32GetResultTmpBufSize + u32DstRoiSize +
        u32DstScoreSize + u32ClassRoiNumSize;

    /*malloc mem*/
    s32Ret = SAMPLE_COMM_SVP_MallocCached("SAMPLE_RCNN_INIT",NULL,(HI_U64*)&u64PhyAddr,
        (void**)&pu8VirAddr,u32TotalSize);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,Malloc memory failed!\n");
    memset(pu8VirAddr,0, u32TotalSize);
    SAMPLE_COMM_SVP_FlushCache(u64PhyAddr,(void*)pu8VirAddr,u32TotalSize);

    /*set addr*/
    pstSoftWareParam->stRpnTmpBuf.u64PhyAddr = u64PhyAddr;
    pstSoftWareParam->stRpnTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr);
    pstSoftWareParam->stRpnTmpBuf.u32Size = u32RpnTmpBufSize;

    pstSoftWareParam->stRpnBbox.u64PhyAddr = u64PhyAddr+u32RpnTmpBufSize;
    pstSoftWareParam->stRpnBbox.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr)+u32RpnTmpBufSize;

    pstSoftWareParam->stGetResultTmpBuf.u64PhyAddr = u64PhyAddr+u32RpnBboxBufSize+u32RpnTmpBufSize;
    pstSoftWareParam->stGetResultTmpBuf.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnBboxBufSize+u32RpnTmpBufSize);
    pstSoftWareParam->stGetResultTmpBuf.u32Size = u32GetResultTmpBufSize;

    pstSoftWareParam->stDstRoi.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstRoi.u64PhyAddr = u64PhyAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize;
    pstSoftWareParam->stDstRoi.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize);
    pstSoftWareParam->stDstRoi.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32)*SAMPLE_SVP_COORDI_NUM);
    pstSoftWareParam->stDstRoi.u32Num = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstRoi.unShape.stWhc.u32Width = u32ClassNum*pstSoftWareParam->u32MaxRoiNum*SAMPLE_SVP_COORDI_NUM;

    pstSoftWareParam->stDstScore.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stDstScore.u64PhyAddr = u64PhyAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize+u32DstRoiSize;
    pstSoftWareParam->stDstScore.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize+u32DstRoiSize);
    pstSoftWareParam->stDstScore.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*pstSoftWareParam->u32MaxRoiNum*sizeof(HI_U32));
    pstSoftWareParam->stDstScore.u32Num = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stDstScore.unShape.stWhc.u32Width = u32ClassNum*pstSoftWareParam->u32MaxRoiNum;

    pstSoftWareParam->stClassRoiNum.enType = SVP_BLOB_TYPE_S32;
    pstSoftWareParam->stClassRoiNum.u64PhyAddr = u64PhyAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize+u32DstRoiSize+u32DstScoreSize;
    pstSoftWareParam->stClassRoiNum.u64VirAddr = (HI_U64)((HI_UL)pu8VirAddr+u32RpnBboxBufSize+u32RpnTmpBufSize+u32GetResultTmpBufSize+u32DstRoiSize+u32DstScoreSize);
    pstSoftWareParam->stClassRoiNum.u32Stride = SAMPLE_SVP_NNIE_ALIGN16(u32ClassNum*sizeof(HI_U32));
    pstSoftWareParam->stClassRoiNum.u32Num = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Chn = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Height = 1;
    pstSoftWareParam->stClassRoiNum.unShape.stWhc.u32Width = u32ClassNum;

    return s32Ret;
}

/******************************************************************************
* function : FasterRcnn parameter initialization
******************************************************************************/
static HI_S32 SAMPLE_SVP_NNIE_FasterRcnn_ParamInit(SAMPLE_SVP_NNIE_CFG_S* pstFasterRcnnCfg,
    SAMPLE_SVP_NNIE_PARAM_S *pstNnieParam, SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S* pstSoftWareParam)
{
    HI_S32 s32Ret = HI_SUCCESS;
    /*init hardware parameter*/
    s32Ret = SAMPLE_COMM_SVP_NNIE_ParamInit(pstFasterRcnnCfg,pstNnieParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_COMM_SVP_NNIE_ParamInit failed!\n",s32Ret);

    /*init software parameter*/
    s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_SoftwareInit(pstFasterRcnnCfg,pstNnieParam,
        pstSoftWareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,INIT_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),SAMPLE_SVP_NNIE_FasterRcnn_SoftwareInit failed!\n",s32Ret);

    return s32Ret;
INIT_FAIL_0:
    s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_Deinit(pstNnieParam,pstSoftWareParam,NULL);
    SAMPLE_SVP_CHECK_EXPR_RET(HI_SUCCESS != s32Ret,s32Ret,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),SAMPLE_SVP_NNIE_FasterRcnn_Deinit failed!\n",s32Ret);
    return HI_FAILURE;

}

/******************************************************************************
* function : show fasterRcnn sample(image 1240x375 U8_C3)
******************************************************************************/
void SAMPLE_SVP_NNIE_FasterRcnn(void)
{
    HI_CHAR *pcSrcFile = "./data/nnie_image/rgb_planar/single_person_1240x375.bgr";
    HI_CHAR *pcModelName = "./data/nnie_model/detection/inst_alexnet_frcnn_cycle.wk";
    HI_U32 u32PicNum = 1;
    HI_FLOAT f32PrintResultThresh = 0.0f;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0;
    SAMPLE_SVP_NNIE_CFG_S   stNnieCfg = {0};
    SAMPLE_SVP_NNIE_INPUT_DATA_INDEX_S stInputDataIdx = {0};
    SAMPLE_SVP_NNIE_PROCESS_SEG_INDEX_S stProcSegIdx = {0};

    /*Set configuration parameter*/
    s_enNetType = SAMPLE_SVP_NNIE_ALEXNET_FASTER_RCNN;
    f32PrintResultThresh = 0.8f;
    stNnieCfg.pszPic= pcSrcFile;
    stNnieCfg.u32MaxInputNum = u32PicNum; //max input image num in each batch
    stNnieCfg.u32MaxRoiNum = 300;
    stNnieCfg.aenNnieCoreId[0] = SVP_NNIE_ID_0; //set NNIE core for 0-th Seg
    stNnieCfg.aenNnieCoreId[1] = SVP_NNIE_ID_0; //set NNIE core for 1-th Seg

    /*Sys init*/
    SAMPLE_COMM_SVP_CheckSysInit();

    /*FasterRcnn Load model*/
    SAMPLE_SVP_TRACE_INFO("FasterRcnn Load model!\n");
    s32Ret = SAMPLE_COMM_SVP_NNIE_LoadModel(pcModelName,&s_stFasterRcnnModel);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_COMM_SVP_NNIE_LoadModel failed!\n");

    /*FasterRcnn para init*/
    /*apcRpnDataLayerName is used to set RPN data layer name
      and search RPN input data,if user has changed network struct, please
      make sure the data layer names are correct*/
    /*FasterRcnn parameters are set in SAMPLE_SVP_NNIE_FasterRcnn_SoftwareInit,
     if user has changed network struct, please make sure the parameter settings in
     SAMPLE_SVP_NNIE_FasterRcnn_SoftwareInit function are correct*/
    SAMPLE_SVP_TRACE_INFO("FasterRcnn parameter initialization!\n");
    s_stFasterRcnnNnieParam.pstModel = &s_stFasterRcnnModel.stModel;
    s_stFasterRcnnSoftwareParam.apcRpnDataLayerName[0] = "rpn_cls_score";
    s_stFasterRcnnSoftwareParam.apcRpnDataLayerName[1] = "rpn_bbox_pred";
    s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_ParamInit(&stNnieCfg,&s_stFasterRcnnNnieParam,
        &s_stFasterRcnnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FasterRcnn_ParamInit failed!\n");

    /*Fill 0-th input node of 0-th seg*/
    SAMPLE_SVP_TRACE_INFO("FasterRcnn start!\n");
    stInputDataIdx.u32SegIdx = 0;
    stInputDataIdx.u32NodeIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_FillSrcData(&stNnieCfg,&s_stFasterRcnnNnieParam,&stInputDataIdx);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FillSrcData failed!\n");

    /*NNIE process 0-th seg*/
    stProcSegIdx.u32SegIdx = 0;
    s32Ret = SAMPLE_SVP_NNIE_Forward(&s_stFasterRcnnNnieParam,&stInputDataIdx,&stProcSegIdx,HI_TRUE);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

    /*RPN*/
    s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_Rpn(&s_stFasterRcnnNnieParam,&s_stFasterRcnnSoftwareParam);
    SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error,SAMPLE_SVP_NNIE_FasterRcnn_Rpn failed!\n");

    if(0 != s_stFasterRcnnSoftwareParam.stRpnBbox.unShape.stWhc.u32Height)
    {
        /*NNIE process 1-th seg, the input conv data comes from 0-th seg's 0-th report node,
         the input roi comes from RPN results*/
        stInputDataIdx.u32SegIdx = 0;
        stInputDataIdx.u32NodeIdx = 0;
        stProcSegIdx.u32SegIdx = 1;
        s32Ret = SAMPLE_SVP_NNIE_ForwardWithBbox(&s_stFasterRcnnNnieParam,&stInputDataIdx,
            &s_stFasterRcnnSoftwareParam.stRpnBbox,&stProcSegIdx,HI_TRUE);
        SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_Forward failed!\n");

        /*GetResult*/
        /*if user has changed net struct, please make sure SAMPLE_SVP_NNIE_FasterRcnn_GetResult
         function's input datas are correct*/
        s32Ret = SAMPLE_SVP_NNIE_FasterRcnn_GetResult(&s_stFasterRcnnNnieParam,&s_stFasterRcnnSoftwareParam);
        SAMPLE_SVP_CHECK_EXPR_GOTO(HI_SUCCESS != s32Ret,FRCNN_FAIL_0,SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error,SAMPLE_SVP_NNIE_FasterRcnn_GetResult failed!\n");
    }
    else
    {
        for (i = 0; i < s_stFasterRcnnSoftwareParam.stClassRoiNum.unShape.stWhc.u32Width; i++)
        {
            *(((HI_U32*)(HI_UL)s_stFasterRcnnSoftwareParam.stClassRoiNum.u64VirAddr)+i) = 0;
        }
    }
	
	/*print report layer result*/
	(void)SAMPLE_SVP_NNIE_PrintReportResult(&s_stFasterRcnnNnieParam);
    /*print result, Alexnet_FasterRcnn has 2 classes:
     class 0:background     class 1:pedestrian */
    SAMPLE_SVP_TRACE_INFO("FasterRcnn result:\n");
    (void)SAMPLE_SVP_NNIE_Detection_PrintResult(&s_stFasterRcnnSoftwareParam.stDstScore,
        &s_stFasterRcnnSoftwareParam.stDstRoi, &s_stFasterRcnnSoftwareParam.stClassRoiNum,
        f32PrintResultThresh);

FRCNN_FAIL_0:
    SAMPLE_SVP_NNIE_FasterRcnn_Deinit(&s_stFasterRcnnNnieParam,&s_stFasterRcnnSoftwareParam,
        &s_stFasterRcnnModel);
    SAMPLE_COMM_SVP_CheckSysExit();
}

/******************************************************************************
* function : fasterRcnn sample signal handle
******************************************************************************/
void SAMPLE_SVP_NNIE_FasterRcnn_HandleSig(void)
{
    SAMPLE_SVP_NNIE_FasterRcnn_Deinit(&s_stFasterRcnnNnieParam,&s_stFasterRcnnSoftwareParam,
        &s_stFasterRcnnModel);
    memset(&s_stFasterRcnnNnieParam,0,sizeof(SAMPLE_SVP_NNIE_PARAM_S));
    memset(&s_stFasterRcnnSoftwareParam,0,sizeof(SAMPLE_SVP_NNIE_FASTERRCNN_SOFTWARE_PARAM_S));
    memset(&s_stFasterRcnnModel,0,sizeof(SAMPLE_SVP_NNIE_MODEL_S));
    SAMPLE_COMM_SVP_CheckSysExit();
}



