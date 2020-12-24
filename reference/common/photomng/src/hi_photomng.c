/**
 * @file    hi_photomng.c
 * @brief   photo picture.
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2018/2/2
 * @version   1.0

 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <math.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#include "hi_timer_ext.h"
#include "hi_eventhub.h"
#include "hi_exif.h"
#include "hi_mapi_vcap.h"
#include "hi_mapi_vproc.h"
#include "hi_mapi_venc.h"

#ifdef CFG_DNG
#include "photomng_dngmux.h"
#endif
#include "hi_photomng.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif  /* End of #ifdef __cplusplus */

/** \addtogroup     PHOTOMNG */
/** @{ */  /** <!-- [PHOTOMNG] */
/** rename module name */
#ifdef HI_MODULE
#undef HI_MODULE
#endif
#define HI_MODULE "PHOTOMNG"

#define PHOTOMNG_TIMER_TICKVALUE_US  (20*1000)  /**< unit:us */
#define PHOTOMNG_PHOTO_TIMEOUT  (10)            /**< unit:second */
#define PHOTOMNG_WAIT_SAVE_TIMEOUT  (45)        /**< unit:second */

/**Current Support belowing BAYER FORAMT*/
/**PIXEL_FORMAT_RGB_BAYER_8BPP   PIXEL_FORMAT_RGB_BAYER_10BPP*/
/**PIXEL_FORMAT_RGB_BAYER_12BPP   PIXEL_FORMAT_RGB_BAYER_14BPP*/
/**PIXEL_FORMAT_RGB_BAYER_16BPP*/
#define PHOTOMNG_DNG_RAW_PIXELFORMAT PIXEL_FORMAT_RGB_BAYER_12BPP
#define DATA_SIZE 6000000 

#ifdef CFG_DNG
#define PHOTOMNG_WAIT_DATA_TIMEOUT  (45)        /**< unit:second */
#define PHOTOMNG_DNG_NUM_MAX    (1)

typedef struct tagPHOTOMNG_DNG_DATA_CTX_S
{
    HI_U32 u32ThmIndex;
    HI_U32 u32RawIndex;
    HI_U32 u32ScreenIndex;
    PHOTOMNG_DNG_DATA_BUFFER_S  astDngDataBuf[PHOTOMNG_DNG_NUM_MAX];
    pthread_mutex_t DataMutex;
    pthread_cond_t  DataCond; /*data ready*/
} PHOTOMNG_DNG_DATA_CTX_S;
#endif

typedef struct tagPHOTOMNG_IMAGE_DATA_S
{
    HI_U32 u32DataLen;
    HI_U8* pu8DataBuf;
} PHOTOMNG_IMAGE_DATA_S;

typedef enum tagPHOTOMNG_TASK_STATE_E
{
    PHOTOMNG_TASK_STATE_READY,
    PHOTOMNG_TASK_STATE_TICKING,
    PHOTOMNG_TASK_STATE_STARTED,
    PHOTOMNG_TASK_STATE_INVALID,
    PHOTOMNG_TASK_STATE_BUTT
} PHOTOMNG_TASK_STATE_E;

typedef struct tagPHOTOMNG_TASK_CTX_S
{
    HI_BOOL bEnableDCF;
    HI_PHOTOMNG_TASK_CFG_S stCfg;
    HI_PHOTOMNG_PHOTO_ATTR_S stPhotoAttr;
    pthread_mutex_t TaskMutex;
    pthread_cond_t  TaskEndCond;
    pthread_cond_t  TaskStoppedCond;    /**< before task end but started ,if taskstop function called,it should wait TaskStoppedCond in taskstop function*/
    PHOTOMNG_TASK_STATE_E enTaskState;
    HI_TIMER_HANDLE hTimerHdl;
    HI_BOOL bLapseStop;

    HI_S32 s32PhotoCnt; /**<photo count*/
    HI_S32 s32SavedCnt; /**<photo success count */
#ifdef CFG_DNG
    PHOTOMNG_DNG_DATA_CTX_S  stDngDataCTX;
#endif
    HI_PHOTOMNG_FILENAME_S  stFileName[HI_PHOTOMNG_FILECNT_MAX];
} PHOTOMNG_TASK_CTX_S;

typedef struct tagPHOTOMNG_CTX_S
{
    pthread_mutex_t CtxMutex;
    HI_PHOTOMNG_VENC_OPERATE_S stVencOps;
    HI_PHOTOMNG_GETDNGUSERINFO_CALLBACK_FN_PTR pfnGetDNGUserInfoCB;
    HI_PHOTOMNG_GETJPEGUSERINFO_CALLBACK_FN_PTR pfnGetJpegUserInfoCB;
    HI_S32 s32TimerGrp;
    PHOTOMNG_TASK_CTX_S astTaskCtx[HI_PHOTOMNG_TASK_NUM_MAX];
} PHOTOMNG_CTX_S;

static pthread_mutex_t s_PHOTOMNGInitMutex = PTHREAD_MUTEX_INITIALIZER;
static HI_BOOL s_bPHOTOMNGInitFlg = HI_FALSE;
static PHOTOMNG_CTX_S s_stPHOTOMNGCtx;

#define PHOTOMNG_CHECK_INIT()   \
    do{ \
        HI_MUTEX_LOCK(s_PHOTOMNGInitMutex); \
        if(HI_TRUE != s_bPHOTOMNGInitFlg)  \
        {   \
            HI_MUTEX_UNLOCK(s_PHOTOMNGInitMutex);   \
            MLOGW("photomng has not been inited,please init first \n"); \
            return HI_PHOTOMNG_ENOTINIT;    \
        }   \
        HI_MUTEX_UNLOCK(s_PHOTOMNGInitMutex);   \
    }while(0)

#ifdef CFG_TIME_STAMP_ON
static struct timespec s_PHOTOMNGBeginTime;
static struct timespec s_PHOTOMNGEndTime;
static HI_U64 s_PhotoMngLastPackPTS;
#endif

static HI_S32 PHOTOMNG_VencStop(PHOTOMNG_TASK_CTX_S* pstTaskContext);

static HI_VOID PHOTOMNG_ReportEvent(HI_EVENT_S* pstEvent, const HI_CHAR* pszFunc, HI_S32 s32Line)
{
    HI_S32 s32Ret = HI_EVTHUB_Publish(pstEvent);

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("location:[%s](%d)  publish event[id=0x%08X] fail:[0x%08X]\n", pszFunc, s32Line, pstEvent->EventID, s32Ret);
    }
    else
    {
        //MLOGI("location:[%s](%d)  publish event[id=0x%08X]\n",pszFunc,s32Line,pstEvent->EventID);
    }

    return;
}

