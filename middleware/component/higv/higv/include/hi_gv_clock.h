/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: Clock API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_CLOCK_H__
#define __HI_GV_CLOCK_H__

/* add include here */
#include "hi_gv_widget.h"
#include <time.h>
#ifdef HIGV_USE_WIDGET_CLOCK
#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* * addtogroup      Clock   */
/* * ��Clock widget��; CNcomment: ��Clock�ؼ��� */
/* *Time structure ; CNcomment:ʱ����Ϣ�ṹ */
typedef struct hiHIG_TIME_S {
    HI_U32 year; /* Year , CNcomment: �� */
    HI_U32 month; /* Month , CNcomment: �� */
    HI_U32 day; /* Day , CNcomment: �� */
    HI_U32 week; /* Week , CNcomment: �� */
    HI_U32 hour; /* Hour , CNcomment: Сʱ */
    HI_U32 minute; /* Minute, CNcomment: �� */
    HI_U32 second; /* Second , CNcomment: �� */
} HIGV_TIME_S;

typedef enum {
    HIGV_CURSORRES_TYPE_COLOR = 0,
    HIGV_CURSORRES_TYPE_IMAGE = 1,
    HIGV_CURSORRES_TYPE_BUTT,
} HIGV_CURSORRES_TYPE_E;

typedef enum {
    HIGV_CLOCK_MODE_TEXT = 0,
    /* User set time infomation by text , CNcomment: �û�ֱ��ͨ���ı�����ʱ�� */
    HIGV_CLOCK_MODE_FORMAT,
    /* User set time infomation by time format , CNcomment: �û�ͨ��ʱ���ʽ��ʽ����ʾʱ�� */
    HIGV_CLOCK_MODE_BUTT,
} HIGV_CLOCK_MODE_E;

typedef struct {
    HIGV_CLOCK_MODE_E
    DispMode; /* display mode ; CNcomment:clock�ؼ��Ƿ������ʾ�ͱ༭���� */
    HIGV_CURSORRES_TYPE_E
    CursorResType; /* The  cursor resource type ; CNcomment:�༭�������Դ���� */
    /* The resource ID of cursor ; CNcomment:clock�ɱ༭���ڵ���ԴID ��ͼƬ����ɫ */
    HI_U32 CursorRes;
} HIGV_CLOCK_STYLE_S;

/* * Structure Definition end */
/* ****************************** API declaration **************************** */
/* * addtogroup      Clock   */
/* * ��Clock widget��; CNcomment: ��Clock�ؼ��� */
/**
* brief Init the clock.CNcomment:��ʼ��clock�ؼ�
* param[in]  hClock Clock handle. CNcomment:�ؼ����
* param[in]  pCreateInfo The info of clock when created. CNcomment:�ؼ�������Ϣ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_EMPTY
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_NORES
*/
HI_S32 HI_GV_Clock_Init(HIGV_HANDLE hClock, HIGV_CLOCK_STYLE_S *pStyle);

/**
* brief Set the colon whether blink.CNcomment:����ʱ��ؼ���λ��ð���Ƿ���˸
* param[in]  hClock Clock handle.CNcomment:�ؼ����
* param[out] bFlash Whether blink.CNcomment:�Ƿ���˸��HI_TRUE��ʾ��˸��HI_FALSE��ʾ����˸
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_Clock_SetFlash(HIGV_HANDLE hClock, HI_BOOL bFlash);

/**
* brief Set time unit of clock widget.CNcomment:����ʱ��ؼ�����С��λ
* attention ::
* CNcomment:
* (1)"second"-�룬"minute"-�֣�"hour"-ʱ��"day"-�գ�"week"-�ܣ�"month"-�£�"year"-��
* (2)���øýӿڻ��Զ�����clock�ؼ�
* param[in]  hClock Clock handle. CNcomment:�ؼ����
* param[out] pUnit  Time unit. CNcomment:���ַ�����ʾ����Сʱ�䵥λ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_EMPTY
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_NORES
*/
HI_S32 HI_GV_Clock_SetTimeUnit(HIGV_HANDLE hClock, const HI_CHAR *pUnit);

/**
* brief Control the clock run state.CNcomment:����ʱ��ؼ���ʼ�������ʱ
* param[in]  hClock Clock handle. CNcomment:�ؼ����
* param[out] bRun  Start or stop. CNcomment:ʱ�ӿؼ����л�ֹͣ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_EMPTY  CNcomment:�û�û��������С����ʱ�䵥λ
*/
HI_S32 HI_GV_Clock_Run(HIGV_HANDLE hClock, HI_BOOL bRun);

/**
* brief Clear the content of clock.CNcomment:���ʱ�ӿؼ�������
* param[in]  hClock Clock handle.�ؼ����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_EMPTY
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_NORES
*/
HI_S32 HI_GV_Clock_ClearContent(HIGV_HANDLE hClock);

/**
* brief Set UTC time.CNcomment:����Clock�ؼ�UTCʱ��
* param[in]  hClock Clock handle. CNcomment:�ؼ����
* param[in]  t UTC. CNcomment:���õ�UTCʱ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_EMPTY
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_NORES
*/
HI_S32 HI_GV_Clock_SetUTC(HIGV_HANDLE hClock, time_t t);

/**
* brief Get UTC time.CNcomment:��ȡClock�ؼ�UTCʱ��
* param[in]  hClock Clock handle. CNcomment:�ؼ����
* param[out]  pt The pointer of UTC. CNcomment:��ȡ��UTCʱ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_EMPTY
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_NORES
*/
HI_S32 HI_GV_Clock_GetUTC(HIGV_HANDLE hClock, time_t *pt);

/**
* brief Set the item switch step.CNcomment:������Ŀ�л�����
* attention : Only support the year item.CNcomment:ֻ��YEAR��Ŀ��Ч
* param[in]  hClock Clock handle. CNcomment:�ؼ����
* param[in]  Step The step of switch item. CNcomment:�л�����
* param[in]  Interval The interval time of switch item CNcomment:���ʱ��(Interval = 0, default:100ms)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_EMPTY
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_NORES
*/
HI_S32 HI_GV_Clock_SetSwitchItemStep(HIGV_HANDLE hClock, HI_U32 Step, HI_U32 Interval);

/*  API declaration end */
#ifdef __cplusplus
}
#endif
#endif
#endif /* __HI_GV_SPIN_H__ */
