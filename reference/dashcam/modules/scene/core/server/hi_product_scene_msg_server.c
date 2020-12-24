/**
* @file    hi_hal_screen.c
* @brief   hal screen implemention
*
* Copyright (c) 2017 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date      2017/12/12
* @version   1.0

*/
#include <string.h>

#include "hi_product_scene.h"
#include "hi_product_scene_msg_server.h"
#include "hi_product_scene_msg_define.h"
#include "hi_appcomm_msg_server.h"

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
#define HI_MODULE "SCENE_SERVER"
VI_PIPE ViPipe=0;
static HI_S32 MSG_PDT_SCENE_InitCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_PDT_SCENE_Init();
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_PDT_SCENE_SetSceneModeCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_MODE_S* pstSceneMode = (HI_PDT_SCENE_MODE_S*)pvRequest;
MLOGD("HI_PDT_SCENE_SetSceneMode  1\n");
    s32Ret = HI_PDT_SCENE_SetSceneMode(pstSceneMode);
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_PDT_SCENE_PauseCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    HI_BOOL* pbEnable = (HI_BOOL*) pvRequest;
MLOGD("HI_PDT_SCENE_Pause  2.\n");
    s32Ret =  HI_PDT_SCENE_Pause(*pbEnable);
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_PDT_SCENE_DeinitCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret =  HI_PDT_SCENE_Deinit();
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_PDT_SCENE_GetSceneModeCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_APPCOMM_CHECK_POINTER(pu32RespLen, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_MODE_S* pstSceneMode = (HI_PDT_SCENE_MODE_S*)malloc(sizeof(HI_PDT_SCENE_MODE_S));
    if(pstSceneMode == NULL)
    {
        MLOGE("malloc memory failed\n");
        return HI_FAILURE;
    }

    s32Ret = HI_PDT_SCENE_GetSceneMode(pstSceneMode);
    if (s32Ret != HI_SUCCESS)
    {
        HI_APPCOMM_SAFE_FREE(pstSceneMode);
        MLOGE("HI_PDT_SCENE_GetSceneMode failed\n");
        return HI_FAILURE;
    }

    *pu32RespLen = sizeof(HI_PDT_SCENE_MODE_S);
    *ppvResponse = pstSceneMode;

    return HI_SUCCESS;
}

static HI_S32 MSG_PDT_SCENE_SetISOCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_PIPE_NUMBER_S* pstISO = (HI_PDT_SCENE_PIPE_NUMBER_S*) pvRequest;

    s32Ret = HI_PDT_SCENE_SetISO(pstISO->VcapPipeHdl, pstISO->u32Number);
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_PDT_SCENE_SetExpTimeCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_PIPE_NUMBER_S* pstExpTime = (HI_PDT_SCENE_PIPE_NUMBER_S*) pvRequest;

    s32Ret = HI_PDT_SCENE_SetExpTime(pstExpTime->VcapPipeHdl, pstExpTime->u32Number);
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_PDT_SCENE_SetEVCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_PIPE_EV_S* pstEV = (HI_PDT_SCENE_PIPE_EV_S*) pvRequest;

    s32Ret = HI_PDT_SCENE_SetEV(pstEV->VcapPipeHdl, pstEV->enEV);
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_PDT_SCENE_SetAntiflikerCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_PIPE_ANTIFLICKER_S * pstAntifliker = (HI_PDT_SCENE_PIPE_ANTIFLICKER_S*) pvRequest;

    s32Ret = HI_PDT_SCENE_SetAntiflicker(pstAntifliker->VcapPipeHdl, pstAntifliker->enAntiflcker);
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}


