/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_comm.c
 * @brief   sample comm function
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */

#include "sample_cfg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


const HI_MAPI_SENSOR_ATTR_S g_stSensor1080PMode0 = {
    .u32SnsMode = 0,
    .enWdrMode = WDR_MODE_NONE,
    .stSize = {
        .u32Width = 1920,
        .u32Height = 1080,
    }
};

const HI_MAPI_SENSOR_ATTR_S g_stSensor4MMode0 = {
    .u32SnsMode = 0,
    .enWdrMode = WDR_MODE_NONE,
    .stSize = {
        .u32Width = 2592,
        .u32Height = 1536,
    }
};

const HI_MAPI_SENSOR_ATTR_S g_stSensor4KMode1 = {
    .u32SnsMode = 1,
    .enWdrMode = WDR_MODE_NONE,
    .stSize = {
        .u32Width = 3840,
        .u32Height = 2160,
    }
};

const HI_MAPI_SENSOR_ATTR_S g_stSensor4KMode0 = {
    .u32SnsMode = 0,
    .enWdrMode = WDR_MODE_NONE,
    .stSize = {
        .u32Width = 3840,
        .u32Height = 2160,
    }
};

const HI_MAPI_SENSOR_ATTR_S g_stSensor12MMode1 = {
    .u32SnsMode = 1,
    .enWdrMode = WDR_MODE_NONE,
    .stSize = {
        .u32Width = 4000,
        .u32Height = 3000,
    }
};

const HI_MAPI_SENSOR_ATTR_S g_stSensor12MMode0 = {
    .u32SnsMode = 0,
    .enWdrMode = WDR_MODE_NONE,
    .stSize = {
        .u32Width = 4000,
        .u32Height = 3000,
    }
};

const HI_MAPI_VCAP_PIPE_ATTR_S g_stPipe1080P30Fps = {
    .stFrameRate = {
        .s32SrcFrameRate = -1,
        .s32DstFrameRate = -1,
    },
    .enCompressMode = COMPRESS_MODE_NONE,
    .bIspBypass = HI_FALSE,
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .stIspPubAttr = {
        .f32FrameRate = 30.0f,
        .stSize = {
            .u32Width = 1920,
            .u32Height = 1080,
        },
        .enBayer = BAYER_RGGB,
    }
};

const HI_MAPI_VCAP_PIPE_ATTR_S g_stPipe4M30Fps = {
    .stFrameRate = {
        .s32SrcFrameRate = -1,
        .s32DstFrameRate = -1,
    },
    .enCompressMode = COMPRESS_MODE_NONE,
    .bIspBypass = HI_FALSE,
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .stIspPubAttr = {
        .f32FrameRate = 30.0f,
        .stSize = {
            .u32Width = 2592,
            .u32Height = 1520,
        },
        .enBayer = BAYER_RGGB,
    }
};

const HI_MAPI_VCAP_PIPE_ATTR_S g_stPipe4K30Fps = {
    .stFrameRate = {
        .s32SrcFrameRate = -1,
        .s32DstFrameRate = -1,
    },
    .enCompressMode = COMPRESS_MODE_NONE,
    .bIspBypass = HI_FALSE,
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .stIspPubAttr = {
        .f32FrameRate = SAMPLE_4K30FPS_ISPPUB_MAX_FRAMERATE,
        .stSize = {
            .u32Width = 3840,
            .u32Height = 2160,
        },
        .enBayer = BAYER_RGGB,
    }
};

const HI_MAPI_VCAP_PIPE_ATTR_S g_stPipe12M30Fps = {
    .stFrameRate = {
        .s32SrcFrameRate = -1,
        .s32DstFrameRate = -1,
    },
    .enCompressMode = COMPRESS_MODE_NONE,
    .bIspBypass = HI_FALSE,
    .enPipeBypassMode = VI_PIPE_BYPASS_NONE,
    .stIspPubAttr = {
        .f32FrameRate = 20.0f,
        .stSize = {
            .u32Width = 4000,
            .u32Height = 3000,
        },
        .enBayer = BAYER_RGGB,
    }
};

