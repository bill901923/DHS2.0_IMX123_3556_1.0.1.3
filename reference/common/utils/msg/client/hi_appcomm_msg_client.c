/**
 * @file    hi_appcomm_msg_client.c
 * @brief   appcomm msg client function.
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/26
 * @version   1.0

 */
#include <pthread.h>
#include <errno.h>
#include <sys/prctl.h>
#include "hi_ipcmsg.h"
#include "hi_appcomm_msg_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** rename module name */
#ifdef HI_MODULE
#undef HI_MODULE
#endif
#define HI_MODULE "MSG"

HI_List_Head_S g_stMSGProcList = HI_LIST_INIT_HEAD_DEFINE(g_stMSGProcList);
HI_S32 g_s32MsgFd = -1;
HI_S32 g_s32MsgFd2 = -1;

extern HI_VOID MSG_Handler(HI_S32 s32Id, HI_IPCMSG_MESSAGE_S* pstMsg);

static HI_VOID* MSG_CLIENT_Run(HI_VOID* arg)
{
    
    MLOGI("in MSG_CLIENT_Run %d\n",g_s32MsgFd);
    pthread_detach(pthread_self());
    prctl(PR_SET_NAME, __func__, 0, 0, 0);
    HI_IPCMSG_Run(g_s32MsgFd);
    MLOGI("out MSG_CLIENT_Run %d\n",g_s32MsgFd);
    HI_LOG_FuncExit();
    return NULL;
}

/**
 * @brief    init the msg client.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/22
 */
HI_S32 HI_MSG_CLIENT_Init(HI_VOID)
{
    HI_S32 s32Ret = 0;
    MLOGI("in HI_MSG_CLIENT_Init %d\n",g_s32MsgFd);
    HI_APPCOMM_CHECK_EXPR(-1 == g_s32MsgFd, HI_EINITIALIZED);
    HI_IPCMSG_CONNECT_S stConnectAttr = {1, HI_APPCOMM_MSG_SRVPORT, 1};
    s32Ret = HI_IPCMSG_AddService(HI_APPCOMM_MSG_SRVNAME, &stConnectAttr);

    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_PrintFuncErr(HI_IPCMSG_AddService, s32Ret);
        return HI_EINTER;
    }

    MLOGI("in 1 HI_IPCMSG_Connect %d\n",g_s32MsgFd);
    s32Ret = HI_IPCMSG_Connect(&g_s32MsgFd, HI_APPCOMM_MSG_SRVNAME, MSG_Handler);
    MLOGI("out 1 HI_IPCMSG_Connect %d\n",g_s32MsgFd);
    if (HI_SUCCESS != s32Ret)
    {
        HI_IPCMSG_DelService(HI_APPCOMM_MSG_SRVNAME);
        HI_LOG_PrintFuncErr(HI_IPCMSG_Connect, s32Ret);
        return HI_EINTER;
    }

    pthread_t threadid;
    s32Ret = pthread_create(&threadid, NULL, MSG_CLIENT_Run, NULL);

    if (HI_SUCCESS != s32Ret)
    {
        HI_IPCMSG_Disconnect(g_s32MsgFd);
        HI_IPCMSG_DelService(HI_APPCOMM_MSG_SRVNAME);
        g_s32MsgFd = -1;
        MLOGE("pthread_create fail:%s\n", strerror(errno));
        return HI_ENORES;
    }

    MLOGI("out HI_MSG_CLIENT_Init %d\n",g_s32MsgFd);
    return HI_SUCCESS;
}

/**
 * @brief    deinit the msg client.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/22
 */
HI_S32 HI_MSG_CLIENT_Deinit(HI_VOID)
{
    HI_S32 s32Ret = 0;
    
    MLOGI("in HI_MSG_CLIENT_Deinit %d\n",g_s32MsgFd);
    HI_APPCOMM_CHECK_EXPR(-1 != g_s32MsgFd, HI_ENOINIT);

    HI_IPCMSG_MESSAGE_S* pstReq = NULL;
    HI_IPCMSG_MESSAGE_S* pstResp = NULL;
    pstReq = HI_IPCMSG_CreateMessage(0, MSG_ID_DISCONNECT, NULL, 0);

    if (pstReq)
    {
        MLOGI("in 1HI_IPCMSG_SendSync %d\n",g_s32MsgFd);
        s32Ret = HI_IPCMSG_SendSync(g_s32MsgFd, pstReq, &pstResp, MSG_SEND_SYNC_TIMEOUT);
        MLOGI("out 1HI_IPCMSG_SendSync %d\n",g_s32MsgFd);
        HI_IPCMSG_DestroyMessage(pstReq);
        HI_IPCMSG_DestroyMessage(pstResp);

        if (HI_SUCCESS != s32Ret)
        {
            HI_LOG_PrintFuncErr(HI_MSG_CLIENT_SendSync, s32Ret);
        }
    }

    s32Ret = HI_IPCMSG_Disconnect(g_s32MsgFd);

    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_PrintFuncErr(HI_IPCMSG_Disconnect, s32Ret);
    }

    s32Ret = HI_IPCMSG_DelService(HI_APPCOMM_MSG_SRVNAME);

    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_PrintFuncErr(HI_IPCMSG_DelService, s32Ret);
    }

    MSG_PROC_S* pstMsgProc = NULL;
    HI_List_Head_S* pstNode = NULL;
    HI_List_For_Each(pstNode, &g_stMSGProcList)
    {
        pstMsgProc = HI_LIST_ENTRY(pstNode, MSG_PROC_S, stList);
        HI_List_Del(pstNode);
        HI_APPCOMM_SAFE_FREE(pstMsgProc);
        pstNode = &g_stMSGProcList;
    }

    g_s32MsgFd = -1;
    
    MLOGI("out HI_MSG_CLIENT_Deinit %d\n",g_s32MsgFd);
    return HI_SUCCESS;
}