static HI_S32 PHOTOMNG_JpegDataExtract(HI_MAPI_VENC_DATA_S* pstVStreamData,
                                       HI_PHOTOMNG_FILENAME_S* pstFile,
                                       PHOTOMNG_IMAGE_DATA_S* pstImageData,
                                       HI_EXIF_BUFFER_SOURCE_S* pstBufSource)
{
    HI_S32 i = 0;
    HI_S32 n = 0;
    HI_U32 u32DataLen = 0;
    HI_U8* pu8TmpBuf = NULL;
   // HI_S32 s32fd = -1;
   // HI_U64 i_U64 = 0;
   // HI_U64 cunt_U64 = 0;

    
    /** count data len*/
    for (i = 0; i < pstVStreamData->u32PackCount; i++)
    {
        HI_U32 u32PackLen = pstVStreamData->astPack[i].au32Len[0];
        HI_U32 u32PackOffset = pstVStreamData->astPack[i].u32Offset;
        u32DataLen += (u32PackLen - u32PackOffset);
    }
        int shmid;
        void *shm = NULL;
        share_memory *shmdata;
        key_t key = ftok(SHM_NAME, 'k');
        MLOGI("shmget Do Jpeg Save  %d\n",key);
        shmid = shmget(key, sizeof(share_memory), 0666|IPC_CREAT);
        if(shmid == -1)
        {
            MLOGI("shmget failed. \r\n");
            exit(-1);
        }
        shm = shmat(shmid, 0, 0);
        if(shm == (void *)-1)
        {
            MLOGI("shmat failed. \r\n");
            exit(-1);
        }
        MLOGI( "PHOTOMNG_JpegDataExtract sharememory at 0x%X \r\n",(unsigned int) shm);
        shmdata = (share_memory *)shm;

        if(shmdata->read_sendflag==1)
        {
           return HI_PHOTOMNG_ENORES;     
        }
        
                
        shmdata->read_sendflag=1;
        shmdata->read_sendflagend = 0;
        MLOGI("shmdata->read_sendflag %d\n",shmdata->read_sendflag);

        /*fifo bt eason*/
        const char *fifo_name = "/app/my_fifo";
        int pipe_fd = -1;
        int res = 0;
        const int open_mode = O_WRONLY;
        if(access(fifo_name, F_OK) == -1)
        {
            MLOGI ("Create the fifo pipe.\n");
            res = mkfifo(fifo_name, 0777);
            if(res != 0)
            {
                MLOGI( "Could not create fifo %s\n", fifo_name);
            }
        }

        MLOGI("Process %d opening FIFO O_WRONLY\n", getpid());
        
        pipe_fd = open(fifo_name, open_mode);
        MLOGI("Process %d result %d\n", getpid(), pipe_fd);
   
    /***********************************/

    
    if (NULL != pstFile)
    {
        if (0 < pstFile->u32PreAllocLen && u32DataLen > pstFile->u32PreAllocLen)
        {
            MLOGE("pre alloc size(%u) is smaller than data len(%u)\n", pstFile->u32PreAllocLen, u32DataLen);
            return HI_PHOTOMNG_ENORES;
        }
        
        //    MLOGI("BE pstFile->szFileName %s\n", pstFile->szFileName);
            sprintf(pstFile->szFileName,"/app/temp/POTOTEST_0.jpg");
        //  MLOGI(" pstFile->szFileName %s\n", pstFile->szFileName);
       /* s32fd = open(pstFile->szFileName, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);

        if (0 > s32fd)
        {
            MLOGE("open %s fail\n", pstFile->szFileName);
            return HI_PHOTOMNG_EOTHER;
        }*/
    }

    if (NULL != pstImageData)
    {
        /*NOT save file,just copy date to pstImageData->pu8DataBuf*/
        pstImageData->u32DataLen = u32DataLen;
        pstImageData->pu8DataBuf = (HI_U8*)malloc(u32DataLen);

        if (NULL == pstImageData->pu8DataBuf)
        {
            MLOGE("malloc buffer fail\n");
            return HI_PHOTOMNG_ENORES;
        }

        pu8TmpBuf = pstImageData->pu8DataBuf;
    }
    for ( i = 0 ; i < pstVStreamData->u32PackCount; i++ )
    {
        HI_U64 u64PackPhyAddr = pstVStreamData->astPack[i].au64PhyAddr[0];
        HI_U32 u32PackLen = pstVStreamData->astPack[i].au32Len[0];
        HI_U32 u32PackOffset = pstVStreamData->astPack[i].u32Offset;
        HI_U64 u64DataPhyAddr = pstVStreamData->astPack[i].au64PhyAddr[1];
        HI_U32 u32DataLen = pstVStreamData->astPack[i].au32Len[1];
        HI_U64 send_data_len=0;
        //  MLOGI(" u64PackPhyAddr %lld\n",u64PackPhyAddr);          
       //   MLOGI(" u32PackLen %d\n",u32PackLen);          
        //  MLOGI(" u64DataPhyAddr %lld\n",u64DataPhyAddr);          
       //   MLOGI(" u32DataLen %d\n",u32DataLen);  
       /* data */   
       if (u64PackPhyAddr + u32PackLen >= u64DataPhyAddr + u32DataLen)
        {
            if (u64PackPhyAddr + u32PackOffset >= u64DataPhyAddr + u32DataLen)
            {     
               send_data_len+=u32PackLen - u32PackOffset;
            }
            else
            {
               HI_U32 u32Left = (u64DataPhyAddr + u32DataLen) - u64PackPhyAddr;
               send_data_len+=u32Left - u32PackOffset;           
            }
        }
        else
        {            
               send_data_len+= u32PackLen - u32PackOffset;              
        }
        
        shmdata->POTO_len=send_data_len;
    }

    for ( i = 0 ; i < pstVStreamData->u32PackCount; i++ )
    {
        HI_U64 u64PackPhyAddr = pstVStreamData->astPack[i].au64PhyAddr[0];
        HI_U32 u32PackLen = pstVStreamData->astPack[i].au32Len[0];
        HI_U32 u32PackOffset = pstVStreamData->astPack[i].u32Offset;
        HI_U64 u64DataVirtAddr = (HI_U64)(HI_UL)pstVStreamData->astPack[i].apu8Addr[1];
        HI_U64 u64DataPhyAddr = pstVStreamData->astPack[i].au64PhyAddr[1];
        HI_U32 u32DataLen = pstVStreamData->astPack[i].au32Len[1];
        HI_VOID* pvPackVirtAddr = pstVStreamData->astPack[i].apu8Addr[0];
       
        /*Send data*/     
        if (u64PackPhyAddr + u32PackLen >= u64DataPhyAddr + u32DataLen)
        {
            /* physical address retrace in offset segment */
            if (u64PackPhyAddr + u32PackOffset >= u64DataPhyAddr + u32DataLen)
            {     
                MLOGI("DO type1\n");       
                HI_VOID* pSrcVirtAddr = (HI_VOID*)(HI_UL)(u64DataVirtAddr + ((u64PackPhyAddr + u32PackOffset) - (u64DataPhyAddr + u32DataLen)));

            /*    if (0 < s32fd)
                {
                    if (0 > write(s32fd, pSrcVirtAddr, u32PackLen - u32PackOffset))
                    {
                        MLOGE("write error\n");
                    }
                }*/
                MLOGI("POTO GO %d\n",__LINE__);
                if(0<pipe_fd)
                {
                    if(0 > write(pipe_fd, pSrcVirtAddr, u32PackLen - u32PackOffset))
                    {
                         MLOGE( "Write error on pipe\n");
                    }
                }
               
                if (pu8TmpBuf)
                {
                    memcpy(pu8TmpBuf, pSrcVirtAddr, u32PackLen - u32PackOffset);
                    pu8TmpBuf = pu8TmpBuf + u32PackLen - u32PackOffset;
                }

                if (pstBufSource)
                {
                    if (n < HI_EXIF_DATA_SEG_MAX_CNT)
                    {
                        pstBufSource->pImgBuf[n] = pSrcVirtAddr;
                        pstBufSource->u32BufSize[n++] = u32PackLen - u32PackOffset;
                    }
                    else
                    {
                        MLOGE("reach exif data seg max cnt\n");
                    }
                }
            }
            else
            {
                MLOGI("POTO GO %d\n",__LINE__);
                MLOGI("DO type2\n");   
                /* physical address retrace in data segment */
                HI_U32 u32Left = (u64DataPhyAddr + u32DataLen) - u64PackPhyAddr;
            /*
                if (0 < s32fd)
                {
                    if (0 > write(s32fd, (HI_VOID*)((HI_UL)pvPackVirtAddr + u32PackOffset), u32Left - u32PackOffset))
                    {
                        MLOGE("write error\n");
                    }

                    if (0 > write(s32fd, (HI_VOID*)(HI_UL)u64DataVirtAddr, u32PackLen - u32Left))
                    {
                        MLOGE("write error\n");
                    }
                }*/
                MLOGI("POTO GO %d\n",__LINE__);
                if(0<pipe_fd)
                {
                    MLOGI("POTO GO %d\n",__LINE__);
                    if (0 > write(pipe_fd, (HI_VOID*)((HI_UL)pvPackVirtAddr + u32PackOffset), u32Left - u32PackOffset))
                    {
                        MLOGE("pipe_fd write error\n");
                    }
                    MLOGI("POTO GO %d\n",__LINE__);
                    if (0 > write(pipe_fd, (HI_VOID*)(HI_UL)u64DataVirtAddr, u32PackLen - u32Left))
                    {
                        MLOGE("pipe_fd write error\n");
                    }
                }

        
                if (pu8TmpBuf)
                {
                    memcpy(pu8TmpBuf, (HI_VOID*)((HI_UL)pvPackVirtAddr + u32PackOffset), u32Left - u32PackOffset);
                    pu8TmpBuf = pu8TmpBuf + u32Left - u32PackOffset;
                    memcpy(pu8TmpBuf, (HI_VOID*)(HI_UL)u64DataVirtAddr, u32PackLen - u32Left);
                    pu8TmpBuf = pu8TmpBuf + u32PackLen - u32Left;
                }

                if (pstBufSource)
                {
                    if (n < HI_EXIF_DATA_SEG_MAX_CNT - 1)
                    {
                        pstBufSource->pImgBuf[n] = (HI_VOID*)((HI_UL)pvPackVirtAddr + u32PackOffset);
                        pstBufSource->u32BufSize[n++] = u32Left - u32PackOffset;
                        pstBufSource->pImgBuf[n] = (HI_VOID*)(HI_UL)u64DataVirtAddr;
                        pstBufSource->u32BufSize[n++] = u32PackLen - u32Left;
                    }
                    else
                    {
                        MLOGE("reach exif data seg max cnt\n");
                    }
                }
            }
        }
        else
        {
            /* physical address retrace does not happen */
         /*   if (0 < s32fd)
            {
                if (0 > write(s32fd, (HI_VOID*)((HI_UL)pvPackVirtAddr + u32PackOffset), u32PackLen - u32PackOffset))
                {
                    MLOGE("write error\n");
                }
            }*/
            if (0 < pipe_fd)
            {
                MLOGI("POTO GO %d  %d   %d\n",__LINE__,i ,(u32PackLen - u32PackOffset));
                if (0 > write(pipe_fd, (HI_VOID*)((HI_UL)pvPackVirtAddr + u32PackOffset), u32PackLen - u32PackOffset))
                {
                    MLOGE("pipe_fd write error\n");
                }
                
                MLOGI("POTO GO %d\n",__LINE__);
            }
            
                MLOGI("POTO GO %d\n",__LINE__);
            if (pu8TmpBuf)
            {
                memcpy(pu8TmpBuf, (HI_VOID*)((HI_UL)pvPackVirtAddr + u32PackOffset), u32PackLen - u32PackOffset);
                pu8TmpBuf = pu8TmpBuf + u32PackLen - u32PackOffset;
            }

            if (pstBufSource)
            {
                if (n < HI_EXIF_DATA_SEG_MAX_CNT)
                {
                    pstBufSource->pImgBuf[n] = (HI_VOID*)((HI_UL)pvPackVirtAddr + u32PackOffset);
                    pstBufSource->u32BufSize[n++] = u32PackLen - u32PackOffset;
                }
                else
                {
                    MLOGE("reach exif data seg max cnt\n");
                }
            }
        }
    }
    

    MLOGI("POTO GO %d\n",__LINE__);

    if(shmdata->read_sendflag==1)
    {
	    shmdata->read_sendflagend=1;
    }
    MLOGI("shmdata->read_sendflagend  %d\n",shmdata->read_sendflagend);
   /* if (0 < s32fd)
    {
        if (u32DataLen < pstFile->u32PreAllocLen)
        {
            ftruncate(s32fd, pstFile->u32PreAllocLen);
        }

        fsync(s32fd);
        close(s32fd);
    }*/
    
    if (0 < pipe_fd)
    {
        close(pipe_fd);
    }
    printf("Process %d finished\n", getpid());
	if(shmdt(shm) == -1)
	{
		MLOGI("shmdt failed. \r\n");
		exit(-1);
	}
    return HI_SUCCESS;
}

#ifdef CFG_EXIF
static HI_S32 PHOTOMNG_EXIF_Muxer(HI_EXIF_BUFFER_SOURCE_S* pstBufSource, HI_PHOTOMNG_FILENAME_S* pstFile)
{
    #define EXIF_TAG_XMPPACKET          (0x02BC)
    #define EXIF_TAG_UNIQUECAMERAMODEL  (0xC614)
    #define EXIF_TAG_MAKERNOTE          (0x927C)
    HI_S32 s32Ret =  HI_SUCCESS;
    HI_PHOTOMNG_JPEG_USERINFO_S stJpegUserInfo;
    memset(&stJpegUserInfo, 0x00, sizeof(HI_PHOTOMNG_JPEG_USERINFO_S));
    s32Ret = s_stPHOTOMNGCtx.pfnGetJpegUserInfoCB(&stJpegUserInfo);

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("pfnGetJpegUserInfoCB failed:[0x%08X]\n", s32Ret);
        return s32Ret;
    }

    HI_MW_PTR hExifHandle = NULL;
    HI_EXIF_ATTR_S stExifAttr;
    stExifAttr.enExifConfigType = HI_EXIF_CONFIG_EDITOR;
    stExifAttr.stEditorConfig.pszFileName = pstFile->szFileName;
    stExifAttr.stEditorConfig.bIsOverWrite = (pstFile->u32PreAllocLen > 0) ? HI_TRUE : HI_FALSE;
    stExifAttr.stEditorConfig.u32FixedFileSize = pstFile->u32PreAllocLen;
    stExifAttr.stEditorConfig.u32MaxExifHeaderSize = 0;
    s32Ret = HI_EXIF_Open(&hExifHandle, &stExifAttr);

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("HI_EXIF_Open failed:[0x%08X]\n", s32Ret);
        return s32Ret;
    }

    s32Ret = HI_EXIF_SetBufferSource(hExifHandle, pstBufSource);

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("HI_EXIF_SetBufferSource failed:[0x%08X]\n", s32Ret);
        goto DESTROY_MUXER;
    }

    HI_EXIF_TAG_ENTRY_S stExifTagEntry;

    /*UniqueCameraModel*/
    if (0 < stJpegUserInfo.u32UniqueCameraModelLen && NULL != stJpegUserInfo.pUniqueCameraModel)
    {
        stExifTagEntry.u16TagNum = EXIF_TAG_UNIQUECAMERAMODEL;
        stExifTagEntry.enDataType = HI_EXIF_DATA_TYPE_ASCII;
        stExifTagEntry.u32Cnt = stJpegUserInfo.u32UniqueCameraModelLen;
        stExifTagEntry.pValue = stJpegUserInfo.pUniqueCameraModel;
        s32Ret = HI_EXIF_SetTag(hExifHandle, &stExifTagEntry, HI_EXIF_APP1_IFD0);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("HI_EXIF_SetTag failed:[0x%08X]\n", s32Ret);
            goto DESTROY_MUXER;
        }
    }

    /*MakerNote*/
    if (0 < stJpegUserInfo.u32MakerNoteLen && NULL != stJpegUserInfo.pMakerNote)
    {
        stExifTagEntry.u16TagNum = EXIF_TAG_MAKERNOTE;
        stExifTagEntry.enDataType = HI_EXIF_DATA_TYPE_UNDEFINED;
        stExifTagEntry.u32Cnt = stJpegUserInfo.u32MakerNoteLen;
        stExifTagEntry.pValue = stJpegUserInfo.pMakerNote;
        s32Ret = HI_EXIF_SetTag(hExifHandle, &stExifTagEntry, HI_EXIF_APP1_EXIFIFD);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("HI_EXIF_SetTag failed:[0x%08X]\n", s32Ret);
            goto DESTROY_MUXER;
        }
    }

    /*XMPPacket*/
    if (0 < stJpegUserInfo.u32XMPPacketLen && NULL != stJpegUserInfo.pXMPPacket)
    {
        stExifTagEntry.u16TagNum = EXIF_TAG_XMPPACKET;
        stExifTagEntry.enDataType = HI_EXIF_DATA_TYPE_BYTE;
        stExifTagEntry.u32Cnt = stJpegUserInfo.u32XMPPacketLen;
        stExifTagEntry.pValue = stJpegUserInfo.pXMPPacket;
        s32Ret = HI_EXIF_SetTag(hExifHandle, &stExifTagEntry, HI_EXIF_APP1_IFD0);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("HI_EXIF_SetTag failed:[0x%08X]\n", s32Ret);
            goto DESTROY_MUXER;
        }
    }

DESTROY_MUXER:
    //destroy muxer
    s32Ret = HI_EXIF_Close(hExifHandle);

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("HI_EXIF_Close failed:[0x%08X]\n", s32Ret);
    }

    return s32Ret;
}
#endif

static HI_S32 PHOTOMNG_SaveJPG(PHOTOMNG_TASK_CTX_S* pstTskContext, HI_MAPI_VENC_DATA_S* pstVStreamData)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_S32 s32Idx = 0;

    if (HI_PHOTOMNG_OUTPUT_FORMAT_DNG_JPEG == pstTskContext->stPhotoAttr.enOutPutFmt)
    {
        s32Idx = 1;
    }

    HI_EVENT_S stEvent;
    snprintf(stEvent.aszPayload, EVENT_PAYLOAD_LEN, pstTskContext->stFileName[s32Idx].szFileName);
    stEvent.EventID = HI_EVENT_PHOTOMNG_FILE_BEGIN;
    PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
    pstTskContext->bEnableDCF=HI_FAILURE;
    #ifdef CFG_EXIF

    if (HI_TRUE == pstTskContext->bEnableDCF&& NULL != s_stPHOTOMNGCtx.pfnGetJpegUserInfoCB)
    {
        HI_EXIF_BUFFER_SOURCE_S stBufSource;
        memset(&stBufSource, 0, sizeof(HI_EXIF_BUFFER_SOURCE_S));
        
        MLOGI("1. PHOTOMNG_JpegDataExtract\n");
        s32Ret = PHOTOMNG_JpegDataExtract(pstVStreamData, NULL, NULL, &stBufSource);

        if (HI_SUCCESS != s32Ret)
        {
            stEvent.EventID = HI_EVENT_PHOTOMNG_ERROR;
            PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
            return s32Ret;
        }

        s32Ret = PHOTOMNG_EXIF_Muxer(&stBufSource, &pstTskContext->stFileName[s32Idx]);
    }
    else
    #endif
    {
        MLOGI("2. PHOTOMNG_JpegDataExtract\n");
        s32Ret = PHOTOMNG_JpegDataExtract(pstVStreamData, &pstTskContext->stFileName[s32Idx], NULL, NULL);

        if (HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }

    if (HI_SUCCESS != s32Ret)
    {
        stEvent.EventID = HI_EVENT_PHOTOMNG_ERROR;
        PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
    }
    else
    {
        stEvent.EventID = HI_EVENT_PHOTOMNG_FILE_END;
        PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
    }

    return s32Ret;
}

