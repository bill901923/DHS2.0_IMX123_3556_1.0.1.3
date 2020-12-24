/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: ScrollView widget API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_SCROLLVIEW_H__
#define __HI_GV_SCROLLVIEW_H__

/* add include here */
#include "hi_gv_widget.h"
#ifdef HIGV_USE_WIDGET_SCROLLVIEW
#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* * addtogroup      scroll view  */
/* * ��ScrollView widget��; CNcomment:��ScrollView�ؼ��� */
/* *Scroll direction; CNcomment: ScrollView�ؼ��Ĺ�������ö�� */
/* *NOTE: It is only valid for touch operation or mouse wheel operation */
typedef enum _SCROLLVIEW_DIRECTION_E {
    SCROLLVIEW_DIRECTION_HORI = 0,  /*Horizal direction ; CNcomment: ˮƽ�������� */
    SCROLLVIEW_DIRECTION_VERT,  /*Vertial direction ; CNcomment:��ֱ�������� */
    SCROLLVIEW_DIRECTION_BUTT
} HIGV_SCROLLVIEW_DIRECTION_E;

/* *Content box style */
typedef enum {
    SCROLLVIEW_STYLE_DEFAULTE = 0x00,
    SCROLLVIEW_STYLE_ALWAY_CENTER = 0x01,
    SCROLLVIEW_STYLE_BUTT = 0x10000
} HIGV_SCROLLVIEW_STYLE_E;

typedef struct hiHIGV_SCROLLVIEW_Init_S {
    HIGV_HANDLE
    hVerticalScrollBar; /* Bind vertical scrollbar handle; CNcomment: �󶨵Ĵ�ֱ��������� */
    HIGV_HANDLE
    hHorizontalScrollbar; /* Bind horizontal scrollbar handle ; CNcomment: �󶨵�ˮƽ��������� */
    HI_U32 LeftMargin; /* Invalid value ; CNcomment: ��ǰ�汾��Ч */
    HI_U32 RightMargin; /* Child widget right margin of background ; CNcomment: ���ӿؼ��ڱ������ұ߾� */
    HI_U32 TopMargin; /* Invalid value ; CNcomment: ��ǰ�汾��Ч */
    HI_U32 BottomMargin; /* Child widget bottom margin of background ; CNcomment: ���ӿؼ��ڱ����ĵױ߾� */
    HI_U32 Step; /* Scroll step ; CNcomment: �������� */
    HI_U32 Interval; /* Scroll interval ; CNcomment: ������� */
    HI_U32 ScrollContentWidth; /* The scroll content invariable width ; CNcomment: �������ݵĹ̶���� */
    HI_U32 ScrollContentHeight; /* The scroll content invariable height ; CNcomment: �������ݵĹ̶��߶� */
    HI_U32 ScrollViewStyle; /* Scroll view private style ; CNcomment: ˽�з�� */
    HIGV_SCROLLVIEW_DIRECTION_E Direction; /* Scroll direction ; CNcomment: �������� */
} HIGV_SCROLLVIEW_Init_S;
/* *  -- Macro Definition end */
/* ****************************** API declaration **************************** */
/* *  addtogroup      scroll view */
/* *  ��ScrollView wiget��   ��ScrollView�ؼ��� */
/**
* brief Init the scrollview.CNcomment:��ʼ��scrollview�ؼ�
* param[in]  hScrollView Scroll view handle. CNcomment:�ؼ����
* param[in]  pStyle The info of scrollview when created. CNcomment:�ؼ�������Ϣ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_EXIST
*/
HI_S32 HI_GV_ScrollView_Init(HIGV_HANDLE hScrollView, HIGV_SCROLLVIEW_Init_S *pInitAttr);

/**
* brief Set content invariable size.CNcomment:�������ݵĹ̶���С
* param[in]  hScrollView Scroll view handle. CNcomment:�ؼ����
* param[in]  Width The invariable width. CNcomment:�̶���ȣ���Χ����ڵ��ڿؼ��Ŀ��
* param[in]  Height The invariable height. CNcomment:�̶��߶ȣ���Χ����ڵ��ڿؼ��ĸ߶�
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_PAERM
*/
HI_S32 HI_GV_ScrollView_SetContentSize(HIGV_HANDLE hScrollView, HI_U32 Width, HI_U32 Height);

