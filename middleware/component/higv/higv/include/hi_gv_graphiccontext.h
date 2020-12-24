/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: graphiccontext moudle API
 * Author: NULL
 * Create: 2014-12-23
*/

#ifndef __HI_GV_GRAPHICCONTEXT_H__
#define __HI_GV_GRAPHICCONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

HI_S32 HI_GV_GraphicContext_Create(HIGV_HANDLE hWidget, HIGV_HANDLE *phGC);

/*
* brief Destroy GraphicContext.CNcomment:���ٻ��ƻ���
* attention
* param[in] hGC GraphicContext handle.CNcomment:GC���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* HI_GV_GraphicContext_Destroy
*/
HI_S32 HI_GV_GraphicContext_Destroy(HIGV_HANDLE hGC);

/*
* brief Begin draw,add the default clip rect .CNcomment:��ʼ���ƣ����Ĭ�ϵļ��о���
* param[in] hGC GraphicContext handle.CNcomment:GC���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_LOST
* see
* HI_GV_GraphicContext_End
*/
HI_S32 HI_GV_GraphicContext_Begin(HIGV_HANDLE hGC);

/*
* brief End draw,empty clip rect.CNcomment:�������ƣ�ɾ�����о���
* param[in] hGC GraphicContext handle.CNcomment:GC���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* see : HI_GV_GraphicContext_Begin
*/
HI_S32 HI_GV_GraphicContext_End(HIGV_HANDLE hGC);

/*
* brief Add clip rect.CNcomment:��Ӽ��о���
* Keep old clip rect,add new clip rect.CNcomment:����ԭ�м��о�����Ӿ���
* param[in] hGC GraphicContext handle.CNcomment:GC���
* param[in] pRect Clip rect.CNcomment:�������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_GraphicContext_AddClipRect(HIGV_HANDLE hGC, const HI_RECT *pRect);

/*
* brief Reset clip rect.CNcomment:������о���
* Clean old clip rect,reset new clip rect.CNcomment:ɾ��ԭ�м��о�������¾���
* param[in] hGC GraphicContext handle.CNcomment:GC���
* param[in] pRect Clip rect.CNcomment:�������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_GraphicContext_SetClipRect(HIGV_HANDLE hGC, const HI_RECT *pRect);

/*
* brief Decode a locale picture.CNcomment:���뱾��ͼƬ
* If ImgWidth or ImgHeight is 0,it indicates that the original picture attributes are retained.
* CNcomment:���������Ϊ�㣬ͼƬ��СΪĬ�ϴ�С
* param[in] pFileName Picture path.CNcomment:ͼƬ·��
* param[in] ImgWidth Image surface width.CNcomment:�����surface���
* param[in] ImgHeight Image surface height.CNcomment:�����surface�߶�
* param[out] phImgSurface Image surface handle.CNcomment:�����õ�surface���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_LOST
* retval ::HIGO_ERR_NULLPTR
* retval ::HI_FAILURE
* retval ::HI_NULL
* retval ::HIGO_ERR_INVIMGDATA
* see::
* hi_go_decoder.h
* HI_GV_GraphicContext_FreeImageSurface
*/
HI_S32 HI_GV_GraphicContext_DecodeImg(const HI_CHAR *pFileName, HI_U32 ImgWidth, HI_U32 ImgHeight,
                                      HIGV_HANDLE *phImgSurface);

/*
* brief Decode memory picture.CNcomment:�����ڴ�ͼƬ
* If ImgWidth or ImgHeight is 0,it indicates that the original picture attributes are retained.
* CNcomment:���������Ϊ�㣬ͼƬ��СΪĬ�ϴ�С
* param[in] pAddr The memory picture address.CNcomment:�ڴ�ͼƬ��ַ
* param[in] Lenth The memory lenth.CNcomment:�ڴ泤��
* param[in] ImgWidth Image surface width.CNcomment:�����surface���
* param[in] ImgHeight Image surface height.CNcomment:�����surface�߶�
* param[out] phImgSurface Image surface handle.CNcomment:�����õ�surface���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_LOST
* retval ::HIGO_ERR_NULLPTR
* retval ::HI_FAILURE
* retval ::HI_NULL
* retval ::HIGO_ERR_INVIMGDATA
* see::
* hi_go_decoder.h
* HI_GV_GraphicContext_FreeImageSurface
*/
HI_S32 HI_GV_GraphicContext_DecodeMemImg(HI_CHAR *pAddr, HI_U32 Length, HI_U32 ImgWidth, HI_U32 ImgHeight,
                                         HIGV_HANDLE *phImgSurface);

/*
* brief Destroy image surface.CNcomment:�ͷŽ���ͼƬsurface
* param[in] hImgSurface Image surface handle.CNcomment:ͼƬsurface���
* retval ::HI_SUCCESS
* retval ::HIGO_ERR_NOTINIT
* retval ::HIGO_ERR_INVHANDLE
* retval ::HIGO_ERR_INUSE
* see ::
* HI_GV_GraphicContext_DecodeImg
* HI_GV_GraphicContext_DecodeMemImg
*/
HI_S32 HI_GV_GraphicContext_FreeImageSurface(HIGV_HANDLE hImgSurface);

