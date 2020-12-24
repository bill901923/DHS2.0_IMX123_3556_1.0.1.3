#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <errno.h>
//#include <linux/kernel.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#include "hi_defines.h"
#include "hi_product_scene.h"
#include "hi_common.h"
#include "hi_comm_video.h"
#include "hi_comm_sys.h"
#include "mpi_sys.h"
#include "hi_comm_vb.h"
#include "mpi_vb.h"
#include "hi_comm_vi.h"
#include "mpi_vi.h"
#include "hi_comm_vgs.h"
#include "mpi_vgs.h"

#include "hi_appcomm_util.h"
#include "hi_buffer.h"

#include <sys/socket.h>

#include "jpeglib.h"
#include "turbojpeg.h"
#include "jmorecfg.h"
#include "jerror.h"
#include "jconfig.h"

#include "dhs_picture.h"
#include "dhs_type.h"

//////////////////////////////////////////////////////////////
//#define DIS_DATA_DEBUG

#define MAX_FRM_CNT     256
#define MAX_FRM_WIDTH   4608
typedef struct hiDUMP_MEMBUF_S
{
    VB_BLK  hBlock;
    VB_POOL hPool;
    HI_U32  u32PoolId;

    HI_U32  u64PhyAddr;
    HI_U8*  pVirAddr;
    HI_S32  s32Mdev;
} DUMP_MEMBUF_S;

static VI_DUMP_ATTR_S astBackUpDumpAttr;
static HI_U32  u32BlkSize = 0;

static HI_CHAR* pUserPageAddr[2] = {HI_NULL,HI_NULL};
static HI_U32 u32Size = 0;

//static VI_CHN_ATTR_S stChnAttrBackup;
static volatile HI_BOOL bQuit = HI_FALSE;   /* bQuit may be set in the signal handler */

static VIDEO_FRAME_INFO_S stFrame;

static VB_POOL hPool  = VB_INVALID_POOLID;
static DUMP_MEMBUF_S stMem = {0};
static VGS_HANDLE hHandle = -1;

static FILE* pfd_test = HI_NULL;
int64_t creat_buff_len=2000000;

char Send_buffer[2000000];
int64_t Send_buffer_get_len=0;

#define ALIGN_UP(x, a)           ( ( ((x) + ((a) - 1) ) / a ) * a )
/*width:图像宽度

 height:图像高度

 yuv_size:yuv大小,等于width*height*3/2

 quality:转换后的图像质量(1~100)

 subsample:*/

static int tyuv2jpeg(unsigned char *yuv_buffer, int yuv_size, int width, int height,
                     int subsample, unsigned char **jpeg_buffer, unsigned long *jpeg_size,
                     int quality)
{

    tjhandle handle = NULL;
    int flags = 0;
    int padding = 1; //must be 1 or 4
    int need_size = 0;
    int ret = 0;

    handle = tjInitCompress();

    flags |= 0;

    need_size = tjBufSizeYUV2(width, padding, height, subsample);
    if (need_size != yuv_size)
    {
        printf("we detect yuv size: %d, but you give: %d, check again.\n",
               need_size, yuv_size);
        return -1;
    }

    ret = tjCompressFromYUV(handle, yuv_buffer, width, padding, height,
                            subsample, jpeg_buffer, jpeg_size, quality, flags);
    if (ret < 0)
    {
        printf("compress to jpeg failed: %s\n", tjGetErrorStr());
    }

    tjDestroy(handle);
    return ret;
}


/* sp420 -> p420 ;  sp422 -> p422  */
HI_S32 vi_dump_save_one_frame(VIDEO_FRAME_S* pVBuf, int sfd)
{   
    
        printf("Do vi_dump_save_one_frame\n");
    // by eason
    unsigned int w, h;
    char* pVBufVirt_Y;
    char* pVBufVirt_C;
  //  char* pMemContent;
   // unsigned char TmpBuff[MAX_FRM_WIDTH];
    HI_U32 phy_addr, size;
  //  HI_CHAR* pUserPageAddr[2];
    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
  //  HI_U32 u32UvHeight;// UV height for planar format 

    if (pVBuf->u32Width > MAX_FRM_WIDTH)
    {
        printf("Over max frame width: %d, can't support.\n", MAX_FRM_WIDTH);
        return HI_FAILURE;
    }

    printf("enPixelFormat =%d\n",enPixelFormat);
    printf("pVBuf->u32Height =%d\n",pVBuf->u32Height);
        
    if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
    {
        size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height) * 3 / 2;
 //       u32UvHeight = pVBuf->u32Height / 2;
    }
    else if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
    {
        size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height) * 2;
//        u32UvHeight = pVBuf->u32Height;
    }
    else if (PIXEL_FORMAT_YUV_400 == enPixelFormat)
    {
        size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height);
//        u32UvHeight = 0;
    }
    else
    {
        printf("not support pixelformat: %d\n", enPixelFormat);
        return HI_FAILURE;
    }

    printf("pVBuf->u64PhyAddr[0]=%lld\n",pVBuf->u64PhyAddr[0]);
   phy_addr = pVBuf->u64PhyAddr[0];

    pUserPageAddr[0] = (HI_CHAR*) HI_MPI_SYS_Mmap(phy_addr, size);
    if (NULL == pUserPageAddr[0])
    {
        return HI_FAILURE;
    }
    printf("stride: %d,%d\n", pVBuf->u32Stride[0], pVBuf->u32Stride[1] );

    pVBufVirt_Y = pUserPageAddr[0];
    pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0]) * (pVBuf->u32Height);
    
    int Set_H = 1000; //設定高
    int Set_W = 1000; //設定寬
    int Do_H = pVBuf->u32Height; //設定高
    int Do_W = pVBuf->u32Width;  //設定寬
    int startH = 0;
    int startW = 0;
     if (Do_H > Set_H)
    {
        startH = (Do_H - Set_H) / 2;
        Do_H = Set_H;
    }
    if (Do_W > Set_W)
    {
        startW = (Do_W - Set_W) / 2;
        Do_W = Set_W;
    }
    int y_size = Do_W * Do_H;
    int yuv_size = y_size * 3 / 2;

    unsigned char *yuvbuf;
    if ((yuvbuf = (unsigned char *)malloc(yuv_size + 1)) != NULL)
    {
        memset(yuvbuf, 0, yuv_size + 1);
    }
    else
    {
        return HI_FAILURE; 
        printf("run here>>>>>>>>> malloc Fail>>>>>>>>>>>>>>>>>>>.\n");
    }
    int idy = 0;
    int idu = 0;
    int idv = 0;

    unsigned char* y_tmp = yuvbuf;
    unsigned char* u_tmp = yuvbuf + y_size;
    unsigned char* v_tmp = yuvbuf + y_size * 5 / 4;

    char *pMemContent_Y;
    char *pMemContent_U;
    char *pMemContent_V;

    for (h = startH; h <( Do_H +startH); h++) 
    {
      pMemContent_Y = pVBufVirt_Y + h * pVBuf->u32Stride[0];
        for (w = startW; w < (Do_W+startW); w++) {
        y_tmp[idy++] = *(pMemContent_Y+w);
            //pMemContent_Y++;
        }
    }

    for (h =  (startH/2); h <  ( (Do_H+startH)/2); h++) {

        pMemContent_U = pVBufVirt_C + h * pVBuf->u32Stride[1];
        pMemContent_V = pVBufVirt_C + h * pVBuf->u32Stride[1];
        pMemContent_U += 1;            
        for (w = (startW/2); w < ((Do_W+startW)/2) ; w++){
            u_tmp[idu++] = *(pMemContent_U+(2*w));
            v_tmp[idv++] = *(pMemContent_V+(2*w));
        }
    }

    unsigned char *jpeg_buf = NULL;
    unsigned long jpeg_size = 0;
 //   unsigned char *picture = (unsigned char *)malloc(yuv_size + 1);

    int ret = tyuv2jpeg(yuvbuf, yuv_size, Do_W, Do_H, TJSAMP_420,
              &jpeg_buf, &jpeg_size, 96); //50:quality
    if(ret <0){
        return HI_FAILURE;
    }

    // FILE * fp_jpg = fopen("1_yuv_to_jpeg_test.jpg", "wb");
    // fwrite(jpeg_buf, jpeg_size, 1, fp_jpg);
    // fflush(fp_jpg);
    // fclose(fp_jpg);
    printf("!!!!!!!!!!!yuv to jpeg success!!!!!!!!!\n");
    int needSend = jpeg_size;
    int pos = 0;
    int len = 0;
    while (pos < needSend)
    {
        len = send(sfd, jpeg_buf + pos, 1024, 0);
        if (len <= 0)
        {
            perror("ERRPR");
            return HI_FAILURE;;
            break;
        }
        pos += len;
    }
    
    HI_MPI_SYS_Munmap(pUserPageAddr[0], size);
    if (yuvbuf != NULL)
    {
        free(yuvbuf);
    }
    if (jpeg_buf != NULL)
    {
        free(jpeg_buf);
    }
    return HI_SUCCESS;
}

