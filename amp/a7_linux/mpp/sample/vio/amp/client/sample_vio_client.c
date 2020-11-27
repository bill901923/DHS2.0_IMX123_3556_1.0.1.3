#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <signal.h>

#include "hi_comm_ipcmsg.h"
#include "hi_ipcmsg.h"
#include "mpi_sys.h"

#include "sample_msg.h"
#include "sample_msg_venc.h"
#include "sample_vio.h"
#include "sample_vio_amp.h"

extern HI_S32 g_MCmsgId;

HI_VOID SAMPLE_VIO_MsgInit(HI_VOID)
{
    Media_Msg_Init();
}

HI_VOID SAMPLE_VIO_MsgExit(HI_VOID)
{
    Media_Msg_Deinit();
}

void SAMPLE_VIO_HandleSig(HI_S32 signo)
{
    if (SIGINT == signo || SIGTERM == signo)
    {
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
    exit(-1);
}

static HI_S32 SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_E enSampleViCmd)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_IPCMSG_MESSAGE_S *pReq = NULL;
    HI_IPCMSG_MESSAGE_S *pResp = NULL;

    pReq = HI_IPCMSG_CreateMessage(SAMPLE_MOD_VI, enSampleViCmd, NULL, 0);
    if(NULL == pReq)
    {
        printf("HI_IPCMSG_CreateMessage failed!\n");
        return HI_FAILURE;
    }

    s32Ret = HI_IPCMSG_SendSync(g_MCmsgId, pReq, &pResp, SAMPLE_SEND_MSG_TIMEOUT);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_IPCMSG_SendSync failed!\n");
        HI_IPCMSG_DestroyMessage(pReq);
        HI_IPCMSG_DestroyMessage(pResp);
        return s32Ret;
    }
    s32Ret = pResp->s32RetVal;

    HI_IPCMSG_DestroyMessage(pReq);
    HI_IPCMSG_DestroyMessage(pResp);

    return s32Ret;
}


static HI_S32 SAMPLE_VIO_StartViOnlineVpssOnlineRoute(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_START_VI_ONLINE_VPSS_ONLINE);
}

static HI_S32 SAMPLE_VIO_StopViOnlineVpssOnlineRoute(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_STOP_VI_ONLINE_VPSS_ONLINE);
}

HI_S32 SAMPLE_VIO_ViOnlineVpssOnlineRoute(HI_U32 u32VoIntfType)
{
    HI_S32 s32Ret;
    VENC_CHN VeChn[1] = {0};
    PAYLOAD_TYPE_E enPayLoadType[1] = {PT_H265};

    s32Ret = HI_MPI_SYS_Init();
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("sys init failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_MSG_VENC_DatafifoInit(VeChn[0]);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_MSG_VENC_DatafifoInit failed!\n");
        HI_MPI_SYS_Exit();
        return s32Ret;
    }

    s32Ret = SAMPLE_VIO_StartViOnlineVpssOnlineRoute();
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_VIO_StartViOnlineVpssOnlineRoute failed!\n");
        SAMPLE_MSG_VENC_DatafifoExit(VeChn[0]);
        HI_MPI_SYS_Exit();
        return s32Ret;
    }

    s32Ret = SAMPLE_MSG_VENC_StartReceiveStream(VeChn, enPayLoadType, sizeof(VeChn)/sizeof(VENC_CHN));
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_MSG_VENC_StartReceiveStream failed!\n");
        SAMPLE_MSG_VENC_StopServerGetStream();
        SAMPLE_MSG_VENC_DatafifoExit(VeChn[0]);
        SAMPLE_VIO_StopViOnlineVpssOnlineRoute();
        HI_MPI_SYS_Exit();
        return s32Ret;
    }

    PAUSE();

    SAMPLE_MSG_VENC_StopReceiveStream();
    SAMPLE_MSG_VENC_StopServerGetStream();

    SAMPLE_MSG_VENC_DatafifoExit(VeChn[0]);

    SAMPLE_VIO_StopViOnlineVpssOnlineRoute();

    HI_MPI_SYS_Exit();

    return HI_SUCCESS;
}

static HI_S32 SAMPLE_VIO_StartViOnlineVpssOfflineRoute(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_START_VI_ONLINE_VPSS_OFFLINE);
}

static HI_S32 SAMPLE_VIO_StopViOnlineVpssOfflineRoute(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_STOP_VI_ONLINE_VPSS_OFFLINE);
}

static HI_S32 SAMPLE_VIO_SetLDCAttr(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_SET_LDC_ATTR);
}

static HI_S32 SAMPLE_VIO_SetDISAttr(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_SET_DIS_ATTR);
}

static HI_S32 SAMPLE_VIO_SetSpreadAttr(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_SET_SPREAD_ATTR);
}

