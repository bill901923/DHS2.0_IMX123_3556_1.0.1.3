#include "securec.h"
#include <sys/time.h>
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
#include "dng_common.h"
#include "dng_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define DNG_MAX_WIDTH (30000)

static HI_S32 DNG_MUXER_ConvertBitPixel(HI_U8 *data, HI_U32 dataNum, HI_U16 bitPerSample,
                                        HI_U16 *outData)
{
    HI_S32 i = 0;
    HI_S32 segmentCnt = 0;
    HI_S32 outCnt = 0;
    HI_U32 val = 0;
    HI_U64 segmentValue = 0;
    HI_U8 *curSegmentBuf = NULL;

    switch (bitPerSample) {
        case 10: {
            /* 4 pixels consist of 5 bytes  */
            segmentCnt = dataNum / 4;

            for (i = 0; i < segmentCnt; i++) {
                /* byte4 byte3 byte2 byte1 byte0 */
                curSegmentBuf = data + 5 * i;
                segmentValue = curSegmentBuf[0] + ((HI_U32)curSegmentBuf[1] << 8) + ((HI_U32)curSegmentBuf[2] << 16) +
                         ((HI_U32)curSegmentBuf[3] << 24) + ((HI_U64)curSegmentBuf[4] << 32);

                outData[outCnt++] = segmentValue & 0x3ff;
                outData[outCnt++] = (segmentValue >> 10) & 0x3ff;
                outData[outCnt++] = (segmentValue >> 20) & 0x3ff;
                outData[outCnt++] = (segmentValue >> 30) & 0x3ff;
            }
        }
        break;

        case 12: {
            /* 2 pixels consist of 3 bytes  */
            segmentCnt = dataNum / 2;

            for (i = 0; i < segmentCnt; i++) {
                /* byte2 byte1 byte0 */
                curSegmentBuf = data + 3 * i;
                val = curSegmentBuf[0] + (curSegmentBuf[1] << 8) + (curSegmentBuf[2] << 16);
                outData[outCnt++] = val & 0xfff;
                outData[outCnt++] = (val >> 12) & 0xfff;
            }
        }
        break;

        case 14: {
            /* 4 pixels consist of 7 bytes  */
            segmentCnt = dataNum / 4;

            for (i = 0; i < segmentCnt; i++) {
                curSegmentBuf = data + 7 * i;
                segmentValue = curSegmentBuf[0] + ((HI_U32)curSegmentBuf[1] << 8) + ((HI_U32)curSegmentBuf[2] << 16) +
                         ((HI_U32)curSegmentBuf[3] << 24) + ((HI_U64)curSegmentBuf[4] << 32) +
                         ((HI_U64)curSegmentBuf[5] << 40) + ((HI_U64)curSegmentBuf[6] << 48);

                outData[outCnt++] = segmentValue & 0x3fff;
                outData[outCnt++] = (segmentValue >> 14) & 0x3fff;
                outData[outCnt++] = (segmentValue >> 28) & 0x3fff;
                outData[outCnt++] = (segmentValue >> 42) & 0x3fff;
            }
        }
        break;

        default:
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsuport raw u16BitPerSample: %d\n", bitPerSample);
            return -1;
            break;
    }

    return outCnt;
}

static HI_S32 DNG_MUXER_WriteRawData(TIFF *tif, HI_DNG_MUXER_IFD_INFO_S *iFDInfo)
{
    HI_U8 *tmpBuf = NULL;
    HI_U8 *dstBuf = NULL;
    HI_U32 row = 0;
    HI_U32 rowLen = 0;
    HI_S32 ret = HI_SUCCESS;
    HI_U32 width = 0;
    HI_U32 height = 0;

    tmpBuf = iFDInfo->stImageData.pu8DataBuf;
    width = iFDInfo->u32Width;
    height = iFDInfo->u32Height;
    rowLen = width;

    if ((iFDInfo->u16BitPerSample > 8) && (width > 0)) {
        rowLen = width * 2;
    } else {
        rowLen = width;
    }
    dstBuf = (HI_U8*)malloc(rowLen);
    if (dstBuf == NULL) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_WriteRawData pu16Data Malloc error\n");
        ret = HI_FAILURE;
        return ret;
    }
    memset_s(dstBuf, rowLen, 0x00, rowLen);

    for (row = 0; row < height; row++) {
        if ((iFDInfo->u16BitPerSample == 8) || (iFDInfo->u16BitPerSample == 16)) {
            memcpy_s(dstBuf, rowLen, tmpBuf, rowLen);
            if (TIFFWriteScanline(tif, dstBuf, row, 0) == -1) {
                DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_WriteRawData TIFFWriteScanline error\n");
                ret = HI_FAILURE;
                goto REL_DATA;
            }
        } else {
            if (DNG_MUXER_ConvertBitPixel(tmpBuf, width, iFDInfo->u16BitPerSample, (HI_U16*)dstBuf) == -1) {
                DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_WriteRawData DNG_MUXER_ConvertBitPixel fail \n");
                ret = HI_FAILURE;
                goto REL_DATA;
            }

            if (TIFFWriteScanline(tif, dstBuf, row, 0) == -1) {
                DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_WriteRawData TIFFWriteScanline fail \n");
                ret = HI_FAILURE;
                goto REL_DATA;
            }
        }

        tmpBuf += iFDInfo->stImageData.u32Stride;
        memset_s(dstBuf, rowLen, 0x00, rowLen);
    }

