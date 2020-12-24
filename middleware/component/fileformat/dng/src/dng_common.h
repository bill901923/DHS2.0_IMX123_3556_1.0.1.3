/**
  * Copyright (C), 2016-2020, Hisilicon Tech. Co., Ltd.
  * All rights reserved.
  *
  * @file       dng_muxer.h
  * @brief      middleware log function.
  * @author   HiMobileCam middleware develop team
  * @date      2018.03.08
  */
#ifndef __DNG_COMMON_H__
#define __DNG_COMMON_H__

#include "hi_mw_type.h"
#include "hi_defs.h"
#include "hi_dng.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
#define DNG_MUXER_SUBIFD_MAX (2)
#define DNG_MAX_COUNT        (32) /* instance count max */

typedef struct HiDNGFormatS {
    HI_DNG_CONFIG_TYPE_E configType;
    TIFF *tiffHandle;
    HI_BOOL iFD0Flag;     // add ifd0 or not
    HI_BOOL getInfoFlag;  // get image info or not
    HI_U32 subIfdNum;   // subifd num
    HI_U32 curIndex;
    List_Head_S dngListPtr; /* list node for mp4 */
    pthread_mutex_t mDngLock;
} DngFormatS;

typedef struct HiDNGCtxS {
    HI_BOOL proc;
    HI_S32 dngNum;
    pthread_mutex_t dngListLock;
    List_Head_S dngList;
} HiDngCtxS;

HI_S32 DNG_MUXER_SetScreenNailIFD(TIFF *tif, HI_DNG_MUXER_IFD_INFO_S *iFDInfo);
HI_S32 DNG_MUXER_SetRawIFD(TIFF *tif, HI_DNG_MUXER_IFD_INFO_S *iFDInfo);
HI_S32 DNG_MUXER_SetIFD0(TIFF *tif, HI_DNG_MUXER_IFD_INFO_S *iFDInfo);
HI_S32 DNG_MUXER_CheckIfdInfo(DngFormatS *dng, HI_DNG_MUXER_IFD_INFO_S *iFDInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DNG_COMMON_H__ */