#ifdef CFG_DNG
static HI_S32 PHOTOMNG_FillThumbNailIFD(PHOTOMNG_TASK_CTX_S*  pstTskContext, HI_MAPI_VENC_DATA_S* pstVStreamData)
{
    HI_S32 s32Ret = HI_FAILURE;
    PHOTOMNG_DNG_DATA_CTX_S* pstDngData = &pstTskContext->stDngDataCTX;
    MLOGI("--------thm %d arrived--------\n", pstDngData->u32ThmIndex);
    s32Ret = PHOTOMNG_JpegDataExtract(pstVStreamData, NULL, &pstDngData->astDngDataBuf[pstDngData->u32ThmIndex].stThmInfo.stThmIFD.stImageData, NULL);

    if (HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    HI_MUTEX_LOCK(pstDngData->DataMutex);
    pstDngData->astDngDataBuf[pstDngData->u32ThmIndex].stThmInfo.bDataArrived = HI_TRUE;

    if (pstDngData->astDngDataBuf[pstDngData->u32ThmIndex].stScreenNailInfo.bDataArrived)
    {
        HI_COND_SIGNAL(pstDngData->DataCond);
    }

    pstDngData->u32ThmIndex++;
    HI_MUTEX_UNLOCK(pstDngData->DataMutex);
    return HI_SUCCESS;
}

static HI_S32 PHOTOMNG_FillScreenNailIFD(PHOTOMNG_TASK_CTX_S*  pstTskContext, HI_MAPI_VENC_DATA_S* pstVStreamData)
{
    HI_S32 s32Ret = HI_FAILURE;
    PHOTOMNG_DNG_DATA_CTX_S* pstDngData = &pstTskContext->stDngDataCTX;
    MLOGI("--------screennail %d arrived--------\n", pstDngData->u32ScreenIndex);

    if (HI_PHOTOMNG_OUTPUT_FORMAT_DNG == pstTskContext->stPhotoAttr.enOutPutFmt)
    {
        MLOGI("3. PHOTOMNG_JpegDataExtract\n");
        s32Ret = PHOTOMNG_JpegDataExtract(pstVStreamData, NULL, &pstDngData->astDngDataBuf[pstDngData->u32ScreenIndex].stScreenNailInfo.stScreenIFD.stImageData, NULL);

        if (HI_SUCCESS != s32Ret)
        {
            return s32Ret;
        }
    }
    else if (HI_PHOTOMNG_OUTPUT_FORMAT_DNG_JPEG == pstTskContext->stPhotoAttr.enOutPutFmt)
    {
        /*Muxer JPG*/
        PHOTOMNG_SaveJPG(pstTskContext, pstVStreamData);
    }

    HI_MUTEX_LOCK(pstDngData->DataMutex);
    pstDngData->astDngDataBuf[pstDngData->u32ScreenIndex].stScreenNailInfo.bDataArrived = HI_TRUE;

    if (pstDngData->astDngDataBuf[pstDngData->u32ScreenIndex].stThmInfo.bDataArrived)
    {
        HI_COND_SIGNAL(pstDngData->DataCond);
    }

    pstDngData->u32ScreenIndex++;
    HI_MUTEX_UNLOCK(pstDngData->DataMutex);
    return HI_SUCCESS;
}

static HI_S32 PHOTOMNG_RawDataProc(HI_HANDLE VcapPipeHdl, HI_MAPI_FRAME_DATA_S* pVCapRawData, HI_S32 s32DataNum, HI_VOID* pPrivateData)
{
    HI_S32 s32Ret = 0;
    HI_U32 u32DataLen = 0;

    PHOTOMNG_TASK_CTX_S* pstTskContext = (PHOTOMNG_TASK_CTX_S*)pPrivateData;
    PHOTOMNG_DNG_DATA_CTX_S* pstDngData = &(pstTskContext->stDngDataCTX);
    MLOGI("--------raw image %d arrived--------\n", pstDngData->u32RawIndex);
    DNG_IMAGE_STATIC_INFO_S stDngStaticInfo;
    memset(&stDngStaticInfo, 0x00, sizeof(DNG_IMAGE_STATIC_INFO_S));
    s32Ret = HI_MAPI_VCAP_GetAttrEx(VcapPipeHdl, pstTskContext->stPhotoAttr.stPhotoSRC.VcapPipeChnHdl,
                                    HI_MAPI_VCAP_CMD_ISP_GetDngImageStaticInfo, &stDngStaticInfo, sizeof(stDngStaticInfo));

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("HI_MAPI_VCAP_GetAttrEx fail, VcapPipeHdl:%d,s32Ret:[0x%08X]\n", VcapPipeHdl, s32Ret);
        return s32Ret;
    }

    switch (pVCapRawData->enPixelFormat)
    {
        case PIXEL_FORMAT_RGB_BAYER_8BPP:
            stDngStaticInfo.stDngRawFormat.u8BitsPerSample = 8;
            break;

        case PIXEL_FORMAT_RGB_BAYER_10BPP:
            stDngStaticInfo.stDngRawFormat.u8BitsPerSample = 10;
            break;

        case PIXEL_FORMAT_RGB_BAYER_12BPP:
            stDngStaticInfo.stDngRawFormat.u8BitsPerSample = 12;
            break;

        case PIXEL_FORMAT_RGB_BAYER_14BPP:
            stDngStaticInfo.stDngRawFormat.u8BitsPerSample = 14;
            break;

        case PIXEL_FORMAT_RGB_BAYER_16BPP:
            stDngStaticInfo.stDngRawFormat.u8BitsPerSample = 16;
            break;

        default:
            MLOGE("Unsupport RawFormat:%d\n", pVCapRawData->enPixelFormat);
            return s32Ret;
    }

    stDngStaticInfo.stDngRawFormat.u32WhiteLevel = pow(2, stDngStaticInfo.stDngRawFormat.u8BitsPerSample) - 1;

    JPEG_DCF_S* pstJpegDcf = (JPEG_DCF_S*)(pVCapRawData->stVideoSupplement.pJpegDCFVirAddr);
    DNG_IMAGE_DYNAMIC_INFO_S* pstDngDynamic = (DNG_IMAGE_DYNAMIC_INFO_S*)(pVCapRawData->stVideoSupplement.pFrameDNGVirAddr);

    u32DataLen = pVCapRawData->u32Height * pVCapRawData->u32Stride[0];

    HI_PHOTOMNG_DNG_USERINFO_S* pstDNGUserInfo = NULL;

    if (NULL != s_stPHOTOMNGCtx.pfnGetDNGUserInfoCB)
    {
        HI_PHOTOMNG_DNG_USERINFO_S stDNGUserInfo;
        s_stPHOTOMNGCtx.pfnGetDNGUserInfoCB(&stDNGUserInfo);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("pfnGetDNGUserInfoCB failed:[0x%08X]\n", s32Ret);
            return s32Ret;
        }

        pstDNGUserInfo = &stDNGUserInfo;
    }

    if (HI_PHOTOMNG_OUTPUT_FORMAT_DNG == pstTskContext->stPhotoAttr.enOutPutFmt)
    {
        pstDngData->astDngDataBuf[pstDngData->u32RawIndex].stRawInfo.stRawIFD.stImageData.u32DataLen = u32DataLen;
        pstDngData->astDngDataBuf[pstDngData->u32RawIndex].stRawInfo.stRawIFD.stImageData.u32Stride = pVCapRawData->u32Stride[0];
        pstDngData->astDngDataBuf[pstDngData->u32RawIndex].stRawInfo.stRawIFD.u32Height = pVCapRawData->u32Height;
        pstDngData->astDngDataBuf[pstDngData->u32RawIndex].stRawInfo.stRawIFD.u32Width = pVCapRawData->u32Width;

        HI_MAPI_VENC_ATTR_S stStreamAttr;
        s32Ret = HI_MAPI_VENC_GetAttr(pstTskContext->stPhotoAttr.stPhotoSRC.VencThmHdl, &stStreamAttr);

        if (HI_SUCCESS != s32Ret)
        {
            goto end;
        }

        PHOTOMNG_DNG_SetThmIFD(&(pstDngData->astDngDataBuf[pstDngData->u32RawIndex].stThmInfo.stThmIFD), stStreamAttr.stVencPloadTypeAttr.u32Width,
                               stStreamAttr.stVencPloadTypeAttr.u32Height,
                               pstDngDynamic, &stDngStaticInfo, pstJpegDcf, pstDNGUserInfo);

        s32Ret = HI_MAPI_VENC_GetAttr(pstTskContext->stPhotoAttr.stPhotoSRC.VencHdl, &stStreamAttr);

        if (HI_SUCCESS != s32Ret)
        {
            goto end;
        }

        PHOTOMNG_DNG_SetScreenIFD(&(pstDngData->astDngDataBuf[pstDngData->u32RawIndex].stScreenNailInfo.stScreenIFD), stStreamAttr.stVencPloadTypeAttr.u32Width, stStreamAttr.stVencPloadTypeAttr.u32Height);
        PHOTOMNG_DNG_SetRawIFD(&(pstDngData->astDngDataBuf[pstDngData->u32RawIndex].stRawInfo.stRawIFD), pstDngDynamic, &stDngStaticInfo);
        HI_MUTEX_LOCK(pstDngData->DataMutex);

        if ((HI_FALSE == pstDngData->astDngDataBuf[pstDngData->u32RawIndex].stThmInfo.bDataArrived)
            || (HI_FALSE == pstDngData->astDngDataBuf[pstDngData->u32RawIndex].stScreenNailInfo.bDataArrived))
        {
            MLOGI("wait AllDataRecvCond\n");
            HI_COND_TIMEDWAIT_WITH_RETURN(pstDngData->DataCond, pstDngData->DataMutex, PHOTOMNG_WAIT_DATA_TIMEOUT*1000*1000, s32Ret);
            MLOGI("wait AllDataRecvCond end\n");

            if (HI_SUCCESS != s32Ret)
            {
                HI_MUTEX_UNLOCK(pstDngData->DataMutex);
                MLOGE(" wait all data receive timeout, s32Ret:%d \n", s32Ret);
                goto end;
            }
        }

        HI_MUTEX_UNLOCK(pstDngData->DataMutex);

        HI_EVENT_S stEvent;
        snprintf(stEvent.aszPayload, EVENT_PAYLOAD_LEN, pstTskContext->stFileName[0].szFileName);
        stEvent.EventID = HI_EVENT_PHOTOMNG_FILE_BEGIN;
        PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
        pstDngData->astDngDataBuf[pstDngData->u32RawIndex].stRawInfo.stRawIFD.stImageData.pu8DataBuf = (HI_U8*)(HI_UL)pVCapRawData->u64VirAddr[0];
        s32Ret = PHOTOMNG_DNG_Muxer(&pstDngData->astDngDataBuf[pstDngData->u32RawIndex], pstTskContext->stFileName[0].szFileName, HI_FALSE);

        if (HI_SUCCESS != s32Ret)
        {
            stEvent.EventID = HI_EVENT_PHOTOMNG_ERROR;
            PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
        }
        else
        {
            stEvent.EventID = HI_EVENT_PHOTOMNG_FILE_END;
            PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
        }

        pstDngData->u32RawIndex++;
    }
    else if (HI_PHOTOMNG_OUTPUT_FORMAT_DNG_JPEG == pstTskContext->stPhotoAttr.enOutPutFmt)
    {
        pstDngData->astDngDataBuf[0].stRawInfo.stRawIFD.stImageData.u32DataLen = u32DataLen;
        pstDngData->astDngDataBuf[0].stRawInfo.stRawIFD.stImageData.u32Stride = pVCapRawData->u32Stride[0];
        pstDngData->astDngDataBuf[0].stRawInfo.stRawIFD.u32Height = pVCapRawData->u32Height;
        pstDngData->astDngDataBuf[0].stRawInfo.stRawIFD.u32Width = pVCapRawData->u32Width;

        PHOTOMNG_DNG_SetIFD0(&(pstDngData->astDngDataBuf[0].stRawInfo.stRawIFD), pstDngDynamic, &stDngStaticInfo, pstJpegDcf, pstDNGUserInfo);

        /*Muxer DNG*/
        HI_EVENT_S stEvent;
        snprintf(stEvent.aszPayload, EVENT_PAYLOAD_LEN, pstTskContext->stFileName[0].szFileName);
        stEvent.EventID = HI_EVENT_PHOTOMNG_FILE_BEGIN;
        PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
        pstDngData->astDngDataBuf[0].stRawInfo.stRawIFD.stImageData.pu8DataBuf = (HI_U8*)(HI_UL)pVCapRawData->u64VirAddr[0];
        s32Ret = PHOTOMNG_DNG_Muxer(&pstDngData->astDngDataBuf[0], pstTskContext->stFileName[0].szFileName, HI_TRUE);

        if (HI_SUCCESS != s32Ret)
        {
            stEvent.EventID = HI_EVENT_PHOTOMNG_ERROR;
            PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
        }
        else
        {
            stEvent.EventID = HI_EVENT_PHOTOMNG_FILE_END;
            PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
        }

        HI_MUTEX_LOCK(pstDngData->DataMutex);

        if (HI_FALSE == pstDngData->astDngDataBuf[0].stScreenNailInfo.bDataArrived)
        {
            /*Wait Muxer JPG*/
            MLOGI("wait jpg DataRecv\n");
            HI_COND_TIMEDWAIT_WITH_RETURN(pstDngData->DataCond, pstDngData->DataMutex, PHOTOMNG_WAIT_DATA_TIMEOUT*1000*1000, s32Ret);
            MLOGI("wait jpg DataRecv end\n");

            if (HI_SUCCESS != s32Ret)
            {
                HI_MUTEX_UNLOCK(pstDngData->DataMutex);
                MLOGE(" wait JPG data receive timeout, s32Ret:%d \n", s32Ret);
                goto end;
            }
        }

        HI_MUTEX_UNLOCK(pstDngData->DataMutex);
    }
    else
    {
        MLOGE("unsupport OutPutFmt:%d\n", pstTskContext->stPhotoAttr.enOutPutFmt);
        goto end;
    }

    HI_MUTEX_LOCK(pstTskContext->TaskMutex);
    pstTskContext->s32SavedCnt++;
    MLOGI("DngSaved %d \n", pstTskContext->s32SavedCnt);

    if (pstTskContext->s32SavedCnt >= pstTskContext->s32PhotoCnt)
    {
        HI_COND_SIGNAL(pstTskContext->TaskEndCond);
    }

    HI_MUTEX_UNLOCK(pstTskContext->TaskMutex);
