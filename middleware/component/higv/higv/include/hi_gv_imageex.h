/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: ImageEx wdiget
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_IMAGEEX_H__
#define __HI_GV_IMAGEEX_H__

/* add include here */
#include "hi_type.h"
#include "hi_gv_conf.h"

#ifdef HIGV_USE_WIDGET_IMAGEEX
#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* * addtogroup      ImageEx     */
/* * ��ImageEx widget�, CN:��ImageEx�ؼ��� */
/* * Alignment of image , CNcomment:  ͼƬ�Ķ��뷽ʽ */
typedef enum {
    HIGV_IMGPOS_CENTER,
    /* Center ; CNcomment: ���� */
    HIGV_IMGPOS_STRETCH,
    /* Stretch ; CNcomment: ���� */
    HIGV_IMGPOS_AUTOSTRETCH,
    /* Autostrech ; CNcomment: �Զ����� */
    HIGV_IMGPOS_BUTT
} HIGV_IMGPOS_E;

/* * Structure Definition end */
/* ****************************** API declaration **************************** */
/* * addtogroup      ImageEx     */
/* * ��ImageEx widget�� CNcomment: ��ImageEx�ؼ��� */
/**
* brief Set the content.CNcomment:����ͼƬ����
* param[in] hImageEx Imagex handle. CNcomment:ͼƬ�ؼ����
* param[in] pImgFile  The path file of image.CNcomment:ͼƬ�ļ�·��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_EXIST Timer is exist.CNcomment:��ʱ����Դ�Ѿ�����
* retval ::HI_ERR_COMM_NORES
*/
HI_S32 HI_GV_ImageEx_SetImage(HIGV_HANDLE hImageEx, const HI_CHAR *pImgFile);

/**
* brief Set the count of playback cycles.CNcomment:���ö���ͼƬ���Ŵ���
* param[in] hImageEx Imagex handle. CNcomment:ͼƬ�ؼ����
* param[in] RepeatCount The count of playback cycles .CNcomment:����ͼƬ���Ŵ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_NORES
*/
HI_S32 HI_GV_ImageEx_SetRepeatCount(HIGV_HANDLE hImageEx, HI_S32 RepeatCount);

/**
* brief Set the position of picture to the widget.CNcomment:����ͼƬ�ڿؼ��е���ʾλ��
* param[in] hImageEx Imagex handle.CNcomment:ͼƬ�ؼ����
* param[in] PosType  Position type of picture.CNcomment:ͼƬλ������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ImageEx_SetPos(HIGV_HANDLE hImageEx, HIGV_IMGPOS_E PosType);

/**
* brief Set the interval of image display.CNcomment:����ͼƬ��ʾ��ʱ����
* attention ::
* it is only valid to dynamic gif format.
* CNcomment:�Զ�̬GIFͼƬ��Ч
* param[in] hImageEx Imagex handle.CNcomment:ͼƬ�ؼ����
* param[in] Interval Interval of picture display.CNcomment:ͼƬ��ʾ��ʱ��������msΪ��λ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ImageEx_SetInterval(HIGV_HANDLE hImageEx, HI_U32 Interval);

/* ==== API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif
#endif /* __HI_GV_IMAGEEX_H__ */
