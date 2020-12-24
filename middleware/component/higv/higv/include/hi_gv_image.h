/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: Image widget
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_IMAGE_H__
#define __HI_GV_IMAGE_H__

/* add include here */
#include "hi_gv.h"

#ifdef HIGV_USE_WIDGET_IMAGE
#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* * addtogroup      Image    */
/* *��Image  widget��;  ��Image�ؼ��� */
typedef struct {
    HI_CHAR *pAddr; /* Memory pointer address ; CNcomment:< �ڴ�ָ���ַ */
    HI_U32 Length; /* Length ; CNcomment:< ���� */
} HI_GV_MemInfo;

/* * Structure Definition end */
/* ****************************** API declaration **************************** */
/* * addtogroup      Image */
/* * ��Image widget�� ;  CNcomment:��Image�ؼ��� */
/**
* brief Set image resource.CNcomment:����ͼƬ��Դ
* param[in] hImage Image handle.CNcomment:�ؼ����
* param[in] Image The resource ID of image.CNcomment:ͼƬ��ԴID
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Image_SetImage(HIGV_HANDLE hImage, HI_RESID Image);

/**
* brief Draw surface image.CNcomment:�����ڴ�ͼƬ
* attention :
* User should call HI_GV_Image_FreeMemSurface to release before call this api.
* CNcomment:�ظ����ô˺���ǰ�����ȵ���HI_GV_Image_FreeMemSurface�ͷ���һ��ͼƬsurface
* �Զ��ػ�ؼ�
* param[in] hImage Image handle.CNcomment:�ؼ����
* param[in] pMemInfo Memory infomation.CNcomment:�ڴ�����
* param[in] ImageHeight Image hegiht.CNcomment:���ý���ͼƬ�߶�
* param[in] ImageWidth Image width.CNcomment:���ý���ͼƬ���
* param[in] pSrcRect Source rect.CNcomment:Image��Դsurface��������,ΪNULLʱ��������ͼƬ
* param[in] pDstRect Destination rect.CNcomment:���Ƶ�Ŀ��surface�ľ�������,ΪNULLʱ�ӿؼ����� 0,0��ʼ����
* param[in] pBlitOpt Bit block transfer operation.CNcomment:���ƻ�ϲ�����������
* param[in] Transparent Transparent.CNcomment:�Ƿ�͸��Ƥ��,HI_TRUE ͸��,HI_FALSE ��͸��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* see :: HI_GV_Image_FreeMemSurface
*/
HI_S32 HI_GV_Image_DrawMemImage(HIGV_HANDLE hImage, HI_GV_MemInfo *MemInfo, HI_U32 ImageHeight, HI_U32 ImageWidth,
                                HI_RECT *pSrcRect, HI_RECT *pDstRect, HIGO_BLTOPT_S *pBlitOpt, HI_BOOL Transparent);


/**
* brief Draw surface picture. CNcomment:����surfaceͼƬ
* param[in] hImage Image handle.CNcomment:�ؼ����
* param[in] hSrcSurface Higo surface handle.CNcomment:HIGO surface���
* param[in] pSrcRect Source rect(null indicate full surface).CNcomment:surface��Դ��������,
* ΪNULLʱ��������surface
* param[in] pDstRect Destination rect(null indicate full surface).CNcomment:
* ���Ƶ�image�ؼ��ľ�������,ΪNULLʱ�ӿؼ�����
* 0,0��ʼ���� \param[in] pBlitOpt Bit block transfer operation.CNcomment:���ƻ�ϲ�����������
* \param[in] Transparent
* Transparent.CNcomment:�Ƿ�͸��Ƥ��,HI_TRUE ͸��,HI_FALSE ��͸��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_Image_DrawSurface(HIGV_HANDLE hImage, HIGV_HANDLE hSrcSurface, HI_RECT *pSrcRect, HI_RECT *pDstRect,
                               HIGO_BLTOPT_S *pBlitOpt, HI_BOOL Transparent);

/**
* brief Release the picture surface.CNcomment:�ͷ��ڴ�ͼƬ��surface
* attention ::
* Release the surface when unload the resouce, and the widget
* will not redraw, so you should send repaint message to higv.
* CNcomment:ж����Դʱ�ؼ��ڲ�ͬ���ͷ�surface
* �ؼ����ػ棬�����ػ��뷢�ͻ�����Ϣ
* param[in] hImage Image handle.CNcomment:�ؼ����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* see :: HI_GV_Image_DrawMemImage
*/
HI_S32 HI_GV_Image_FreeMemSurface(HIGV_HANDLE hImage);

/**
* brief Set bit block transfer operation.CNcomment:����ͼƬ�İ��ƻ�ϲ�������
* param[in] hImage Image handle.CNcomment:�ؼ����
* param[in] pBlitOpt Bit block transfer operation.CNcomment:���ƻ�ϲ�����������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Image_SetBlitOpt(HIGV_HANDLE hImage, HIGO_BLTOPT_S *pBlitOpt);

/**
* brief Get bit block transfer operation.CNcomment:��ȡͼƬ�İ��ƻ�ϲ�������
* param[in] hImage Image handle.CNcomment:�ؼ����
* param[out] pBlitOpt Bit block transfer operation.CNcomment:���ƻ�ϲ�����������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_NOTINIT
*/
HI_S32 HI_GV_Image_GetBlitOpt(HIGV_HANDLE hImage, HIGO_BLTOPT_S *pBlitOpt);

/* ==== API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif
#endif /* __HI_GV_IMAGE_H__ */