const HI_MAPI_VCAP_DEV_ATTR_S g_stDev1080P = {
    .stBasSize = {
        .u32Width = 1920,
        .u32Height = 1080,
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
    }
};

const HI_MAPI_VCAP_DEV_ATTR_S g_stDev4M = {
    .stBasSize = {
        .u32Width = 2592,
        .u32Height = 1520,
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
    }

};

const HI_MAPI_VCAP_DEV_ATTR_S g_stDev4K = {
    .stBasSize = {
        .u32Width = 3840,
        .u32Height = 2160,
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
    },

};

const HI_MAPI_VCAP_DEV_ATTR_S g_stDev12M = {
    .stBasSize = {
        .u32Width = 4000,
        .u32Height = 3000,
    },
    .stWDRAttr = {
        .enWDRMode = WDR_MODE_NONE,
    }
};

const HI_MAPI_PIPE_CHN_ATTR_S g_stChn1080P30Fps = {
    .stDestSize = {
        .u32Width = 1920,
        .u32Height = 1080,
    },
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enCompressMode = COMPRESS_MODE_NONE,
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
};

const HI_MAPI_PIPE_CHN_ATTR_S g_stChn4M30Fps = {
    .stDestSize = {
        .u32Width = 2592,
        .u32Height = 1520,
    },
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enCompressMode = COMPRESS_MODE_NONE,
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
};

const HI_MAPI_PIPE_CHN_ATTR_S g_stChn4K30Fps = {
    .stDestSize = {
        .u32Width = 3840,
        .u32Height = 2160,
    },
    .stFrameRate = {
        .s32SrcFrameRate = 29,
        .s32DstFrameRate = 29,
    },
    .enCompressMode = COMPRESS_MODE_NONE,
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
};

const HI_MAPI_PIPE_CHN_ATTR_S g_stChn12M30Fps = {
    .stDestSize = {
        .u32Width = 4000,
        .u32Height = 3000,
    },
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enCompressMode = COMPRESS_MODE_NONE,
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
};

const HI_MAPI_VPSS_ATTR_S g_stVpss1080P30Fps = {
    .u32MaxW = 1920,
    .u32MaxH = 1080,
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .bNrEn = HI_TRUE,
    .stNrAttr = {
        .enCompressMode = COMPRESS_MODE_NONE,
        .enNrMotionMode = NR_MOTION_MODE_NORMAL,
    },
};

const HI_MAPI_VPSS_ATTR_S g_stVpss4M30Fps = {
    .u32MaxW = 2592,
    .u32MaxH = 1520,
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .bNrEn = HI_TRUE,
    .stNrAttr = {
        .enCompressMode = COMPRESS_MODE_NONE,
        .enNrMotionMode = NR_MOTION_MODE_NORMAL,
    },
};

const HI_MAPI_VPSS_ATTR_S g_stVpss4K30Fps = {
    .u32MaxW = 3840,
    .u32MaxH = 2160,
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .bNrEn = HI_TRUE,
    .stNrAttr = {
        .enCompressMode = COMPRESS_MODE_NONE,
        .enNrMotionMode = NR_MOTION_MODE_NORMAL,
    },
};

const HI_MAPI_VPSS_ATTR_S g_stVpss12M30Fps = {
    .u32MaxW = 4000,
    .u32MaxH = 3000,
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .bNrEn = HI_TRUE,
    .stNrAttr = {
        .enCompressMode = COMPRESS_MODE_NONE,
        .enNrMotionMode = NR_MOTION_MODE_NORMAL,
    },
};

const HI_MAPI_VPORT_ATTR_S g_stVport320x180P30Fps = {
    .u32Width = 320,
    .u32Height = 180,
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
};

const HI_MAPI_VPORT_ATTR_S g_stVport640x480P30Fps = {
    .u32Width = 640,
    .u32Height = 480,
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
};

const HI_MAPI_VPORT_ATTR_S g_stVport1080P30Fps = {
    .u32Width = 1920,
    .u32Height = 1080,
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
};

