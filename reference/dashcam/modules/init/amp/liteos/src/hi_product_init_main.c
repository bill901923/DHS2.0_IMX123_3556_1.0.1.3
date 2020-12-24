/**
 * @file      hi_product_init_main.c
 * @brief     init in HuaweiLite
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/3/20
 * @version   1.0

 */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include "sys/types.h"
#include "sys/time.h"
#include "unistd.h"
#include "stdio.h"
#include "hi_type.h"
#include "asm/io.h"
#include "string.h"
#include "stdlib.h"
#include "pthread.h"
#include <sys/prctl.h>
#include "shell.h"
#include "hi_appcomm.h"

#include "hi_mapi_log.h"
#include "hi_product_init_os.h"
#include "hi_product_init_chip.h"
#include "hi_product_init_peripheral.h"
#include "hi_product_init_service.h"
#include "hi_timestamp.h"

//#ifdef CONFIG_PQT_SUPPORT_ON
//#include "hi_product_statemng.h"
//#endif
HI_BOOL PQFLAG=HI_TRUE;
static HI_S32 PDT_INIT_PreInit(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;

    MLOGD("HI_PDT_INIT_OS_PreInit ...\n");
    s32Ret = HI_PDT_INIT_OS_PreInit();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);

    MLOGD("HI_PDT_INIT_CHIP_PreInit ...\n");
    s32Ret = HI_PDT_INIT_CHIP_PreInit();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);

    MLOGD("HI_PDT_INIT_PERIPHERAL_PreInit ...\n");
    s32Ret = HI_PDT_INIT_PERIPHERAL_PreInit();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);

//#ifndef CONFIG_PQT_STREAM_SUPPORT_ON
//if(DHS_PQENB()==0)
{
    MLOGD("HI_PDT_INIT_SERVICE_PreInit ...\n");
    s32Ret = HI_PDT_INIT_SERVICE_PreInit();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);
}
//#endif
    return s32Ret;
}

static HI_S32 PDT_INIT_Init(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;

    MLOGD("HI_PDT_INIT_OS_Init ...\n");
    s32Ret = HI_PDT_INIT_OS_Init();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);

    MLOGD("HI_PDT_INIT_CHIP_Init ...\n");
    s32Ret = HI_PDT_INIT_CHIP_Init();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);

    MLOGD("HI_PDT_INIT_PERIPHERAL_Init ...\n");
    s32Ret = HI_PDT_INIT_PERIPHERAL_Init();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);
//#ifndef CONFIG_PQT_STREAM_SUPPORT_ON
if(DHS_PQENB()==0)
{
    MLOGD("HI_PDT_INIT_SERVICE_Init ...\n");
    s32Ret = HI_PDT_INIT_SERVICE_Init();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);
}
//#endif
    return s32Ret;
}

static HI_S32 PDT_INIT_PostInit(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MLOGD("HI_PDT_INIT_OS_PostInit ...\n");
    HI_TIME_STAMP;
    s32Ret = HI_PDT_INIT_OS_PostInit();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);

    MLOGD("HI_PDT_INIT_CHIP_PostInit ...\n");
    HI_TIME_STAMP;
    s32Ret = HI_PDT_INIT_CHIP_PostInit();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);

    MLOGD("HI_PDT_INIT_PERIPHERAL_PostInit ...\n");
    HI_TIME_STAMP;
    s32Ret = HI_PDT_INIT_PERIPHERAL_PostInit();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);
//#ifndef CONFIG_PQT_STREAM_SUPPORT_ON
if(DHS_PQENB()==0)
{
    MLOGD("HI_PDT_INIT_SERVICE_PostInit ...\n");
    HI_TIME_STAMP;
    s32Ret = HI_PDT_INIT_SERVICE_PostInit();
    HI_APPCOMM_CHECK_RETURN(s32Ret, HI_FAILURE);
}
//#endif
    return s32Ret;
}

#ifdef CFG_DEBUG_LOG_ON