REL_DATA:
    if (dstBuf) {
        free(dstBuf);
        dstBuf = NULL;
    }

    return ret;
}

static HI_S32 DNG_MUXER_SetNoramlTag(TIFF *tif, HI_DNG_MUXER_IFD_INFO_S *iFDInfo)
{
    TIFFSetField(tif, TIFFTAG_SUBFILETYPE, iFDInfo->u32SubfileType);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, iFDInfo->u16SamplePerPixel);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, iFDInfo->u16Compression);
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, iFDInfo->u32Width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, iFDInfo->u32Height);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, iFDInfo->u16Photometric);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, iFDInfo->u16PlanarConfig);

    if (iFDInfo->u16Compression == COMPRESSION_JPEG) {
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, iFDInfo->u16BitPerSample);
        TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
    } else if (iFDInfo->u16Compression == COMPRESSION_NONE) {
        if (iFDInfo->u16BitPerSample > 8) {
            TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 16);
        } else {
            TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
        }
    }

    return HI_SUCCESS;
}

static HI_S32 DNG_MUXER_SetExifIFD(TIFF *tif, HI_DNG_MUXER_EXIF_INFO_S *exifInfo, HI_U64 *exifOffset)
{
    HI_U64 tmpExifOffset = 0;

    if (TIFFCreateEXIFDirectory(tif) != 0) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetExifIFD fail TIFFCreateEXIFDirectory error\n");
        return HI_FAILURE;
    }

    TIFFSetField(tif, EXIFTAG_ISOSPEEDRATINGS, 1, &exifInfo->u16ISOSpeedRatings);
    TIFFSetField(tif, EXIFTAG_EXPOSUREBIASVALUE, exifInfo->fExposureBiasValue);
    TIFFSetField(tif, EXIFTAG_EXPOSURETIME, exifInfo->fExposureTime);
    TIFFSetField(tif, EXIFTAG_FNUMBER, exifInfo->fFNumber);
    TIFFSetField(tif, EXIFTAG_FOCALLENGTH, exifInfo->fFocalLength);
    TIFFSetField(tif, EXIFTAG_EXPOSUREMODE, exifInfo->u16ExposureMode);
    TIFFSetField(tif, EXIFTAG_EXPOSUREPROGRAM, exifInfo->u16ExposureProgram);
    TIFFSetField(tif, EXIFTAG_LIGHTSOURCE, exifInfo->u16LightSource);
    TIFFSetField(tif, EXIFTAG_METERINGMODE, exifInfo->u16MeteringMode);
    TIFFSetField(tif, EXIFTAG_SATURATION, exifInfo->u16Saturation);
    TIFFSetField(tif, EXIFTAG_SHARPNESS, exifInfo->u16Sharpness);
    TIFFSetField(tif, EXIFTAG_WHITEBALANCE, exifInfo->u16WhiteBalance);

    TIFFSetField(tif, EXIFTAG_DATETIMEORIGINAL, exifInfo->aszDateTimeOriginal);
    TIFFSetField(tif, EXIFTAG_DATETIMEDIGITIZED, exifInfo->aszCaptureTime);
    TIFFSetField(tif, EXIFTAG_FLASH, exifInfo->u16Flash);
    TIFFSetField(tif, EXIFTAG_DIGITALZOOMRATIO, exifInfo->fDigitalZoomRatio);
    TIFFSetField(tif, EXIFTAG_MAXAPERTUREVALUE, exifInfo->fMaxApertureValue);
    TIFFSetField(tif, EXIFTAG_CONTRAST, exifInfo->u16Contrast);
    TIFFSetField(tif, EXIFTAG_CUSTOMRENDERED, exifInfo->u16CustomRendered);
    TIFFSetField(tif, EXIFTAG_FOCALLENGTHIN35MMFILM, exifInfo->u16FocalLengthIn35mmFilm);
    TIFFSetField(tif, EXIFTAG_GAINCONTROL, exifInfo->u16GainControl);

    TIFFSetField(tif, EXIFTAG_SCENECAPTURETYPE, exifInfo->u16SceneCaptureType);
    TIFFSetField(tif, EXIFTAG_SCENETYPE, exifInfo->u8SceneType);

    if (TIFFWriteCustomDirectory(tif, (uint64 *)&tmpExifOffset) != 1) {  //
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetExifIFD fail TIFFWriteCustomDirectory error\n");
        return HI_FAILURE;
    }

    *exifOffset = tmpExifOffset;
    return HI_SUCCESS;
}

