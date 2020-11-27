#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <sys/time.h>
#include <sys/select.h>
#include <sys/prctl.h>

#include "hi_common.h"
#include "hi_comm_ipcmsg.h"
#include "hi_ipcmsg.h"
#include "hi_datafifo.h"
#include "sample_comm.h"
#include "sample_msg_venc.h"

static HI_DATAFIFO_HANDLE g_DatafifoHandle[VENC_MAX_CHN_NUM] = {HI_DATAFIFO_INVALID_HANDLE};

static pthread_t gs_VencPid;
static SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;

static void SAMPLE_VENC_ReleaseStream(void *pArg)
{
    HI_S32 s32Ret;
    VENC_CHN VencChn;
    HI_U64 u64PackPhyAddr;
    VENC_STREAM_S *pstStream;
    SAMPLE_VENC_STREAM_S *pstSampleVencStream;

    pstSampleVencStream = (SAMPLE_VENC_STREAM_S *)pArg;

    VencChn = pstSampleVencStream->s32VencChn;
    u64PackPhyAddr = pstSampleVencStream->u64PackPhyAddr;
    pstStream = &pstSampleVencStream->stStream;

    s32Ret = HI_MPI_VENC_ReleaseStream(VencChn, pstStream);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_ReleaseStream failed!\n");
        HI_MPI_SYS_MmzFree(u64PackPhyAddr, (HI_VOID *)pstStream->pstPack);
        pstStream->pstPack = NULL;
        return;
    }

    HI_MPI_SYS_MmzFree(u64PackPhyAddr, (HI_VOID *)pstStream->pstPack);
    pstStream->pstPack = NULL;
}

static HI_S32 SAMPLE_VENC_Server_DatafifoInit(HI_S32 s32VencChn, HI_U64 *pu64PhyAddr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_DATAFIFO_PARAMS_S stDatafifo;
    HI_U64 u64PhyAddr;

    stDatafifo.u32EntriesNum        = 10;
    stDatafifo.u32CacheLineSize     = sizeof(SAMPLE_VENC_STREAM_S);
    stDatafifo.bDataReleaseByWriter = HI_TRUE;
    stDatafifo.enOpenMode           = DATAFIFO_WRITER;

    s32Ret = HI_DATAFIFO_Open(&g_DatafifoHandle[s32VencChn], &stDatafifo);
    if (HI_SUCCESS != s32Ret)
    {
        printf("open datafifo error:%x\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_DATAFIFO_CMD(g_DatafifoHandle[s32VencChn], DATAFIFO_CMD_GET_PHY_ADDR, &u64PhyAddr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("get datafifo phy addr error:%x\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_DATAFIFO_CMD(g_DatafifoHandle[s32VencChn], DATAFIFO_CMD_SET_DATA_RELEASE_CALLBACK, SAMPLE_VENC_ReleaseStream);
    if (HI_SUCCESS != s32Ret)
    {
        printf("register callback funtion fail! s32Ret: 0x%x\n", s32Ret);
        return HI_FAILURE;
    }

    *pu64PhyAddr = u64PhyAddr;

    return HI_SUCCESS;
}

static HI_VOID SAMPLE_VENC_Server_DatafifoExit(HI_S32 s32VencChn)
{
    HI_S32 s32Ret;

    // call write NULL to flush and release stream buffer.
    s32Ret = HI_DATAFIFO_Write(g_DatafifoHandle[s32VencChn], NULL);
    if (HI_SUCCESS != s32Ret)
    {
        printf("write error:%x\n", s32Ret);
    }

    HI_DATAFIFO_Close(g_DatafifoHandle[s32VencChn]);
    g_DatafifoHandle[s32VencChn] = HI_DATAFIFO_INVALID_HANDLE;
}

static HI_S32 MSG_COMM_VENC_Server_DatafifoInit(HI_S32 s32MsgId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *respMsg;
    HI_S32 s32VencChn = pstMsg->as32PrivData[0];
    HI_U64 u64PhyAddr;

    s32Ret = SAMPLE_VENC_Server_DatafifoInit(s32VencChn, &u64PhyAddr);

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, s32Ret, &u64PhyAddr, sizeof(HI_U64));

    HI_IPCMSG_SendAsync(s32MsgId, respMsg, NULL);
    HI_IPCMSG_DestroyMessage(respMsg);

    return HI_SUCCESS;
}

static HI_S32 MSG_COMM_VENC_Server_DatafifoExit(HI_S32 s32MsgId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_IPCMSG_MESSAGE_S *respMsg;
    HI_S32 s32VencChn = pstMsg->as32PrivData[0];

    SAMPLE_VENC_Server_DatafifoExit(s32VencChn);

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, HI_SUCCESS, NULL, 0);

    HI_IPCMSG_SendAsync(s32MsgId, respMsg, NULL);
    HI_IPCMSG_DestroyMessage(respMsg);

    return HI_SUCCESS;
}

