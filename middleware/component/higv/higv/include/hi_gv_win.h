/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: Window widget API
 * Author: NULL
 * Create: 2009-2-9
 */

#ifndef __HI_GV_WIN_H__
#define __HI_GV_WIN_H__

/* add include here */
#include "hi_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ****************************** API declaration **************************** */
/* * addtogroup      Window   */
/* *  ��Window widget�� CNcomment: �����ڿؼ��� */

/**
* brief Set window opacity.CNcomment:���ô��ڿɼ���
* param[in] hWindow Window handle.CNcomment:���ھ��
* param[in] Opacity Window opacity.CNcomment:���ڿɼ���
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Win_SetOpacity(HIGV_HANDLE hWindow, HI_U8 Opacity);

/**
* brief Get first highlight child widget of widnow CNcomment:��ȡ���ڵ�һ�������ĺ��Ӿ��
* attention :This api is uded to get the hightlight widget for softkeyboard.
* CNcomment:��Ҫ��Ӧ����������ϻ�ȡ��ǰ�����ؼ�
* param[in] hWindow Window handle.CNcomment:���ھ��
* param[out] phHilghtChild Hightlight child.CNcomment:��������
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Win_GetHilightChild(HIGV_HANDLE hWindow, HIGV_HANDLE *phHilghtChild);

/**
* brief Reset all hightlight child and hightlight the first child.CNcomment:��λ�����еĸ������ӣ������ڵ�һ���ӿؼ�����
* attention:This API is used to redisplay the softkeyboard.
* CNcomment:��Ҫ��Ӧ��������ʾ�����ʱ��λ�����
* param[in] hWindow Window handle.CNcomment:���ھ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Win_ResetHilightChild(HIGV_HANDLE hWindow);

/**
* brief Sync show win and get widget handle of done this show.CNcomment:ͬ����ʾwindow����ȡ������ʾ�Ŀؼ����
* attention:Return the second arg of HI_GV_Win_EndSyncShow.CNcomment:����HI_GV_Win_EndSyncShow�ӿڵĵڶ�������
* param[in] hWindow Window handle.CNcomment:���ھ��
* retval ::HIGV_HANDLE
* retval ::HI_FAILURE
* see : HI_GV_Win_EndSyncShow
*/
HI_S32 HI_GV_Win_SyncShow(HIGV_HANDLE hWindow);

/**
* brief Done the sync show.CNcomment:��������ͬ����ʾ
* param[in] hWindow Window handle.CNcomment:���ھ��
* param[in] hWidget Widget handle of end window sync show .CNcomment:����ͬ����ʾʱ���صĿؼ����
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
* retval ::HI_ERR_COMM_NOOP
* see : HI_GV_Win_SyncShow
*/
HI_S32 HI_GV_Win_EndSyncShow(HIGV_HANDLE hWindow, HIGV_HANDLE hWidget);

/**
* brief Get window level.CNcomment:��ȡ���ڵȼ�
* param[in] hWindow Window handle.CNcomment:���ھ��
* param[out] pLevel Window level.CNcomment:�����Ĵ��ڵȼ�
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Win_GetWinLevel(HIGV_HANDLE hWindow, HI_U32 *pLevel);

/**
* brief Advance window and child to load skin.CNcomment:���ش��ڼ����ӿؼ��ĵ�Ƥ��
* attention : HIGV_STYLE_HIDE_UNLODRES style .CNcomment:�������ͷ���Դ���ʱ��Ч
* param[in] hWindow Window handle.CNcomment:���ھ��
* retval ::HI_SUCCESS
* retval ::HI_ERR_COMM_LOST
*/
HI_S32 HI_GV_Win_LoadSkin(HIGV_HANDLE hWindow);
/* *  ==== API declaration end ==== */
#ifdef __cplusplus
}
#endif
#endif /* __HI_GV_WIN_H__ */