end:
    return s32Ret;
}

static HI_S32 PHOTOMNG_ThmDataProc(HI_HANDLE VencHdl, HI_MAPI_VENC_DATA_S* pstVStreamData, HI_VOID* pvPrivateData)
{
    HI_S32 s32Ret = HI_FAILURE;
    PHOTOMNG_TASK_CTX_S* pstTskContext = (PHOTOMNG_TASK_CTX_S*)pvPrivateData;

    /*end of stream pack is invalid packet*/
    if ((HI_TRUE == pstVStreamData->bEndOfStream) && (0 == pstVStreamData->u32PackCount))
    {
        MLOGE("stream pack is invalid\n");
        return HI_SUCCESS;
    }

    if (HI_PHOTOMNG_OUTPUT_FORMAT_DNG == pstTskContext->stPhotoAttr.enOutPutFmt)
    {
        s32Ret = PHOTOMNG_FillThumbNailIFD(pstTskContext, pstVStreamData);
    }
    else
    {
        MLOGE("output format:%d unsupport independent thumbnail \n", pstTskContext->stPhotoAttr.enOutPutFmt );
    }

    return s32Ret;
}

static HI_S32 PHOTOMNG_DumpRaw(PHOTOMNG_TASK_CTX_S* pstTskContext)
{
    
    MLOGI("PHOTOMNG_DumpRaw\n");
    HI_S32 i = 0, s32Ret = HI_FAILURE;
    PHOTOMNG_DNG_DATA_CTX_S* pstDngData = &pstTskContext->stDngDataCTX;
    HI_S32 s32DngPhotoCount = pstTskContext->s32PhotoCnt;
    HI_EVENT_S stEvent;

    HI_MAPI_VCAP_RAW_DATA_S stVCapRawData;
    stVCapRawData.pPrivateData = (HI_VOID*)pstTskContext;
    stVCapRawData.pfn_VCAP_RawDataProc = PHOTOMNG_RawDataProc;
    MLOGI("HI_MAPI_VCAP_StartDumpRaw\n");
    s32Ret = HI_MAPI_VCAP_StartDumpRaw(pstTskContext->stPhotoAttr.stPhotoSRC.VcapPipeHdl, s32DngPhotoCount, &stVCapRawData);

    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_PrintFuncErr(HI_MAPI_VCAP_DumpRaw, s32Ret);
        goto DATA_FREE;
    }

    HI_MUTEX_LOCK(pstTskContext->TaskMutex);
    MLOGI("wait DngSavedCond \n");
    HI_COND_TIMEDWAIT_WITH_RETURN(pstTskContext->TaskEndCond, pstTskContext->TaskMutex, PHOTOMNG_WAIT_SAVE_TIMEOUT*1000*1000, s32Ret);
    HI_MUTEX_UNLOCK(pstTskContext->TaskMutex);

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("wait DngSavedCond failed,s32Ret:%d  \n", s32Ret);
    }

    MLOGI("wait DngSavedCond end\n");
    HI_MAPI_VCAP_StopDumpRaw(pstTskContext->stPhotoAttr.stPhotoSRC.VcapPipeHdl);

    HI_MAPI_DUMP_ATTR_S stDumpAttr;
    memset(&stDumpAttr,0,sizeof(HI_MAPI_DUMP_ATTR_S));
    HI_MAPI_VCAP_SetDumpRawAttr(pstTskContext->stPhotoAttr.stPhotoSRC.VcapPipeHdl, &stDumpAttr);

DATA_FREE:
    HI_MUTEX_LOCK(pstDngData->DataMutex);

    for (i = 0; i < s32DngPhotoCount; i++)
    {
        HI_APPCOMM_SAFE_FREE(pstTskContext->stDngDataCTX.astDngDataBuf[i].stThmInfo.stThmIFD.stImageData.pu8DataBuf);
        HI_APPCOMM_SAFE_FREE(pstTskContext->stDngDataCTX.astDngDataBuf[i].stScreenNailInfo.stScreenIFD.stImageData.pu8DataBuf);
        pstTskContext->stDngDataCTX.astDngDataBuf[i].stScreenNailInfo.bDataArrived = HI_FALSE;
        pstTskContext->stDngDataCTX.astDngDataBuf[i].stThmInfo.bDataArrived = HI_FALSE;
    }

    pstTskContext->stDngDataCTX.u32RawIndex = 0;
    pstTskContext->stDngDataCTX.u32ThmIndex = 0;
    pstTskContext->stDngDataCTX.u32ScreenIndex = 0;
    HI_MUTEX_UNLOCK(pstDngData->DataMutex);

    PHOTOMNG_VencStop(pstTskContext);

    HI_MUTEX_LOCK(pstTskContext->TaskMutex);

    if (HI_PHOTOMNG_PHOTO_TYPE_LAPSE != pstTskContext->stPhotoAttr.enPhotoType)
    {
        pstTskContext->enTaskState = PHOTOMNG_TASK_STATE_READY;
        stEvent.EventID = HI_EVENT_PHOTOMNG_TASK_END;
        stEvent.s32Result = pstTskContext->s32SavedCnt;
        PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
    }
    else if (pstTskContext->bLapseStop)/*lapse stopped*/
    {
        pstTskContext->enTaskState = PHOTOMNG_TASK_STATE_READY;
        pstTskContext->bLapseStop = HI_FALSE;
        HI_COND_SIGNAL(pstTskContext->TaskStoppedCond);
    }
    else/*lapse continue*/
    {
        pstTskContext->s32PhotoCnt = 1;
        pstTskContext->enTaskState = PHOTOMNG_TASK_STATE_TICKING;
    }

    MLOGI("signal TaskEndCond \n");
    HI_COND_SIGNAL(pstTskContext->TaskEndCond);
    HI_MUTEX_UNLOCK(pstTskContext->TaskMutex);
    return s32Ret;
}
#endif

static HI_S32 PHOTOMNG_JpegDataProc(HI_HANDLE VencHdl, HI_MAPI_VENC_DATA_S* pstVStreamData, HI_VOID* pvPrivateData)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_EVENT_S stEvent;
    PHOTOMNG_TASK_CTX_S* pstTskContext = (PHOTOMNG_TASK_CTX_S*)pvPrivateData;

    /*end of stream pack is invalid packet*/
    if ((HI_TRUE == pstVStreamData->bEndOfStream) && (0 == pstVStreamData->u32PackCount))
    {
        MLOGE("stream pack is invalid\n");
        HI_MUTEX_LOCK(pstTskContext->TaskMutex);
        HI_COND_SIGNAL(pstTskContext->TaskEndCond);
        HI_MUTEX_UNLOCK(pstTskContext->TaskMutex);
        return HI_SUCCESS;
    }

#ifdef CFG_DNG

    if (HI_PHOTOMNG_OUTPUT_FORMAT_DNG == pstTskContext->stPhotoAttr.enOutPutFmt)
    {
        s32Ret = PHOTOMNG_FillScreenNailIFD(pstTskContext, pstVStreamData);
    }
    else if (HI_PHOTOMNG_OUTPUT_FORMAT_DNG_JPEG == pstTskContext->stPhotoAttr.enOutPutFmt)
    {
        pstTskContext->stDngDataCTX.astDngDataBuf[0].stThmInfo.bDataArrived = HI_TRUE;
        s32Ret = PHOTOMNG_FillScreenNailIFD(pstTskContext, pstVStreamData);
    }
    else
#endif
    {
#ifdef CFG_TIME_STAMP_ON

        /*single photo or lapse but is not video ,need print time from trigger to venc end*/
        if (((HI_PHOTOMNG_PHOTO_TYPE_SINGLE == pstTskContext->stPhotoAttr.enPhotoType)
             && (pstTskContext->stPhotoAttr.unPhotoTypeAttr.stSingleAttr.bNeedTrigger))
            || ((HI_PHOTOMNG_PHOTO_TYPE_LAPSE == pstTskContext->stPhotoAttr.enPhotoType)
                && (pstTskContext->stPhotoAttr.unPhotoTypeAttr.stLapseAttr.bNeedTrigger)))
        {
            clock_gettime(CLOCK_MONOTONIC, &s_PHOTOMNGEndTime);
            MLOGI("time trigger to venc end: %ld ms \n", (s_PHOTOMNGEndTime.tv_sec * 1000 + s_PHOTOMNGEndTime.tv_nsec / 1000000) - (s_PHOTOMNGBeginTime.tv_sec * 1000 + s_PHOTOMNGBeginTime.tv_nsec / 1000000 ));
        }
        else if (HI_PHOTOMNG_PHOTO_TYPE_MULTIPLE == pstTskContext->stPhotoAttr.enPhotoType) /*multiple photo need print the interval time between two frame capture*/
        {
            if (0 == s_PhotoMngLastPackPTS)
            {
                s_PhotoMngLastPackPTS = pstVStreamData->astPack[0].u64PTS;
                MLOGI("this is first frame \n");
            }
            else
            {
                MLOGI("the capture time from last frame: %llu us \n", pstVStreamData->astPack[0].u64PTS - s_PhotoMngLastPackPTS);
                s_PhotoMngLastPackPTS = pstVStreamData->astPack[0].u64PTS;
            }
        }

#endif

        if (HI_NULL != pstTskContext->stCfg.stGetNameCB.pfnGetNameCallBack && HI_PHOTOMNG_PHOTO_TYPE_MULTIPLE == pstTskContext->stPhotoAttr.enPhotoType)
        {
            //MLOGI("pstTskContext->stFileName %s \n", pstTskContext->stFileName);
            s32Ret = pstTskContext->stCfg.stGetNameCB.pfnGetNameCallBack(1, pstTskContext->stFileName, pstTskContext->stCfg.stGetNameCB.pvPrivateData);

            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("get file name fail,s32Ret:%x \n", s32Ret);
                stEvent.EventID = HI_EVENT_PHOTOMNG_ERROR;
                PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
                return HI_PHOTOMNG_ETAKEPHOTO;
            }
        }

        s32Ret = PHOTOMNG_SaveJPG(pstTskContext, pstVStreamData);
        HI_MUTEX_LOCK(pstTskContext->TaskMutex);

        if (HI_SUCCESS == s32Ret)
        {
            pstTskContext->s32SavedCnt++;
        }

        if (pstTskContext->s32PhotoCnt > 0)
        {
            pstTskContext->s32PhotoCnt--;
        }

        if (((HI_TRUE == pstVStreamData->bEndOfStream) && (HI_MAPI_VENC_LIMITLESS_FRAME_COUNT != pstTskContext->s32PhotoCnt))
            || (0 == pstTskContext->s32PhotoCnt))
        {
            if (HI_PHOTOMNG_PHOTO_TYPE_LAPSE != pstTskContext->stPhotoAttr.enPhotoType)
            {
                pstTskContext->enTaskState = PHOTOMNG_TASK_STATE_READY;
                stEvent.EventID = HI_EVENT_PHOTOMNG_TASK_END;
                stEvent.s32Result = pstTskContext->s32SavedCnt;
                PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
            }
            else if (pstTskContext->bLapseStop)/*lapse stopped*/
            {
                pstTskContext->enTaskState = PHOTOMNG_TASK_STATE_READY;
                stEvent.EventID = HI_EVENT_PHOTOMNG_TASK_END;
                stEvent.s32Result = pstTskContext->s32SavedCnt;
                PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
                pstTskContext->bLapseStop = HI_FALSE;
            }
            else/*lapse continue*/
            {
                pstTskContext->s32PhotoCnt = 1;
                pstTskContext->enTaskState = PHOTOMNG_TASK_STATE_TICKING;
            }

            MLOGI("signal TaskEndCond \n");
            HI_COND_SIGNAL(pstTskContext->TaskEndCond);
        }

        HI_MUTEX_UNLOCK(pstTskContext->TaskMutex);
    }

    return s32Ret;
}