static void Print_enVideoFormat(PIXEL_FORMAT_E enPixelFormat)
{
    switch(enPixelFormat)
    {
        case  PIXEL_FORMAT_RGB_444:
        printf(" PIXEL_FORMAT_RGB_444\r\n");
        break;

        case PIXEL_FORMAT_RGB_555:
        printf("PIXEL_FORMAT_RGB_555\r\n");
        break;

        case PIXEL_FORMAT_RGB_565:
        printf("PIXEL_FORMAT_RGB_565\r\n");
        break;

        case PIXEL_FORMAT_RGB_888:
        printf("PIXEL_FORMAT_RGB_888\r\n");
        break;

        case PIXEL_FORMAT_BGR_444:
        printf("PIXEL_FORMAT_BGR_444\r\n");
        break;

        case PIXEL_FORMAT_BGR_555:
        printf("PIXEL_FORMAT_BGR_555\r\n");
        break;

        case PIXEL_FORMAT_BGR_565:
        printf("PIXEL_FORMAT_BGR_565\r\n");
        break;

        case PIXEL_FORMAT_BGR_888:
        printf("PIXEL_FORMAT_BGR_888\r\n");
        break;

        case PIXEL_FORMAT_ARGB_1555:
        printf("PIXEL_FORMAT_ARGB_1555\r\n");
        break;

        case PIXEL_FORMAT_ARGB_4444:
        printf("PIXEL_FORMAT_ARGB_4444\r\n");
        break;

        case PIXEL_FORMAT_ARGB_8565:
        printf("PIXEL_FORMAT_ARGB_8565\r\n");
        break;

        case PIXEL_FORMAT_ARGB_8888:
        printf("PIXEL_FORMAT_ARGB_8888\r\n");
        break;

        case PIXEL_FORMAT_ARGB_2BPP:
        printf("PIXEL_FORMAT_ARGB_2BPP\r\n");
        break;

        case PIXEL_FORMAT_ABGR_1555:
        printf("PIXEL_FORMAT_ABGR_1555\r\n");
        break;

        case PIXEL_FORMAT_ABGR_4444:
        printf("PIXEL_FORMAT_ABGR_4444\r\n");
        break;

        case PIXEL_FORMAT_ABGR_8565:
        printf("PIXEL_FORMAT_ABGR_8565\r\n");
        break;

        case PIXEL_FORMAT_ABGR_8888:
        printf("PIXEL_FORMAT_ABGR_8888\r\n");
        break;

        case PIXEL_FORMAT_RGB_BAYER_8BPP:
        printf("PIXEL_FORMAT_RGB_BAYER_8BPP\r\n");
        break;

        case PIXEL_FORMAT_RGB_BAYER_10BPP:
        printf("PIXEL_FORMAT_RGB_BAYER_10BPP\r\n");
        break;

        case PIXEL_FORMAT_RGB_BAYER_12BPP:
        printf("PIXEL_FORMAT_RGB_BAYER_12BPP\r\n");
        break;

        case PIXEL_FORMAT_RGB_BAYER_14BPP:
        printf("PIXEL_FORMAT_RGB_BAYER_14BPP\r\n");
        break;

        case PIXEL_FORMAT_RGB_BAYER_16BPP:
        printf("PIXEL_FORMAT_RGB_BAYER_16BPP\r\n");
        break;

        case PIXEL_FORMAT_YVU_PLANAR_422:
        printf("PIXEL_FORMAT_YVU_PLANAR_422\r\n");
        break;

        case PIXEL_FORMAT_YVU_PLANAR_420:
        printf("PIXEL_FORMAT_YVU_PLANAR_420\r\n");
        break;

        case PIXEL_FORMAT_YVU_PLANAR_444:
        printf("PIXEL_FORMAT_YVU_PLANAR_444\r\n");
        break;

        case PIXEL_FORMAT_YVU_SEMIPLANAR_422:
        printf("PIXEL_FORMAT_YVU_SEMIPLANAR_422\r\n");
        break;

        case PIXEL_FORMAT_YVU_SEMIPLANAR_420:
        printf("PIXEL_FORMAT_YVU_SEMIPLANAR_420\r\n");
        break;

        case PIXEL_FORMAT_YVU_SEMIPLANAR_444:
        printf("PIXEL_FORMAT_YVU_SEMIPLANAR_444\r\n");
        break;

        case PIXEL_FORMAT_YUV_SEMIPLANAR_422:
        printf("PIXEL_FORMAT_YUV_SEMIPLANAR_422\r\n");
        break;

        case PIXEL_FORMAT_YUV_SEMIPLANAR_420:
        printf("PIXEL_FORMAT_YUV_SEMIPLANAR_420\r\n");
        break;

        case PIXEL_FORMAT_YUV_SEMIPLANAR_444:
        printf("PIXEL_FORMAT_YUV_SEMIPLANAR_444\r\n");
        break;

        case PIXEL_FORMAT_YUYV_PACKAGE_422:
        printf("PIXEL_FORMAT_YUYV_PACKAGE_422\r\n");
        break;

        case PIXEL_FORMAT_YVYU_PACKAGE_422:
        printf("PIXEL_FORMAT_YVYU_PACKAGE_422\r\n");
        break;

        case PIXEL_FORMAT_UYVY_PACKAGE_422:
        printf("PIXEL_FORMAT_UYVY_PACKAGE_422\r\n");
        break;

        case PIXEL_FORMAT_VYUY_PACKAGE_422:
        printf("PIXEL_FORMAT_VYUY_PACKAGE_422\r\n");
        break;

        case PIXEL_FORMAT_YYUV_PACKAGE_422:
        printf("PIXEL_FORMAT_YYUV_PACKAGE_422\r\n");
        break;

        case PIXEL_FORMAT_YYVU_PACKAGE_422:
        printf("PIXEL_FORMAT_YYVU_PACKAGE_422\r\n");
        break;

        case PIXEL_FORMAT_UVYY_PACKAGE_422:
        printf("PIXEL_FORMAT_UVYY_PACKAGE_422\r\n");
        break;

        case PIXEL_FORMAT_VUYY_PACKAGE_422:
        printf("PIXEL_FORMAT_VUYY_PACKAGE_422\r\n");
        break;

        case PIXEL_FORMAT_VY1UY0_PACKAGE_422:
        printf("PIXEL_FORMAT_VY1UY0_PACKAGE_422\r\n");
        break;

        case PIXEL_FORMAT_YUV_400:
        printf("PIXEL_FORMAT_YUV_400\r\n");
        break;

        case PIXEL_FORMAT_UV_420:
        printf("PIXEL_FORMAT_UV_420\r\n");
        break;

        case PIXEL_FORMAT_BGR_888_PLANAR:
        printf("PIXEL_FORMAT_BGR_888_PLANAR\r\n");
        break;

        case PIXEL_FORMAT_HSV_888_PACKAGE:
        printf("PIXEL_FORMAT_HSV_888_PACKAGE\r\n");
        break;

        case PIXEL_FORMAT_HSV_888_PLANAR:
        printf("PIXEL_FORMAT_HSV_888_PLANAR\r\n");
        break;

        case PIXEL_FORMAT_LAB_888_PACKAGE:
        printf("PIXEL_FORMAT_LAB_888_PACKAGE\r\n");
        break;

        case PIXEL_FORMAT_LAB_888_PLANAR:
        printf("PIXEL_FORMAT_LAB_888_PLANAR\r\n");
        break;

        case PIXEL_FORMAT_S8C1:
        printf("PIXEL_FORMAT_S8C1\r\n");
        break;

        case PIXEL_FORMAT_S8C2_PACKAGE:
        printf("PIXEL_FORMAT_S8C2_PACKAGE\r\n");
        break;

        case PIXEL_FORMAT_S8C2_PLANAR:
        printf("PIXEL_FORMAT_S8C2_PLANAR\r\n");
        break;

        case PIXEL_FORMAT_S8C3_PLANAR:
        printf("PIXEL_FORMAT_S8C3_PLANAR\r\n");
        break;

        case PIXEL_FORMAT_S16C1:
        printf("PIXEL_FORMAT_S16C1\r\n");
        break;

        case PIXEL_FORMAT_U8C1:
        printf("PIXEL_FORMAT_U8C1\r\n");
        break;

        case PIXEL_FORMAT_U16C1:
        printf("PIXEL_FORMAT_U16C1\r\n");
        break;

        case PIXEL_FORMAT_S32C1:
        printf("PIXEL_FORMAT_S32C1\r\n");
        break;

        case PIXEL_FORMAT_U32C1:
        printf("PIXEL_FORMAT_U32C1\r\n");
        break;

        case PIXEL_FORMAT_U64C1:
        printf("PIXEL_FORMAT_U64C1\r\n");
        break;

        case PIXEL_FORMAT_S64C1:
        printf("PIXEL_FORMAT_S64C1\r\n");
        break;

        case PIXEL_FORMAT_BUTT:
        printf("PIXEL_FORMAT_BUTT\r\n");
        break;
    }
}

/******************************************************************************
* function : Set system memory location
******************************************************************************/
HI_S32 DHS_COMM_SYS_MemConfig(HI_VOID)
{
    
    printf("Do DHS_COMM_SYS_MemConfig\n");
    printf("VI_MAX_PIPE_NUM %d\n",VI_MAX_PIPE_NUM);
    printf("VI_MAX_CHN_NUM %d\n",VI_MAX_CHN_NUM);
    HI_S32 i, j;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR* pcMmzName = NULL;
    MPP_CHN_S stMppChn;

    /*config memory for vi*/
    for (i = 0; i < VI_MAX_PIPE_NUM; i++)
    {
        for (j = 0; j < VI_MAX_CHN_NUM; j++)
        {
            stMppChn.enModId  = HI_ID_VI;
            stMppChn.s32DevId = i;
            stMppChn.s32ChnId = j;
            s32Ret = HI_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

            if (s32Ret)
            {
                printf("HI_MPI_SYS_SetMemConfig ERR !\n");
                return HI_FAILURE;
            }
            else
            {
                printf("HI_MPI_SYS_SetMemConfig Pass %d %d\n",i,j);
            }
        }
    }

    /*config memory for avs */
  /*  for (i = 0; i < AVS_MAX_GRP_NUM; i++)
    {
        stMppChn.enModId  = HI_ID_AVS;
        stMppChn.s32DevId = i;
        stMppChn.s32ChnId = 0;
        s32Ret = HI_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

        if (s32Ret)
        {
            printf("HI_MPI_SYS_SetMemConfig ERR !\n");
            return HI_FAILURE;
        }
    }
*/
    /*config memory for vpss */
 /*   for (i = 0; i < VPSS_MAX_GRP_NUM; i++)
    {
        stMppChn.enModId  = HI_ID_VPSS;
        stMppChn.s32DevId = i;
        stMppChn.s32ChnId = 0;
        s32Ret = HI_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

        if (s32Ret)
        {
            printf("HI_MPI_SYS_SetMemConfig ERR !\n");
            return HI_FAILURE;
        }
    }
*/
    /*config memory for venc */
 /*   for (i = 0; i < VENC_MAX_CHN_NUM; i++)
    {

        stMppChn.enModId  = HI_ID_VENC;
        stMppChn.s32DevId = 0;
        stMppChn.s32ChnId = i;
        s32Ret = HI_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

        if (s32Ret)
        {
            printf("HI_MPI_SYS_SetMemConf ERR !\n");
            return HI_FAILURE;
        }
    }
*/
    /*config memory for vo*/
 /*   for (i = 0; i < VO_MAX_LAYER_NUM; i++)
    {
        for (j = 0; j < VO_MAX_CHN_NUM; j++)
        {
            stMppChn.enModId    = HI_ID_VO;
            stMppChn.s32DevId = i;
            stMppChn.s32ChnId = j;
            s32Ret = HI_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

            if (s32Ret)
            {
                printf("HI_MPI_SYS_SetMemConfig ERR !\n");
                return HI_FAILURE;
            }
        }
    }
*/
    /*config memory for vdec */
  /*  for (i = 0; i < VDEC_MAX_CHN_NUM; i++)
    {

        stMppChn.enModId  = HI_ID_VDEC;
        stMppChn.s32DevId = 0;
        stMppChn.s32ChnId = i;
        s32Ret = HI_MPI_SYS_SetMemConfig(&stMppChn, pcMmzName);

        if (s32Ret)
        {
            printf("HI_MPI_SYS_SetMemConf ERR !\n");
            return HI_FAILURE;
        }
    }

*/
    return s32Ret;
}

