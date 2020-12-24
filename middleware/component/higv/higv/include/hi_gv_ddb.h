/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: hi_gv_ddb.h
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_DDB_H__
#define __HI_GV_DDB_H__

/* add include here */
#include "hi_type.h"
#include "hi_gv_adm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* * addtogroup      DDB  */
/* * ��DDB�� ; CNcomment: �����ݻ��桿 */

/* *Insert position ; CNcomment:�������� */
typedef enum {
    DDB_INSERT_PRE = 0,
    /*  Insert before,   CNcomment: �������֮ǰ */
    DDB_INSERT_NEXT,
    /*  Insert after,   CNcomment: �������֮�� */
    DDB_INSERT_BUTT
} HIGV_DDBINSERT_E;

/* *  ==== Structure Definition end ==== */
/* ****************************** API declaration **************************** */
/* * addtogroup      DDB  */
/* * ��DDB�� ;  CNcomment:�����ݻ��桿 */
/**
* brief Create a DDB.CNcomment:����һ�����ݻ���ʵ��
* param[in] FieldCount Field count.CNcomment:�ֶθ���
* param[in] pFieldAttr Field attribute.CNcomment:�ֶ�����
* param[out] phDDB    DDB handle. CNcomment:���ݻ�����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_DDB_Create(HI_U32 FieldCount, const HIGV_FIELDATTR_S *pFieldAttr, HIGV_HANDLE *phDDB);

/**
* brief Destroy a DDB.CNcomment:ɾ��һ�����ݻ���ʵ��
* param[in] hDDB DDB handle.CNcomment:���ݻ�����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_DDB_Destroy(HIGV_HANDLE hDDB);

/**
* brief Insert a recoder to the databuffer.CNcomment:�����ݻ����в���һ����¼
* param[in] hDDB DDB handle.CNcomment:���ݻ�����
* param[in] Row Row position.CNcomment:��ʶ�������ݵ�λ��
* param[in] pData The pointer of recoder.CNcomment:��¼�����ݣ�ע��pData->size����С�ڸ������ݳ���֮��
* param[in] eInsert The postion of insert.CNcomment:��ʶ�������ݵ�λ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_DDB_NULLPTR
* retval ::HI_ERR_DDB_INVAIDPARA
*/
HI_S32 HI_GV_DDB_Insert(HIGV_HANDLE hDDB, HI_U32 Row, const HIGV_DBROW_S *pData, HIGV_DDBINSERT_E eInsert);

/**
* brief Modify a recoder of data buffer.CNcomment:�޸����ݻ����е�һ����¼
* param[in] hDDB DDB handle.CNcomment:���ݻ�����
* param[in] Row Row index.CNcomment:Ҫ�޸ĵ��к�
* param[in] pdata Recoder data.CNcomment:��¼�����ݣ�ע��pData->size����С�ڸ������ݳ���֮��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_DDB_NULLPTR
* retval ::HI_ERR_DDB_OUTOFRANGE
*/
HI_S32 HI_GV_DDB_Modify(HIGV_HANDLE hDDB, HI_U32 Row, const HIGV_DBROW_S *pData);

/**
* brief Add a recoder to the tail of data buffer.CNcomment:�����ݻ���β�����һ����¼
* param[in] DDB handle.CNcomment:hDDB ���ݻ�����
* param[in] pData Recoder data.CNcomment:��¼�����ݣ�ע��pData->size����С�ڸ������ݳ���֮��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_DDB_NULLPTR
* retval ::HI_ERR_DDB_BUFFSMALL
*/
HI_S32 HI_GV_DDB_Append(HIGV_HANDLE hDDB, const HIGV_DBROW_S *pData);

/**
* brief Clear the recoder in the data buffer.CNcomment:������ݻ����еļ�¼
* param[in] hDDB DDB handle.
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_DDB_Clear(HIGV_HANDLE hDDB);

/**
* brief Delete a recoder by key.CNcomment:����keyֵɾ��һ����¼
* param[in] hDDB DDB handle.CNcomment:���ݻ�����
* param[in] Row Row index.CNcomment:Ҫ������¼���к�
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_DDB_OUTOFRANGE
*/
HI_S32 HI_GV_DDB_Del(HIGV_HANDLE hDDB, HI_U32 Row);

