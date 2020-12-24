/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: APIWindow manageer moudle API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_WM_H__
#define __HI_GV_WM_H__

/* add include here */
#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* * addtogroup      WindowManager  */
/* *  ��Window manager ��; �����ڹ��� */
/* * Stack capacity ; CNcomment:����ջ���� */
#define HIGV_MAX_WINNUM 50

/* * ==== Structure Definition end ==== */
/* ****************************** API declaration **************************** */
/* * addtogroup      WindowManager  */
/* * ��Window manager �� ; CNcomment: �����ڹ��� */
/**
* brief Push window to statck.CNcomment:��������ջ
* param[in] hLayer  Layer handle. CNcomment:ͼ����
* param[in] hWindow Window handle. CNcomment:���ھ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_FULL Stack full.CNcomment:ջ��
* see ::HI_GV_WM_PopWindow
*/
HI_S32 HI_GV_WM_PushWindow(HIGV_HANDLE hLayer, HIGV_HANDLE hWindow);

/**
* brief Pop up window from stack.CNcomment:�����ڳ�ջ
* param[in] hLayer  Layer handle. CNcomment:ͼ����
* param[in] hWindow Window handle.CNcomment:���ھ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_EMPTY Stack empty.CNcomment:ջ��
* see ::HI_GV_WM_PushWindow
*/
HI_S32 HI_GV_WM_PopWindow(HIGV_HANDLE hLayer, HIGV_HANDLE hWindow);

/**
* brief Get top window of stack. CNcomment:��ȡջ������
* param[in] hLayer  Layer handle. CNcomment:ͼ����
* param[out] pWindow Window handle. CNcomment: ���ھ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_EMPTY Stack empty.CNcomment:ջ��
*/
HI_S32 HI_GV_WM_GetTopWindow(HIGV_HANDLE hLayer, HIGV_HANDLE *phWindow);

/**
* brief Clear window of the stack.CNcomment:���ջ�д���
* param[in] hLayer  Layer handle. CNcomment:ͼ����
* retval ::HI_SUCCESS
*/
HI_S32 HI_GV_WM_ClearAllWindow(HIGV_HANDLE hLayer);

/**
* brief Get capture mouse window. CNcomment:�����겶�񴰿�
* param[in]  hLayer    Layer handle. CNcomment:ͼ����
* param[out] phWidget  The window which capture mouse. CNcomment:�������Ĵ���
* retval ::HI_SUCCESS
*/
HI_S32 HI_GV_WM_GetMouseCapture(HIGV_HANDLE hLayer, HIGV_HANDLE *phWidget);

/**
* brief Set the capture mouse window.CNcomment:������겶�񴰿�
* param[in] hLayer  Layer handle.CNcomment:ͼ����
* param[in] hWidget Set the window which capture mouse.CNcomment:������겶�񴰿�
* retval ::HI_SUCCESS
N/A.CNcomment:��
*/
HI_S32 HI_GV_WM_SetMouseCapture(HIGV_HANDLE hLayer, HIGV_HANDLE hWidget);

/**
* brief Get the higo layer handle correspoding layer.CNcomment:��ȡͼ���Ӧ��HIGOͼ����
* attention : CNcomment:���ֶ�ͼ��Ĳ�����Ҫ�õײ��higoͼ��ӿ�
* param[in] hLayer     Layer handle.CNcomment:ͼ����
* param[out] pHigoLayer Higo layer handle. CNcomment:HIGOͼ����
* retval ::HI_SUCCESS
*/
HI_S32 HI_GV_WM_GetHigoLayer(HIGV_HANDLE hLayer, HIGV_HANDLE *pHigoLayer);

/**
* brief Touch msg bind with widget. CNcomment:������Ϣ�Ϳؼ���
* param[in] hLayer  Layer handle.CNcomment:ͼ����
* param[in] bIsBind Bind tag, HI_TRUE or HI_FALSE.CNcomment:�Ƿ�󶨿ؼ�
* retval ::HI_SUCCESS
*/
HI_S32 HI_GV_WM_BindTouchMsg(HIGV_HANDLE hLayer, HI_BOOL bIsBind);

/**
* brief Get bind status.CNcomment:��ȡ��״̬
* param[in] hLayer  Layer handle.CNcomment:ͼ����
* param[in] pIsBind Bind status .CNcomment:��״̬��HI_TRUE��ʾ�Ѱ󶨣�HI_FALSE��ʾδ��
* retval ::HI_SUCCESS
*/
HI_S32 HI_GV_WM_GetBindTouchStatus(HIGV_HANDLE hLayer, HI_U32 *pIsBind);

/* * ==== API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif /* __HI_GV_WM_H__ */
