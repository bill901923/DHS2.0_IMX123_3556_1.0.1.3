#include "securec.h"
#include <sys/time.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include "hi_mw_type.h"
#include "hi_list.h"
#include "tiffio.h"
#include "hi_dng_err.h"
#include "hi_dng.h"
#include "dng_log.h"
#include "dng_common.h"

#define N(a) (sizeof(a) / sizeof(a[0]))

#define DNG_CHECK_NULL_ERROR(condition) \
    do {                                \
        if (condition == NULL) {        \
            return HI_ERR_DNG_NULL_PTR; \
        }                               \
    } while (0)
#define DNG_LOCK(mutex)                       \
    do {                                      \
        (HI_VOID)pthread_mutex_lock(&mutex); \
    } while (0)

#define DNG_UNLOCK(mutex)                       \
    do {                                        \
        (HI_VOID)pthread_mutex_unlock(&mutex); \
    } while (0)

static const TIFFFieldInfo XTIFFFIELDINFO[] = {
    { TIFFTAG_PROFILEHUESATMAPDIMS,  3,  3,  TIFF_LONG,  FIELD_CUSTOM, 0, 0, (char *)("ProfileHueSatMapDims") },
    { TIFFTAG_PROFILEHUESATMAPDATA1, -1, -1, TIFF_FLOAT, FIELD_CUSTOM, 0, 1, (char *)("ProfileHueSatMapData1") },
    { TIFFTAG_PROFILEHUESATMAPDATA2, -1, -1, TIFF_FLOAT, FIELD_CUSTOM, 0, 1, (char *)("ProfileHueSatMapData2") },
    { TIFFTAG_PROFILEEMBEDPOLICY,    1,  1,  TIFF_LONG,  FIELD_CUSTOM, 0, 0, (char *)("ProfileEmbedPolicy") },

    { TIFFTAG_ORIGINALDEFAULTFINALSIZE, 2,  2,  TIFF_LONG,      FIELD_CUSTOM, 0, 0, (char *)("OriginalDefaultFinalSize") },
    { TIFFTAG_ORIGINALBESTQUALITYSIZE,  2,  2,  TIFF_LONG,      FIELD_CUSTOM, 0, 0, (char *)("OriginalBestQualitySize") },
    { TIFFTAG_ORIGINALDEFAULTCROPSIZE,  2,  2,  TIFF_RATIONAL,  FIELD_CUSTOM, 0, 0, (char *)("OriginalDefaultCropSize") },
    { TIFFTAG_FORWARDMATRIX1,           9,  9,  TIFF_SRATIONAL, FIELD_CUSTOM, 0, 0, (char *)("ForwardMatrix1") },
    { TIFFTAG_FORWARDMATRIX2,           9,  9,  TIFF_SRATIONAL, FIELD_CUSTOM, 0, 0, (char *)("ForwardMatrix2") },
    { TIFFTAG_OPCODELIST1,              -1, -1, TIFF_UNDEFINED, FIELD_CUSTOM, 0, 1, (char *)("OpcodeList1") },
    { TIFFTAG_DEFAULTUSERCROP,          4,  4,  TIFF_RATIONAL,  FIELD_CUSTOM, 0, 0, (char *)("DefaultUserCrop") },
    { TIFFTAG_NOISEPROFILE,             -1, -1, TIFF_DOUBLE,    FIELD_CUSTOM, 0, 1, (char *)("NoiseProfile") },
    { TIFFTAG_XPCOMMENT,                -1, -1, TIFF_BYTE,      FIELD_CUSTOM, 0, 1, (char *)("XPComment") },
    { TIFFTAG_XPKEYWORDS,               -1, -1, TIFF_BYTE,      FIELD_CUSTOM, 0, 1, (char *)("XPKeywords") },

    { GPSTAG_VERSIONID,    4, 4, TIFF_BYTE,     FIELD_CUSTOM, 0, 0, (char *)("GPSVersionId") },
    { GPSTAG_LATITUDEREF,  2, 2, TIFF_ASCII,    FIELD_CUSTOM, 0, 0, (char *)("GPSLatitudeRef") },
    { GPSTAG_LATITUDE,     3, 3, TIFF_RATIONAL, FIELD_CUSTOM, 0, 0, (char *)("GPSLatitude") },
    { GPSTAG_LONGITUDEREF, 2, 2, TIFF_ASCII,    FIELD_CUSTOM, 0, 0, (char *)("GPSLongitudeRef") },
    { GPSTAG_LONGITUDE,    3, 3, TIFF_RATIONAL, FIELD_CUSTOM, 0, 0, (char *)("GPSLongitude") },
    { GPSTAG_ALTITUDEREF,  1, 1, TIFF_BYTE,     FIELD_CUSTOM, 0, 0, (char *)("GPSAltitudeRef") },
    { GPSTAG_ALTITUDE,     1, 1, TIFF_RATIONAL, FIELD_CUSTOM, 0, 0, (char *)("GPSAltitude") },

};
// The const_cast<char*> casts are necessary because the
// string literals are inherently const,
// but the definition of TIFFFieldInfo
// requires a non-const string pointer.
// The Intel and Microsoft compilers
// tolerate this, but gcc doesn't.