const HI_MAPI_VPORT_ATTR_S g_stVport4M30Fps = {
    .u32Width = 2592,
    .u32Height = 1520,
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
};

const HI_MAPI_VPORT_ATTR_S g_stVport4K30Fps = {
    .u32Width = 3840,
    .u32Height = 2160,
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
};

const HI_MAPI_VPORT_ATTR_S g_stVport12M30Fps = {
    .u32Width = 4000,
    .u32Height = 3000,
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .enVideoFormat = VIDEO_FORMAT_LINEAR,
    .enCompressMode = COMPRESS_MODE_NONE,
};

const HI_MAPI_VPORT_ATTR_S g_stVport1080P30Fps16Bpp = {
    .u32Width = 1920,
    .u32Height = 1080,
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .enVideoFormat = VIDEO_FORMAT_TILE_16x8,
    .enCompressMode = COMPRESS_MODE_NONE,
};

const HI_MAPI_VPORT_ATTR_S g_stVport4K30Fps16Bpp = {
    .u32Width = 3840,
    .u32Height = 2160,
    .stFrameRate = {
        .s32SrcFrameRate = 30,
        .s32DstFrameRate = 30,
    },
    .enPixelFormat = PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .enVideoFormat = VIDEO_FORMAT_TILE_16x8,
    .enCompressMode = COMPRESS_MODE_NONE,
};

const HI_MAPI_VENC_ATTR_S g_stVenc320x180Jpeg = {
    .stVencPloadTypeAttr = {
        .enType = HI_MAPI_PAYLOAD_TYPE_JPEG,
        .u32Width = 320,
        .u32Height = 180,
        .u32BufSize = 320 * 180 * 3 / 2,
        .u32Profile = 0,
        .stAttrJpege = {
            .bEnableDCF = HI_TRUE,
            .u32Qfactor = 10,
            .enJpegEncodeMode = JPEG_ENCODE_SNAP,
            .stAttrMPF = {
                .u8LargeThumbNailNum = 0,
            },
        },
    },
};

const HI_MAPI_VENC_ATTR_S g_stVenc1080P30FpsJpeg = {
    .stVencPloadTypeAttr = {
        .enType = HI_MAPI_PAYLOAD_TYPE_JPEG,
        .u32Width = 1920,
        .u32Height = 1080,
        .u32BufSize = 1920 * 1080 * 3 / 2,
        .stAttrJpege = {
            .bEnableDCF = HI_TRUE,
            .u32Qfactor = 90,
            .enJpegEncodeMode = JPEG_ENCODE_SNAP,
            .stAttrMPF = {
                .u8LargeThumbNailNum = 0,
            },
        },
    },
};


const HI_MAPI_VENC_ATTR_S g_stVenc4K30FpsJpeg = {
    .stVencPloadTypeAttr = {
        .enType = HI_MAPI_PAYLOAD_TYPE_JPEG,
        .u32Width = 3840,
        .u32Height = 2160,
        .u32BufSize = 3840 * 2160 * 2,
        .u32Profile = 0,
        .stAttrJpege = {
            .bEnableDCF = HI_TRUE,
            .u32Qfactor = 90,
            .enJpegEncodeMode = JPEG_ENCODE_SNAP,
            .stAttrMPF = {
                .u8LargeThumbNailNum = 0,
            },
        },
    },
};

const HI_MAPI_VENC_ATTR_S g_stVenc720P30FpsH264 = {
    .stVencPloadTypeAttr = {
        .enType = HI_MAPI_PAYLOAD_TYPE_H264,
        .u32Width = 1280,
        .u32Height = 720,
        .u32BufSize = 1280 * 720 * 2,
        .u32Profile = 0,
        .enSceneMode = HI_MAPI_VENC_SCENE_MODE_DV,
    },
    .stRcAttr = {
        .stAttrCbr = {
            .u32Gop = 30,
            .u32StatTime = 2,
            .u32SrcFrameRate = 30,
            .fr32DstFrameRate = 30,
            .u32BitRate = 4096,
        }
    }
};