HI_S32 SAMPLE_VI_BackupAttr(VI_CHN ViChn)
{
  /*  VI_CHN_ATTR_S stChnAttr;

    //MEM_DEV_OPEN();

    if (ViChn != 0)
    {
        return 0;
    }

    if (DHS_MPI_VI_GetChnAttr(ViChn, &stChnAttrBackup))
    {
        printf("DHS_MPI_VI_GetChnAttr err, vi chn %d \n", ViChn);
        return -1;
    }

    // clear list 
    DHS_MPI_VI_SetFrameDepth(ViChn, 0);
    sleep(1);

    printf("compress mode: %d -> %d. \n", stChnAttrBackup.enCompressMode, COMPRESS_MODE_NONE);
    // compress none 
    memcpy(&stChnAttr, &stChnAttrBackup, sizeof(VI_CHN_ATTR_S));

    stChnAttr.enCompressMode = COMPRESS_MODE_NONE;


    if (DHS_MPI_VI_SetChnAttr(ViChn, &stChnAttr))
    {
        printf("DHS_MPI_VI_SetChnAttr err, vi chn %d \n", ViChn);
        return -1;
    }
*/
    return 0;
}

static void sample_yuv_8bit_dump(VIDEO_FRAME_S* pVBuf, FILE* pfd)
{
    
        printf("sample_yuv_8bit_dump\n");
    unsigned int w, h;
    char* pVBufVirt_Y;
    char* pVBufVirt_C;
    char* pMemContent;
    unsigned char TmpBuff[MAX_FRM_WIDTH];

    HI_U64 phy_addr;
    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
    VIDEO_FORMAT_E  enVideoFormat = stFrame.stVFrame.enVideoFormat;
    HI_U32 u32UvHeight = 0;

    if (PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat)
    {
        if (VIDEO_FORMAT_TILE_16x8 != enVideoFormat)
        {
            u32Size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height) * 3 / 2;
            u32UvHeight = pVBuf->u32Height / 2;
        }
        else
        {
            u32Size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height);
            u32UvHeight = 0;
        }
    }
    else if(PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat)
    {
        u32Size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height) * 2;
        u32UvHeight = pVBuf->u32Height;
    }
    else if(PIXEL_FORMAT_YUV_400 == enPixelFormat)
    {
        u32Size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height);
        u32UvHeight = pVBuf->u32Height;
    }

    phy_addr = pVBuf->u64PhyAddr[0];

    //printf("phy_addr:%x, size:%d\n", phy_addr, size);
    pUserPageAddr[0] = (HI_CHAR*) HI_MPI_SYS_Mmap(phy_addr, u32Size);
    if (HI_NULL == pUserPageAddr[0])
    {
        return;
    }
    //printf("stride: %d,%d\n",pVBuf->u32Stride[0],pVBuf->u32Stride[1] );

    pVBufVirt_Y = pUserPageAddr[0];
    pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0]) * (pVBuf->u32Height);

    /* save Y ----------------------------------------------------------------*/
    fprintf(stderr, "saving......Y......");
    fflush(stderr);
    if(VIDEO_FORMAT_TILE_16x8 == enVideoFormat)
    {
        for (h = 0; h < pVBuf->u32Height; h++)
        {
            pMemContent = pVBufVirt_Y + h * pVBuf->u32Stride[0];
            fwrite(pMemContent, pVBuf->u32Stride[0], 1, pfd);
        }
    }
    else
    {
        for (h = 0; h < pVBuf->u32Height; h++)
        {
            pMemContent = pVBufVirt_Y + h * pVBuf->u32Stride[0];
            fwrite(pMemContent, pVBuf->u32Width, 1, pfd);
        }
    }

    if(PIXEL_FORMAT_YUV_400 != enPixelFormat && VIDEO_FORMAT_TILE_16x8 != enVideoFormat)
    {
        fflush(pfd);
        /* save U ----------------------------------------------------------------*/
        fprintf(stderr, "U......");
        fflush(stderr);

        for (h = 0; h < u32UvHeight; h++)
        {
            pMemContent = pVBufVirt_C + h * pVBuf->u32Stride[1];

            pMemContent += 1;

            for (w = 0; w < pVBuf->u32Width / 2; w++)
            {
                TmpBuff[w] = *pMemContent;
                pMemContent += 2;
            }
            fwrite(TmpBuff, pVBuf->u32Width / 2, 1, pfd);
        }
        fflush(pfd);

        /* save V ----------------------------------------------------------------*/
        fprintf(stderr, "V......");
        fflush(stderr);
        for (h = 0; h < u32UvHeight; h++)
        {
            pMemContent = pVBufVirt_C + h * pVBuf->u32Stride[1];

            for (w = 0; w < pVBuf->u32Width / 2; w++)
            {
                TmpBuff[w] = *pMemContent;
                pMemContent += 2;
            }
            fwrite(TmpBuff, pVBuf->u32Width / 2, 1, pfd);
        }
    }
    fflush(pfd);

    fprintf(stderr, "done %d!\n", pVBuf->u32TimeRef);
    fflush(stderr);

    HI_MPI_SYS_Munmap(pUserPageAddr[0], u32Size);
    pUserPageAddr[0] = HI_NULL;
}