static HiDngCtxS g_dngCtx = { HI_FALSE, 0, PTHREAD_MUTEX_INITIALIZER,
                                 HI_LIST_INIT_HEAD_DEFINE(g_dngCtx.dngList)
                               };

static TIFFExtendProc g_parentExtender = NULL;  // In case we want a chain of extensions

static void registerCustomTIFFTags(TIFF *tif)
{
    /* Install the extended Tag field info */
    HI_S32 ret = HI_SUCCESS;
    ret = TIFFMergeFieldInfo(tif, XTIFFFIELDINFO, N(XTIFFFIELDINFO));
    if (ret < 0) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "TIFFMergeFieldInfo failed   \n");
    }
    if (g_parentExtender) {
       (*g_parentExtender)(tif);
    }
    return;
}

HI_VOID SAMPLE_AugmentTiffWithCustomTags()
{
    static HI_BOOL firstTime = HI_TRUE;

    if (!firstTime) {
        return;
    }
    firstTime = HI_FALSE;
    g_parentExtender = TIFFSetTagExtender(registerCustomTIFFTags);

    return;
}

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static HI_S32 DNG_CheckCfg(HI_DNG_CONFIG_S *dngCfg)
{
    HI_U32 i = 0;
    HI_BOOL found = HI_FALSE;
    for (i = 0; i < DNG_MAX_FILE_NAME; i++) {
        if ('\0' == dngCfg->aszFileName[i]) {
            found = HI_TRUE;
            break;
        } else if (';' == dngCfg->aszFileName[i]) {
            return HI_ERR_DNG_INVALIDARG;
        }
    }

    if ((HI_TRUE != found) || (strlen(dngCfg->aszFileName) >= DNG_MAX_FILE_NAME)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport DNG filename \n");
        return HI_ERR_DNG_INVALIDARG;
    }

    if (dngCfg->enConfigType < HI_DNG_CONFIG_TYPE_MUXER || dngCfg->enConfigType >= HI_DNG_CONFIG_TYPE_BUTT) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport DNG config type :%d \n", dngCfg->enConfigType);
        return HI_ERR_DNG_INVALIDARG;
    }

    return HI_SUCCESS;
}
static HI_BOOL DNG_FindDng(HI_MW_PTR pHandle)
{
    List_Head_S *tmpNode = NULL;
    List_Head_S *posNode = NULL;
    DngFormatS *dngNode = NULL;

    DNG_LOCK(g_dngCtx.dngListLock);
    HI_List_For_Each_Safe(posNode, tmpNode, &g_dngCtx.dngList)
    {
        dngNode = HI_LIST_ENTRY(posNode, DngFormatS, dngListPtr);
        if (pHandle == (HI_MW_PTR)dngNode) {
            DNG_UNLOCK(g_dngCtx.dngListLock);
            return HI_TRUE;
        }
    }
    DNG_UNLOCK(g_dngCtx.dngListLock);

    return HI_FALSE;
}