static HI_S32 DNG_MUXER_SetGpsIFD(TIFF *tif, HI_DNG_MUXER_GPS_INFO_S *gpsInfo, HI_U64 *gpsOffset)
{
    HI_U64 tmpGpsOffset = 0;

    if (TIFFCreateDirectory(tif) != 0) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetGpsIFD TIFFCreateDirectory failed.\n");
        return HI_FAILURE;
    }
    TIFFSetField(tif, GPSTAG_VERSIONID, gpsInfo->au8GpsVersion);
    TIFFSetField(tif, GPSTAG_ALTITUDEREF, gpsInfo->u8GPSAltitudeRef);
    TIFFSetField(tif, GPSTAG_ALTITUDE, gpsInfo->fGPSAltitude);
    TIFFSetField(tif, GPSTAG_LATITUDE, gpsInfo->afGPSLatitude);
    TIFFSetField(tif, GPSTAG_LONGITUDE, gpsInfo->afGPSLongitude);
    TIFFSetField(tif, GPSTAG_LATITUDEREF, &gpsInfo->chGPSLatitudeRef);
    TIFFSetField(tif, GPSTAG_LONGITUDEREF, &gpsInfo->chGPSLongitudeRef);

    if (TIFFWriteCustomDirectory(tif, (uint64 *)&tmpGpsOffset) != 1) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetExifIFD fail TIFFWriteCustomDirectory error\n");
        return HI_FAILURE;
    }

    *gpsOffset = tmpGpsOffset;

    return HI_SUCCESS;
}

