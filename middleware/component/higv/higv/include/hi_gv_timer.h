/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: Time moudle API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_TIMER_H__
#define __HI_GV_TIMER_H__

/* add include here */
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* * The timer Max number */
#define TIMER_MAX_NUM 0x40

/* ****************************** API declaration **************************** */
/* * addtogroup      Timer   */
/* * ��Timer moudle��  ����ʱ��ģ�顿 */
/**
* brief Create timer.CNcomment:������ʱ��
* param[in] hWidget Widget handle.CNcomment:���ڻ�ؼ����
* param[in] TimerID Timer ID. CNcomment:��ʱ��ID
* param[in] Speed  Timer interval. CNcomment:��ʱ���������msΪ��λ
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_EXIST
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_NORES
* see ::HI_GV_Timer_Destroy
*/
HI_S32 HI_GV_Timer_Create(HIGV_HANDLE hWidget, HI_U32 TimerID, HI_U32 Speed);

/**
* brief Destory timer.CNcomment:���ٶ�ʱ��
* param[in] hWidget Widget handle.CNcomment:���ڻ�ؼ����
* param[in] TimerID Timer ID.CNcomment:��ʱ��ID
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_NOOP Couldn't stop timer.CNcomment:����ֹͣ��ʱ��
* see ::HI_GV_Timer_Create
*/
HI_S32 HI_GV_Timer_Destroy(HIGV_HANDLE hWidget, HI_U32 TimerID);

/**
* brief Start timer.CNcomment:������ʱ��
* param[in] hWidget Widget handle. CNcomment:���ڻ�ؼ����
* param[in] TimerID  Timer ID.CNcomment:��ʱ��ID
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_NOOP CNcomment:���ö�ʱ��ģ��������ʱ��ʧ��
* see ::HI_GV_Timer_Stop
*/
HI_S32 HI_GV_Timer_Start(HIGV_HANDLE hWidget, HI_U32 TimerID);

/**
* brief Stop timer.CNcomment:ֹͣ��ʱ��
* param[in] hWidget Widget handle. CNcomment:���ڻ�ؼ����
* param[in] TimerID  Timer ID.CNcomment:��ʱ��ID
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_NOOP Couldn't stop timer.CNcomment:����ֹͣ��ʱ��
* see::HI_GV_Timer_Start
*/
HI_S32 HI_GV_Timer_Stop(HIGV_HANDLE hWidget, HI_U32 TimerID);

/**
* brief Reset timer.CNcomment:��λ��ʱ������ʱ�����ֿ�ʼ��ʱ
* param[in] hWidget  Widget handle.CNcomment:���ڻ�ؼ����
* param[in] TimerID  Timer ID.CNcomment:��ʱ��ID
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_NOOP Couldn't stop timer. CNcomment:����ֹͣ��ʱ��
*/
HI_S32 HI_GV_Timer_Reset(HIGV_HANDLE hWidget, HI_U32 TimerID);

/**
* brief Set timer speed.CNcomment:���ö�ʱ���ٶ�
* param[in] hWidget  Widget handle.CNcomment:���ڻ�ؼ����
* param[in] TimerID  Timer ID.CNcomment:��ʱ��ID
* param[in] Speed  Timer speed(unit: ms).CNcomment:��ʱ��Speed (ms)
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Timer_SetSpeed(HIGV_HANDLE hWidget, HI_U32 TimerID, HI_U32 Speed);

/* ==== API declaration end ==== */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __HI_GV_TIMER_H__ */