static HI_S32 MSG_PDT_SCENE_SetWBCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_PIPE_NUMBER_S* pstWB = (HI_PDT_SCENE_PIPE_NUMBER_S*) pvRequest;

    s32Ret = HI_PDT_SCENE_SetWB(pstWB->VcapPipeHdl, pstWB->u32Number);
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_PDT_SCENE_SetMETRYCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_PIPE_METRY_S* pstMetryParam = (HI_PDT_SCENE_PIPE_METRY_S*) pvRequest;

    s32Ret = HI_PDT_SCENE_SetMetry(pstMetryParam->VcapPipeHdl, &pstMetryParam->stMetry);
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}
static HI_S32 MSG_Set_config_for_ISPCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    DHS_ISP_SET_S* pstSceneMode = (DHS_ISP_SET_S*)pvRequest;
        MLOGI("DHS_Set_config_for_ISP server\n");
    s32Ret = DHS_Set_config_for_ISP(pstSceneMode);
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_Set_config_for_AWBCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    DHS_AWB_SET_S* pstSceneMode = (DHS_AWB_SET_S*)pvRequest;
    MLOGI("DHS_Set_config_for_AWB server\n");
    s32Ret = DHS_Set_config_for_AWB(pstSceneMode);
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}
static HI_S32 MSG_GET_DHS_SCENE_SET_EXPCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    DHS_ISP_EXPOSURE_ATTR_S* pstSceneMode = (DHS_ISP_EXPOSURE_ATTR_S*)pvRequest;
        MLOGI("DHS_PDT_SCENE_Set_Exp server\n");
    s32Ret = DHS_PDT_SCENE_Set_Exp(pstSceneMode->VcapPipeHdl,&pstSceneMode->ISP_EXPOSUR);
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_Get_config_for_ISPCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
    HI_APPCOMM_CHECK_POINTER(pu32RespLen, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;
    DHS_ISP_SET_S* pstSceneMode = (DHS_ISP_SET_S*)malloc(sizeof(DHS_ISP_SET_S));
    if(pstSceneMode == NULL)
    {
        MLOGE("malloc memory failed\n");
        return HI_FAILURE;
    }

    MLOGI("DHS_Get_config_for_ISP server\n");
    s32Ret = DHS_Get_config_for_ISP(pstSceneMode);
    if (s32Ret != HI_SUCCESS)
    {
        HI_APPCOMM_SAFE_FREE(pstSceneMode);
        MLOGE("DHS_Get_config_for_ISP failed\n");
        return HI_FAILURE;
    }

    *pu32RespLen = sizeof(DHS_ISP_SET_S);
    *ppvResponse = pstSceneMode;
    return HI_SUCCESS;
}

static HI_S32 MSG_Get_config_for_AWBCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
    HI_APPCOMM_CHECK_POINTER(pu32RespLen, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;
    DHS_AWB_SET_S* pstSceneMode = (DHS_AWB_SET_S*)malloc(sizeof(DHS_AWB_SET_S));
    if(pstSceneMode == NULL)
    {
        MLOGE("malloc memory failed\n");
        return HI_FAILURE;
    }

    MLOGI("DHS_Get_config_for_AWB server\n");
    s32Ret = DHS_Get_config_for_AWB(pstSceneMode);
    if (s32Ret != HI_SUCCESS)
    {

        HI_APPCOMM_SAFE_FREE(pstSceneMode);
        MLOGE("DHS_Get_config_for_AWB failed\n");
        return HI_FAILURE;
    }

*pu32RespLen = sizeof(DHS_AWB_SET_S);
*ppvResponse = pstSceneMode;
return HI_SUCCESS;
    
}


static HI_S32 MSG_DHL_GET_PHOPOCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

  DHS_Photo_ATTR_S*  stDHS_Photo = (DHS_Photo_ATTR_S*)pvRequest;
        MLOGI("DHS_Set_config_for_AWB server\n");
    s32Ret = DHL_SAMPLE_MISC_ViDump(stDHS_Photo);
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}



static HI_S32 MSG_DHS_MPI_VI_SETPIPEDUMPATTRCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

     DHS_MPI_VI_PipeDumpAttr_S*  in_Data = (DHS_MPI_VI_PipeDumpAttr_S*)pvRequest;
        MLOGI("DHS_MPI_VI_ReleasePipeFrame server\n");
        s32Ret = HI_MPI_VI_SetPipeDumpAttr(in_Data->ViPipe, &in_Data->pstDumpAtt);
        if(0 != s32Ret)
        {
             printf("Release Chn Frame error!!! %d\n",s32Ret);
        }
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_DHS_MPI_VI_GETPIPEDUMPATTRCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
    HI_APPCOMM_CHECK_POINTER(pu32RespLen, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;
    
    VI_PIPE* in_Data = (VI_PIPE*)pvRequest;
    VI_DUMP_ATTR_S* out_Data = (VI_DUMP_ATTR_S*)malloc(sizeof(VI_DUMP_ATTR_S));
    if(out_Data == NULL)
    {
        MLOGE("malloc memory failed\n");
        return HI_FAILURE;
    }

    MLOGI("DHS_Get_config_for_ISP server\n");
    s32Ret = HI_MPI_VI_GetPipeDumpAttr(*in_Data,out_Data);
    if (s32Ret != HI_SUCCESS)
    {
        HI_APPCOMM_SAFE_FREE(out_Data);
        MLOGE("HI_MPI_VI_GetPipeDumpAttr failed\n");
        return HI_FAILURE;
    }

    *pu32RespLen = sizeof(VI_DUMP_ATTR_S);
    *ppvResponse = out_Data;
    return HI_SUCCESS;
}