void sample_yuv_10bit_dump(VIDEO_FRAME_S* pVBuf, FILE* pfd, HI_U32 u32ByteAlign)
{
    
        printf("sample_yuv_10bit_dump\n");
    unsigned int w, h, k, wy, wuv;
    char* pVBufVirt_Y;
    char* pVBufVirt_C;
    char* pMemContent;
    char* pMem;
    HI_U64 phy_addr;
    HI_U16  src[MAX_FRM_WIDTH];
    HI_U8  dest[MAX_FRM_WIDTH];
    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
    VIDEO_FORMAT_E  enVideoFormat = stFrame.stVFrame.enVideoFormat;
    HI_U32 u32UvHeight = 0;
    HI_U32 u32YWidth;

    if (PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat)
    {
        u32Size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height) * 3 / 2;
        u32UvHeight = pVBuf->u32Height / 2;
    }
    else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat)
    {
        u32Size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height) * 2;
        u32UvHeight = pVBuf->u32Height;
    }
    else if (PIXEL_FORMAT_YUV_400 == enPixelFormat)
    {
        u32Size = (pVBuf->u32Stride[0]) * (pVBuf->u32Height);
        u32UvHeight = pVBuf->u32Height;
    }

    u32YWidth = (pVBuf->u32Width * 10 + 7) / 8;

    phy_addr = pVBuf->u64PhyAddr[0];
    pUserPageAddr[0] = (HI_CHAR*) HI_MPI_SYS_Mmap(phy_addr, u32Size);

    if (HI_NULL == pUserPageAddr[0])
    {
        return;
    }

    pVBufVirt_Y = pUserPageAddr[0];
    pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0]) * (pVBuf->u32Height);

    /* save Y ----------------------------------------------------------------*/
    fprintf(stderr, "saving......Y......");
    fflush(stderr);

    if (VIDEO_FORMAT_TILE_16x8 == enVideoFormat)
    {
        for (h = 0; h < pVBuf->u32Height; h++)
        {
            pMemContent = pVBufVirt_Y + h * pVBuf->u32Stride[0];
            fwrite(pMemContent, pVBuf->u32Stride[0], 1, pfd);
        }
    }

    else
    {
        /* 16_byte align */
        for (h = 0; h < pVBuf->u32Height; h++)
        {
            pMemContent = pVBufVirt_Y + h * pVBuf->u32Stride[0];

            if (1 == u32ByteAlign)
            {
                wy = 0;

                for (w = 0; w < u32YWidth - 1; w++)
                {
                    dest[w] = *pMemContent;
                    dest[w + 1] = *(pMemContent + 1);
                    k = wy % 4;

                    switch (k)
                    {
                        case 0:
                            src[wy] = (((HI_U16)(dest[w]))) + (((dest[w + 1]) & 0x3) << 8);
                            break;

                        case 1:
                            src[wy] = ((((HI_U16)(dest[w])) & 0xfc) >> 2) + (((HI_U16)(dest[w + 1]) & 0xf) << 6);
                            break;

                        case 2:
                            src[wy] = ((((HI_U16)(dest[w])) & 0xf0) >> 4) + (((HI_U16)(dest[w + 1]) & 0x3f) << 4);
                            break;

                        case 3:
                            src[wy] = ((((HI_U16)(dest[w])) & 0xc0) >> 6) + ((HI_U16)(dest[w + 1]) << 2);
                            w++;
                            pMemContent += 1;
                            break;
                    }

                    pMemContent += 1;
                    wy++;
                }

                fwrite(src, pVBuf->u32Width * 2, 1, pfd);
            }
            else
            {
                fwrite(pMemContent, pVBuf->u32Stride[0], 1, pfd);
            }
        }

    }


    if (PIXEL_FORMAT_YUV_400 != enPixelFormat && VIDEO_FORMAT_TILE_16x8 != enVideoFormat)
    {
        fflush(pfd);
        /* save U ----------------------------------------------------------------*/
        fprintf(stderr, "U......");
        fflush(stderr);

        if (1 == u32ByteAlign)
        {
            for (h = 0; h < u32UvHeight; h++)
            {
                pMemContent = pVBufVirt_C + h * pVBuf->u32Stride[1]; // pMemContent = pVBufVirt_C + h * pVBuf->u32Stride[1];

                //pMemContent += 1;
                wy = 0;
                wuv = 0;

                for (w = 0; w < u32YWidth - 1; w++)
                {
                    dest[w] = *pMemContent;
                    dest[w + 1] = *(pMemContent + 1);
                    k = wuv % 4;

                    switch (k)
                    {
                        case 0:
                            // src[wy] = (((HI_U16)(dest[w]))) + (((dest[w+1])&0x3)<<8);
                            break;

                        case 1:
                            src[wy] = ((((HI_U16)(dest[w])) & 0xfc) >> 2) + (((HI_U16)(dest[w + 1]) & 0xf) << 6);
                            wy++;
                            break;

                        case 2:
                            //src[wy] = ((((HI_U16)(dest[w]))&0xf0)>>4) + (((HI_U16)(dest[w+1])&0x3f)<<4);
                            break;

                        case 3:
                            src[wy] = ((((HI_U16)(dest[w])) & 0xc0) >> 6) + ((HI_U16)(dest[w + 1]) << 2);
                            wy++;
                            w++;
                            pMemContent += 1;
                            break;
                    }

                    wuv++;
                    pMemContent += 1;

                }

                fwrite(src, pVBuf->u32Width , 1, pfd);
            }

            fflush(pfd);

            /* save V ----------------------------------------------------------------*/
            fprintf(stderr, "V......");
            fflush(stderr);

            for (h = 0; h < u32UvHeight; h++)
            {
                pMemContent = pVBufVirt_C + h * pVBuf->u32Stride[1];
                wy = 0;
                wuv = 0;

                for (w = 0; w < u32YWidth - 1; w++)
                {
                    dest[w] = *pMemContent;
                    dest[w + 1] = *(pMemContent + 1);
                    k = wuv % 4;

                    switch (k)
                    {
                        case 0:
                            src[wy] = (((HI_U16)(dest[w]))) + (((dest[w + 1]) & 0x3) << 8);
                            wy++;
                            break;

                        case 1:
                            //src[wy] = ((((HI_U16)(dest[w]))&0xfc)>>2) + (((HI_U16)(dest[w+1])&0xf)<<6);
                            break;

                        case 2:
                            src[wy] = ((((HI_U16)(dest[w])) & 0xf0) >> 4) + (((HI_U16)(dest[w + 1]) & 0x3f) << 4);
                            wy++;
                            break;

                        case 3:
                            //src[wy] = ((((HI_U16)(dest[w]))&0xc0)>>6) + ((HI_U16)(dest[w+1])<<2);
                            w++;
                            pMemContent += 1;
                            break;
                    }

                    pMemContent += 1;
                    wuv ++;
                }

                fwrite(src, pVBuf->u32Width, 1, pfd);
            }
        }
        else
        {
            for (h = 0; h < u32UvHeight; h++)
            {
                pMem = pVBufVirt_C + h * pVBuf->u32Stride[1]; // pMemContent = pVBufVirt_C + h * pVBuf->u32Stride[1];

                if (0 == h % 2)
                {
                    wy = 0;
                }

                for (w = 0; w < (u32YWidth - 19); w += 20)
                {
                    src[wy] = (((pMem[w + 1]) & 0xfc) >> 2) + (((pMem[w + 2]) & 0xf) << 6) + (((pMem[w + 3]) & 0xc0) << 4) + (((pMem[w + 4]) & 0xf) << 12);
                    src[wy + 1] = (((pMem[w + 4]) & 0xf0) >> 4) + ((pMem[w + 6] & 0xfc) << 2) + (((pMem[w + 7]) & 0xf) << 10) + (((pMem[w + 8]) & 0xc0) << 8);
                    src[wy + 2] = ((pMem[w + 9]) & 0xff) + (((pMem[w + 11]) & 0xfc) << 6) + ((pMem[w + 12] & 0x3) << 14);
                    src[wy + 3] = (((pMem[w + 12]) & 0xc) >> 2) + (((pMem[w + 13]) & 0xc0) >> 4) + (((pMem[w + 14]) & 0xff) << 4) + (((pMem[w + 16]) & 0x3c) << 10);
                    src[wy + 4] = (((pMem[w + 16]) & 0xc0) >> 6) + (((pMem[w + 17]) & 0xf) << 2) + ((pMem[w + 18]) & 0xc0) + (((pMem[w + 19]) & 0xff) << 8);

                    pMem += 20;
                    wy += 5;
                }

                fwrite(src, (pVBuf->u32Stride[1] / 2), 1, pfd);
            }

            fflush(pfd);

            /* save V ----------------------------------------------------------------*/
            fprintf(stderr, "V......");
            fflush(stderr);

            for (h = 0; h < u32UvHeight; h++)
            {
                pMem = pVBufVirt_C + h * pVBuf->u32Stride[1]; // pMemContent = pVBufVirt_C + h * pVBuf->u32Stride[1];
                wy = 0;

                for (w = 0; w < u32YWidth - 19; w += 20)
                {
                    src[wy] = (pMem[w] & 0xff) + (((pMem[w + 1]) & 0x3) << 8) + (((pMem[w + 2]) & 0xf0) << 6) + (((pMem[w + 3]) & 0x3) << 14);
                    src[wy + 1] = (((pMem[w + 3]) & 0x3c) >> 2) + (pMem[w + 5] & 0xff) + (((pMem[w + 6]) & 0x3) << 12) + (((pMem[w + 7]) & 0x30) << 10);
                    src[wy + 2] = (((pMem[w + 7]) & 0xc0) >> 6) + (((pMem[w + 8]) & 0x3f) << 2) + (pMem[w + 10] & 0xff);
                    src[wy + 3] = ((pMem[w + 11]) & 0x3) + (((pMem[w + 12]) & 0xf0) >> 2) + (((pMem[w + 13]) & 0x3f) << 6) + (((pMem[w + 15]) & 0xf) << 12);
                    src[wy + 4] = (((pMem[w + 15]) & 0xf0) >> 4) + (((pMem[w + 16]) & 0x3) << 4) + (((pMem[w + 17]) & 0xf0) << 2) + (((pMem[w + 18]) & 0x3f) << 10);

                    pMem += 20;
                    wy += 5;
                }

                fwrite(src, (pVBuf->u32Stride[1] / 2), 1, pfd);
            }
        }
    }

    fflush(pfd);

    fprintf(stderr, "done %d!\n", pVBuf->u32TimeRef);
    fflush(stderr);

    HI_MPI_SYS_Munmap(pUserPageAddr[0], u32Size);
    pUserPageAddr[0] = HI_NULL;
}