HI_S32 HI_DNG_Create(HI_MW_PTR *ppDng, HI_DNG_CONFIG_S *dngCfg)
{
    HI_S32 ret = HI_SUCCESS;
    DngFormatS *dng = NULL;
    TIFF *tif = NULL;
    HI_U16 count16 = 0;
    HI_U64 *subifdoffset = NULL;
    HI_CHAR chrFileName[PATH_MAX + 1] = "";

    DNG_CHECK_NULL_ERROR(ppDng);
    DNG_CHECK_NULL_ERROR(dngCfg);

    ret = DNG_CheckCfg(dngCfg);
    if (HI_SUCCESS != ret) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "HI_DNG_Create fail cfg param illegal\n");
        return HI_ERR_DNG_INVALIDARG;
    }

    memset_s(chrFileName, sizeof(chrFileName), 0, sizeof(chrFileName));

    if (0 == g_dngCtx.dngNum) {
        SAMPLE_AugmentTiffWithCustomTags();
    }
    if (DNG_MAX_COUNT <= g_dngCtx.dngNum) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "HI_DNG_Create fail, is full \n");
        return HI_ERR_DNG_REACH_MAX;
    }

    dng = (DngFormatS *)malloc(sizeof(DngFormatS));
    if (NULL == dng) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng muxer malloc error \n");
        return HI_ERR_DNG_MALLOC;
    }
    memset_s(dng, sizeof(DngFormatS), 0x00, sizeof(DngFormatS));
    dng->configType = dngCfg->enConfigType;
    pthread_mutex_init(&dng->mDngLock, NULL);

    if (HI_DNG_CONFIG_TYPE_MUXER == dngCfg->enConfigType) {
        snprintf_s(chrFileName, PATH_MAX + 1, DNG_MAX_FILE_NAME, "%s", dngCfg->aszFileName);
        tif = TIFFOpen(chrFileName, "w");
        if (NULL == tif) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng muxer open file error \n");
            ret = HI_ERR_DNG_CREATE_MUXER;
            goto ERROR;
        }
    } else if (HI_DNG_CONFIG_TYPE_DEMUXER == dngCfg->enConfigType) {
        if (NULL == realpath(dngCfg->aszFileName, chrFileName)) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_WARN, "HI_DNG_Create cfg file is not exist!\n");
            ret = HI_ERR_DNG_INVALIDARG;
            goto ERROR;
        }
        tif = TIFFOpen(chrFileName, "rb");
        if (NULL == tif) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng demuxer open file error \n");
            ret = HI_ERR_DNG_CREATE_DEMUXER;
            goto ERROR;
        }
        TIFFGetField(tif, TIFFTAG_SUBIFD, &count16, &subifdoffset);
        dng->subIfdNum = count16;
    }

    dng->tiffHandle = tif;

    DNG_LOCK(g_dngCtx.dngListLock);

    HI_List_Add(&(dng->dngListPtr), &(g_dngCtx.dngList));
    g_dngCtx.dngNum += 1;

    DNG_UNLOCK(g_dngCtx.dngListLock);

    *ppDng = (HI_MW_PTR)dng;

    return HI_SUCCESS;

ERROR:
    if (dng) {
        free(dng);
        dng = NULL;
    }
    return ret;
}

HI_S32 HI_DNG_Destroy(HI_MW_PTR pHandle)
{
    TIFF *tif = NULL;
    DngFormatS *dng = NULL;
    DNG_CHECK_NULL_ERROR(pHandle);
    List_Head_S *posNode = NULL;
    List_Head_S *tmpNode = NULL;
    DngFormatS *dngNode = NULL;

    if (!DNG_FindDng(pHandle)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng muxer handle not find \n");
        return HI_ERR_DNG_HANDLE_INVALID;
    }

    dng = (DngFormatS *)pHandle;

    DNG_LOCK(g_dngCtx.dngListLock);
    HI_List_For_Each_Safe(posNode, tmpNode, &g_dngCtx.dngList)
    {
        dngNode = HI_LIST_ENTRY(posNode, DngFormatS, dngListPtr);
        /* delete the stream */
        if (pHandle == (HI_MW_PTR)dngNode) {
            HI_List_Del(&(dngNode->dngListPtr));
            g_dngCtx.dngNum -= 1;
        }
    }

    DNG_UNLOCK(g_dngCtx.dngListLock);

    tif = (TIFF *)dng->tiffHandle;
    TIFFClose(tif);
    pthread_mutex_destroy(&dng->mDngLock);

    if (dng) {
        free(dng);
        dng = NULL;
    }

    return HI_SUCCESS;
}

