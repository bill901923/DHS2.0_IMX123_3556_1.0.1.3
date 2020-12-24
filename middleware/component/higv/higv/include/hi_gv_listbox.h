/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: Listbox wiget API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_LIST_H__
#define __HI_GV_LIST_H__
#include "hi_gv_conf.h"
#ifdef HIGV_USE_WIDGET_LISTBOX
/* add include here */
#include "hi_gv_resm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* * addtogroup      Listbox     */
/* *  ��listbox widget��; CNcomment:  ��listbox�ؼ��� */
/* * Max col number */
#define HIGV_LISTBOX_COL_MAX 20

/* * Type of column ; CNcomment: ListBox�ؼ�������ö�� */
typedef enum {
    LIST_COLTYPE_TXT = 0,
    /* Text ; CNcomment:  �ı� */
    LIST_COLTYPE_IMG,
    /* Picture ; CNcomment:  ͼƬ */
    LIST_COLTYPE_TXTICONLEFT,
    /* Text and icon,icon on left ; CNcomment:  �ı���ͼ�꣬ͼ������ */
    LIST_COLTYPE_TXTICONRIGHT,
    /* Text and icon,icon on right ; CNcomment:  �ı���ͼ�꣬ͼ������ */
    LIST_COLTYPE_BUTT
} HIGV_LIST_COLTYPE_E;

/* * Callback function of convert string ; CNcomment: �ַ���ת������:��DB�б�����ַ���ת��Ϊ��ʾʱ��Ҫ���ַ��� */
typedef HI_S32 (*HIGV_LIST_CONV_CB)(HIGV_HANDLE hList, HI_U32 Col, HI_U32 Item, const HI_CHAR *pSrcStr,
                                    HI_CHAR *pDstStr, HI_U32 Length);

/* At cell foucs mode, the callback function is used to get the cell width */
/* CNcomment:
* ��Ԫ�񽹵�ģʽ�£��ؼ�������ȡ��Ԫ���ȵĻص�������HIGV�Ὣ������
* Ҫ��ʾ�ĵ�Ԫ����к������δ��룬���ݷ���ֵ0-100�����õ�Ԫ���ȡ�0��
* ʾ���ص�Ԫ��100��ʾ�˵�Ԫ��ռ��һ�С�����ֵ����0-100�ڵĲ��Կ����
* ������
*/
typedef HI_S32 (*HIGV_GET_WIDTH_CB)(HIGV_HANDLE hList, HI_U32 Item, HI_U32 Col);

/* * Attribute structure of listbox Column ; CNcomment: Listbox�ؼ������Խṹ */
typedef struct {
    HIGV_LIST_COLTYPE_E Type; /*  Column type ; CNcomment: ������ */
    HI_U32 Width; /*  Column width ; CNcomment: �п� */
    HI_U32 Align; /*  Alignment text ; CNcomment: �ı����뷽ʽ */
    HI_COLOR Fgidx; /*  Font color of column text; CNcomment: ���ı�������ɫ */
    HIGV_HANDLE hImage;  /* Picture surface handle (only support type is LIST_COLTYPE_IMG)
                            CNcomment:  ��Type=LIST_COLTYPE_IMGʱ��Ч����Ҫ��ʾ��ͼƬsurface��� */
    HI_U32 FieldColIdxInDb; /* Column index in db ; CNcomment:  ���������ݿ��е��к� */
    HIGV_LIST_CONV_CB ConvString;  /* function which convert string (only support type is LIST_COLTYPE_TXT)
                                    CNcomment:  �ַ���ת��������ֻ��LIST_COLTYPE_TXT��Ч */
} HIGV_LIST_COLATTR_S;