HI_S32 DNG_MUXER_CheckIfdInfo(DngFormatS *dng, HI_DNG_MUXER_IFD_INFO_S *IFDInfo)
{
    if (IFDInfo->stImageData.pu8DataBuf == NULL) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport pstIFDInfo->stImageData.pu8DataBuf null \n");
        return HI_ERR_DNG_NULL_PTR;
    }

    if (IFDInfo->stImageData.u32DataLen == 0) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport pstIFDInfo->stImageData.u32DataLen zero \n");
        return HI_ERR_DNG_INVALIDARG;
    }

    if (IFDInfo->u32Height == 0 || IFDInfo->u32Width == 0) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport IFD  width or height \n");
        return HI_ERR_DNG_INVALIDARG;
    }

    if ((IFDInfo->u32Width > DNG_MAX_WIDTH) || (IFDInfo->u32Height > DNG_MAX_WIDTH)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport IFD  width larger than max \n");
        return HI_ERR_DNG_INVALIDARG;
    }

    if (IFDInfo->enImageType < HI_DNG_MUXER_IMAGE_IFD0 || IFDInfo->enImageType > HI_DNG_MUXER_IMAGE_RAW) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport enImageType: %d \n", IFDInfo->enImageType);
        return HI_ERR_DNG_INVALIDARG;
    }

    if (dng->iFD0Flag == HI_FALSE && IFDInfo->enImageType != HI_DNG_MUXER_IMAGE_IFD0) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "not add ifd0 yet, do not add other ifd:%d !  \n",
                       IFDInfo->enImageType);
        return HI_ERR_DNG_ADD_IFD;
    }

    if (dng->iFD0Flag == HI_TRUE) {
        if (IFDInfo->enImageType == HI_DNG_MUXER_IMAGE_IFD0) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "already add ifd0 yet, do not add ifd0 again ! \n");
            return HI_ERR_DNG_ADD_IFD;
        }
        if (dng->subIfdNum == 0) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "already add all subifd yet, do not add subifd again ! \n");
            return HI_ERR_DNG_ADD_IFD;
        }
    }

    if (PHOTOMETRIC_CFA != IFDInfo->u16Photometric && PHOTOMETRIC_YCBCR != IFDInfo->u16Photometric) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport pstIFDInfo->u16Photometric: %d \n",
                       IFDInfo->u16Photometric);
        return HI_ERR_DNG_INVALIDARG;
    }

    if (COMPRESSION_NONE != IFDInfo->u16Compression && COMPRESSION_JPEG != IFDInfo->u16Compression) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport pstIFDInfo->u16Compressions: %d \n",
                       IFDInfo->u16Compression);
        return HI_ERR_DNG_INVALIDARG;
    }

    if (HI_DNG_MUXER_IMAGE_RAW == IFDInfo->enImageType) {
        if (IFDInfo->u32Height * IFDInfo->stImageData.u32Stride > IFDInfo->stImageData.u32DataLen) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER data info error height %d  datalen %d stride:%d \n",
                           IFDInfo->u32Height, IFDInfo->stImageData.u32DataLen, IFDInfo->stImageData.u32Stride);
            return HI_ERR_DNG_INVALIDARG;
        }

        if (IFDInfo->stRawIFDInfo.stRawFormat.enCfaLayout < DNG_MUXER_CFALAYOUT_TYPE_RECTANGULAR ||
            IFDInfo->stRawIFDInfo.stRawFormat.enCfaLayout > DNG_MUXER_CFALAYOUT_TYPE_H) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR,
                           "unsupport pstIFDInfo->stIFD0Info.stRawFormat.enCfaLayout: %d for raw\n",
                           IFDInfo->stIFD0Info.stRawFormat.enCfaLayout);
            return HI_ERR_DNG_INVALIDARG;
        }

        if (0 != IFDInfo->u32SubfileType % 2) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport pstIFDInfo->u32SubfileType: %d for raw\n",
                           IFDInfo->u32SubfileType);
            return HI_ERR_DNG_INVALIDARG;
        }

        if (COMPRESSION_NONE != IFDInfo->u16Compression) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport pstIFDInfo->u16Compressions: %d for raw\n",
                           IFDInfo->u16Compression);
            return HI_ERR_DNG_INVALIDARG;
        }
    }
    if (HI_DNG_MUXER_IMAGE_SCREEN == IFDInfo->enImageType) {
        if (0 == IFDInfo->u32SubfileType % 2) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport pstIFDInfo->u32SubfileType: %d for screen image\n",
                           IFDInfo->u32SubfileType);
            return HI_ERR_DNG_INVALIDARG;
        }
        if (COMPRESSION_NONE == IFDInfo->u16Compression) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport pstIFDInfo->u16Compressions: %d for screen image\n",
                           IFDInfo->u16Compression);
            return HI_ERR_DNG_INVALIDARG;
        }
    }

    if (HI_DNG_MUXER_IMAGE_IFD0 == IFDInfo->enImageType &&
        0 != IFDInfo->stIFD0Info.u32SubIFDs) {  // ifd0 is thm,include sub raw

        if (IFDInfo->stIFD0Info.u32SubIFDs > DNG_MUXER_SUBIFD_MAX) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport subifd num: %d for ifd0 \n",
                           IFDInfo->stIFD0Info.u32SubIFDs);
            return HI_ERR_DNG_INVALIDARG;
        }

        if (0 == IFDInfo->u32SubfileType % 2) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport pstIFDInfo->u32SubfileType: %d for ifd0 \n",
                           IFDInfo->u32SubfileType);
            return HI_ERR_DNG_INVALIDARG;
        }

        if (COMPRESSION_NONE == IFDInfo->u16Compression) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR,
                           "unsupport pstIFDInfo->u16Compressions: %d for thm ifd0 image\n", IFDInfo->u16Compression);
            return HI_ERR_DNG_INVALIDARG;
        }
    }

    if (HI_DNG_MUXER_IMAGE_IFD0 == IFDInfo->enImageType && 0 == IFDInfo->stIFD0Info.u32SubIFDs) {  // one raw
        if (IFDInfo->u32Height * IFDInfo->stImageData.u32Stride > IFDInfo->stImageData.u32DataLen) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER data info error height %d  datalen %d stride:%d \n",
                           IFDInfo->u32Height, IFDInfo->stImageData.u32DataLen, IFDInfo->stImageData.u32Stride);
            return HI_ERR_DNG_INVALIDARG;
        }

        if (IFDInfo->stIFD0Info.stRawFormat.enCfaLayout < DNG_MUXER_CFALAYOUT_TYPE_RECTANGULAR ||
            IFDInfo->stIFD0Info.stRawFormat.enCfaLayout > DNG_MUXER_CFALAYOUT_TYPE_H) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR,
                           "unsupport pstIFDInfo->stIFD0Info.stRawFormat.enCfaLayout: %d for raw\n",
                           IFDInfo->stIFD0Info.stRawFormat.enCfaLayout);
            return HI_ERR_DNG_INVALIDARG;
        }

        if (0 != IFDInfo->u32SubfileType % 2) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport pstIFDInfo->u32SubfileType: %d for raw\n",
                           IFDInfo->u32SubfileType);
            return HI_ERR_DNG_INVALIDARG;
        }

        if (COMPRESSION_NONE != IFDInfo->u16Compression) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "unsupport pstIFDInfo->u16Compressions: %d for raw\n",
                           IFDInfo->u16Compression);
            return HI_ERR_DNG_INVALIDARG;
        }
    }

    return HI_SUCCESS;
}

