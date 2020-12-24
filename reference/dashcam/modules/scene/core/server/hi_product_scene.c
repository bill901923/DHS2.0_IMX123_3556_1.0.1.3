/**
 * @file    hi_product_scene.c
 * @brief   photo picture.
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/2/2
 * @version   1.0

 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include "product_scene_inner.h"
#include "product_scene_setparam_inner.h"

#include "hi_appcomm_util.h"
#include "mpi_vpss.h"
#include "mpi_awb.h"
#include "mpi_ae.h"
#include "mpi_isp.h"
/*
#include "jpeglib.h"
#include "turbojpeg.h"
#include "jmorecfg.h"
#include "jerror.h"
#include "jconfig.h"
*/
#include "hi_product_scene.h"

#if (!defined(CONFIG_DEBUG) || !defined(AMP_LINUX_HUAWEILITE))
#include "sceneparam.dat"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** \addtogroup     SCENE */
/** @{ */  /** <!-- [SCENE] */
/** rename module name */
#ifdef HI_MODULE
#undef HI_MODULE
#endif
#define HI_MODULE "PDT_SCENE_SERVER"

#if (defined(CONFIG_DEBUG) && defined(AMP_LINUX_HUAWEILITE))
#define SCENEAUTO_PARAM_PATH HI_SHAREFS_ROOT_PATH"/sceneparam.bin"
#endif

#ifdef CONFIG_SCENEAUTO_SUPPORT
#define SCENE_DELAYSET_MAXCNT                 (3)
#define SCENE_DRC_DELAYSET_CNT                (0)
#define SCENE_DEHAZE_DELAYSET_CNT             (2)
#define SCENE_LDCI_DELAYSET_CNT               (1)
#endif

#define ALIGN_UP(x, a)           ( ( ((x) + ((a) - 1) ) / a ) * a )
#define MAX_FRM_CNT     256
#define MAX_FRM_WIDTH   4608

#define SHWE_EASOBN_LOG
typedef struct tagPDT_SCENE_CTX_S
{
    PDT_SCENE_STATE_S stSceneState;                     /**notes scene state*/
    HI_PDT_SCENE_MODE_S stSceneMode;                    /**notes scene mode*/
    PDT_SCENE_PARAM_S* pstSceneParam;                /**scene param address*/
    pthread_mutex_t SceneModuleMutex;                   /**module mutex*/
    pthread_mutex_t SceneStateMutex;
    pthread_t sceneDelayID;
} PDT_SCENE_CTX_S;
static PDT_SCENE_CTX_S s_stSceneCTX =
{
    .SceneModuleMutex = PTHREAD_MUTEX_INITIALIZER,
    .SceneStateMutex = PTHREAD_MUTEX_INITIALIZER,
    .sceneDelayID = -1,
};

typedef struct hiDUMP_MEMBUF_S
{
    VB_BLK  hBlock;
    VB_POOL hPool;
    HI_U32  u32PoolId;

    HI_U32  u64PhyAddr;
    HI_U8*  pVirAddr;
    HI_S32  s32Mdev;
} DUMP_MEMBUF_S;

static VI_DUMP_ATTR_S astBackUpDumpAttr;
static HI_U32  u32BlkSize = 0;

static HI_CHAR* pUserPageAddr[2] = {HI_NULL,HI_NULL};
static HI_U32 u32Size = 0;

static FILE* pfd = HI_NULL;

//static HI_U32 u32SignalFlag = 0;
//static VI_PIPE ViPipe = 0;
//static HI_U32 u32OrigDepth = 0;
static VIDEO_FRAME_INFO_S stFrame;


static VB_POOL hPool  = VB_INVALID_POOLID;
static DUMP_MEMBUF_S stMem = {0};
static VGS_HANDLE hHandle = -1;


/**use to check if the module init*/
#define PDT_SCENE_CHECK_INIT()\
    do{\
        HI_MUTEX_LOCK(s_stSceneCTX.SceneModuleMutex);\
        if (HI_FALSE == s_stSceneCTX.stSceneState.bSceneInit)\
        {\
            MLOGE("please init sceneauto first!\n");\
            HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);\
            return HI_PDT_SCENE_ENOTINIT;\
        }\
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);\
    }while(0);

/**use to check if scene was pause*/
#define PDT_SCENE_CHECK_PAUSE()\
    do{\
        if(HI_TRUE == s_stSceneCTX.stSceneState.bPause)\
        {\
            return HI_SUCCESS;\
        }\
    }while(0);

//char off_auto=0; 
/*DHS_ISP_SET_S set_IspExp={0,1,10000,10000,10000,50000};
DHS_AWB_SET_S set_AwbGain={0,1,800,800,800,800};
DHS_ISP_SET_S set_IspExp_bak={0,0,0,0,0,0};
DHS_AWB_SET_S set_AwbGain_bak={0,0,0,0,0,0};*/

/**-------------------------internal function interface------------------------- */
static HI_S32 PDT_SCENE_GetMainPipeIndex(HI_HANDLE VcapMainPipeHdl, HI_S32* ps32Index)
{
    HI_APPCOMM_CHECK_POINTER(ps32Index, HI_PDT_SCENE_EINVAL);
    HI_S32 i = 0;

        MLOGI("PDT_SCENE_GetMainPipeIndex\n");
    for (i = 0; i < s_stSceneCTX.stSceneState.u32MainPipeNum; i++)
    {
        if (VcapMainPipeHdl == s_stSceneCTX.stSceneState.astMainPipe[i].MainPipeHdl)
        {
            *ps32Index = i;
            break;
        }
    }

    if (i == s_stSceneCTX.stSceneState.u32MainPipeNum)
    {
        MLOGE("Invalid pipe.\n");
        return HI_PDT_SCENE_EINVAL;
    }
    return HI_SUCCESS;
}

#ifdef CONFIG_SCENEAUTO_SUPPORT
static HI_S32 PDT_SCENE_GetPipeIndexFromSceneMode(HI_HANDLE VcapPipeHdl, HI_S32* ps32Index)
{
   //     MLOGE("PDT_SCENE_GetPipeIndexFromSceneMode\n");
    HI_APPCOMM_CHECK_POINTER(ps32Index, HI_PDT_SCENE_EINVAL);
    HI_S32 i = 0;

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (VcapPipeHdl == s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl)
        {
            *ps32Index = i;
            break;
        }
    }

    if (i == HI_PDT_SCENE_PIPE_MAX_NUM)
    {
        MLOGE("Invalid pipe.\n");
        return HI_PDT_SCENE_EINVAL;
    }
    return HI_SUCCESS;
}
#endif
static HI_S32 PDT_SCENE_SetMainPipeState(const HI_PDT_SCENE_MODE_S* pstSceneMode)
{
    HI_S32 i, j = 0;
    HI_U32 u32MainPipeCnt = 0;
    /**if not use, set to 0*/
    
        MLOGI("PDT_SCENE_SetMainPipeState\n");
    memset(s_stSceneCTX.stSceneState.astMainPipe, 0, sizeof(PDT_SCENE_MAINPIPE_STATE_S) * HI_PDT_SCENE_PIPE_MAX_NUM);

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        if (0 == u32MainPipeCnt)
        {
            s_stSceneCTX.stSceneState.astMainPipe[u32MainPipeCnt].MainPipeHdl = s_stSceneCTX.stSceneMode.astPipeAttr[i].MainPipeHdl;
            s_stSceneCTX.stSceneState.astMainPipe[u32MainPipeCnt].enPipeMode = s_stSceneCTX.stSceneMode.astPipeAttr[i].enPipeMode;
            s_stSceneCTX.stSceneState.astMainPipe[u32MainPipeCnt].ispBypass = s_stSceneCTX.stSceneMode.astPipeAttr[i].bBypassIsp;
            u32MainPipeCnt++;
            continue;
        }

        for (j = 0; j < u32MainPipeCnt; j++)
        {
            if (s_stSceneCTX.stSceneState.astMainPipe[j].MainPipeHdl == s_stSceneCTX.stSceneMode.astPipeAttr[i].MainPipeHdl)
            {
                break;
            }
        }

        if (u32MainPipeCnt == j)
        {
            s_stSceneCTX.stSceneState.astMainPipe[u32MainPipeCnt].MainPipeHdl = s_stSceneCTX.stSceneMode.astPipeAttr[i].MainPipeHdl;
            s_stSceneCTX.stSceneState.astMainPipe[u32MainPipeCnt].enPipeMode = s_stSceneCTX.stSceneMode.astPipeAttr[i].enPipeMode;
            s_stSceneCTX.stSceneState.astMainPipe[u32MainPipeCnt].ispBypass = s_stSceneCTX.stSceneMode.astPipeAttr[i].bBypassIsp;
            u32MainPipeCnt++;
        }
    }

    /**set subpipe in certain mainpipe*/
    for (i = 0; i < u32MainPipeCnt; i++)
    {
        HI_U32 u32SubPipeCnt = 0;

        for (j = 0; j < HI_PDT_SCENE_PIPE_MAX_NUM; j++)
        {
            if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[j].bEnable)
            {
                continue;
            }

            if (s_stSceneCTX.stSceneState.astMainPipe[i].MainPipeHdl == s_stSceneCTX.stSceneMode.astPipeAttr[j].MainPipeHdl)
            {
               s_stSceneCTX.stSceneState.astMainPipe[i].aSubPipeHdl[u32SubPipeCnt] = s_stSceneCTX.stSceneMode.astPipeAttr[j].VcapPipeHdl;
               u32SubPipeCnt++;
            }
        }

        s_stSceneCTX.stSceneState.astMainPipe[i].u32SubPipeNum = u32SubPipeCnt;
    }

    s_stSceneCTX.stSceneState.u32MainPipeNum = u32MainPipeCnt;

    return HI_SUCCESS;
}

#ifdef CONFIG_SCENEAUTO_SUPPORT
static HI_S32 PDT_SCENE_CalculateExp(VI_PIPE ViPipe, HI_U32* pu32Iso, HI_U64* pu64Exposure)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U64 u64SysGainValue = 0;

    ISP_EXP_INFO_S stIspExpInfo;
    ISP_PUB_ATTR_S stPubAttr;

    PDT_SCENE_CHECK_PAUSE();

    s32Ret = HI_MPI_ISP_QueryExposureInfo(ViPipe, &stIspExpInfo);
    if (s32Ret != HI_SUCCESS)
    {
        MLOGW("Query IspInfo error.\n");
        return HI_PDT_SCENE_EINTER;
    }


    s32Ret = HI_MPI_ISP_GetPubAttr(ViPipe,&stPubAttr);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    //    MLOGI("B WDRMode = %d A:%d D:%d ISPD:%d EXP:%d\n",stPubAttr.enWDRMode,stIspExpInfo.u32AGain,stIspExpInfo.u32DGain,stIspExpInfo.u32ISPDGain,stIspExpInfo.u32LongExpTime);
     /*   
    if(off_auto>0)
    {      
        stIspExpInfo.u32AGain=set_IspExp.again;
        stIspExpInfo.u32DGain=set_IspExp.dgain;
        stIspExpInfo.u32ISPDGain=set_IspExp.ispgain;
        stIspExpInfo.u32LongExpTime=set_IspExp.exptime;
        stIspExpInfo.u32MedianExpTime=set_IspExp.exptime;
        stIspExpInfo.u32ShortExpTime=set_IspExp.exptime;
        stIspExpInfo.u32ExpTime=set_IspExp.exptime;
    }
        set_IspExp_bak.again=stIspExpInfo.u32AGain;
        set_IspExp_bak.dgain=stIspExpInfo.u32DGain;
        set_IspExp_bak.ispgain=stIspExpInfo.u32ISPDGain;*/
    u64SysGainValue = (HI_U64)stIspExpInfo.u32AGain * (HI_U64)stIspExpInfo.u32DGain * (HI_U64)stIspExpInfo.u32ISPDGain;

    *pu32Iso = (u64SysGainValue * 100) >> 30;

    if(WDR_MODE_4To1_LINE == stPubAttr.enWDRMode)
    {
        *pu64Exposure = (u64SysGainValue * (HI_U64)stIspExpInfo.u32LongExpTime) >> 30;
    }
    else if(WDR_MODE_3To1_LINE == stPubAttr.enWDRMode)
    {
        *pu64Exposure = (u64SysGainValue * (HI_U64)stIspExpInfo.u32MedianExpTime) >> 30;
    }
    else if(WDR_MODE_2To1_LINE == stPubAttr.enWDRMode)
    {
        *pu64Exposure = (u64SysGainValue * (HI_U64)stIspExpInfo.u32ShortExpTime) >> 30;
    }
    else
    {
        *pu64Exposure = (u64SysGainValue * (HI_U64)stIspExpInfo.u32ExpTime) >> 30;
    }

    return HI_SUCCESS;
}

