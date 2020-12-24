/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: ScrollGrid widget API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_SCROLLGRID_H__
#define __HI_GV_SCROLLGRID_H__
#include "hi_gv_conf.h"

#ifdef HIGV_USE_WIDGET_SCROLLGRID
/* add include here */
#include "hi_gv_resm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* *  addtogroup      scrollgird     */
/* *  ��scrollgird widget�� ; CNcomment: ��scrollgird�ؼ��� */
#define HIGV_SCROLLGRID_COL_MAX 20

typedef enum {  /* ScrollGrid ; CNcomment: ScrollGrid */
    SCROLLGRID_COLTYPE_TXT = 0, /* text ; CNcomment: �ı� */
    SCROLLGRID_COLTYPE_IMG, /* picture ; CNcomment: ͼƬ */
    SCROLLGRID_COLTYPE_IMG_MEM,  /* memory picture ; CNcomment: �ڴ�ͼƬ */
    SCROLLGRID_COLTYPE_BUTT
} HIGV_SCROLLGRID_COLTYPE_E;

/* * The callback function is uded to convert string */
/* *CNcomment:  �ַ���ת������:��DB�б�����ַ���ת��Ϊ��ʾʱ��Ҫ���ַ��� */
typedef HI_S32 (*HIGV_SCROLLGRID_CONV_CB)(HIGV_HANDLE hScrollGrid, HI_U32 CellCol, HI_U32 Cell,
                                          const HI_CHAR *pSrcStr,
                                          HI_CHAR *pDstStr, HI_U32 Length);

typedef struct _HIGV_SCROLLGRID_COLATTR_S { /* the column attribute structure; CNcomment: ScrollGrid�ؼ������Խṹ */

    HIGV_SCROLLGRID_COLTYPE_E Type; /* The type of column ; CNcomment:CELL�� ������ */
    HI_U32 Top; /* The top position of cell; CNcomment: ���CELL������λ�� */
    HI_U32 Left; /* The top position of cell ; CNcomment: ���CELL�� ����λ�� */
    HI_U32 Width; /* The widht of cell ; CNcomment: CELL�� �п� */
    HI_U32 Height; /* The height of cell ; CNcomment: CELL�� �и� */
    HI_U32 Align; /* The aligment of text ; CNcomment: �ı����뷽ʽ */
    HIGV_HANDLE hImage;  /*  The picture handle(only support the cloumn type is LIST_COLTYPE_IMG)
                            CNcomment: ��Type=LIST_COLTYPE_IMGʱ��Ч����Ҫ��ʾ��ͼƬsurface��� */
    HI_U32 FieldColIdxInDb;  /*The index in database correspoding column in scrollgrid ;
                                CNcomment: ���������ݿ��е��к� */
    HIGV_SCROLLGRID_CONV_CB ConvString;  /* The convet string callback ;
                                        CNcomment: �ַ���ת��������ֻ��LIST_COLTYPE_TXT��Ч */
} HIGV_SCROLLGRID_COLATTR_S;

typedef enum _SCROLLGRID_DIRECTION_E {  /* Scroll direction ; CNcomment: ScrollGrid�ؼ��Ĺ���(����)����ö�� */
    SCROLLGRID_DIRECTION_HORI,      /* <Horizal direction ; CNcomment: ˮƽ�������� */
    SCROLLGRID_DIRECTION_VERT,  /* Vertial direction ; CNcomment:��ֱ�������� */
    SCROLLGRID_DIRECTION_BUTT
} HIGV_SCROLLGRID_DIRECTION_E;

