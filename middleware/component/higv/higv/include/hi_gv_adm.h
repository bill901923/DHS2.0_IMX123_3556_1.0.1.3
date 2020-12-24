/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: ADM API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_ADM_H__
#define __HI_GV_ADM_H__

#include "hi_type.h"
#include "hi_gv_widget.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* * ADM  */
/* *  ��abstract data model��; CNcomment: ��ADMģ�顿 */

/* *ADM and DDB support  MAX filed number ; CNconmment: ���ݿ��Ĭ�ϻ���֧�ֵ�����ֶθ��� */
#define HIGV_DB_MAX_FIELDNUM      (64)
#define HIGV_DB_MAX_FIELD_STR_LEN (4096)

/* * The type of data are supported.; CNcomment:֧�ֵ��������Ͷ��� */
typedef enum {
    HIGV_DT_S8 = 0, /* char */
    HIGV_DT_U8,     /* unsigned char */
    HIGV_DT_S16,    /* short */
    HIGV_DT_U16,    /* unsigned short */
    HIGV_DT_S32,    /* int */
    HIGV_DT_U32,    /* unsigned int */
    HIGV_DT_S64,    /* long long */
    HIGV_DT_U64,    /* unsigned long long */
    HIGV_DT_F32,    /* float */
    HIGV_DT_D64,    /* double */
    HIGV_DT_STRING, /* char * */
    HIGV_DT_HIMAGE, /* image handle */
    HIGV_DT_STRID,  /* multi-langugae  string ID */
    HIGV_DT_BUTT
} HIGV_DT_E;

/* * The type of data change. ; CNconmment:���ݱ仯������ */
typedef enum {
    HIGV_DC_INSERT = 0,
    /* *< insert data ; CNcomment:���� */
    HIGV_DC_MODIFY,
    /* *< modify data ; CNcomment:�޸� */
    HIGV_DC_DELETE,
    /* *< delete data ; CNcomment:ɾ�� */
    HIGV_DC_UPDATE,
    /* *< updata data,sealed operations ; CNcomment:���ݱ仯������δ֪ */
    HIGV_DC_CLEAR,
    /* *< clear all the data ; CNcomment:����ȫ������ */
    HIGV_DC_DESTROY,
    /* *< destroy the data source ; CNcomment:����Դ���� */
    HIGV_DC_BUTT
} HIGV_DC_E;

/* * The data change info.  ; CNconmment:���ݱ仯��Ϣ */
typedef struct {
    HIGV_DC_E Action; /**< the type of data change */     /**< CNcomment:�仯�������� */
    HI_U16 StartRow; /**< the start row of data change */ /**< CNcomment:�仯����ʼ�� */
    HI_U16 Rows; /**< rows aggregate have data change */ /**< CNcomment:�仯�������� */
} HIGV_DCINFO_S;

/* * Cell attribute.CNconmment:�ֶ����� */
typedef struct hiHIGV_CELLATTR_S {
    HIGV_DT_E eDataType; /* *< data type ; CNcomment:�������� */
    /* *< max field str len, value must less than ::HIGV_DB_MAX_FIELD_STR_LEN */
    /* *< CNcomment:��󳤶ȣ���С����С��::HIGV_DB_MAX_FIELD_STR_LEN */
    HI_U32 MaxSize;
} HIGV_FIELDATTR_S;

/* * Row deferable.CNcomment: �����ݴ洢 */
typedef struct hiHIGV_DBROW_S {
    HI_U32 Size; /* data size  , CNcomment:���ݳ��� */
    HI_VOID *pData; /* data pointer , CNcomment:���� */
} HIGV_DBROW_S;

typedef HI_S32 (*GetCountFunc)(HIGV_HANDLE DBSource, HI_U32 *RowCnt);
typedef HI_S32 (*GetRowValueFunc)(HIGV_HANDLE DBSource, HI_U32 Row, HI_U32 Num, HI_VOID *pData,
               HI_U32 *pRowNum);
typedef HI_S32 (*RegisterDataChangeFunc)(HIGV_HANDLE DBSource, HIGV_HANDLE hADM);
typedef HI_S32 (*UnregisterDataChangeFunc)(HIGV_HANDLE DBSource, HIGV_HANDLE hADM);

