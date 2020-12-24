/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    mapi_vcap_hal_ahd.c
 * @brief   server vcap ahd module
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/prctl.h>
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vpss.h"
#include "mpi_vi.h"
#ifdef SUPPORT_IVE
#include "mpi_ive.h"
#include "hi_comm_ive.h"
#endif
#include "hi_comm_vi.h"
#include "hi_mapi_comm_define.h"
#include "hi_mapi_errno.h"
#include "hi_type.h"
#include "hi_mapi_log.h"
#include "mapi_comm_inner.h"
#include "hi_mapi_vproc_define.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#ifdef SUPPORT_IVE

typedef struct tagMAPI_IVE_PARAM_S {
    HI_BOOL bThreadStart;
    HI_HANDLE VcapPipeHdl;
    HI_HANDLE VpssHdl;
    VI_YUV_DATA_SEQ_E enYuvSeq;
    VB_BLK vbBlk;
    pthread_t pThreadId;
}MAPI_IVE_PARAM_S;


#define MAPI_IVE_ALIGN 16
#define MAPI_IVE_GET_FRMAE_SLEEP_MS 30
#define MAPI_VPSS_SEND_FRMAE_TIMEOUT_MS 10

static MAPI_IVE_PARAM_S g_stIVEParam[HI_MAPI_VPSS_MAX_NUM] = {0};


HI_U16 MAPI_IVE_CalcStride (HI_U32 u32Width, HI_U8 u8Align)
{
    return (u32Width + (u8Align - u32Width % u8Align) % u8Align);
}


HI_S32 MAPI_IVE_CreateFrame(const VIDEO_FRAME_INFO_S* refFrame, VIDEO_FRAME_INFO_S* stFrame, MAPI_IVE_PARAM_S *pIveParam)
{
    /* only support yuv422  pixel format, one pixel occupy two byte*/
    HI_U32 u32PixelSize = 2;
    HI_U32 u32Height = refFrame->stVFrame.u32Height;
    HI_U32 u32Stride = MAPI_IVE_CalcStride(refFrame->stVFrame.u32Width, MAPI_IVE_ALIGN);
    stFrame->stVFrame.u32Stride[0] = u32Stride;
    HI_U32 u32Size = u32Stride*u32Height*u32PixelSize;
    VB_BLK hVbBlk = HI_MPI_VB_GetBlock(VB_INVALID_POOLID,u32Size,NULL);
    if(hVbBlk == VB_INVALID_HANDLE)
    {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "exec HI_MPI_VB_GetBlock fail, size:%u \n", u32Size);
        return HI_FAILURE;
    }

    pIveParam->vbBlk = hVbBlk;
    HI_U64 u64Addr = HI_MPI_VB_Handle2PhysAddr(hVbBlk);
    if(u64Addr == 0)
    {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "exec HI_MPI_VB_Handle2PhysAddr fail \n");
        return HI_FAILURE;
    }

    VB_POOL vbPool = HI_MPI_VB_Handle2PoolId(hVbBlk);
    if(vbPool < 0)
    {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "exec HI_MPI_VB_Handle2PoolId fail \n");
        return HI_FAILURE;
    }

    stFrame->u32PoolId = vbPool;
    stFrame->stVFrame.u64PhyAddr[0] = u64Addr;
    stFrame->stVFrame.u64VirAddr[0] = u64Addr;
    stFrame->stVFrame.u32Stride[1] = u32Stride;
    stFrame->stVFrame.u64PhyAddr[1] = stFrame->stVFrame.u64PhyAddr[0] + stFrame->stVFrame.u32Stride[0] * u32Height;
    stFrame->stVFrame.u64VirAddr[1] = stFrame->stVFrame.u64VirAddr[0] + stFrame->stVFrame.u32Stride[0] * u32Height;
    return HI_SUCCESS;
}

