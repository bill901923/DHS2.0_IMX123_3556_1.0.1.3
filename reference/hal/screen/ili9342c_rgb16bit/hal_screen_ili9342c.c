/**
* @file    hal_screen_ota5182.c
* @brief   hal screen implemention
*
* Copyright (c) 2017 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date      2017/12/12
* @version   1.0

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "stdlib.h"

#include "hi_appcomm_util.h"
#include "hi_hal_common.h"
#include "hi_hal_screen.h"
#include "hi_hal_screen_inner.h"
#include "hi_hal_gpio_inner.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** \addtogroup     SCREEN */
/** @{ */  /** <!-- [SCREEN] */


/**when in linux and liteos version, the branch is in Liteos end*/
/**Single Linux os Single Liteos*/
#if (defined(AMP_LINUX_HUAWEILITE) && defined(__HuaweiLite__)) || !(defined(AMP_LINUX_HUAWEILITE))

#if !defined(AMP_LINUX)
extern int  hi_ssp_lcd_init(void);
extern void  hi_ssp_lcd_exit(void);
#else
/**Single Linux os use KO*/
#define HAL_SCREEN_DEV       ("/dev/ssp_9342c")
static HI_S32 s_s32HALSCREENFd = -1;
#endif

typedef struct tagHAL_SCREEN_ILI9342C_CTX_S
{
    HI_HAL_SCREEN_STATE_E enSCREENDisplayState;
} HAL_SCREEN_ILI9342C_CTX_S;
static HAL_SCREEN_ILI9342C_CTX_S s_stHALSCREENILI9342CCtx = {HI_HAL_SCREEN_STATE_BUIT};


static HI_S32 HAL_SCREEN_ILI9342C_Init(HI_VOID)
{
MLOGD("HAL_SCREEN_ILI9342C_Init!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    HI_S32 s32Ret = HI_SUCCESS;
#if !defined(AMP_LINUX)

    s32Ret = hi_ssp_lcd_init();
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("init screen failed\n");
        return HI_HAL_EINTER;
    };

#else

    s32Ret = HI_insmod(HI_APPFS_KOMOD_PATH"/hi_ssp_ili9342.ko",NULL);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE("insmod hi_ssp_ili9342:failed\n");
        return HI_HAL_EINTER;
    }

    s_s32HALSCREENFd = open(HAL_SCREEN_DEV, O_RDWR);
    if (HAL_FD_INITIALIZATION_VAL == s_s32HALSCREENFd)
    {
        HI_rmmod(HI_APPFS_KOMOD_PATH"/hi_ssp_ili9342.ko");
        return HI_HAL_EINTER;
    }

#endif
    s_stHALSCREENILI9342CCtx.enSCREENDisplayState = HI_TRUE;

    return HI_SUCCESS;
}