HI_S32 SAMPLE_MISC_GETVB(VIDEO_FRAME_INFO_S* pstOutFrame, VIDEO_FRAME_INFO_S* pstInFrame,
                         VB_BLK* pstVbBlk, VB_POOL pool)
{
  /*  HI_U32 u32Size;
    VB_BLK VbBlk = VB_INVALID_HANDLE;
    HI_U32 u64PhyAddr;
    HI_VOID* pVirAddr;
    HI_U32 u32LumaSize, u32ChrmSize;
    HI_U32 u32LStride, u32CStride;
    HI_U32 u32Width, u32Height;


    u32Width = pstInFrame->stVFrame.u32Width;
    u32Height = pstInFrame->stVFrame.u32Height;
    u32LStride = pstInFrame->stVFrame.u32Stride[0];
    u32CStride = pstInFrame->stVFrame.u32Stride[1];
    if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 == pstInFrame->stVFrame.enPixelFormat)
    {
        u32Size =  u32LStride * u32Height << 1;
        u32LumaSize = u32LStride * u32Height;
        u32ChrmSize = u32LumaSize;
    }
    else if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == pstInFrame->stVFrame.enPixelFormat)
    {
        u32Size = (3 * u32LStride * u32Height) >> 1;
        u32LumaSize = u32LStride * u32Height;
        u32ChrmSize = u32LumaSize >> 1;
    }
    else if (PIXEL_FORMAT_RGB_BAYER == pstInFrame->stVFrame.enPixelFormat)
    {
        u32Size = u32LStride * u32Height;
        u32LumaSize = u32LStride * u32Height;
        u32ChrmSize = 0;
    }
    else if (PIXEL_FORMAT_YUV_400 == pstInFrame->stVFrame.enPixelFormat)
    {
        u32Size     = u32LStride * u32Height;
        u32LumaSize = u32LStride * u32Height;
        u32ChrmSize = 0;
    }
    else
    {
        printf("Error!!!, not support PixelFormat: %d\n", pstInFrame->stVFrame.enPixelFormat);
        return HI_FAILURE;
    }

    VbBlk = HI_MPI_VB_GetBlock(pool, u32Size, HI_NULL);
    *pstVbBlk = VbBlk;

    if (VB_INVALID_HANDLE == VbBlk)
    {
        printf("HI_MPI_VB_GetBlock err! size:%d\n", u32Size);
        return HI_FAILURE;
    }

    u64PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
    if (0 == u64PhyAddr)
    {
        printf("HI_MPI_VB_Handle2PhysAddr err!\n");
        return HI_FAILURE;
    }

    pVirAddr = (HI_U8*) HI_MPI_SYS_Mmap(u64PhyAddr, u32Size);
    if (NULL == pVirAddr)
    {
        printf("HI_MPI_SYS_Mmap err!\n");
        return HI_FAILURE;
    }

    pstOutFrame->u32PoolId = HI_MPI_VB_Handle2PoolId(VbBlk);
    if (VB_INVALID_POOLID == pstOutFrame->u32PoolId)
    {
        printf("u32PoolId err!\n");
        return HI_FAILURE;
    }

    pstOutFrame->stVFrame.u64PhyAddr[0] = u64PhyAddr;

    //printf("\nuser u32phyaddr = 0x%x\n", pstOutFrame->stVFrame.u64PhyAddr[0]);
    pstOutFrame->stVFrame.u64PhyAddr[1] = pstOutFrame->stVFrame.u64PhyAddr[0] + u32LumaSize;
    pstOutFrame->stVFrame.u64PhyAddr[2] = pstOutFrame->stVFrame.u64PhyAddr[1] + u32ChrmSize;

    pstOutFrame->stVFrame.pVirAddr[0] = pVirAddr;
    pstOutFrame->stVFrame.pVirAddr[1] = (HI_VOID*)((unsigned long)pstOutFrame->stVFrame.pVirAddr[0] + u32LumaSize);
    pstOutFrame->stVFrame.pVirAddr[2] = (HI_VOID*)((unsigned long)pstOutFrame->stVFrame.pVirAddr[1] + u32ChrmSize);

    pstOutFrame->stVFrame.u32Width  = u32Width;
    pstOutFrame->stVFrame.u32Height = u32Height;
    pstOutFrame->stVFrame.u32Stride[0] = u32LStride;
    pstOutFrame->stVFrame.u32Stride[1] = u32CStride;
    pstOutFrame->stVFrame.u32Stride[2] = u32CStride;
    pstOutFrame->stVFrame.u32Field = VIDEO_FIELD_FRAME;
    pstOutFrame->stVFrame.enCompressMode = COMPRESS_MODE_NONE;
    pstOutFrame->stVFrame.enPixelFormat = pstInFrame->stVFrame.enPixelFormat;
 */
    return HI_SUCCESS;
}
static HI_S32 VI_Restore(VI_PIPE Pipe)
{
    HI_S32 s32Ret= HI_FAILURE;

    printf("Do VI_Restore\n");
    if(VB_INVALID_POOLID != stFrame.u32PoolId)
    {
        //s32Ret = HI_MPI_VI_ReleasePipeFrame(Pipe, &stFrame);
        s32Ret = DHS_MPI_VI_ReleasePipeFrame(Pipe, &stFrame);
        if(HI_SUCCESS != s32Ret)
        {
             printf("Release Chn Frame error!!!\n");
        }
        stFrame.u32PoolId = VB_INVALID_POOLID;
    }
    if(-1 != hHandle)
    {
       // HI_MPI_VGS_CancelJob(hHandle);
        DHS_MPI_VGS_CancelJob(hHandle);
        hHandle = -1;
    }
    if(HI_NULL != stMem.pVirAddr)
    {
        HI_MPI_SYS_Munmap((HI_VOID*)stMem.pVirAddr, u32BlkSize );
        stMem.u64PhyAddr = HI_NULL;
    }
    if(VB_INVALID_POOLID != stMem.hPool)
    {
        HI_MPI_VB_ReleaseBlock(stMem.hBlock);
        stMem.hPool = VB_INVALID_POOLID;
    }

    if (VB_INVALID_POOLID != hPool)
    {
        HI_MPI_VB_DestroyPool( hPool );
        hPool = VB_INVALID_POOLID;
    }

    if(HI_NULL != pUserPageAddr[0])
    {
        HI_MPI_SYS_Munmap(pUserPageAddr[0], u32Size);
        pUserPageAddr[0] = HI_NULL;
    }

    if(pfd_test)
    {
        fclose(pfd_test);
        
        printf("fclose\n");
        pfd_test = HI_NULL;
    }

    //s32Ret = HI_MPI_VI_SetPipeDumpAttr(Pipe, &astBackUpDumpAttr);
    
    s32Ret = DHS_MPI_VI_SetPipeDumpAttr(Pipe, &astBackUpDumpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Set Pipe %d dump attr failed!\n",Pipe);
        return s32Ret;
    }

    return HI_SUCCESS;
}