static HI_S32 MSG_COMM_VENC_Server_StopGetStream(HI_S32 s32MsgId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_IPCMSG_MESSAGE_S *respMsg;

    SAMPLE_MSG_VENC_StopGetStream();

    respMsg = HI_IPCMSG_CreateRespMessage(pstMsg, HI_SUCCESS, NULL, 0);

    HI_IPCMSG_SendAsync(s32MsgId, respMsg, NULL);
    HI_IPCMSG_DestroyMessage(respMsg);

    return HI_SUCCESS;
}

HI_S32 MSG_COMM_VENC_PROC(HI_S32 s32MsgId, HI_IPCMSG_MESSAGE_S *pstMsg)
{
    HI_S32 s32Ret = HI_SUCCESS;

    switch(pstMsg->u32CMD)
    {
        case SAMPLE_COMM_VENC_CMD_DATAFIFO_INIT:
            s32Ret = MSG_COMM_VENC_Server_DatafifoInit(s32MsgId, pstMsg);
            break;
        case SAMPLE_COMM_VENC_CMD_DATAFIFO_EXIT:
            s32Ret = MSG_COMM_VENC_Server_DatafifoExit(s32MsgId, pstMsg);
            break;
        case SAMPLE_COMM_VENC_CMD_STOP_GET_STREAM:
            s32Ret = MSG_COMM_VENC_Server_StopGetStream(s32MsgId, pstMsg);
            break;
        default:
            printf("venc comm error cmd %d\n", pstMsg->u32CMD);
            break;
    }

    return s32Ret;
}

static HI_BOOL Is_StreamDatafifoFull(HI_S32 s32VencChn)
{
    HI_S32 s32Ret;
    HI_U32 u32AvailWriteLen = 0;

    s32Ret = HI_DATAFIFO_Write(g_DatafifoHandle[s32VencChn], NULL);
    if (HI_SUCCESS != s32Ret)
    {
        printf("write error:%x\n", s32Ret);
        return HI_TRUE;
    }

    s32Ret = HI_DATAFIFO_CMD(g_DatafifoHandle[s32VencChn], DATAFIFO_CMD_GET_AVAIL_WRITE_LEN, &u32AvailWriteLen);
    if (HI_SUCCESS != s32Ret)
    {
        printf("get available write len error:%x\n", s32Ret);
        return HI_TRUE;
    }

    if (u32AvailWriteLen > sizeof(SAMPLE_VENC_STREAM_S))
    {
        return HI_FALSE;
    }
    else
    {
        return HI_TRUE;
    }
}

