/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_public_vcap.h
 * @brief   sample public vcap header
 * @author  HiMobileCam NDK develop team
 * @date  2019-4-16
 */
#ifndef __SAMPLE_PUBLIC_VCAP_H__
#define __SAMPLE_PUBLIC_VCAP_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


HI_VOID SAMPLE_COMM_VCAP_AdaptDevAttrFromSensor(HI_MAPI_SENSOR_ATTR_S *pstSensorAttr,
    HI_MAPI_VCAP_DEV_ATTR_S* pstVcapDevAttr);
HI_S32 SAMPLE_COMM_VCAP_SnapTrigger(HI_S32 VCapPipeHdl, HI_S32 VEncHdl, HI_S32 s32Framecnt);

HI_S32 SAMPLE_VCAP_CreateSingle(HI_HANDLE VcapDevHdl, HI_HANDLE VcapPipeHdl, HI_HANDLE PipeChnHdl,
    HI_MAPI_SENSOR_ATTR_S* pstVcapSensorAttr, HI_MAPI_VCAP_ATTR_S* pstVcapAttr);

HI_S32 SAMPLE_VCAP_DestorySingle(HI_HANDLE VcapDevHdl, HI_HANDLE VcapPipeHdl, HI_HANDLE PipeChnHdl);

HI_S32 SAMPLE_VCAP_CreateDouble(HI_HANDLE VcapDevHdl, HI_HANDLE VcapPipeHdl1, HI_HANDLE PipeChnHdl1,HI_HANDLE VcapPipeHdl2,HI_HANDLE PipeChnHdl2,
    HI_MAPI_SENSOR_ATTR_S* pstVcapSensorAttr, HI_MAPI_VCAP_ATTR_S* pstVcapAttr);
HI_S32 SAMPLE_VCAP_DestoryDouble(HI_HANDLE VcapDevHdl, HI_HANDLE VcapPipeHdl1, HI_HANDLE PipeChnHdl1,HI_HANDLE VcapPipeHdl2,HI_HANDLE PipeChnHdl2);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SAMPLE_PUBLIC_VCAP_H__ */
