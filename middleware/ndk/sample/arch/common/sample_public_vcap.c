/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_public_venc.c
 * @brief   sample public venc module
 * @author  HiMobileCam NDK develop team
 * @date  2019-5-16
 */

#include "sample_cfg.h"
#include "sample_public_venc.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

HI_S32 SAMPLE_COMM_VCAP_SnapTrigger(HI_S32 VCapPipeHdl, HI_S32 VEncHdl, HI_S32 s32Framecnt)
{
    HI_S32 s32Ret;

    s32Ret = HI_MAPI_VENC_Stop(VEncHdl);

    if (s32Ret != HI_SUCCESS) {
        printf("HI_MAPI_VEnc_Stop fail s32Ret:%d\n", s32Ret);
        return s32Ret;
    }

    s32Ret = HI_MAPI_VENC_Start(VEncHdl, s32Framecnt);

    if (s32Ret != HI_SUCCESS) {
        printf("HI_MAPI_VEnc_Start fail s32Ret:%d\n", s32Ret);
        return s32Ret;
    }

    s32Ret = HI_MAPI_VCAP_StartTrigger(VCapPipeHdl);

    if (s32Ret != HI_SUCCESS) {
        printf("HI_MAPI_VCap_Trigger fail s32Ret:%d\n", s32Ret);
        return s32Ret;
    }

    sleep(1);
    s32Ret = HI_MAPI_VCAP_StopTrigger(VCapPipeHdl);

    if (s32Ret != HI_SUCCESS) {
        printf("HI_MAPI_VCap_Trigger fail s32Ret:%d\n", s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_VOID SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(HI_MAPI_SENSOR_ATTR_S *pstSensorAttr,
    HI_MAPI_VCAP_DEV_ATTR_S* pstVcapDevAttr)
{
    pstVcapDevAttr->enIntfMode = VI_MODE_MIPI;
    pstVcapDevAttr->enInputDataType = VI_DATA_TYPE_RGB;
    if (pstSensorAttr->stInputDevAttr.inputDataType == DATA_TYPE_RAW_10BIT) {
        pstVcapDevAttr->au32ComponentMask[0] = 0xFFC00000;
        pstVcapDevAttr->au32ComponentMask[1] = 0;
    } else if (pstSensorAttr->stInputDevAttr.inputDataType == DATA_TYPE_RAW_12BIT) {
        pstVcapDevAttr->au32ComponentMask[0] = 0xFFF00000;
        pstVcapDevAttr->au32ComponentMask[1] = 0;
    } else {
        printf("other vcap dev attr ComponentMask has not be adapt yet\n");
    }
}

HI_S32 SAMPLE_VCAP_CreateSingle(HI_HANDLE VcapDevHdl, HI_HANDLE VcapPipeHdl, HI_HANDLE PipeChnHdl,
                                                HI_MAPI_SENSOR_ATTR_S* pstVcapSensorAttr, HI_MAPI_VCAP_ATTR_S* pstVcapAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;

    SAMPLE_CHECK_RET(HI_MAPI_VCAP_InitSensor(VcapDevHdl, pstVcapSensorAttr, HI_TRUE));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_SetAttr(VcapDevHdl, pstVcapAttr));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartDev(VcapDevHdl));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartChn(VcapPipeHdl, PipeChnHdl));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_InitISP(VcapPipeHdl));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartPipe(VcapPipeHdl));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartISP(VcapPipeHdl));

    return s32Ret;
}

HI_S32 SAMPLE_VCAP_DestorySingle(HI_HANDLE VcapDevHdl, HI_HANDLE VcapPipeHdl, HI_HANDLE PipeChnHdl)
{
    HI_S32 s32Ret = HI_SUCCESS;

    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StopISP(VcapPipeHdl));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StopPipe(VcapPipeHdl));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_DeInitISP(VcapPipeHdl));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StopChn(VcapPipeHdl, PipeChnHdl));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StopDev(VcapDevHdl));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_DeinitSensor(VcapDevHdl));

    return s32Ret;
}

HI_S32 SAMPLE_VCAP_CreateDouble(HI_HANDLE VcapDevHdl, HI_HANDLE VcapPipeHdl1, HI_HANDLE PipeChnHdl1,HI_HANDLE VcapPipeHdl2,HI_HANDLE PipeChnHdl2,
                                                   HI_MAPI_SENSOR_ATTR_S* pstVcapSensorAttr, HI_MAPI_VCAP_ATTR_S* pstVcapAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;

    SAMPLE_CHECK_RET(HI_MAPI_VCAP_InitSensor(VcapDevHdl, pstVcapSensorAttr, HI_TRUE));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_SetAttr(VcapDevHdl, pstVcapAttr));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartDev(VcapDevHdl));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartChn(VcapPipeHdl1, PipeChnHdl1));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartChn(VcapPipeHdl2, PipeChnHdl2));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_InitISP(VcapPipeHdl1));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_InitISP(VcapPipeHdl2));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartPipe(VcapPipeHdl1));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartPipe(VcapPipeHdl2));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartISP(VcapPipeHdl1));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StartISP(VcapPipeHdl2));

    return s32Ret;
}

HI_S32 SAMPLE_VCAP_DestoryDouble(HI_HANDLE VcapDevHdl, HI_HANDLE VcapPipeHdl1, HI_HANDLE PipeChnHdl1,HI_HANDLE VcapPipeHdl2,HI_HANDLE PipeChnHdl2)
{
    HI_S32 s32Ret = HI_SUCCESS;

    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StopISP(VcapPipeHdl1));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StopISP(VcapPipeHdl2));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StopPipe(VcapPipeHdl1));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StopPipe(VcapPipeHdl2));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_DeInitISP(VcapPipeHdl1));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_DeInitISP(VcapPipeHdl2));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StopChn(VcapPipeHdl1, PipeChnHdl1));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StopChn(VcapPipeHdl2, PipeChnHdl2));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_StopDev(VcapDevHdl));
    SAMPLE_CHECK_RET(HI_MAPI_VCAP_DeinitSensor(VcapDevHdl));

    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