typedef struct _HIGV_SCROLLGRID_STYLE_S {   /*  Layout structure ; CNcomment: scrollgrid�ؼ���񲼾����� */
    HI_U32 RowNum; /* Row number in a page ; CNcomment: ҳ��ʾ���� */
    HI_U32 ColNum; /*  Colum number in a page ; CNcomment: ҳ��ʾ���� */
    HI_U32 LeftMargin; /* left margin of the page ; CNcomment: ҳ��߾� */
    HI_U32 RightMargin; /* Right margin of the page ; CNcomment: ҳ�ұ߾� */
    HI_U32 TopMargin; /* Up margin of the page ; CNcomment: ҳ�ϱ߾� */
    HI_U32 BtmMargin; /* Bottom margin of the page ; CNcomment: ҳ�±߾� */
    HI_U32 RowSpace; /* The space of row ; CNcomment: ҳ�м�� */
    HI_U32 ColSpace; /* The space of column ; CNcomment: ҳ�м�� */
    HI_U32 HLHeight; /* The height of horizontal line ; CNcomment: ˮƽ�����߿�� */
    HI_COLOR HLColor; /* The color of horizontal line ;  CNcomment: ˮƽ��������ɫ */
    HI_U32 VLWidth; /* The width of vertical line ; CNcomment:  ��ֱ�����߿�� */
    HI_COLOR VLColor; /* The color of vertical line ;  CNcomment: ��ֱ��������ɫ */
    HI_U32 FocusRectAnchor; /* The anchor position of focus item ; CNcomment: ������Ŀ����ʱ�����êλ�� */
    HI_U32 IsFocusAtBg; /* The focus is drawn at the background ; CNcomment:���������ڱ���֮�� */
    HI_U32 ImgDecIndex; /* The index of decode picture ; CNcomment:ͼƬ�������� */
    HI_U32 IsDynamicDecImg; /*Is Dynamic decode image ; CNcomment:����Ƿ�̬ͼƬ���� */
    HIGV_SCROLLGRID_DIRECTION_E Direction; /* Scroll direction ; CNcomment: ���췽�� */
    HI_RESID FocusRectSkin;
    HI_RESID FocusRectNormSkin;
} HIGV_SCROLLGRID_STYLE_S;

/*  Structure Definition end */
/* ****************************** API declaration **************************** */
/* * addtogroup      ScrollGrid        */
/* * ��ScrollGrid widget�� ; ��ScrollGrid�ؼ��� */
/**
* brief Initiatlize the scrollgird. CNcomment:ScrollGrid �ؼ���ʼ��
* attention : Set the head of scrollgrid and row num.
* CNcomment:����Grid�ؼ��ı�ͷ(������������)������
* param[in] hScrollGrid    Scrollgrid handle. CNcomment:ScrollGrid�ؼ����
* param[in] pStyle   Scrollgrid handle.CNcomment:�ؼ���񲼾�����
* param[in] pColAttr Scrollgrid handle.CNcomment:����������
* param[in] CellColNum Scrollgrid handle.CNcomment:�������������Ԫ�ظ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_NORES
*/
HI_S32 HI_GV_ScrollGrid_Init(HIGV_HANDLE hScrollGrid, HIGV_SCROLLGRID_STYLE_S *pStyle,
                             const HIGV_SCROLLGRID_COLATTR_S *pColAttr, HI_U32 CellColNum);

/**
* brief Get the content of scrollgrid.CNcomment:��ȡScrollGrid�ؼ����ݸ������
* param[in] hList   Scrollgrid handle.CNcomment:ScrollGrid�ؼ����
* param[in] Cell   Cell. CNcomment:��Ԫ��
* param[in] CellCol The column attribute of cell. CNcomment:��Ԫ���������
* param[out] pValue the conten of cell. CNcomment:��Ԫ�������е�����
* param[in] Length String length. CNcomment:pValue�ַ�������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST Maybe the handle is invaldi or no data.CNcomment:�����Ч���߶Ծ��к���û������
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollGrid_GetCellValue(HIGV_HANDLE hScrollGrid, HI_U32 Cell, HI_U32 CellCol, HI_CHAR *pValue,
                                     HI_U32 Length);

/**
* brief Set the selected cell of the scrollgrid.CNcomment:���� ScrollGrid��ǰ��Ԫ
* attention : This api only support the current selected item.
* CNcomment:�ýӿڽ����õ�ǰѡ����Ŀ
* param[in] hScrollGrid CNcomment:�ؼ����
* param[in] Cell  Cell index.CNcomment:��Ŀ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* see ::HI_GV_ScrollGrid_GetSelCell
*/
HI_S32 HI_GV_ScrollGrid_SetSelCell(HIGV_HANDLE hScrollGrid, HI_U32 Cell);

/**
* brief Get the selected of cell.CNcomment:��ȡScrollGrid��ǰ��Ԫ
* param[in] hScrollGrid  Scrollgrid handle.CNcomment:�ؼ����
* param[out] pCell Cell index. CNcomment:��Ŀ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* see ::HI_GV_ScrollGrid_SetSelCell
*/
HI_S32 HI_GV_ScrollGrid_GetSelCell(HIGV_HANDLE hScrollGrid, HI_U32 *pCell);

