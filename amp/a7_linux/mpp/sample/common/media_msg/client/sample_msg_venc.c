#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <sys/prctl.h>
#include <unistd.h>
#include "mpi_sys.h"
#include "hi_common.h"
#include "hi_comm_ipcmsg.h"
#include "hi_ipcmsg.h"
#include "hi_datafifo.h"
#include "sample_msg.h"
#include "sample_msg_venc.h"

extern HI_S32 g_MCmsgId;
static pthread_t g_VencPid;
static SAMPLE_VENC_RECEIVE_STREAM_S g_stReceiveStream;
VENC_STREAM_BUF_INFO_S g_stStreamBufInfo[VENC_MAX_CHN_NUM];
static HI_DATAFIFO_HANDLE g_DatafifoHandle[VENC_MAX_CHN_NUM] = {HI_DATAFIFO_INVALID_HANDLE};

static HI_S32 SAMPLE_VENC_Client_DatafifoInit(HI_S32 s32VencChn, HI_U64 u64PhyAddr)
{
    HI_S32 s32Ret;
    HI_DATAFIFO_PARAMS_S stDatafifo;

    stDatafifo.u32EntriesNum        = 10;
    stDatafifo.u32CacheLineSize     = sizeof(SAMPLE_VENC_STREAM_S);
    stDatafifo.bDataReleaseByWriter = HI_TRUE;
    stDatafifo.enOpenMode           = DATAFIFO_READER;

    s32Ret = HI_DATAFIFO_OpenByAddr(&g_DatafifoHandle[s32VencChn], &stDatafifo, u64PhyAddr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("chn %d open datafifo error:%x\n", s32VencChn, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static HI_VOID SAMPLE_VENC_Client_DatafifoExit(HI_S32 s32VencChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 readLen;
    HI_VOID *ptr;

    while(1)
    {
        readLen = 0;
        s32Ret = HI_DATAFIFO_CMD(g_DatafifoHandle[s32VencChn], DATAFIFO_CMD_GET_AVAIL_READ_LEN, &readLen);
        if (readLen > 0)
        {
            s32Ret = HI_DATAFIFO_Read(g_DatafifoHandle[s32VencChn], (HI_VOID **)&ptr);
            if (HI_SUCCESS == s32Ret)
            {
                s32Ret = HI_DATAFIFO_CMD(g_DatafifoHandle[s32VencChn], DATAFIFO_CMD_READ_DONE, ptr);
            }
        }
        else
        {
            break;
        }
    }

    HI_DATAFIFO_Close(g_DatafifoHandle[s32VencChn]);
    g_DatafifoHandle[s32VencChn] = HI_DATAFIFO_INVALID_HANDLE;
}

static HI_S32 SAMPLE_VENC_Server_DatafifoInit(HI_S32 s32VencChn, HI_U64 *pu64PhyAddr)
{
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *pReq = NULL;
    HI_IPCMSG_MESSAGE_S *pResp = NULL;

    pReq = HI_IPCMSG_CreateMessage(SAMPLE_COMM_MOD_VENC, SAMPLE_COMM_VENC_CMD_DATAFIFO_INIT, NULL, 0);
    if(NULL == pReq)
    {
        printf("HI_IPCMSG_CreateMessage failed!\n");
        return HI_FAILURE;
    }

    pReq->as32PrivData[0] = s32VencChn;
    s32Ret = HI_IPCMSG_SendSync(g_MCmsgId, pReq, &pResp, SAMPLE_SEND_MSG_TIMEOUT);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_IPCMSG_SendSync failed!\n");
        HI_IPCMSG_DestroyMessage(pReq);
        HI_IPCMSG_DestroyMessage(pResp);
        return s32Ret;
    }

    *pu64PhyAddr = *(HI_U64 *)(pResp->pBody);

    HI_IPCMSG_DestroyMessage(pReq);
    HI_IPCMSG_DestroyMessage(pResp);

    return s32Ret;
}

static HI_VOID SAMPLE_VENC_Server_DatafifoExit(HI_S32 s32VencChn)
{
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *pReq = NULL;
    HI_IPCMSG_MESSAGE_S *pResp = NULL;

    pReq = HI_IPCMSG_CreateMessage(SAMPLE_COMM_MOD_VENC, SAMPLE_COMM_VENC_CMD_DATAFIFO_EXIT, HI_NULL, 0);
    if(NULL == pReq)
    {
        printf("HI_IPCMSG_CreateMessage failed!\n");
        return;
    }

    pReq->as32PrivData[0] = s32VencChn;

    s32Ret = HI_IPCMSG_SendSync(g_MCmsgId, pReq, &pResp, SAMPLE_SEND_MSG_TIMEOUT);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_IPCMSG_SendSync failed!\n");
        HI_IPCMSG_DestroyMessage(pReq);
        HI_IPCMSG_DestroyMessage(pResp);
        return;
    }

    s32Ret = pResp->s32RetVal;

    HI_IPCMSG_DestroyMessage(pReq);
    HI_IPCMSG_DestroyMessage(pResp);
}

HI_S32 SAMPLE_MSG_VENC_DatafifoInit(HI_S32 s32VencChn)
{
    HI_S32 s32Ret;
    HI_U64 u64phyAddr;

    s32Ret = SAMPLE_VENC_Server_DatafifoInit(s32VencChn, &u64phyAddr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("SAMPLE_VENC_Server_DatafifoInit error:%x\n", s32Ret);
        SAMPLE_VENC_Client_DatafifoExit(s32VencChn);
        return -1;
    }

    s32Ret = SAMPLE_VENC_Client_DatafifoInit(s32VencChn, u64phyAddr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("SAMPLE_VENC_Client_DatafifoInit error:%x\n", s32Ret);
        return -1;
    }

    return HI_SUCCESS;
}

HI_VOID SAMPLE_MSG_VENC_DatafifoExit(HI_S32 s32VencChn)
{
    SAMPLE_VENC_Client_DatafifoExit(s32VencChn);
    SAMPLE_VENC_Server_DatafifoExit(s32VencChn);
}

static HI_S32 SAMPLE_VENC_GetFilePostfix(PAYLOAD_TYPE_E enPayload, char *szFilePostfix)
{
    if (PT_H264 == enPayload)
    {
        strcpy(szFilePostfix, ".h264");
    }
    else if (PT_H265 == enPayload)
    {
        strcpy(szFilePostfix, ".h265");
    }
    else if (PT_JPEG == enPayload)
    {
        strcpy(szFilePostfix, ".jpg");
    }
    else if (PT_MJPEG == enPayload)
    {
        strcpy(szFilePostfix, ".mjp");
    }
    else
    {
        printf("payload type err!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static HI_S32 SAMPLE_VENC_SaveStream_PhyAddr(FILE *pFd, VENC_STREAM_BUF_INFO_S *pstStreamBuf, VENC_STREAM_S *pstStream, VENC_PACK_S *pPackVirAddr)
{
    HI_U32 i, j;
    HI_U64 u64SrcPhyAddr;
    HI_U32 u32Left;
    HI_S32 s32Ret = 0;
    HI_VOID *pVirAddr = NULL;

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        for (j = 0; j < MAX_TILE_NUM; j++)
        {
            if ((pPackVirAddr[i].u64PhyAddr >= pstStreamBuf->u64PhyAddr[j]) && \
                (pPackVirAddr[i].u64PhyAddr <= pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j]))
            {
                break;
            }
        }

        if (pPackVirAddr[i].u64PhyAddr + pPackVirAddr[i].u32Len >=
            pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j])
        {
            if (pPackVirAddr[i].u64PhyAddr + pPackVirAddr[i].u32Offset >=
                pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j])
            {
                /* physical address retrace in offset segment */
                u64SrcPhyAddr = pstStreamBuf->u64PhyAddr[j] +
                                ((pPackVirAddr[i].u64PhyAddr + pPackVirAddr[i].u32Offset) -
                                 (pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j]));
                pVirAddr = (HI_VOID *)(HI_UL)((HI_U64)(HI_UL)pstStreamBuf->pUserAddr[j] + (u64SrcPhyAddr - pstStreamBuf->u64PhyAddr[j]));
                s32Ret = fwrite(pVirAddr, pPackVirAddr[i].u32Len - pPackVirAddr[i].u32Offset, 1, pFd);
                if (s32Ret < 0)
                {
                    printf("fwrite err %d\n", s32Ret);
                    return s32Ret;
                }
            }
            else
            {
                /* physical address retrace in data segment */
                u32Left = (pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j]) - pPackVirAddr[i].u64PhyAddr;
                u64SrcPhyAddr = pPackVirAddr[i].u64PhyAddr + pPackVirAddr[i].u32Offset;
                pVirAddr = (HI_VOID *)(HI_UL)((HI_U64)(HI_UL)pstStreamBuf->pUserAddr[j] + (u64SrcPhyAddr - pstStreamBuf->u64PhyAddr[j]));
                s32Ret = fwrite(pVirAddr, u32Left - pPackVirAddr[i].u32Offset, 1, pFd);
                if (s32Ret < 0)
                {
                    printf("fwrite err %d\n", s32Ret);
                    return s32Ret;
                }

                u64SrcPhyAddr = pstStreamBuf->u64PhyAddr[j];
                pVirAddr = (HI_VOID *)(HI_UL)((HI_U64)(HI_UL)pstStreamBuf->pUserAddr[j] + (u64SrcPhyAddr - pstStreamBuf->u64PhyAddr[j]));
                s32Ret = fwrite(pVirAddr, pPackVirAddr[i].u32Len - u32Left, 1, pFd);
                if (s32Ret < 0)
                {
                    printf("fwrite err %d\n", s32Ret);
                    return s32Ret;
                }
            }
        }
        else
        {
            /* physical address retrace does not happen */
            u64SrcPhyAddr = pPackVirAddr[i].u64PhyAddr + pPackVirAddr[i].u32Offset;
            pVirAddr = (HI_VOID *)(HI_UL)((HI_U64)(HI_UL)pstStreamBuf->pUserAddr[j] + (u64SrcPhyAddr - pstStreamBuf->u64PhyAddr[j]));
            s32Ret = fwrite(pVirAddr, pPackVirAddr[i].u32Len - pPackVirAddr[i].u32Offset, 1, pFd);
            if (s32Ret != 1)
            {
                printf("fwrite err %d\n", s32Ret);
                return s32Ret;
            }
        }
        fflush(pFd);
    }

    return HI_SUCCESS;
}