const HI_MAPI_VENC_ATTR_S g_stVenc1080P30FpsVideoH264 = {
    .stVencPloadTypeAttr = {
        .enType = HI_MAPI_PAYLOAD_TYPE_H264,
        .u32Width = 1920,
        .u32Height = 1080,
        .u32BufSize = 1920 * 1080 * 3 / 2,
        .u32Profile = 0,
        .enSceneMode = HI_MAPI_VENC_SCENE_MODE_DV,
    },
    .stRcAttr = {
        .enRcMode = HI_MAPI_VENC_RC_MODE_CBR,
            .stAttrCbr = {
            .u32Gop = 30,
            .u32StatTime = 2,
            .u32SrcFrameRate = 30,
            .fr32DstFrameRate = 30,
            .u32BitRate = 4096,
        }
    }
};

const HI_MAPI_VENC_ATTR_S g_stVenc4K30FpsVideoH264 = {
    .stVencPloadTypeAttr = {
        .enType = HI_MAPI_PAYLOAD_TYPE_H264,
        .u32Width = 3840,
        .u32Height = 2160,
        .u32BufSize = 3840 * 2160 * 2,
        .u32Profile = 0,
        .enSceneMode = HI_MAPI_VENC_SCENE_MODE_DV,
    },
    .stRcAttr = {
        .stAttrCbr = {
            .u32Gop = 30,
            .u32StatTime = 2,
            .u32SrcFrameRate = 30,
            .fr32DstFrameRate = 30,
            .u32BitRate = 10000,
        }
    }
};

const HI_MAPI_VENC_ATTR_S g_stVenc12M15FpsVideoH264 = {
    .stVencPloadTypeAttr = {
        .enType = HI_MAPI_PAYLOAD_TYPE_H264,
        .u32Width = 4000,
        .u32Height = 3000,
        .u32BufSize = 4000 * 3000 * 2,
        .u32Profile = 1,
        .enSceneMode = HI_MAPI_VENC_SCENE_MODE_NORMAL,
    },
    .stRcAttr = {
        .stAttrCbr = {
            .u32Gop = 15,
            .u32StatTime = 2,
            .u32SrcFrameRate = 15,
            .fr32DstFrameRate = 15,
            .u32BitRate = 10000,
        }
    }
};

const HI_MAPI_VENC_ATTR_S g_stVenc640x480P30FpsVideoH265 = {
    .stVencPloadTypeAttr = {
        .enType = HI_MAPI_PAYLOAD_TYPE_H265,
        .u32Width = 640,
        .u32Height = 480,
        .u32BufSize = 640 * 480 * 3 / 2,
        .u32Profile = 0,
        .enSceneMode = HI_MAPI_VENC_SCENE_MODE_DV,
    },
    .stRcAttr = {
        .stAttrCbr = {
            .u32Gop = 30,
            .u32StatTime = 2,
            .u32SrcFrameRate = 30,
            .fr32DstFrameRate = 30,
            .u32BitRate = 4096,
        }
    }
};

const HI_MAPI_VENC_ATTR_S g_stVenc1080P30FpsVideoH265 = {
    .stVencPloadTypeAttr = {
        .enType = HI_MAPI_PAYLOAD_TYPE_H265,
        .u32Width = 1920,
        .u32Height = 1080,
        .u32BufSize = 1920 * 1080 * 3 / 2,
        .u32Profile = 0,
        .enSceneMode = HI_MAPI_VENC_SCENE_MODE_DV,
    },
    .stRcAttr = {
        .stAttrCbr = {
            .u32Gop = 30,
            .u32StatTime = 2,
            .u32SrcFrameRate = 30,
            .fr32DstFrameRate = 30,
            .u32BitRate = 4000,
        }
    }
};

const HI_MAPI_VENC_ATTR_S g_stVenc4K30FpsVideoH265 = {
    .stVencPloadTypeAttr = {
        .enType = HI_MAPI_PAYLOAD_TYPE_H265,
        .u32Width = 3840,
        .u32Height = 2160,
        .u32BufSize = 3840 * 2160 * 2,
        .u32Profile = 0,
        .enSceneMode = HI_MAPI_VENC_SCENE_MODE_DV,
    },
    .stRcAttr = {
        .stAttrCbr = {
            .u32Gop = 30,
            .u32StatTime = 2,
            .u32SrcFrameRate = 30,
            .fr32DstFrameRate = 30,
            .u32BitRate = 10000,
        }
    }
};