//HI_S32 SAMPLE_MISC_ViDump(VI_CHN ViChn, HI_U32 u32Cnt,int sfd)
HI_S32 DHL_MISC_ViDump_Liunx(DHS_Photo_ATTR_S*  stDHS_Photo)
{
    
    MLOGI("DHL_SAMPLE_MISC_ViDump \n");
    HI_CHAR szYuvName[128];
    HI_CHAR szPixFrm[10];
    HI_CHAR szVideoFrm[10];    
    
 //   VIDEO_FRAME_INFO_S stFrame;
 //   VIDEO_FRAME_INFO_S* pstOutFrame;
 //   VIDEO_FRAME_INFO_S *astFrame = HI_NULL;
  //  VGS_HANDLE hHandle= -1;
  
    HI_U32 u32ByteAlign = 1;
    HI_U32 u32Cnt = 1;
    HI_U32 u32Depth = 2;
    HI_S32 s32MilliSec = -1;
    HI_S32 s32Times = 10;
    HI_BOOL bSendToVgs = HI_FALSE;
    VIDEO_FRAME_INFO_S stFrmInfo;
    VGS_TASK_ATTR_S stTask;
    HI_U32 u32LumaSize = 0;
    HI_U32 u32PicLStride = 0;
    HI_U32 u32PicCStride = 0;
    HI_U32 u32Width = 0;
    HI_U32 u32Height = 0;
    HI_S32 i = 0;
    HI_S32 s32Ret;
    HI_U32 u32BitWidth;
    VI_DUMP_ATTR_S    stDumpAttr;
    VB_POOL_CONFIG_S stVbPoolCfg;
    
   // VI_DUMP_ATTR_S astBackUpDumpAttr;
    //VB_BLK VbBlk = VB_INVALID_HANDLE;
  //  VB_POOL hPool  = VB_INVALID_POOLID;

        printf("stDHS_Photo->ViChn %d\n",stDHS_Photo->ViChn);
        printf("Do HI_MPI_VI_GetPipeDumpAttr\n");
        
    s32Ret = DHS_MPI_VI_GetPipeDumpAttr(stDHS_Photo->ViChn, &astBackUpDumpAttr);
    //s32Ret = HI_MPI_VI_GetPipeDumpAttr(stDHS_Photo->ViChn, &astBackUpDumpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Get stDHS_Photo->ViChn %d dump attr failed!\n", stDHS_Photo->ViChn);
        return HI_PDT_SCENE_EINVAL;
    } 
    

    printf("astBackUpDumpAttr.bEnable %d\n",astBackUpDumpAttr.bEnable);       
    printf("astBackUpDumpAttr.u32Depth %d\n",astBackUpDumpAttr.u32Depth);   
    printf("astBackUpDumpAttr.enDumpType %d\n",astBackUpDumpAttr.enDumpType);  
    memcpy(&stDumpAttr, &astBackUpDumpAttr, sizeof(VI_DUMP_ATTR_S));
    stDumpAttr.bEnable  = HI_TRUE;
    stDumpAttr.u32Depth = u32Depth;
   // stDumpAttr.enDumpType=VI_DUMP_TYPE_YUV;
        printf("Do HI_MPI_VI_SetPipeDumpAttr\n");
   // s32Ret = HI_MPI_VI_SetPipeDumpAttr(stDHS_Photo->ViChn, &stDumpAttr);
    
    s32Ret = DHS_MPI_VI_SetPipeDumpAttr(stDHS_Photo->ViChn, &stDumpAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("Set stDHS_Photo->ViChn %d dump attr failed!\n",stDHS_Photo->ViChn);
        return HI_PDT_SCENE_EINVAL;
    }

    memset(&stFrame, 0, sizeof(stFrame));
    stFrame.u32PoolId = VB_INVALID_POOLID;
    
        printf("Do DHS_MPI_VI_GetPipeFrame\n");
        
    printf("BE stFrame.stVFrame.u32Width %d\n",stFrame.stVFrame.u32Width);
    printf("BE stFrame.stVFrame.u32Height %d\n",stFrame.stVFrame.u32Height);
  //  while (HI_MPI_VI_GetPipeFrame(stDHS_Photo->ViChn, &stFrame, s32MilliSec) != HI_SUCCESS)
    while (DHS_MPI_VI_GetPipeFrame(stDHS_Photo->ViChn, &stFrame, s32MilliSec) != HI_SUCCESS)
    {
        s32Times--;
        if(0 >= s32Times)
        {
            printf("get frame error for 10 times,now exit !!!\n");
            VI_Restore(stDHS_Photo->ViChn);
            return HI_PDT_SCENE_EINVAL;
        }
        
        printf("wait DHS_MPI_VI_GetPipeFrame = %d\n",s32Times);
        HI_usleep(40000);
    }

    printf("stFrame.stVFrame.enPixelFormat %d\n",stFrame.stVFrame.enPixelFormat);
    Print_enVideoFormat(stFrame.stVFrame.enPixelFormat);
    switch (stFrame.stVFrame.enPixelFormat)
    {
        case PIXEL_FORMAT_YVU_SEMIPLANAR_420:
            snprintf(szPixFrm, 10, "P420");
            break;
        case PIXEL_FORMAT_YVU_SEMIPLANAR_422:
            snprintf(szPixFrm, 10, "P422");
            break;
        case PIXEL_FORMAT_YUV_400:
            snprintf(szPixFrm, 10, "P400");
            break;
        default:
            printf("pixel format err, pipe_yuvdump is dump YUV, not RAW.\n");
            snprintf(szPixFrm, 10, "--");
            break;
    }


    printf("stFrame.stVFrame.enVideoFormat %d\n",stFrame.stVFrame.enVideoFormat);
    

    switch (stFrame.stVFrame.enVideoFormat)
    {
        case VIDEO_FORMAT_LINEAR:
            snprintf(szVideoFrm, 10, "linear");
            break;
        case VIDEO_FORMAT_TILE_64x16:
            snprintf(szVideoFrm, 10, "tile_64X16");
            break;
        case VIDEO_FORMAT_TILE_16x8:
            snprintf(szVideoFrm, 10, "tile_16X8");
            break;
        default:
            snprintf(szVideoFrm, 10, "--");
            break;
    }

    printf("stFrame.stVFrame.u32Width %d\n",stFrame.stVFrame.u32Width);
    printf("stFrame.stVFrame.u32Height %d\n",stFrame.stVFrame.u32Height);

    /* make file name */
    snprintf(szYuvName, 128, "./vi_pipe%d_w%d_h%d_%s_%s_%d.yuv", stDHS_Photo->ViChn,stFrame.stVFrame.u32Width, stFrame.stVFrame.u32Height, szPixFrm, szVideoFrm, u32Cnt);
    printf("Dump YUV frame of vi pipe %d  to file: \"%s\"\n", stDHS_Photo->ViChn, szYuvName);
    fflush(stdout);

        printf("Do HI_MPI_VI_ReleasePipeFrame\n");
        //s32Ret = HI_MPI_VI_ReleasePipeFrame(stDHS_Photo->ViChn, &stFrame);
        s32Ret = DHS_MPI_VI_ReleasePipeFrame(stDHS_Photo->ViChn, &stFrame);
    if(HI_SUCCESS != s32Ret)
    {
        printf("Release frame error ,now exit !!!\n");
        VI_Restore(stDHS_Photo->ViChn);
        return HI_PDT_SCENE_EINVAL;
    }
    stFrame.u32PoolId = VB_INVALID_POOLID;
     /* open file */
    pfd_test = fopen(szYuvName, "wb");

    if (HI_NULL == pfd_test)
    {
        printf("open file failed:%s!\n", strerror(errno));
      //  VI_Restore(stDHS_Photo->ViChn);
      //  return HI_PDT_SCENE_EINVAL;
    }

    /* get frame  */
    while (u32Cnt--)
    {
        
        printf("Do DHS_MPI_VI_GetPipeFrame\n");
       // if (HI_MPI_VI_GetPipeFrame(stDHS_Photo->ViChn, &stFrame, s32MilliSec) != HI_SUCCESS)
        if (DHS_MPI_VI_GetPipeFrame(stDHS_Photo->ViChn, &stFrame, s32MilliSec) != HI_SUCCESS)
        {
            printf("Get frame fail \n");
            HI_usleep(1000);
            continue;
        }

        bSendToVgs = ((COMPRESS_MODE_NONE != stFrame.stVFrame.enCompressMode)
                                    || (VIDEO_FORMAT_LINEAR != stFrame.stVFrame.enVideoFormat));

        if (bSendToVgs)
        {

            u32Width    = stFrame.stVFrame.u32Width;
            u32Height   = stFrame.stVFrame.u32Height;
            
            printf("u32Width %d\n",u32Width);
            printf("u32Height %d\n",u32Height);

            u32BitWidth = (DYNAMIC_RANGE_SDR8 == stFrame.stVFrame.enDynamicRange) ? 8 : 10;

            printf("u32BitWidth %d\n",u32BitWidth);

            u32PicLStride = ALIGN_UP((u32Width * u32BitWidth + 7) >> 3, 16);
            u32PicCStride = u32PicLStride;
            u32LumaSize = u32PicLStride * u32Height;
            
            printf("u32PicLStride %d\n",u32PicLStride);
            printf("u32PicCStride %d\n",u32PicCStride);
            printf("u32LumaSize %d\n",u32LumaSize);

            if(PIXEL_FORMAT_YVU_SEMIPLANAR_420 == stFrame.stVFrame.enPixelFormat)
            {
                u32BlkSize = u32PicLStride * u32Height * 3 >> 1;
            }
            else if(PIXEL_FORMAT_YVU_SEMIPLANAR_422 == stFrame.stVFrame.enPixelFormat)
            {
                u32BlkSize = u32PicLStride * u32Height * 2;
            }
            else if(PIXEL_FORMAT_YUV_400 == stFrame.stVFrame.enPixelFormat)
            {
                u32BlkSize = u32PicLStride * u32Height;
            }
            else
            {
                u32BlkSize=VI_GetRawBufferSize(u32Width, u32Height,stFrame.stVFrame.enPixelFormat, COMPRESS_MODE_NONE,DEFAULT_ALIGN);
               // u32BlkSize = u32PicLStride * u32Height;
                if(u32BlkSize<=0) 
                {

                    printf("Unsupported pixelformat %d\n",stFrame.stVFrame.enPixelFormat);
                }   
               // VI_Restore(stDHS_Photo->ViChn);
               // return HI_PDT_SCENE_EINVAL;
            }

       
        printf("u32BlkSize %d\n",u32BlkSize);
        MPP_CHN_S stSrcChn;
      //  MPP_CHN_S stDestChn;

        stSrcChn.enModId = HI_ID_VI;
        stSrcChn.s32DevId = 0;
        stSrcChn.s32ChnId = 0;

       // stDestChn.enModId = HI_ID_VPSS;
       // stDestChn.s32DevId = 0;
       // stDestChn.s32ChnId = 0;                
       // s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
        //2.在vi所在的内存的DDR中创建VB
        HI_CHAR *MmzName=NULL;
        printf("Do HI_MPI_SYS_GetMemConfig\n");
        s32Ret=HI_MPI_SYS_GetMemConfig(&stSrcChn, MmzName);    

             memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
            stVbPoolCfg.u64BlkSize  = u32BlkSize;
            stVbPoolCfg.u32BlkCnt   = 2;
            stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;
          //  stVbPoolCfg.acMmzName=MmzName;
        sprintf(stVbPoolCfg.acMmzName,"%s",MmzName);
        printf("stVbPoolCfg.u64BlkSize %lld\n",stVbPoolCfg.u64BlkSize);
        printf("stVbPoolCfg.acMmzName %s\n",stVbPoolCfg.acMmzName);

        printf("Do HI_MPI_VB_CreatePool\n");
            hPool   = HI_MPI_VB_CreatePool(&stVbPoolCfg);
            if (hPool == VB_INVALID_POOLID)
            {
                printf("HI_MPI_VB_CreatePool failed! \n");
                VI_Restore(stDHS_Photo->ViChn);
                return HI_PDT_SCENE_EINVAL;
            }



            stMem.hPool = hPool;

            while ((stMem.hBlock = HI_MPI_VB_GetBlock(stMem.hPool, u32BlkSize, HI_NULL)) == VB_INVALID_HANDLE)
            {
                ;
            }
            stMem.u64PhyAddr = HI_MPI_VB_Handle2PhysAddr(stMem.hBlock);

            stMem.pVirAddr = (HI_U8*) HI_MPI_SYS_Mmap(stMem.u64PhyAddr, u32BlkSize );
            if (stMem.pVirAddr == HI_NULL)
            {
                printf("Mem dev may not open\n");
                VI_Restore(stDHS_Photo->ViChn);
                return HI_PDT_SCENE_EINVAL;
            }

            memset(&stFrmInfo.stVFrame, 0, sizeof(VIDEO_FRAME_S));
            stFrmInfo.stVFrame.u64PhyAddr[0] = stMem.u64PhyAddr;
            stFrmInfo.stVFrame.u64PhyAddr[1] = stFrmInfo.stVFrame.u64PhyAddr[0] + u32LumaSize;

            stFrmInfo.stVFrame.u64VirAddr[0] = (HI_U64)(HI_UL)stMem.pVirAddr;
            stFrmInfo.stVFrame.u64VirAddr[1] = stFrmInfo.stVFrame.u64VirAddr[0] + u32LumaSize;

            stFrmInfo.stVFrame.u32Width     = u32Width;
            stFrmInfo.stVFrame.u32Height    = u32Height;
            stFrmInfo.stVFrame.u32Stride[0] = u32PicLStride;
            stFrmInfo.stVFrame.u32Stride[1] = u32PicCStride;

            stFrmInfo.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
            stFrmInfo.stVFrame.enPixelFormat  = stFrame.stVFrame.enPixelFormat;
            stFrmInfo.stVFrame.enVideoFormat  = VIDEO_FORMAT_LINEAR;
            stFrmInfo.stVFrame.enDynamicRange =  stFrame.stVFrame.enDynamicRange;

            stFrmInfo.stVFrame.u64PTS     = (i * 40);
            stFrmInfo.stVFrame.u32TimeRef = (i * 2);

            stFrmInfo.u32PoolId = hPool;
            stFrmInfo.enModId = HI_ID_VGS;

        printf("Do DHS_MPI_VGS_BeginJob\n");
          //  s32Ret = HI_MPI_VGS_BeginJob(&hHandle);
            s32Ret = DHS_MPI_VGS_BeginJob(&hHandle);
            if (s32Ret != HI_SUCCESS)
            {
                printf("DHS_MPI_VGS_BeginJob failed\n");
                hHandle = -1;
                VI_Restore(stDHS_Photo->ViChn);
                return HI_PDT_SCENE_EINVAL;
            }
            memcpy(&stTask.stImgIn, &stFrame, sizeof(VIDEO_FRAME_INFO_S));
            memcpy(&stTask.stImgOut , &stFrmInfo, sizeof(VIDEO_FRAME_INFO_S));
            
        printf("Do DHS_MPI_VGS_AddScaleTask\n");
           // s32Ret = HI_MPI_VGS_AddScaleTask(hHandle, &stTask, VGS_SCLCOEF_NORMAL);
            s32Ret = DHS_MPI_VGS_AddScaleTask(hHandle, &stTask, VGS_SCLCOEF_NORMAL);
            if (s32Ret != HI_SUCCESS)
            {
                printf("HI_MPI_VGS_AddScaleTask failed\n");
                VI_Restore(stDHS_Photo->ViChn);
                return HI_PDT_SCENE_EINVAL;
            }

        printf("Do DHS_MPI_VGS_EndJob\n");
           // s32Ret = HI_MPI_VGS_EndJob(hHandle);
            s32Ret = DHS_MPI_VGS_EndJob(hHandle);
            if (s32Ret != HI_SUCCESS)
            {
                printf("DHS_MPI_VGS_EndJob failed\n");
                VI_Restore(stDHS_Photo->ViChn);
                return HI_PDT_SCENE_EINVAL;
            }
            hHandle = -1;
            /* save VO frame to file */
        
            if(DYNAMIC_RANGE_SDR8 == stFrame.stVFrame.enDynamicRange)
            {
                sample_yuv_8bit_dump(&stFrmInfo.stVFrame, pfd_test);
            }
            else
            {
                sample_yuv_10bit_dump(&stFrmInfo.stVFrame, pfd_test, u32ByteAlign);
            }
            
           
            // save VI frame to file 
            
            // memcpy(&stDHS_Photo->stFrmInfo, &stFrmInfo.stVFrame, sizeof(VIDEO_FRAME_INFO_S));
            s32Ret = vi_dump_save_one_frame(&stFrmInfo.stVFrame, stDHS_Photo->sfd);
            
        printf("Do HI_MPI_VB_ReleaseBlock\n");
            HI_MPI_VB_ReleaseBlock(stMem.hBlock);

            stMem.hPool =  VB_INVALID_POOLID;
            hHandle = -1;
            if(HI_NULL != stMem.pVirAddr)
            {
                
        printf("Do HI_MPI_SYS_Munmap\n");
                HI_MPI_SYS_Munmap((HI_VOID*)stMem.pVirAddr, u32BlkSize );
                stMem.u64PhyAddr = HI_NULL;
            }
            if (hPool != VB_INVALID_POOLID)
            {
                
        printf("Do HI_MPI_VB_DestroyPool\n");
                HI_MPI_VB_DestroyPool( hPool );
                hPool = VB_INVALID_POOLID;
            }

        }
        else
        {
            
            // save VI frame to file 
          //  memcpy(&stDHS_Photo->stFrmInfo, &stFrmInfo.stVFrame, sizeof(VIDEO_FRAME_INFO_S));
            s32Ret = vi_dump_save_one_frame(&stFrmInfo.stVFrame, stDHS_Photo->sfd);
            
            if(DYNAMIC_RANGE_SDR8 == stFrame.stVFrame.enDynamicRange)
            {
                sample_yuv_8bit_dump(&stFrame.stVFrame, pfd_test);
            }
            else
            {
                sample_yuv_10bit_dump(&stFrame.stVFrame, pfd_test, u32ByteAlign);
            }
        }

        printf("Get ViPipe %d frame %d!!\n", stDHS_Photo->ViChn, u32Cnt);
        /* release frame after using */
        //s32Ret = HI_MPI_VI_ReleasePipeFrame(stDHS_Photo->ViChn, &stFrame);
        s32Ret = DHS_MPI_VI_ReleasePipeFrame(stDHS_Photo->ViChn, &stFrame);
        if(HI_SUCCESS != s32Ret)
        {
            printf("Release frame error ,now exit !!!\n");
            VI_Restore(stDHS_Photo->ViChn);
            return HI_PDT_SCENE_EINVAL;
        }

        stFrame.u32PoolId = VB_INVALID_POOLID;
    }
    VI_Restore(stDHS_Photo->ViChn);
    return HI_SUCCESS;
}
/*
HI_S32 SAMPLE_MISC_ViDump_old(DHS_Photo_ATTR_S*  stDHS_Photo)
{
   VI_CHN ViChn = 0;
    HI_U32 u32FrmCnt = 1;
    HI_S32 s32FrmCnt, j, s32Ret = HI_FAILURE;
    VIDEO_FRAME_INFO_S stFrame;
    VIDEO_FRAME_INFO_S* pstOutFrame;
    VIDEO_FRAME_INFO_S *astFrame = HI_NULL;
    HI_CHAR szYuvName[128] = {0};
    HI_CHAR szPixFrm[10] = {0};
    HI_S32 s32MilliSec = 2000;
    VGS_TASK_ATTR_S stTask;
    memset(&stTask, 0, sizeof(stTask));
    VGS_HANDLE hHandle= -1;
    VB_BLK VbBlk = VB_INVALID_HANDLE;
    VB_POOL hPool  = VB_INVALID_POOLID;
    HI_U32  u32BlkSize = 0;
    HI_U32 u32OldDepth = -1U;

    s32Ret = HI_MPI_VI_GetPipeDumpAttr(stDHS_Photo->ViChn, &astBackUpDumpAttr);
    if (DHS_MPI_VI_GetFrameDepth(stDHS_Photo->ViChn, &u32OldDepth))
    {
        printf("DHS_MPI_VI_GetFrameDepth err, vi chn %d \n", stDHS_Photo->ViChn);
        return HI_FAILURE;
    }
    
    if (DHS_MPI_VI_SetFrameDepth(stDHS_Photo->ViChn, 1))
    {
        printf("DHS_MPI_VI_SetFrameDepth err, vi chn %d \n", stDHS_Photo->ViChn);
        return HI_FAILURE;
    }

    usleep(90000);
    if (HI_TRUE == bQuit)
    {
        DHS_MPI_VI_SetFrameDepth(stDHS_Photo->ViChn, u32OldDepth);
        return HI_FAILURE;
    }

    if (DHS_MPI_VI_GetFrame(stDHS_Photo->ViChn, &stFrame, s32MilliSec))
    {
        printf("DHS_MPI_VI_GetFrame err, vi chn %d \n", stDHS_Photo->ViChn);
        DHS_MPI_VI_SetFrameDepth(stDHS_Photo->ViChn, u32OldDepth);
        return HI_FAILURE;
    }

    if (COMPRESS_MODE_NONE != stFrame.stVFrame.enCompressMode)
    {
        if (PIXEL_FORMAT_YUV_400 == stFrame.stVFrame.enPixelFormat)
        {
            u32BlkSize = stFrame.stVFrame.u32Stride[0] * stFrame.stVFrame.u32Height;
        }
        else if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == stFrame.stVFrame.enPixelFormat)
        {
            u32BlkSize = stFrame.stVFrame.u32Stride[0] * stFrame.stVFrame.u32Height * 3 >> 1;
        }
        else if (PIXEL_FORMAT_YUV_SEMIPLANAR_422 == stFrame.stVFrame.enPixelFormat)
        {
            u32BlkSize = stFrame.stVFrame.u32Stride[0] * stFrame.stVFrame.u32Height * 2;
        }
        else
        {
            printf("Not support this pix format %d!\n", stFrame.stVFrame.enPixelFormat);
            DHS_MPI_VI_ReleaseFrame(stDHS_Photo->ViChn, &stFrame);
            DHS_MPI_VI_SetFrameDepth(stDHS_Photo->ViChn, u32OldDepth);
            return HI_FAILURE;
        }

        //create comm vb pool
        hPool   = HI_MPI_VB_CreatePool(u32BlkSize, 2 , NULL);
        if (hPool == VB_INVALID_POOLID)
        {
            printf("HI_MPI_VB_CreatePool failed! \n");
            DHS_MPI_VI_ReleaseFrame(stDHS_Photo->ViChn, &stFrame);
            DHS_MPI_VI_SetFrameDepth(stDHS_Photo->ViChn, u32OldDepth);
            return HI_FAILURE;
        }
    }

    astFrame = (VIDEO_FRAME_INFO_S*)malloc(u32Cnt*sizeof(VIDEO_FRAME_INFO_S));
    if (HI_NULL == astFrame)
    {
        printf("malloc  %d failed! \n", (u32Cnt*sizeof(VIDEO_FRAME_INFO_S)));
        DHS_MPI_VI_ReleaseFrame(stDHS_Photo->ViChn, &stFrame);
        DHS_MPI_VI_SetFrameDepth(stDHS_Photo->ViChn, u32OldDepth);
        return HI_FAILURE;
    }

    // get VI frame  
    for (j= 0; j < u32Cnt; j++)
    {
        astFrame[j].stVFrame.u64PhyAddr[0] = 0;
    }
    for (j= 0; j < u32Cnt; j++)
    {
        if (HI_TRUE == bQuit)
        {
            break;
        }
        if (DHS_MPI_VI_GetFrame(stDHS_Photo->ViChn, &astFrame[j], s32MilliSec) < 0)
        {
            printf("get vi chn %d frame err\n", stDHS_Photo->ViChn);
            break;
        }
    }
    s32FrmCnt = j;   // frame count is s32FrmCnt 
    DHS_MPI_VI_ReleaseFrame(stDHS_Photo->ViChn, &stFrame);
    if ((HI_TRUE == bQuit) || (0 >= s32FrmCnt))
    {
        goto exit;
    }
    printf("get %d frame\n", s32FrmCnt);

     // make file name 
    strncpy(szPixFrm,
            (PIXEL_FORMAT_YUV_400 == stFrame.stVFrame.enPixelFormat) ? "p400" :
            ((PIXEL_FORMAT_YUV_SEMIPLANAR_420 == stFrame.stVFrame.enPixelFormat) ? "p420" : "p422" ), 9);
   

    for (j = 0; j < s32FrmCnt; j++)
    {
        if (HI_TRUE == bQuit)
        {
            break;
        }
        
        if (COMPRESS_MODE_NONE != astFrame[j].stVFrame.enCompressMode)
        {
            pstOutFrame = &stTask.stImgOut;
            memcpy(&stTask.stImgIn.stVFrame, &astFrame[j].stVFrame, sizeof(VIDEO_FRAME_S));
            stTask.stImgIn.u32PoolId = astFrame[j].u32PoolId;
            if (HI_SUCCESS != SAMPLE_MISC_GETVB(pstOutFrame, &astFrame[j], &VbBlk, hPool))
            {
                DHS_MPI_VI_ReleaseFrame(stDHS_Photo->ViChn, &astFrame[j]);
                astFrame[j].stVFrame.u64PhyAddr[0] = 0;
                if (VB_INVALID_HANDLE != VbBlk)
                {
                    HI_MPI_VB_ReleaseBlock(VbBlk);
                }
                continue;
            }

            s32Ret = HI_MPI_VGS_BeginJob(&hHandle);
            if (s32Ret != HI_SUCCESS)
            {
                printf("HI_MPI_VGS_BeginJob failed\n");
                HI_MPI_VB_ReleaseBlock(VbBlk);
                goto exit;
            }

            s32Ret =  HI_MPI_VGS_AddScaleTask(hHandle, &stTask);
            if (s32Ret != HI_SUCCESS)
            {
                printf("HI_MPI_VGS_AddScaleTask failed, %#x\n", s32Ret);
                HI_MPI_VGS_CancelJob(hHandle);
                HI_MPI_VB_ReleaseBlock(VbBlk);
                goto exit;
            }

            s32Ret = HI_MPI_VGS_EndJob(hHandle);
            if (s32Ret != HI_SUCCESS)
            {
                printf("HI_MPI_VGS_EndJob failed\n");
                HI_MPI_VGS_CancelJob(hHandle);
                HI_MPI_VB_ReleaseBlock(VbBlk);
                goto exit;
            }
            // save VI frame to file 
            s32Ret = vi_dump_save_one_frame(&pstOutFrame->stVFrame, sfd);
            HI_MPI_VB_ReleaseBlock(VbBlk);
        }
        else
        {
            s32Ret = vi_dump_save_one_frame(&astFrame[j].stVFrame, sfd);
        }

        // release frame after using 
        DHS_MPI_VI_ReleaseFrame(stDHS_Photo->ViChn, &astFrame[j]);
        astFrame[j].stVFrame.u64PhyAddr[0] = 0;
    }

exit:
    if (HI_SUCCESS != s32Ret)
    {
        if (remove(szYuvName))
        {
			printf("remove file:%s failed!\n", szYuvName);
		}
    }
    if (hPool != VB_INVALID_POOLID)
    {
        HI_MPI_VB_DestroyPool(hPool);
    }
    for (j= 0; j < s32FrmCnt; j++)
    {
        if (0 != astFrame[j].stVFrame.u64PhyAddr[0])
        {
            DHS_MPI_VI_ReleaseFrame(stDHS_Photo->ViChn, &astFrame[j]);
        }
    }
    DHS_MPI_VI_SetFrameDepth(stDHS_Photo->ViChn, u32OldDepth);

    if (HI_NULL != astFrame)
    {
        free(astFrame);
        astFrame = HI_NULL;
    }
    return s32Ret;

}

*/
HI_BOOL IsViVpssOnline(void)
{
    
    HI_U32 u32OnlineMode = 0;
    /*
    
    HI_S32 s32Ret = HI_SUCCESS;
    s32Ret = HI_MPI_SYS_GetViVpssMode(&u32OnlineMode);

    if (s32Ret != HI_SUCCESS)
    {
        printf("get vi-vpss online mode failed\n");
        return HI_TRUE;
    }
*/
    return (u32OnlineMode ? HI_TRUE : HI_FALSE);
}