static HI_S32 PHOTOMNG_VencStart(PHOTOMNG_TASK_CTX_S* pstTaskContext)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_MAPI_VENC_CALLBACK_S  stVencCb;
    HI_PHOTOMNG_PHOTO_SRC_S* pstPhotoSrc = &pstTaskContext->stPhotoAttr.stPhotoSRC;

    MLOGI("venc start VencHdl:%d\n", pstPhotoSrc->VencHdl);
    stVencCb.pfnDataCB = PHOTOMNG_JpegDataProc;
    stVencCb.pPrivateData = pstTaskContext;
    s32Ret = HI_MAPI_VENC_RegisterCallback(pstPhotoSrc->VencHdl, &stVencCb);

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("HI_MAPI_VENC_RegisterCallback fail,VencHdl:%d  \n", pstPhotoSrc->VencHdl);
        return HI_PHOTOMNG_EOTHER;
    }
    MLOGI("pstTaskContext->s32PhotoCnt:%d\n", pstTaskContext->s32PhotoCnt);
    s32Ret =  s_stPHOTOMNGCtx.stVencOps.pfnStart(pstPhotoSrc->VencHdl, pstTaskContext->s32PhotoCnt);

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("venc start fail,VencHdl:%d\n", pstPhotoSrc->VencHdl);
        HI_MAPI_VENC_UnRegisterCallback(pstPhotoSrc->VencHdl, &stVencCb);
        return HI_PHOTOMNG_EOTHER;
    }

#ifdef CFG_DNG

    /*if output format is dng,need start another jpeg venc for thumbnail*/
    if (HI_PHOTOMNG_OUTPUT_FORMAT_DNG == pstTaskContext->stPhotoAttr.enOutPutFmt)
    {
        HI_MAPI_VENC_CALLBACK_S  stThmVencCb;
        stThmVencCb.pfnDataCB = PHOTOMNG_ThmDataProc;
        stThmVencCb.pPrivateData = pstTaskContext;
        s32Ret = HI_MAPI_VENC_RegisterCallback(pstPhotoSrc->VencThmHdl, &stThmVencCb);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("HI_MAPI_VENC_RegisterCallback fail,VencHdl:%d  \n", pstPhotoSrc->VencThmHdl);
            s_stPHOTOMNGCtx.stVencOps.pfnStop(pstPhotoSrc->VencHdl);
            HI_MAPI_VENC_UnRegisterCallback(pstPhotoSrc->VencHdl, &stVencCb);
            return HI_PHOTOMNG_EOTHER;
        }

        s32Ret = s_stPHOTOMNGCtx.stVencOps.pfnStart(pstPhotoSrc->VencThmHdl, pstTaskContext->s32PhotoCnt);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("venc start fail,VencHdl:%d\n", pstPhotoSrc->VencHdl);
            s_stPHOTOMNGCtx.stVencOps.pfnStop(pstPhotoSrc->VencHdl);
            HI_MAPI_VENC_UnRegisterCallback(pstPhotoSrc->VencHdl, &stVencCb);
            HI_MAPI_VENC_UnRegisterCallback(pstPhotoSrc->VencThmHdl, &stThmVencCb);
            return HI_PHOTOMNG_EOTHER;
        }
    }

#endif
    return s32Ret;
}

static HI_S32 PHOTOMNG_VencStop(PHOTOMNG_TASK_CTX_S* pstTaskContext)
{
    HI_MAPI_VENC_CALLBACK_S  stVencCb;
    HI_PHOTOMNG_PHOTO_SRC_S* pstPhotoSrc = &pstTaskContext->stPhotoAttr.stPhotoSRC;

    stVencCb.pfnDataCB = PHOTOMNG_JpegDataProc;
    stVencCb.pPrivateData = pstTaskContext;
    s_stPHOTOMNGCtx.stVencOps.pfnStop(pstPhotoSrc->VencHdl);
    HI_MAPI_VENC_UnRegisterCallback(pstPhotoSrc->VencHdl, &stVencCb);

#ifdef CFG_DNG

    /*if output format is dng,need start another jpeg venc for thumbnail*/
    if (HI_PHOTOMNG_OUTPUT_FORMAT_DNG == pstTaskContext->stPhotoAttr.enOutPutFmt)
    {
        HI_MAPI_VENC_CALLBACK_S stThmVencCb;
        stThmVencCb.pfnDataCB = PHOTOMNG_ThmDataProc;
        stThmVencCb.pPrivateData = pstTaskContext;
        s_stPHOTOMNGCtx.stVencOps.pfnStop(pstPhotoSrc->VencThmHdl);
        HI_MAPI_VENC_UnRegisterCallback(pstPhotoSrc->VencHdl, &stThmVencCb);
    }

#endif
    return HI_SUCCESS;
}

static HI_S32 PHOTOMNG_WaitJPEGFinish(PHOTOMNG_TASK_CTX_S* pstTskContext)
{
    HI_S32 s32Ret = HI_SUCCESS;

    HI_MUTEX_LOCK(pstTskContext->TaskMutex);

    if (PHOTOMNG_TASK_STATE_READY !=  pstTskContext->enTaskState)
    {
        MLOGI("wait TaskEndCond \n");
        HI_COND_TIMEDWAIT_WITH_RETURN(pstTskContext->TaskEndCond, pstTskContext->TaskMutex,
            PHOTOMNG_WAIT_SAVE_TIMEOUT * pstTskContext->s32PhotoCnt * 1000 * 1000, s32Ret);
        if (HI_SUCCESS != s32Ret)
        {
            pstTskContext->enTaskState = PHOTOMNG_TASK_STATE_READY;         /*restore task state to ready*/
            MLOGE("pthread_cond_timedwait failed, s32Ret:%d \n", s32Ret);
            s32Ret = HI_PHOTOMNG_ETAKEPHOTO;
        }
    }

    HI_COND_SIGNAL(pstTskContext->TaskStoppedCond);
    HI_MUTEX_UNLOCK(pstTskContext->TaskMutex);
    PHOTOMNG_VencStop(pstTskContext);
    return s32Ret;
}

static HI_S32 PHOTOMNG_TakePhoto(PHOTOMNG_TASK_CTX_S* pstTaskContext)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_EVENT_S stEvent;
    HI_BOOL bNeedTrigger = HI_FALSE;
    HI_S32 s32FileCnt = 1;
    HI_PHOTOMNG_PHOTO_SRC_S* pstPhotoSrc = &pstTaskContext->stPhotoAttr.stPhotoSRC;

    MLOGI("PHOTOMNG_TakePhoto  %d\n",pstTaskContext->stPhotoAttr.enPhotoType);
    if (HI_PHOTOMNG_PHOTO_TYPE_SINGLE == pstTaskContext->stPhotoAttr.enPhotoType)
    {
        bNeedTrigger = pstTaskContext->stPhotoAttr.unPhotoTypeAttr.stSingleAttr.bNeedTrigger;
    }
    else if (HI_PHOTOMNG_PHOTO_TYPE_LAPSE == pstTaskContext->stPhotoAttr.enPhotoType)
    {
        bNeedTrigger = pstTaskContext->stPhotoAttr.unPhotoTypeAttr.stLapseAttr.bNeedTrigger;
    }

    HI_PHOTOMNG_PHOTO_PARAM_S stPhotoParam;
    memset(&stPhotoParam, 0x00, sizeof(HI_PHOTOMNG_PHOTO_PARAM_S));
    s32Ret = pstTaskContext->stCfg.stGetParamCB.pfnGetParamCallBack(&stPhotoParam, pstTaskContext->stCfg.stGetParamCB.pvPrivateData);

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("pfnGetParamCallBack fail,s32Ret:[0x%08X]\n", s32Ret);
        goto TASK_STOP;
    }

    /**1.set exif attr*/
    
    MLOGI("set exif attr  \n");
    if (0 < strnlen((const HI_CHAR*)stPhotoParam.stExifInfo.au8ImageDescription, HI_MAPI_EXIF_DRSCRIPTION_LENGTH))
    {
        HI_MAPI_VENC_ATTR_S stStreamAttr;
        
    MLOGI("Do HI_MAPI_VENC_GetAttr \n");
        s32Ret = HI_MAPI_VENC_GetAttr(pstPhotoSrc->VencHdl, &stStreamAttr);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("HI_MAPI_VENC_GetAttr fail,s32Ret:[0x%08X],VencHdl:%d  \n", s32Ret, pstPhotoSrc->VencHdl);
            goto TASK_STOP;
        }

        if (HI_MAPI_PAYLOAD_TYPE_JPEG == stStreamAttr.stVencPloadTypeAttr.enType
            && HI_TRUE == stStreamAttr.stVencPloadTypeAttr.stAttrJpege.bEnableDCF)
        {
            MLOGI("Do HI_MAPI_VCAP_SetExifInfo \n");
            s32Ret = HI_MAPI_VCAP_SetExifInfo(pstPhotoSrc->VcapPipeHdl, &stPhotoParam.stExifInfo);

            if (HI_SUCCESS != s32Ret)
            {
                MLOGE("HI_MAPI_VCAP_SetExifInfo fail,s32Ret:[0x%08X],VcapPipeHdl:%d  \n", s32Ret, pstPhotoSrc->VcapPipeHdl);
                goto TASK_STOP;
            }

            pstTaskContext->bEnableDCF = HI_TRUE;
        }
        else
        {
            MLOGW("jpeg venc [%d] dcf is not enable !\n", pstPhotoSrc->VencHdl);
        }
    }

    if (HI_NULL != pstTaskContext->stCfg.stGetNameCB.pfnGetNameCallBack && HI_PHOTOMNG_PHOTO_TYPE_MULTIPLE != pstTaskContext->stPhotoAttr.enPhotoType)
    {
        if (HI_PHOTOMNG_OUTPUT_FORMAT_DNG_JPEG == pstTaskContext->stPhotoAttr.enOutPutFmt)
        {
            s32FileCnt = 2;
        }
        MLOGI("Do pfnGetNameCallBack \n");
        s32Ret = pstTaskContext->stCfg.stGetNameCB.pfnGetNameCallBack(s32FileCnt, pstTaskContext->stFileName, pstTaskContext->stCfg.stGetNameCB.pvPrivateData);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("get file name fail,s32Ret:%x \n", s32Ret);
            stEvent.EventID = HI_EVENT_PHOTOMNG_ERROR;
            PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
            goto TASK_STOP;
        }
    }

    /**2.start venc*/
    
    MLOGI("start venc  \n");
     MLOGI("Do PHOTOMNG_VencStart \n");
    s32Ret = PHOTOMNG_VencStart(pstTaskContext);

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("PHOTOMNG_VencStart fail,s32Ret: %d \n", s32Ret);
        goto TASK_STOP;
    }

    if (bNeedTrigger)
    {
        /**3.set snap attr*/
        
    MLOGI("set snap attr \n");
     MLOGI("Do HI_MAPI_VCAP_SetSnapAttr \n");
        s32Ret = HI_MAPI_VCAP_SetSnapAttr(pstPhotoSrc->VcapPipeHdl, &stPhotoParam.stSnapAttr);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("HI_MAPI_VCAP_SetSnapAttr fail,s32Ret:[0x%08X],VcapPipeHdl:%d  \n", s32Ret, pstPhotoSrc->VcapPipeHdl);
            goto TASK_STOP;
        }

        if (VPROC_PHOTO_TYPE_BUTT != stPhotoParam.enPhotoType)
        {
#ifdef CFG_POST_PROCESS
            MLOGI("HI_MAPI_VPROC_InitPhoto.\n");
     MLOGI("Do HI_MAPI_VPROC_InitPhoto \n");
            s32Ret = HI_MAPI_VPROC_InitPhoto(pstPhotoSrc->VprocHdl, pstPhotoSrc->VportHdl, stPhotoParam.enPhotoType);

            if (HI_SUCCESS != s32Ret)
            {
                HI_LOG_PrintFuncErr(HI_MAPI_VPROC_InitPhoto, s32Ret);
                goto TASK_STOP;
            }

#else
            MLOGW("Photo Post Process is Unsupported.\n");
#endif
        }

            MLOGI("pstTaskContext->stPhotoAttr.enOutPutFmt  %d.\n",pstTaskContext->stPhotoAttr.enOutPutFmt);
        if (HI_PHOTOMNG_OUTPUT_FORMAT_JPEG != pstTaskContext->stPhotoAttr.enOutPutFmt)
        {
            HI_MAPI_DUMP_ATTR_S stDumpAttr;
            memset(&stDumpAttr,0,sizeof(HI_MAPI_DUMP_ATTR_S));
            stDumpAttr.enPixFmt = PHOTOMNG_DNG_RAW_PIXELFORMAT;
            stDumpAttr.stDumpAttr.bEnable = HI_TRUE;
            stDumpAttr.stDumpAttr.u32Depth = pstTaskContext->s32PhotoCnt;
            MLOGI("HI_MAPI_VCAP_SetDumpRawAttr.\n");
            s32Ret = HI_MAPI_VCAP_SetDumpRawAttr(pstPhotoSrc->VcapPipeHdl, &stDumpAttr);

            if (HI_SUCCESS != s32Ret)
            {
                HI_LOG_PrintFuncErr(HI_MAPI_VCAP_SetDumpRawAttr, s32Ret);
                goto TASK_STOP;
            }
        }

        /**4.Trigger*/
        MLOGI("Trigger \n");