static HI_S32 SAMPLE_VENC_WriteStreamToDatafifo(HI_S32 s32VencChn, SAMPLE_VENC_STREAM_S *pstVencStream)
{
    HI_S32 s32Ret;

    s32Ret = HI_DATAFIFO_Write(g_DatafifoHandle[s32VencChn], pstVencStream);
    if (HI_SUCCESS != s32Ret)
    {
        printf("write error:%x\n", s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_DATAFIFO_CMD(g_DatafifoHandle[s32VencChn], DATAFIFO_CMD_WRITE_DONE, NULL);
    if (HI_SUCCESS != s32Ret)
    {
        printf("write done error:%x\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_VOID *SAMPLE_MSG_VENC_GetVencStreamProc(HI_VOID *pArg)
{
    HI_S32 i;
    HI_S32 s32ChnTotal;
    SAMPLE_VENC_GETSTREAM_PARA_S *pstPara;
    HI_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_S32 VencFd[VENC_MAX_CHN_NUM];
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    VENC_CHN VencChn;
    HI_U64 u64PackPhyAddr;
    VENC_STREAM_BUF_INFO_S stStreamBufInfo[VENC_MAX_CHN_NUM];
    SAMPLE_VENC_STREAM_S stSampleVencStream;

    prctl(PR_SET_NAME, "GetVencStream", 0, 0, 0);

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S *)pArg;
    s32ChnTotal = pstPara->s32Cnt;

    /******************************************
     step 1:  check venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return NULL;
    }

    for (i = 0; i < s32ChnTotal; i++)
    {
        VencChn = pstPara->VeChn[i];

        /* Set Venc Fd. */
        VencFd[i] = HI_MPI_VENC_GetFd(VencChn);
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetFd failed with %#x!\n", VencFd[i]);
            return NULL;
        }

        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }

        s32Ret = HI_MPI_VENC_GetStreamBufInfo(VencChn, &stStreamBufInfo[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetStreamBufInfo failed with %#x!\n", s32Ret);
            return NULL;
        }
    }

    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (HI_TRUE == pstPara->bThreadStart)
    {
        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }

        TimeoutVal.tv_sec  = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            for (i = 0; i < s32ChnTotal; i++)
            {
                VencChn = pstPara->VeChn[i];

                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    if (Is_StreamDatafifoFull(VencChn))
                    {
                        continue;
                    }
                    /*******************************************************
                     step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    memset(&stStream, 0, sizeof(stStream));

                    s32Ret = HI_MPI_VENC_QueryStatus(VencChn, &stStat);
                    if (HI_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("HI_MPI_VENC_QueryStatus chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }

                    /*******************************************************
                    step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                     if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                     {
                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                     }
                    *******************************************************/
                    if (0 == stStat.u32CurPacks)
                    {
                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                    }
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    s32Ret = HI_MPI_SYS_MmzAlloc(&u64PackPhyAddr, (HI_VOID**)&(stStream.pstPack), NULL, NULL, sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (HI_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("malloc stream pack failed!\n");
                        break;
                    }

                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = HI_MPI_VENC_GetStream(VencChn, &stStream, HI_TRUE);
                    if (HI_SUCCESS != s32Ret)
                    {
                        HI_MPI_SYS_MmzFree(u64PackPhyAddr, (HI_VOID *)stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
                        break;
                    }

                    stSampleVencStream.s32VencChn = VencChn;
                    stSampleVencStream.u64PackPhyAddr = u64PackPhyAddr;
                    memcpy(&stSampleVencStream.stStream, &stStream, sizeof(VENC_STREAM_S));
                    memcpy(&stSampleVencStream.stStreamBufInfo, &stStreamBufInfo[i], sizeof(VENC_STREAM_BUF_INFO_S));

                    s32Ret = SAMPLE_VENC_WriteStreamToDatafifo(VencChn, &stSampleVencStream);
                    if (HI_SUCCESS != s32Ret)
                    {
                        HI_MPI_VENC_ReleaseStream(VencChn, &stStream);
                        HI_MPI_SYS_MmzFree(u64PackPhyAddr, (HI_VOID *)stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("SAMPLE_VENC_WriteStreamToDatafifo failed with %#x!\n", s32Ret);
                        break;
                    }

                }
            }
        }
    }

    return NULL;
}

HI_S32 SAMPLE_MSG_VENC_StartGetStream(VENC_CHN VeChn[], HI_S32 s32Cnt)
{
    HI_U32 i;

    gs_stPara.bThreadStart = HI_TRUE;
    gs_stPara.s32Cnt = s32Cnt;

    for(i = 0; i < s32Cnt; i++)
    {
        gs_stPara.VeChn[i] = VeChn[i];
    }

    return pthread_create(&gs_VencPid, 0, SAMPLE_MSG_VENC_GetVencStreamProc, (HI_VOID*)&gs_stPara);
}

HI_VOID SAMPLE_MSG_VENC_StopGetStream(HI_VOID)
{
    if (HI_TRUE == gs_stPara.bThreadStart)
    {
        gs_stPara.bThreadStart = HI_FALSE;
        pthread_join(gs_VencPid, 0);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