/**
* brief Move the view to center.CNcomment:���ӿ���������
* param[in]  hScrollView scroll view handle. CNcomment:�ؼ����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_FAILURE
*/
HI_S32 HI_GV_ScrollView_MoveToCenter(HIGV_HANDLE hScrollView);

/**
* brief Get the content rectangle.CNcomment:��ȡ���ݵľ���
* param[in]  hScrollView Scroll view handle. CNcomment:�ؼ����
* param[out] pRect Rectangle. CNcomment:�����������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollView_GetContentRect(HIGV_HANDLE hScrollView, HI_RECT *pRect);

/**
* brief Get the view coordinate in the background content.CNcomment:��ȡ�ӿ�������ݵ�����
* attention \n
     _____________________
    |  background content |
    |        ____         |
    |       |view|        |
    |       |____|        |
    |_____________________|
* param[in]  hScrollView Scroll view handle. CNcomment:�ؼ����
* param[out] px X abscissa. CNcomment:x����
* param[out] py Y abscissa. CNcomment:y����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollView_GetViewCoordinate(HIGV_HANDLE hScrollView, HIGV_CORD *px, HIGV_CORD *py);

/**
* brief Set the view coordinate in the background content.CNcomment:�����ӿ�������ݵ�����
* param[in]  hScrollView Scroll view handle. CNcomment:�ؼ����
* param[out] px X abscissa. CNcomment:x����
* param[out] py Y abscissa. CNcomment:y����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollView_SetViewCoordinate(HIGV_HANDLE hScrollView, HIGV_CORD x, HIGV_CORD y);

/**
* brief Set scroll step.CNcomment:���ù�������
* param[in]  hScrollView Scroll view handle. CNcomment:�ؼ����
* param[in]  Step Scroll step.CNcomment:��������(��pixelΪ��λ��Ĭ��Ϊ20pixel)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollView_SetStep(HIGV_HANDLE hScrollView, HI_U32 Step);

/**
* brief Set scroll Interval.CNcomment:���ù������
* param[in]  hScrollView Scroll view handle. CNcomment:�ؼ����
* param[in]  Interval Interval. CNcomment:���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollView_SetInterval(HIGV_HANDLE hScrollView, HI_U32 Interval);

/**
* brief Bind the scrollbar to scrollbar.CNcomment:�󶨹�����
* param[in]  hScrollView Scroll view handle. CNcomment:�ؼ����
* param[in]  hVertical vertical scrollbar. CNcomment:��ֱ������
* param[in]  hHorizontal horizontal scrollbar. CNcomment:ˮƽ������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollView_BindScrollBar(HIGV_HANDLE hScrollView, HIGV_HANDLE hVertical, HIGV_HANDLE hHorizontal);

/**
* brief Check the focus widget in the view.CNcomment:���ȷ������ؼ����ӿ���
* param[in]  hScrollView Scroll view handle. CNcomment:�ؼ����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollView_CheckFocusPos(HIGV_HANDLE hScrollView);

/**
* brief Set scroll sensibility.CNcomment: ���û���������������ϵ����Ĭ����1.0
* param[in]  hScrollView Scroll view handle. CNcomment:�ؼ����
* param[in]  ScrollParam scroll sensibility param. CNcomment:����������ϵ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollView_SetScrollParam(HIGV_HANDLE hScrollView, HI_FLOAT ScrollParam);

/**
* brief Set fling sensibility.CNcomment:������ɨ������������ϵ����Ĭ����1.0
* param[in]  hScrollView Scroll view handle. CNcomment:�ؼ����
* param[in]  FlingParam fling sensibility param. CNcomment:��ɨ������ϵ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollView_SetFlingParam(HIGV_HANDLE hScrollView, HI_FLOAT FlingParam);

/* API declaration end */
#ifdef __cplusplus
}
#endif
#endif
#endif /* __HI_GV_SCROLLVIEW_H__ */
