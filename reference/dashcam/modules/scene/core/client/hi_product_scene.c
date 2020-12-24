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
#include <string.h>
#include "hi_product_scene.h"
#include "hi_appcomm_msg_client.h"
#include "hi_product_scene_msg_client.h"
#include "hi_ipcmsg.h"
#include <pthread.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <errno.h>




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
#define HI_MODULE "SCENE_CLIENT"


HI_S32 HI_PDT_SCENE_Init()
{
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_INIT, HI_NULL, 0, HI_NULL,HI_NULL, 0);

    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_INIT Failed, %x\n\n, "NONE, s32Ret);
        return s32Ret;
    }
    else
    {
        MLOGI(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_INIT Pass\n\n, "NONE);
    }
    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_SetSceneMode(const HI_PDT_SCENE_MODE_S* pstSceneMode)
{
    HI_APPCOMM_CHECK_POINTER(pstSceneMode, HI_PDT_SCENE_EINVAL);

    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_SETSCENEMODE, pstSceneMode, sizeof(HI_PDT_SCENE_MODE_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_SETSCENEMODE Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_Pause(HI_BOOL bEnable)
{
    HI_S32 s32Ret = HI_SUCCESS;

    if(bEnable==HI_TRUE)
    {
            MLOGE(YELLOW"HI_PDT_SCENE_Pause HI_TRUE\n\n"NONE);}
    else
    {
            MLOGE(YELLOW"HI_PDT_SCENE_Pause HI_Fail\n\n"NONE);
    }
    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_PAUSE, &bEnable, sizeof(HI_BOOL), HI_NULL, HI_NULL, 0);

   

    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_PAUSE Failed\n\n"NONE);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_GetSceneMode(HI_PDT_SCENE_MODE_S* pstSceneMode)
{
    HI_APPCOMM_CHECK_POINTER(pstSceneMode, HI_PDT_SCENE_EINVAL);
    HI_S32 s32Ret = HI_SUCCESS;

    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_GETSCENEMODE, HI_NULL, 0, HI_NULL, pstSceneMode, sizeof(HI_PDT_SCENE_MODE_S));


    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_GETSCENEMODE Failed\n\n"NONE);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_SetISO(HI_HANDLE VcapPipeHdl, HI_U32 u32ISO)
{
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_PIPE_NUMBER_S stPipeNumber;
    stPipeNumber.VcapPipeHdl = VcapPipeHdl;
    stPipeNumber.u32Number = u32ISO;

    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_SETISO, &stPipeNumber, sizeof(HI_PDT_SCENE_PIPE_NUMBER_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_SETISO Failed\n\n"NONE);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_SetExpTime(HI_HANDLE VcapPipeHdl, HI_U32 u32ExpTime_us)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MLOGE("HI_PDT_SCENE_SetExpTime22 %d\n",u32ExpTime_us);
    HI_PDT_SCENE_PIPE_NUMBER_S stPipeNumber;
    stPipeNumber.VcapPipeHdl = VcapPipeHdl;
    stPipeNumber.u32Number = u32ExpTime_us;
    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_SETEXPTIME, &stPipeNumber, sizeof(HI_PDT_SCENE_PIPE_NUMBER_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_SETEXPTIME Failed\n\n"NONE);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_SetEV(HI_HANDLE VcapPipeHdl, HI_PDT_SCENE_EV_E enEV)
{
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_PIPE_EV_S stPipeEV;
    stPipeEV.VcapPipeHdl = VcapPipeHdl;
    stPipeEV.enEV = enEV;



    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_SETEV, &stPipeEV, sizeof(HI_PDT_SCENE_PIPE_EV_S), HI_NULL, HI_NULL, 0);

  

    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_SETEV Failed\n\n"NONE);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_SetAntiflicker(HI_HANDLE VcapPipeHdl, HI_PDT_SCENE_ANTIFLICKER_E enAntiflicker)
{
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_PIPE_ANTIFLICKER_S stPipeAntiflicker;
    stPipeAntiflicker.VcapPipeHdl = VcapPipeHdl;
    stPipeAntiflicker.enAntiflcker = enAntiflicker;



    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_SETANTIFLICKER, &stPipeAntiflicker, sizeof(HI_PDT_SCENE_PIPE_ANTIFLICKER_S), HI_NULL, HI_NULL, 0);

 

    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_SETANTIFLICKER Failed\n\n"NONE);
        return s32Ret;
    }

    return HI_SUCCESS;
}


HI_S32 HI_PDT_SCENE_SetWB(HI_HANDLE VcapPipeHdl, HI_U32 u32WB)
{
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_PIPE_NUMBER_S stPipeNumber;
    stPipeNumber.VcapPipeHdl = VcapPipeHdl;
    stPipeNumber.u32Number = u32WB;

    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_SETWB, &stPipeNumber, sizeof(HI_PDT_SCENE_PIPE_NUMBER_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_SETWB Failed\n\n"NONE);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_SetMetry(HI_HANDLE VcapPipeHdl, const HI_PDT_SCENE_METRY_S* pstMetry)
{
    HI_APPCOMM_CHECK_POINTER(pstMetry, HI_PDT_SCENE_EINVAL);

    HI_S32 s32Ret = HI_SUCCESS;
    HI_PDT_SCENE_PIPE_METRY_S stPipeMetry;
    stPipeMetry.VcapPipeHdl = VcapPipeHdl;
    memcpy(&stPipeMetry.stMetry, pstMetry, sizeof(HI_PDT_SCENE_METRY_S));
    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_SETMETRY, &stPipeMetry, sizeof(HI_PDT_SCENE_PIPE_METRY_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_SETMETRY Failed\n\n"NONE);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_PDT_SCENE_Deinit(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;



    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_DEINIT, HI_NULL, 0, HI_NULL, HI_NULL, 0);

    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_DEINIT Failed\n\n"NONE);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 DHS_Set_config_for_ISP(const DHS_ISP_SET_S* Set_SceneMode)
{       

    HI_APPCOMM_CHECK_POINTER(Set_SceneMode, HI_PDT_SCENE_EINVAL);

    HI_S32 s32Ret = HI_SUCCESS;



    printf("DHS_Set_config_for_ISP\n");

    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());



    s32Ret = HI_MSG_SendSync(Set_SceneMode->item,MSG_SET_CONFIG_FOR_ISP, Set_SceneMode, sizeof(DHS_ISP_SET_S), HI_NULL, HI_NULL, 0);

    if(HI_SUCCESS != s32Ret)

    {

        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_SET_CONFIG_FOR_ISP Failed\n\n"NONE);

        return s32Ret;

    }



    return HI_SUCCESS;

}

HI_S32 DHS_Set_config_for_AWB(const DHS_AWB_SET_S* Set_SceneMode)
{
    HI_APPCOMM_CHECK_POINTER(Set_SceneMode, HI_PDT_SCENE_EINVAL);
    HI_S32 s32Ret = HI_SUCCESS;
    printf("DHS_Set_config_for_AWB\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());


    s32Ret = HI_MSG_SendSync(Set_SceneMode->item,MSG_SET_CONFIG_FOR_AWB, Set_SceneMode, sizeof(DHS_AWB_SET_S), HI_NULL, HI_NULL, 0);

    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_SET_CONFIG_FOR_AWB Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;
       
}

HI_S32 DHS_Get_config_for_ISP(DHS_ISP_SET_S* Set_SceneMode)
{
    HI_APPCOMM_CHECK_POINTER(Set_SceneMode, HI_PDT_SCENE_EINVAL);
    HI_S32 s32Ret = HI_SUCCESS;
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());
    s32Ret = HI_MSG_SendSync(Set_SceneMode->item,MSG_GET_CONFIG_FOR_ISP, HI_NULL, 0, HI_NULL, Set_SceneMode, sizeof(DHS_ISP_SET_S));
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_GET_CONFIG_FOR_ISP Failed\n\n"NONE);
        return s32Ret;
    }

    return HI_SUCCESS;

}

HI_S32 DHS_Get_config_for_AWB(DHS_AWB_SET_S* Set_SceneMode)
{

    HI_APPCOMM_CHECK_POINTER(Set_SceneMode, HI_PDT_SCENE_EINVAL);
    HI_S32 s32Ret = HI_SUCCESS;
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());

    s32Ret = HI_MSG_SendSync(Set_SceneMode->item,MSG_GET_CONFIG_FOR_AWB, HI_NULL, 0, HI_NULL, Set_SceneMode, sizeof(DHS_AWB_SET_S));
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_GET_CONFIG_FOR_AWB Failed\n\n"NONE);
        return s32Ret;

    }



    return HI_SUCCESS;

}

HI_S32 DHS_PDT_SCENE_Set_Exp(HI_HANDLE VcapPipeHdl, const ISP_EXPOSURE_ATTR_S*  stExposure)
{

    HI_APPCOMM_CHECK_POINTER(stExposure, HI_PDT_SCENE_EINVAL);
    HI_S32 s32Ret = HI_SUCCESS;
    printf("DHS_PDT_SCENE_Set_Exp\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());


    DHS_ISP_EXPOSURE_ATTR_S stDHS_ISP_EXPOSURE;
    stDHS_ISP_EXPOSURE.VcapPipeHdl=VcapPipeHdl;
    memcpy(&stDHS_ISP_EXPOSURE.ISP_EXPOSUR, stExposure, sizeof(ISP_EXPOSURE_ATTR_S));
    

        MLOGI(" set .u32AGain =%d u32DGain =%d u32ISPDGain =%d u32ExpTime =%d \n", stDHS_ISP_EXPOSURE.ISP_EXPOSUR.stManual.u32AGain   
        ,stDHS_ISP_EXPOSURE.ISP_EXPOSUR.stManual.u32DGain 
        , stDHS_ISP_EXPOSURE.ISP_EXPOSUR.stManual.u32ISPDGain
        , stDHS_ISP_EXPOSURE.ISP_EXPOSUR.stManual.u32ExpTime);    
    s32Ret = HI_MSG_SendSync(1,MSG_GET_DHS_SCENE_SET_EXP,&stDHS_ISP_EXPOSURE, sizeof(DHS_ISP_EXPOSURE_ATTR_S), HI_NULL, HI_NULL, 0);

    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_GET_DHS_SCENE_SET_EXP Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;
}

HI_S32 DHL_SAMPLE_MISC_ViDump(DHS_Photo_ATTR_S*  stDHS_Photo)
{
    
     HI_APPCOMM_CHECK_POINTER(stDHS_Photo, HI_PDT_SCENE_EINVAL);
    HI_S32 s32Ret = HI_SUCCESS;
    printf("DHL_SAMPLE_MISC_ViDump\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());


    s32Ret = HI_MSG_SendSync(1,MSG_DHL_GET_PHOPO, stDHS_Photo, sizeof(DHS_Photo_ATTR_S), HI_NULL, stDHS_Photo, sizeof(DHS_Photo_ATTR_S));

    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_SET_CONFIG_FOR_AWB Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;
}


HI_S32 DHS_MPI_VI_GetPipeDumpAttr(VI_PIPE ViPipe, VI_DUMP_ATTR_S *pstDumpAttr)                              
{
    HI_APPCOMM_CHECK_POINTER(pstDumpAttr, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VI_GetPipeDumpAttr	\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());

	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VI_GETPIPEDUMPATTR,&ViPipe, sizeof(VI_PIPE), HI_NULL, pstDumpAttr, sizeof(VI_DUMP_ATTR_S));
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_VI_GETPIPEDUMPATTR	 Failed\n\n"NONE);
        return s32Ret;

    }
    return HI_SUCCESS;
}

HI_S32 DHS_MPI_VI_SetPipeDumpAttr(VI_PIPE ViPipe, const VI_DUMP_ATTR_S *pstDumpAttr)                        
{
    HI_APPCOMM_CHECK_POINTER(pstDumpAttr, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VI_SetPipeDumpAttr	\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());
    DHS_MPI_VI_PipeDumpAttr_S in_Data;
    in_Data.ViPipe=ViPipe;
    memcpy(&in_Data.pstDumpAtt, pstDumpAttr, sizeof(VI_DUMP_ATTR_S));
    
	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VI_SETPIPEDUMPATTR, &in_Data, sizeof(DHS_MPI_VI_PipeDumpAttr_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_VI_SETPIPEDUMPATTR	 Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;
}


HI_S32 DHS_VI_Restore(VI_PIPE ViPipe)                                                                       
{
    //HI_APPCOMM_CHECK_POINTER(Set_SceneMode, HI_PDT_SCENE_EINVAL);
    HI_S32 s32Ret = HI_SUCCESS;
    printf("DHS_VI_Restore				\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());
    DHS_MPI_VI_ViPipe_S in_Data;
    in_Data.ViPipe=ViPipe; 
    s32Ret = HI_MSG_SendSync(1, MSG_DHS_RESTORE, &in_Data, sizeof(DHS_MPI_VI_ViPipe_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_RESTORE Failed\n\n"NONE);
        return s32Ret;

    }
    return HI_SUCCESS;
}
HI_S32 DHS_MPI_VI_GetPipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec)       
{
    HI_APPCOMM_CHECK_POINTER(pstVideoFrame, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VI_GetPipeFrame		\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());

   // DHS_MPI_VI_PipeFrame_S in_Data;
   // in_Data.ViPipe=ViPipe;     
  //  memcpy(&in_Data.pstVideoFrame, pstVideoFrame, sizeof(VIDEO_FRAME_INFO_S));
    //in_Data.s32MilliSec=s32MilliSec;

    s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VI_GETPIPEFRAME, pstVideoFrame,sizeof(VIDEO_FRAME_INFO_S), HI_NULL,pstVideoFrame, sizeof(VIDEO_FRAME_INFO_S));
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_VI_GETPIPEFRAME Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;
}

HI_S32 DHS_MPI_VI_ReleasePipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame)                 
{
    HI_APPCOMM_CHECK_POINTER(pstVideoFrame, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VI_ReleasePipeFrame	\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());
    
  // DHS_MPI_VI_PipeFrame_S in_Data;
   // in_Data.ViPipe=ViPipe; 
// memcpy(&in_Data.pstVideoFrame, pstVideoFrame, sizeof(VIDEO_FRAME_INFO_S));
   s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VI_RELEASEPIPEFRAME	,pstVideoFrame, sizeof(VIDEO_FRAME_INFO_S), HI_NULL,pstVideoFrame, sizeof(VIDEO_FRAME_INFO_S));
    
    //s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VI_RELEASEPIPEFRAME	,&in_Data, sizeof(DHS_MPI_VI_PipeFrame_S), HI_NULL,pstVideoFrame, sizeof(VIDEO_FRAME_INFO_S));
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_VI_RELEASEPIPEFRAME	 Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;

}

HI_S32 DHS_MPI_VB_CreatePool(VB_POOL_CONFIG_S *pstVbPoolCfg)                                                
{
    HI_APPCOMM_CHECK_POINTER(pstVbPoolCfg, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VB_CreatePool		\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());

	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VB_CREATEPOOL		, pstVbPoolCfg, sizeof(VB_POOL_CONFIG_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_VB_CREATEPOOL		 Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;
}

HI_S32 DHS_MPI_VB_GetBlock(VB_POOL Pool, HI_U64 u64BlkSize, const HI_CHAR *pcMmzName)                       
{
    HI_APPCOMM_CHECK_POINTER(pcMmzName, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VB_GetBlock			\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());
    
    DHS_MPI_VB_GetBlock_S in_Data;
    in_Data.Pool=Pool; 
    in_Data.u64BlkSize=u64BlkSize; 
    memcpy(&in_Data.pcMmzName, pcMmzName, sizeof(HI_CHAR));
	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VB_GETBLOCK			, &in_Data, sizeof(DHS_MPI_VB_GetBlock_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_VB_GETBLOCK			 Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;
}

HI_S32 DHS_MPI_VB_Handle2PhysAddr(VB_BLK Block)                                                             
{
    //HI_APPCOMM_CHECK_POINTER(Set_SceneMode, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VB_Handle2PhysAddr	\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());
    
    DHS_MPI_VB_BLKB_S in_Data;
    in_Data.Block=Block;
	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VB_HANDLE2PHYSADDR	, &in_Data, sizeof(DHS_MPI_VB_BLKB_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_VB_HANDLE2PHYSADDR	 Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;
}

HI_S32 DHS_MPI_SYS_Mmap(HI_U64 u64PhyAddr, HI_U32 u32Size)                                                  
{
    //HI_APPCOMM_CHECK_POINTER(Set_SceneMode, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_SYS_Mmap			\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());
    
    DHS_MPI_SYS_Mmap_S in_Data;
    in_Data.u64PhyAddr=u64PhyAddr; 
    in_Data.u32Size=u32Size;
	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_SYS_MMAP			, &in_Data, sizeof(DHS_MPI_SYS_Mmap_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
    MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_SYS_MMAP			 Failed\n\n"NONE);
    return s32Ret;
    }
    return HI_SUCCESS;
}

HI_S32 DHS_MPI_VGS_BeginJob(VGS_HANDLE *phHandle)                                                           
{
    HI_APPCOMM_CHECK_POINTER(phHandle, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VGS_BeginJob		\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());
	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VGS_BEGINJOB		, phHandle, sizeof(VGS_HANDLE), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_VGS_BEGINJOB		 Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;

}

HI_S32 DHS_MPI_VGS_EndJob(VGS_HANDLE hHandle)                                                               
{
   // HI_APPCOMM_CHECK_POINTER(Set_SceneMode, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VGS_EndJob			\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());
    
	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VGS_ENDJOB, &hHandle, sizeof(VGS_HANDLE), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_VGS_ENDJOB			 Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;

}

HI_S32 DHS_MPI_VGS_CancelJob(VGS_HANDLE hHandle)                                                               
{
   // HI_APPCOMM_CHECK_POINTER(Set_SceneMode, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VGS_CancelJob			\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());
    
	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VGS_CANCEJOB, &hHandle, sizeof(VGS_HANDLE), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync DHS_MPI_VGS_CancelJob			 Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;

}
HI_S32 DHS_MPI_VB_ReleaseBlock(VB_BLK Block)                                                                
{
   // HI_APPCOMM_CHECK_POINTER(Set_SceneMode, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VB_ReleaseBlock		\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());
    
    DHS_MPI_VB_BLKB_S in_Data;
    in_Data.Block=Block;
	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VB_RELEASEBLOCK		, &in_Data, sizeof(DHS_MPI_VB_BLKB_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_VB_RELEASEBLOCK		 Failed\n\n"NONE);
        return s32Ret;
    }
    return HI_SUCCESS;

}

HI_S32 DHS_MPI_SYS_Munmap(HI_VOID *pVirAddr, HI_U32 u32Size)                                                
{
    HI_APPCOMM_CHECK_POINTER(pVirAddr, HI_PDT_SCENE_EINVAL);
		HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_SYS_Munmap			\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());  
    
    DHS_MPI_SYS_Munmap_S in_Data;  
    memcpy(&in_Data.pVirAddr, pVirAddr, sizeof(HI_VOID));
    in_Data.u32Size=u32Size;
	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_SYS_MUNMAP			, &in_Data, sizeof(DHS_MPI_SYS_Munmap_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_SYS_MUNMAP			 Failed\n\n"NONE);
        return s32Ret;

    }
    return HI_SUCCESS;

}

HI_S32 DHS_MPI_VB_DestroyPool(VB_POOL Pool )																
{
   // HI_APPCOMM_CHECK_POINTER(Set_SceneMode, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VB_DestroyPool		\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());

    DHS_MPI_VB_POOL_S in_Data;
    in_Data.Pool=Pool; 
	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VB_DESTROYPOOL		, &in_Data, sizeof(DHS_MPI_VB_POOL_S), HI_NULL, HI_NULL, 0);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_VB_DESTROYPOOL		 Failed\n\n"NONE);
        return s32Ret;

    }
    return HI_SUCCESS;

}


HI_S32 DHS_MPI_VGS_AddScaleTask(VGS_HANDLE hHandle, VGS_TASK_ATTR_S *pstTask, VGS_SCLCOEF_MODE_E enScaleCoefMode)
{

    // HI_APPCOMM_CHECK_POINTER(Set_SceneMode, HI_PDT_SCENE_EINVAL);
	HI_S32 s32Ret = HI_SUCCESS;
	printf("DHS_MPI_VB_DestroyPool		\n");
    printf("DHS_MSG_CLIENT_Init  %x\n",DHS_MSG_CLIENT_Init());

    
    DHS_MPI_VGS_Add_S in_Data;
    in_Data.hHandle=hHandle; 
    in_Data.enScaleCoefMode=enScaleCoefMode; 
    memcpy(&in_Data.pstTask, pstTask, sizeof(VGS_TASK_ATTR_S));

	s32Ret = HI_MSG_SendSync(1, MSG_DHS_MPI_VGS_ADDSCALETASK		, &in_Data, sizeof(DHS_MPI_VGS_Add_S), HI_NULL,  pstTask, sizeof(VGS_TASK_ATTR_S));
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_DHS_MPI_VB_DESTROYPOOL		 Failed\n\n"NONE);
        return s32Ret;

    }
    return HI_SUCCESS;
}










#ifdef CFG_POST_PROCESS
HI_S32 HI_PDT_SCENE_LockAE(HI_HANDLE VcapPipeHdl, const HI_BOOL bLock)
{
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_PIPE_ENABLE_S stPipeEnable;
    stPipeEnable.VcapPipeHdl = VcapPipeHdl;
    stPipeEnable.bEnable = bLock;

    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_LOCKAE, &stPipeEnable, sizeof(HI_PDT_SCENE_PIPE_ENABLE_S), HI_NULL, HI_NULL, 0);

    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_LOCKAE Failed\n\n"NONE);
        return s32Ret;
    }

   return HI_SUCCESS;

}

HI_S32 HI_PDT_SCENE_LockAWB(HI_HANDLE VcapPipeHdl, const HI_BOOL bLock)
{
    HI_S32 s32Ret = HI_SUCCESS;

    HI_PDT_SCENE_PIPE_ENABLE_S stPipeEnable;
    stPipeEnable.VcapPipeHdl = VcapPipeHdl;
    stPipeEnable.bEnable = bLock;



    s32Ret = HI_MSG_SendSync(0,MSG_PDT_SCENE_LOCKAWB, &stPipeEnable, sizeof(HI_PDT_SCENE_PIPE_ENABLE_S), HI_NULL, HI_NULL, 0);

    if(HI_SUCCESS != s32Ret)
    {
        MLOGE(YELLOW"HI_MSG_CLIENT_SendSync MSG_PDT_SCENE_LOCKAWB Failed\n\n"NONE);
        return s32Ret;
    }

    return HI_SUCCESS;
}

#endif

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

/** @}*/  /** <!-- ==== SCENE End ====*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
