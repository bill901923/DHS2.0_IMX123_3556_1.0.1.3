#ifndef __SAMPLE_MSG_VENC_H__
#define __SAMPLE_MSG_VENC_H__

#include "hi_common.h"
#include "hi_comm_venc.h"
#include "hi_comm_ipcmsg.h"
#include "hi_ipcmsg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef enum hiSAMPLE_COMM_VENC_CMD
{
    SAMPLE_COMM_VENC_CMD_DATAFIFO_INIT = 0,
    SAMPLE_COMM_VENC_CMD_DATAFIFO_EXIT,
    SAMPLE_COMM_VENC_CMD_STOP_GET_STREAM,

    SAMPLE_COMM_VENC_CMD_BUTT,
} SAMPLE_COMM_VENC_CMD_E;

typedef struct hiSAMPLE_VENC_STREAM_S
{
    HI_S32 s32VencChn;
    HI_U64 u64PackPhyAddr;
    VENC_STREAM_S stStream;
    VENC_STREAM_BUF_INFO_S stStreamBufInfo;
} SAMPLE_VENC_STREAM_S;

typedef struct hiSAMPLE_VENC_RECEIVE_STREAM_S
{
    HI_BOOL bThreadStart;
    VENC_CHN VeChn[VENC_MAX_CHN_NUM];
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
    HI_S32  s32Cnt;
} SAMPLE_VENC_RECEIVE_STREAM_S;

HI_S32 MSG_COMM_VENC_PROC(HI_S32 s32MsgId, HI_IPCMSG_MESSAGE_S *pstMsg);

HI_S32 SAMPLE_MSG_VENC_StartGetStream(VENC_CHN VeChn[], HI_S32 s32Cnt);
HI_VOID SAMPLE_MSG_VENC_StopGetStream(HI_VOID);

HI_S32 SAMPLE_MSG_VENC_DatafifoInit(HI_S32 s32VencChn);
HI_VOID SAMPLE_MSG_VENC_DatafifoExit(HI_S32 s32VencChn);

HI_S32 SAMPLE_MSG_VENC_StartReceiveStream(VENC_CHN VeChn[], PAYLOAD_TYPE_E enPayLoadType[], HI_S32 s32Cnt);
HI_VOID SAMPLE_MSG_VENC_StopReceiveStream(HI_VOID);

HI_VOID SAMPLE_MSG_VENC_StopServerGetStream(HI_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_MSG_VI_H__*/