/**
* brief Get the recoder by row number.CNcomment:�����кŻ�ȡһ����¼
* param[in] hDDB DDB handle.CNcomment:���ݻ�����
* param[in] Row The row of recoder.CNcomment:Ҫ������¼��λ��
* param[out] pData Recoder data.CNcomment:��¼������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_DDB_NULLPTR
* retval ::HI_ERR_DDB_OUTOFRANGE
*/
HI_S32 HI_GV_DDB_GetRow(HIGV_HANDLE hDDB, HI_U32 Row, HIGV_DBROW_S *pData);

/**
* brief Get n rows form the DDB.CNcomment:��ָ���г���ȡn�е�����
* param[in] hDDB DDB handle.���ݻ�����
* param[in] Row  Row index.CNcomment:Ҫ��ȡ���ݵ���ʼ�к�
* param[in] Num  Row number.CNcomment:Ҫ��ȡ��������
* param[out] pData    Data cache.CNcomment:���ݻ���
* param[out] pRowNum  Row number.CNcomment:ʵ�ʻ�ȡ������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_DDB_NULLPTR
* retval ::HI_ERR_DDB_OUTOFRANGE
* retval ::HI_ERR_DDB_INVAIDPARA
*/
HI_S32 HI_GV_DDB_GetRows(HIGV_HANDLE hDDB, HI_U32 Row, HI_U32 Num, HI_VOID *pData, HI_U32 *pRowNum);

/**
* brief Get the column index of data.CNcomment:���кŻ�ȡ����
* param[in]  hDDB DDB handle.CNcomment:���ݻ�����
* param[in]  Row Row index.CNcomment:�к�
* param[in]  Col Column index.CNcomment:�к�
* param[out] pFieldData CNcomment: �ֶ�buffer
* param[in]  Len Buffer size.CNcomment:�ֶ�buffer��С
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_DDB_NULLPTR
* retval ::HI_ERR_DDB_OUTOFRANGE
* retval ::HI_ERR_DDB_BUFFSMALL
*/
HI_S32 HI_GV_DDB_GetCellData(HIGV_HANDLE hDDB, HI_U32 Row, HI_U32 Col, HI_VOID *pFieldData, HI_U32 Len);

/**
* brief Set the column index of data.CNcomment:���к���������
* param[in]  hDDB DDB handle.CNcomment:���ݻ�����
* param[in]  Row Row index.CNcomment:�к�
* param[in]  Col Column index.CNcomment:�к�
* param[in]  pFieldData CNcomment: �ֶ�buffer
* param[in]  Len Buffer size.CNcomment:�ֶ�buffer��С
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_DDB_NULLPTR
* retval ::HI_ERR_DDB_OUTOFRANGE
* retval ::HI_ERR_DDB_OUTOFRANGE
*/
HI_S32 HI_GV_DDB_SetCellData(HIGV_HANDLE hDDB, HI_U32 Row, HI_U32 Col, HI_VOID *pFieldData, HI_U32 Len);

/**
* brief Get the total row number.CNcomment:��ȡ������
* param[in] hDDB DDB handle.CNcomment:Ĭ�����ݻ�����
* param[out] pRowCount Row count.CNcomment:������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_DDB_NULLPTR
*/
HI_S32 HI_GV_DDB_GetRowCount(HIGV_HANDLE hDDB, HI_U32 *pRowCount);

/**
* brief Register the function when the data change.CNcomment:ע�����ݱ仯֪ͨ����
* param[in] hDDB DDB handle.CNcomment:Ĭ�����ݻ�����
* param[in] hADM The object which notify.CNcomment:��Ҫ֪ͨ�Ķ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_DDB_RegisterDataChange(HIGV_HANDLE hDDB, HIGV_HANDLE hADM);

/**
* brief Unregister the function when data change.CNcomment:ȡ�����ݱ仯֪ͨ
* param[in] hDDB DDB handle.CNcomment:Ĭ�����ݻ�����
* param[in] hADM The object which notify.CNcomment:��Ҫ֪ͨ�Ķ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_DDB_UnRegisterDataChange(HIGV_HANDLE hDDB, HIGV_HANDLE hADM);

/**
* brief Register callback when data change.CNcomment:���ݱ仯֪ͨʹ�ܿ���
* param[in] hDDB  DDB handle. CNcomment: ���ݻ�����
* param[in] bEnable Enable flag. CNcomment:���ݱ仯֪ͨʹ�ܿ��أ�HI_TRUE��ʾʹ�ܣ�HI_FALSE��ʾȥʹ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_DDB_EnableDataChange(HIGV_HANDLE hDDB, HI_BOOL bEnable);

/* *==== API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif /* __HI_GV_DDB_H__ */
