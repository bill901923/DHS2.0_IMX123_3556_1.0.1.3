/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: ScrollText widget API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_SCROLLTEXT_H__
#define __HI_GV_SCROLLTEXT_H__

/* add include here */
#include "hi_gv.h"

#ifdef HIGV_USE_WIDGET_SCROLLTEXT

#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* * addtogroup      ScrollText        */
/* * ��ScrollText widget��CNcomment: ��ScrollText�ؼ��� */
/* *Scrolltext style: alawy scroll ; CNcomment: ScrollText�ؼ��Զ�����--��������״̬�£�ʼ��ѭ������ */
#define ST_STYLE_ALWAYSSCROLL 0x4

/* * Scrolltext style: alawy scroll; CNcomment:ScrollText�ؼ��Զ�����--��������״̬�£�ʼ��ѭ������ */
#define HIGV_ST_STYLE_ALWAYSSCROLL HIGV_STYLE_DEFINE(HIGV_WIDGET_SCROLLTEXT, ST_STYLE_ALWAYSSCROLL)

/* *scroll direction ; CNcomment: �������� */
typedef enum {
    HIGV_DIRECTION_LEFT = 0,
    /* left scroll ; CNcomment: ����������� */
    HIGV_DIRECTION_RIGHT,
    /* right scroll ; CNcomment: �������ҹ��� */
    HIGV_DIRECTION_BUTT
} HIGV_DIRECTION_E;

/* * ==== Structure Definition end ==== */
/* ****************************** API declaration **************************** */
/* * addtogroup      ScrollText        */
/* *  ��ScrollText widget�� ; CNcomment: ��ScrollText�ؼ��� */
/**
* brief Set content of scrolltext. CNcomment:���ù�����Ļ�ؼ��е�����
* param[in] hScrollText Scrolltext handle.CNcomment:�ؼ����
* param[in] Image Picture resouce ID.CNcomment:ͼƬ��ԴID��INVALID_RESIDΪ��ͼƬ
* param[in] String String.CNcomment:�ı��ַ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* see ::HI_GV_ScrollText_GetContent
*/
HI_S32 HI_GV_ScrollText_SetContent(HIGV_HANDLE hScrollText, HI_RESID Image, HI_CHAR *String);

/**
* brief Set content of scrolltext.CNcomment:���ù�����Ļ�ؼ��е�����
* param[in] hScrollText Scrolltext handle.CNcomment:�ؼ����
* param[in] Image Picture resouce ID.CNcomment:ͼƬ��ԴID��INVALID_RESIDΪ��ͼƬ
* param[in] String String.CNcomment:�ı��ַ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* see ::HI_GV_ScrollText_GetContent
*/
HI_S32 HI_GV_ScrollText_SetContentByID(HIGV_HANDLE hScrollText, HI_RESID Image, HI_U32 StrID);

/**
* brief Get content of scrolltext.CNcomment:��ȡ������Ļ�ؼ�������
* param[in] hScrollText Scrolltext handle.CNcomment:�ؼ����
* param[out] pImage Picture resouce ID.CNcomment:ͼƬ��ԴID
* param[out] pString String.CNcomment:�ı�
* param[in] TextBufLen String length.CNcomment:�ı�����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL  CNcomment:ָ��Ϊ�ջ��ı����ȳ�������ı�����
* see ::HI_GV_ScrollText_SetContent
*/
HI_S32 HI_GV_ScrollText_GetContent(HIGV_HANDLE hScrollText, HI_RESID *pImage, HI_CHAR *pString, HI_U32 TextBufLen);

/**
* brief Set start position of scrolltext.CNcomment:����ͼƬ����ʼλ��
* param[in] hScrollText  Scrolltext handle.CNcomment:�ؼ����
* param[in] x the x coordiate of picture.CNcomment:ͼƬ�ڿؼ��е�x����(����ڿؼ�ԭ�㣬Ĭ����Կؼ�����)
* param[in] y the y coordiate of picture.CNcomment:ͼƬ�ڿؼ��е�y����(����ڿؼ�ԭ�㣬Ĭ����Կؼ�����)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ScrollText_SetImagePos(HIGV_HANDLE hScrollText, HI_S32 x, HI_S32 y);

/**
* brief Set start position of scrolltext.CNcomment:�����ı�����ʼλ��
* param[in] hScrollText   Scrolltext handle.CNcomment:�ؼ����
* param[in] x the x coordiate of text.CNcomment:�ı��ڿؼ��е�x����(����ڿؼ�ԭ�㣬Ĭ����Կؼ�����)
* param[in] y the y coordiate of text.CNcomment:�ı��ڿؼ��е�y����(����ڿؼ�ԭ�㣬Ĭ����Կؼ�����)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ScrollText_SetTextPos(HIGV_HANDLE hScrollText, HI_S32 x, HI_S32 y);

/**
* brief Set interval of scrolltext.CNcomment:���ÿؼ�������ʱ����
* param[in] hScrollText Scrolltext handle.CNcomment:�ؼ����
* param[in] TimeInterval Time interval.CNcomment:ʱ����(��msΪ��λ��Ĭ��Ϊ30ms)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ScrollText_SetTimeInterval(HIGV_HANDLE hScrollText, HI_U32 TimeInterval);

/**
* brief Set step of scrolltext.CNcomment:���ÿؼ���������
* param[in] hScrollText CNcomment:�ؼ����
* param[in] Step Scroll step.CNcomment:��������(��pixelΪ��λ��Ĭ��Ϊ5pixel)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ScrollText_SetStep(HIGV_HANDLE hScrollText, HI_U32 Step);

/**
* brief Set direction of scrolltext.CNcomment:���ù�������
* param[in] hScrollText Scrolltext handle.CNcomment:�ؼ����
* param[in] Direction Scroll direction.CNcomment:��������(Ĭ����������)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ScrollText_SetDirection(HIGV_HANDLE hScrollText, HIGV_DIRECTION_E Direction);

/**
* brief Set widget content.CNcomment:���ÿؼ�״̬
* param[in] hScrollText Scrolltext handle.CNcomment:�ؼ����
* param[in] Status The satus of scrolltext.CNcomment:�ؼ�״̬(TRUEΪ������FALSEΪֹͣ������Ĭ��ΪTRUE)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ScrollText_SetStatus(HIGV_HANDLE hScrollText, HI_BOOL Status);

/**
* brief Get the position of the content.CNcomment:��ȡ�ؼ����ݵ�ǰ���ڵ�λ��
* param[in] hScrollText Scrolltext handle.CNcomment:�ؼ����
* param[out] x         x coordinate. CNcomment:x����
* param[out] y        y coordinate.  CNcomment:y����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ScrollText_GetCurPos(HIGV_HANDLE hScrollText, HI_S32 *x, HI_S32 *y);

/**
* brief Reset  the position of the content.CNcomment:��λ�ؼ����ݵ�λ��
* param[in] hScrollText Scrolltext handle.CNcomment:�ؼ����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ScrollText_ResetPos(HIGV_HANDLE hScrollText);

/* API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif
#endif /* __HI_GV_SCROLLTEXT_H__ */
