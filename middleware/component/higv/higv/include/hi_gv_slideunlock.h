/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: slideunlock widget API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_SLIDEUNLOCK_H__
#define __HI_GV_SLIDEUNLOCK_H__

#include "hi_gv_conf.h"
#ifdef HIGV_USE_WIDGET_SLIDEUNLOCK
/* add include here */
#include "hi_type.h"
#include "hi_go.h"
#include "hi_gv_resm.h"

#ifdef __cplusplus
extern "C" {
#endif
/* ************************** Structure Definition *************************** */
/* * addtogroup      SlideUnlock  */
/* * ��SlideUnlock widget�� ; CNcomment: ��SlideUnlock�ؼ��� */

#define SLIDEUNLOCK_LENGTH  40  /* <length of slideunlock ; CNcomment: ���۳��ȣ����ֵ */
#define SLIDEUNLOCK_DUATION 250 /* <kickback duation ; CNcomment: �ص�ʱ�� */

typedef enum HI_SLIDEUNLOCK_STYLE {
    SLIDEUNLOCK_H = 0,  /* <Horizontal ; <CNcomment: ˮƽ */
    SLIDEUNLOCK_V,   /* <Vertical ; <CNcomment: ��ֱ */
    SLIDEUNLOCK_BUT
} SLIDEUNLOCK_STYLE;

typedef enum hiHIGV_SLIDEUNLOCK_E { /* * SlideUnlock type ; CNcomment:  ����ؼ����� */
    HIGV_SLIDEUNLOCK_NORMAL = 0,
    HIGV_SLIDEUNLOCK_TOUCH,
    HIGV_SLIDEUNLOCK_DONE,
    HIGV_SLIDEUNLOCK_BUTT
} HIGV_SLIDEUNLOCK_E;

#define HIGV_SLIDEUNLOCK_H HIGV_STYLE_DEFINE(HIGV_WIDGET_SLIDEUNLOCK, SLIDEUNLOCK_H)
#define HIGV_SLIDEUNLOCK_V HIGV_STYLE_DEFINE(HIGV_WIDGET_SLIDEUNLOCK, SLIDEUNLOCK_V)

/* * Structure Definition end */
/* ****************************** API declaration **************************** */
/* * addtogroup      SlideUnlock  */
/* *  ��SlideUnlock widget��; CNcomment: ��SlideUnlock�ؼ��� */
/**
* brief Set skin of trackbar. CNcomment:����SlideUnlock���˵�Ƥ��
* param[in] hSlideUnlock SlideUnlock handle.CNcomment:hSlideUnlock���
* param[in] hRes    Picture resource ID.CNcomment:hSlideUnlockͼƬ��ԴID
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_SlideUnlock_SetSliImage(HIGV_HANDLE hSlideUnlock, HI_RESID hRes);

/**
* brief Set the skin of slideblock.CNcomment:����SlideUnlock�����Ƥ��
* param[in] hSlideUnlock    SlideUnlock handle.CNcomment:hSlideUnlock���
* param[in] hSkinIndex  Skin index.CNcomment:hSlideUnlockƤ��״̬
* param[in] hRes   Picture resource ID of hSlideUnlock.CNcomment:hSlideUnlockͼƬ��ԴID
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_SlideUnlock_SetTraImage(HIGV_HANDLE hSlideUnlock, HI_U32 hSkinIndex, HI_RESID hRes);

/**
* brief Set status of slideunlock.CNcomment:����SlideUnlockҵ���ʹ��״̬
* param[in] hSlideUnlock    SlideUnlock handle.CNcomment:hSlideUnlock���
* param[in] bIsEnable    enable status.CNcomment:ҵ��ʹ��״̬
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_SlideUnlock_SetStatus(HIGV_HANDLE hSlideUnlock, HI_BOOL bIsEnable);

/**
* brief get status of slideunlock.CNcomment:��ȡSlideUnlockҵ���ʹ��״̬
* param[in] hSlideUnlock SlideUnlock handle.CNcomment:hSlideUnlock���
* param[out] pIsEnable enable status pointer.CNcomment:ҵ��ʹ��ָ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_SlideUnlock_GetStatus(HIGV_HANDLE hSlideUnlock, HI_U32 *pIsEnable);

/**
* brief back to init status.CNcomment:�ָ�����ʼ״̬
* param[in] hSlideUnlock SlideUnlock handle.CNcomment:hSlideUnlock���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_SlideUnlock_ReSet(HIGV_HANDLE hSlideUnlock);

/**
* brief Register track widget.CNcomment:ע��SlideUnlock�ؼ�
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_SlideUnlock_RegisterWidget(HI_VOID);

/* API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif /* __HI_GV_SLIDEUNLOCK_H__ */
#endif