/* * Attribute structure of listbox ;  CNcomment: Listbox�ؼ����Խṹ */
typedef struct {
    HI_U32 RowNum; /* Row number ; CNcomment:  ����Ŀ */
    HI_U32 ColNum; /* Column number ; CNcomment:  ����Ŀ */
    HI_BOOL NoFrame; /* Is no frame Draw ; CNcomment:  �Ƿ��ޱ߿� */
    HI_BOOL Cyc; /* Is cycle ; CNcomment:  �Ƿ�ѭ������ */
    HI_BOOL IsCellActive; /* Is cell active ; CNcomment:  �Ƿ�Ԫ�񽹵� */
    HI_BOOL AutoSwitchCell; /* If cell active auto suwich active cell ; CNcomment:  ��Ԫ�񽹵��Ƿ��Զ��л����� */
    HI_BOOL Scroll; /* Is scroll ; CNcomment:  �Ƿ������Ԫ����ʾ */
    HI_BOOL Fromleft; /* Is Scroll from left ; CNcomment:  �Ƿ����߿�ʼ���� */
    HI_U32 Timeinterval; /* Time interval ; CNcomment:  ����ʱ���� */
    HI_U32 Step; /* Scroll step ; CNcomment:  ������Ԫ��Ĺ������� */
    HI_U32 ScrollCol; /* If item active,scroll col of item ; CNcomment:  ��Ŀ����ʱ�ɹ����� */
    HIGV_GET_WIDTH_CB GetWidthCb;  /*  Function get cell width from call back ;
                                    CNcomment:  �ؼ��ڲ��ص��û����õ�Ԫ���Ⱥ��� */
    HIGV_LIST_COLATTR_S *pColAttr;  /* List column attributes array ;
                                        CNcomment:  �б��ؼ������Խṹ���飬��ColNum��Ԫ�� */
} HIGV_LIST_ATTRIBUTE_S;

/* *  Structure Definition end */
/* ****************************** API declaration **************************** */
/* *  addtogroup      Listbox     */
/* *  ��listbox widget�� CNcomment:  ��listbox�ؼ��� */
/**
* brief Init the listbox.CNcomment: ListBox�ؼ���ʼ��
* attention ::
* CNcomment: Set the head of listbox.CNcomment: ����listbox�ؼ��ı�ͷ(������������)������
* param[in] hList    CNcomment: ListBox�ؼ����
* param[in] RowNum  Row number of dispaly. CNcomment: ��ʾ����
* param[in] ColNum   Column number.CNcomment: ����
* param[in] pColAttr Column attribute array.CNcomment: ����������(����ColNum��Ԫ��)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_NORES
*/
HI_S32 HI_GV_List_Init(HIGV_HANDLE hList, HI_U32 RowNum, HI_U32 ColNum, const HIGV_LIST_COLATTR_S *pColAttr);

/* * addtogroup      Listbox     */
/* * ��listbox widget�� ; CNcomment:  ��listbox�ؼ��� */
/**
* brief Init the listbox.CNcomment: ListBox�ؼ���ʼ��
* attention ::
* CNcomment: Set the head of listbox.CNcomment: ����listbox�ؼ��ı�ͷ(������������)������
* param[in] hList    CNcomment: ListBox�ؼ����
* param[in] pListAttr  Init listbox attributes. CNcomment: ��ʼ���б������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_NORES
*/
HI_S32 HI_GV_List_InitEx(HIGV_HANDLE hList, const HIGV_LIST_ATTRIBUTE_S *pListAttr);