static HI_S32 PDT_INIT_SetLog(HI_S32 argc, HI_CHAR* argv[])
{
    if (3 != argc)
    {
        printf(
                "\n"
                "*************************************************\n"
                "Usage: ./setlog [LogOn] [LogTimeOn] [LogLevel]\n"
                "LogLevel: \n"
                "   HI_LOG_LEVEL_FATAL: %d\n"
                "   HI_LOG_LEVEL_ERROR: %d\n"
                "   HI_LOG_LEVEL_WARNING: %d\n"
                "   HI_LOG_LEVEL_INFO: %d\n"
                "   HI_LOG_LEVEL_DEBUG: %d\n"
                "e.g : ./setlog 1 1 3\n"
                "*************************************************\n"
                "\n", HI_LOG_LEVEL_FATAL, HI_LOG_LEVEL_ERROR, HI_LOG_LEVEL_WARNING,
                HI_LOG_LEVEL_INFO, HI_LOG_LEVEL_DEBUG);
        return HI_FAILURE;
    }
    HI_BOOL bLogOn = atoi(argv[0]);
    HI_BOOL bLogTimeOn = atoi(argv[1]);
    HI_LOG_LEVEL_E enLogLevel = atoi(argv[2]);
    HI_APPCOMM_CHECK_EXPR(((enLogLevel >= HI_LOG_LEVEL_FATAL) && (enLogLevel < HI_LOG_LEVEL_BUTT)) ,HI_FAILURE);
    MLOGD("LogOn[%d] LogTimeOn[%d] Level[%d]\n", bLogOn, bLogTimeOn, enLogLevel);
    HI_LOG_Config(bLogOn, bLogTimeOn, enLogLevel);
    return HI_SUCCESS;
}

#endif

#if (defined(CONFIG_PQT_SUPPORT_ON) && !defined(CONFIG_PQT_STREAM_SUPPORT_ON))
/*
static HI_S32 PDT_PQDebug_Callback(HI_S32 s32MsgID, const HI_VOID* pvRequest, HI_U32 u32ReqLen, HI_MSG_PRIV_DATA_S* pstPrivData,
        HI_VOID** ppvResponse, HI_U32* pu32RespLen, HI_VOID* pvUserData)
{
    HI_BOOL pqDebug = *(HI_BOOL *)pvRequest;

    extern HI_VOID app_control(HI_S32 argc, HI_CHAR **argv );
    if (pqDebug) {
        char* cmd = HI_SHAREFS_ROOT_PATH;
        app_control(1, &cmd);
        MLOGI("start the app_control\n");
    } else {
        char* cmd = "stop";
        app_control(1, &cmd);
        MLOGI("stop the app_control\n");
    }

    return HI_SUCCESS;
}
*/
#endif


static HI_VOID PDT_INIT_CmdReg(HI_VOID)
{
#ifdef CFG_DEBUG_LOG_ON
    osCmdReg(CMD_TYPE_EX, "setlog", 1, (CMD_CBK_FUNC)PDT_INIT_SetLog);
#endif

//#ifdef CONFIG_PQT_SUPPORT_ON
//#ifdef CONFIG_PQT_STREAM_SUPPORT_ON
    extern HI_VOID app_control(HI_S32 argc, HI_CHAR **argv );
    osCmdReg(CMD_TYPE_EX,"pq_control", 0, (CMD_CBK_FUNC)app_control);

    extern HI_VOID app_stream(HI_S32 argc, HI_CHAR **argv );
    osCmdReg(CMD_TYPE_EX,"pq_stream", 0, (CMD_CBK_FUNC)app_stream);
//#else
//    (HI_VOID)HI_MSG_RegisterMsgProc(HI_MSGID_STATEMNG_PQDEBUG, PDT_PQDebug_Callback, NULL);
//#endif
//#endif
    /* cat logmpp */
    extern void CatLogShell();
    CatLogShell();
}

HI_VOID app_init(HI_VOID)
{
#ifdef TIME_STAMP_BASE_ADDR_LITEOS
    HI_TimeStamp_Init(TIME_STAMP_BASE_ADDR_LITEOS, HI_TRUE);
#endif
#ifdef CONFIG_DEBUG
    HI_LOG_Config(HI_TRUE, HI_FALSE, HI_LOG_LEVEL_INFO);
    HI_MAPI_LOG_SetEnabledLevel(HI_MAPI_LOG_LEVEL_DEBUG);
#else
    HI_LOG_Config(HI_TRUE, HI_FALSE, HI_LOG_LEVEL_ERROR);
    HI_MAPI_LOG_SetEnabledLevel(HI_MAPI_LOG_LEVEL_ERR);
#endif
    HI_TIME_STAMP;
    PDT_INIT_PreInit();
    PDT_INIT_Init();
    PDT_INIT_PostInit();
    HI_TIME_STAMP;
    PDT_INIT_CmdReg();
    HI_TIME_PRINT;
    return ;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

