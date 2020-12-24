/**
* @file    hi_product_scene.h
* @brief   scene module struct and interface declaration
*
* Copyright (c) 2012 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date       2017/12/13
* @version   1.0

*/

#ifndef _HI_PRODUCT_SCENE_H_
#define _HI_PRODUCT_SCENE_H_

#include "hi_product_scene_define.h"
#if (defined(AMP_LINUX_HUAWEILITE) && !defined(__HuaweiLite__)) || defined(AMP_HUAWEILITE) || defined(AMP_LINUX)
#include "hi_eventhub.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** \addtogroup     SCENE */
/** @{ */  /** <!-- [SCENE] */

/** error code define */
#define HI_PDT_SCENE_EINVAL         HI_APPCOMM_ERR_ID(HI_APP_MOD_SCENE, HI_EINVAL)      /**<Invalid argument */
#define HI_PDT_SCENE_EINITIALIZED   HI_APPCOMM_ERR_ID(HI_APP_MOD_SCENE, HI_EINITIALIZED)/**<Already initialized */
#define HI_PDT_SCENE_ENOTINIT       HI_APPCOMM_ERR_ID(HI_APP_MOD_SCENE, HI_ENOINIT)     /**<Not inited */
#define HI_PDT_SCENE_EINTER         HI_APPCOMM_ERR_ID(HI_APP_MOD_SCENE, HI_EINTER)      /**<Internal error */

#define HI_EVENT_PDT_SCENE_FRAMERATE_CHANGE  HI_APPCOMM_EVENT_ID(HI_APP_MOD_SCENE, 1)

/**
 * @brief          Init SCENE module
 * @return        0 success, non-zero error code.
 * @exception   None
 * @author       HiMobileCam Reference Develop Team
 * @date          2017/12/11
 */
HI_S32 HI_PDT_SCENE_Init(HI_VOID);

/**
 * @brief          SetScenemode, start sceneauto thread.
 * @param[in]   PstSceneMode : Media config(each pipe state), and decide Sceneauto param in which pipe
 * @return        0 success, non-zero error code.
 * @exception   None
 * @author       HiMobileCam Reference Develop Team
 * @date          2017/12/11
 */
HI_S32 HI_PDT_SCENE_SetSceneMode(const HI_PDT_SCENE_MODE_S* pstSceneMode);

/**
 * @brief          GetSceneMode.
 * @param[out] pstSceneMode : Media config
 * @return        0 success, non-zero error code.
 * @exception   None
 * @author       HiMobileCam Reference Develop Team
 * @date          2017/12/11
 */
HI_S32 HI_PDT_SCENE_GetSceneMode(HI_PDT_SCENE_MODE_S* pstSceneMode);

/**
 * @brief          Pause sceneauto when isp stoped(in playback state or when reset mediaRoute)
 *                   Note: when Pause is true, the sceneauto thread can's set param executed
 * @param[in]   bEnable: True is Pause, False is Resume
 * @return        0 success, non-zero error code.
 * @exception   None
 * @author       HiMobileCam Reference Develop Team
 * @date          2017/12/11
 */
HI_S32 HI_PDT_SCENE_Pause(HI_BOOL bEnable);

/**
 * @brief          Set Metry
 * @param[in]   VcapPipeHdl : MainIsp Pipe (Sensor Control Pipe)
 * @param[in]   pstMetry : Metry type and param.
 * @return        0 success, non-zero error code.
 * @exception   None
 * @author       HiMobileCam Reference Develop Team
 * @date          2017/12/11
 */
HI_S32 HI_PDT_SCENE_SetMetry(HI_HANDLE VcapPipeHdl, const HI_PDT_SCENE_METRY_S* pstMetry);

/**
 * @brief          Set ISO
 * @param[in]   VcapPipeHdl : MainIsp Pipe (Sensor Control Pipe)
 * @param[in]   u32ISO : ISO value, range [100,6400], 0 is auto.
 * @return        0 success, non-zero error code.
 * @exception   None
 * @author       HiMobileCam Reference Develop Team
 * @date          2017/12/11
 */
HI_S32 HI_PDT_SCENE_SetISO(HI_HANDLE VcapPipeHdl, HI_U32 u32ISO);

/**
 * @brief          Set ExpTime, must smaller than 1 / FrameRate
 * @param[in]   VcapPipeHdl : MainIsp Pipe (Sensor Control Pipe)
 * @param[in]   u32ExpTime_us : 0 is auto, must smaller than 1 / FrameRate
 * @return        0 success, non-zero error code.
 * @exception   None
 * @author       HiMobileCam Reference Develop Team
 * @date          2017/12/11
 */
HI_S32 HI_PDT_SCENE_SetExpTime(HI_HANDLE VcapPipeHdl, HI_U32 u32ExpTime_us);