/**
* brief Get the content of listbox.CNcomment: ��ȡListBox�ؼ����ݸ������
* param[in] hList   Listbox handle.CNcomment: ListBox�ؼ����
* param[in] Item  Item index. CNcomment: ��
* param[in] Col   Column index. CNcomment: ��
* param[out] pValue Cell's content.CNcomment: ��Ԫ������
* param[in] Length Length of pValue.CNcomment: pValue�ַ�������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST CNcomment: �����Ч���߶Ծ��к���û������
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_List_GetCell(HIGV_HANDLE hList, HI_U32 Item, HI_U32 Col, HI_CHAR *pValue, HI_U32 Length);

/**
* brief Set skin fo focus item.CNcomment: ���ý����е�Ƥ��
* param[in] hList    Listbox handle. CNcomment: �ؼ����
* param[in] SelSkin   The skin of Selected status.CNcomment: �ؼ����ڽ���״̬��ѡ����Ƥ��
* param[in] NormSkin  The skin of normal status.CNcomment: �ؼ����ڷǽ���״̬��ѡ����Ƥ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_List_SetRowSkin(HIGV_HANDLE hList, HIGV_HANDLE SelSkin, HIGV_HANDLE NormSkin);

/**
* brief Set selected item of listbox.CNcomment: ���� ListBox��ǰ��Ŀ
* attention ::The api is only support the selected item and apply this api at the init stage.
* CNcomment: �ýӿڽ����õ�ǰѡ����Ŀ����ˢ�¿ؼ�����Ҫ�ڳ�ʼ��Listbox�ؼ�ʱʹ��
* param[in] hList Listbox handle. CNcomment: �ؼ����
* param[in] Item  Item index.CNcomment: ��Ŀ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_PAERM Foucs moe error.CNcomment: ����ģʽ����
* see :: HI_GV_List_GetSelItem
*/
HI_S32 HI_GV_List_SetSelItem(HIGV_HANDLE hList, HI_U32 Item);

/**
* brief Get the item of listbox.CNcomment: ��ȡListBox��ǰ��Ŀ
* param[in] hList  Listbox handle.CNcomment: �ؼ����
* param[out] pItem Item index.CNcomment: ��Ŀ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* see :: HI_GV_List_SetSelItem
*/
HI_S32 HI_GV_List_GetSelItem(HIGV_HANDLE hList, HI_U32 *pItem);

/**
* brief Get the total item number of listbox.CNcomment: ��ȡListBox��ǰ����������
* param[in]  hList  Listbox handle. CNcomment:  �ؼ����
* param[out] pItemNum Toal item number.CNcomment: ����Ŀ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_List_GetItemNum(HIGV_HANDLE hList, HI_U32 *pItemNum);

/**
* brief CNcomment: �Ƿ���ListBox�ؼ�����
* param[in]  hList  CNcomment: �ؼ����
* retval ::HI_TRUE
* retval ::HI_FALSE
*/
HI_BOOL HI_GV_List_IsListBoxType(HIGV_HANDLE hList);

/**
* brief Set the attribute of listbox.CNcomment: ������������
* param[in]  hList  Listbox handle.CNcomment: �ؼ����
* param[in]  HLHeight  The height of horizontal line.CNcomment: ˮƽ�߸߶�
* param[in]  HLColor  The color of horizontal line.CNcomment: ˮƽ����ɫ
* param[in]  VLWidth The width of vertical line.CNcomment:  ��ֱ�߿��
* param[in]  VLColor  The color of vertical line.CNcomment: ��ֱ����ɫ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* see ::HI_GV_List_GetGridding
*/
HI_S32 HI_GV_List_SetGridding(HIGV_HANDLE hList, HI_U32 HLHeight, HI_COLOR HLColor, HI_U32 VLWidth,
                              HI_COLOR VLColor);

/**
* brief Get the attribute of listbox.CNcomment: ��ȡ��������
* param[in]  hList  Listbox handle.�ؼ����
* param[out]  pHLHeight The height of horizontal line. CNcomment: ˮƽ�߸߶�
* param[out]  pHLColor  The color of horizontal line.CNcomment: ˮƽ����ɫ
* param[out]  pVLWidth  The width of vertical line.CNcomment: ��ֱ�߿��
* param[out]  pVLColor  The color of vertical line.CNcomment: ��ֱ����ɫ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* see ::HI_GV_List_SetGridding
*/
HI_S32 HI_GV_List_GetGridding(HIGV_HANDLE hList, HI_U32 *pHLHeight, HI_COLOR *pHLColor, HI_U32 *pVLWidth,
                              HI_COLOR *pVLColor);

