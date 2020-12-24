/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: Scrollbar widget API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_SCROOBAL_H__
#define __HI_GV_SCROOBAL_H__

/* add include here */
#include "hi_gv_widget.h"

#ifdef HIGV_USE_WIDGET_SCROLLBAR

#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* * addtogroup      ScrollBar       */
/* * ��ScrollBar widget��; CNcomment:  ��ScrollBar�ؼ��� */
/* *CNcomment:  ScrollBar���� */
typedef enum {
    SCROLLBAR_STYLE_VERTICAL,
    /* *<Vertical style ; CNcomment:  ��ֱ���ScrollBar */
    SCROLLBAR_STYLE_HORIZONTAL,
    /* *<Horizontal style ; CNcomment:  ˮƽ���ScrollBar */
    SCROLLBAR_BUTT
} HIGV_SCROLLBAR_SYLE_E;

/* *Only suport vetical scrollbar ; CNcomment:  �ؼ�Ĭ�Ϸ��Ϊ��ֱ���,Ŀǰ��ʱֻ֧�ִ�ֱ��� */
/* * Vertical  scrollbar ; CNcomment:��ֱ���ScrollBar */
#define HIGV_SCROLLBAR_STYLE_VERTICAL HIGV_STYLE_DEFINE(HIGV_WIDGET_SCROLLBAR, SCROLLBAR_STYLE_VERTICAL)
/* * CNcomment: Horizonal ScrollBar ; CNcomment: ˮƽ���ScrollBar */
#define HIGV_SCROLLBAR_STYLE_HORIZONTAL HIGV_STYLE_DEFINE(HIGV_WIDGET_SCROLLBAR, SCROLLBAR_STYLE_HORIZONTAL)

/* *   ==== Structure Definition end ==== */
/* ****************************** API declaration **************************** */
/* * addtogroup      ScrollBar       */
/* *  ��ScrollBar widget�� ; ��ScrollBar�ؼ��� */
/**
* brief Set scrollbar infomation. CNcomment: ���û�����Ϣ
* attention :The offset of slide block is dicided by the height of left & top corner picture.
* CNcomment: ����ĳ�ʼƫ��λ������ScrollBar�ı���Ƥ�������Ͻ�ltopindexͼƬ�߶Ⱦ�����
* param[in] hScrollBar Scrollbar handle.CNcomment: ���������
* param[in] SlideRes Resource information.CNcomment: ��Դ��Ϣ
* retval ::HI_SUCCESS Success.CNcomment: �ɹ�
* retval ::HI_ERR_COMM_LOST Invalid handle.CNcomment: �����Ч
* retval ::HI_ERR_COMM_INVAL CNcomment: ��Ч����
*/
HI_S32 HI_GV_ScrollBar_SetSlideRes(HIGV_HANDLE hScrollBar, HI_RESID SlideRes);

/**
* brief Set the position of slide block. CNcomment: ���û����λ��
* param[in] hScrollBar Scrollbar handle.CNcomment: ���������
* param[in] Pos  Slide block position.CNcomment: �����λ�ã������Կ�����ʱ������Ч���ã�������0��ʼ��
* retval ::HI_SUCCESS SuccessCNcomment: �ɹ�
* retval ::HI_ERR_COMM_LOST Invalid handle.CNcomment: �����Ч
* retval ::HI_ERR_COMM_INVAL CNcomment: ��Ч����
* see ::HI_GV_ScrollBar_GetPos
*/
HI_S32 HI_GV_ScrollBar_SetPos(HIGV_HANDLE hScrollBar, HI_U32 Pos);

/**
* brief Get the position of slide block. CNcomment: ��ȡ�����λ��
* param[in]  hScrollBar Scrollbar handle.CNcomment: ���������
* param[out] pPos Position pointer. CNcomment: �����λ��ָ��
* retval ::HI_SUCCESS SuccessCNcomment: �ɹ�
* retval ::HI_ERR_COMM_LOST Invalid handle.CNcomment: �����Ч
* retval ::HI_ERR_COMM_INVAL Invalid paramater.CNcomment: ��Ч����
* see ::HI_GV_ScrollBar_SetPos
*/
HI_S32 HI_GV_ScrollBar_GetPos(HIGV_HANDLE hScrollBar, HI_U32 *pPos);