int takePhotoOfOnline(int sfd){
 
   // return SAMPLE_MISC_ViDump(stDHS_Photo->ViChn, u32FrmCnt,sfd);
   DHS_Photo_ATTR_S  stDHS_Photo;
    stDHS_Photo.sfd=sfd;
    
    DHS_COMM_SYS_MemConfig();
    stDHS_Photo.ViChn=0;
    stDHS_Photo.u32Cnt=1;
   // DHL_SAMPLE_MISC_ViDump(&stDHS_Photo);
    DHL_MISC_ViDump_Liunx(&stDHS_Photo);
    
    //VIDEO_FRAME_S stFrmInfo;
   // memcpy(&stFrmInfo, &stDHS_Photo.stFrmInfo.stVFrame, sizeof(VIDEO_FRAME_S));
   // vi_dump_save_one_frame(&stFrmInfo, stDHS_Photo.sfd);
    return 0;
}
/*
static int HEXTOASCII(char *out_buffer,char *in_buffer,int in_len)
{
    int i2for=0;
    int Ourlen=0;
    char tempvuff[5];  
   
    for(i2for=0;i2for<in_len;i2for++)
    {
      sprintf(tempvuff,"%X",in_buffer[i2for]);
      strcat(out_buffer,tempvuff);
      Ourlen+=2;
    }
    return Ourlen;
}*/
int sendPhoto_len(void)
{
    return Send_buffer_get_len;
}