/**
* brief Change the image of column.CNcomment: �ı���ʾ��ͼƬ
* param[in] hList Listbox handle.CNcomment: �ؼ����
* param[in] Col    Column index.CNcomment: �к�
* param[in] hImage Picture handle.CNcomment: ͼƬ����������Ϊ��Ч����������֮ǰ���õ���Ч���
* retval N/A.CNcomment:��
*/
HI_S32 HI_GV_List_ChangeImage(HIGV_HANDLE hList, HI_U32 Col, HIGV_HANDLE hImage);

/* brief Get the start index of current page. CNcomment: ��ȡ��ǰ��ʾҳ���������� */
HI_S32 HI_GV_List_GetStartItem(HIGV_HANDLE hList, HI_U32 *Item);

/* brief Set the start index of current page. CNcomment: ���õ�ǰ��ʾҳ���������� */
HI_S32 HI_GV_List_SetStartItem(HIGV_HANDLE hList, HI_U32 Item);

/* brief Get the index of the end item.CNcomment: ��ȡ��ǰ��ʾҳ��ĩ������ */
HI_S32 HI_GV_List_GetEndItem(HIGV_HANDLE hList, HI_U32 *Item);

/**
* brief Set the foucus of listbox.CNcomment: ����ListBox��ǰ���㵥Ԫ��
* attention : The api is only support the selected cell.
* CNcomment: �ýӿڽ����õ�ǰѡ�е�Ԫ�񣬲�ˢ�¿ؼ�
* param[in] hList  Listbox handle. CNcomment: ListBox�ؼ����
* param[in] Item   Row index.CNcomment: ��
* param[in] Col    Column index.CNcomment: ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_PAERM Error at focus mode.CNcomment: ����ģʽ����
* see ::HI_GV_List_GetSelCell
*/
HI_S32 HI_GV_List_SetSelCell(HIGV_HANDLE hList, HI_U32 Item, HI_U32 Col);

/**
* brief Get the position of cell which selected.CNcomment: ��ȡListBox��ǰѡ�е�Ԫ��λ��
* param[in] hList  Listbox handle. CNcomment: ListBox�ؼ����
* param[out] pItem   row index.CNcomment: ��
* param[out] pCol   Column index. CNcomment: ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* see ::HI_GV_List_SetSelCell
*/
HI_S32 HI_GV_List_GetSelCell(HIGV_HANDLE hList, HI_U32 *pItem, HI_U32 *pCol);

/**
* brief Get the width of cell at focus mode.CNcomment: ��Ԫ�񽹵�ģʽ�»�ȡ��Ԫ����
* attention : if the cell not display, it will return 0.
* CNcomment: δ��ʾ�ĵ�Ԫ�񷵻�0
* param[in] hList  Listbox handle. CNcomment: ListBox�ؼ����
* param[in] Item   item index.CNcomment: ��
* param[in] Col    Column index.CNcomment: ��
* param[out] pWidth  The pointer of width.CNcomment: ��ȡ��ȵ�ָ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_List_GetCellWidth(HIGV_HANDLE hList, HI_U32 Item, HI_U32 Col, HI_U32 *pWidth);

/**
* brief Set scroll step of text.CNcomment: �����ı��Ĺ�������
* attention : it is only valid at foucs mode.
* CNcomment: ��Ԫ�񽹵�ģʽ����Ч
* param[in] hList Listbox handle.CNcomment: �ؼ����
* param[in] Step Scroll step(unit:pixel, default:10piexl).CNcomment: ��������(��pixelΪ��λ��Ĭ��Ϊ10pixel)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_PAERM Error at focus mode.CNcomment: ����ģʽ����
*/
HI_S32 HI_GV_List_SetStep(HIGV_HANDLE hList, HI_U32 Step);