static HI_S32 PDT_SCENE_CalculateWdrParam(VI_PIPE ViPipe, HI_U32 *pu32ActRation)
{
    HI_S32 s32Ret = HI_SUCCESS;

    ISP_INNER_STATE_INFO_S stInnerStateInfo;

    PDT_SCENE_CHECK_PAUSE();
    s32Ret = HI_MPI_ISP_QueryInnerStateInfo(ViPipe, &stInnerStateInfo);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    *pu32ActRation = stInnerStateInfo.u32WDRExpRatioActual[0];
    return HI_SUCCESS;
}

static HI_S32 PDT_SCENE_SetMainPipeSpecialParam(VI_PIPE ViPipe, HI_U8 u8Index, HI_BOOL bMetryFixed)
{
    HI_S32 s32Ret = HI_SUCCESS;

        MLOGI("PDT_SCENE_SetMainPipeSpecialParam %d.\n",u8Index);
         MLOGI("s_stSceneCTX.stSceneState.bRefreshIgnore %d.\n",s_stSceneCTX.stSceneState.bRefreshIgnore);
    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        s32Ret = PDT_SCENE_SetStaticAE(ViPipe, u8Index);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        s32Ret = PDT_SCENE_SetStaticAERouteEX(ViPipe, u8Index);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        s32Ret = PDT_SCENE_SetStaticAERoute(ViPipe, u8Index);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        s32Ret = PDT_SCENE_SetStaticSFAERoute(ViPipe, u8Index);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        s32Ret = PDT_SCENE_SetStaticAWB(ViPipe, u8Index);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        s32Ret = PDT_SCENE_SetStaticSaturation(ViPipe, u8Index);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        s32Ret = PDT_SCENE_SetStaticCCM(ViPipe, u8Index);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        
        MLOGI("PDT_SCENE_SetStaticStatisticsCfg.\n");
        s32Ret = PDT_SCENE_SetStaticStatisticsCfg(ViPipe, u8Index, bMetryFixed);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    s32Ret = PDT_SCENE_SetStaticLDCI(ViPipe, u8Index);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    return HI_SUCCESS;
}