#ifdef CFG_TIME_STAMP_ON
        clock_gettime(CLOCK_MONOTONIC, &s_PHOTOMNGBeginTime);
#endif

        MLOGI("HI_MAPI_VCAP_StartTrigger. %d\n",pstPhotoSrc->VcapPipeHdl);
        s32Ret = HI_MAPI_VCAP_StartTrigger(pstPhotoSrc->VcapPipeHdl);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("HI_MAPI_VCAP_StartTrigger fail,s32Ret:[0x%08X],VcapPipeHdl:%d  \n", s32Ret, pstPhotoSrc->VcapPipeHdl);
            goto TASK_STOP;
        }

#ifdef CFG_POST_PROCESS

        /**5.Photo Process if need*/
        
        MLOGI("stPhotoParam.enPhotoType. %d\n",stPhotoParam.enPhotoType);
        if (VPROC_PHOTO_TYPE_BUTT != stPhotoParam.enPhotoType)
        {
#ifdef CFG_TIME_STAMP_ON
            struct timespec s_PPBeginTime,s_PPEndTime;
            (HI_VOID)clock_gettime(CLOCK_MONOTONIC, &s_PPBeginTime);
#endif

            s32Ret = HI_MAPI_VPROC_PhotoProcess(pstPhotoSrc->VprocHdl, pstPhotoSrc->VportHdl);

#ifdef CFG_TIME_STAMP_ON
            (HI_VOID)clock_gettime(CLOCK_MONOTONIC, &s_PPEndTime);
            HI_U32 u32WaitTime_ms = (s_PPEndTime.tv_sec * 1000 + s_PPEndTime.tv_nsec / 1000000)
                - (s_PPBeginTime.tv_sec * 1000 + s_PPBeginTime.tv_nsec / 1000000);
            MLOGI(GREEN"Take photo post process type: %d ,time %dms\n"NONE,stPhotoParam.enPhotoType,u32WaitTime_ms);
#endif
            if (HI_SUCCESS != s32Ret)
            {
                HI_LOG_PrintFuncErr(HI_MAPI_VPROC_PhotoProcess, s32Ret);
                goto TASK_STOP;
            }
        }

#endif
    }

    if (HI_PHOTOMNG_OUTPUT_FORMAT_JPEG != pstTaskContext->stPhotoAttr.enOutPutFmt)
    {
#ifdef CFG_DNG
        /**6.Dump Raw for DNG*/
        
        MLOGI("PHOTOMNG_DumpRaw\n");
        s32Ret = PHOTOMNG_DumpRaw(pstTaskContext);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("PHOTOMNG_DumpRaw,s32Ret: %d  \n", s32Ret);
        }

#else
        s32Ret = HI_PHOTOMNG_EINVAL;
        MLOGE("Unsupported OutPutFmt: %d.\n", pstTaskContext->stPhotoAttr.enOutPutFmt);
        goto TASK_STOP;
#endif
    }
    else if ((HI_PHOTOMNG_PHOTO_TYPE_MULTIPLE != pstTaskContext->stPhotoAttr.enPhotoType)
             || ((HI_PHOTOMNG_PHOTO_TYPE_MULTIPLE == pstTaskContext->stPhotoAttr.enPhotoType)
                 && (-1 != pstTaskContext->stPhotoAttr.unPhotoTypeAttr.stMultipleAttr.s32Count)))    /*continuous photo do not wait */
    {
        s32Ret = PHOTOMNG_WaitJPEGFinish(pstTaskContext);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("PHOTOMNG_WaitJPEGFinish fail,s32Ret: %d  \n", s32Ret);
        }
    }

    if (bNeedTrigger)
    {
#ifdef CFG_POST_PROCESS

        if (VPROC_PHOTO_TYPE_BUTT != stPhotoParam.enPhotoType)
        {
            HI_MAPI_VPROC_DeinitPhoto(pstPhotoSrc->VprocHdl, pstPhotoSrc->VportHdl);
        }

#endif
        HI_MAPI_VCAP_StopTrigger(pstPhotoSrc->VcapPipeHdl);
    }

    return s32Ret;

TASK_STOP:

    if (bNeedTrigger)
    {
#ifdef CFG_POST_PROCESS

        if (VPROC_PHOTO_TYPE_BUTT != stPhotoParam.enPhotoType)
        {
            HI_MAPI_VPROC_DeinitPhoto(pstPhotoSrc->VprocHdl, pstPhotoSrc->VportHdl);
        }

#endif
        HI_MAPI_VCAP_StopTrigger(pstPhotoSrc->VcapPipeHdl);
    }

    PHOTOMNG_VencStop(pstTaskContext);
    HI_MUTEX_LOCK(pstTaskContext->TaskMutex);
    pstTaskContext->enTaskState = PHOTOMNG_TASK_STATE_READY;
    stEvent.EventID = HI_EVENT_PHOTOMNG_TASK_END;
    stEvent.s32Result = pstTaskContext->s32SavedCnt;
    PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
    HI_COND_SIGNAL(pstTaskContext->TaskStoppedCond);
    HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);

    return s32Ret;
}

static HI_VOID PHOTOMNG_PhotoProc(HI_VOID* pClientData, struct timeval* nowP)
{
    PHOTOMNG_TASK_CTX_S*  pstTaskContext = (PHOTOMNG_TASK_CTX_S*)pClientData;
    HI_MUTEX_LOCK(pstTaskContext->TaskMutex);

    if (PHOTOMNG_TASK_STATE_TICKING != pstTaskContext->enTaskState)
    {
        HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
        return;
    }

    pstTaskContext->enTaskState = PHOTOMNG_TASK_STATE_STARTED;
    HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
    PHOTOMNG_TakePhoto(pstTaskContext);
    return;
}

static HI_S32 PHOTOMNG_SinglePhoto(PHOTOMNG_TASK_CTX_S* pstTaskContext)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_TIMER_S  stDelayTimer;

    HI_MUTEX_LOCK(pstTaskContext->TaskMutex);

    if (PHOTOMNG_TASK_STATE_READY != pstTaskContext->enTaskState)
    {
        HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
        MLOGE("state :%d is not ready \n", pstTaskContext->enTaskState);
        return HI_PHOTOMNG_EBUSY;
    }

    HI_EVENT_S stEvent;
    stEvent.EventID = HI_EVENT_PHOTOMNG_TASK_STARTED;
    PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);

    pstTaskContext->s32PhotoCnt = 1;
    pstTaskContext->s32SavedCnt = 0;

    MLOGI("pstTaskContext->stPhotoAttr.unPhotoTypeAttr.stSingleAttr.s32Time_sec %d\n", pstTaskContext->stPhotoAttr.unPhotoTypeAttr.stSingleAttr.s32Time_sec);
    if (pstTaskContext->stPhotoAttr.unPhotoTypeAttr.stSingleAttr.s32Time_sec <= 0)
    {
        pstTaskContext->enTaskState = PHOTOMNG_TASK_STATE_STARTED;
        HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
        s32Ret = PHOTOMNG_TakePhoto(pstTaskContext);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("single photo failed  \n");
            s32Ret = HI_PHOTOMNG_ETAKEPHOTO;
        }
    }
    else
    {
        memset(&stDelayTimer, 0x00, sizeof(HI_TIMER_S));
        stDelayTimer.now = HI_NULL;
        stDelayTimer.interval_ms = pstTaskContext->stPhotoAttr.unPhotoTypeAttr.stSingleAttr.s32Time_sec * 1000;
        stDelayTimer.periodic = HI_FALSE;
        stDelayTimer.timer_proc = PHOTOMNG_PhotoProc;
        stDelayTimer.clientData = (HI_VOID*)pstTaskContext;
        pstTaskContext->hTimerHdl = HI_Timer_Create(s_stPHOTOMNGCtx.s32TimerGrp, &stDelayTimer);

        if (HI_NULL == pstTaskContext->hTimerHdl)
        {
            HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
            MLOGE("create delay timer failed  \n");
            return HI_PHOTOMNG_ETIMERCREATE;
        }

        pstTaskContext->enTaskState = PHOTOMNG_TASK_STATE_TICKING;
        HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
        s32Ret = HI_SUCCESS;
    }

    return s32Ret;
}

static HI_S32 PHOTOMNG_LapsePhoto(PHOTOMNG_TASK_CTX_S* pstTaskContext)
{
    HI_TIMER_S stLapseTimer;
    HI_EVENT_S stEvent;

    HI_MUTEX_LOCK(pstTaskContext->TaskMutex);

    if (PHOTOMNG_TASK_STATE_READY != pstTaskContext->enTaskState)
    {
        HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
        MLOGE("state :%d is not ready \n", pstTaskContext->enTaskState);
        return HI_PHOTOMNG_EBUSY;
    }

    pstTaskContext->s32PhotoCnt = 1;
    pstTaskContext->s32SavedCnt = 0;
    pstTaskContext->enTaskState = PHOTOMNG_TASK_STATE_TICKING;
    stEvent.EventID = HI_EVENT_PHOTOMNG_TASK_STARTED;
    PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
    HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);

    /* first: create timer */
    memset(&stLapseTimer, 0x00, sizeof(HI_TIMER_S));
    stLapseTimer.now = HI_NULL;
    stLapseTimer.interval_ms = pstTaskContext->stPhotoAttr.unPhotoTypeAttr.stLapseAttr.s32Interval_ms;
    stLapseTimer.periodic = HI_TRUE;
    stLapseTimer.timer_proc = PHOTOMNG_PhotoProc;
    stLapseTimer.clientData = (HI_VOID*)pstTaskContext;
    pstTaskContext->hTimerHdl = HI_Timer_Create(s_stPHOTOMNGCtx.s32TimerGrp, &stLapseTimer);

    if (HI_NULL == pstTaskContext->hTimerHdl)
    {
        MLOGE("create lapse timer failed  \n");
        return HI_PHOTOMNG_ETIMERCREATE;
    }
#if 0
    /* second: take a photo */
    s32Ret = PHOTOMNG_TakePhoto(pstTaskContext);
    if (HI_SUCCESS != s32Ret)
    {
        s32Ret = HI_Timer_Destroy(s_stPHOTOMNGCtx.s32TimerGrp, pstTaskContext->hTimerHdl);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("HI_Timer_Destroy timer failed \n");
        }
        pstTaskContext->hTimerHdl = HI_NULL;
        MLOGE("start lapse photo failed  \n");
        return HI_PHOTOMNG_ETAKEPHOTO;
    }
#endif
    return HI_SUCCESS;
}

static HI_S32 PHOTOMNG_MultiplePhoto(PHOTOMNG_TASK_CTX_S* pstTaskContext)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_EVENT_S stEvent;

    HI_MUTEX_LOCK(pstTaskContext->TaskMutex);

    if (PHOTOMNG_TASK_STATE_READY != pstTaskContext->enTaskState)
    {
        HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
        MLOGE("state :%d is not ready \n", pstTaskContext->enTaskState);
        return HI_PHOTOMNG_EBUSY;
    }

    pstTaskContext->enTaskState = PHOTOMNG_TASK_STATE_STARTED;
    pstTaskContext->s32PhotoCnt = pstTaskContext->stPhotoAttr.unPhotoTypeAttr.stMultipleAttr.s32Count;
    pstTaskContext->s32SavedCnt = 0;
    stEvent.EventID = HI_EVENT_PHOTOMNG_TASK_STARTED;
    PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
    HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);

#ifdef CFG_TIME_STAMP_ON
    s_PhotoMngLastPackPTS = 0;
#endif

    s32Ret = PHOTOMNG_TakePhoto(pstTaskContext);

    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("multiple photo failed  \n");
        return HI_PHOTOMNG_ETAKEPHOTO;
    }

    return HI_SUCCESS;
}