/**
* brief Set the scroll direction of listbox.CNcomment: ���ù�������
* attention : it is only valid at foucs mode.
* CNcomment: ��Ԫ�񽹵�ģʽ����Ч
* param[in] hList Listbox handle.CNcomment: �ؼ����
* param[in] FromLeft Scroll direction.CNcomment: ��������Ĭ��HI_FALSE��������,����HI_TRUE��ʾ�������ҹ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_PAERM Error at focus mode.CNcomment: ����ģʽ����
*/
HI_S32 HI_GV_List_SetDirection(HIGV_HANDLE hList, HI_BOOL FromLeft);

/**
* brief  Set the status of widget.CNcomment: ���ÿؼ�״̬
* attention : it is only valid at foucs mode.
* CNcomment: ��Ԫ�񽹵�ģʽ����Ч
* param[in] hList Listbox handle.CNcomment: �ؼ����
* param[in] Scroll Scroll status.CNcomment: ����״̬(TRUEΪ������FALSEΪֹͣ������Ĭ��ΪFALSE)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_PAERM Error at focus mode.CNcomment: ����ģʽ����
*/
HI_S32 HI_GV_List_SetScroll(HIGV_HANDLE hList, HI_BOOL Scroll);

/**
* brief Set the foreground color of column.CNcomment: ������ǰ����ɫ
* attention :it is only support the column type is LIST_COLTYPE_TXT,
* if col is -1, it will effect the color of text to all column .
* CNcomment: �ı�����ΪLIST_COLTYPE_TXTʱ��Ч,ColΪ-1ʱ����������������ɫ
* param[in] hList Listbox handle.CNcomment: �ؼ����
* param[in] Col   Column index.CNcomment: ��
* param[in] FgIdx Foreground color index.CNcomment: ��ɫֵ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_PAERM Unsupport the column to set picture.CNcomment: ��֧������ͼƬ��
*/
HI_S32 HI_GV_List_SetColFgIdx(HIGV_HANDLE hList, HI_U32 Col, HI_COLOR FgIdx);

/**
* brief Set the exterior line of listbox.CNcomment: �����Ƿ�����������
* param[in] hList Listbox handle.CNcomment: �ؼ����
* param[in] NoFrame Is not draw.CNcomment: �Ƿ����(TRUE���棬FALSE�棬Ĭ��ΪFALSE)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_List_SetNoFrame(HIGV_HANDLE hList, HI_BOOL NoFrame);

/**
* brief Set the foucs whether cycle.CNcomment: �����Ƿ�ѭ������
* attention :It is only valid at focus mode.
* CNcomment: ��Ŀ����ģʽ����Ч
* param[in] hList Listbox handle.CNcomment: �ؼ����
* param[in] Cyc Focus cycle.CNcomment: TRUEѭ����FALSE��ѭ����Ĭ��ΪFALSE
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_List_SetCyc(HIGV_HANDLE hList, HI_BOOL Cyc);

/**
* brief Set scroll param.CNcomment:���û���������ϵ��(Ĭ��Ϊ1.0�����ڵ���1.0)
\attention \n
N/A.CNcomment:��
* param[in] hList Listbox handle.    CNcomment:�ؼ����
* param[in] ScrollParam scroll index.    CNcomment:����������ϵ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_List_SetScrollParam(HIGV_HANDLE hList, HI_FLOAT ScrollParam);

/**
* brief Set fling param.CNcomment:������ɨ������ϵ��(Ĭ��Ϊ2�����ڵ���1)
* param[in] hList Listbox handle.    CNcomment:�ؼ����
* param[in] FlingParam fling index.    CNcomment:��ɨ������ϵ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_List_SetFlingParam(HIGV_HANDLE hList, HI_S32 FlingParam);

/* API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif
#endif /* __HI_GV_LIST_H__ */