HI_S32 MAPI_IVE_ReleaseFrame(VB_BLK vbBlk)
{
    HI_S32 s32Ret = HI_MPI_VB_ReleaseBlock(vbBlk);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "release vb failed, ret:[%#x]!!\n", s32Ret);
        return HI_MAPI_VPROC_ENOT_PERM;
    }

    return HI_SUCCESS;
}

HI_S32 MAPI_IVE_DealYvyu1080p(const VIDEO_FRAME_INFO_S* pstSrcFrame, const VIDEO_FRAME_INFO_S* pstDstFrame)
{
    IVE_DATA_S stDataSrc = {0};
    IVE_DST_DATA_S stDataDst = {0};
    IVE_DMA_CTRL_S stDmaCtrl = {IVE_DMA_MODE_INTERVAL_COPY,
                                 0, 2, 1, 1};
    IVE_HANDLE IveHandle;
    HI_BOOL bFinish;
    HI_S32 s32Ret;
    /*
      ive dma only support 1920*1080 byte one time at most, so we need to separate it into two parts.
      we deal with left part for y pixel first
    */
    stDataSrc.u64VirAddr     = pstSrcFrame->stVFrame.u64VirAddr[0];
    stDataSrc.u64PhyAddr     = pstSrcFrame->stVFrame.u64PhyAddr[0];
    stDataSrc.u32Width       = pstSrcFrame->stVFrame.u32Width;
    stDataSrc.u32Height     = pstSrcFrame->stVFrame.u32Height;
    stDataSrc.u32Stride     = pstSrcFrame->stVFrame.u32Stride[0];

    stDataDst.u64VirAddr     = pstDstFrame->stVFrame.u64VirAddr[0];
    stDataDst.u64PhyAddr     = pstDstFrame->stVFrame.u64PhyAddr[0];
    stDataDst.u32Width       = pstDstFrame->stVFrame.u32Width/2;
    stDataDst.u32Height     = pstDstFrame->stVFrame.u32Height;
    stDataDst.u32Stride     = pstDstFrame->stVFrame.u32Stride[0];
    s32Ret = HI_MPI_IVE_DMA(&IveHandle, &stDataSrc, &stDataDst,&stDmaCtrl,HI_FALSE);
    if ( s32Ret != HI_SUCCESS ) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "ive dma exec failed , ret:[%#x]!!\n", s32Ret);
        return HI_MAPI_VPROC_ENOT_PERM;
    }


    /* deal with right part for y pixel */
    stDataSrc.u64VirAddr     = pstSrcFrame->stVFrame.u64VirAddr[0] + pstDstFrame->stVFrame.u32Width;
    stDataSrc.u64PhyAddr     = pstSrcFrame->stVFrame.u64PhyAddr[0] + pstDstFrame->stVFrame.u32Width;

    stDataDst.u64VirAddr     = pstDstFrame->stVFrame.u64VirAddr[0] + pstDstFrame->stVFrame.u32Width/2;
    stDataDst.u64PhyAddr     = pstDstFrame->stVFrame.u64PhyAddr[0] + pstDstFrame->stVFrame.u32Width/2;
    stDataDst.u32Width       = pstDstFrame->stVFrame.u32Width/2;
    stDataDst.u32Height     = pstDstFrame->stVFrame.u32Height;
    stDataDst.u32Stride     = pstDstFrame->stVFrame.u32Stride[0];
    s32Ret = HI_MPI_IVE_DMA(&IveHandle, &stDataSrc, &stDataDst,&stDmaCtrl,HI_FALSE);
    if ( s32Ret != HI_SUCCESS ) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "ive dma exec failed , ret:[%#x]!!\n", s32Ret);
        return HI_MAPI_VPROC_ENOT_PERM;
    }


    /* deal with left part for vu pixel */
    stDataSrc.u64VirAddr     = pstSrcFrame->stVFrame.u64VirAddr[0] + sizeof(HI_U8);
    stDataSrc.u64PhyAddr     = pstSrcFrame->stVFrame.u64PhyAddr[0] + sizeof(HI_U8);

    stDataDst.u64VirAddr     = pstDstFrame->stVFrame.u64VirAddr[1];
    stDataDst.u64PhyAddr     = pstDstFrame->stVFrame.u64PhyAddr[1];
    stDataDst.u32Width       = pstDstFrame->stVFrame.u32Width/2;
    stDataDst.u32Height     = pstDstFrame->stVFrame.u32Height;
    stDataDst.u32Stride     = pstDstFrame->stVFrame.u32Stride[1];
    s32Ret = HI_MPI_IVE_DMA(&IveHandle, &stDataSrc, &stDataDst,&stDmaCtrl,HI_FALSE);
    if ( s32Ret != HI_SUCCESS ) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "ive dma exec failed , ret:[%#x]!!\n", s32Ret);
        return HI_MAPI_VPROC_ENOT_PERM;
    }


     /*
       deal with right part for vu pixel
       caution: be sure the address of w+1 must be valid , or it will ocuur access exception
     */
    stDataSrc.u64VirAddr     =  pstSrcFrame->stVFrame.u64VirAddr[0] + sizeof(HI_U8)+ pstDstFrame->stVFrame.u32Width;
    stDataSrc.u64PhyAddr     =  pstSrcFrame->stVFrame.u64PhyAddr[0]+ sizeof(HI_U8) + pstDstFrame->stVFrame.u32Width;

    stDataDst.u64VirAddr     = pstDstFrame->stVFrame.u64VirAddr[1] + pstDstFrame->stVFrame.u32Width/2;
    stDataDst.u64PhyAddr     = pstDstFrame->stVFrame.u64PhyAddr[1]+ pstDstFrame->stVFrame.u32Width/2;
    stDataDst.u32Width       = pstDstFrame->stVFrame.u32Width/2;
    stDataDst.u32Height     = pstDstFrame->stVFrame.u32Height;
    stDataDst.u32Stride     = pstDstFrame->stVFrame.u32Stride[1];
    s32Ret = HI_MPI_IVE_DMA(&IveHandle, &stDataSrc, &stDataDst,&stDmaCtrl,HI_FALSE);
    if ( s32Ret != HI_SUCCESS ) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "ive dma exec failed , ret:[%#x]!!\n", s32Ret);
        return HI_MAPI_VPROC_ENOT_PERM;
    }


    s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, HI_TRUE);
    while (HI_ERR_IVE_QUERY_TIMEOUT == s32Ret)
    {
       s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, HI_TRUE);
    }

    return s32Ret;
}