static HI_S32 PHOTOMNG_TaskStop(PHOTOMNG_TASK_CTX_S* pstTaskContext)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_EVENT_S stEvent;
    HI_MUTEX_LOCK(pstTaskContext->TaskMutex);

    /* destroy delay timer or lapse */
    if (HI_NULL != pstTaskContext->hTimerHdl)
    {
        s32Ret = HI_Timer_Destroy(s_stPHOTOMNGCtx.s32TimerGrp, pstTaskContext->hTimerHdl);

        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("HI_Timer_Destroy timer failed \n");
        }

        pstTaskContext->hTimerHdl = HI_NULL;
    }

    if ((PHOTOMNG_TASK_STATE_INVALID ==  pstTaskContext->enTaskState)
        || (PHOTOMNG_TASK_STATE_BUTT ==  pstTaskContext->enTaskState))
    {
        s32Ret = HI_PHOTOMNG_EINVAL;
    }
    else if (PHOTOMNG_TASK_STATE_READY ==  pstTaskContext->enTaskState)
    {
        s32Ret = HI_SUCCESS;
    }
    else if (PHOTOMNG_TASK_STATE_TICKING == pstTaskContext->enTaskState)
    {
        pstTaskContext->enTaskState = PHOTOMNG_TASK_STATE_READY;
        /*make sure if need publish task end event here*/
        stEvent.EventID = HI_EVENT_PHOTOMNG_TASK_END;
        PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
        s32Ret = HI_SUCCESS;
    }
    else if (PHOTOMNG_TASK_STATE_STARTED == pstTaskContext->enTaskState)
    {
        if ((HI_PHOTOMNG_PHOTO_TYPE_MULTIPLE == pstTaskContext->stPhotoAttr.enPhotoType)
            && ( pstTaskContext->stPhotoAttr.unPhotoTypeAttr.stMultipleAttr.s32Count == -1))
        {
            /* stop continuous*/
            HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
            PHOTOMNG_VencStop(pstTaskContext);  /*at the time  PHOTOMNG_VencDePrepare can not with lock TaskMutex*/
            HI_MUTEX_LOCK(pstTaskContext->TaskMutex);
            pstTaskContext->enTaskState = PHOTOMNG_TASK_STATE_READY;
        }
        else
        {
            if (HI_PHOTOMNG_PHOTO_TYPE_LAPSE == pstTaskContext->stPhotoAttr.enPhotoType)  /*stop lapse*/
            {
                pstTaskContext->bLapseStop = HI_TRUE;         /*set stop flag*/
            }

            /*wait task stopped*/
            MLOGI("wait TaskstoppeCond \n");
            HI_COND_TIMEDWAIT_WITH_RETURN(pstTaskContext->TaskStoppedCond, pstTaskContext->TaskMutex,
                PHOTOMNG_PHOTO_TIMEOUT*1000*1000, s32Ret);

            if (HI_SUCCESS != s32Ret)
            {
                pstTaskContext->enTaskState = PHOTOMNG_TASK_STATE_READY;         /*restore task state to ready*/
                MLOGE("wait TaskstoppeCond failed, s32Ret:%d \n", s32Ret);
                s32Ret = HI_PHOTOMNG_EOTHER;
            }

            MLOGI("Task is stop\n");
        }

        stEvent.EventID = HI_EVENT_PHOTOMNG_TASK_END;
        PHOTOMNG_ReportEvent(&stEvent, __func__, __LINE__);
    }

    HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
    return s32Ret;
}

static HI_S32 PHOTOMNG_TaskDestroy(PHOTOMNG_TASK_CTX_S* pstTaskContext)
{
    HI_MUTEX_LOCK(pstTaskContext->TaskMutex);

    if (PHOTOMNG_TASK_STATE_INVALID == pstTaskContext->enTaskState)
    {
        HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
        //MLOGW("TaskHdl: %d has not been created \n",TaskHdl);
        return HI_PHOTOMNG_EINVAL;
    }

    if ((PHOTOMNG_TASK_STATE_TICKING == pstTaskContext->enTaskState)
        || (PHOTOMNG_TASK_STATE_STARTED == pstTaskContext->enTaskState))
    {
        HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
        //MLOGE("Task is busy,enTaskState:%d,please stop first \n", pstTaskContext->enTaskState);
        PHOTOMNG_TaskStop(pstTaskContext);
    }

    HI_COND_DESTROY(pstTaskContext->TaskEndCond);
    HI_COND_DESTROY(pstTaskContext->TaskStoppedCond);
#ifdef CFG_DNG
    /*destroy dng ctx cond and mutex*/
    HI_MUTEX_DESTROY(pstTaskContext->stDngDataCTX.DataMutex);
    HI_COND_DESTROY(pstTaskContext->stDngDataCTX.DataCond);
#endif
    pstTaskContext->enTaskState = PHOTOMNG_TASK_STATE_INVALID;
    HI_MUTEX_UNLOCK(pstTaskContext->TaskMutex);
    HI_MUTEX_DESTROY(pstTaskContext->TaskMutex);
    return HI_SUCCESS;
}

HI_S32 HI_PHOTOMNG_Init(const HI_PHOTOMNG_VENC_OPERATE_S* pstVencOps)
{
    HI_S32 i = 0, s32TimerGrp = 0;
    HI_S32 s32Ret = HI_FAILURE;
    HI_APPCOMM_CHECK_POINTER(pstVencOps, HI_PHOTOMNG_EINVAL);
    HI_APPCOMM_CHECK_POINTER(pstVencOps->pfnStart, HI_PHOTOMNG_EINVAL);
    HI_APPCOMM_CHECK_POINTER(pstVencOps->pfnStop, HI_PHOTOMNG_EINVAL);

    PHOTOMNG_CTX_S* pstPhotoMngCtx = &s_stPHOTOMNGCtx;

    HI_MUTEX_LOCK(s_PHOTOMNGInitMutex);

    if (HI_TRUE == s_bPHOTOMNGInitFlg)
    {
        HI_MUTEX_UNLOCK(s_PHOTOMNGInitMutex);
        MLOGW("photomng has been inited \n");
        return HI_PHOTOMNG_EINITIALIZED;
    }

#ifdef CFG_EXIF
    s32Ret =  HI_EXIF_Init();

    if (HI_SUCCESS != s32Ret)
    {
        HI_MUTEX_UNLOCK(s_PHOTOMNGInitMutex);
        HI_LOG_PrintFuncErr(HI_EXIF_Init, s32Ret);
        return HI_PHOTOMNG_EOTHER;
    }

#endif

    s32TimerGrp = HI_Timer_Init(HI_FALSE);

    if (s32TimerGrp < 0)
    {
#ifdef CFG_EXIF
        HI_EXIF_DeInit();
#endif
        HI_MUTEX_UNLOCK(s_PHOTOMNGInitMutex);
        HI_LOG_PrintFuncErr(HI_Timer_Init, s32TimerGrp);
        return HI_PHOTOMNG_ETIMERCREATE;
    }

    s32Ret = HI_Timer_SetTickValue(s32TimerGrp, PHOTOMNG_TIMER_TICKVALUE_US);

    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_PrintFuncErr(HI_Timer_SetTickValue, s32Ret);
    }

    pstPhotoMngCtx->s32TimerGrp = s32TimerGrp;
    HI_MUTEX_INIT_LOCK(pstPhotoMngCtx->CtxMutex);
    pstPhotoMngCtx->pfnGetDNGUserInfoCB = HI_NULL;
    pstPhotoMngCtx->pfnGetJpegUserInfoCB = HI_NULL;

    for (i = 0; i < HI_PHOTOMNG_TASK_NUM_MAX; i++)
    {
        memset(&pstPhotoMngCtx->astTaskCtx[i], 0, sizeof(PHOTOMNG_TASK_CTX_S));
        pstPhotoMngCtx->astTaskCtx[i].enTaskState = PHOTOMNG_TASK_STATE_INVALID;
    }

    pstPhotoMngCtx->stVencOps.pfnStart = pstVencOps->pfnStart;
    pstPhotoMngCtx->stVencOps.pfnStop = pstVencOps->pfnStop;
    s_bPHOTOMNGInitFlg = HI_TRUE;
    HI_MUTEX_UNLOCK(s_PHOTOMNGInitMutex);
    return HI_SUCCESS;
}

HI_S32 HI_PHOTOMNG_Deinit(HI_VOID)
{
    HI_S32 i = 0, s32Ret = HI_FAILURE;
    PHOTOMNG_CTX_S* pstPhotoMngCtx = &s_stPHOTOMNGCtx;

    HI_MUTEX_LOCK(s_PHOTOMNGInitMutex);

    if (HI_FALSE == s_bPHOTOMNGInitFlg)
    {
        HI_MUTEX_UNLOCK(s_PHOTOMNGInitMutex);
        MLOGW("photomng has not been inited \n");
        return HI_PHOTOMNG_ENOTINIT;
    }

    HI_MUTEX_LOCK(pstPhotoMngCtx->CtxMutex);

    for (i = 0; i < HI_PHOTOMNG_TASK_NUM_MAX; i++)
    {
        PHOTOMNG_TaskDestroy(&pstPhotoMngCtx->astTaskCtx[i]);
    }

    s32Ret =  HI_Timer_DeInit(pstPhotoMngCtx->s32TimerGrp);

    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_PrintFuncErr(HI_Timer_DeInit, s32Ret);
    }

    pstPhotoMngCtx->s32TimerGrp = -1;
    pstPhotoMngCtx->pfnGetDNGUserInfoCB = HI_NULL;
    pstPhotoMngCtx->pfnGetJpegUserInfoCB = HI_NULL;
    HI_MUTEX_DESTROY(pstPhotoMngCtx->CtxMutex);
#ifdef CFG_EXIF
    s32Ret =  HI_EXIF_DeInit();

    if (HI_SUCCESS != s32Ret)
    {
        HI_LOG_PrintFuncErr(HI_EXIF_DeInit, s32Ret);
    }

#endif

    s_bPHOTOMNGInitFlg = HI_FALSE;
    HI_MUTEX_UNLOCK(s_PHOTOMNGInitMutex);
    return HI_SUCCESS;
}

HI_S32 HI_PHOTOMNG_TaskCreate(const HI_PHOTOMNG_TASK_CFG_S* pstTaskConfig, HI_HANDLE* pTaskHdl)
{
    PHOTOMNG_CHECK_INIT();
    HI_APPCOMM_CHECK_POINTER(pstTaskConfig, HI_PHOTOMNG_EINVAL);
    HI_APPCOMM_CHECK_POINTER(pstTaskConfig->stGetNameCB.pfnGetNameCallBack, HI_PHOTOMNG_EINVAL);
    HI_APPCOMM_CHECK_POINTER(pstTaskConfig->stGetParamCB.pfnGetParamCallBack, HI_PHOTOMNG_EINVAL);
    HI_APPCOMM_CHECK_POINTER(pTaskHdl, HI_PHOTOMNG_EINVAL);
    HI_S32 i = 0;
    PHOTOMNG_CTX_S* pstPhotoMngCtx = &s_stPHOTOMNGCtx;
    HI_MUTEX_LOCK(pstPhotoMngCtx->CtxMutex);

    for (i = 0; i < HI_PHOTOMNG_TASK_NUM_MAX; i++)
    {
        if (PHOTOMNG_TASK_STATE_INVALID == pstPhotoMngCtx->astTaskCtx[i].enTaskState)
        {
            break;
        }
    }

    if (HI_PHOTOMNG_TASK_NUM_MAX == i)
    {
        HI_MUTEX_UNLOCK(pstPhotoMngCtx->CtxMutex);
        MLOGE(" task count can be not larger than %d \n", HI_PHOTOMNG_TASK_NUM_MAX);
        return HI_PHOTOMNG_EFULL;
    }

    PHOTOMNG_TASK_CTX_S* pstTaskCtx = &pstPhotoMngCtx->astTaskCtx[i];
    memset(pstTaskCtx, 0x00, sizeof(PHOTOMNG_TASK_CTX_S));
    /*init task  mutex and cond*/
    HI_MUTEX_INIT_LOCK(pstTaskCtx->TaskMutex);
    HI_COND_INIT(pstTaskCtx->TaskEndCond);
    HI_COND_INIT(pstTaskCtx->TaskStoppedCond);
#ifdef CFG_DNG
    /*init dng  mutex and cond*/
    HI_MUTEX_INIT_LOCK(pstTaskCtx->stDngDataCTX.DataMutex);
    HI_COND_INIT(pstTaskCtx->stDngDataCTX.DataCond);
#endif
    memcpy(&pstTaskCtx->stCfg, pstTaskConfig, sizeof(HI_PHOTOMNG_TASK_CFG_S));
    pstTaskCtx->enTaskState = PHOTOMNG_TASK_STATE_READY;
    *pTaskHdl = i;
    HI_MUTEX_UNLOCK(pstPhotoMngCtx->CtxMutex);
    return HI_SUCCESS;
}

HI_S32 HI_PHOTOMNG_TaskDestroy(HI_HANDLE TaskHdl)
{
    HI_S32 s32Ret = HI_SUCCESS;
    PHOTOMNG_CHECK_INIT();
    HI_APPCOMM_CHECK_EXPR(0 <= TaskHdl, HI_PHOTOMNG_EINVAL);
    HI_APPCOMM_CHECK_EXPR(HI_PHOTOMNG_TASK_NUM_MAX > TaskHdl, HI_PHOTOMNG_EINVAL);
    HI_MUTEX_LOCK(s_stPHOTOMNGCtx.CtxMutex);
    s32Ret = PHOTOMNG_TaskDestroy(&s_stPHOTOMNGCtx.astTaskCtx[TaskHdl]);
    HI_MUTEX_UNLOCK(s_stPHOTOMNGCtx.CtxMutex);
    return s32Ret;
}