typedef struct hiADM_OPT_S {
    HI_U32 DBSource; /* *< data source ; CNcomment:����Դ��ʶ */
    HI_U32 FieldCount; /* *< data source field count ; CNcomment:����Դ���ֶ����� */
    HIGV_FIELDATTR_S *pFieldAttr; /* *< field attribute ; CNcomment:����Դ��ÿ���ֶ����� */
    HI_U32 BufferRows; /* *< rows at buffer ; CNcomment:ϣ���Ļ������� */
    GetCountFunc GetCount; /* * get rows count function. ; CNcomment:��ȡ�����ӿ� */
    GetRowValueFunc GetRowValue;
    /* * get rows value from  appoint rows function. ; CNcomment:��ָ���д���ȡn������ */
    RegisterDataChangeFunc RegisterDataChange;
    /* * Register the function when data change ; CNcomment:ע�����ݱ仯֪ͨ�ӿ� */
    UnregisterDataChangeFunc UnregisterDataChange;
    /* * Unregister the function when data change. ; CNcomment:ȡ�����ݱ仯֪ͨ�ӿ� */
} HIGV_ADMOPT_S;

/* * ==== Structure Definition end ==== */
/* ****************************** API declaration **************************** */
/* * addtogroup      ADM  */
/* *��abstract data model��; CNcomment: ��ADMģ�顿 */
/**
* brief CNcomment:Creates a abstract data model example. CNcomment:����һ����������ģ��ʵ��
* param[in] pDataSource CNcomment: ����Դ��������
* param[out] phADM  ADM handle.CNcomment: ����ģ�;��
* retval ::HI_SUCCESS
* retval ::HI_ERR_ADM_NULLPTR
* retval ::HI_ERR_ADM_ZEROFIELD
* retval ::HI_ERR_ADM_OUTOFRANGE
* retval ::HI_ENORES
*/
HI_S32 HI_GV_ADM_Create(HIGV_ADMOPT_S *pDataSource, HIGV_HANDLE *phADM);

/**
* brief Create ADM by handle.CNcomment:����ָ����Handle����һ����������ģ��ʵ������XML2Binʹ��
* param[in] pDataSource operation of datasouce.CNcomment:����Դ��������
* param[in] hADM   ADM handle.CNcomment: ����ģ�;��
* retval ::HI_SUCCESS
* retval ::HI_ERR_ADM_NULLPTR
* retval ::HI_ERR_ADM_ZEROFIELD
* retval ::HI_ERR_ADM_OUTOFRANGE
* retval ::HI_ENORES
*/
HI_S32 HI_GV_ADM_CreateByHandle(HIGV_ADMOPT_S *pDataSource, HIGV_HANDLE hADM);

/**
* brief Create DDB by default.CNcomment:����Ĭ�ϵ����ݻ��崴������ģ��
* param[in] hDB DB handle.CNcomment:Ĭ�ϵ����ݻ�����
* param[in] FieldCount Field count.CNcomment:�ֶθ���
* param[in] pFieldAttr  Field attribute.CNcomment:�ֶ�����
* param[out] phADM ADM handle.CNcomment:����ģ�;��
* retval ::HI_SUCCESS
* retval ::HI_ERR_ADM_NULLPTR
* retval ::HI_ERR_ADM_ZEROFIELD
* retval ::HI_ERR_ADM_OUTOFRANGE
* retval ::HI_ENORES
*/
HI_S32 HI_GV_ADM_CreateDefault(HIGV_HANDLE hDB, HI_U32 FieldCount, HIGV_FIELDATTR_S *pFieldAttr,
                               HIGV_HANDLE *phADM);

/**
* brief Create default DDB by handle.CNcomment:ͨ��ָ����Handle������Ĭ�ϵ����ݻ��崴������ģ��
* param[in] hDB  DB handle.CNcomment:Ĭ�ϵ����ݻ�����
* param[in] FieldCount Field count.CNcomment:�ֶθ���
* param[in] pFieldAttr  Field attribute.CNcomment:�ֶ�����
* param[in] hADM ADM handle.CNcomment:����ģ�;��
* retval ::HI_SUCCESS
* retval ::HI_ERR_ADM_NULLPTR
* retval ::HI_ERR_ADM_OUTOFRANGE
* retval ::HI_ENORES
*/
HI_S32 HI_GV_ADM_CreateDefaultByHandle(HIGV_HANDLE hDB, HI_U32 FieldCount, HIGV_FIELDATTR_S *pFieldAttr,
                                       HIGV_HANDLE hADM);

/**
* brief Destroy ADM.CNcomment:ɾ��һ������ģ��ʵ��
* param[in] hADM ADM handle.CNcomment:����ģ�;��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ADM_Destroy(HIGV_HANDLE hADM);

/**
* brief Bind adm to widget.CNcomment:��һ���ؼ��󶨵�����ģ��
* param[in] hWidget Widget handle.CNcomment:�ؼ����
* param[in] hADM ADM handle.CNcomment:����ģ�;��
* retval ::HI_SUCCESS
*/
HI_S32 HI_GV_ADM_Bind(HIGV_HANDLE hWidget, HIGV_HANDLE hADM);

