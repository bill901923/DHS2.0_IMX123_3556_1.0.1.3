/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: hi_gv_app.h
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_APP_H__
#define __HI_GV_APP_H__

/* add include here */
#include "hi_type.h"
#include "hi_gv_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ************************** Structure Definition *************************** */
/* * addtogroup      TaskManager     */
/* * ��Task manager  �� CNcomment: ����������� */

/* *Key hook go on ; CNcomment:�����������´��� */
#define HI_GV_KEYHOOK_GOON 0x1000
/* *Key hook stop ; CNcomment:����ֹͣ���´��� */
#define HI_GV_KEYHOOK_STOP 0x1001

/* *The max static handle number for app ; CNcomment: �ṩӦ��ʹ�õľ�̬������� */
#define HIGV_RESERVE_STATIC_HANDLE_NUM 1000

typedef enum {
    COMM_LEVEL_EMERGENCY = 0, /*  emergency event */
    COMM_LEVEL_NOTIFY,        /*  notify event */
    COMM_LEVEL_BUTT
} COMM_LEVEL_E;

typedef struct {
    HI_U32 id;          /*  message sequence id */
    HI_U32 msg;         /*  message type */
    COMM_LEVEL_E level; /*  message level */
} COMM_MSG_S;

typedef void (*COMM_MSG_HANDLER)(COMM_MSG_S *pmsg);
typedef void (*COMM_ERR_HANDLER)(void);

typedef enum {
    HIGV_VSYNC_HW = 0, /*  hardware vsync signal */
    HIGV_VSYNC_SW,     /*  software vsync signal */
    HIGV_VSYNC_BUTT
} HIGV_VSYNC_E;

/* ==== Structure Definition end ==== */
/* ****************************** API declaration **************************** */
/* * addtogroup      TaskManager     */
/* * ��Task manager  ��; CNcomment: ����������� */
/**
* brief Callback of key hook.CNcomment:�������ӻص�����
* param[in] MsgId  Message ID.CNcomment:��ϢID��MSG_KEYDOWN��MSG_KEYUP
* param[in] Param1 Paramater 1, key value.CNcomment:��Ϣ����1������ֵ
* param[in] Param2 Paramater 2, key status.CNcomment:��Ϣ����2�������״̬(Ԥ��)
* retval ::HI_SUCCESS
* retval ::HI_GV_KEYHOOK_GOON
* retval ::HI_GV_KEYHOOK_STOP
*/
typedef HI_S32 (*PTR_KEYHOOK_CallBack)(HI_U32 MsgId, HI_PARAM Param1, HI_PARAM Param2);

/**
* brief Create application.CNcomment:����HiGVӦ�ó���ʵ��
* attention : A application correspoding a message.
* CNcomment:һ��Ӧ�ó���ʵ����Ӧһ����Ϣѭ��
* param[in] pName  Application name.CNcomment:��Ӧ�ó�����������Ϊ��
* param[in] pApp Application handle. CNcomment:��Ӧ�ó���ʵ�����
* retval ::HI_SUCCESS
* retval ::HI_ERR_MSGM_VTOPMSGCREATE
* retval ::HI_ERR_COMM_NORES
* see :: HI_GV_App_Destroy
*/
HI_S32 HI_GV_App_Create(const HI_CHAR *pName, HIGV_HANDLE *pApp);

/**
* brief Start application.CNcomment:����Ӧ�ó���
* attention : The app will not accept any message if you not start the app.
* CNcomment:δ����֮ǰ�޷��յ��κ���Ϣ
* param[in] hApp Application handle.CNcomment:Ӧ�ó���ʵ�����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_NORES
* see :: HI_GV_App_Stop
*/
HI_S32 HI_GV_App_Start(HIGV_HANDLE hApp);

/**
* brief Stop the application.CNcomment:ֹͣӦ�ó�����Ϣѭ��
* param[in] hApp Application instance.CNcomment:Ӧ�ó���ʵ�����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* see :: HI_GV_App_Start
*/
HI_S32 HI_GV_App_Stop(HIGV_HANDLE hApp);

/**
* brief Destroy the application.CNcomment:����Ӧ�ó���ʵ��
* param[in] hApp CNcomment:Ӧ�ó���ʵ�����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_TASK_TASKNOTSTOP
* see:: HI_GV_App_Create
*/
HI_S32 HI_GV_App_Destroy(HIGV_HANDLE hApp);

/**
* brief Acitve higv app to display on screen.CNcomment: ����APP��ʹ�ø�APP������ʾ����Ļ��
* When Mutile higv app start up on the same time, This only suport one active app display on the sceen.
* retval ::HI_SUCCESS
* retval ::HI_FAILURE
* see:: HI_GV_App_DeActive
*/
HI_S32 HI_GV_App_Active(HI_VOID);

/**
* brief Enabel app run in background.CNcomment:���� Ӧ�ú�̨����
* retval ::HI_SUCCESS
* retval ::HI_FAILURE
* see :: HI_GV_App_DeActive
*/
HI_S32 HI_GV_App_DeActive(HI_VOID);

/**
* brief Get the app wether active CNcomment:��ȡӦ���Ƿ���ǰ̨����״̬
* retval ::HI_SUCCESS
* retval ::HI_FAILURE
* see:
* HI_GV_App_Active
* HI_GV_App_DeActive
*/
HI_BOOL HI_GV_App_IsActive(HI_VOID);