static HI_VOID* DHS_MSG_CLIENT_Run(HI_VOID* arg)
{
    
    MLOGI("in MSG_CLIENT_Run %d\n",g_s32MsgFd2);
    pthread_detach(pthread_self());
    prctl(PR_SET_NAME, __func__, 0, 0, 0);
    HI_IPCMSG_Run(g_s32MsgFd2);
    MLOGI("out MSG_CLIENT_Run %d\n",g_s32MsgFd2);
    HI_LOG_FuncExit();
    return NULL;
}

/**
 * @brief    init the msg client.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/22
 */
HI_S32 DHS_MSG_CLIENT_Init(HI_VOID)
{
    HI_S32 s32Ret = 0;
   // MLOGI("in DHS_MSG_CLIENT_Init %d\n",g_s32MsgFd2);
    HI_APPCOMM_CHECK_EXPR(-1 == g_s32MsgFd2, HI_EINITIALIZED);
    HI_IPCMSG_CONNECT_S stConnectAttr = {1, DHS_MSG_SRVPORT, 1};
    s32Ret = HI_IPCMSG_AddService(DHS_MSG_SRVNAME, &stConnectAttr);

    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_PrintFuncErr(HI_IPCMSG_AddService, s32Ret);
        return HI_EINTER;
    }

  //  MLOGI("in 1 HI_IPCMSG_Connect %d\n",g_s32MsgFd2);
    s32Ret = HI_IPCMSG_Connect(&g_s32MsgFd2, DHS_MSG_SRVNAME, MSG_Handler);

  //  MLOGI("out 1 HI_IPCMSG_Connect %d\n",g_s32MsgFd2);
    if (HI_SUCCESS != s32Ret)
    {
            HI_IPCMSG_DelService( DHS_MSG_SRVNAME);
        HI_LOG_PrintFuncErr(HI_IPCMSG_Connect, s32Ret);
        return HI_EINTER;
    }

    pthread_t threadid;
    s32Ret = pthread_create(&threadid, NULL, DHS_MSG_CLIENT_Run, NULL);

    if (HI_SUCCESS != s32Ret)
    {
        HI_IPCMSG_Disconnect(g_s32MsgFd2);
        HI_IPCMSG_DelService(DHS_MSG_SRVNAME);
        g_s32MsgFd2 = -1;
        MLOGE("pthread_create fail:%s\n", strerror(errno));
        return HI_ENORES;
    }

   /////////////// MLOGI("out HI_MSG_CLIENT_Init %d\n",g_s32MsgFd2);
    return HI_SUCCESS;
}

/**
 * @brief    deinit the msg client.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/22
 */
HI_S32 DHS_MSG_CLIENT_Deinit(HI_VOID)
{
    HI_S32 s32Ret = 0;
    
    MLOGI("in HI_MSG_CLIENT_Deinit %d\n",g_s32MsgFd2);
    HI_APPCOMM_CHECK_EXPR(-1 != g_s32MsgFd2, HI_ENOINIT);

    HI_IPCMSG_MESSAGE_S* pstReq = NULL;
    HI_IPCMSG_MESSAGE_S* pstResp = NULL;
    pstReq = HI_IPCMSG_CreateMessage(0, MSG_ID_DISCONNECT, NULL, 0);

    if (pstReq)
    {
        
    MLOGI("in 1HI_IPCMSG_SendSync %d\n",g_s32MsgFd2);
        s32Ret = HI_IPCMSG_SendSync(g_s32MsgFd2, pstReq, &pstResp, MSG_SEND_SYNC_TIMEOUT);
        
    MLOGI("out 1HI_IPCMSG_SendSync %d\n",g_s32MsgFd2);
        HI_IPCMSG_DestroyMessage(pstReq);
        HI_IPCMSG_DestroyMessage(pstResp);

        if (HI_SUCCESS != s32Ret)
        {
            HI_LOG_PrintFuncErr(HI_MSG_CLIENT_SendSync, s32Ret);
        }
    }

    s32Ret = HI_IPCMSG_Disconnect(g_s32MsgFd2);

    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_PrintFuncErr(HI_IPCMSG_Disconnect, s32Ret);
    }

    s32Ret = HI_IPCMSG_DelService(DHS_MSG_SRVNAME);
    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_PrintFuncErr(HI_IPCMSG_DelService, s32Ret);
    }

    MSG_PROC_S* pstMsgProc = NULL;
    HI_List_Head_S* pstNode = NULL;
    HI_List_For_Each(pstNode, &g_stMSGProcList)
    {
        pstMsgProc = HI_LIST_ENTRY(pstNode, MSG_PROC_S, stList);
        HI_List_Del(pstNode);
        HI_APPCOMM_SAFE_FREE(pstMsgProc);
        pstNode = &g_stMSGProcList;
    }

    g_s32MsgFd2 = -1;
    
    MLOGI("out HI_MSG_CLIENT_Deinit %d\n",g_s32MsgFd2);
    return HI_SUCCESS;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