HI_S32 HI_PHOTOMNG_TaskStart(HI_HANDLE TaskHdl)
{
    HI_S32 s32Ret = HI_FAILURE;
    PHOTOMNG_CTX_S* pstPhotoMngCtx = &s_stPHOTOMNGCtx;

    MLOGI("HI_PHOTOMNG_TaskStart  %d\n",TaskHdl);
    PHOTOMNG_CHECK_INIT();
    HI_APPCOMM_CHECK_EXPR(0 <= TaskHdl, HI_PHOTOMNG_EINVAL);
    HI_APPCOMM_CHECK_EXPR(HI_PHOTOMNG_TASK_NUM_MAX > TaskHdl, HI_PHOTOMNG_EINVAL);

    PHOTOMNG_TASK_CTX_S* pTaskCtx = &pstPhotoMngCtx->astTaskCtx[TaskHdl];
    HI_MUTEX_LOCK(pTaskCtx->TaskMutex);

    if ((PHOTOMNG_TASK_STATE_INVALID ==  pTaskCtx->enTaskState)
        || (PHOTOMNG_TASK_STATE_BUTT ==  pTaskCtx->enTaskState))
    {
        HI_MUTEX_UNLOCK(pTaskCtx->TaskMutex);
        MLOGE("TaskHdl: %d has not been created \n", TaskHdl);
        return HI_PHOTOMNG_EINVAL;
    }

    HI_MUTEX_UNLOCK(pTaskCtx->TaskMutex);

    MLOGI("pTaskCtx->stPhotoAttr.enPhotoTyp %d  \n",pTaskCtx->stPhotoAttr.enPhotoType);
    switch (pTaskCtx->stPhotoAttr.enPhotoType)
    {
        case HI_PHOTOMNG_PHOTO_TYPE_SINGLE:
            s32Ret = PHOTOMNG_SinglePhoto(pTaskCtx);
            break;

        case HI_PHOTOMNG_PHOTO_TYPE_LAPSE:
            s32Ret = PHOTOMNG_LapsePhoto(pTaskCtx);
            break;

        case HI_PHOTOMNG_PHOTO_TYPE_MULTIPLE:
            s32Ret = PHOTOMNG_MultiplePhoto(pTaskCtx);
            break;

        default:
        {
            MLOGE("enPhotoType: %d unsupport \n", pTaskCtx->stPhotoAttr.enPhotoType);
            return HI_PHOTOMNG_EINVAL;
        }
    }

    return s32Ret;
}

HI_S32 HI_PHOTOMNG_TaskStop(HI_HANDLE TaskHdl)
{
    PHOTOMNG_CHECK_INIT();
    HI_APPCOMM_CHECK_EXPR(0 <= TaskHdl, HI_PHOTOMNG_EINVAL);
    HI_APPCOMM_CHECK_EXPR(HI_PHOTOMNG_TASK_NUM_MAX > TaskHdl, HI_PHOTOMNG_EINVAL);
    return PHOTOMNG_TaskStop(&s_stPHOTOMNGCtx.astTaskCtx[TaskHdl]);
}

HI_S32 HI_PHOTOMNG_SetPhotoAttr(HI_HANDLE TaskHdl, const HI_PHOTOMNG_PHOTO_ATTR_S* pstPhotoAttr)
{
    HI_BOOL  bFlag = HI_FALSE;
    PHOTOMNG_CTX_S* pstPhotoMngCtx = &s_stPHOTOMNGCtx;

    PHOTOMNG_CHECK_INIT();
    HI_APPCOMM_CHECK_EXPR(0 <= TaskHdl, HI_PHOTOMNG_EINVAL);
    HI_APPCOMM_CHECK_EXPR(HI_PHOTOMNG_TASK_NUM_MAX > TaskHdl, HI_PHOTOMNG_EINVAL);

    HI_APPCOMM_CHECK_POINTER(pstPhotoAttr, HI_PHOTOMNG_EINVAL);
    bFlag = (HI_BOOL)HI_APPCOMM_CHECK_RANGE(pstPhotoAttr->enPhotoType, HI_PHOTOMNG_PHOTO_TYPE_SINGLE, HI_PHOTOMNG_PHOTO_TYPE_BUTT - 1);

    if (!bFlag)
    {
        MLOGE("enPhotoType: %d out of range \n", pstPhotoAttr->enPhotoType);
        return HI_PHOTOMNG_EINVAL;
    }

    bFlag = (HI_BOOL)HI_APPCOMM_CHECK_RANGE(pstPhotoAttr->enOutPutFmt, HI_PHOTOMNG_OUTPUT_FORMAT_JPEG, HI_PHOTOMNG_OUTPUT_FORMAT_BUTT - 1);

    if (!bFlag)
    {
        MLOGE("enOutPutFmt: %d out of range \n", pstPhotoAttr->enOutPutFmt);
        return HI_PHOTOMNG_EINVAL;
    }

    if ((HI_PHOTOMNG_PHOTO_TYPE_SINGLE == pstPhotoAttr->enPhotoType)
        && (pstPhotoAttr->unPhotoTypeAttr.stSingleAttr.s32Time_sec < 0))
    {
        MLOGE("s32Time_sec: %d out of range \n", pstPhotoAttr->unPhotoTypeAttr.stSingleAttr.s32Time_sec);
        return HI_PHOTOMNG_EINVAL;
    }

    if ((HI_PHOTOMNG_PHOTO_TYPE_LAPSE == pstPhotoAttr->enPhotoType)
        && (pstPhotoAttr->unPhotoTypeAttr.stLapseAttr.s32Interval_ms <= 0))
    {
        MLOGE("s32Interval_ms: %d out of range \n", pstPhotoAttr->unPhotoTypeAttr.stLapseAttr.s32Interval_ms);
        return HI_PHOTOMNG_EINVAL;
    }

    if ((HI_PHOTOMNG_PHOTO_TYPE_MULTIPLE == pstPhotoAttr->enPhotoType)
        && ((pstPhotoAttr->unPhotoTypeAttr.stMultipleAttr.s32Count < -1) || (pstPhotoAttr->unPhotoTypeAttr.stMultipleAttr.s32Count == 0)))
    {
        MLOGE("s32Count: %d out of range \n", pstPhotoAttr->unPhotoTypeAttr.stMultipleAttr.s32Count);
        return HI_PHOTOMNG_EINVAL;
    }

    PHOTOMNG_TASK_CTX_S* pTaskCtx = &pstPhotoMngCtx->astTaskCtx[TaskHdl];

    HI_MUTEX_LOCK(pTaskCtx->TaskMutex);

    /*if task is not idle, it should be not set photoattr*/
    if ((PHOTOMNG_TASK_STATE_INVALID ==  pTaskCtx->enTaskState)
        || (PHOTOMNG_TASK_STATE_BUTT ==  pTaskCtx->enTaskState))
    {
        HI_MUTEX_UNLOCK(pTaskCtx->TaskMutex);
        MLOGE("TaskHdl: %d is invalid \n", TaskHdl);
        return HI_PHOTOMNG_EINVAL;
    }
    else if ((PHOTOMNG_TASK_STATE_TICKING ==  pTaskCtx->enTaskState)
             || (PHOTOMNG_TASK_STATE_STARTED ==  pTaskCtx->enTaskState))
    {
        HI_MUTEX_UNLOCK(pTaskCtx->TaskMutex);
        MLOGE("TaskHdl: %d is busy \n", TaskHdl);
        return HI_PHOTOMNG_EBUSY;
    }

    memcpy(&pTaskCtx->stPhotoAttr, pstPhotoAttr, sizeof(HI_PHOTOMNG_PHOTO_ATTR_S));
    HI_MUTEX_UNLOCK(pTaskCtx->TaskMutex);
    return HI_SUCCESS;
}

HI_S32 HI_PHOTOMNG_GetPhotoAttr(HI_HANDLE TaskHdl, HI_PHOTOMNG_PHOTO_ATTR_S* pstPhotoAttr)
{
    HI_BOOL  bFlag = HI_FALSE;
    PHOTOMNG_CTX_S* pstPhotoMngCtx = &s_stPHOTOMNGCtx;

    PHOTOMNG_CHECK_INIT();
    bFlag = (HI_BOOL)HI_APPCOMM_CHECK_RANGE(TaskHdl, 0, HI_PHOTOMNG_TASK_NUM_MAX - 1);

    if (!bFlag)
    {
        MLOGE("TaskHdl: %d out of range \n", TaskHdl);
        return HI_PHOTOMNG_EINVAL;
    }

    HI_APPCOMM_CHECK_POINTER(pstPhotoAttr, HI_PHOTOMNG_EINVAL);

    PHOTOMNG_TASK_CTX_S* pTaskCtx = &pstPhotoMngCtx->astTaskCtx[TaskHdl];
    HI_MUTEX_LOCK(pTaskCtx->TaskMutex);

    if ((PHOTOMNG_TASK_STATE_INVALID ==  pTaskCtx->enTaskState)
        || (PHOTOMNG_TASK_STATE_BUTT ==  pTaskCtx->enTaskState))
    {
        HI_MUTEX_UNLOCK(pTaskCtx->TaskMutex);
        MLOGE("TaskHdl: %d is invalid \n", TaskHdl);
        return HI_PHOTOMNG_EINVAL;
    }

    memcpy(pstPhotoAttr, &pTaskCtx->stPhotoAttr, sizeof(HI_PHOTOMNG_PHOTO_ATTR_S));
    HI_MUTEX_UNLOCK(pTaskCtx->TaskMutex);
    return HI_SUCCESS;
}

HI_S32 HI_PHOTOMNG_RegGetDNGUserInfoCB(HI_PHOTOMNG_GETDNGUSERINFO_CALLBACK_FN_PTR pfnGetDNGUserInfo)
{
        PHOTOMNG_CHECK_INIT();
    #ifdef CFG_DNG
        PHOTOMNG_CTX_S* pstPhotoMngCtx = &s_stPHOTOMNGCtx;
        HI_MUTEX_LOCK(pstPhotoMngCtx->CtxMutex);
        pstPhotoMngCtx->pfnGetDNGUserInfoCB = pfnGetDNGUserInfo;
        HI_MUTEX_UNLOCK(pstPhotoMngCtx->CtxMutex);
    #else
        MLOGW("DNG is unsupported!\n");
    #endif
        return HI_SUCCESS;
}

HI_S32 HI_PHOTOMNG_RegGetJPEGUserInfoCB(HI_PHOTOMNG_GETJPEGUSERINFO_CALLBACK_FN_PTR pfnGetJPEGUserInfo)
{
        PHOTOMNG_CHECK_INIT();
    #ifdef CFG_EXIF
        PHOTOMNG_CTX_S* pstPhotoMngCtx = &s_stPHOTOMNGCtx;
        HI_MUTEX_LOCK(pstPhotoMngCtx->CtxMutex);
        pstPhotoMngCtx->pfnGetJpegUserInfoCB = pfnGetJPEGUserInfo;
        HI_MUTEX_UNLOCK(pstPhotoMngCtx->CtxMutex);
    #else
        MLOGW("EXIF is unsupported!\n");
    #endif
    return HI_SUCCESS;
}

HI_S32 HI_PHOTOMNG_RegisterEvent(HI_VOID)
{
    HI_S32 s32Ret = HI_FAILURE;

    s32Ret = HI_EVTHUB_Register(HI_EVENT_PHOTOMNG_FILE_BEGIN);

    if (s32Ret != HI_SUCCESS)
    {
        MLOGE("HI_EVENT_PHOTOMNG_FILE_BEGIN register fail \n");
        return HI_FAILURE;
    }

    s32Ret = HI_EVTHUB_Register(HI_EVENT_PHOTOMNG_FILE_END);

    if (s32Ret != HI_SUCCESS)
    {
        MLOGE("HI_EVENT_PHOTOMNG_FILE_END register fail \n");
        return HI_FAILURE;
    }

    s32Ret = HI_EVTHUB_Register(HI_EVENT_PHOTOMNG_TASK_STARTED);

    if (s32Ret != HI_SUCCESS)
    {
        MLOGE("HI_EVENT_PHOTOMNG_TASK_STARTED register fail \n");
        return HI_FAILURE;
    }

    s32Ret = HI_EVTHUB_Register(HI_EVENT_PHOTOMNG_TASK_END);

    if (s32Ret != HI_SUCCESS)
    {
        MLOGE("HI_EVENT_PHOTOMNG_TASK_END register fail \n");
        return HI_FAILURE;
    }

    s32Ret = HI_EVTHUB_Register(HI_EVENT_PHOTOMNG_ERROR);

    if (s32Ret != HI_SUCCESS)
    {
        MLOGE("HI_EVENT_PHOTOMNG_ERROR register fail \n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


/** @}*/  /** <!-- ==== PHOTOMNG End ====*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* End of #ifdef __cplusplus */