const VO_PUB_ATTR_S g_stDispHdmi1080P30 = {
    .u32BgColor = 0xFF,
    .enIntfType = VO_INTF_HDMI,
    .enIntfSync = VO_OUTPUT_1080P30,
    .stSyncInfo = {
        .bSynm = 0,
        .bIop = 0,
        .u8Intfb = 0,

        .u16Vact = 0,
        .u16Vbb = 0,
        .u16Vfb = 0,

        .u16Hact = 0,
        .u16Hbb = 0,
        .u16Hfb = 0,
        .u16Hmid = 0,

        .u16Bvact = 0,
        .u16Bvbb = 0,
        .u16Bvfb = 0,

        .u16Hpw = 0,
        .u16Vpw = 0,

        .bIdv = 0,
        .bIhs = 0,
        .bIvs = 0,
    },
};

const HI_MAPI_DISP_WINDOW_ATTR_S g_stWnd1920x1080 = {
    .u32Priority = 0,
    .stRect = {
        .s32X = 0,
        .s32Y = 0,
        .u32Width = 1920,
        .u32Height = 1080,

    },

};


HI_S32 SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_SEQ_E enSensorSeq,
    HI_MAPI_SENSOR_ATTR_S *pstSensorAttr)
{
    HI_S32 s32Ret;

    if (pstSensorAttr == NULL) {
        printf("Sample Get Sensor Cfg is null pointer.\n");
        return HI_FAILURE;
    }

    if (CFG_SENSOR_TYPE0 == IMX377) {
        if (enSensorSeq == SAMPLE_SENSOR_4K30) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor4KMode1,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
            pstSensorAttr->stInputDevAttr.inputDataType = DATA_TYPE_RAW_10BIT;
        } else if (enSensorSeq == SAMPLE_SENSOR_12M) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor12MMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
            pstSensorAttr->stInputDevAttr.inputDataType = DATA_TYPE_RAW_12BIT;
        } else if (enSensorSeq == SAMPLE_SENSOR_1080P30) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor1080PMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
            pstSensorAttr->stInputDevAttr.inputDataType = DATA_TYPE_RAW_12BIT;
        } else {
            printf("other sensor seq to be add later\n");
        }
        pstSensorAttr->stInputDevAttr.inputMode = INPUT_MODE_MIPI;
    } else if (CFG_SENSOR_TYPE0 == IMX477) {
        if (enSensorSeq == SAMPLE_SENSOR_4K30) {
             SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor4KMode1,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
            pstSensorAttr->stInputDevAttr.inputDataType = DATA_TYPE_RAW_12BIT;
        } else if (enSensorSeq == SAMPLE_SENSOR_12M) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor12MMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
            pstSensorAttr->stInputDevAttr.inputDataType = DATA_TYPE_RAW_12BIT;
        } else if (enSensorSeq == SAMPLE_SENSOR_1080P30) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor1080PMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
            pstSensorAttr->stInputDevAttr.inputDataType = DATA_TYPE_RAW_10BIT;
        } else {
            printf("other sensor seq to be add later\n");
        }
        pstSensorAttr->stInputDevAttr.inputMode = INPUT_MODE_MIPI;
    } else if (CFG_SENSOR_TYPE0 == IMX458) {
        if (enSensorSeq == SAMPLE_SENSOR_4K30) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor4KMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
        } else if (enSensorSeq == SAMPLE_SENSOR_12M) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor12MMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
        } else if (enSensorSeq == SAMPLE_SENSOR_1080P30) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor1080PMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
        } else {
            printf("other sensor seq to be add later\n");
        }

        pstSensorAttr->stInputDevAttr.inputMode = INPUT_MODE_MIPI;
        pstSensorAttr->stInputDevAttr.inputDataType = DATA_TYPE_RAW_10BIT;
    } else if (CFG_SENSOR_TYPE0 == IMX307) {
        if (enSensorSeq == SAMPLE_SENSOR_1080P30) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor1080PMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
        } else {
            printf("other sensor seq to be add later\n");
        }
        pstSensorAttr->stInputDevAttr.inputMode = INPUT_MODE_MIPI;
        pstSensorAttr->stInputDevAttr.inputDataType = DATA_TYPE_RAW_12BIT;
    } else if (CFG_SENSOR_TYPE0 == GC2053) {
        if (enSensorSeq == SAMPLE_SENSOR_1080P30) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor1080PMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
        } else {
            printf("other sensor seq to be add later\n");
        }
        pstSensorAttr->stInputDevAttr.inputMode = INPUT_MODE_MIPI;
        pstSensorAttr->stInputDevAttr.inputDataType = DATA_TYPE_RAW_10BIT;
    } else if (CFG_SENSOR_TYPE0 == IMX123) {
        if (enSensorSeq == SAMPLE_SENSOR_4M30) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor4MMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
        } else {
            printf("other sensor seq to be add later\n");
        }
        pstSensorAttr->stInputDevAttr.inputMode = INPUT_MODE_MIPI;
        pstSensorAttr->stInputDevAttr.inputDataType = DATA_TYPE_RAW_12BIT;
    } else if (CFG_SENSOR_TYPE0 == OS05A) {
        if (enSensorSeq == SAMPLE_SENSOR_4M30) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor4MMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
        } else {
            printf("other sensor seq to be add later\n");
        }
        pstSensorAttr->stInputDevAttr.inputMode = INPUT_MODE_MIPI;
        pstSensorAttr->stInputDevAttr.inputDataType = DATA_TYPE_RAW_12BIT;
    } else {
        printf("this sensor is not adapt yet in sample, please adapt it !!!");
        if (enSensorSeq == SAMPLE_SENSOR_4K30) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor4KMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
        } else if (enSensorSeq == SAMPLE_SENSOR_12M) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor12MMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
        } else if (enSensorSeq == SAMPLE_SENSOR_1080P30) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor1080PMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
        } else if (enSensorSeq == SAMPLE_SENSOR_4M30) {
            SAMPLE_CHECK_RET(memcpy_s(pstSensorAttr, sizeof(HI_MAPI_SENSOR_ATTR_S), &g_stSensor4MMode0,
                sizeof(HI_MAPI_SENSOR_ATTR_S)));
        } else {
            printf("other sensor seq to be add later\n");
        }

        pstSensorAttr->stInputDevAttr.inputMode = INPUT_MODE_MIPI;
        pstSensorAttr->stInputDevAttr.inputDataType = DATA_TYPE_RAW_10BIT;
    }
    return HI_SUCCESS;
}