static void *SAMPLE_VENC_ReceiveStreamThread(void *arg)
{
    HI_S32 i;
    HI_S32 s32Ret;
    HI_S32 s32ChnTotal;
    VENC_CHN VencChn;
    HI_U32 readLen = 0;
    char szFilePostfix[10];
    HI_CHAR aszFileName[VENC_MAX_CHN_NUM][32];
    FILE* pfd[VENC_MAX_CHN_NUM];
    SAMPLE_VENC_STREAM_S *pstVencStream;
    HI_VOID *pPackViraddr;
    SAMPLE_VENC_RECEIVE_STREAM_S *pstReceiveStream;

    pstReceiveStream = (SAMPLE_VENC_RECEIVE_STREAM_S *)arg;

    prctl(PR_SET_NAME, "ReceiveVencStream", 0,0,0);

    s32ChnTotal = pstReceiveStream->s32Cnt;

    for (i = 0; i < s32ChnTotal; i++)
    {
        VencChn = pstReceiveStream->VeChn[i];

        s32Ret = SAMPLE_VENC_GetFilePostfix(pstReceiveStream->enPayLoadType[i], szFilePostfix);
        if (s32Ret != HI_SUCCESS)
        {
            return NULL;
        }

        snprintf(aszFileName[i], 32, "stream_chn%d%s", VencChn, szFilePostfix);

        pfd[i] = fopen(aszFileName[i], "wb");
        if (!pfd[i])
        {
            printf("open file[%s] failed!\n", aszFileName[i]);
            return NULL;
        }
    }

    while (HI_TRUE == pstReceiveStream->bThreadStart)
    {
        for (i = 0; i < s32ChnTotal; i++)
        {
            VencChn = pstReceiveStream->VeChn[i];

            readLen = 0;
            s32Ret = HI_DATAFIFO_CMD(g_DatafifoHandle[VencChn], DATAFIFO_CMD_GET_AVAIL_READ_LEN, &readLen);
            if (HI_SUCCESS != s32Ret)
            {
                printf("get available read len error:%x\n", s32Ret);
                break;
            }

            if (readLen > 0)
            {
                s32Ret = HI_DATAFIFO_Read(g_DatafifoHandle[VencChn], (HI_VOID **)&pstVencStream);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("read error:%x\n", s32Ret);
                    break;
                }

                if (NULL == g_stStreamBufInfo[VencChn].pUserAddr[0])
                {
                    g_stStreamBufInfo[VencChn].u64PhyAddr[0] = pstVencStream->stStreamBufInfo.u64PhyAddr[0];
                    g_stStreamBufInfo[VencChn].u64BufSize[0] = pstVencStream->stStreamBufInfo.u64BufSize[0];

                    g_stStreamBufInfo[VencChn].pUserAddr[0] = HI_MPI_SYS_Mmap(g_stStreamBufInfo[VencChn].u64PhyAddr[0],
                                                                                   g_stStreamBufInfo[VencChn].u64BufSize[0]);
                    if (g_stStreamBufInfo[VencChn].pUserAddr[0] == NULL)
                    {
                        printf("stream buffer HI_MPI_SYS_MmapCache fail.\n");
                        break;
                    }
                }

                pPackViraddr = HI_MPI_SYS_Mmap(pstVencStream->u64PackPhyAddr, sizeof(VENC_PACK_S) * pstVencStream->stStream.u32PackCount);
                if (NULL == pPackViraddr)
                {
                    printf("HI_MPI_SYS_Mmap fail!\n");
                    break;
                }

                s32Ret = SAMPLE_VENC_SaveStream_PhyAddr(pfd[i], &g_stStreamBufInfo[VencChn], &pstVencStream->stStream, (VENC_PACK_S *)pPackViraddr);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("SAMPLE_COMM_VENC_SaveStream_PhyAddr fail!\n");
                    HI_MPI_SYS_Munmap(pPackViraddr, sizeof(VENC_PACK_S) * pstVencStream->stStream.u32PackCount);
                    break;
                }

                s32Ret = HI_DATAFIFO_CMD(g_DatafifoHandle[VencChn], DATAFIFO_CMD_READ_DONE, pstVencStream);
                if (HI_SUCCESS != s32Ret)
                {
                    printf("break: read done error:%x\n", s32Ret);
                    HI_MPI_SYS_Munmap(pPackViraddr, sizeof(VENC_PACK_S) * pstVencStream->stStream.u32PackCount);
                    break;
                }

                HI_MPI_SYS_Munmap(pPackViraddr, sizeof(VENC_PACK_S) * pstVencStream->stStream.u32PackCount);
            }
        }

        usleep(10000);
    }

    for (i = 0; i < s32ChnTotal; i++)
    {
        VencChn = pstReceiveStream->VeChn[i];

        if (NULL != g_stStreamBufInfo[VencChn].pUserAddr[0])
        {
            HI_MPI_SYS_Munmap(g_stStreamBufInfo[VencChn].pUserAddr[0], g_stStreamBufInfo[VencChn].u64BufSize[0]);
            g_stStreamBufInfo[VencChn].pUserAddr[0] = NULL;
        }

        fclose(pfd[i]);
    }

    return NULL;
}