HI_S32 MAPI_IVE_DealPixel(const VIDEO_FRAME_INFO_S* pstSrcFrame, const VIDEO_FRAME_INFO_S* pstDstFrame, VI_YUV_DATA_SEQ_E enYuvSeq)
{
    if (pstSrcFrame->stVFrame.u32Width > 1920
         || pstSrcFrame->stVFrame.u32Height > 1080) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "not support this resolution, width:%d, height:%d "
           , pstSrcFrame->stVFrame.u32Width, pstSrcFrame->stVFrame.u32Height);
        return HI_FAILURE;
    }

    if (enYuvSeq ==  VI_DATA_SEQ_YVYU || enYuvSeq == VI_DATA_SEQ_VYUY) {
        return MAPI_IVE_DealYvyu1080p(pstSrcFrame, pstDstFrame);
    } else {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "not support this seq:%d ", enYuvSeq);
        return HI_FAILURE;
    }

}

/* it is using ive module to transform yuv422 package to yuv422 semi-planar */
HI_VOID* IveDataProcThread(HI_VOID* param)
{
    MAPI_IVE_PARAM_S *pIveParam = (MAPI_IVE_PARAM_S *)param;
    HI_S32 s32Ret = HI_FAILURE;
    HI_CHAR szThreadName[HI_MAPI_PTHREAD_NAME_LEN] = {0};
    VI_PIPE vcapPipe = pIveParam->VcapPipeHdl;
    HI_U32 u32ViDumpDepth = 2;
    VPSS_GRP vpssGrp = pIveParam->VpssHdl;

    s32Ret = snprintf_s(szThreadName, HI_MAPI_PTHREAD_NAME_LEN, HI_MAPI_PTHREAD_NAME_LEN, "Hi_IveData_%u",
        vpssGrp);
    if (s32Ret == HI_FAILURE)
    {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "snprintf_s fail and return:[%#x]!!\n", s32Ret);
        return HI_NULL;
    }
    prctl(PR_SET_NAME, (unsigned long)szThreadName, 0, 0, 0);


    VI_DUMP_ATTR_S stDumpAttr;
    stDumpAttr.bEnable  = HI_TRUE;
    stDumpAttr.u32Depth = u32ViDumpDepth;
    stDumpAttr.enDumpType = VI_DUMP_TYPE_RAW;
    s32Ret = HI_MPI_VI_SetPipeDumpAttr(vcapPipe, &stDumpAttr);
    if ( s32Ret != HI_SUCCESS ) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "pipe:%d HI_MPI_VI_SetPipeDumpAttr exec failed and return:[%#x]!!\n", vcapPipe, s32Ret);
        return NULL;
    }

    VIDEO_FRAME_INFO_S stFrame, stDstFrame;

    (HI_VOID)memset_s(&stFrame, sizeof(VIDEO_FRAME_INFO_S), 0, sizeof(VIDEO_FRAME_INFO_S));
    (HI_VOID)memset_s(&stDstFrame, sizeof(VIDEO_FRAME_INFO_S), 0, sizeof(VIDEO_FRAME_INFO_S));


    while (pIveParam->bThreadStart) {
        s32Ret = HI_MPI_VI_GetPipeFrame(vcapPipe, &stFrame, MAPI_IVE_GET_FRMAE_SLEEP_MS);
        if ( s32Ret != HI_SUCCESS ) {
            if( s32Ret == HI_ERR_VI_BUF_EMPTY) {
                continue;
            } else {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "pipe:%d get pipe frame exec failed and return:[%#x]!!\n", vcapPipe, s32Ret);
                return NULL;
            }
        }

        MAPI_IVE_CreateFrame(&stFrame, &stDstFrame, pIveParam);

        s32Ret = HI_MPI_VI_ReleasePipeFrame(vcapPipe, &stFrame);
        if(HI_SUCCESS != s32Ret)
        {
            MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "pipe:%d, vi release frame failed, ret:%x \n", vcapPipe, s32Ret);
            return NULL;
        }

        break;
    }



    while (pIveParam->bThreadStart)
    {
        s32Ret = HI_MPI_VI_GetPipeFrame(vcapPipe, &stFrame, MAPI_IVE_GET_FRMAE_SLEEP_MS);
        if ( s32Ret != HI_SUCCESS ) {
            if( s32Ret == HI_ERR_VI_BUF_EMPTY) {
                continue;
            } else {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "pipe:%d get pipe frame exec failed and return:[%#x]!!\n", vcapPipe, s32Ret);
                return NULL;
            }

        }

        stDstFrame.enModId = stFrame.enModId;
        stDstFrame.stVFrame.enField = stFrame.stVFrame.enField;
        stDstFrame.stVFrame.u32MaxLuminance = stFrame.stVFrame.u32MaxLuminance;
        stDstFrame.stVFrame.u32MinLuminance = stFrame.stVFrame.u32MinLuminance;
        stDstFrame.stVFrame.u32TimeRef = stFrame.stVFrame.u32TimeRef;
        stDstFrame.stVFrame.u64PTS = stFrame.stVFrame.u64PTS;
        stDstFrame.stVFrame.u64PrivateData = stFrame.stVFrame.u64PrivateData;
        stDstFrame.stVFrame.u32FrameFlag = stFrame.stVFrame.u32FrameFlag;
        stDstFrame.stVFrame.stSupplement = stFrame.stVFrame.stSupplement;
        stDstFrame.stVFrame.u32Width = stFrame.stVFrame.u32Width;
        stDstFrame.stVFrame.u32Height = stFrame.stVFrame.u32Height;
        stDstFrame.stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_422;
        stDstFrame.stVFrame.enVideoFormat = stFrame.stVFrame.enVideoFormat;

        s32Ret = MAPI_IVE_DealPixel(&stFrame, &stDstFrame,  pIveParam->enYuvSeq);
        if ( s32Ret != HI_SUCCESS ) {
            MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "exec ive deal with frame failed, ret:%x \n", s32Ret);
        }

        s32Ret = HI_MPI_VPSS_SendFrame(vpssGrp, 0, &stDstFrame, MAPI_VPSS_SEND_FRMAE_TIMEOUT_MS);
        if(HI_SUCCESS != s32Ret)
        {
            MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "exec vpss send frame failed, ret:%x \n", s32Ret);
        }

        s32Ret = HI_MPI_VI_ReleasePipeFrame(vcapPipe, &stFrame);
        if(HI_SUCCESS != s32Ret)
        {
            MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "pipe:%d, vi release frame failed, ret:%x \n", vcapPipe, s32Ret);
            return NULL;
        }
    }

    return NULL;
}

