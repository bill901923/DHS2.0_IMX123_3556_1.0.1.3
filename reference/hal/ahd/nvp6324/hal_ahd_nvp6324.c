/**
* @file    hal_ahd_nvp6324.c
* @brief   hal ahd nvp6324 implemention
*
* Copyright (c) 2017 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date      2017/12/12
* @version   1.0

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>

#include "jaguar1_common.h"

#include "hi_appcomm.h"
#include "hi_type.h"
#include "hi_appcomm_util.h"
#include "hi_hal_common.h"
#include "hi_hal_common_inner.h"
#include "hi_mapi_hal_ahd_define.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** \addtogroup     AHD_NVP6324 */
/** @{ */  /** <!-- [AHD_NVP6324] */
extern HI_HAL_AHD_DEV_S ahd_nvp6324_Obj;
#define HAL_AHD_DEV "/dev/jaguar1"
typedef struct
{
    HI_S32 ahd_fd;
    HI_HAL_AHD_CFG_S ahd_cfg;
} ahd_nvp6324_context;

static ahd_nvp6324_context ahd_nvp6324_ctx={HAL_FD_INITIALIZATION_VAL,{HI_HAL_AHD_RES_BUIT,HI_HAL_AHD_FPS_BUIT}};


HI_S32 hal_check_param_valid(HI_HAL_AHD_DEV_S* dev)
{
    HI_APPCOMM_CHECK_POINTER(dev,HI_FAILURE);
    HI_APPCOMM_CHECK_EXPR((dev->id == ahd_nvp6324_Obj.id),HI_FAILURE);
    HI_APPCOMM_CHECK_EXPR(memcmp(dev->name,ahd_nvp6324_Obj.name,sizeof(dev->name)) == 0,HI_FAILURE);
    return HI_SUCCESS;
}


HI_S32 hal_ahd_nvp6324_init(HI_HAL_AHD_DEV_S* dev)
{
    HI_S32 ret_value = HI_SUCCESS;
    HI_U32 i2c_num = 0;
    HI_APPCOMM_CHECK_RETURN(hal_check_param_valid(dev),HI_FAILURE);
    if (ahd_nvp6324_ctx.ahd_fd != HAL_FD_INITIALIZATION_VAL)
    {
        MLOGE("already init");
        return HI_HAL_EINITIALIZED;
    }
#ifdef HI3519AV100
#ifdef BOARD_DEMB
        i2c_num = 1;
#else
        MLOGE("please point board type,fail\n\n");
        return HI_HAL_EINTER;
#endif
#elif defined(HI3559V200)
        i2c_num = 0;
        ahd_nvp6324_Obj.bUsedChn[2] = HI_FALSE;
        ahd_nvp6324_Obj.bUsedChn[3] = HI_FALSE;
#else
        MLOGE("please point chip type,fail\n\n");
        return HI_HAL_EINTER;
#endif

    ret_value = jaguar1_module_init(i2c_num);
    if(HI_SUCCESS != ret_value)
    {
        MLOGE("insmod nvp6324: failed\n");
        return HI_HAL_EINTER;
    }
    ahd_nvp6324_ctx.ahd_fd = open(HAL_AHD_DEV, O_RDWR);
    if (ahd_nvp6324_ctx.ahd_fd < 0)
    {
        MLOGE("open [%s] failed\n",HAL_AHD_DEV);
        return HI_HAL_EINVOKESYS;
    }

    return HI_SUCCESS;
}

HI_S32 hal_ahd_nvp6324_start(HI_HAL_AHD_DEV_S* dev)
{
    HI_APPCOMM_CHECK_RETURN(hal_check_param_valid(dev),HI_FAILURE);
    if (ahd_nvp6324_ctx.ahd_fd < 0)
    {
        MLOGE("not init");
        return HI_HAL_ENOINIT;
    }
    return HI_SUCCESS;
}

HI_S32 hal_ahd_nvp6324_start_chn(HI_HAL_AHD_DEV_S* dev, HI_U32 chn)
{
    HI_APPCOMM_CHECK_RETURN(hal_check_param_valid(dev),HI_FAILURE);
    if (ahd_nvp6324_ctx.ahd_fd < 0)
    {
        MLOGE("not init");
        return HI_HAL_ENOINIT;
    }
    video_chnl_control chn_info;
    chn_info.devnum = 0;
    chn_info.chn = chn;
    HI_S32 ret_value = ioctl(ahd_nvp6324_ctx.ahd_fd, IOC_VDEC_START_CHNL, &chn_info);/**chip detect check*/
    if(HI_SUCCESS != ret_value)
    {
        MLOGE("nvp6324 ioctl IOC_VDEC_INPUT_INIT: failed\n");
        return HI_HAL_EINTER;
    }
    return HI_SUCCESS;
}


