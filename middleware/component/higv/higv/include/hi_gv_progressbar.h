/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: ProgressBar widget API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_PROGRESSBAR_H__
#define __HI_GV_PROGRESSBAR_H__

/* add include here */
#include "hi_go_comm.h"
#include "hi_gv_conf.h"

#ifdef HIGV_USE_WIDGET_PROGRESSBAR
#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* * addtogroup      ProgressBar   */
/* *  ��ProgressBar widget��; CNcomment: ��ProgressBar�ؼ��� */
/* *the macro of horizontal style ; CNcomment: ��������� */
#define PROGRESSBAR_STYLE_HORIZON 0x1

/* *the macro of vertical style ; CNcomment: ��������� */
#define PROGRESSBAR_STYLE_VERTICAL 0x2

/* *Horizontal style ; CNcomment: ������������ */
#define HIGV_PROGRESSBAR_STYLE_HORIZON HIGV_STYLE_DEFINE(HIGV_WIDGET_PROGRESS, PROGRESSBAR_STYLE_HORIZON)

/* *Vertical style ; CNcomment: ������������ */
#define HIGV_PROGRESSBAR_STYLE_VERTICAL HIGV_STYLE_DEFINE(HIGV_WIDGET_PROGRESS, PROGRESSBAR_STYLE_VERTICAL)

/* *  == Structure Definition end ==== */
/* ****************************** API declaration **************************** */
/* * addtogroup      ProgressBar         */
/* * ��ProgressBar widget��;p CNcomment: ��ProgressBar�ؼ��� */
/**
* brief Set range of progressbar.CNcomment:���ý�������Χ
* param[in] hPgBar Progressbar handle.CNcomment:���������
* param[in] Min    Min valude.CNcomment:��Сֵ
* param[in] Max  Max value. CNcomment: ���ֵ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ProgressBar_SetRange(HIGV_HANDLE hPgBar, HI_U32 Min, HI_U32 Max);

/**
* brief Set step.CNcomment:���ò���
* param[in] hPgBar Progressbar handle.CNcomment:���������
* param[in] Step   Step.CNcomment:����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ProgressBar_SetStep(HIGV_HANDLE hPgBar, HI_U32 Step);

/**
* brief Set the posithion of progressbar.CNcomment:���ý�������ǰλ��
* attention :If set pos out of the range, value will be min or max, return HI_SUCCESS.
* CNcomment:���ֵ������Χ����������Ϊ���ֵ����Сֵ�������سɹ�
* param[in] hPgBar Progressbar handle.CNcomment:���������
* param[in] Pos    CNcomment:λ��
* retval ::HI_SUCCESS
* \retval ::HI_ERR_COMM_LOST
* see :: HI_GV_ProgressBar_GetPos
*/
HI_S32 HI_GV_ProgressBar_SetPos(HIGV_HANDLE hPgBar, HI_U32 Pos);

/**
* brief Get the posithion of progressbar.CNcomment:��ȡ��������ǰλ��
* param[in] hPgBar Progressbar handle.CNcomment:���������
* param[out] pPos  Position.CNcomment:λ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* see ::HI_GV_ProgressBar_SetPos
*/
HI_S32 HI_GV_ProgressBar_GetPos(HIGV_HANDLE hPgBar, HI_U32 *pPos);

/**
* brief Set the freground of progressbar.CNcomment:���ý�������ǰ�����
* param[in] hPgBar  Progressbar handle.CNcomment:���������
* param[in] pFgRect The range of progress.CNcomment:������ǰ����ʾ�����Χ(��Խ�����λ��)
* Ϊ�ձ�ʾǰ����Χ��������������С��ͬ \param[in] FgStyle   Fregroud style handle.CNcomment:�����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ProgressBar_SetFg(HIGV_HANDLE hPgBar, HI_RECT *pFgRect, HI_RESID FgStyle);

/*  API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif
#endif /* __HI_GV_PROGRESSBAR_H__ */