/**
* brief Set the range of slide block. CNcomment: ���ù�����������Χ
* param[in] hScrollBar  Scrollbar handle.CNcomment: ���������
* param[in] ScrollRange The range of slide block, CNcomment: ������������Χ
* retval ::HI_SUCCESS SuccessCNcomment: �ɹ�
* retval ::HI_ERR_COMM_LOST Invalid handle.CNcomment: �����Ч
*/
HI_S32 HI_GV_ScrollBar_SetScrollRange(HIGV_HANDLE hScrollBar, HI_U32 ScrollRange);

/**
* brief Set the length of visual area. CNcomment: ���ÿɼ����򳤶�
* param[in] hScrollBar  Scrollbar handle.CNcomment: ���������
* param[in] Len The length of visual area.CNcomment: �ؼ��ɼ�����ĳ���
* retval ::HI_SUCCESS Success.CNcomment: �ɹ�
* retval ::HI_ERR_COMM_LOST Invalid handle.CNcomment: �����Ч
*/
HI_S32 HI_GV_ScrollBar_SetVisibleLen(HIGV_HANDLE hScrollBar, HI_U32 Len);

/**
* brief Set the length of content.CNcomment: �������ݳ���
* param[in] hScrollBar  Scrollbar handle.CNcomment: ���������
* param[in] Len CNcomment: �ؼ���Ч���ݳ��ȣ����������ú�ʱ��Ч
* retval ::HI_SUCCESS SuccessCNcomment: �ɹ�
* retval ::HI_ERR_COMM_LOST Invalid handle.CNcomment: �����Ч
*/
HI_S32 HI_GV_ScrollBar_SetContentLen(HIGV_HANDLE hScrollBar, HI_U32 Len);

/**
* brief Set the picture resouce of the button which is pressed or normal state.CNcomment:
* ���ù��������°�ť�����Ͱ���״̬ͼƬ \attention \n This api is not support 9 pitchs style.
* The noraml state and pressed
* state will not effect at the same time. CNcomment:
* ����״̬�Ͱ���״̬�µ�ͼƬ������ͬʱ��Ч�����һ��״̬��Ч��
* ��һ��״̬��Ч������Ч��״̬����Ч��״̬ʹ��ͬһ��ͼƬ
* ͼƬ������һ���ţ���֧�־Ź�����ͼ��ʽ
* param[in] hScrollBar  Scrollbar handle.CNcomment: ���������
* param[in] UpNormalImg Up arrow which is normal state.CNcomment: �ϰ�ť����״̬ͼƬ
* param[in] UpPressImg Up arrow which is pressed picture..CNcomment: �ϰ�ť����״̬ͼƬ
* param[in] DownNormalImg Down arrow which is normal picture.CNcomment: �°�ť����״̬ͼƬ
* param[in] DownPressImg Down arrow which is pressed picture. CNcomment: �°�ť����״̬ͼƬ
* retval ::HI_SUCCESS SuccessCNcomment: �ɹ�
* retval ::HI_ERR_COMM_LOST Invalid handle.CNcomment: �����Ч
*/
HI_S32 HI_GV_ScrollBar_SetButtonImg(HIGV_HANDLE hScrollBar, HI_RESID UpNormalImg, HI_RESID UpPressImg,
                                    HI_RESID DownNormalImg, HI_RESID DownPressImg);

/**
* brief No paint  scrollbar when suspend on the container widget  and content length
* not above container widget length.
* CNcomment: ��ǰscrollbarΪ����״̬���Ұ󶨵Ŀؼ������ݵĳ���С�ڵ��ڿؼ��ĳ���ʱ��������scrollbar�ؼ�
* param[in] hScrollBar Scrollbar handle.CNcomment: ���������
* param[in] bIsSuspending  Is suspending status.CNcomment: ����״̬�Ƿ�����
* retval ::HI_SUCCESS SuccessCNcomment: �ɹ�
* retval ::HI_ERR_COMM_LOST Invalid handle.CNcomment: �����Ч
* retval ::HI_ERR_COMM_INVAL CNcomment: ��Ч����
*/
HI_S32 HI_GV_ScrollBar_SetStatus(HIGV_HANDLE hScrollBar, HI_BOOL bIsSuspending);

/* * API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif
#endif /* __HI_GV_SCROOBAL_H__ */
