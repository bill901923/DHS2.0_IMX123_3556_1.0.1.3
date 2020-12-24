/**

* @brief   scene module struct and interface declaration
*
* Copyright (c) 2012 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date       2017/12/13
* @version   1.0

*/

#ifndef _MSG_SCENE_DEFINE_H_
#define _MSG_SCENE_DEFINE_H_

#include "hi_product_scene.h"
#include "hi_appcomm_msg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** \addtogroup     SCENE */
/** @{ */  /** <!-- [SCENE] */

#define PDT_SCENE_MSG_SEND_SYNC_TIMEOUT (10000)

typedef enum tagMSG_PDT_SCENE_CMD_E
{
    MSG_PDT_SCENE_INIT = HI_APPCOMM_MSG_ID(HI_APP_MOD_SCENE, 0),
    MSG_PDT_SCENE_SETSCENEMODE,
    MSG_PDT_SCENE_PAUSE,
    MSG_PDT_SCENE_DEINIT,
    MSG_PDT_SCENE_GETSCENEMODE,
    MSG_PDT_SCENE_SETISO,
    MSG_PDT_SCENE_SETEXPTIME,
    MSG_PDT_SCENE_SETEV,
    MSG_PDT_SCENE_SETANTIFLICKER,
    MSG_PDT_SCENE_SETWB,
    MSG_PDT_SCENE_SETMETRY,
    MSG_PDT_SCENE_FRAMERATE_FROM120TO60,
    MSG_PDT_SCENE_FRAMERATE_FROM60TO120,
    MSG_PDT_SCENE_BUTT,
    MSG_SET_CONFIG_FOR_ISP,
    MSG_SET_CONFIG_FOR_AWB,
    MSG_GET_CONFIG_FOR_ISP,
    MSG_GET_CONFIG_FOR_AWB,
    MSG_GET_DHS_SCENE_SET_EXP,
    MSG_DHL_GET_PHOPO,
    MSG_DHS_MPI_VI_GETPIPEDUMPATTR,
    MSG_DHS_MPI_VI_SETPIPEDUMPATTR,
    MSG_DHS_MPI_VI_GETPIPEFRAME,		
    MSG_DHS_RESTORE,					
    MSG_DHS_MPI_VI_RELEASEPIPEFRAME,	
    MSG_DHS_MPI_VB_CREATEPOOL,		
    MSG_DHS_MPI_VB_GETBLOCK,			
    MSG_DHS_MPI_VB_HANDLE2PHYSADDR,	
    MSG_DHS_MPI_SYS_MMAP,				
    MSG_DHS_MPI_VGS_BEGINJOB,
    MSG_DHS_MPI_VGS_CANCEJOB,			
    MSG_DHS_MPI_VGS_ENDJOB,			
    MSG_DHS_MPI_VB_RELEASEBLOCK,		
    MSG_DHS_MPI_SYS_MUNMAP,			
    MSG_DHS_MPI_VB_DESTROYPOOL,
    MSG_DHS_MPI_VGS_ADDSCALETASK
} MSG_PDT_SCENE_CMD_E;

/**an encapsulation of ptoto pipe index and pipe param Index */
typedef struct hiPDT_SCENE_PHOTOPIPE_INFO_S
{
    HI_HANDLE VcapPipeHdl;
    HI_U8 u8PipeParamIndex;
}HI_PDT_SCENE_PHOTOPIPE_INFO_S;

/**an encapsulation of pipe index and ev param */
typedef struct hiPDT_SCENE_PIPE_EV_S
{
    HI_HANDLE VcapPipeHdl;
    HI_PDT_SCENE_EV_E enEV;
}HI_PDT_SCENE_PIPE_EV_S;

/**an encapsulation of pipe index and ANTIFLICKER param */
typedef struct hiPDT_SCENE_PIPE_ANTIFLICKER_S
{
    HI_HANDLE VcapPipeHdl;
    HI_PDT_SCENE_ANTIFLICKER_E enAntiflcker;
}HI_PDT_SCENE_PIPE_ANTIFLICKER_S;


/**an encapsulation of pipe index and metry param */
typedef struct hiPDT_SCENE_PIPE_METRY_S
{
    HI_HANDLE VcapPipeHdl;
    HI_PDT_SCENE_METRY_S stMetry;
}HI_PDT_SCENE_PIPE_METRY_S;

/**an encapsulation of pipe index and enable status */
typedef struct hiPDT_SCENE_PIPE_ENABLE_S
{
    HI_HANDLE VcapPipeHdl;
    HI_BOOL bEnable;
}HI_PDT_SCENE_PIPE_ENABLE_S;

/**an encapsulation of pipe index and input number */
typedef struct hiPDT_SCENE_PIPE_NUMBER_S
{
    HI_HANDLE VcapPipeHdl;
    HI_U32 u32Number;
}HI_PDT_SCENE_PIPE_NUMBER_S;

typedef struct _DHS_ISP_EXPOSURE_ATTR_S
{
    HI_HANDLE VcapPipeHdl;
    ISP_EXPOSURE_ATTR_S ISP_EXPOSUR;
}DHS_ISP_EXPOSURE_ATTR_S;

typedef struct _DHS_VI_DUMP_ATTR_S
{
    VI_PIPE ViPipe;
    VI_DUMP_ATTR_S pstDumpAttr;
}DHS_VI_DUMP_ATTR_S;



/** @}*/  /** <!-- ==== SCENE End ====*/
/*typedef struct _DHS_Photo_ATTR_S
{
    VI_CHN ViChn;
    HI_U32 u32Cnt;
    int sfd;
    VIDEO_FRAME_S* pVBuf;
}DHS_Photo_ATTR_S;*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* End of #ifndef  _MSG_SCENE_DEFINE_ */