HI_S32 HI_DNG_AddIFD(HI_MW_PTR pDng, HI_DNG_MUXER_IFD_INFO_S *iFDInfo)
{
    HI_S32 ret = HI_SUCCESS;
    TIFF *tif = NULL;
    DngFormatS *dng = NULL;

    DNG_CHECK_NULL_ERROR(pDng);
    DNG_CHECK_NULL_ERROR(iFDInfo);

    if (!DNG_FindDng(pDng)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng handle invalid \n");
        return HI_ERR_DNG_HANDLE_INVALID;
    }
    dng = (DngFormatS *)pDng;
    if (HI_DNG_CONFIG_TYPE_MUXER != dng->configType) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng  add ifd not support config type \n");
        return HI_ERR_DNG_ADD_IFD;
    }
    ret = DNG_MUXER_CheckIfdInfo(dng, iFDInfo);
    if (HI_SUCCESS != ret) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng add ifd info parm illegal \n");
        return ret;
    }

    DNG_LOCK(dng->mDngLock);

    tif = (TIFF *)dng->tiffHandle;
    if (HI_DNG_MUXER_IMAGE_IFD0 == iFDInfo->enImageType) {
        ret = DNG_MUXER_SetIFD0(tif, iFDInfo);
        if (HI_SUCCESS != ret) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "HI_DNG_AddIFD fail set IFD error image type %d \n",
                           iFDInfo->enImageType);
            DNG_UNLOCK(dng->mDngLock);
            return ret;
        }

        dng->iFD0Flag = HI_TRUE;
        dng->subIfdNum = iFDInfo->stIFD0Info.u32SubIFDs;
    } else if ((HI_DNG_MUXER_IMAGE_RAW == iFDInfo->enImageType) && (dng->subIfdNum > 0)) {
        ret = DNG_MUXER_SetRawIFD(tif, iFDInfo);
        if (HI_SUCCESS != ret) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "HI_DNG_AddIFD fail set IFD error image type %d \n",
                           iFDInfo->enImageType);
            DNG_UNLOCK(dng->mDngLock);
            return ret;
        }
        dng->subIfdNum -= 1;
    } else if ((HI_DNG_MUXER_IMAGE_SCREEN == iFDInfo->enImageType) && (dng->subIfdNum > 0)) {
        ret = DNG_MUXER_SetScreenNailIFD(tif, iFDInfo);
        if (HI_SUCCESS != ret) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "HI_DNG_AddIFD fail set IFD error image type %d \n",
                           iFDInfo->enImageType);
            DNG_UNLOCK(dng->mDngLock);
            return ret;
        }
        dng->subIfdNum -= 1;
    }

    if (1 != TIFFWriteDirectory(tif)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "HI_DNG_AddIFD fail TIFFWriteDirectory error \n");
        DNG_UNLOCK(dng->mDngLock);
        return HI_FAILURE;
    }

    DNG_UNLOCK(dng->mDngLock);

    return HI_SUCCESS;
}

