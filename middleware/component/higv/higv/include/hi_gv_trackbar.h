/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: trackbar widget API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_TRACKBAR_H__
#define __HI_GV_TRACKBAR_H__

#include "hi_gv_conf.h"
#ifdef HIGV_USE_WIDGET_TRACKBAR
/* add include here */
#include "hi_type.h"
#include "hi_go.h"
#include "hi_gv_resm.h"

#ifdef __cplusplus
extern "C" {
#endif
/* ************************** Structure Definition *************************** */
/* * addtogroup      TrackBar  */
/* * ��TrackBar widget�� ; CNcomment: ��TrackBar�ؼ��� */

typedef enum HI_TRACKBAR_STYLE {
    TRACKBAR_H = 0,  /* <Horizontal ; <CNcomment: ˮƽ */
    TRACKBAR_V,  /* <Vertical ; <CNcomment: ��ֱ */
    TRACKBAR_BUT
} TRACKBAR_STYLE;

typedef enum hiHIGV_TRACKBAR_E {    /* Trackbar type ; CNcomment:  ����ؼ����� */
    HIGV_TRACKBAR_NORMAL = 0,
    HIGV_TRACKBAR_ACTIVE,
    HIGV_TRACKBAR_MOUSEDOWN,
    HIGV_TRACKBAR_BUTT
} HIGV_TRACKBAR_E;

#define HIGV_TRACKBAR_H HIGV_STYLE_DEFINE(HIGV_WIDGET_TRACKBAR, TRACKBAR_H)
#define HIGV_TRACKBAR_V HIGV_STYLE_DEFINE(HIGV_WIDGET_TRACKBAR, TRACKBAR_V)

/* *  Structure Definition end */
/* ****************************** API declaration **************************** */
/* * addtogroup      TrackBar  */
/* *  ��TrackBar widget�� ; CNcomment: ��TrackBar�ؼ��� */
/**
* brief Set skin of trackbar. CNcomment:����TrackBar���˵�Ƥ��
* param[in] hTrack Trackbar handle.CNcomment:Track���
* param[in] hSkinIndex  Skin index.CNcomment:TrackƤ��״̬
* param[in] hRes    Picture resource ID.CNcomment:TrackͼƬ��ԴID
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Track_SetSliImage(HIGV_HANDLE hTrack, HI_U32 hSkinIndex, HI_RESID hRes);

/**
* brief Set the skin of slideblock.CNcomment:����TrackBar�����Ƥ��
* param[in] hTrack    Trackbar handle.CNcomment:Track���
* param[in] hSkinIndex  Skin index.CNcomment:TrackƤ��״̬
* param[in] hRes   Picture resource ID of track bar.CNcomment:TrackͼƬ��ԴID
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Track_SetTraImage(HIGV_HANDLE hTrack, HI_U32 hSkinIndex, HI_RESID hRes);

/**
* brief Set the value of trackbar.CNcomment:����TrackBar��ǰֵ
* param[in] hTrack    Trackbar handle.   CNcomment:TrackBar���
* param[in] value     Get current valude of cursor.  CNcomment:�õ��α�ĵ�ǰֵ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Track_SetCurVal(HIGV_HANDLE hTrack, HI_U32 Value);

/**
* brief Get current graduation of cursror.CNcomment:�õ��α굱ǰ�̶�
* param[in] hTrack      Trackbar handle.CNcomment: TrackBar���
* param[out] pValue     Get value of current cursor. CNcomment:�õ��α�ĵ�ǰֵ
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Track_GetCurVal(HIGV_HANDLE hTrack, HI_U32 *pValue);

/**
* brief Set the range of cursor. CNcomment:�趨�α�ķ�Χֵ
* param[in] hTrack     Trackbar handle.CNcomment:Track���
* param[in] MaxnValue Max value of cursor.CNcomment: �α����ֵ
* param[in] MinValue  Min value of cursor.CNcomment: �α���Сֵ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Track_SetRange(HIGV_HANDLE hTrack, HI_U32 MinValue, HI_U32 MaxValue);

/**
* brief Get the range of cursor.CNcomment:��ȡ�α�ķ�Χ
* param[in]  hTrack     Trackbar handle.CNcomment:Track���
* param[out] pMaxValue  Max value of cursor.CNcomment:�õ��α����ֵ
* param[out] pMinValue  Min value of cursor.CNcomment:�õ��α���Сֵ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Track_GetRange(HIGV_HANDLE hTrack, HI_U32 *pMinValue, HI_U32 *pMaxValue);

/**
* brief Set gesture enable.CNcomment:���ÿؼ��Ƿ�֧�ִ���
* param[in]  hTrack  Scrollgrid handle. CNcomment:�ؼ����
* param[in]  bIsEnable   Is enable gesture. CNcomment:�Ƿ�֧�ִ���( HI_TRUE��ʾ֧�֣�HI_FALSE��ʾ��ֹ)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_Track_EnableGesture(HIGV_HANDLE hTrack, HI_BOOL bIsEnable);

/**
* brief Get gesture enable.CNcomment:��ȡ�ؼ��Ƿ�֧�ִ���
* param[in]  hTrack  Scrollgrid handle. CNcomment:�ؼ����
* param[in]  pIsEnable   Is enable gesture. CNcomment:�Ƿ�֧�ִ���( HI_TRUE��ʾ֧�֣�HI_FALSE��ʾ��ֹ��ϵͳĬ��ΪHI_TRUE)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_Track_IsGestureEnable(HIGV_HANDLE hTrack, HI_BOOL *pIsEnable);

/**
* brief Register track widget.CNcomment:ע��track�ؼ�
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_Track_RegisterWidget(HI_VOID);

/* ==== API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif /* __HI_GV_TRACKBAR_H__ */
#endif