static HI_S32 PDT_SCENE_SetPipeStaticParam(HI_VOID)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

        MLOGI("PDT_SCENE_SetPipeStaticParam.\n");
    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
        {
            if (HI_TRUE!= s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
            {
                continue;
            }

            s32Ret = PDT_SCENE_SetStaticAE(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
        }
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
        {
            if (HI_TRUE!= s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
            {
                continue;
            }

            s32Ret = PDT_SCENE_SetStaticAERouteEX(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
        }
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
        {
            if (HI_TRUE!= s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
            {
                continue;
            }

            s32Ret = PDT_SCENE_SetStaticAERoute(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
        }
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
        {
            if (HI_TRUE!= s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
            {
                continue;
            }

            s32Ret = PDT_SCENE_SetStaticSFAERoute(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
        }
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
        {
            if (HI_TRUE!= s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
            {
                continue;
            }

            s32Ret = PDT_SCENE_SetStaticAWB(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
        }
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
        {
            if (HI_TRUE!= s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
            {
                continue;
            }

            s32Ret = PDT_SCENE_SetStaticSaturation(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
        }
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
        {
            if (HI_TRUE!= s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
            {
                continue;
            }

            s32Ret = PDT_SCENE_SetStaticCCM(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
        }
    }

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
        {
            if (HI_TRUE!= s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
            {
                continue;
            }

            HI_S32 s32Index = 0;
            HI_HANDLE MainPipeHdl = s_stSceneCTX.stSceneMode.astPipeAttr[i].MainPipeHdl;
            HI_BOOL bMetryFixed = HI_FALSE;

            s32Ret = PDT_SCENE_GetMainPipeIndex(MainPipeHdl, &s32Index);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

            bMetryFixed = s_stSceneCTX.stSceneState.astMainPipe[s32Index].bMetryFixed;

            s32Ret = PDT_SCENE_SetStaticStatisticsCfg(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex, bMetryFixed);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
        }
    }


    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticNr(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticCAC(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }
    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }
        s32Ret = PDT_SCENE_SetStaticLDCI(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticClut(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticShading(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticCA(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticCrossTalk(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticSharpen(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticDE(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticDPC(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticDemosaic(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        /**no matter in WDR or in Linear, Always set StaticDRC. Linear Set Enable to HI_FALSE*/
        s32Ret = PDT_SCENE_SetStaticDRC(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        if (HI_PDT_SCENE_PIPE_MODE_WDR != s_stSceneCTX.stSceneMode.astPipeAttr[i].enPipeMode)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticWDRExposure(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        if (HI_PDT_SCENE_PIPE_MODE_WDR != s_stSceneCTX.stSceneMode.astPipeAttr[i].enPipeMode)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticFSWDR(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable)
        {
            continue;
        }

        s32Ret = PDT_SCENE_SetStaticDEHAZE(s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    return HI_SUCCESS;
}

static HI_S32 PDT_SCENE_SetPipeDynamicParam(HI_VOID)
{
    HI_S32 i = 0;
    HI_U32 u32Iso = 0;
    HI_U64 u64Exposure = 0;
   // MLOGI("PDT_SCENE_SetPipeDynamicParam \n");
    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (!s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable
            || s_stSceneCTX.stSceneMode.astPipeAttr[i].bBypassIsp
            || s_stSceneCTX.stSceneMode.astPipeAttr[i].bBypassVenc) {
            continue;
        }
        VI_PIPE ViPipe = s_stSceneCTX.stSceneState.astMainPipe[i].MainPipeHdl;
        PDT_SCENE_CalculateExp(ViPipe, &u32Iso, &u64Exposure);
   
        PDT_SCENE_SetDynamicQVBRVENC(s_stSceneCTX.stSceneMode.astPipeAttr[i].VencHdl,
        s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex,
        u64Exposure, s_stSceneCTX.stSceneState.astMainPipe[i].u64LastNormalExposure);
    }

    return HI_SUCCESS;
}

static HI_S32 PDT_SCENE_SetVIPipeParam(HI_VOID)
{
    HI_S32 i, j = 0;
    HI_U8  u8Index = 0;
    HI_HANDLE SubPipeHdl = -1;
    HI_S32 s32Ret = HI_SUCCESS;

        MLOGI("PDT_SCENE_SetVIPipeParam.\n");
    s32Ret = PDT_SCENE_SetPipeStaticParam();

    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    /*set mainIsp param*/
    for (i = 0; i < s_stSceneCTX.stSceneState.u32MainPipeNum ; i++)
    {
        VI_PIPE ViPipe = s_stSceneCTX.stSceneState.astMainPipe[i].MainPipeHdl;

        /**if MainIsp Pipe have SubIsp, Then Use SubIsp's param to set MainIsp*/
        /**Here, Only AE AWB*/
        /**The reason to do this, because of reducing the pipeType Number*/
        if (s_stSceneCTX.stSceneState.astMainPipe[i].u32SubPipeNum > 1)
        {
            for (j = 0; j < s_stSceneCTX.stSceneState.astMainPipe[i].u32SubPipeNum; j++)
            {
                if (ViPipe != s_stSceneCTX.stSceneState.astMainPipe[i].aSubPipeHdl[j])
                {
                    SubPipeHdl = s_stSceneCTX.stSceneState.astMainPipe[i].aSubPipeHdl[j];
                    u8Index = s_stSceneCTX.stSceneMode.astPipeAttr[SubPipeHdl].u8PipeParamIndex;
                    break;
                }
            }

            s32Ret = PDT_SCENE_SetMainPipeSpecialParam(ViPipe, u8Index, s_stSceneCTX.stSceneState.astMainPipe[i].bMetryFixed);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
        }
    }

    return HI_SUCCESS;
}

static HI_S32 PDT_SCENE_SetVPSSParam(HI_VOID)
{
    return HI_SUCCESS;
}

static HI_S32 PDT_SCENE_SetVENCParam(HI_VOID)
{
    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    if (HI_TRUE != s_stSceneCTX.stSceneState.bRefreshIgnore)
    {
        for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
        {
            if (HI_TRUE!= s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable
                || s_stSceneCTX.stSceneMode.astPipeAttr[i].bBypassVenc)
            {
                continue;
            }
            s32Ret = PDT_SCENE_SetStaticQP(s_stSceneCTX.stSceneMode.astPipeAttr[i].VencHdl, s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
        }
    }
    return HI_SUCCESS;
}

static HI_S32 PDT_SCENE_SetLDCIEnableAttr(HI_VOID)
{
    HI_S32 i = 0;
    HI_S32 ret = HI_SUCCESS;
    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; ++i) {
        if (s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable != HI_TRUE) {
            continue;
        }
        HI_U8 u8Index = s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex;
        VI_PIPE ViPipe = s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl;
        if (!s_stSceneCTX.pstSceneParam->astPipeParam[u8Index].stStaticLdci.bUsed) {
            continue;
        }
        ISP_LDCI_ATTR_S stLDCIAttr;
        ret = HI_MPI_ISP_GetLDCIAttr(ViPipe, &stLDCIAttr);
        HI_APPCOMM_CHECK_RETURN(ret, HI_PDT_SCENE_EINTER);
        stLDCIAttr.bEnable = s_stSceneCTX.pstSceneParam->astPipeParam[u8Index].stStaticLdci.bEnable;
        ret = HI_MPI_ISP_SetLDCIAttr(ViPipe, &stLDCIAttr);
        HI_APPCOMM_CHECK_RETURN(ret, HI_PDT_SCENE_EINTER);
    }
    return HI_SUCCESS;
}

static HI_S32 PDT_SCENE_SetDRCEnableAttr(HI_VOID)
{
    HI_S32 i = 0;
    HI_S32 ret = HI_SUCCESS;
    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; ++i) {
        if (s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable != HI_TRUE) {
            continue;
        }
        HI_U8 u8Index = s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex;
        VI_PIPE ViPipe = s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl;
        if (!s_stSceneCTX.pstSceneParam->astPipeParam[u8Index].stStaticDrc.bUsed) {
            continue;
        }
        ISP_DRC_ATTR_S stDrcAttr;
        ret = HI_MPI_ISP_GetDRCAttr(ViPipe, &stDrcAttr);
        HI_APPCOMM_CHECK_RETURN(ret, HI_PDT_SCENE_EINTER);
        stDrcAttr.bEnable = s_stSceneCTX.pstSceneParam->astPipeParam[u8Index].stStaticDrc.bEnable;
        ret = HI_MPI_ISP_SetDRCAttr(ViPipe, &stDrcAttr);
        HI_APPCOMM_CHECK_RETURN(ret, HI_PDT_SCENE_EINTER);
    }
    return HI_SUCCESS;
}

static HI_S32 PDT_SCENE_SetDEHAZEEnableAttr(HI_VOID)
{
    HI_S32 i = 0;
    HI_S32 ret = HI_SUCCESS;
    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; ++i) {
        if (s_stSceneCTX.stSceneMode.astPipeAttr[i].bEnable != HI_TRUE) {
            continue;
        }
        HI_U8 u8Index = s_stSceneCTX.stSceneMode.astPipeAttr[i].u8PipeParamIndex;
        VI_PIPE ViPipe = s_stSceneCTX.stSceneMode.astPipeAttr[i].VcapPipeHdl;
        if (!s_stSceneCTX.pstSceneParam->astPipeParam[u8Index].stStaticDehaze.bUsed) {
            continue;
        }
        ISP_DEHAZE_ATTR_S stDehazeAttr;
        ret = HI_MPI_ISP_GetDehazeAttr(ViPipe, &stDehazeAttr);
        HI_APPCOMM_CHECK_RETURN(ret, HI_PDT_SCENE_EINTER);
        stDehazeAttr.bEnable = s_stSceneCTX.pstSceneParam->astPipeParam[u8Index].stStaticDehaze.bEnable;
        ret = HI_MPI_ISP_SetDehazeAttr(ViPipe, &stDehazeAttr);
        HI_APPCOMM_CHECK_RETURN(ret, HI_PDT_SCENE_EINTER);
    }
    return HI_SUCCESS;
}

static HI_S32 PDT_SCENE_SetParamDelayed(HI_VOID)
{

    pthread_detach(pthread_self());
    HI_S32 ret = HI_SUCCESS;
    #ifdef CONFIG_SCENEAUTO_SUPPORT
        /**Get VD in First Time*/
        HI_S32 i = 0;
        for (i = 0; i < SCENE_DELAYSET_MAXCNT; ++i) {
            HI_MPI_ISP_GetVDTimeOut(s_stSceneCTX.stSceneState.astMainPipe[0].MainPipeHdl, ISP_VD_FE_START, 1000);
            if (i == SCENE_LDCI_DELAYSET_CNT) {
                ret = PDT_SCENE_SetLDCIEnableAttr();
                HI_APPCOMM_CHECK_RETURN(ret, HI_PDT_SCENE_EINTER);
            }

            if (i == SCENE_DRC_DELAYSET_CNT) {
                ret = PDT_SCENE_SetDRCEnableAttr();
                HI_APPCOMM_CHECK_RETURN(ret, HI_PDT_SCENE_EINTER);
            }

            if (i == SCENE_DEHAZE_DELAYSET_CNT) {
                ret = PDT_SCENE_SetDEHAZEEnableAttr();
                HI_APPCOMM_CHECK_RETURN(ret, HI_PDT_SCENE_EINTER);
            }
        }
    #endif
    return ret;
}


static HI_S32 PDT_SCENE_SetDelayStaticParamThread(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = pthread_create(&s_stSceneCTX.sceneDelayID, NULL, (void*)PDT_SCENE_SetParamDelayed, NULL);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    return s32Ret;
}


/**If MainPipe Has more then one SubPipe, Then Use SubPipe's Param to set MainPipe.*/
/**DashCam don't need to concern it. For two pipe Snap*/
static HI_S32 PDT_SCENE_GetSubepipeIndexFromMainPipeState(HI_S32 s32MainPipeArrayIdx,
    HI_HANDLE VcapPipeHdl, HI_U8* pu8Index)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32HdlIndex = -1;
    HI_HANDLE SubPipeHdl = 0;
    HI_S32 j = 0;
    s32Ret = PDT_SCENE_GetPipeIndexFromSceneMode(VcapPipeHdl, &s32HdlIndex);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_NULL);
    // MLOGI("PDT_SCENE_GetSubepipeIndexFromMainPipeState \n");
    if (s_stSceneCTX.stSceneState.astMainPipe[s32MainPipeArrayIdx].u32SubPipeNum <= 1)
    {
        *pu8Index = s_stSceneCTX.stSceneMode.astPipeAttr[s32HdlIndex].u8PipeParamIndex;
    }
    else
    {
        for (j = 0; j < s_stSceneCTX.stSceneState.astMainPipe[s32MainPipeArrayIdx].u32SubPipeNum; j++)
        {
            if (VcapPipeHdl != s_stSceneCTX.stSceneState.astMainPipe[s32MainPipeArrayIdx].aSubPipeHdl[j])
            {
                SubPipeHdl = s_stSceneCTX.stSceneState.astMainPipe[s32MainPipeArrayIdx].aSubPipeHdl[j];

                s32Ret = PDT_SCENE_GetPipeIndexFromSceneMode(SubPipeHdl, &s32HdlIndex);
                HI_APPCOMM_CHECK_RETURN(s32Ret, HI_NULL);

                *pu8Index = s_stSceneCTX.stSceneMode.astPipeAttr[s32HdlIndex].u8PipeParamIndex;
                break;
            }
        }
    }
    return HI_SUCCESS;
}

HI_VOID* PDT_SCENE_NormalAutoThread(HI_VOID* pVoid)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i = 0;
    HI_U32 u32ISO = 0;
    HI_U32 u32LastISO = 0;
    HI_U64 u64Exposure = 0;
    HI_U64 u64LastExposure = 0;
    HI_U8 u8PipeIndex = 0;
           
    prctl(PR_SET_NAME, "HI_PDT_SCENE_NormalThread", 0, 0, 0);

    while (HI_TRUE == s_stSceneCTX.stSceneState.stThreadNormal.bThreadFlag)
    {
        if (s_stSceneCTX.stSceneState.bPause)
        {
            HI_usleep(30000);
            continue;
        }
        /**Set Dynamic Param in every pipe*/
        s32Ret = PDT_SCENE_SetPipeDynamicParam();
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_NULL);

        /**Set Dynamic Param in main pipe*/
        for (i = 0; i < s_stSceneCTX.stSceneState.u32MainPipeNum; i++)
        {
            if (s_stSceneCTX.stSceneState.astMainPipe[i].ispBypass) {
                continue;
            }
            HI_HANDLE MainPipeHdl = s_stSceneCTX.stSceneState.astMainPipe[i].MainPipeHdl;
            PDT_SCENE_CalculateExp(MainPipeHdl, &u32ISO, &u64Exposure);
            u32LastISO = s_stSceneCTX.stSceneState.astMainPipe[i].u32LastNormalIso;
            u64LastExposure = s_stSceneCTX.stSceneState.astMainPipe[i].u64LastNormalExposure;
            
            s_stSceneCTX.stSceneState.astMainPipe[i].u64LastNormalExposure = u64Exposure;
            s_stSceneCTX.stSceneState.astMainPipe[i].u32LastNormalIso = u32ISO;
            s32Ret = PDT_SCENE_GetSubepipeIndexFromMainPipeState(i, MainPipeHdl, &u8PipeIndex);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_NULL);

            PDT_SCENE_SetDynamicClut(MainPipeHdl, u8PipeIndex, u32ISO, u32LastISO);
            PDT_SCENE_SetDynamicDEHAZE(MainPipeHdl, u8PipeIndex, u64Exposure, u64LastExposure);
            PDT_SCENE_SetDynamicNR(MainPipeHdl, u8PipeIndex, u32ISO, u32LastISO);
            PDT_SCENE_SetDynamicCAC(MainPipeHdl, u8PipeIndex, u32ISO, u32LastISO);
            PDT_SCENE_SetDynamicBlackLevel(MainPipeHdl, u8PipeIndex, u32ISO, u32LastISO);
            PDT_SCENE_SetDynamicShading(MainPipeHdl, u8PipeIndex, u64Exposure, u64LastExposure);
            PDT_SCENE_SetDynamicDRC(MainPipeHdl, u8PipeIndex, u32ISO, u32LastISO);
        }

        HI_usleep(30000);
    }

    return HI_NULL;
}

HI_VOID* PDT_SCENE_LuminanceAutoThread(HI_VOID* pVoid)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 i = 0;
    HI_U32 u32ISO = 0;
    HI_U64 u64Exposure = 0;
    HI_U64 u64LastExposure = 0;
    HI_U8 u8PipeIndex = 0;

    prctl(PR_SET_NAME, "HI_PDT_SCENE_LuminanceThread", 0, 0, 0);
   // Set by eason
   // MLOGI("PDT_SCENE_LuminanceAutoThread \n");
    while (HI_TRUE == s_stSceneCTX.stSceneState.stThreadLuminance.bThreadFlag)
    {
        if (s_stSceneCTX.stSceneState.bPause)
        {
            HI_usleep(30000);
            continue;
        }
        
         //       MLOGI("PDT_SCENE_SetDynamicAE \n");
        for (i = 0; i < s_stSceneCTX.stSceneState.u32MainPipeNum; i++)
        {
            if (s_stSceneCTX.stSceneState.astMainPipe[i].ispBypass) {
                continue;
            }
            HI_HANDLE MainPipeHdl = s_stSceneCTX.stSceneState.astMainPipe[i].MainPipeHdl;
            PDT_SCENE_CalculateExp(MainPipeHdl, &u32ISO, &u64Exposure);
            u64LastExposure = s_stSceneCTX.stSceneState.astMainPipe[i].u64LastLuminanceExposure;
            s_stSceneCTX.stSceneState.astMainPipe[i].u64LastLuminanceExposure = u64Exposure;
            s_stSceneCTX.stSceneState.astMainPipe[i].u32LastLuminanceISO = u32ISO;

            s32Ret = PDT_SCENE_GetSubepipeIndexFromMainPipeState(i, MainPipeHdl, &u8PipeIndex);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_NULL);

            HI_MUTEX_LOCK(s_stSceneCTX.SceneStateMutex);
            
             //   MLOGI("PDT_SCENE_SetDynamicAE \n");
            PDT_SCENE_SetDynamicAE(MainPipeHdl, u64Exposure, u64LastExposure, u8PipeIndex,
                s_stSceneCTX.stSceneState.astMainPipe[i].bAntiFlikerFixed);
            HI_MUTEX_UNLOCK(s_stSceneCTX.SceneStateMutex);

            PDT_SCENE_SetDynamicGamma(MainPipeHdl, u64Exposure, u64LastExposure, u8PipeIndex);
        }
        HI_usleep(30000);
    }
   return HI_NULL;
}

HI_VOID* PDT_SCENE_WDRAutoThread(HI_VOID* pVoid)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 i = 0;
    HI_U32 u32ISO = 0;
    HI_U32 u32LastISO = 0;
    HI_U64 u64Exposure = 0;
    HI_U64 u64LastExposure = 0;
    HI_U8 u8PipeIndex = 0;
    HI_S32 s32HdlIndex = 0;

     // MLOGI("PDT_SCENE_WDRAutoThread \n");
    prctl(PR_SET_NAME, "HI_PDT_SCENE_WDRThread", 0, 0, 0);

    while (HI_TRUE == s_stSceneCTX.stSceneState.stThreadWdr.bThreadFlag)
    {
        if (s_stSceneCTX.stSceneState.bPause)
        {
            HI_usleep(30000);
            continue;
        }
        for (i = 0; i < s_stSceneCTX.stSceneState.u32MainPipeNum; i++)
        {
            if (s_stSceneCTX.stSceneState.astMainPipe[i].ispBypass) {
                continue;
            }
            HI_HANDLE MainPipeHdl = s_stSceneCTX.stSceneState.astMainPipe[i].MainPipeHdl;

            PDT_SCENE_CalculateExp(MainPipeHdl, &u32ISO, &u64Exposure);
            u32LastISO = s_stSceneCTX.stSceneState.astMainPipe[i].u32LastWdrISO;
            u64LastExposure = s_stSceneCTX.stSceneState.astMainPipe[i].u64LastWdrExposure;
  
            s_stSceneCTX.stSceneState.astMainPipe[i].u64LastWdrExposure = u64Exposure;
            s_stSceneCTX.stSceneState.astMainPipe[i].u32LastWdrISO = u32ISO;

            s32Ret = PDT_SCENE_CalculateWdrParam(MainPipeHdl, &(s_stSceneCTX.stSceneState.astMainPipe[i].u32ActRation));
            if (HI_PDT_SCENE_EINTER == s32Ret)
            {
                s_stSceneCTX.stSceneState.astMainPipe[i].u32ActRation = 0;
            }
            else if (HI_SUCCESS == s32Ret)
            {
            }
            else
            {
                MLOGE("Error.\n");
                return HI_NULL;
            }

            s32Ret = PDT_SCENE_GetPipeIndexFromSceneMode(MainPipeHdl, &s32HdlIndex);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_NULL);

            if (HI_PDT_SCENE_PIPE_MODE_WDR != s_stSceneCTX.stSceneMode.astPipeAttr[s32HdlIndex].enPipeMode)
            {
                continue;
            }

            s32Ret = PDT_SCENE_GetSubepipeIndexFromMainPipeState(i, MainPipeHdl, &u8PipeIndex);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_NULL);

            PDT_SCENE_SetDynamicFsWdr(MainPipeHdl, u8PipeIndex, s_stSceneCTX.stSceneState.astMainPipe[i].u32ActRation,
                u32ISO, u32LastISO);
            PDT_SCENE_SetDynamicWdrExposure(MainPipeHdl, u8PipeIndex, s_stSceneCTX.stSceneState.astMainPipe[i].u32ActRation,
                u64Exposure, u64LastExposure);
        }
        HI_usleep(30000);
    }

    return HI_NULL;
}


static HI_S32 PDT_SCENE_StartAutoThread(HI_VOID)
{
 /*   HI_S32 s32Ret = HI_SUCCESS;

    if (HI_FALSE == s_stSceneCTX.stSceneState.stThreadNormal.bThreadFlag)
    {
        pthread_attr_init(&(s_stSceneCTX.stSceneState.stThreadnormattr));
        pthread_attr_setdetachstate(&(s_stSceneCTX.stSceneState.stThreadnormattr), PTHREAD_CREATE_DETACHED);
#if (!defined(AMP_LINUX))
        s_stSceneCTX.stSceneState.stThreadnormattr.inheritsched = PTHREAD_EXPLICIT_SCHED;
        s_stSceneCTX.stSceneState.stThreadnormattr.schedparam.sched_priority = 10;
#endif
        s_stSceneCTX.stSceneState.stThreadNormal.bThreadFlag = HI_TRUE;
        s32Ret = pthread_create(&s_stSceneCTX.stSceneState.stThreadNormal.pThread, &(s_stSceneCTX.stSceneState.stThreadnormattr), PDT_SCENE_NormalAutoThread, NULL);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    if (HI_FALSE == s_stSceneCTX.stSceneState.stThreadLuminance.bThreadFlag)
    {
        pthread_attr_init(&(s_stSceneCTX.stSceneState.stThreadluminanceattr));
        pthread_attr_setdetachstate(&(s_stSceneCTX.stSceneState.stThreadnormattr), PTHREAD_CREATE_DETACHED);
#if (!defined(AMP_LINUX))
        s_stSceneCTX.stSceneState.stThreadluminanceattr.inheritsched = PTHREAD_EXPLICIT_SCHED;
        s_stSceneCTX.stSceneState.stThreadluminanceattr.schedparam.sched_priority = 10;
#endif
        s_stSceneCTX.stSceneState.stThreadLuminance.bThreadFlag = HI_TRUE;
        s32Ret = pthread_create(&s_stSceneCTX.stSceneState.stThreadLuminance.pThread, &(s_stSceneCTX.stSceneState.stThreadluminanceattr), PDT_SCENE_LuminanceAutoThread, NULL);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }

    if (HI_FALSE == s_stSceneCTX.stSceneState.stThreadWdr.bThreadFlag)
    {
        pthread_attr_init(&(s_stSceneCTX.stSceneState.stThreadWdrattr));
        pthread_attr_setdetachstate(&(s_stSceneCTX.stSceneState.stThreadWdrattr), PTHREAD_CREATE_DETACHED);
#if (!defined(AMP_LINUX))
        s_stSceneCTX.stSceneState.stThreadWdrattr.inheritsched = PTHREAD_EXPLICIT_SCHED;
        s_stSceneCTX.stSceneState.stThreadWdrattr.schedparam.sched_priority = 10;
#endif
        s_stSceneCTX.stSceneState.stThreadWdr.bThreadFlag = HI_TRUE;
        s32Ret = pthread_create(&s_stSceneCTX.stSceneState.stThreadWdr.pThread, &(s_stSceneCTX.stSceneState.stThreadWdrattr), PDT_SCENE_WDRAutoThread, NULL);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
    }
*/
    return HI_SUCCESS;
}

static HI_S32 PDT_SCENE_StopAutoThread(HI_VOID)
{
    if (HI_TRUE == s_stSceneCTX.stSceneState.stThreadNormal.bThreadFlag)
    {
        s_stSceneCTX.stSceneState.stThreadNormal.bThreadFlag = HI_FALSE;
        pthread_attr_destroy(&(s_stSceneCTX.stSceneState.stThreadnormattr));
    }

    if (HI_TRUE == s_stSceneCTX.stSceneState.stThreadLuminance.bThreadFlag)
    {
        s_stSceneCTX.stSceneState.stThreadLuminance.bThreadFlag = HI_FALSE;
        pthread_attr_destroy(&(s_stSceneCTX.stSceneState.stThreadluminanceattr));
    }

    if (HI_TRUE == s_stSceneCTX.stSceneState.stThreadWdr.bThreadFlag)
    {
        s_stSceneCTX.stSceneState.stThreadWdr.bThreadFlag = HI_FALSE;
        pthread_attr_destroy(&(s_stSceneCTX.stSceneState.stThreadWdrattr));
    }

    return HI_SUCCESS;
}
#endif

static HI_S32 PDT_SCENE_LoadParam(HI_VOID)
{
//#if (defined(CONFIG_DEBUG) && defined(AMP_LINUX_HUAWEILITE))
    HI_S32 s32Fd = -1;

    s32Fd = open(SCENEAUTO_PARAM_PATH, O_RDWR, 777);
    HI_APPCOMM_CHECK_EXPR_WITH_ERRINFO(s32Fd >= 0, HI_FAILURE, "Open "SCENEAUTO_PARAM_PATH);

    MLOGI("Load param from sceneauto.bin\n");
    ssize_t readCount = read(s32Fd, s_stSceneCTX.pstSceneParam, sizeof(PDT_SCENE_PARAM_S));
    close(s32Fd);
    HI_APPCOMM_CHECK_EXPR_WITH_ERRINFO(0 < readCount, HI_FAILURE, "Read "SCENEAUTO_PARAM_PATH);
//#else
//    extern unsigned char sceneparam_bin[];
//    s_stSceneCTX.pstSceneParam = (PDT_SCENE_PARAM_S*)sceneparam_bin;
//#endif
    return HI_SUCCESS;
}

/**-------------------------external function interface-------------------------*/
HI_S32 HI_PDT_SCENE_Init()
{
    HI_S32 s32Ret = HI_SUCCESS;

    HI_MUTEX_LOCK(s_stSceneCTX.SceneModuleMutex);

    MLOGI("HI_PDT_SCENE_Init .\n");
    if (HI_TRUE == s_stSceneCTX.stSceneState.bSceneInit)
    {
        MLOGE("SCENE module has already been inited.\n");
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
        return HI_SUCCESS;
    }

#if (defined(CONFIG_DEBUG) && defined(AMP_LINUX_HUAWEILITE))
    s_stSceneCTX.pstSceneParam = (PDT_SCENE_PARAM_S*)malloc(sizeof(PDT_SCENE_PARAM_S));
    if (HI_NULL == s_stSceneCTX.pstSceneParam)
    {
        MLOGE("malloc scene param failed.\n");
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
        return HI_SUCCESS;
    }
#endif
    s32Ret = PDT_SCENE_LoadParam();
    if (HI_SUCCESS != s32Ret)
    {
#if (defined(CONFIG_DEBUG) && defined(AMP_LINUX_HUAWEILITE))
        HI_APPCOMM_SAFE_FREE(s_stSceneCTX.pstSceneParam);
#endif
        MLOGE("PDT_SCENE_LoadParam failed.\n");
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
        return HI_FAILURE;
    }

    s32Ret = PDT_SCENE_SetPipeParam(s_stSceneCTX.pstSceneParam->astPipeParam, PDT_SCENE_PIPETYPE_CNT);
    if (HI_SUCCESS != s32Ret)
    {
#if (defined(CONFIG_DEBUG) && defined(AMP_LINUX_HUAWEILITE))
        HI_APPCOMM_SAFE_FREE(s_stSceneCTX.pstSceneParam);
#endif
        MLOGE("HI_PDT_SCENE_SetPipeParam failed.\n");
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
        return HI_FAILURE;
    }
    MLOGI("The PDT_SCENE_PIPETYPE_CNT is %d.\n", PDT_SCENE_PIPETYPE_CNT);

    s_stSceneCTX.stSceneState.bRefreshIgnore = HI_FALSE;
    s_stSceneCTX.stSceneState.bSceneInit = HI_TRUE;
    HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
    MLOGI("SCENE module has been inited successfully.\n");
    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_SetSceneMode(const HI_PDT_SCENE_MODE_S* pstSceneMode)
{
    //dprintf("Lation@ HI_PDT_SCENE_SetSceneMode %d, %d, %d\n", pstSceneMode->astPipeAttr[0].bEnable, pstSceneMode->astPipeAttr[0].MainPipeHdl, pstSceneMode->astPipeAttr[0].u8PipeParamIndex);

    PDT_SCENE_CHECK_INIT();
    HI_APPCOMM_CHECK_POINTER(pstSceneMode, HI_PDT_SCENE_EINVAL);

    HI_S32 i = 0;
    HI_S32 s32Ret = HI_SUCCESS;

    HI_MUTEX_LOCK(s_stSceneCTX.SceneModuleMutex);

            MLOGI("HI_PDT_SCENE_SetSceneMode 4\n");
    for (i = 0; i < HI_PDT_SCENE_PIPE_MAX_NUM; i++)
    {
        if (HI_TRUE != pstSceneMode->astPipeAttr[i].bEnable)
        {
            continue;
        }

        if (HI_PDT_SCENE_PIPE_TYPE_SNAP != pstSceneMode->astPipeAttr[i].enPipeType && HI_PDT_SCENE_PIPE_TYPE_VIDEO != pstSceneMode->astPipeAttr[i].enPipeType)
        {
            MLOGE("Pipe Type is not video or snap!\n");
            HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
            return HI_PDT_SCENE_EINVAL;
        }

        if (pstSceneMode->astPipeAttr[i].u8PipeParamIndex < 0 || pstSceneMode->astPipeAttr[i].u8PipeParamIndex >= PDT_SCENE_PIPETYPE_CNT)
        {
            MLOGE("Pipe param index is out of range!\n");
            HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
            return HI_PDT_SCENE_EINVAL;
        }
    }

    memcpy(&s_stSceneCTX.stSceneMode, pstSceneMode, sizeof(HI_PDT_SCENE_MODE_S));

    s32Ret = PDT_SCENE_SetMainPipeState(pstSceneMode);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_SetMainIspState failed.\n");
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
        return HI_FAILURE;
    }
#ifdef CONFIG_SCENEAUTO_SUPPORT

        MLOGI("PDT_SCENE_SetVIPipeParam \n");
    s32Ret = PDT_SCENE_SetVIPipeParam();
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_SetIspParam failed.\n");
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
        return HI_FAILURE;
    }

    extern HI_S32 PDT_SCENE_SetStatic3DNR(const HI_PDT_SCENE_MODE_S* pstSceneMode);
    s32Ret = PDT_SCENE_SetStatic3DNR(pstSceneMode);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("PDT_SCENE_SetStaticThreeDNR failed.\n");
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
        return HI_FAILURE;
    }

    s32Ret = PDT_SCENE_SetVPSSParam();
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_SetVPSSParam failed.\n");
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
        return HI_FAILURE;
    }

    s32Ret = PDT_SCENE_SetVENCParam();
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_SetVPSSParam failed.\n");
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
        return HI_FAILURE;
    }

    s32Ret = PDT_SCENE_StartAutoThread();
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("SCENE_StartThread failed.\n");
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
        return HI_FAILURE;
    }

    s32Ret = PDT_SCENE_SetDelayStaticParamThread();
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("PDT_SCENE_SetDelayStaticParam failed.\n");
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);
        return HI_FAILURE;
    }
#endif
    HI_MUTEX_UNLOCK(s_stSceneCTX.SceneModuleMutex);

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_GetSceneMode(HI_PDT_SCENE_MODE_S* pstSceneMode)
{
    PDT_SCENE_CHECK_INIT();
    HI_APPCOMM_CHECK_POINTER(pstSceneMode, HI_PDT_SCENE_EINVAL);

    memcpy(pstSceneMode, &s_stSceneCTX.stSceneMode, sizeof(HI_PDT_SCENE_MODE_S));

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_Pause(HI_BOOL bEnable)
{
    PDT_SCENE_CHECK_INIT();

    HI_S32 s32Ret = HI_SUCCESS;

    s_stSceneCTX.stSceneState.bPause = bEnable;

    s32Ret = PDT_SCENE_SetPause(bEnable);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_SetISO(HI_HANDLE VcapPipeHdl, HI_U32 u32ISO)
{
    PDT_SCENE_CHECK_INIT();

    if (u32ISO != 0 && (u32ISO < 100 || u32ISO > 6400))
    {
        return HI_PDT_SCENE_EINVAL;
    }

    ISP_EXPOSURE_ATTR_S stExposure;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Index = 0;
    static HI_U32 u32AutoMax = 0;
    static HI_U32 u32AutoMin = 0;
    static HI_U32 u32LastIso = 0;
    static HI_BOOL bAERouteExValid = HI_FALSE;

    s32Ret = PDT_SCENE_GetMainPipeIndex(VcapPipeHdl, &s32Index);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    s32Ret = HI_MPI_ISP_GetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stExposure);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    if (0 == u32LastIso)
    {
        u32AutoMax = stExposure.stAuto.stSysGainRange.u32Max;
        u32AutoMin = stExposure.stAuto.stSysGainRange.u32Min;
        bAERouteExValid = stExposure.bAERouteExValid;
    }

    u32LastIso = u32ISO;

    if (0 != u32ISO)
    {
        stExposure.enOpType = OP_TYPE_AUTO;
        stExposure.bAERouteExValid = HI_FALSE;
        stExposure.stAuto.stSysGainRange.u32Max = u32ISO / 100 * 1024;
        stExposure.stAuto.stSysGainRange.u32Min = u32ISO / 100 * 1024;
    }
    else
    {
        stExposure.enOpType = OP_TYPE_AUTO;
        stExposure.bAERouteExValid = bAERouteExValid;
        stExposure.stAuto.stSysGainRange.u32Max = u32AutoMax;
        stExposure.stAuto.stSysGainRange.u32Min = u32AutoMin;
    }

    s32Ret = HI_MPI_ISP_SetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stExposure);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_SetEV(HI_HANDLE VcapPipeHdl, HI_PDT_SCENE_EV_E enEV)
{
    PDT_SCENE_CHECK_INIT();

    ISP_EXPOSURE_ATTR_S stExposure;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Index = 0;

    s32Ret = PDT_SCENE_GetMainPipeIndex(VcapPipeHdl, &s32Index);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    s32Ret = HI_MPI_ISP_GetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stExposure);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    stExposure.enOpType = OP_TYPE_AUTO;

    switch (enEV)
    {

        case HI_PDT_SCENE_EV_0:
            stExposure.stAuto.u16EVBias = PDT_SCENE_ORIGINAL_EVBIAS * pow(2, 0);
            break;

        case HI_PDT_SCENE_EV_0_5:
            stExposure.stAuto.u16EVBias = PDT_SCENE_ORIGINAL_EVBIAS * pow(2, 0.5);
            break;

        case HI_PDT_SCENE_EV_B0_5:
            stExposure.stAuto.u16EVBias = PDT_SCENE_ORIGINAL_EVBIAS * pow(2, -0.5);
            break;

        case HI_PDT_SCENE_EV_1:
            stExposure.stAuto.u16EVBias = PDT_SCENE_ORIGINAL_EVBIAS * pow(2, 1);
            break;

        case HI_PDT_SCENE_EV_B1:
            stExposure.stAuto.u16EVBias = PDT_SCENE_ORIGINAL_EVBIAS * pow(2, -1);
            break;

        case HI_PDT_SCENE_EV_1_5:
            stExposure.stAuto.u16EVBias = PDT_SCENE_ORIGINAL_EVBIAS * pow(2, 1.5);
            break;

        case HI_PDT_SCENE_EV_B1_5:
            stExposure.stAuto.u16EVBias = PDT_SCENE_ORIGINAL_EVBIAS * pow(2, -1.5);
            break;

        case HI_PDT_SCENE_EV_2:
            stExposure.stAuto.u16EVBias = PDT_SCENE_ORIGINAL_EVBIAS * pow(2, 2);
            break;

        case HI_PDT_SCENE_EV_B2:
            stExposure.stAuto.u16EVBias = PDT_SCENE_ORIGINAL_EVBIAS * pow(2, -2);
            break;
        default:
            MLOGE("Error Ev!\n");
            return HI_PDT_SCENE_EINVAL;
    }

    s32Ret = HI_MPI_ISP_SetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stExposure);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_SetAntiflicker(HI_HANDLE VcapPipeHdl, HI_PDT_SCENE_ANTIFLICKER_E enAntiflicker)
{
    PDT_SCENE_CHECK_INIT();

    ISP_EXPOSURE_ATTR_S stExposure;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Index = 0;
    s32Ret = PDT_SCENE_GetMainPipeIndex(VcapPipeHdl, &s32Index);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    s32Ret = HI_MPI_ISP_GetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stExposure);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    stExposure.enOpType = OP_TYPE_AUTO;
    HI_MUTEX_LOCK(s_stSceneCTX.SceneStateMutex);
    switch (enAntiflicker)
    {

        case HI_PDT_SCENE_ANTIFLICKER_NONE:
            stExposure.stAuto.stAntiflicker.bEnable = HI_FALSE;
            s_stSceneCTX.stSceneState.astMainPipe[s32Index].bAntiFlikerFixed = HI_FALSE;
            break;

        case HI_PDT_SCENE_ANTIFLICKER_50:
            stExposure.stAuto.stAntiflicker.bEnable = HI_TRUE;
            stExposure.stAuto.stAntiflicker.u8Frequency = 50;
            stExposure.stAuto.stAntiflicker.enMode = ISP_ANTIFLICKER_NORMAL_MODE;
            s_stSceneCTX.stSceneState.astMainPipe[s32Index].bAntiFlikerFixed = HI_TRUE;
            break;

        case HI_PDT_SCENE_ANTIFLICKER_60:
            stExposure.stAuto.stAntiflicker.bEnable = HI_TRUE;
            stExposure.stAuto.stAntiflicker.u8Frequency = 60;
            stExposure.stAuto.stAntiflicker.enMode = ISP_ANTIFLICKER_NORMAL_MODE;
            s_stSceneCTX.stSceneState.astMainPipe[s32Index].bAntiFlikerFixed = HI_TRUE;
            break;
        default:
            MLOGE("Error antiflicker choice!\n");
            HI_MUTEX_UNLOCK(s_stSceneCTX.SceneStateMutex);
            return HI_PDT_SCENE_EINVAL;
    }   
    s32Ret = HI_MPI_ISP_SetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stExposure);
    if (HI_SUCCESS != s32Ret)
    {
        HI_MUTEX_UNLOCK(s_stSceneCTX.SceneStateMutex);
        MLOGE("HI_MPI_ISP_SetExposureAttr error!\n");
        return HI_PDT_SCENE_EINTER;
    }
    HI_MUTEX_UNLOCK(s_stSceneCTX.SceneStateMutex);

    return HI_SUCCESS;
}


HI_S32 HI_PDT_SCENE_SetWB(HI_HANDLE VcapPipeHdl, HI_U32 u32WB)
{
    PDT_SCENE_CHECK_INIT();
    if (u32WB != 0 && (u32WB < 1500 || u32WB > 15000))
    {
        return HI_PDT_SCENE_EINVAL;
    }

    ISP_WB_ATTR_S stWbAttr;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Index = 0;

    HI_U16 u16ColorTemp = (HI_U16)u32WB;

    s32Ret = PDT_SCENE_GetMainPipeIndex(VcapPipeHdl, &s32Index);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);


    s32Ret = HI_MPI_ISP_GetWBAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stWbAttr);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    if (u32WB == 0)
    {
        stWbAttr.bByPass = HI_FALSE; 
        stWbAttr.enOpType =  OP_TYPE_AUTO;
    }
    else
    {
        HI_U16 u16AwbGain[4];

        s32Ret = HI_MPI_ISP_CalGainByTemp(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stWbAttr, u16ColorTemp, 0, u16AwbGain);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

        
      /*  if(off_auto>0)
        {
           
            stWbAttr.stManual.u16Rgain = set_AwbGain.rgain;
            stWbAttr.stManual.u16Grgain = set_AwbGain.grgain;
            stWbAttr.stManual.u16Gbgain = set_AwbGain.gbgain;
            stWbAttr.stManual.u16Bgain = set_AwbGain.bgain;

            set_AwbGain_bak.rgain = set_AwbGain.rgain;
            set_AwbGain_bak.grgain = set_AwbGain.grgain;
            set_AwbGain_bak.gbgain = set_AwbGain.gbgain;
            set_AwbGain_bak.bgain = set_AwbGain.bgain;
        }
        else
        {*/
            stWbAttr.stManual.u16Rgain = u16AwbGain[0];
            stWbAttr.stManual.u16Grgain = u16AwbGain[1];
            stWbAttr.stManual.u16Gbgain = u16AwbGain[2];
            stWbAttr.stManual.u16Bgain = u16AwbGain[3];            
             /*   set_AwbGain_bak.rgain = u16AwbGain[0];
                set_AwbGain_bak.grgain = u16AwbGain[1];
                set_AwbGain_bak.gbgain = u16AwbGain[2];
                set_AwbGain_bak.bgain = u16AwbGain[3];*/
      /*  }*/
        stWbAttr.bByPass = HI_FALSE;
        stWbAttr.enOpType = OP_TYPE_MANUAL;
    }

    s32Ret = HI_MPI_ISP_SetWBAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stWbAttr);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_SetExpTime(HI_HANDLE VcapPipeHdl, HI_U32 u32ExpTime_us)
{
    PDT_SCENE_CHECK_INIT();

        MLOGE("HI_PDT_SCENE_SetExpTime11 %d\n",u32ExpTime_us);
    /**The Time must smaller than 1/FrameRate*/
    if (u32ExpTime_us != 0 && u32ExpTime_us > PDT_SCENE_EXPTIME_VIDEOLIMIT_US)
    {
        return HI_PDT_SCENE_EINVAL;
    }

    ISP_EXPOSURE_ATTR_S stExposure;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Index = 0;
    static HI_U32 u32AutoMax = 0;
    static HI_U32 u32AutoMin = 0;
    static HI_U32 u32LastExpTime = 0;

    s32Ret = PDT_SCENE_GetMainPipeIndex(VcapPipeHdl, &s32Index);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    s32Ret = HI_MPI_ISP_GetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stExposure);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    if (0 == u32LastExpTime)
    {
        u32AutoMax = stExposure.stAuto.stExpTimeRange.u32Max;
        u32AutoMin = stExposure.stAuto.stExpTimeRange.u32Min;
    }

    u32LastExpTime = u32ExpTime_us;

    if (0 != u32ExpTime_us)
    {
        stExposure.enOpType = OP_TYPE_AUTO;
        stExposure.stAuto.stExpTimeRange.u32Max = u32ExpTime_us;
        stExposure.stAuto.stExpTimeRange.u32Min = u32ExpTime_us;
    }
    else
    {
        stExposure.enOpType = OP_TYPE_AUTO;
        stExposure.stAuto.stExpTimeRange.u32Max = u32AutoMax;
        stExposure.stAuto.stExpTimeRange.u32Min = u32AutoMin;
    }

    s32Ret = HI_MPI_ISP_SetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stExposure);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_SetMetry(HI_HANDLE VcapPipeHdl, const HI_PDT_SCENE_METRY_S* pstMetry)
{
    PDT_SCENE_CHECK_INIT();
    HI_APPCOMM_CHECK_POINTER(pstMetry,HI_PDT_SCENE_EINVAL);

    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32Index = 0;
    HI_U32 u32ArrayIndex = 0;
    ISP_STATISTICS_CFG_S stStatisticsCfg;

    s32Ret = PDT_SCENE_GetMainPipeIndex(VcapPipeHdl, &s32Index);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    s32Ret = HI_MPI_ISP_GetStatisticsConfig(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stStatisticsCfg);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    switch(pstMetry->enMetryType)
    {
        case HI_PDT_SCENE_METRY_TYPE_CENTER:
            u32ArrayIndex = pstMetry->unParam.stCenterMetryParam.u32Index;
            memcpy(stStatisticsCfg.stAECfg.au8Weight, s_stSceneCTX.pstSceneParam->stExParam.astCenterMetryAeWeight[u32ArrayIndex].au8AEWeight, \
                               sizeof(stStatisticsCfg.stAECfg.au8Weight));
            s32Ret = HI_MPI_ISP_SetStatisticsConfig(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stStatisticsCfg);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
            s_stSceneCTX.stSceneState.astMainPipe[s32Index].bMetryFixed = HI_TRUE;
            break;

        case HI_PDT_SCENE_METRY_TYPE_AVERAGE:
            u32ArrayIndex = pstMetry->unParam.stAverageMetryParam.u32Index;
            memcpy(stStatisticsCfg.stAECfg.au8Weight, s_stSceneCTX.pstSceneParam->stExParam.astAvergeMetryAeWeight[u32ArrayIndex].au8AEWeight, \
                               sizeof(stStatisticsCfg.stAECfg.au8Weight));
            s32Ret = HI_MPI_ISP_SetStatisticsConfig(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stStatisticsCfg);
            HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
            s_stSceneCTX.stSceneState.astMainPipe[s32Index].bMetryFixed = HI_FALSE;
            break;

        case HI_PDT_SCENE_METRY_TYPE_SPOT:
            {
                HI_U8 au8AEWeight[AE_ZONE_ROW][AE_ZONE_COLUMN] = {0};

                HI_APPCOMM_CHECK_EXPR(HI_TRUE == HI_APPCOMM_CHECK_RANGE(pstMetry->unParam.stSpotMetryParam.stPoint.s32X, 0, 100), HI_PDT_SCENE_EINVAL);
                HI_APPCOMM_CHECK_EXPR(HI_TRUE == HI_APPCOMM_CHECK_RANGE(pstMetry->unParam.stSpotMetryParam.stPoint.s32Y, 0, 100), HI_PDT_SCENE_EINVAL);
                HI_APPCOMM_CHECK_EXPR(HI_TRUE == HI_APPCOMM_CHECK_RANGE(pstMetry->unParam.stSpotMetryParam.stSize.u32Height, 0, 100), HI_PDT_SCENE_EINVAL);
                HI_APPCOMM_CHECK_EXPR(HI_TRUE == HI_APPCOMM_CHECK_RANGE(pstMetry->unParam.stSpotMetryParam.stSize.u32Width, 0, 100), HI_PDT_SCENE_EINVAL);
                HI_S32 s32X = pstMetry->unParam.stSpotMetryParam.stPoint.s32X;
                HI_S32 s32Y = pstMetry->unParam.stSpotMetryParam.stPoint.s32Y;
                HI_U32 u32Height = pstMetry->unParam.stSpotMetryParam.stSize.u32Height;
                HI_U32 u32Weight = pstMetry->unParam.stSpotMetryParam.stSize.u32Width;

                HI_U8 u8StartRowPos = (HI_U8)((HI_FLOAT)(s32X - u32Weight/2) / 100.0 * AE_ZONE_ROW);
                HI_U8 u8EndRowPos = (HI_U8)((HI_FLOAT)(s32X + u32Weight/2) / 100.0 * AE_ZONE_ROW) + 1;
                HI_U8 u8StartColumnPos = (HI_U8)((HI_FLOAT)(s32Y - u32Height/2) / 100.0 * AE_ZONE_COLUMN);
                HI_U8 u8EndColumnPos = (HI_U8)((HI_FLOAT)(s32Y + u32Height/2) / 100.0 * AE_ZONE_COLUMN) + 1;

                HI_S32 i, j = 0;

                /**Margin Check*/
                if ((u8StartRowPos - 1) < 0 ||
                    (u8StartColumnPos - 1) < 0 ||
                    u8EndRowPos < (u8StartRowPos - 1) ||
                    u8EndColumnPos < (u8StartColumnPos - 1) ||
                    (u8StartRowPos - 1)  > AE_ZONE_ROW ||
                    u8EndRowPos > AE_ZONE_ROW ||
                    (u8StartColumnPos - 1)  > AE_ZONE_COLUMN ||
                    u8EndColumnPos > AE_ZONE_ROW
                    )
                {
                    MLOGE("Spot Metry param is error.\n");
                    return HI_PDT_SCENE_EINTER;
                }

                for (i = u8StartRowPos - 1; i < u8EndRowPos; i++)
                {
                    for (j = u8StartColumnPos - 1; j < u8EndColumnPos; j++)
                    {
                        au8AEWeight[i][j] = 1;
                    }
                }
                memcpy(stStatisticsCfg.stAECfg.au8Weight, au8AEWeight, sizeof(stStatisticsCfg.stAECfg.au8Weight));
                s32Ret = HI_MPI_ISP_SetStatisticsConfig(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stStatisticsCfg);
                HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
                s_stSceneCTX.stSceneState.astMainPipe[s32Index].bMetryFixed = HI_TRUE;
            }
        default:
            s_stSceneCTX.stSceneState.astMainPipe[s32Index].bMetryFixed = HI_FALSE;
            break;
    }


    return HI_SUCCESS;
}
HI_S32 DHS_Set_config_for_ISP(const DHS_ISP_SET_S* Set_SceneMode)
{       
   // off_auto = Set_SceneMode->DHS_AUTO;   
   // if(off_auto>0)
   // {
        
        //set_IspExp.again=Set_SceneMode->again;
        //set_IspExp.dgain=Set_SceneMode->dgain;
        //set_IspExp.ispgain=Set_SceneMode->ispgain;
        //set_IspExp.exptime=Set_SceneMode->exptime;
        
       // MLOGI(" set_IspExp.again =%d \n", set_IspExp.again);    
       // MLOGI(" set_IspExp.dgain =%d \n", set_IspExp.dgain);   
       /// MLOGI(" set_IspExp.ispgain =%d \n", set_IspExp.ispgain);   
       // MLOGI(" set_IspExp.exptime =%d \n", set_IspExp.exptime);  
        ISP_EXPOSURE_ATTR_S lstExposure;
        HI_S32 s32Ret = HI_SUCCESS;

        HI_S32 s32Index = 0;

        s32Ret = PDT_SCENE_GetMainPipeIndex(0, &s32Index);
         HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

        s32Ret = HI_MPI_ISP_GetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &lstExposure);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
        
            lstExposure.bByPass = HI_FALSE;
            lstExposure.enOpType = OP_TYPE_MANUAL;
            lstExposure.stManual.enAGainOpType = OP_TYPE_MANUAL;
            lstExposure.stManual.enDGainOpType = OP_TYPE_MANUAL;
            lstExposure.stManual.enISPDGainOpType = OP_TYPE_MANUAL;
            lstExposure.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
         //   lstExposure.stManual.u32AGain = Set_SceneMode->again;
         //   lstExposure.stManual.u32DGain =Set_SceneMode->dgain;
         //   lstExposure.stManual.u32ISPDGain = Set_SceneMode->ispgain;
         //   lstExposure.stManual.u32ExpTime= Set_SceneMode->exptime; 
        s32Ret = HI_MPI_ISP_SetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &lstExposure);
  //  }
  //  MLOGI(" off_auto =%d \n", off_auto);    
    return HI_SUCCESS;
}
HI_S32 DHS_Set_config_for_AWB(const DHS_AWB_SET_S* Set_SceneMode)
{
    ISP_WB_ATTR_S stWbAttr;
    HI_S32 s32Index = 0;
    HI_S32 s32Ret = HI_SUCCESS;

        s32Ret = PDT_SCENE_GetMainPipeIndex(0, &s32Index);
         HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
         
        s32Ret = HI_MPI_ISP_GetWBAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stWbAttr);     
        

        stWbAttr.bByPass=0;
        stWbAttr.u8AWBRunInterval=2;
        stWbAttr.enAlgType=ALG_AWB;
        stWbAttr.enOpType=Set_SceneMode->DHS_type;
        if( stWbAttr.enOpType == OP_TYPE_MANUAL)
        {

                    printf("set DHS_Set_AWB OP_TYPE_MANUAL %d\r\n",stWbAttr.stAuto.bEnable);
            #ifdef SHWE_EASOBN_LOG   
                MLOGI(" B Set_SceneMode->rgain =%d \n", stWbAttr.stManual.u16Rgain);    //256
                MLOGI(" B Set_SceneMode->grgain =%d \n", stWbAttr.stManual.u16Grgain);   //256
                MLOGI(" B Set_SceneMode->gbgain =%d \n", stWbAttr.stManual.u16Gbgain);   //256
                MLOGI(" B Set_SceneMode->bgain =%d \n", stWbAttr.stManual.u16Bgain);  //256
            #endif     
                stWbAttr.stManual.u16Rgain = Set_SceneMode->rgain;
                stWbAttr.stManual.u16Grgain = Set_SceneMode->grgain;
                stWbAttr.stManual.u16Gbgain = Set_SceneMode->gbgain;
                stWbAttr.stManual.u16Bgain = Set_SceneMode->bgain ;
                
            #ifdef SHWE_EASOBN_LOG  
                MLOGI(" A stWbAttr.enOpType=%d \n", stWbAttr.enOpType); 
                MLOGI(" A Set_SceneMode->rgain =%d \n", stWbAttr.stManual.u16Rgain);    
                MLOGI(" A Set_SceneMode->grgain =%d \n", stWbAttr.stManual.u16Grgain);   
                MLOGI(" A Set_SceneMode->gbgain =%d \n", stWbAttr.stManual.u16Gbgain);   
                MLOGI(" A Set_SceneMode->bgain =%d \n", stWbAttr.stManual.u16Bgain);   
            #endif     
        }
        else
        {
            
                stWbAttr.enOpType =OP_TYPE_AUTO;
                stWbAttr.stAuto.bEnable=Set_SceneMode->DHS_AUTO;
                stWbAttr.stAuto.u16RefColorTemp=4850;
                stWbAttr.stAuto.au16StaticWB[0]=Set_SceneMode->StaticWB[0];
                stWbAttr.stAuto.au16StaticWB[1]=Set_SceneMode->StaticWB[1];
                stWbAttr.stAuto.au16StaticWB[2]=Set_SceneMode->StaticWB[2];
                stWbAttr.stAuto.au16StaticWB[3]=Set_SceneMode->StaticWB[3];
            
            #ifdef SHWE_EASOBN_LOG       
                MLOGI(" A stWbAttr.enOpType=%d \n", stWbAttr.enOpType); 
                MLOGI(" A stWbAttr.stAuto.bEnable =%d \n", stWbAttr.stAuto.bEnable); 
                MLOGI(" A stWbAttr.stAuto.au16StaticWB[0] =%d \n", stWbAttr.stAuto.au16StaticWB[0]); 
                MLOGI(" A stWbAttr.stAuto.au16StaticWB[1] =%d \n", stWbAttr.stAuto.au16StaticWB[1]); 
                MLOGI(" A stWbAttr.stAuto.au16StaticWB[2] =%d \n", stWbAttr.stAuto.au16StaticWB[2]); 
                MLOGI(" A stWbAttr.stAuto.au16StaticWB[3] =%d \n", stWbAttr.stAuto.au16StaticWB[3]);  
            #endif     

                stWbAttr.stAuto.as32CurvePara[0]=80;
                stWbAttr.stAuto.as32CurvePara[1]=5;
                stWbAttr.stAuto.as32CurvePara[2]=-171;
                stWbAttr.stAuto.as32CurvePara[3]=181935;
                stWbAttr.stAuto.as32CurvePara[4]=128;
                stWbAttr.stAuto.as32CurvePara[5]=-136460;  

                stWbAttr.stAuto.enAlgType=AWB_ALG_ADVANCE;
                stWbAttr.stAuto.u8RGStrength=128;
                stWbAttr.stAuto.u8BGStrength=128;
                stWbAttr.stAuto.u16Speed=256;
                stWbAttr.stAuto.u16ZoneSel=32;
                stWbAttr.stAuto.u16HighColorTemp=10000;
                stWbAttr.stAuto.u16LowColorTemp=2800;
                stWbAttr.stAuto.stCTLimit.bEnable=1;
                stWbAttr.stAuto.bShiftLimitEn=1;
                stWbAttr.stAuto.bGainNormEn=1;

                stWbAttr.stAuto.bNaturalCastEn=1;
                stWbAttr.stAuto.stCbCrTrack.bEnable=1;
                stWbAttr.stAuto.stLumaHist.bEnable=1;
                stWbAttr.stAuto.bAWBZoneWtEn=0;
                for(int i=0;i<AWB_ZONE_ORIG_ROW;i++)
                {
                    for(int j=0;j<AWB_ZONE_ORIG_COLUMN;j++)
                    {
                        stWbAttr.stAuto.au8ZoneWt[i *j] =8 ;
                    }
                }   
            
            printf("set DHS_Set_AWB OP_TYPE_AUTO %d\r\n",stWbAttr.stAuto.bEnable);
        }
        
        
        s32Ret = HI_MPI_ISP_SetWBAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stWbAttr);
    return HI_SUCCESS;
       
}
HI_S32 DHS_Get_config_for_ISP( DHS_ISP_SET_S* Set_SceneMode)
{ 


        ISP_EXPOSURE_ATTR_S lstExposure;
        HI_S32 s32Index = 0;
    HI_S32 s32Ret = HI_SUCCESS;

        s32Ret = PDT_SCENE_GetMainPipeIndex(0, &s32Index);
         HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

        s32Ret = HI_MPI_ISP_GetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &lstExposure);
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
       // Set_SceneMode->again=set_IspExp_bak.again;
       // Set_SceneMode->dgain=set_IspExp_bak.dgain;
       // Set_SceneMode->ispgain=set_IspExp_bak.ispgain;
       // Set_SceneMode->exptime=set_IspExp_bak.exptime;
        
    return HI_SUCCESS;
}
HI_S32 DHS_Get_config_for_AWB( DHS_AWB_SET_S* Set_SceneMode)
{  

     ISP_WB_ATTR_S stWbAttr;
        HI_S32 s32Index = 0;
    HI_S32 s32Ret = HI_SUCCESS;

        s32Ret = PDT_SCENE_GetMainPipeIndex(0, &s32Index);
         HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
         
        s32Ret = HI_MPI_ISP_GetWBAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &stWbAttr);    
        HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);    

       // memcpy(&Set_SceneMode->stWb_ATTR, &stWbAttr, sizeof(ISP_WB_ATTR_S));
       Set_SceneMode->rgain = stWbAttr.stManual.u16Rgain;
       Set_SceneMode->grgain = stWbAttr.stManual.u16Grgain ;
       Set_SceneMode->gbgain = stWbAttr.stManual.u16Gbgain ;
       Set_SceneMode->bgain = stWbAttr.stManual.u16Bgain;  

       Set_SceneMode->StaticWB[0] =stWbAttr.stAuto.au16StaticWB[0];
       Set_SceneMode->StaticWB[1] =stWbAttr.stAuto.au16StaticWB[1];
       Set_SceneMode->StaticWB[2] =stWbAttr.stAuto.au16StaticWB[2];
       Set_SceneMode->StaticWB[3] =stWbAttr.stAuto.au16StaticWB[3];

    return HI_SUCCESS;
}
HI_S32 DHS_PDT_SCENE_Set_Exp(HI_HANDLE VcapPipeHdl,const ISP_EXPOSURE_ATTR_S*  stExposure)
{
    PDT_SCENE_CHECK_INIT();

    ISP_EXPOSURE_ATTR_S lstExposure;
    HI_S32 s32Ret = HI_SUCCESS;

    HI_S32 s32Index = 0;

    s32Ret = PDT_SCENE_GetMainPipeIndex(VcapPipeHdl, &s32Index);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    s32Ret = HI_MPI_ISP_GetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &lstExposure);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);


        MLOGI(" F.bByPass =%d \n", lstExposure.bByPass);  
        MLOGI(" F.enOpType =%d \n", lstExposure.enOpType);   
        MLOGI(" F.enAGainOpType =%d \n", lstExposure.stManual.enAGainOpType);  // 0 
        MLOGI(" F.enDGainOpType =%d \n", lstExposure.stManual.enDGainOpType);   // 0
        MLOGI(" F.enISPDGainOpType =%d \n", lstExposure.stManual.enISPDGainOpType);    ////0
        MLOGI(" F.enExpTimeOpType =%d \n", lstExposure.stManual.enExpTimeOpType);    //0
        MLOGI(" F.u32AGain =%d \n", lstExposure.stManual.u32AGain);    //1024
        MLOGI(" F.u32DGain =%d \n", lstExposure.stManual.u32DGain);   //1024
        MLOGI(" F.u32ISPDGain =%d \n", lstExposure.stManual.u32ISPDGain);   //1024
        MLOGI(" F.u32ExpTime =%d \n", lstExposure.stManual.u32ExpTime);    //16384
        lstExposure.bByPass                     =stExposure->bByPass                       ;
        lstExposure.enOpType                    =stExposure->enOpType                      ;
        lstExposure.stManual.enAGainOpType      =stExposure->stManual.enAGainOpType        ;
        lstExposure.stManual.enDGainOpType      =stExposure->stManual.enDGainOpType        ;
        lstExposure.stManual.enISPDGainOpType   =stExposure->stManual.enISPDGainOpType     ;
        lstExposure.stManual.enExpTimeOpType    =stExposure->stManual.enExpTimeOpType      ;
        lstExposure.stManual.u32AGain           =stExposure->stManual.u32AGain             ;
        lstExposure.stManual.u32DGain           =stExposure->stManual.u32DGain             ;
        lstExposure.stManual.u32ISPDGain        =stExposure->stManual.u32ISPDGain          ;
        lstExposure.stManual.u32ExpTime         =stExposure->stManual.u32ExpTime           ;

        MLOGI(" L.bByPass =%d \n", lstExposure.bByPass);    
        MLOGI(" L.enOpType =%d \n", lstExposure.enOpType); 
        MLOGI(" L.enAGainOpType =%d \n", lstExposure.stManual.enAGainOpType);    
        MLOGI(" L.enDGainOpType =%d \n", lstExposure.stManual.enDGainOpType);    
        MLOGI(" L.enISPDGainOpType =%d \n", lstExposure.stManual.enISPDGainOpType);    
        MLOGI(" L.enExpTimeOpType =%d \n", lstExposure.stManual.enExpTimeOpType);    
        MLOGI(" L.u32AGain =%d \n", lstExposure.stManual.u32AGain);    
        MLOGI(" L.32DGain =%d \n", lstExposure.stManual.u32DGain);   
        MLOGI(" L.u32ISPDGain =%d \n", lstExposure.stManual.u32ISPDGain);   
        MLOGI(" L.u32ExpTime =%d \n", lstExposure.stManual.u32ExpTime);    
    s32Ret = HI_MPI_ISP_SetExposureAttr(s_stSceneCTX.stSceneState.astMainPipe[s32Index].MainPipeHdl, &lstExposure);
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);

    return HI_SUCCESS;
}
static HI_S32 VI_Restore(VI_PIPE Pipe)
{
    HI_S32 s32Ret= HI_FAILURE;

    if(VB_INVALID_POOLID != stFrame.u32PoolId)
    {
        s32Ret = HI_MPI_VI_ReleasePipeFrame(Pipe, &stFrame);
        if(HI_SUCCESS != s32Ret)
        {
             printf("Release Chn Frame error!!!\n");
        }
        stFrame.u32PoolId = VB_INVALID_POOLID;
    }
    if(-1 != hHandle)
    {
        HI_MPI_VGS_CancelJob(hHandle);
        hHandle = -1;
    }
    if(HI_NULL != stMem.pVirAddr)
    {
        HI_MPI_SYS_Munmap((HI_VOID*)stMem.pVirAddr, u32BlkSize );
        stMem.u64PhyAddr = HI_NULL;
    }
    if(VB_INVALID_POOLID != stMem.hPool)
    {
        HI_MPI_VB_ReleaseBlock(stMem.hBlock);
        stMem.hPool = VB_INVALID_POOLID;
    }

    if (VB_INVALID_POOLID != hPool)
    {
        HI_MPI_VB_DestroyPool( hPool );
        hPool = VB_INVALID_POOLID;
    }

    if(HI_NULL != pUserPageAddr[0])
    {
        HI_MPI_SYS_Munmap(pUserPageAddr[0], u32Size);
        pUserPageAddr[0] = HI_NULL;
    }

    if(pfd)
    {
        fclose(pfd);
        pfd = HI_NULL;
    }

    s32Ret = HI_MPI_VI_SetPipeDumpAttr(Pipe, &astBackUpDumpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Set Pipe %d dump attr failed!\n",Pipe);
        return s32Ret;
    }

    return HI_SUCCESS;
}