HI_S32 DNG_MUXER_SetIFD0(TIFF *tif, HI_DNG_MUXER_IFD_INFO_S *IFDInfo)
{
    HI_U64 exifOffset = 0;
    HI_U64 gpsOffset = 0;
    HI_U64 *subifdoffset = NULL;
    HI_S32 ret = HI_SUCCESS;
    if (IFDInfo->stIFD0Info.bHasExifIFD) {
        if (HI_SUCCESS != DNG_MUXER_SetExifIFD(tif, &IFDInfo->stIFD0Info.stExifInfo, &exifOffset)) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetIFD0 fail DNG_MUXER_SetExifIFD error\n");
            return HI_ERR_DNG_ADD_IFD;
        }
    }

    if (IFDInfo->stIFD0Info.bHasGpsIFD) {
        if (HI_SUCCESS != DNG_MUXER_SetGpsIFD(tif, &IFDInfo->stIFD0Info.stGpsInfo, &gpsOffset)) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetIFD0 fail DNG_MUXER_SetGpsIFD error\n");
            return HI_ERR_DNG_ADD_IFD;
        }
    }

    TIFFCreateDirectory(tif);

    DNG_MUXER_SetNoramlTag(tif, IFDInfo);

    if (0 != IFDInfo->stIFD0Info.u32SubIFDs) {
        subifdoffset = (HI_U64 *)malloc(IFDInfo->stIFD0Info.u32SubIFDs * sizeof(HI_U64));
        if (NULL == subifdoffset) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetIFD0 malloc subifdoffset error \n");
            return HI_ERR_DNG_MALLOC;
        }
        memset_s(subifdoffset, IFDInfo->stIFD0Info.u32SubIFDs * sizeof(HI_U64), 0x00,
                 IFDInfo->stIFD0Info.u32SubIFDs * sizeof(HI_U64));
        TIFFSetField(tif, TIFFTAG_SUBIFD, IFDInfo->stIFD0Info.u32SubIFDs, subifdoffset);
    }

    TIFFSetField(tif, TIFFTAG_MAKE, IFDInfo->stIFD0Info.aszMake);
    TIFFSetField(tif, TIFFTAG_MODEL, IFDInfo->stIFD0Info.aszModel);
    TIFFSetField(tif, TIFFTAG_SOFTWARE, IFDInfo->stIFD0Info.aszSoftware);
    TIFFSetField(tif, TIFFTAG_DATETIME, IFDInfo->stIFD0Info.aszDateTime);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, IFDInfo->stIFD0Info.u8Orientation);  //

    if (IFDInfo->stIFD0Info.u32XmpDataLen != 0 && IFDInfo->stIFD0Info.pu8XmpBuf != NULL) {
        TIFFSetField(tif, TIFFTAG_XMLPACKET, IFDInfo->stIFD0Info.u32XmpDataLen, IFDInfo->stIFD0Info.pu8XmpBuf);
    }

    if (0 != IFDInfo->stIFD0Info.u32DNGPrivDataLen && NULL != IFDInfo->stIFD0Info.pu8DNGPrivData) {
        TIFFSetField(tif, TIFFTAG_DNGPRIVATEDATA, IFDInfo->stIFD0Info.u32DNGPrivDataLen,
                     IFDInfo->stIFD0Info.pu8DNGPrivData);
    }

    if (0 != IFDInfo->stIFD0Info.u32XPCommentLen && NULL != IFDInfo->stIFD0Info.pu8XPComment) {
        TIFFSetField(tif, TIFFTAG_XPCOMMENT, IFDInfo->stIFD0Info.u32XPCommentLen, IFDInfo->stIFD0Info.pu8XPComment);
    }
    if (0 != IFDInfo->stIFD0Info.u32XPKeywordsLen && NULL != IFDInfo->stIFD0Info.pu8XPKeywords) {
        TIFFSetField(tif, TIFFTAG_XPKEYWORDS, IFDInfo->stIFD0Info.u32XPKeywordsLen,
                     IFDInfo->stIFD0Info.pu8XPKeywords);
    }

    static const HI_U32 profileHueSatMapDim[] = { 6, 6, 3 };
    TIFFSetField(tif, TIFFTAG_PROFILEHUESATMAPDIMS, profileHueSatMapDim);
    if (0 != IFDInfo->stIFD0Info.u32Map1Len && NULL != IFDInfo->stIFD0Info.pfProfileHueSatMapData1) {
        TIFFSetField(tif, TIFFTAG_PROFILEHUESATMAPDATA1, IFDInfo->stIFD0Info.u32Map1Len,
                     IFDInfo->stIFD0Info.pfProfileHueSatMapData1);
    }
    if (0 != IFDInfo->stIFD0Info.u32Map2Len && NULL != IFDInfo->stIFD0Info.pfProfileHueSatMapData2) {
        TIFFSetField(tif, TIFFTAG_PROFILEHUESATMAPDATA2, IFDInfo->stIFD0Info.u32Map2Len,
                     IFDInfo->stIFD0Info.pfProfileHueSatMapData2);
    }
    TIFFSetField(tif, TIFFTAG_PROFILEEMBEDPOLICY, 0);
    TIFFSetField(tif, TIFFTAG_LENSINFO, IFDInfo->stIFD0Info.afLensInfo);

    TIFFSetField(tif, TIFFTAG_ASSHOTNEUTRAL, DNG_MUXER_CFACOLORPLANE,
                 IFDInfo->stIFD0Info.stDynamicInfo.afAsShotNeutral);
    TIFFSetField(tif, TIFFTAG_COLORMATRIX1, DNG_MUXER_MATRIX_SIZE, IFDInfo->stIFD0Info.stStaticInfo.afColorMatrix1);
    TIFFSetField(tif, TIFFTAG_COLORMATRIX2, DNG_MUXER_MATRIX_SIZE, IFDInfo->stIFD0Info.stStaticInfo.afColorMatrix2);
    TIFFSetField(tif, TIFFTAG_CAMERACALIBRATION1, DNG_MUXER_MATRIX_SIZE,
                 IFDInfo->stIFD0Info.stStaticInfo.afCameraCalibration1);
    TIFFSetField(tif, TIFFTAG_CAMERACALIBRATION2, DNG_MUXER_MATRIX_SIZE,
                 IFDInfo->stIFD0Info.stStaticInfo.afCameraCalibration2);

    TIFFSetField(tif, TIFFTAG_CALIBRATIONILLUMINANT1, IFDInfo->stIFD0Info.stStaticInfo.u8CalibrationIlluminant1);
    TIFFSetField(tif, TIFFTAG_CALIBRATIONILLUMINANT2, IFDInfo->stIFD0Info.stStaticInfo.u8CalibrationIlluminant2);
    TIFFSetField(tif, TIFFTAG_FORWARDMATRIX1, IFDInfo->stIFD0Info.stStaticInfo.afForwadMatrix1);
    TIFFSetField(tif, TIFFTAG_FORWARDMATRIX2, IFDInfo->stIFD0Info.stStaticInfo.afForwadMatrix2);
    TIFFSetField(tif, TIFFTAG_NOISEPROFILE, DNG_MUXER_NP_SIZE, IFDInfo->stIFD0Info.stDynamicInfo.adNoiseProfile);

    // set inside
    TIFFSetField(tif, TIFFTAG_DNGVERSION, "\01\01\00\00");          //
    TIFFSetField(tif, TIFFTAG_DNGBACKWARDVERSION, "\01\00\00\00");  //
    TIFFSetField(tif, TIFFTAG_UNIQUECAMERAMODEL, IFDInfo->stIFD0Info.aszUniqueCameraModel);
    TIFFSetField(tif, TIFFTAG_CAMERASERIALNUMBER, IFDInfo->stIFD0Info.aszCameraSerialNumber);

    static const float analogBalance[DNG_MUXER_CFACOLORPLANE] = { 1.0, 1.0, 1.0 };
    TIFFSetField(tif, TIFFTAG_ANALOGBALANCE, DNG_MUXER_CFACOLORPLANE, analogBalance);

    static const float baselineExposure = 0.0;
    TIFFSetField(tif, TIFFTAG_BASELINEEXPOSURE, baselineExposure);
    static const float baselineNoise = 1.0;
    TIFFSetField(tif, TIFFTAG_BASELINENOISE, baselineNoise);

    static const float baselineSharpness = 1.0;
    TIFFSetField(tif, TIFFTAG_BASELINESHARPNESS, baselineSharpness);

    static const float linearResponseLimit = 1.0;
    TIFFSetField(tif, TIFFTAG_LINEARRESPONSELIMIT, linearResponseLimit);

    static const float shadowScale = 1.0;
    TIFFSetField(tif, TIFFTAG_SHADOWSCALE, shadowScale);

    static const HI_U32 defaultFinalSize[2] = { 0, 0 };
    TIFFSetField(tif, TIFFTAG_ORIGINALDEFAULTFINALSIZE, defaultFinalSize);

    static const HI_U32 defaultBestQualitySize[2] = { 0, 0 };
    TIFFSetField(tif, TIFFTAG_ORIGINALBESTQUALITYSIZE, defaultBestQualitySize);

    static const HI_FLOAT defaultCropSize[2] = { 0.0, 0.0 };
    TIFFSetField(tif, TIFFTAG_ORIGINALDEFAULTCROPSIZE, defaultCropSize);

    if (IFDInfo->stIFD0Info.bHasExifIFD) {
        TIFFSetField(tif, TIFFTAG_EXIFIFD, exifOffset);
    }
    if (IFDInfo->stIFD0Info.bHasGpsIFD) {
        TIFFSetField(tif, TIFFTAG_GPSIFD, gpsOffset);
    }

    if (0 == IFDInfo->stIFD0Info.u32SubIFDs) {  // means there is no subifd, store raw data in ifd0
        TIFFSetField(tif, TIFFTAG_BLACKLEVEL, 4, IFDInfo->stIFD0Info.stRawFormat.af32BlackLevel);

        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1);
        TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
        TIFFSetField(tif, TIFFTAG_WHITELEVEL, 1, &IFDInfo->stIFD0Info.stRawFormat.u32WhiteLevel);
        TIFFSetField(tif, TIFFTAG_BLACKLEVELREPEATDIM, IFDInfo->stIFD0Info.stRawFormat.au16BlcRepeatDim);

        TIFFSetField(tif, TIFFTAG_CFALAYOUT, IFDInfo->stIFD0Info.stRawFormat.enCfaLayout);
        TIFFSetField(tif, TIFFTAG_CFAPLANECOLOR, DNG_MUXER_CFACOLORPLANE,
                     IFDInfo->stIFD0Info.stRawFormat.au8CfaPlaneColor);
        TIFFSetField(tif, TIFFTAG_CFAREPEATPATTERNDIM, IFDInfo->stIFD0Info.stRawFormat.au16CfaRepeatPatternDim);
        TIFFSetField(tif, TIFFTAG_CFAPATTERN, IFDInfo->stIFD0Info.stRawFormat.au8CfaPattern);

        TIFFSetField(tif, TIFFTAG_DEFAULTSCALE, IFDInfo->stIFD0Info.stRawFormat.afDefaultScale);
        TIFFSetField(tif, TIFFTAG_BAYERGREENSPLIT, 0);
        TIFFSetField(tif, TIFFTAG_ANTIALIASSTRENGTH, 1.0);
        TIFFSetField(tif, TIFFTAG_BESTQUALITYSCALE, 1.0);

        static const HI_FLOAT defaultUserCrop[4] = { 0, 0, 1.0, 1.0 };
        TIFFSetField(tif, TIFFTAG_DEFAULTUSERCROP, defaultUserCrop);

        HI_U32 activeArea[4] = { 0, 0, 0, 0 };
        activeArea[2] = IFDInfo->u32Height;
        activeArea[3] = IFDInfo->u32Width;
        TIFFSetField(tif, TIFFTAG_ACTIVEAREA, activeArea);
    }

    if (1 != TIFFCheckpointDirectory(tif)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetIFD0 TIFFCheckpointDirectory fail\n");
        ret = HI_ERR_DNG_ADD_IFD;
        goto ERROR;
    }

    // write data
    if (COMPRESSION_JPEG == IFDInfo->u16Compression) {
        if (-1 == TIFFWriteRawStrip(tif, 0, IFDInfo->stImageData.pu8DataBuf, IFDInfo->stImageData.u32DataLen)) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetIFD0 TIFFWriteRawStrip fail\n");
            ret = HI_ERR_DNG_WRITE;
            goto ERROR;
        }
    } else if (COMPRESSION_NONE == IFDInfo->u16Compression) {
        if (HI_SUCCESS != DNG_MUXER_WriteRawData(tif, IFDInfo)) {
            DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetIFD0 DNG_MUXER_WriteRawData fail\n");
            ret = HI_ERR_DNG_WRITE;
            goto ERROR;
        }
    }