/**
* brief Get the cell number of scrollgrid.CNcomment:��ȡScrollGrid��ǰ�����ܵ�Ԫ��
* param[in]  hScrollGrid  Scrollgrid handle. CNcomment:�ؼ����
* param[out] pCellNum Cell number.CNcomment:����Ŀ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollGrid_GetCellNum(HIGV_HANDLE hScrollGrid, HI_U32 *pCellNum);

/**
* brief Page up or page right.CNcomment:���»����ҷ�ҳ
* param[in]  hScrollGrid  Scrollgrid handle. CNcomment:�ؼ����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollGrid_PageForward(HIGV_HANDLE hScrollGrid);

/**
* brief Page up or page left.CNcomment:���ϻ�����ҳ
* param[in]  hScrollGrid  Scrollgrid handle. CNcomment:�ؼ����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollGrid_PageBackward(HIGV_HANDLE hScrollGrid);

/**
* brief Back to origin.CNcomment:���ñ����ƶ�����ʼλ��
* param[in]  hScrollGrid  Scrollgrid handle. CNcomment:�ؼ����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollGrid_MoveToOrigin(HIGV_HANDLE hScrollGrid);

/**
* brief Move to last.CNcomment:���ñ����ƶ����յ�λ��
* param[in]  hScrollGrid  Scrollgrid handle. CNcomment:�ؼ����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollGrid_MoveToLast(HIGV_HANDLE hScrollGrid);

/**
* brief Set the attribute of scrollgird.CNcomment: ��������������
* param[in]  hScrollGrid  ScrollGrid handle.CNcomment: �ؼ����
* param[in]  HLHeight  The height of horizontal line.CNcomment: ˮƽ�߸߶�
* param[in]  HLColor  The color of horizontal line.CNcomment: ˮƽ����ɫ
* param[in]  VLWidth The width of vertical line.CNcomment:  ��ֱ�߿��
* param[in]  VLColor  The color of vertical line.CNcomment: ��ֱ����ɫ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* see ::HI_GV_ScrollGrid_GetGridding
*/
HI_S32 HI_GV_ScrollGrid_SetGridding(HIGV_HANDLE hScrollGrid, HI_U32 HLHeight, HI_COLOR HLColor, HI_U32 VLWidth,
                                    HI_COLOR VLColor);

/**
* brief Get the attribute of scrollgird.CNcomment: ��ȡ����������
* param[in]  hScrollGrid  ScrollGrid handle.�ؼ����
* param[out]  pHLHeight The height of horizontal line. CNcomment: ˮƽ�߸߶�
* param[out]  pHLColor  The color of horizontal line.CNcomment: ˮƽ����ɫ
* param[out]  pVLWidth  The width of vertical line.CNcomment: ��ֱ�߿��
* param[out]  pVLColor  The color of vertical line.CNcomment: ��ֱ����ɫ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* see::HI_GV_ScrollGrid_SetGridding
*/
HI_S32 HI_GV_ScrollGrid_GetGridding(HIGV_HANDLE hScrollGrid, HI_U32 *pHLHeight, HI_COLOR *pHLColor,
                                    HI_U32 *pVLWidth,
                                    HI_COLOR *pVLColor);

/**
* brief Get touch diff.CNcomment:��ȡ�������ƫ�ƾ���
* param[in]  hScrollGrid  Scrollgrid handle. CNcomment:�ؼ����
* param[out]  Diff  diff distance. CNcomment:ƫ�ƾ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollGrid_GetTouchDiff(HIGV_HANDLE hScrollGrid, HI_S32 *Diff);

/**
* brief Set content location.CNcomment:���ÿؼ�����ƫ��
* param[in]  hScrollGrid  Scrollgrid handle. CNcomment:�ؼ����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollGrid_Adjust(HIGV_HANDLE hScrollGrid, HI_S32 Diff);

/**
* brief Set gesture enable.CNcomment:���ÿؼ��Ƿ�֧�ִ���
* param[in]  hScrollGrid  Scrollgrid handle. CNcomment:�ؼ����
* param[in]  bIsEnable   Is enable gesture. CNcomment:�Ƿ�֧�ִ���( HI_TRUE��ʾ֧�֣�HI_FALSE��ʾ��ֹ)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollGrid_EnableGesture(HIGV_HANDLE hScrollGrid, HI_BOOL bIsEnable);

/**
* brief Get gesture enable.CNcomment:��ȡ�ؼ��Ƿ�֧�ִ���
* param[in]  hScrollGrid  Scrollgrid handle. CNcomment:�ؼ����
* param[in]  pIsEnable   Is enable gesture. CNcomment:�Ƿ�֧�ִ���( HI_TRUE��ʾ֧�֣�HI_FALSE��ʾ��ֹ��ϵͳĬ��ΪHI_TRUE)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollGrid_IsGestureEnable(HIGV_HANDLE hScrollGrid, HI_BOOL *pIsEnable);

/**
* brief Register scrollgrid widget.CNcomment:ע��scrollgrid�ؼ�
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_ScrollGrid_RegisterWidget(HI_VOID);

/* ==== API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif /* __HI_GV_LIST_H__ */
#endif