/*
static int tyuv2jpeg(unsigned char *yuv_buffer, int yuv_size, int width, int height,
                     int subsample, unsigned char **jpeg_buffer, unsigned long *jpeg_size,
                     int quality)
{

    tjhandle handle = NULL;
    int flags = 0;
    int padding = 1; //must be 1 or 4
    int need_size = 0;
    int ret = 0;

    handle = tjInitCompress();

    flags |= 0;

    need_size = tjBufSizeYUV2(width, padding, height, subsample);
    if (need_size != yuv_size)
    {
        printf("we detect yuv size: %d, but you give: %d, check again.\n",
               need_size, yuv_size);
        return -1;
    }

    ret = tjCompressFromYUV(handle, yuv_buffer, width, padding, height,
                            subsample, jpeg_buffer, jpeg_size, quality, flags);
    if (ret < 0)
    {
        printf("compress to jpeg failed: %s\n", tjGetErrorStr());
    }

    tjDestroy(handle);
    return ret;
}
static HI_S32 vi_dump_save_one_frame(VIDEO_FRAME_S* pVBuf, int sfd)
{   
    // by eason
    unsigned int w, h;
    char* pVBufVirt_Y;
    char* pVBufVirt_C;
   // char* pMemContent;
   // unsigned char TmpBuff[MAX_FRM_WIDTH];
    HI_U32 phy_addr, size;
    HI_CHAR* pUserPageAddr[2];
    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
    //HI_U32 u32UvHeight;// UV height for planar format 

    if (pVBuf->u32Width > MAX_FRM_WIDTH)
    {
        printf("Over max frame width: %d, can't support.\n", MAX_FRM_WIDTH);
        return HI_FAILURE;
    }

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
    {
        size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height) * 3 / 2;
        //u32UvHeight = pVBuf->u32Height / 2;
    }
    else if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
    {
        size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height) * 2;
       // u32UvHeight = pVBuf->u32Height;
    }
    else if (PIXEL_FORMAT_YUV_400 == enPixelFormat)
    {
        size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height);
       // u32UvHeight = 0;
    }
    else
    {
        printf("not support pixelformat: %d\n", enPixelFormat);
        return HI_FAILURE;
    }

   phy_addr = pVBuf->u64PhyAddr[0];

    pUserPageAddr[0] = (HI_CHAR*) HI_MPI_SYS_Mmap(phy_addr, size);
    if (NULL == pUserPageAddr[0])
    {
        return HI_FAILURE;
    }
    printf("stride: %d,%d\n", pVBuf->u32Stride[0], pVBuf->u32Stride[1] );

    pVBufVirt_Y = pUserPageAddr[0];
    pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0]) * (pVBuf->u32Height);
    
    int Set_H = 1000; //設定高
    int Set_W = 1000; //設定寬
    int Do_H = pVBuf->u32Height; //設定高
    int Do_W = pVBuf->u32Width;  //設定寬
    int startH = 0;
    int startW = 0;
     if (Do_H > Set_H)
    {
        startH = (Do_H - Set_H) / 2;
        Do_H = Set_H;
    }
    if (Do_W > Set_W)
    {
        startW = (Do_W - Set_W) / 2;
        Do_W = Set_W;
    }
    int y_size = Do_W * Do_H;
    int yuv_size = y_size * 3 / 2;

    unsigned char *yuvbuf;
    if ((yuvbuf = (unsigned char *)malloc(yuv_size + 1)) != NULL)
    {
        memset(yuvbuf, 0, yuv_size + 1);
    }
    else
    {
        return HI_FAILURE; 
        printf("run here>>>>>>>>> malloc Fail>>>>>>>>>>>>>>>>>>>.\n");
    }
    int idy = 0;
    int idu = 0;
    int idv = 0;

    unsigned char* y_tmp = yuvbuf;
    unsigned char* u_tmp = yuvbuf + y_size;
    unsigned char* v_tmp = yuvbuf + y_size * 5 / 4;

    char *pMemContent_Y;
    char *pMemContent_U;
    char *pMemContent_V;

    for (h = startH; h <( Do_H +startH); h++) 
    {
      pMemContent_Y = pVBufVirt_Y + h * pVBuf->u32Stride[0];
        for (w = startW; w < (Do_W+startW); w++) {
        y_tmp[idy++] = *(pMemContent_Y+w);
            //pMemContent_Y++;
        }
    }

    for (h =  (startH/2); h <  ( (Do_H+startH)/2); h++) {

        pMemContent_U = pVBufVirt_C + h * pVBuf->u32Stride[1];
        pMemContent_V = pVBufVirt_C + h * pVBuf->u32Stride[1];
        pMemContent_U += 1;            
        for (w = (startW/2); w < ((Do_W+startW)/2) ; w++){
            u_tmp[idu++] = *(pMemContent_U+(2*w));
            v_tmp[idv++] = *(pMemContent_V+(2*w));
        }
    }

    unsigned char *jpeg_buf = NULL;
    unsigned long jpeg_size = 0;
    //unsigned char *picture = (unsigned char *)malloc(yuv_size + 1);

    int ret = tyuv2jpeg(yuvbuf, yuv_size, Do_W, Do_H, TJSAMP_420,
              &jpeg_buf, &jpeg_size, 96); //50:quality
    if(ret <0){
        return HI_FAILURE;
    }

    // FILE * fp_jpg = fopen("1_yuv_to_jpeg_test.jpg", "wb");
    // fwrite(jpeg_buf, jpeg_size, 1, fp_jpg);
    // fflush(fp_jpg);
    // fclose(fp_jpg);
    printf("!!!!!!!!!!!yuv to jpeg success!!!!!!!!!\n");
    int needSend = jpeg_size;
    int pos = 0;
    int len = 0;
    while (pos < needSend)
    {
        len = send(sfd, jpeg_buf + pos, 1024, 0);
        if (len <= 0)
        {
            perror("ERRPR");
            return HI_FAILURE;;
            break;
        }
        pos += len;
    }
    
    HI_MPI_SYS_Munmap(pUserPageAddr[0], size);
    if (yuvbuf != NULL)
    {
        free(yuvbuf);
    }
    if (jpeg_buf != NULL)
    {
        free(jpeg_buf);
    }
    return HI_SUCCESS;
}
*/
HI_S32 DHL_SAMPLE_MISC_ViDump(DHS_Photo_ATTR_S*  stDHS_Photo)
{
    
   VI_PIPE Pipe=stDHS_Photo->ViChn;
    MLOGI("DHL_SAMPLE_MISC_ViDump \n");
 //   HI_CHAR szYuvName[128];
    HI_CHAR szPixFrm[10];
    HI_CHAR szVideoFrm[10];    
    
 //   VIDEO_FRAME_INFO_S stFrame;
 //   VIDEO_FRAME_INFO_S* pstOutFrame;
 //   VIDEO_FRAME_INFO_S *astFrame = HI_NULL;
    VGS_HANDLE hHandle= -1;
    HI_U32 u32Cnt = 1;
    HI_U32 u32Depth = 2;
    HI_S32 s32MilliSec = -1;
    HI_S32 s32Times = 10;
    HI_BOOL bSendToVgs = HI_FALSE;
    VIDEO_FRAME_INFO_S stFrmInfo;
    VGS_TASK_ATTR_S stTask;
    HI_U32 u32LumaSize = 0;
    HI_U32 u32PicLStride = 0;
    HI_U32 u32PicCStride = 0;
    HI_U32 u32Width = 0;
    HI_U32 u32Height = 0;
    HI_S32 i = 0;
    HI_S32 s32Ret;
    HI_U32 u32BitWidth;
    VI_DUMP_ATTR_S    stDumpAttr;
    VB_POOL_CONFIG_S stVbPoolCfg;
    
    //VB_BLK VbBlk = VB_INVALID_HANDLE;
    VB_POOL hPool  = VB_INVALID_POOLID;

        printf("Do HI_MPI_VI_GetPipeDumpAttr\n");
    s32Ret = HI_MPI_VI_GetPipeDumpAttr(stDHS_Photo->ViChn, &astBackUpDumpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Get Pipe %d dump attr failed!\n", Pipe);
        return HI_PDT_SCENE_EINVAL;
    } 
    

    printf("astBackUpDumpAttr.bEnable %d\n",astBackUpDumpAttr.bEnable);       
    printf("astBackUpDumpAttr.u32Depth %d\n",astBackUpDumpAttr.u32Depth);   
    printf("astBackUpDumpAttr.enDumpType %d\n",astBackUpDumpAttr.enDumpType);  
    memcpy(&stDumpAttr, &astBackUpDumpAttr, sizeof(VI_DUMP_ATTR_S));
    stDumpAttr.bEnable  = HI_TRUE;
    stDumpAttr.u32Depth = u32Depth;
   // stDumpAttr.enDumpType=VI_DUMP_TYPE_YUV;
        printf("Do HI_MPI_VI_SetPipeDumpAttr\n");
    s32Ret = HI_MPI_VI_SetPipeDumpAttr(Pipe, &stDumpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Set Pipe %d dump attr failed!\n",Pipe);
        return HI_PDT_SCENE_EINVAL;
    }

    memset(&stFrame, 0, sizeof(stFrame));
    stFrame.u32PoolId = VB_INVALID_POOLID;
    
        printf("Do HI_MPI_VI_GetPipeFrame\n");
    while (HI_MPI_VI_GetPipeFrame(Pipe, &stFrame, s32MilliSec) != HI_SUCCESS)
    {
        s32Times--;
        if(0 >= s32Times)
        {
            printf("get frame error for 10 times,now exit !!!\n");
            VI_Restore(Pipe);
            return HI_PDT_SCENE_EINVAL;
        }
        
        printf("wait HI_MPI_VI_GetPipeFrame = %d\n",s32Times);
        HI_usleep(40000);
    }

    printf("stFrame.stVFrame.enPixelFormat %d\n",stFrame.stVFrame.enPixelFormat);
    switch (stFrame.stVFrame.enPixelFormat)
    {
        case PIXEL_FORMAT_YVU_SEMIPLANAR_420:
            snprintf(szPixFrm, 10, "P420");
            break;
        case PIXEL_FORMAT_YVU_SEMIPLANAR_422:
            snprintf(szPixFrm, 10, "P422");
            break;
        case PIXEL_FORMAT_YUV_400:
            snprintf(szPixFrm, 10, "P400");
            break;
        default:
            printf("pixel format err, pipe_yuvdump is dump YUV, not RAW.\n");
            snprintf(szPixFrm, 10, "--");
            break;
    }


    printf("stFrame.stVFrame.enVideoFormat %d\n",stFrame.stVFrame.enVideoFormat);
    switch (stFrame.stVFrame.enVideoFormat)
    {
        case VIDEO_FORMAT_LINEAR:
            snprintf(szVideoFrm, 10, "linear");
            break;
        case VIDEO_FORMAT_TILE_64x16:
            snprintf(szVideoFrm, 10, "tile_64X16");
            break;
        case VIDEO_FORMAT_TILE_16x8:
            snprintf(szVideoFrm, 10, "tile_16X8");
            break;
        default:
            snprintf(szVideoFrm, 10, "--");
            break;
    }


    /* make file name */
  //  snprintf(szYuvName, 128, "./vi_pipe%d_w%d_h%d_%s_%s_%d.yuv", Pipe,stFrame.stVFrame.u32Width, stFrame.stVFrame.u32Height, szPixFrm, szVideoFrm, u32Cnt);
  //  printf("Dump YUV frame of vi pipe %d  to file: \"%s\"\n", Pipe, szYuvName);
    fflush(stdout);

        printf("Do HI_MPI_VI_ReleasePipeFrame\n");
    s32Ret = HI_MPI_VI_ReleasePipeFrame(Pipe, &stFrame);
    if(HI_SUCCESS != s32Ret)
    {
        printf("Release frame error ,now exit !!!\n");
        VI_Restore(Pipe);
        return HI_PDT_SCENE_EINVAL;
    }
    stFrame.u32PoolId = VB_INVALID_POOLID;
     /* open file */
 //   pfd = fopen(szYuvName, "wb");

 /*   if (HI_NULL == pfd)
    {
        printf("open file failed:%s!\n", strerror(errno));
        VI_Restore(Pipe);
        return HI_PDT_SCENE_EINVAL;
    }
*/
    /* get frame  */
    while (u32Cnt--)
    {
        
        printf("Do HI_MPI_VI_GetPipeFrame\n");
        if (HI_MPI_VI_GetPipeFrame(Pipe, &stFrame, s32MilliSec) != HI_SUCCESS)
        {
            printf("Get frame fail \n");
            HI_usleep(1000);
            continue;
        }

        bSendToVgs = ((COMPRESS_MODE_NONE != stFrame.stVFrame.enCompressMode)
                                    || (VIDEO_FORMAT_LINEAR != stFrame.stVFrame.enVideoFormat));

        if (bSendToVgs)
        {

            u32Width    = stFrame.stVFrame.u32Width;
            u32Height   = stFrame.stVFrame.u32Height;

            u32BitWidth = (DYNAMIC_RANGE_SDR8 == stFrame.stVFrame.enDynamicRange) ? 8 : 10;

            u32PicLStride = ALIGN_UP((u32Width * u32BitWidth + 7) >> 3, 16);
            u32PicCStride = u32PicLStride;
            u32LumaSize = u32PicLStride * u32Height;

            if(PIXEL_FORMAT_YVU_SEMIPLANAR_420 == stFrame.stVFrame.enPixelFormat)
            {
                u32BlkSize = u32PicLStride * u32Height * 3 >> 1;
            }
            else if(PIXEL_FORMAT_YVU_SEMIPLANAR_422 == stFrame.stVFrame.enPixelFormat)
            {
                u32BlkSize = u32PicLStride * u32Height * 2;
            }
            else if(PIXEL_FORMAT_YUV_400 == stFrame.stVFrame.enPixelFormat)
            {
                u32BlkSize = u32PicLStride * u32Height;
            }
            else
            {
                printf("Unsupported pixelformat %d\n",stFrame.stVFrame.enPixelFormat);
               // VI_Restore(Pipe);
               // return HI_PDT_SCENE_EINVAL;
            }

            memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
            stVbPoolCfg.u64BlkSize  = u32BlkSize;
            stVbPoolCfg.u32BlkCnt   = 1;
            stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;


            printf("Do HI_MPI_VB_CreatePool\n");
            hPool   = HI_MPI_VB_CreatePool(&stVbPoolCfg);
            if (hPool == VB_INVALID_POOLID)
            {
                printf("HI_MPI_VB_CreatePool failed! \n");
                VI_Restore(Pipe);
                return HI_PDT_SCENE_EINVAL;
            }



            stMem.hPool = hPool;

            while ((stMem.hBlock = HI_MPI_VB_GetBlock(stMem.hPool, u32BlkSize, HI_NULL)) == VB_INVALID_HANDLE)
            {
                ;
            }
            stMem.u64PhyAddr = HI_MPI_VB_Handle2PhysAddr(stMem.hBlock);

            stMem.pVirAddr = (HI_U8*) HI_MPI_SYS_Mmap(stMem.u64PhyAddr, u32BlkSize );
            if (stMem.pVirAddr == HI_NULL)
            {
                printf("Mem dev may not open\n");
                VI_Restore(Pipe);
                return HI_PDT_SCENE_EINVAL;
            }

            memset(&stFrmInfo.stVFrame, 0, sizeof(VIDEO_FRAME_S));
            stFrmInfo.stVFrame.u64PhyAddr[0] = stMem.u64PhyAddr;
            stFrmInfo.stVFrame.u64PhyAddr[1] = stFrmInfo.stVFrame.u64PhyAddr[0] + u32LumaSize;

            stFrmInfo.stVFrame.u64VirAddr[0] = (HI_U64)(HI_UL)stMem.pVirAddr;
            stFrmInfo.stVFrame.u64VirAddr[1] = stFrmInfo.stVFrame.u64VirAddr[0] + u32LumaSize;

            stFrmInfo.stVFrame.u32Width     = u32Width;
            stFrmInfo.stVFrame.u32Height    = u32Height;
            stFrmInfo.stVFrame.u32Stride[0] = u32PicLStride;
            stFrmInfo.stVFrame.u32Stride[1] = u32PicCStride;

            stFrmInfo.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
            stFrmInfo.stVFrame.enPixelFormat  = stFrame.stVFrame.enPixelFormat;
            stFrmInfo.stVFrame.enVideoFormat  = VIDEO_FORMAT_LINEAR;
            stFrmInfo.stVFrame.enDynamicRange =  stFrame.stVFrame.enDynamicRange;

            stFrmInfo.stVFrame.u64PTS     = (i * 40);
            stFrmInfo.stVFrame.u32TimeRef = (i * 2);

            stFrmInfo.u32PoolId = hPool;
            stFrmInfo.enModId = HI_ID_VGS;

        printf("Do HI_MPI_VGS_BeginJob\n");
            s32Ret = HI_MPI_VGS_BeginJob(&hHandle);
            if (s32Ret != HI_SUCCESS)
            {
                printf("HI_MPI_VGS_BeginJob failed\n");
                hHandle = -1;
                VI_Restore(Pipe);
                return HI_PDT_SCENE_EINVAL;
            }
            memcpy(&stTask.stImgIn, &stFrame, sizeof(VIDEO_FRAME_INFO_S));
            memcpy(&stTask.stImgOut , &stFrmInfo, sizeof(VIDEO_FRAME_INFO_S));
            
        printf("Do HI_MPI_VGS_AddScaleTask\n");
            s32Ret = HI_MPI_VGS_AddScaleTask(hHandle, &stTask, VGS_SCLCOEF_NORMAL);
            if (s32Ret != HI_SUCCESS)
            {
                printf("HI_MPI_VGS_AddScaleTask failed\n");
                VI_Restore(Pipe);
                return HI_PDT_SCENE_EINVAL;
            }

        printf("Do HI_MPI_VGS_EndJob\n");
            s32Ret = HI_MPI_VGS_EndJob(hHandle);
            if (s32Ret != HI_SUCCESS)
            {
                printf("HI_MPI_VGS_EndJob failed\n");
                VI_Restore(Pipe);
                return HI_PDT_SCENE_EINVAL;
            }
            hHandle = -1;
            /* save VO frame to file */
        /*
            if(DYNAMIC_RANGE_SDR8 == stFrame.stVFrame.enDynamicRange)
            {
                sample_yuv_8bit_dump(&stFrmInfo.stVFrame, pfd);
            }
            else
            {
                sample_yuv_10bit_dump(&stFrmInfo.stVFrame, pfd, u32ByteAlign);
            }
            */
           
            // save VI frame to file 
            
             memcpy(&stDHS_Photo->stFrmInfo, &stFrmInfo.stVFrame, sizeof(VIDEO_FRAME_INFO_S));
            //s32Ret = vi_dump_save_one_frame(&stFrmInfo.stVFrame, stDHS_Photo->sfd);
            
        printf("Do HI_MPI_VB_ReleaseBlock\n");
            HI_MPI_VB_ReleaseBlock(stMem.hBlock);

            stMem.hPool =  VB_INVALID_POOLID;
            hHandle = -1;
            if(HI_NULL != stMem.pVirAddr)
            {
                
        printf("Do HI_MPI_SYS_Munmap\n");
                HI_MPI_SYS_Munmap((HI_VOID*)stMem.pVirAddr, u32BlkSize );
                stMem.u64PhyAddr = HI_NULL;
            }
            if (hPool != VB_INVALID_POOLID)
            {
                
        printf("Do HI_MPI_VB_DestroyPool\n");
                HI_MPI_VB_DestroyPool( hPool );
                hPool = VB_INVALID_POOLID;
            }

        }
        else
        {
            
            // save VI frame to file 
            memcpy(&stDHS_Photo->stFrmInfo, &stFrmInfo.stVFrame, sizeof(VIDEO_FRAME_INFO_S));
           // s32Ret = vi_dump_save_one_frame(&stFrmInfo.stVFrame, stDHS_Photo->sfd);
            /*
            if(DYNAMIC_RANGE_SDR8 == stFrame.stVFrame.enDynamicRange)
            {
                sample_yuv_8bit_dump(&stFrame.stVFrame, pfd);
            }
            else
            {
                sample_yuv_10bit_dump(&stFrame.stVFrame, pfd, u32ByteAlign);
            }*/
        }

        printf("Get ViPipe %d frame %d!!\n", Pipe, u32Cnt);
        /* release frame after using */
        s32Ret = HI_MPI_VI_ReleasePipeFrame(Pipe, &stFrame);
        if(HI_SUCCESS != s32Ret)
        {
            printf("Release frame error ,now exit !!!\n");
            VI_Restore(Pipe);
            return HI_PDT_SCENE_EINVAL;
        }

        stFrame.u32PoolId = VB_INVALID_POOLID;
    }
    VI_Restore(Pipe);
    return HI_SUCCESS;
}