HI_S32 SAMPLE_VIO_WDR_LDC_DIS_SPREAD(HI_U32 u32VoIntfType)
{
    HI_S32 s32Ret;
    VENC_CHN VeChn[1] = {0};
    PAYLOAD_TYPE_E enPayLoadType[1] = {PT_H265};

    s32Ret = HI_MPI_SYS_Init();
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("sys init failed!\n");
        return s32Ret;
    }

    s32Ret = SAMPLE_MSG_VENC_DatafifoInit(VeChn[0]);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_MSG_VENC_DatafifoInit failed!\n");
        HI_MPI_SYS_Exit();
        return s32Ret;
    }

    s32Ret = SAMPLE_VIO_StartViOnlineVpssOfflineRoute();
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_VIO_StartViOnlineVpssOfflineRoute failed!\n");
        SAMPLE_MSG_VENC_DatafifoExit(VeChn[0]);
        HI_MPI_SYS_Exit();
        return s32Ret;
    }

    s32Ret = SAMPLE_MSG_VENC_StartReceiveStream(VeChn, enPayLoadType, sizeof(VeChn)/sizeof(VENC_CHN));
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_MSG_VENC_StartReceiveStream failed!\n");
        SAMPLE_MSG_VENC_StopServerGetStream();
        SAMPLE_MSG_VENC_DatafifoExit(VeChn[0]);
        SAMPLE_VIO_StopViOnlineVpssOfflineRoute();
        HI_MPI_SYS_Exit();
        return s32Ret;
    }

    printf("Press Enter key to Enable LDC!\n");
    getchar();

    SAMPLE_VIO_SetLDCAttr();

    printf("Press Enter key to Enable DIS!\n");
    getchar();

    SAMPLE_VIO_SetDISAttr();

    printf("Press Enter key to Enable Spread!\n");
    getchar();

    SAMPLE_VIO_SetSpreadAttr();

    PAUSE();

    SAMPLE_MSG_VENC_StopReceiveStream();
    SAMPLE_MSG_VENC_StopServerGetStream();

    SAMPLE_MSG_VENC_DatafifoExit(VeChn[0]);

    SAMPLE_VIO_StopViOnlineVpssOfflineRoute();

    HI_MPI_SYS_Exit();

    return HI_SUCCESS;
}

static HI_S32 SAMPLE_VIO_StartViDoublePipeRoute(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_START_VI_DOUBLE_PIPE);
}

static HI_S32 SAMPLE_VIO_StopViDoublePipeRoute(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_STOP_VI_DOUBLE_PIPE);
}

HI_S32 SAMPLE_VIO_ViDoublePipeRoute(HI_U32 u32VoIntfType)
{
    SAMPLE_VIO_StartViDoublePipeRoute();

    PAUSE();

    SAMPLE_VIO_StopViDoublePipeRoute();

    return HI_SUCCESS;
}

static HI_S32 SAMPLE_VIO_StartViWdrSwitch(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_START_VI_WDR_SWITCH);
}

static HI_S32 SAMPLE_VIO_LinearSwitchWdr(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_LINEAR_SWITCH_WDR);
}

static HI_S32 SAMPLE_VIO_WdrSwitchLinear(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_WDR_SWITCH_LINEAR);
}

static HI_S32 SAMPLE_VIO_StopViWdrSwitch(HI_VOID)
{
    return SAMPLE_VIO_SEND_MSG(SAMPLE_VI_CMD_STOP_VI_WDR_SWITCH);
}

HI_S32 SAMPLE_VIO_ViWdrSwitch(HI_U32 u32VoIntfType)
{
    SAMPLE_VIO_StartViWdrSwitch();

    printf("switch to wdr mode========\n");
    getchar();

    SAMPLE_VIO_LinearSwitchWdr();

    printf("switch to linear mode========\n");
    getchar();

    SAMPLE_VIO_WdrSwitchLinear();

    PAUSE();

    SAMPLE_VIO_StopViWdrSwitch();

    return HI_SUCCESS;
}

HI_S32 SAMPLE_VIO_ViVpssLowDelay(HI_U32 u32VoIntfType)
{
    SAMPLE_PRT("Not Support!\n");
    return HI_SUCCESS;
}

HI_S32 SAMPLE_VIO_Rotate(HI_U32 u32VoIntfType)
{
    SAMPLE_PRT("Not Support!\n");
    return HI_SUCCESS;
}

HI_S32 SAMPLE_VIO_FPN(HI_U32 u32VoIntfType)
{
    SAMPLE_PRT("Not Support!\n");
    return HI_SUCCESS;
}

HI_S32 SAMPLE_VIO_ResoSwitch(HI_U32 u32VoIntfType)
{
    SAMPLE_PRT("Not Support!\n");
    return HI_SUCCESS;
}

HI_S32 SAMPLE_VIO_ViDoubleWdrPipe(HI_U32 u32VoIntfType)
{
    return HI_SUCCESS;
}

HI_S32 SAMPLE_VIO_SetUsrPic(HI_U32 u32VoIntfType)
{
    return HI_SUCCESS;
}

HI_S32 SAMPLE_VIO_VPSS_VO_MIPI_TX(HI_U32 u32VoIntfType)
{
    SAMPLE_PRT("Not Support!\n");
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