HI_S32 MAPI_SAMPLE_COMM_GetMediaCFG(SAMPLE_SENSOR_SEQ_E enSensorSeq,
    SAMPLE_MODE_COMM_CONFIG_S *pstCommConfig)
{
    if (pstCommConfig == NULL) {
        printf("Input is null!\n");
        return HI_FAILURE;
    }

    if (enSensorSeq == SAMPLE_SENSOR_4K30) {
        pstCommConfig->pstVcapDevAttr = &g_stDev4K;
        pstCommConfig->pstVcapPipeAttr = &g_stPipe4K30Fps;
        pstCommConfig->pstPipeChnAttr = &g_stChn4K30Fps;
        pstCommConfig->pstVpssAttr = &g_stVpss4K30Fps;
        pstCommConfig->pstVPortAttr = &g_stVport4K30Fps;
        pstCommConfig->pstRecVencAttr = &g_stVenc4K30FpsVideoH264;
        pstCommConfig->pstSnapVencAttr = &g_stVenc4K30FpsJpeg;
    }

    else if (enSensorSeq == SAMPLE_SENSOR_4M30) {
        pstCommConfig->pstVcapDevAttr = &g_stDev4M;
        pstCommConfig->pstVcapPipeAttr = &g_stPipe4M30Fps;
        pstCommConfig->pstPipeChnAttr = &g_stChn4M30Fps;
        pstCommConfig->pstVpssAttr = &g_stVpss4M30Fps;
        pstCommConfig->pstVPortAttr = &g_stVport4M30Fps;
        pstCommConfig->pstRecVencAttr = &g_stVenc1080P30FpsVideoH265;
        pstCommConfig->pstSnapVencAttr = &g_stVenc1080P30FpsJpeg;
    }

    else if (enSensorSeq == SAMPLE_SENSOR_1080P30) {
        pstCommConfig->pstVcapDevAttr = &g_stDev1080P;
        pstCommConfig->pstVcapPipeAttr = &g_stPipe1080P30Fps;
        pstCommConfig->pstPipeChnAttr = &g_stChn1080P30Fps;
        pstCommConfig->pstVpssAttr = &g_stVpss1080P30Fps;
        pstCommConfig->pstVPortAttr = &g_stVport1080P30Fps;
        pstCommConfig->pstRecVencAttr = &g_stVenc1080P30FpsVideoH265;
        pstCommConfig->pstSnapVencAttr = &g_stVenc1080P30FpsJpeg;
    } else {
        pstCommConfig->pstVcapDevAttr = &g_stDev1080P;
        pstCommConfig->pstVcapPipeAttr = &g_stPipe1080P30Fps;
        pstCommConfig->pstPipeChnAttr = &g_stChn1080P30Fps;
        pstCommConfig->pstVpssAttr = &g_stVpss1080P30Fps;
        pstCommConfig->pstVPortAttr = &g_stVport1080P30Fps;
        pstCommConfig->pstRecVencAttr = &g_stVenc1080P30FpsVideoH265;
        pstCommConfig->pstSnapVencAttr = &g_stVenc1080P30FpsJpeg;
        printf("other MediaCFG to be add later, default use  1080P30 Mode\n");
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_VENC_GetMediaMode(SAMPLE_SENSOR_SEQ_E *penSensorSeq,
    HI_MAPI_SENSOR_ATTR_S *pstVcapSensorAttr)
{
    char ch;
    HI_S32 s32Ret = HI_SUCCESS;

    // IMX307 only support 1080P
    if (CFG_SENSOR_TYPE0 == IMX307) {
        ch = '1';
    } else if ((CFG_SENSOR_TYPE0 == OS05A) || (CFG_SENSOR_TYPE0 == IMX123)) {
    // IMX123 OS05A only support 4M
        ch = '2';
    } else {
        printf("Usage : \n");
        printf("\t 0) media cfg :4K@30fps.\n");
        printf("\t 1) media cfg :1080p@30fps.\n");
        ch = (char)getchar();
        (void)getchar();
    }

    switch (ch) {
        case '0':
            *penSensorSeq = SAMPLE_SENSOR_4K30;
            s32Ret = SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_4K30, pstVcapSensorAttr);
            if (s32Ret != HI_SUCCESS) {
                printf("SAMPLE_COMM_GetSensorCfg error!\n");
                return HI_FAILURE;
            }
            break;

        case '1':
            *penSensorSeq = SAMPLE_SENSOR_1080P30;
            s32Ret = SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_1080P30, pstVcapSensorAttr);
            if (s32Ret != HI_SUCCESS) {
                printf("SAMPLE_COMM_GetSensorCfg error!\n");
                return HI_FAILURE;
            }
            break;

        case '2':
            *penSensorSeq = SAMPLE_SENSOR_4M30;
            s32Ret = SAMPLE_COMM_GetSensorCfg(SAMPLE_SENSOR_4M30, pstVcapSensorAttr);
            if (s32Ret != HI_SUCCESS) {
                printf("SAMPLE_COMM_GetSensorCfg error!\n");
                return HI_FAILURE;
            }
            break;

        default:
            printf("the index is invaild!\n");
            printf("Usage : \n");
            printf("\t 0) media cfg :4K@30fps + 1080p HDMI Preview.\n");
            printf("\t 1) media cfg :1080p@30fps + 1080p HDMI Preview.\n");
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
