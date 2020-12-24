/**
* Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
*
* @file    register_sensor.c
* @brief   register sensor functions
* @author  HiMobileCam NDK develop team
* @date    2018-2-6
*/
#include <stdio.h>
#include <sensor_comm.h>
#include <sensor_interface_cfg_params.h>
#include "linux_list.h"
#include "mapi_vcap_inner.h"
#include "mapi_comm_inner.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static MAPI_LIST_HEAD(s_stSensorCfgHead);

/* Register the sensor configuration, the sensor structure will be placed in the list */
HI_S32 MAPI_SENSOR_Register(HI_MAPI_COMBO_DEV_ATTR_S *pstSensorCfg)
{
    cbb_list_add_tail(&pstSensorCfg->list, &s_stSensorCfgHead);

    return HI_SUCCESS;
}

HI_S32 MAPI_SENSOR_GetConfig(HI_S32 s32SensorType, HI_MAPI_COMBO_DEV_ATTR_S *pstSensorCfg)
{
    HI_MAPI_COMBO_DEV_ATTR_S *pstTmp;

    cbb_list_for_each_entry(pstTmp, &s_stSensorCfgHead, list)
    {
        if (pstTmp->s32SensorType == s32SensorType) {
            *pstSensorCfg = *pstTmp;
            return HI_SUCCESS;
        }
    }

    MAPI_ERR_TRACE(HI_MAPI_MOD_VCAP, "do not have this sensor type:%d !\n", s32SensorType);
    return HI_FAILURE;
}

HI_S32 MAPI_SENSOR_Load(HI_VOID)
{
    HI_S32 s32Ret = HI_FAILURE;

    MAPI_INFO_TRACE(HI_MAPI_MOD_VCAP, "sensor param load begin...\n");

#ifdef SENSOR_IMX123
    s32Ret = SensorImx123Init();
    CHECK_MAPI_VCAP_RET(s32Ret, "SensorImx123 param load fail!!!");
    MAPI_INFO_TRACE(HI_MAPI_MOD_VCAP, "SensorImx123 param load success!\n");
#endif


#ifdef SENSOR_AHD
    s32Ret = SensorAHDInit();
    CHECK_MAPI_VCAP_RET(s32Ret, "AHD param load fail!!!");
    MAPI_INFO_TRACE(HI_MAPI_MOD_VCAP, "AHD param load success!\n");
#endif

    if (s32Ret == HI_FAILURE)
    {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VCAP, "The Sensor type is not support!\n");
    }

    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