HI_S32 HI_DNG_GetImageInfo(HI_MW_PTR pDng, HI_U32 ifdIndex, HI_DNG_DEMUXER_IMAGE_INFO_S *imageInfo)
{
    DngFormatS *dng = NULL;

    TIFF *tif = NULL;
    HI_S32 size = 0;
    HI_U16 count16 = 0;
    HI_U64 *subifdoffset = NULL;

    DNG_CHECK_NULL_ERROR(pDng);
    DNG_CHECK_NULL_ERROR(imageInfo);

    if (!DNG_FindDng(pDng)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng demuxer handle invalid \n");
        return HI_ERR_DNG_HANDLE_INVALID;
    }

    dng = (DngFormatS *)pDng;
    if (HI_DNG_CONFIG_TYPE_DEMUXER != dng->configType) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng getImageInfo not support config type \n");
        return HI_ERR_DNG_GET_INFO;
    }

    tif = (TIFF *)dng->tiffHandle;

    if (ifdIndex > dng->subIfdNum) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng demuxer do not have such ifd index:%d  ifd sum num :%d \n",
                       ifdIndex, (dng->subIfdNum + 1));
        return HI_ERR_DNG_INVALIDARG;
    }

    DNG_LOCK(dng->mDngLock);

    if (0 == ifdIndex) {
        if (0 == TIFFSetDirectory(tif, 0)) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng demuxer TIFFSetDirectory for ifd0 failed \n");
            DNG_UNLOCK(dng->mDngLock);
            return HI_ERR_DNG_GET_INFO;
        }
        size = TIFFRawStripSize(tif, 0);

    } else {
        if (0 == TIFFGetField(tif, TIFFTAG_SUBIFD, &count16, &subifdoffset)) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng demuxer get image info TIFFTAG_SUBIFD error \n");
            DNG_UNLOCK(dng->mDngLock);
            return HI_ERR_DNG_GET_INFO;
        }
        if (ifdIndex > TIFFTAG_SUBIFD) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR,
                           "dng demuxer do not have such ifd index:%d , ifd sum num :%d \n", ifdIndex, (count16 + 1));
            DNG_UNLOCK(dng->mDngLock);
            return HI_ERR_DNG_INVALIDARG;
        }

        if (0 == TIFFSetSubDirectory(tif, subifdoffset[ifdIndex - 1])) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng demuxer TIFFSetSubDirectory for subifd failed \n");
            DNG_UNLOCK(dng->mDngLock);
            return HI_ERR_DNG_GET_INFO;
        }
        size = TIFFRawStripSize(tif, 0);
    }

    if (size <= 0) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng demuxer get ifd size error \n");
        DNG_UNLOCK(dng->mDngLock);
        return HI_ERR_DNG_GET_INFO;
    }

    imageInfo->u32DataLen = size;

    if (0 == TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageInfo->u32Width)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng demuxer get TIFFTAG_IMAGEWIDTH error \n");
        DNG_UNLOCK(dng->mDngLock);
        return HI_ERR_DNG_GET_INFO;
    }

    if (0 == TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageInfo->u32Height)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng demuxer get TIFFTAG_IMAGELENGTH  error \n");
        DNG_UNLOCK(dng->mDngLock);
        return HI_ERR_DNG_GET_INFO;
    }

    dng->getInfoFlag = HI_TRUE;
    dng->curIndex = ifdIndex;

    DNG_UNLOCK(dng->mDngLock);

    return HI_SUCCESS;
}

// please call after get the image info,HI_DNG_GetImageInfo
HI_S32 HI_DNG_GetImageData(HI_MW_PTR pHandle, HI_U8 *buffer, HI_U32 dataLen)
{
    DngFormatS *dng = NULL;
    TIFF *tif = NULL;

    DNG_CHECK_NULL_ERROR(pHandle);
    DNG_CHECK_NULL_ERROR(buffer);
    if (!DNG_FindDng(pHandle)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng handle invalid \n");
        return HI_ERR_DNG_HANDLE_INVALID;
    }

    dng = (DngFormatS *)pHandle;
    if (HI_DNG_CONFIG_TYPE_DEMUXER != dng->configType) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng getImageData not support config type \n");
        return HI_ERR_DNG_GET_IMAGE;
    }

    if (HI_FALSE == dng->getInfoFlag) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR,
                       "please get image info first \n");
        return HI_ERR_DNG_GET_IMAGE;
    }

    tif = (TIFF *)dng->tiffHandle;
    if (NULL == dng->tiffHandle) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "dng tiff handle is null \n");
        return HI_ERR_DNG_GET_IMAGE;
    }
    DNG_LOCK(dng->mDngLock);

    if (-1 == TIFFReadRawStrip(tif, 0, buffer, dataLen)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "HI_DNG_GetImageData TIFFReadRawStrip failed  \n");
        DNG_UNLOCK(dng->mDngLock);
        return HI_ERR_DNG_GET_IMAGE;
    }

    DNG_UNLOCK(dng->mDngLock);

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