HI_S32 SAMPLE_MSG_VENC_StartReceiveStream(VENC_CHN VeChn[], PAYLOAD_TYPE_E enPayLoadType[], HI_S32 s32Cnt)
{
    HI_U32 i;

    g_stReceiveStream.bThreadStart = HI_TRUE;
    g_stReceiveStream.s32Cnt = s32Cnt;

    for(i = 0; i < s32Cnt; i++)
    {
        g_stReceiveStream.VeChn[i] = VeChn[i];
        g_stReceiveStream.enPayLoadType[i] = enPayLoadType[i];
    }

    memset(&g_stStreamBufInfo, 0, VENC_MAX_CHN_NUM * sizeof(VENC_STREAM_BUF_INFO_S));

    return pthread_create(&g_VencPid, 0, SAMPLE_VENC_ReceiveStreamThread, (HI_VOID*)&g_stReceiveStream);
}

HI_VOID SAMPLE_MSG_VENC_StopReceiveStream(HI_VOID)
{
    if (HI_TRUE == g_stReceiveStream.bThreadStart)
    {
        g_stReceiveStream.bThreadStart = HI_FALSE;
        pthread_join(g_VencPid, 0);
    }
}

HI_VOID SAMPLE_MSG_VENC_StopServerGetStream(HI_VOID)
{
    HI_S32 s32Ret;
    HI_IPCMSG_MESSAGE_S *pReq = NULL;
    HI_IPCMSG_MESSAGE_S *pResp = NULL;

    pReq = HI_IPCMSG_CreateMessage(SAMPLE_COMM_MOD_VENC, SAMPLE_COMM_VENC_CMD_STOP_GET_STREAM, HI_NULL, 0);
    if(NULL == pReq)
    {
        printf("HI_IPCMSG_CreateMessage failed!\n");
        return;
    }

    s32Ret = HI_IPCMSG_SendSync(g_MCmsgId, pReq, &pResp, SAMPLE_SEND_MSG_TIMEOUT);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_IPCMSG_SendSync failed!\n");
        HI_IPCMSG_DestroyMessage(pReq);
        HI_IPCMSG_DestroyMessage(pResp);
        return;
    }

    s32Ret = pResp->s32RetVal;

    HI_IPCMSG_DestroyMessage(pReq);
    HI_IPCMSG_DestroyMessage(pResp);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