static HI_S32 HAL_SCREEN_ILI9342C_GetAttr(HI_HAL_SCREEN_ATTR_S* pstAttr)
{
    MLOGD("16BIT 0xA9997 new HAL_SCREEN_ILI9342C_GetAttr!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    HI_APPCOMM_CHECK_POINTER(pstAttr, HI_HAL_EINVAL);
    pstAttr->enType = HI_HAL_SCREEN_INTF_TYPE_LCD;
    pstAttr->unScreenAttr.stLcdAttr.enType = HI_HAL_SCREEN_LCD_INTF_16BIT;
    pstAttr->stAttr.u32Width = 320;
    pstAttr->stAttr.u32Height = 240;
    pstAttr->stAttr.u32Framerate = 60;
    pstAttr->stAttr.stClkAttr.enClkType = HI_HAL_SCREEN_CLK_TYPE_LCDMCLK;
    //pstAttr->stAttr.stClkAttr.u32OutClk = 0xA9997;
    pstAttr->stAttr.stClkAttr.u32OutClk = 0x93C10;
    pstAttr->stAttr.stClkAttr.u32DevDiv = 1;
    pstAttr->stAttr.stClkAttr.bClkReverse = HI_TRUE;

    pstAttr->stAttr.stSynAttr.u16Hact = 320;
    pstAttr->stAttr.stSynAttr.u16Hbb =  30;
    pstAttr->stAttr.stSynAttr.u16Hfb = 10;
    pstAttr->stAttr.stSynAttr.u16Hpw = 10;
    pstAttr->stAttr.stSynAttr.u16Vact = 240;
    //pstAttr->stAttr.stSynAttr.u16Vbb = 2;
    //pstAttr->stAttr.stSynAttr.u16Vfb = 2;
    pstAttr->stAttr.stSynAttr.u16Vbb = 4;
    pstAttr->stAttr.stSynAttr.u16Vfb = 4;
    pstAttr->stAttr.stSynAttr.u16Vpw = 1;
    

    pstAttr->stAttr.stSynAttr.bIdv = 0;
    pstAttr->stAttr.stSynAttr.bIhs = 1;
    pstAttr->stAttr.stSynAttr.bIvs = 1;
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_GetDisplayState(HI_HAL_SCREEN_STATE_E* penDisplayState)
{
    MLOGD("HAL_SCREEN_ILI9342C_GetDisplayState!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    *penDisplayState = s_stHALSCREENILI9342CCtx.enSCREENDisplayState;
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_SetDisplayState(HI_HAL_SCREEN_STATE_E enDisplayState)
{
    MLOGD("HAL_SCREEN_ILI9342C_SetDisplayState!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    s_stHALSCREENILI9342CCtx.enSCREENDisplayState = enDisplayState;
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_GetLuma(HI_U32* pu32Luma)
{
    MLOGD("HAL_SCREEN_ILI9342C_GetLuma!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_SetLuma(HI_U32 u32Luma)
{
    MLOGD("HAL_SCREEN_ILI9342C_SetLuma!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_GetSatuature(HI_U32* pu32Satuature)
{
    MLOGD("HAL_SCREEN_ILI9342C_GetSatuature!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_SetSatuature(HI_U32 u32Satuature)
{
    MLOGD("HAL_SCREEN_ILI9342C_SetSatuature!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_GetContrast(HI_U32* pu32Contrast)
{
    MLOGD("HAL_SCREEN_ILI9342C_GetContrast!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_SetContrast(HI_U32 u32Contrast)
{
    MLOGD("HAL_SCREEN_ILI9342C_SetContrast!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_SetBackLightState(HI_HAL_SCREEN_STATE_E enBackLightState)
{
    MLOGD("HAL_SCREEN_ILI9342C_SetBackLightState!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
#if defined(HI3556AV100) || defined(HI3519AV100)
    /**GPIO0_1*/
    HI_U32 u32SetValue = 0x1 << 1;
    HI_U32 mask = 0x1 << 1;

    /**Set direction*/
    writereg(u32SetValue, mask, 0x045F2400);

    if (HI_HAL_SCREEN_STATE_OFF == enBackLightState)
    {
        himm(0x045F2008,0x0);
    }
    else if (HI_HAL_SCREEN_STATE_ON == enBackLightState)
    {
        himm(0x045F2008,0x2);
    }
#elif defined(HI3559V200)

    /**GPIO8_4*/
    HI_U32 u32SetValue = 0x1 << 4;
    HI_U32 mask = 0x1 << 4;

    /**Set direction*/
    writereg(u32SetValue, mask, 0x120D8400);

    if (HI_HAL_SCREEN_STATE_OFF == enBackLightState)
    {
        himm(0x120D8040,0x0);
    }
    else if (HI_HAL_SCREEN_STATE_ON == enBackLightState)
    {
        himm(0x120D8040,0x10);
    }

#endif
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_GetBackLightState(HI_HAL_SCREEN_STATE_E* penBackLightState)
{
    MLOGD("HAL_SCREEN_ILI9342C_GetBackLightState!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_Deinit(HI_VOID)
{
    MLOGD("HAL_SCREEN_ILI9342C_Deinit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
#if !defined(AMP_LINUX)
    hi_ssp_lcd_exit();
#else
    HI_rmmod(HI_APPFS_KOMOD_PATH"/hi_ssp_ili9342.ko");
    close(s_s32HALSCREENFd);
#endif

    return HI_SUCCESS;
}

#else

static HI_S32 HAL_SCREEN_ILI9342C_Init(HI_VOID)
{
    MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_Init FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_GetAttr(HI_HAL_SCREEN_ATTR_S* pstAttr)
{
    MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_GetAttr FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_GetDisplayState(HI_HAL_SCREEN_STATE_E* penDisplayState)
{
    MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_GetDisplayState FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_SetDisplayState(HI_HAL_SCREEN_STATE_E enDisplayState)
{
    MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_SetDisplayState FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_GetLuma(HI_U32* pu32Luma)
{
    MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_GetLuma FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_SetLuma(HI_U32 u32Luma)
{
        MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_SetLuma FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_GetSatuature(HI_U32* pu32Satuature)
{
        MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_GetSatuature FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_SetSatuature(HI_U32 u32Satuature)
{
    MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_SetSatuature FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_GetContrast(HI_U32* pu32Contrast)
{
    MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_GetContrast FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_SetContrast(HI_U32 u32Contrast)
{
    MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_SetContrast FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_SetBackLightState(HI_HAL_SCREEN_STATE_E enBackLightState)
{
    MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_SetBackLightState FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_GetBackLightState(HI_HAL_SCREEN_STATE_E* penBackLightState)
{
    MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_GetBackLightState FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return HI_SUCCESS;
}

static HI_S32 HAL_SCREEN_ILI9342C_Deinit(HI_VOID)
{
    MLOGD("FUCK1 FUCK HAL_SCREEN_ILI9342C_Deinit FUCK FUCK FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return HI_SUCCESS;
}

#endif

HI_HAL_SCREEN_OBJ_S stHALSCREENObj =
{
    .pfnInit = HAL_SCREEN_ILI9342C_Init,
    .pfnGetAttr = HAL_SCREEN_ILI9342C_GetAttr,
    .pfnSetDisplayState = HAL_SCREEN_ILI9342C_SetDisplayState,
    .pfnGetDisplayState = HAL_SCREEN_ILI9342C_GetDisplayState,
    .pfnSetBackLightState = HAL_SCREEN_ILI9342C_SetBackLightState,
    .pfnGetBackLightState = HAL_SCREEN_ILI9342C_GetBackLightState,
    .pfnSetLuma = HAL_SCREEN_ILI9342C_SetLuma,
    .pfnGetLuma = HAL_SCREEN_ILI9342C_GetLuma,
    .pfnSetSaturature = HAL_SCREEN_ILI9342C_SetSatuature,
    .pfnGetSaturature = HAL_SCREEN_ILI9342C_GetSatuature,
    .pfnSetContrast = HAL_SCREEN_ILI9342C_SetContrast,
    .pfnGetContrast = HAL_SCREEN_ILI9342C_GetContrast,
    .pfnDeinit = HAL_SCREEN_ILI9342C_Deinit,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