/*
* brief Set foreground color. CNcomment:����ǰ��ɫ
* param[in] hGC GraphicContext handle.CNcomment:GC���
* param[in] FgColor Foreground color.CNcomment:ǰ��ɫ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_GraphicContext_SetFgColor(HIGV_HANDLE hGC, HI_COLOR FgColor);

/*
* brief Set background color. CNcomment:���ñ���ɫ
* Set text background colorN/A.CNcomment:�������ֵı���ɫ
* param[in] hGC GraphicContext handle.CNcomment:GC���
* param[in] BgColor text background color.CNcomment:���ֱ���ɫ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_GraphicContext_SetBgColor(HIGV_HANDLE hGC, HI_COLOR BgColor);

/*
* brief Draw line .CNcomment:����
* The (x0,y0,x1,y1) >= 0 && < Widget width.
* param[in] hGC GraphicContext handle.CNcomment:GC���
* param[in] x0,y0 Start point.CNcomment:���
* param[in] x1,y1 End point.CNcomment:�յ�
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_INVSIBLE
* retval ::HIGO_ERR_UNSUPPORTED
* retval ::HIGO_ERR_INVPIXELFMT
* retval ::HIGO_ERR_DEPEND_TDE
*/
HI_S32 HI_GV_GraphicContext_DrawLine(HIGV_HANDLE hGC, HI_U32 x0, HI_U32 y0, HI_U32 x1, HI_U32 y1);

/*
* brief Draw surface picture. CNcomment:����surfaceͼƬ
* param[in] hGC GraphicContext handle.CNcomment:GC���
* param[in] pDstRect Destination rect(null indicate full surface).CNcomment:���Ƶ�image�ؼ��ľ�������,ΪNULLʱ�ӿؼ�����
* 0,0��ʼ���� \param[in] hImage Image surface handle.CNcomment:��� \param[in] pSrcRect Source rect(null indicate full
* surface).CNcomment:surface��Դ��������,ΪNULLʱ��������surface \param[in] pBlitOpt Bit block transfer
* operation.CNcomment:���ƻ�ϲ�����������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_INVSIBLE
*/
HI_S32 HI_GV_GraphicContext_DrawImage(HIGV_HANDLE hGC, HI_RECT *pDstRect, HIGV_HANDLE hImage, HI_RECT *pSrcRect,
                                      HIGO_BLTOPT_S *pBlitOpt);

/*
* brief Draw string. CNcomment:�����ı�
* param[in] hGC GraphicContext handle.CNcomment:GC���
* param[in] pText String.CNcomment:�����ı�����
* param[in] pRect Paint rect on the bind widget surface.CNcomment:�󶨿ؼ�surface�Ļ��ƾ���
* param[in] Align Alignment of text.CNcomment:�ı����뷽ʽ
* HIGV_ALIGN_RIGHT
* HIGV_ALIGN_HCENTER
* HIGV_ALIGN_TOP
* HIGV_ALIGN_BOTTOM
* HIGV_ALIGN_VCENTER
* HIGV_ALIGN_WRAP
* HIGV_ALIGN_WORDELLIPSIS
* HIGV_ALIGN_FORCE_LEFT
* HIGV_ALIGN_FORCE_RIGHT
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_INVSIBLE
* retval ::HIGO_ERR_NULLPTR
*/
HI_S32 HI_GV_GraphicContext_DrawText(HIGV_HANDLE hGC, const HI_CHAR *pText, const HI_RECT *pRect, HI_U32 Align);

/*
* brief Draw text by string ID. CNcomment:���ƶ������ִ�
* param[in] hGC GraphicContext handle.CNcomment:GC���
* param[in] StrID String ID.CNcomment:�������ִ�ID
* param[in] pRect Paint rect on the bind widget surface.CNcomment:�󶨿ؼ�surface�Ļ��ƾ���
* param[in] Align Alignment of text.CNcomment:�ı����뷽ʽ
* HIGV_ALIGN_RIGHT
* HIGV_ALIGN_HCENTER
* HIGV_ALIGN_TOP
* HIGV_ALIGN_BOTTOM
* HIGV_ALIGN_VCENTER
* HIGV_ALIGN_WRAP
* HIGV_ALIGN_WORDELLIPSIS
* HIGV_ALIGN_FORCE_LEFT
* HIGV_ALIGN_FORCE_RIGHT
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_INVSIBLE
* retval ::HIGO_ERR_NULLPTR
*/
HI_S32 HI_GV_GraphicContext_DrawTextByID(HIGV_HANDLE hGC, const HI_U32 StrID, const HI_RECT *pRect, HI_U32 Align);

/*
* brief Set text font. CNcomment:�����ı�����
* param[in] hGC GraphicContext handle.CNcomment:GC���
* param[in] hFont Font handle.CNcomment:������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_GraphicContext_SetFont(HIGV_HANDLE hGC, HIGV_HANDLE hFont);

/*
* brief Fill rect. CNcomment:�������
* param[in] hGC GraphicContext handle.CNcomment:GC���
* param[in] pRect Fill rect on the bind widget surface.CNcomment:�ڰ�surface����������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_INVSIBLE
* see :: HI_GV_GraphicContext_SetBgColor
*/
HI_S32 HI_GV_GraphicContext_FillRect(HIGV_HANDLE hGC, const HI_RECT *pRect);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __HI_GV_GRAPHICCONTEXT_H__ */