static HI_S32 MSG_DHS_MPI_VI_RELEASEPIPEFRAMECallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

     VIDEO_FRAME_INFO_S*  in_Data = (VIDEO_FRAME_INFO_S*)pvRequest;
    // VIDEO_FRAME_INFO_S* out_Data =in_Data->pstVideoFrame;
        MLOGI("MPI_VI_ReleasePipeFrame server  ViPipe = %d \n",ViPipe);
    //    MLOGI("in_Data->pstVideoFrame  %d \n",&out_Data);
        s32Ret = HI_MPI_VI_ReleasePipeFrame(ViPipe, in_Data);
        if(0 != s32Ret)
        {
             printf("Release Chn Frame error!!! \n");
        }
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    *pu32RespLen = sizeof(VIDEO_FRAME_INFO_S);
    *ppvResponse = in_Data;
    return HI_SUCCESS;
}
static HI_S32 MSG_DHS_MPI_VI_GETPIPEFRAMECallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
    HI_APPCOMM_CHECK_POINTER(pu32RespLen, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;
    HI_S32 s32MilliSec = -1;
    VIDEO_FRAME_INFO_S* in_Data = (VIDEO_FRAME_INFO_S*)pvRequest;
    //VIDEO_FRAME_INFO_S* out_Data = (VIDEO_FRAME_INFO_S*)malloc(sizeof(VIDEO_FRAME_INFO_S));
    //out_Data=&in_Data->pstVideoFrame;
    ViPipe=0;
    //    MLOGI("out_Data  %d \n",out_Data);
    //
        MLOGI("&in_Data->pstVideoFrame  %d \n",in_Data->u32PoolId);
    // memset(out_Data, &in_Data->pstVideoFrame, sizeof(VIDEO_FRAME_INFO_S));
     
    if(in_Data == NULL)
    {
        MLOGE("malloc memory failed\n");
        return HI_FAILURE;
    }

    MLOGI("HI_MPI_VI_GetPipeFrame server\n");
    s32Ret = HI_MPI_VI_GetPipeFrame(ViPipe, in_Data, s32MilliSec);

   
        MLOGI("&in_Data->pstVideoFrame  %d \n",in_Data->u32PoolId);
    if (s32Ret != HI_SUCCESS)
    {
        HI_APPCOMM_SAFE_FREE(in_Data);
        MLOGE("HI_MPI_VI_GetPipeDumpAttr failed\n");
        return HI_FAILURE;
    }

    *pu32RespLen = sizeof(VIDEO_FRAME_INFO_S);
    *ppvResponse = in_Data;
    return HI_SUCCESS;
}

static HI_S32 MSG_DHS_MPI_VGS_BEGINJOBCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    VGS_HANDLE* in_Data = (VGS_HANDLE*) pvRequest;
        MLOGI("DHS_MPI_VGS_BeginJob server\n");
        s32Ret = HI_MPI_VGS_BeginJob(in_Data);
        if(0 != s32Ret)
        {
             printf("Release Chn Frame error!!! %d\n",s32Ret);
        }
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_DHS_MPI_VGS_ENDJOBCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    VGS_HANDLE* in_Data = (VGS_HANDLE*) pvRequest;
        MLOGI("HI_MPI_VGS_EndJob server\n");
        s32Ret = HI_MPI_VGS_EndJob(*in_Data);
        if(0 != s32Ret)
        {
             printf("Release Chn Frame error!!! %d\n",s32Ret);
        }
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_DHS_MPI_VGS_CANCEJOBCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
    HI_APPCOMM_CHECK_POINTER(pvRequest, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;

    VGS_HANDLE* in_Data = (VGS_HANDLE*) pvRequest;
        MLOGI("HI_MPI_VGS_CancelJob server\n");
        s32Ret = HI_MPI_VGS_CancelJob(*in_Data);
        if(0 != s32Ret)
        {
             printf("Release Chn Frame error!!! %d\n",s32Ret);
        }
    HI_APPCOMM_CHECK_RETURN(s32Ret, s32Ret);

    return HI_SUCCESS;
}