HI_VOID MAPI_VPROC_IveInit(HI_HANDLE VpssHdl)
{
    g_stIVEParam[VpssHdl].bThreadStart = HI_FALSE;
    return;
}


HI_S32  MAPI_VPROC_StartRecvYuv(HI_HANDLE VpssHdl, HI_HANDLE VcapPipeHdl, VI_YUV_DATA_SEQ_E enYuvSeq)
{
    if (enYuvSeq != VI_DATA_SEQ_VYUY && enYuvSeq != VI_DATA_SEQ_YVYU)
    {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, " ive not support this yuv seq:%d \n", enYuvSeq);
        return HI_MAPI_VPROC_ENOT_SUPPORT;
    }

    if (g_stIVEParam[VpssHdl].bThreadStart == HI_TRUE) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, " ive handling thread has started\n");
        return HI_SUCCESS;
    }

    HI_S32 s32Ret;
    g_stIVEParam[VpssHdl].bThreadStart = HI_TRUE;
    g_stIVEParam[VpssHdl].VcapPipeHdl = VcapPipeHdl;
    g_stIVEParam[VpssHdl].VpssHdl = VpssHdl;
    g_stIVEParam[VpssHdl].enYuvSeq = enYuvSeq;
    s32Ret = pthread_create(&g_stIVEParam[VpssHdl].pThreadId, 0, IveDataProcThread, (HI_VOID*)&g_stIVEParam[VpssHdl]);
    if( s32Ret != HI_SUCCESS )
    {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "pthread ive handling thread  faile!, ret:%x \n",s32Ret);
        return HI_MAPI_VPROC_ENOT_PERM;
    }

    return HI_SUCCESS;
}

HI_S32  MAPI_VPROC_StopRecvYuv(HI_HANDLE VpssHdl)
{
    HI_S32 s32Ret;
    if (g_stIVEParam[VpssHdl].bThreadStart == HI_FALSE) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, " ive handling thread has stoped\n");
        return HI_SUCCESS;
    }

    g_stIVEParam[VpssHdl].bThreadStart = HI_FALSE;

    s32Ret = pthread_join(g_stIVEParam[VpssHdl].pThreadId, HI_NULL);
    if (s32Ret != HI_SUCCESS ) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "pthread ive handling thread  join failed!, ret:%x \n",s32Ret);
        return HI_MAPI_VPROC_ENOT_PERM;
    }

    s32Ret = MAPI_IVE_ReleaseFrame(g_stIVEParam[VpssHdl].vbBlk);
    if (s32Ret != HI_SUCCESS ) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VPROC, "MAPI_IVE_ReleaseFrame failed!, ret:%x \n",s32Ret);
    }

    return s32Ret;
}


#endif



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