HI_S32 hal_ahd_nvp6324_stop_chn(HI_HAL_AHD_DEV_S* dev, HI_U32 chn)
{
    HI_APPCOMM_CHECK_RETURN(hal_check_param_valid(dev),HI_FAILURE);
    if (ahd_nvp6324_ctx.ahd_fd < 0)
    {
        MLOGE("not init");
        return HI_HAL_ENOINIT;
    }
    video_chnl_control chn_info;
    chn_info.devnum = 0;
    chn_info.chn = chn;
    HI_S32 ret_value = ioctl(ahd_nvp6324_ctx.ahd_fd, IOC_VDEC_STOP_CHNL, &chn_info);/**chip detect check*/
    if(HI_SUCCESS != ret_value)
    {
        MLOGE("nvp6324 ioctl IOC_VDEC_INPUT_INIT: failed\n");
        return HI_HAL_EINTER;
    }
    return HI_SUCCESS;
}

HI_S32 hal_ahd_nvp6324_get_status(HI_HAL_AHD_DEV_S* dev, HI_HAL_AHD_STATUS_E ahd_status[], HI_U32 cnt)
{
    HI_APPCOMM_CHECK_RETURN(hal_check_param_valid(dev),HI_FAILURE);
    if (ahd_nvp6324_ctx.ahd_fd < 0)
    {
        MLOGE("not init");
        return HI_HAL_ENOINIT;
    }
    ;//todo
    return HI_SUCCESS;
}


HI_S32 hal_ahd_nvp6324_get_attr( HI_HAL_AHD_DEV_S* dev, HI_HAL_AHD_CFG_S* ahd_cfg)
{
    HI_APPCOMM_CHECK_RETURN(hal_check_param_valid(dev),HI_FAILURE);
    HI_APPCOMM_CHECK_POINTER(ahd_cfg,HI_FAILURE);
    if(ahd_nvp6324_ctx.ahd_cfg.enRes == HI_HAL_AHD_RES_BUIT)
    {
        MLOGE("not init");
        return HI_FAILURE;
    }
    memcpy(ahd_cfg,&ahd_nvp6324_ctx.ahd_cfg,sizeof(HI_HAL_AHD_CFG_S));
    return HI_SUCCESS;
}

HI_S32 hal_ahd_nvp6324_set_attr(HI_HAL_AHD_DEV_S* dev, const HI_HAL_AHD_CFG_S* ahd_cfg)
{
    HI_APPCOMM_CHECK_RETURN(hal_check_param_valid(dev),HI_FAILURE);
    HI_APPCOMM_CHECK_POINTER(ahd_cfg,HI_FAILURE);
    HI_APPCOMM_CHECK_EXPR((ahd_cfg->enRes >=HI_HAL_AHD_RES_720P)&&(ahd_cfg->enRes < HI_HAL_AHD_RES_BUIT),HI_FAILURE);
    HI_APPCOMM_CHECK_EXPR((ahd_cfg->enFps >=HI_HAL_AHD_FPS_25)&&(ahd_cfg->enFps <=HI_HAL_AHD_FPS_BUIT),HI_FAILURE);

    HI_S32 ret_value = HI_SUCCESS;
    if (ahd_nvp6324_ctx.ahd_fd < 0)
    {
        MLOGE("not init");
        return HI_HAL_ENOINIT;
    }
    MLOGI("AHD config enRes[%d],enFps[%d]\n",ahd_cfg->enRes,ahd_cfg->enFps);
    if(memcmp(&ahd_nvp6324_ctx.ahd_cfg,ahd_cfg,sizeof(HI_HAL_AHD_CFG_S)) == 0)
    {
        MLOGW("the same config enRes[%d],enFps[%d]\n",ahd_cfg->enRes,ahd_cfg->enFps);
        return HI_SUCCESS;
    }
    video_init_all video_config={0};
    NC_VIVO_CH_FORMATDEF format;
    if(HI_HAL_AHD_RES_720P == ahd_cfg->enRes){
        video_config.mipi_clk = 1;
        if(HI_HAL_AHD_FPS_25 == ahd_cfg->enFps){
            format = AHD20_720P_25P_EX_Btype;
        }
        else{
            format = AHD20_720P_30P_EX_Btype;
        }
    }
    else{
        video_config.mipi_clk = 3;
        if(HI_HAL_AHD_FPS_25 == ahd_cfg->enFps){
            format = AHD20_1080P_25P;
        }
        else{
            format = AHD20_1080P_30P;
        }
    }

#if defined(HI3559V200)&&defined(LANE_MODE_0)
    video_config.mipi_lane_num = 4;
#elif defined(HI3559V200)&&defined(LANE_MODE_1)
    video_config.mipi_lane_num = 2;
#else
    video_config.mipi_lane_num = 4;
#endif
    video_config.dev_num = 0;
    video_config.mipi_yuv_type = 1;
    for (HI_S32 i=0; i<MIN(HI_HAL_AHD_CHN_MAX,sizeof(video_config.ch_param)/sizeof(video_config.ch_param[0])); i++)
    {
        video_config.ch_param[i].ch=i;
        video_config.ch_param[i].input = SINGLE_ENDED;
        video_config.ch_param[i].format = format;
        if(HI_TRUE == ahd_nvp6324_Obj.bUsedChn[i]){
            video_config.ch_param[i].input = SINGLE_ENDED;
            video_config.ch_param[i].ch_mipiout_en = ENABLE;
        }else{
            video_config.ch_param[i].ch_mipiout_en = DISABLE;
       }
        video_config.ch_param[i].uv_seq = 4;
    }
    ret_value = ioctl(ahd_nvp6324_ctx.ahd_fd, IOC_VDEC_INIT_ALL, &video_config);/**chip detect check*/
    if(HI_SUCCESS != ret_value)
    {
        MLOGE("nvp6324 ioctl IOC_VDEC_INPUT_INIT: failed\n");
        return HI_HAL_EINTER;
    }

    memcpy(&ahd_nvp6324_ctx.ahd_cfg,ahd_cfg,sizeof(HI_HAL_AHD_CFG_S));

    return HI_SUCCESS;
}