ERROR:
    if (NULL != subifdoffset) {
        free(subifdoffset);
        subifdoffset = NULL;
    }

    return ret;
}

HI_S32 DNG_MUXER_SetRawIFD(TIFF *tif, HI_DNG_MUXER_IFD_INFO_S *IFDInfo)
{
    DNG_MUXER_SetNoramlTag(tif, IFDInfo);

    TIFFSetField(tif, TIFFTAG_DEFAULTCROPORIGIN, IFDInfo->stRawIFDInfo.afDefaultCropOrigin);
    TIFFSetField(tif, TIFFTAG_DEFAULTCROPSIZE, IFDInfo->stRawIFDInfo.afDefaultCropSize);
    TIFFSetField(tif, TIFFTAG_BLACKLEVEL, DNG_MUXER_ISP_BAYER_CHN,
                 IFDInfo->stRawIFDInfo.stRawFormat.af32BlackLevel);

    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1);
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
    TIFFSetField(tif, TIFFTAG_WHITELEVEL, 1, &IFDInfo->stRawIFDInfo.stRawFormat.u32WhiteLevel);
    TIFFSetField(tif, TIFFTAG_BLACKLEVELREPEATDIM, IFDInfo->stRawIFDInfo.stRawFormat.au16BlcRepeatDim);

    TIFFSetField(tif, TIFFTAG_CFALAYOUT, IFDInfo->stRawIFDInfo.stRawFormat.enCfaLayout);
    TIFFSetField(tif, TIFFTAG_CFAPLANECOLOR, DNG_MUXER_CFACOLORPLANE,
                 IFDInfo->stRawIFDInfo.stRawFormat.au8CfaPlaneColor);
    TIFFSetField(tif, TIFFTAG_CFAREPEATPATTERNDIM, IFDInfo->stRawIFDInfo.stRawFormat.au16CfaRepeatPatternDim);
    TIFFSetField(tif, TIFFTAG_CFAPATTERN, IFDInfo->stRawIFDInfo.stRawFormat.au8CfaPattern);

    TIFFSetField(tif, TIFFTAG_DEFAULTSCALE, IFDInfo->stRawIFDInfo.stRawFormat.afDefaultScale);
    TIFFSetField(tif, TIFFTAG_BAYERGREENSPLIT, 0);
    TIFFSetField(tif, TIFFTAG_ANTIALIASSTRENGTH, 1.0);
    TIFFSetField(tif, TIFFTAG_BESTQUALITYSCALE, 1.0);

    static const HI_FLOAT defaultUserCrop[4] = { 0, 0, 1.0, 1.0 };
    TIFFSetField(tif, TIFFTAG_DEFAULTUSERCROP, defaultUserCrop);

    HI_U32 activeArea[4] = { 0, 0, 0, 0 };
    activeArea[2] = IFDInfo->u32Height;
    activeArea[3] = IFDInfo->u32Width;
    TIFFSetField(tif, TIFFTAG_ACTIVEAREA, activeArea);

    if (IFDInfo->stRawIFDInfo.u32OpcodeList1DataLen != 0 && IFDInfo->stRawIFDInfo.pu8OpcodeList1 != NULL) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetRawIFD OpcodeList1 len %d \n",
                       IFDInfo->stRawIFDInfo.u32OpcodeList1DataLen);
        TIFFSetField(tif, TIFFTAG_OPCODELIST1, IFDInfo->stRawIFDInfo.u32OpcodeList1DataLen,
                     IFDInfo->stRawIFDInfo.pu8OpcodeList1);
    }

    if (1 != TIFFCheckpointDirectory(tif)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetRawIFD TIFFCheckpointDirectory fail\n");
        return HI_ERR_DNG_ADD_IFD;
    }

    if (HI_SUCCESS != DNG_MUXER_WriteRawData(tif, IFDInfo)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetRawIFD DNG_MUXER_WriteRawData fail\n");
        return HI_ERR_DNG_WRITE;
    }

    return HI_SUCCESS;
}