int sendPhoto(int sfd)
{
    int64_t i2for=0;
    //int64_t i3for=0;
    int64_t send_check=0;
    int64_t send_len_all=0;
    int len = 0;
    int send_len=0;

    printf("send(%d)  %lld\n",sfd,Send_buffer_get_len);
    send_len_all=Send_buffer_get_len;
    for(i2for=0;i2for<Send_buffer_get_len;)
    {
        if((send_len_all-i2for)>1024)
        {

            send_len=1024;
        }
        else
        {
            send_len=send_len_all-i2for;
        }

    /*    printf("%lld - ",i2for);
        for(i3for=i2for;i3for<i2for+10;i3for++)
        {
            printf("%02x ",Send_buffer[i3for]);
        }
            printf("\n");*/
        len = send(sfd, Send_buffer + i2for, send_len, 0);
        send_check+=send_len;
        if (len <= 0)
        {
            printf("send Erroe\n");
            return HI_FAILURE;;
        }
        i2for += len;
       // printf("i2for  %lld\n",i2for);

    }
    printf("send_check  %lld\n",send_check);
    return send_check;
}

int takePhotoOfFIFO(int sfd,int type)
{
 
	printf( "Do takePhotoOfFIFO\r\n");
    int shmid;
	void *shm = NULL;
	share_memory *shmdata;
	key_t key = ftok(SHM_NAME, 'k');
	shmid = shmget(key, sizeof(share_memory), 0666|IPC_CREAT);
	if(shmid == -1)
	{
		printf( "FIFO shmget failed. ");
		exit(-1);
	}
	shm = shmat(shmid, 0, 0);
	if(shm == (void *)-1)
	{
		printf( "FIFO shmat failed. ");
		exit(-1);
	}
	printf( "PhotoOfFIFO sharememory at 0x%X \r\n",(unsigned int) shm);
	shmdata = (share_memory *)shm;

    /*fifp*/
    const char *fifo_name = "/app/my_fifo";
    int pipe_fd = -1;
    //int data_fd = -1;
    int res = 0;
    int open_mode = O_RDONLY;
    char buffer[PIPE_BUF + 1];
    //int creat_buff_lean=PIPE_BUF*2+1024;
   // int64_t creat_buff_lean=PIPE_BUF*100;
    //char Head[1024]="{\"CMD\":\"PIC1\",\"DATA_LEN\":%d,\"DATA\":\"";
   // char END[1024]="}";
    int send_len=0;
    int send_ALL_len=0;
    int Bsend_ALL_len=0;
    int send_check=0;
    int bytes_read = 0;
    //char tempvuff[30];
    //int bytes_write = 0;


    memset(Send_buffer, 0, creat_buff_len);
    memset(buffer, '\0', sizeof(buffer));
        /******************/
    
    
            printf("Process %d opening FIFO O_RDONLY\n", getpid());      
            pipe_fd = open(fifo_name, open_mode);
            printf("Process %d result %d\n",getpid(), pipe_fd);
            
                if(pipe_fd != -1)
                {
                    int len = 0;
                    int pos = 0;

                    do
                    {
                        pos=0;
                        res = read(pipe_fd, buffer, PIPE_BUF);
                        if(Bsend_ALL_len==0)
                        {
                            Bsend_ALL_len=shmdata->POTO_len;
                            printf("shmdata->POTO_len %lld\n",shmdata->POTO_len);
                            shmdata->POTO_len=0;
                        }
                        //bytes_write = write(data_fd, buffer, res);
                        send_len=res;
                         bytes_read += res;
                        if(type==1)
                        {
                           // printf("have %d get %d\n",send_check,send_len);
                            for(int ui=0;ui<send_len;ui++)
                            {
                               Send_buffer[send_check++]= buffer[ui];
                            }

                          //  printf("send_check len %d\n",send_check);
                            
                        }
                        else
                        {

                            while (pos < send_len)
                            {
                                if((send_len-pos)>1024)
                                {
                                    len = send(sfd, buffer + pos, 1024, 0);
                                    send_check+=1024;
                                }
                                else
                                {
                                    len = send(sfd, buffer + pos, 1024, 0);
                                    send_check+=send_len-pos;
                                }
                                
                                if (len <= 0)
                                {
                                    printf("send Erroe\n");
                                    return HI_FAILURE;;
                                }
                                pos += len;
                            }
                        }
                    }while((res > 0)|(shmdata->read_sendflagend==0));

                 /* if(type==1)
                    {
                        memset(Send_buffer, 0, (creat_buff_lean));
                        if(bytes_read==0)
                        {
                            sprintf(tempvuff,Head,Bsend_ALL_len);
                            strcat(Send_buffer,tempvuff);
                        }
                        if(res>0)
                        {
                            HEXTOASCII(Send_buffer,buffer,res);
                        }     
                        if((res<PIPE_BUF)&&(shmdata->read_sendflagend>0))
                        {
                            // sprintf(tempvuff,END,Bsend_ALL_len);
                            strcat(Send_buffer,END);
                        }


                        //send_len=strlen(Send_buffer);

                        send_ALL_len=send_len;
                        printf("send_ALL_len %d\n",send_ALL_len);
                    }*/

                    printf("res %d , %d   ,send_ALL_len=%d\n",res,shmdata->read_sendflagend,send_ALL_len);
                    printf("Process %d , %d bytes read  %d\n", getpid(), bytes_read,shmdata->read_sendflagend);
                    printf("Total send  %d\n", send_check);
                    close(pipe_fd);
                   // close(data_fd);
                }
    
    if(type==1)
    {
        Send_buffer_get_len=send_check;
        //free(Send_buffer);
    }
    else
    {
        sleep(1);
    }
    
    if(shmdt(shm) == -1)
	{
		printf( "shmdt failed. ");
		exit(-1);
	}

	printf( "End takePhotoOfFIFO\r\n");
    return 0;
}