HI_S32 hal_ahd_nvp6324_stop(HI_HAL_AHD_DEV_S* dev)
{
    HI_APPCOMM_CHECK_RETURN(hal_check_param_valid(dev),HI_FAILURE);
    if (ahd_nvp6324_ctx.ahd_fd < 0)
    {
        MLOGE("not init");
        return HI_HAL_ENOINIT;
    }
    return HI_SUCCESS;
}

HI_S32 hal_ahd_nvp6324_deinit(HI_HAL_AHD_DEV_S* dev)
{
    HI_APPCOMM_CHECK_RETURN(hal_check_param_valid(dev),HI_FAILURE);
    HI_S32 ret_value;
    if (ahd_nvp6324_ctx.ahd_fd == HAL_FD_INITIALIZATION_VAL)
    {
        MLOGE("ahd not initialized,no need to close\n");
        return HI_HAL_ENOINIT;
    }
    ret_value = close(ahd_nvp6324_ctx.ahd_fd);
    if (0 > ret_value)
    {
        MLOGE("halAhdfd[%d] close,fail\n",ahd_nvp6324_ctx.ahd_fd);
        return HI_HAL_EINVOKESYS;
    }
    ahd_nvp6324_ctx.ahd_fd = HAL_FD_INITIALIZATION_VAL;
    ahd_nvp6324_ctx.ahd_cfg.enRes = HI_HAL_AHD_RES_BUIT;
    ahd_nvp6324_ctx.ahd_cfg.enFps = HI_HAL_AHD_FPS_BUIT;
    jaguar1_module_deinit();
    return HI_SUCCESS;
}

HI_HAL_AHD_DEV_S ahd_nvp6324_Obj =
{
    .id = 0,
    .name = "HALNVP6324",
    .u32ChnMax= 4,
    .bUsedChn={HI_TRUE,HI_TRUE,HI_TRUE,HI_TRUE}, /*HI_TRUE means used the chn, and only change chn will update*/
    .pfnAhdInit = hal_ahd_nvp6324_init,
    .pfnAhdStart = hal_ahd_nvp6324_start,
    .pfnAhdGetStatus = hal_ahd_nvp6324_get_status,
    .pfnAhdStartChn = hal_ahd_nvp6324_start_chn,
    .pfnAhdStopChn = hal_ahd_nvp6324_stop_chn,
    .pfnAhdGetAttr = hal_ahd_nvp6324_get_attr,
    .pfnAhdSetAttr = hal_ahd_nvp6324_set_attr,
    .pfnAhdStop = hal_ahd_nvp6324_stop,
    .pfnAhdDeinit = hal_ahd_nvp6324_deinit,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