HI_S32 DNG_MUXER_SetScreenNailIFD(TIFF *tif, HI_DNG_MUXER_IFD_INFO_S *IFDInfo)
{
    DNG_MUXER_SetNoramlTag(tif, IFDInfo);

    TIFFSetField(tif, TIFFTAG_REFERENCEBLACKWHITE, IFDInfo->stScreenIFDInfo.fReferenceBlackWhite);
    TIFFSetField(tif, TIFFTAG_YCBCRPOSITIONING, 1);

    static const HI_FLOAT afYCbCrCoefficients[DNG_MUXER_CFACOLORPLANE] = { 0.299, 0.587, 0.114 };
    TIFFSetField(tif, TIFFTAG_YCBCRCOEFFICIENTS, afYCbCrCoefficients);

    HI_U16 yCbCrSubSamplingH = 2;
    HI_U16 yCbCrSubSamplingV = 2;
    TIFFSetField(tif, TIFFTAG_YCBCRSUBSAMPLING, yCbCrSubSamplingH, yCbCrSubSamplingV);

    if (1 != TIFFCheckpointDirectory(tif)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetScreenNailIFD TIFFCheckpointDirectory fail\n");
        return HI_ERR_DNG_ADD_IFD;
    }

    // write data
    if (-1 == TIFFWriteRawStrip(tif, 0, IFDInfo->stImageData.pu8DataBuf, IFDInfo->stImageData.u32DataLen)) {
        DNG_LOG_Printf(MODULE_NAME_DNG, DNG_LOG_LEVEL_ERR, "DNG_MUXER_SetScreenNailIFD TIFFWriteRawStrip fail\n");
        return HI_ERR_DNG_WRITE;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