HI_S32 HI_PDT_SCENE_Deinit(HI_VOID)
{
    PDT_SCENE_CHECK_INIT();

#ifdef CONFIG_SCENEAUTO_SUPPORT
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = PDT_SCENE_StopAutoThread();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_PDT_SCENE_EINTER);
#endif

#if (defined(CONFIG_DEBUG) && defined(AMP_LINUX_HUAWEILITE))
    HI_APPCOMM_SAFE_FREE(s_stSceneCTX.pstSceneParam);
#endif

    s_stSceneCTX.stSceneState.bSceneInit = HI_FALSE;

    MLOGI("SCENE Module has been deinited successfully!\n");

    return HI_SUCCESS;
}

#if (defined(AMP_LINUX_HUAWEILITE) && !defined(__HuaweiLite__)) || defined(AMP_HUAWEILITE) || defined(AMP_LINUX)
HI_S32 HI_PDT_SCENE_RegisterEvent(HI_VOID)
{
    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_EVTHUB_Register(HI_EVENT_PDT_SCENE_FRAMERATE_CHANGE);

    if (s32Ret != HI_SUCCESS)
    {
        MLOGE("HI_EVENT_PDT_SCENE_FRAMERATE_CHANGE register fail \n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#endif

/** @}*/  /** <!-- ==== SCENE End ====*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