/**
* brief Unbinde adm to widget.CNcomment:���һ���ؼ�������ģ�͵İ�
* param[in] hWidget Widget handle.CNcomment:�ؼ����
* param[in] hADM ADM handle.CNcomment:����ģ�;��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ADM_UnBind(HIGV_HANDLE hWidget, HIGV_HANDLE hADM);

/**
* brief Get the data type of field. CNcomment:��ȡָ���ֶε���������
* param[in] hADM ADM handle.CNcomment:����ģ�;��
* param[in] Col Column index.CNcomment:�к�
* param[out] pFieldAttr Field attribute.CNcomment:�ֶ�����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_ADM_NULLPTR
* retval ::HI_ERR_ADM_OUTOFRANGE
*/
HI_S32 HI_GV_ADM_GetFieldAttr(HIGV_HANDLE hADM, HI_U32 Col, HIGV_FIELDATTR_S *pFieldAttr);

/**
* brief Get total row number.CNcomment:��ȡ����������
* param[in] hADM ADM handle.CNcomment:����ģ�;��
* param[out] pCount Row count.CNcomment:��������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_ADM_NULLPTR
*/
HI_S32 HI_GV_ADM_GetRowCount(HIGV_HANDLE hADM, HI_U32 *pCount);

/**
* brief Get the data by row and column index.CNcomment:��ȡָ����ָ���е�����
* param[in] hADM ADM handle.CNcomment:����ģ�;��
* param[in] Row Row index.CNcomment:�к�
* param[in] Col Column index.CNcomment:�к�
* param[out] pData CNcomment:��������
* param[in] Len pData CNcomment:�������ɵ��ֽ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_ADM_NULLPTR
* retval ::HI_ERR_ADM_OUTOFRANGE
* retval ::HI_ERR_ADM_BUFFSMALL
* retval ::HI_ERR_ADM_GETDATA
*/
HI_S32 HI_GV_ADM_GetCellData(HIGV_HANDLE hADM, HI_U32 Row, HI_U32 Col, HI_VOID *pData, HI_U32 Len);

/**
* brief Get the data of column.CNcomment:��ȡָ����ָ���е����ݣ����ַ�����ʾ
* param[in] hADM ADM handle. CNcomment:����ģ�;��
* param[in] Row Row index.CNcomment:�к�
* param[in] Col Column index.CNcomment:�к�
* param[out] pDataString Data content.CNcomment:��������
* param[in] Len pDataString buffer size.CNcomment:pDataString�������ɵ��ֽ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_ADM_NULLPTR
* retval ::HI_ERR_ADM_OUTOFRANGE
* retval ::HI_ERR_ADM_BUFFSMALL pData
* retval ::HI_ERR_ADM_GETDATA
*/
HI_S32 HI_GV_ADM_GetCellDataString(HIGV_HANDLE hADM, HI_U32 Row, HI_U32 Col, HI_CHAR *pDataString, HI_U32 Len);

/**
* brief Set the callback function when data change.CNcomment:�������ݱ仯�Ļص�����
* param[in] hADM ADM handle.CNcomment:����ģ�;��
* param[in] CustomProc notify callback of data change.CNcomment:���ݱ仯֪ͨ�ص�����
* param[in] ProcOrder callback of time choice.CNcomment:�ص���ʱ��ѡ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ADM_SetDCCallBack(HIGV_HANDLE hADM, HIGV_MSG_PROC CustomProc, HIGV_PROCORDER_E ProcOrder);

/**
* brief Get the DDB handle by ADM handle.CNcomment:����ADM��ȡDDB
* param[in] ADM handle.CNcomment:hADM  ����ģ�;��
* param[out] phDDB DDB handle.CNcomment:DDB���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_ADM_NULLPTR
*/
HI_S32 HI_GV_ADM_GetDDBHandle(HIGV_HANDLE hADM, HIGV_HANDLE *phDDB);

/**
* brief Get column number.CNcomment:��ȡ������
* param[in] hADM ADM handle.CNcomment:����ģ�;��
* param[out] pColNum colunm count.CNcomment:������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_ADM_NULLPTR
*/
HI_S32 HI_GV_ADM_GetColNum(HIGV_HANDLE hADM, HI_U32 *pColNum);

/**
* brief Sync between  ADM and DB. CNcomment:ADM��DBͬ���ӿ�,���ô˽ӿں��Ժ��ȡ�����ݾ��������ݿ�ͬ����
* param[in] ADM handle.CNcomment:hADM ����ģ�;��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ADM_Sync(HIGV_HANDLE hADM);

/**
* brief Clear all cache data in the ADM.CNcomment:�������ADM�еĻ�������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_ADM_ClearAllData(HI_VOID);

/* * ==== API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif /* __HI_GV_ADM_H__ */