/**
* brief Init HIGV.CNcomment:��ʼ��HiGV
* retval ::HI_SUCCESS
* retval ::HI_ERR_MSGM_VTOPMSGSERVER
* retval ::HI_ERR_COMM_OTHER
* see :: HI_GV_Deinit
*/
HI_S32 HI_GV_Init(HI_VOID);

/**
* brief Deinit HIGV.CNcomment:ȥ��ʼ��HiGV
* see::HI_GV_Init
*/
HI_VOID HI_GV_Deinit(HI_VOID);

/**
* brief Set key hook.CNcomment:���ü��̹��ӻص�����
* param[in] fnKeyCallBack Callback.CNcomment: �ص�����
* retval ::HI_SUCCESS
*/
HI_S32 HI_GV_SetKeyHook(PTR_KEYHOOK_CallBack fnKeyCallBack);

/**
* brief Set the max value of handle.CNcomment:���þ�ָ̬����Handle�����ֵ��ֻ��ָ��һ��
* param[in] hMaxHandle Handle CNcomment:���ֵ
* retval ::HI_SUCCESS
* retval ::HI_FAILURE
*/
HI_S32 HI_GV_SetMaxAssignHandle(HIGV_HANDLE hMaxHandle);

/* brief Get the reserved start static handle. CNcomment: ��ȡ�ؼ������������ľ�̬handle��ʼֵ */
HIGV_HANDLE HI_GV_GetReserveStartStaticHandle(HI_VOID);

/**
* brief Get the version of HIGV. CNcomment:��ȡHIGV�İ汾��
* param[out] pfVersion HIGV Version.CNcomment:HIGV�İ汾�ţ��ɱ���svn�汾�� +  ����ʱ����ɵĸ�����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_FAILURE
*/
HI_S32 HI_GV_GetVersion(HI_DOUBLE *pfVersion);

/**
* brief Set the type of  vsync. CNcomment:����VSYNC���ź�Դ����
* param[in] vsyncType  vsync type.CNcomment: vsync�ź�Դ����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
* retval ::HI_ERR_COMM_PAERM
*/
HI_S32 HI_GV_SetVsyncType(HIGV_VSYNC_E vsyncType);

/**
* brief Get the type of  vsync. CNcomment:��ȡVSYNC���ź�Դ����
* retval ::HIGV_VSYNC_HW
* retval ::HIGV_VSYNC_SW
*/
HIGV_VSYNC_E HI_GV_GetVsyncType(HI_VOID);

/**
* brief Set the period of software vsync. CNcomment:�������VSYNC���ź�����
* param[in] s64Period  Sw vsync period.CNcomment:VSYNC����,��λus
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_SetSWVsyncPeriod(HI_S64 s64Period);

/**
* brief Get the period of software vsync. CNcomment:��ȡ���VSYNC���ź�����
* param[out] ps64Period Sw vsync period.CNcomment:VSYNC����ָ��,��λus
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_GetSWVsyncPeriod(HI_S64 *ps64Period);

/**
* brief Set vsync lost frame policy. CNcomment:����VSYNC��֡����
* param[in] bIsLostFrame Is lost frame.CNcomment:�Ƿ�֡��HI_TRUE��ʾ��֡���ԣ�HI_FALSE��ʾ����֡����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_SetVsyncLostFrame(HI_BOOL bIsLostFrame);

/**
* brief Is vsync lost frame. CNcomment:��ȡVSYNC�Ķ�֡����
* retval ::HI_TRUE��ʾ��֡
* retval ::HI_FALSE��ʾ����֡
*/
HI_BOOL HI_GV_IsVsyncLostFrame(HI_VOID);

/**
* brief Set the threshold of lost frame. CNcomment:����VSYNC��֡����ֵ
* param[in] u32Threshold Lost frame threshold.CNcomment:��֡����ֵ,��λus
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_SetLostFrameThreshold(HI_U32 u32Threshold);

/**
* brief Get the threshold of lost frame. CNcomment:��ȡVSYNC��֡����ֵ
* param[out] pu32Threshold Lost frame threshold.CNcomment:��֡����ֵ,��λus
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_GetLostFrameThreshold(HI_U32 *pu32Threshold);

/**
* brief Sync the render cmd. CNcomment:Render����ͬ���ӿڣ�ִ������ӿڻ�������ֱ��Render����ִ����ɡ�
* retval ::HI_SUCCESS
*/
HI_S32 HI_GV_RenderCmdSync(HI_VOID);

/**
* brief Set render refresh cmd combine. CNcomment:����Refresh�����Ƿ�ϲ�
* param[in] bIsCombine Is combine.CNcomment:�Ƿ�ϲ���HI_TRUE��ʾ�ϲ���HI_FALSE��ʾ���ϲ�
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_INVAL
*/
HI_S32 HI_GV_SetRefreshCombine(HI_BOOL bIsCombine);

/**
* brief Is refresh cmd combine. CNcomment:��ȡRefresh�����Ƿ�ϲ�״̬
* retval ::HI_TRUE��ʾ�ϲ�
* retval ::HI_FALSE��ʾ���ϲ�
*/
HI_BOOL HI_GV_IsRefreshCombine(HI_VOID);

/* * -- API declaration end */

#ifdef __cplusplus
}
#endif
#endif /* __HI_GV_APP_H__ */
