/**
* Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
*
* @file    sensor_interface_cfg_params.c
* @brief   sensor dev interface config params:
* one type interface config params must be choiced at global var "user_dev_attr"
* @author  HiMobileCam NDK develop team
* @date    2018-2-7
*/

#include "mpi_vi.h"
#include "mpi_isp.h"
#include "hi_sns_ctrl.h"
#include "sensor_comm.h"
#include "sensor_interface_cfg_params.h"
#include "../include/sensor_interface_cfg_adapt.h"

static const HI_MAPI_MIPI_INTF_S g_stSensorMipiIntfCfg[] = {
    /* config 5M@30fps */
    {
        .img_rect = {
            .x = 0,
            .y = 0,
            .width = 2048,
            .height = 1536
        },
        .stSnsSize = {
            .u32Width = 2048,
            .u32Height = 1536
        },
        .mipi_attr =
        {
            .input_data_type = DATA_TYPE_RAW_12BIT,
            .wdr_mode = HI_MIPI_WDR_MODE_NONE,
        },
    },

    /* config 4M@30fps */
    {
        .img_rect = {
            .x = 0,
            .y = 0,
            .width = 1920,
            .height = 1080
        },
        .stSnsSize = {
            .u32Width = 1920,
            .u32Height = 1080
        },
        .mipi_attr = {
            .input_data_type = DATA_TYPE_RAW_12BIT,
            .wdr_mode = HI_MIPI_WDR_MODE_NONE,
        },
    },
};

/* width and height must align with 4 bytes */
static const HI_MAPI_SENSOR_MODE_S g_stSensorMode[] = {
    /* SENSOR_MODE_5M_30FPS  frameRate:[1.0-30] */
    {
        .s32Width = 2048,
        .s32Height = 1536,
        .u32SnsMode = 0,
        .enWdrMode = WDR_MODE_NONE,
        .f32SnsMaxFrameRate = 30.0f
    },

    /* SENSOR_MODE_4M_30FPS  frameRate:[1.0-30] */
    {
        .s32Width = 1920,
        .s32Height = 1080,
        .u32SnsMode = 0,
        .enWdrMode = WDR_MODE_NONE,
        .f32SnsMaxFrameRate = 30.0f
    },

};

/* final interface here */
static HI_MAPI_COMBO_DEV_ATTR_S g_stSensorCfg = {
    /* config SENSOR */
    .pSensorObj = &stSnsImx123Obj,
    .s32SensorType = IMX123,
    .input_mode = INPUT_MODE_MIPI,
    .data_rate = MIPI_DATA_RATE_X1,
    .sensor_i2c_addr = 0x34,
    .stSensorInputAttr = {
        .enBayerFormat = BAYER_RGGB,
        .enSensorCommBusType = HI_SENSOR_COMMBUS_TYPE_I2C,
    },

    .stSensorMode = {
        .s32SensorModeCnt = sizeof(g_stSensorMode) / sizeof(HI_MAPI_SENSOR_MODE_S),
        .pSensorMode = g_stSensorMode,
        .pstIntf = g_stSensorMipiIntfCfg
    },

    .aLaneId = {
        /* mipi_dev0 */
        {
            { 0, 1, 2, 3 }, /* 5M@30FPS */
            { 0, 1, 2, 3 }, /* 4M@30FPS */
        },

        /* No mipi_dev1 */
        {
            {}, {}, {}, {}, {}, {},

        }
    },
};

HI_U32 SensorImx123Init(void)
{
    return MAPI_SENSOR_Register((HI_MAPI_COMBO_DEV_ATTR_S *)&g_stSensorCfg);
}