static HI_S32 MSG_DHS_MPI_VGS_ADDSCALETASKCallback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData, HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{ 
  
    HI_APPCOMM_CHECK_POINTER(pu32RespLen, HI_PDT_SCENE_EINTER);
    HI_S32 s32Ret = HI_SUCCESS;
    
    DHS_MPI_VGS_Add_S* in_Data = (DHS_MPI_VGS_Add_S*)pvRequest;
    VGS_TASK_ATTR_S* out_Data = (VGS_TASK_ATTR_S*)malloc(sizeof(VGS_TASK_ATTR_S));
    if(in_Data == NULL)
    {
        MLOGE("malloc memory failed\n");
        return HI_FAILURE;
    }
    memcpy(out_Data, &in_Data->pstTask, sizeof(VGS_TASK_ATTR_S));
    MLOGI("DHS_Get_config_for_ISP server\n");
        s32Ret = HI_MPI_VGS_AddScaleTask(in_Data->hHandle,out_Data, in_Data->enScaleCoefMode);
    if (s32Ret != HI_SUCCESS)
    {
      //  HI_APPCOMM_SAFE_FREE(*in_Data->pstTask);
        MLOGE("HI_MPI_VI_GetPipeDumpAttr failed\n");
        return HI_FAILURE;
    }

    *pu32RespLen = sizeof(VGS_TASK_ATTR_S);
    *ppvResponse = out_Data;
    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_MSG_SERVER_Init(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;

    MLOGD("Param input is OK!\n");

    s32Ret = HI_MSG_RegisterMsgProc(MSG_PDT_SCENE_INIT, MSG_PDT_SCENE_InitCallback, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_PDT_SCENE_SETSCENEMODE, MSG_PDT_SCENE_SetSceneModeCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_PDT_SCENE_PAUSE, MSG_PDT_SCENE_PauseCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_PDT_SCENE_DEINIT, MSG_PDT_SCENE_DeinitCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_PDT_SCENE_GETSCENEMODE, MSG_PDT_SCENE_GetSceneModeCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_PDT_SCENE_SETISO, MSG_PDT_SCENE_SetISOCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_PDT_SCENE_SETEXPTIME, MSG_PDT_SCENE_SetExpTimeCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_PDT_SCENE_SETEV, MSG_PDT_SCENE_SetEVCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_PDT_SCENE_SETANTIFLICKER, MSG_PDT_SCENE_SetAntiflikerCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_PDT_SCENE_SETWB, MSG_PDT_SCENE_SetWBCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_PDT_SCENE_SETMETRY, MSG_PDT_SCENE_SetMETRYCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_SET_CONFIG_FOR_ISP, MSG_Set_config_for_ISPCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_SET_CONFIG_FOR_AWB, MSG_Set_config_for_AWBCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_GET_CONFIG_FOR_ISP, MSG_Get_config_for_ISPCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }
    s32Ret = HI_MSG_RegisterMsgProc(MSG_GET_CONFIG_FOR_AWB, MSG_Get_config_for_AWBCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }
       s32Ret = HI_MSG_RegisterMsgProc(MSG_GET_DHS_SCENE_SET_EXP, MSG_GET_DHS_SCENE_SET_EXPCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_DHL_GET_PHOPO, MSG_DHL_GET_PHOPOCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

      s32Ret = HI_MSG_RegisterMsgProc(MSG_DHS_MPI_VI_RELEASEPIPEFRAME, MSG_DHS_MPI_VI_RELEASEPIPEFRAMECallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }
    s32Ret = HI_MSG_RegisterMsgProc(MSG_DHS_MPI_VI_SETPIPEDUMPATTR, MSG_DHS_MPI_VI_SETPIPEDUMPATTRCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }
    s32Ret = HI_MSG_RegisterMsgProc(MSG_DHS_MPI_VI_GETPIPEDUMPATTR, MSG_DHS_MPI_VI_GETPIPEDUMPATTRCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_DHS_MPI_VI_GETPIPEFRAME,MSG_DHS_MPI_VI_GETPIPEFRAMECallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

     s32Ret = HI_MSG_RegisterMsgProc(MSG_DHS_MPI_VGS_BEGINJOB,MSG_DHS_MPI_VGS_BEGINJOBCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }
    
     s32Ret = HI_MSG_RegisterMsgProc(MSG_DHS_MPI_VGS_CANCEJOB,MSG_DHS_MPI_VGS_CANCEJOBCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_DHS_MPI_VGS_ADDSCALETASK,MSG_DHS_MPI_VGS_ADDSCALETASKCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }

    s32Ret = HI_MSG_RegisterMsgProc(MSG_DHS_MPI_VGS_ENDJOB,MSG_DHS_MPI_VGS_ENDJOBCallback,NULL);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE(RED"msg RegReq failed\n\n"NONE);
    }
    return s32Ret;
}
