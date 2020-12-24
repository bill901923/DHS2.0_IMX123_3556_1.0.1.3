/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    sample_public_disp.c
 * @brief   sample public disp module
 * @author  HiMobileCam NDK develop team
 * @date  2019-5-16
 */

#include "sample_cfg.h"
#include "sample_public_disp.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */



HI_S32 SAMPLE_COMM_StartHdmi(HI_VOID)
{

    return HI_SUCCESS;
}

HI_VOID SAMPLE_COMM_StopHdmi(HI_HANDLE HdmiHdl)
{

}

HI_S32 SAMPLE_COMM_StartPreview(HI_HANDLE GrpHdl, HI_HANDLE PortHdl, HI_BOOL bStitch)
{
    /* start disp */
    HI_S32 s32Ret = HI_SUCCESS;
    HI_HANDLE DispHdl = SAMPLE_MAPI_DISP_HANDLE;
    HI_MAPI_DISP_ATTR_S stDispAttr = {0};
    HI_MAPI_DISP_VIDEOLAYER_ATTR_S stVideoLayerAttr = {0};

    stDispAttr.stPubAttr.u32BgColor = 0xFF;
    stDispAttr.stPubAttr.enIntfType = VO_INTF_HDMI;
    stDispAttr.stPubAttr.enIntfSync = VO_OUTPUT_1080P30;
    SAMPLE_CHECK_RET(memset_s(&stDispAttr.stPubAttr.stSyncInfo, sizeof(VO_SYNC_INFO_S), 0, sizeof(VO_SYNC_INFO_S)));
    SAMPLE_CHECK_RET(memset_s(&stVideoLayerAttr, sizeof(HI_MAPI_DISP_VIDEOLAYER_ATTR_S), 0,
        sizeof(HI_MAPI_DISP_VIDEOLAYER_ATTR_S)));
    stVideoLayerAttr.u32BufLen = 3;
    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Init(DispHdl, &stDispAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Start(DispHdl, &stVideoLayerAttr));

    SAMPLE_CHECK_GOTO(SAMPLE_COMM_StartHdmi());

    /* start window */
    HI_MAPI_DISP_WINDOW_ATTR_S stWndAttr;
    HI_HANDLE WndHdl = SAMPLE_MAPI_WIND_HANDLE;

    stWndAttr.stRect.s32X = 0;
    stWndAttr.stRect.s32Y = 0;
    stWndAttr.stRect.u32Width = 1920;
    stWndAttr.stRect.u32Height = 1080;
    stWndAttr.u32Priority = 0;
    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_SetWindowAttr(DispHdl, WndHdl, &stWndAttr));
    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_StartWindow(DispHdl, WndHdl));
    SAMPLE_CHECK_GOTO(HI_MAPI_DISP_Bind_VProc(GrpHdl, PortHdl, DispHdl, WndHdl, bStitch));
exit:
    return s32Ret;
}

HI_VOID SAMPLE_COMM_StopPreview(HI_HANDLE GrpHdl, HI_HANDLE PortHdl, HI_BOOL bStitch)
{
    /* start disp */
    HI_HANDLE DispHdl = SAMPLE_MAPI_DISP_HANDLE;
    HI_HANDLE HdmiHdl = SAMPLE_MAPI_HDMI_HANDLE;
    HI_HANDLE WndHdl = SAMPLE_MAPI_WIND_HANDLE;

    HI_MAPI_DISP_UnBind_VProc(GrpHdl, PortHdl, DispHdl, WndHdl, bStitch);
    HI_MAPI_DISP_StopWindow(DispHdl, WndHdl);
    SAMPLE_COMM_StopHdmi(HdmiHdl);
    HI_MAPI_DISP_Stop(DispHdl);
    HI_MAPI_DISP_Deinit(DispHdl);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