/**
 * @brief          Set EV
 * @param[in]   VcapPipeHdl : MainIsp Pipe (Sensor Control Pipe)
 * @param[in]   enEV : EV value
 * @return        0 success, non-zero error code.
 * @exception   None
 * @author       HiMobileCam Reference Develop Team
 * @date          2017/12/11
 */
HI_S32 HI_PDT_SCENE_SetEV(HI_HANDLE VcapPipeHdl, HI_PDT_SCENE_EV_E enEV);

/**
 * @brief          Set ANTIFLICKER
 * @param[in]   VcapPipeHdl : MainIsp Pipe (Sensor Control Pipe)
 * @param[in]   enAntiflicker : Antiflicker value
 * @return        0 success, non-zero error code.
 * @exception   None
 * @author       HiMobileCam Reference Develop Team
 * @date          2017/12/11
 */
HI_S32 HI_PDT_SCENE_SetAntiflicker(HI_HANDLE VcapPipeHdl, HI_PDT_SCENE_ANTIFLICKER_E enAntiflicker);

/**
 * @brief          Set WB
 * @param[in]   VcapPipeHdl : MainIsp Pipe (Sensor Control Pipe)
 * @param[in]   u32WB : WB Value,[1600,16000] 0 is auto
 * @return        0 success, non-zero error code.
 * @exception   None
 * @author       HiMobileCam Reference Develop Team
 * @date          2017/12/11
 */

HI_S32 HI_PDT_SCENE_SetWB(HI_HANDLE VcapPipeHdl, HI_U32 u32WB);

/**
 * @brief          Deinit SCENE module.
 * @return        0 success, non-zero error code.
 * @exception   None
 * @author       HiMobileCam Reference Develop Team
 * @date          2017/12/11
 */
HI_S32 HI_PDT_SCENE_Deinit(HI_VOID);

/**
 * @brief          Register SCENE Event.
 * @return        0 success, non-zero error code.
 * @exception   None
 * @author       HiMobileCam Reference Develop Team
 * @date          2017/12/11
 */
HI_S32 HI_PDT_SCENE_RegisterEvent(HI_VOID);
/** @}*/  /** <!-- ==== SCENE End ====*/


HI_S32 DHS_Set_config_for_ISP(const DHS_ISP_SET_S* Set_SceneMode);
HI_S32 DHS_Set_config_for_AWB(const DHS_AWB_SET_S* Set_SceneMode);
HI_S32 DHS_Get_config_for_ISP( DHS_ISP_SET_S* Set_SceneMode);
HI_S32 DHS_Get_config_for_AWB( DHS_AWB_SET_S* Set_SceneMode);
HI_S32 DHS_PDT_SCENE_Set_Exp(HI_HANDLE VcapPipeHdl, const ISP_EXPOSURE_ATTR_S*  stExposure);
HI_S32 DHL_SAMPLE_MISC_ViDump(DHS_Photo_ATTR_S*  stDHS_Photo);

HI_S32 DHS_MPI_VI_GetPipeDumpAttr(VI_PIPE ViPipe, VI_DUMP_ATTR_S *pstDumpAttr);
HI_S32 DHS_MPI_VI_SetPipeDumpAttr(VI_PIPE ViPipe, const VI_DUMP_ATTR_S *pstDumpAttr);
HI_S32 DHS_MPI_VI_GetPipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, HI_S32 s32MilliSec);
HI_S32 DHS_VI_Restore(VI_PIPE ViPipe);
HI_S32 DHS_MPI_VI_ReleasePipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame);
HI_S32 DHS_MPI_VB_CreatePool(VB_POOL_CONFIG_S *pstVbPoolCfg);
HI_S32 DHS_MPI_VB_GetBlock(VB_POOL Pool, HI_U64 u64BlkSize, const HI_CHAR *pcMmzName);
HI_S32 DHS_MPI_VB_Handle2PhysAddr(VB_BLK Block);
HI_S32 DHS_MPI_SYS_Mmap(HI_U64 u64PhyAddr, HI_U32 u32Size);
HI_S32 DHS_MPI_VGS_BeginJob(VGS_HANDLE *phHandle);
HI_S32 DHS_MPI_VGS_EndJob(VGS_HANDLE hHandle);
HI_S32 DHS_MPI_VB_ReleaseBlock(VB_BLK Block);
HI_S32 DHS_MPI_SYS_Munmap(HI_VOID *pVirAddr, HI_U32 u32Size);
HI_S32 DHS_MPI_VB_DestroyPool(VB_POOL Pool );
HI_S32 DHS_MPI_VGS_AddScaleTask(VGS_HANDLE hHandle,  VGS_TASK_ATTR_S *pstTask, VGS_SCLCOEF_MODE_E enScaleCoefMode);
HI_S32 DHS_MPI_VGS_CancelJob(VGS_HANDLE hHandle);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* End of #ifndef __HI_SCENE_H__ */
