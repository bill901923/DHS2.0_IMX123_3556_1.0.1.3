
/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : IMX123_forcar_cmos.c

  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/11/07
  Description   :
  History       :
  1.Date        : 2013/11/07
    Author      :
    Modification: Created file

******************************************************************************/
#if !defined(__IMX123_FORCAR_CMOS_H_)
#define __IMX123_FORCAR_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hi_comm_sns.h"
#include "hi_comm_video.h"
#include "hi_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"

#include "imx123_forcar_cmos_ex.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define IMX123_ID 335

#define IMX123_FORCAR_HIG_4BITS(x) (((x) & 0xF0000) >> 16)
#define IMX123_FORCAR_MID_8BITS(x) (((x) & 0x0FF00) >> 8)
#define IMX123_FORCAR_LOW_8BITS(x)  ((x) & 0x000ff)

#ifndef MAX
#define MAX(a, b) (((a) < (b)) ?  (b) : (a))
#endif

#ifndef MIN
#define MIN(a, b) (((a) > (b)) ?  (b) : (a))
#endif

ISP_SNS_STATE_S *g_pastImx123_forcar[ISP_MAX_PIPE_NUM] = {HI_NULL};

#define IMX123_FORCAR_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pastImx123_forcar[dev])
#define IMX123_FORCAR_SENSOR_SET_CTX(dev, pstCtx)   (g_pastImx123_forcar[dev] = pstCtx)
#define IMX123_FORCAR_SENSOR_RESET_CTX(dev)         (g_pastImx123_forcar[dev] = HI_NULL)

static HI_U32 g_au32InitExposure[ISP_MAX_PIPE_NUM] = { 0 };
static HI_U32 g_au32LinesPer500ms[ISP_MAX_PIPE_NUM] = { 0 };
static HI_U16 g_au16InitWBGain[ISP_MAX_PIPE_NUM][3] = { { 0 } };
static HI_U16 g_au16SampleRgain[ISP_MAX_PIPE_NUM] = { 0 };
static HI_U16 g_au16SampleBgain[ISP_MAX_PIPE_NUM] = { 0 };

ISP_SNS_COMMBUS_U g_aunImx123_forcar_BusInfo[ISP_MAX_PIPE_NUM] =
{
    [0] = {.s8I2cDev = 0 },
    [1] = {.s8I2cDev = 1 }
};

static HI_BOOL bFirstFps[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = 1};
typedef struct hiIMX123_FORCAR_STATE_S
{
	HI_U8       u8Hcg;
    HI_U32      u32BRL;
    HI_U32      u32RHS1_MAX;
    HI_U32      u32RHS2_MAX;
} IMX123_FORCAR_STATE_S;

IMX123_FORCAR_STATE_S g_astimx123_forcar_State[ISP_MAX_PIPE_NUM] = {{0}};


static HI_U32 gu32STimeFps = 30;
//static HI_U32 gu32LGain = 0;

/****************************************************************************
 * extern                                                                   *
 ****************************************************************************/
extern unsigned char imx123_forcar_i2c_addr;
extern unsigned int  imx123_forcar_addr_byte;
extern unsigned int  imx123_forcar_data_byte;

extern void IMX123_forcar_init(VI_PIPE ViPipe);
extern void IMX123_forcar_exit(VI_PIPE ViPipe);
extern void IMX123_forcar_standby(VI_PIPE ViPipe);
extern void IMX123_forcar_restart(VI_PIPE ViPipe);
extern int  IMX123_forcar_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  IMX123_forcar_read_register(VI_PIPE ViPipe, int addr);

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/
#define IMX123_FULL_LINES_MAX  (0xFFFF)

#define  IMX123_VMAX_ADDR_L             (0x3018)
#define  IMX123_VMAX_ADDR_M             (0x3019)
#define  IMX123_VMAX_ADDR_H             (0x301A)
//#define  IMX123_Y_OUT_SIZE_LOW          (0x3056)
//#define  IMX123_Y_OUT_SIZE_HIGH         (0x3057)
#define  IMX123_SHR0_LOW                 (0x301E)
#define  IMX123_SHR0_MIDDLE              (0x301F)
#define  IMX123_SHR0_HIGH               (0x3020)
#define  IMX123_SHR1_LOW                (0x3021)
#define  IMX123_SHR1_MIDDLE              (0x3022)
#define  IMX123_SHR1_HIGH                (0x3023)
#define  IMX123_RHS1_LOW                 (0x302E)
#define  IMX123_RHS1_MIDDLE              (0x302F)
#define  IMX123_RHS1_HIGH                (0x3030)
#define  IMX123_GAIN_LONG_LOW           (0x3014)
#define  IMX123_GAIN_LONG_HIGH          (0x3015)
//#define  IMX123_GAIN_SHORT_LOW          (0x30EA)
//#define  IMX123_GAIN_SHORT_HIGH         (0x30EB)

#define IMX123_INCREASE_LINES (1) /* make real fps less than stand fps because NVR require*/

#define IMX123_VMAX_1080P_60FPS_LINEAR  (1125+IMX123_INCREASE_LINES)    // 1080p60
#define IMX123_VMAX_QXGA_30FPS_LINEAR   (0x672+IMX123_INCREASE_LINES)   // 1650 QXGA 30fps
#define IMX123_VMAX_QXGA_60FPS_LINEAR   (0x708+IMX123_INCREASE_LINES)   // 1536p60 ;Frame switching WDR

#define IMX123_forcar_1080P_60FPS_MODE   (0)
#define IMX123_forcar_1080P_30FPS_MODE   (1)
#define IMX123_forcar_QXGA_30FPS_MODE    (2)

#define IMX123_RES_IS_5M_12BIT_LINEAR(w, h)    (((w) == 2048) && ((h) == 1536))
#define IMX123_RES_IS_1080P_12BIT_LINEAR(w, h)    (((w) == 1920) && ((h) == 1080))

//static HI_U32 gu32RHS1_LocRLimit = (2* IMX123_VMAX_5M_30FPS_12BIT_WDR - 2*1564 - 11) - (((2* IMX123_VMAX_5M_30FPS_12BIT_WDR - 2* 1564 - 11) - 10 ) %4) ;
//sensor gain


#define IMX123_FRAME_TYPE_SHORT      1
#define IMX123_FRAME_TYPE_LONG       0

static HI_U32 gu32RHS1 = 0xE6;
static HI_BOOL bInit = HI_FALSE;
HI_BOOL bSensorInit = HI_FALSE; 
static ISP_FSWDR_MODE_E genFSWDRMode = ISP_FSWDR_NORMAL_MODE;
static HI_U32 gu32MaxTimeGetCnt = 0;



/* AE default parameter and function */

static HI_S32 cmos_get_ae_default(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{

     printf("cmos_get_ae_default \r\n");
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }

    HI_U32 u32Fll = 0;
    HI_U32 U32MaxFps = 0;

    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER(pstAeSnsDft);
    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    ISP_TRACE(HI_DBG_ERR, "pstSnsState->u8ImgMode: %d\n", pstSnsState->u8ImgMode);
    if (IMX123_forcar_1080P_60FPS_MODE == pstSnsState->u8ImgMode)
    {
        u32Fll = IMX123_VMAX_1080P_60FPS_LINEAR;
        U32MaxFps = 60;
    }
    else if (IMX123_forcar_QXGA_30FPS_MODE == pstSnsState->u8ImgMode)
    {
        u32Fll = IMX123_VMAX_QXGA_30FPS_LINEAR;
        U32MaxFps = 60;
    }
    else
    {
        u32Fll = IMX123_forcar_1080P_30FPS_MODE;
        U32MaxFps = 30;
    }


    printf("IMX123_INCREASE_LINES %d\n", IMX123_INCREASE_LINES);
    printf("u32Fll %d\n",u32Fll);
    printf("U32MaxFps %d\n", U32MaxFps);

    pstSnsState->u32FLStd = u32Fll;


    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FlickerFreq = 0;
    pstAeSnsDft->u32FullLinesMax = IMX123_FULL_LINES_MAX;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
    pstAeSnsDft->stIntTimeAccu.f32Offset = 0;

    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.1;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 1;
    
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 8 << pstAeSnsDft->u32ISPDgainShift;

    pstAeSnsDft->u32MaxAgain = 1446438;
    pstAeSnsDft->u32MinAgain = 1024;
    pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
    pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

    pstAeSnsDft->u32MaxDgain = 1;  /* if Dgain enable,please set ispdgain bigger than 1*/
    pstAeSnsDft->u32MinDgain = 1;
    pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
    pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;

    memcpy(&pstAeSnsDft->stPirisAttr, &gstPirisAttr, sizeof(ISP_PIRIS_ATTR_S));
    pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_4;
    pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_5_6;

    pstAeSnsDft->bAERouteExValid = HI_FALSE;
    pstAeSnsDft->stAERouteAttr.u32TotalNum = 0;
    pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;
    
    switch(pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:   // linear mode
        {
            printf("WDR_MODE_NONE\n");
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;

            pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 4;
            pstAeSnsDft->u32MinIntTime = 3;
            pstAeSnsDft->u32MaxIntTimeTarget = 0xffff;
            pstAeSnsDft->u32MinIntTimeTarget = pstAeSnsDft->u32MinIntTime;

            pstAeSnsDft->u8AeCompensation = 0x38;
            pstAeSnsDft->enAeExpMode = AE_EXP_HIGHLIGHT_PRIOR;

	    pstAeSnsDft->u32InitExposure = g_au32InitExposure[ViPipe] ? g_au32InitExposure[ViPipe] : 148859;
        }
        break;
    }
    
    return HI_SUCCESS;
}



/* the function of sensor set fps */
static HI_VOID cmos_fps_set(VI_PIPE ViPipe, HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{

    HI_U32 u32MaxFps;
    HI_U32 u32Lines;

    ISP_SNS_STATE_S *pstSnsState = HI_NULL;
     printf("cmos_fps_set \r\n");

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    switch (pstSnsState->u8ImgMode)
    {
        case IMX123_forcar_QXGA_30FPS_MODE:
            if ((f32Fps <= 30) && (f32Fps >= 2))
            {
                u32MaxFps = 30;
                
                    u32Lines  = (IMX123_VMAX_QXGA_30FPS_LINEAR * u32MaxFps) / f32Fps;
                    u32Lines  = (pstSnsState->u32FLStd  > IMX123_FULL_LINES_MAX) ? IMX123_FULL_LINES_MAX : u32Lines ;
                    pstSnsState->u32FLStd = u32Lines ;
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }
            break;
        
        case IMX123_forcar_1080P_60FPS_MODE:
            if ((f32Fps <= 60) && (f32Fps >= 0.5))
            {
                u32MaxFps = 60;
                u32Lines  = (IMX123_VMAX_1080P_60FPS_LINEAR * u32MaxFps) / DIV_0_TO_1_FLOAT(f32Fps);
                u32Lines  = (u32Lines  > IMX123_FULL_LINES_MAX) ? IMX123_FULL_LINES_MAX : u32Lines ;
                pstSnsState->u32FLStd = u32Lines ;
            }
            else
            {
                printf("Not support Fps: %f\n", f32Fps);
                return;
            }
            break;
        
        default:
            ISP_TRACE(HI_DBG_ERR, "Not support this Mode!!!\n");
            return;
            break;
    }

    /* SHR 16bit, So limit full_lines as 0xFFFF */
    if (f32Fps > u32MaxFps)
    {
        ISP_TRACE(HI_DBG_ERR, "Not support Fps: %f\n", f32Fps);
        return;
    }

    if (u32Lines > IMX123_FULL_LINES_MAX)
    {
        u32Lines = IMX123_FULL_LINES_MAX;
    }
 	
    pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = IMX123_FORCAR_LOW_8BITS(pstSnsState->au32FL[0]);
    pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = IMX123_FORCAR_MID_8BITS(pstSnsState->au32FL[0]);
    pstSnsState->astRegsInfo[0].astI2cData[7].u32Data = IMX123_FORCAR_HIG_4BITS(pstSnsState->au32FL[0]);

  
    pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 4;
    
    pstAeSnsDft->f32Fps = f32Fps;
    gu32STimeFps = (HI_U32)f32Fps;
    pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * f32Fps / 2;
    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];
    pstAeSnsDft->u32HmaxTimes = (1000000) / (pstSnsState->u32FLStd * DIV_0_TO_1_FLOAT(f32Fps));

    if (bFirstFps[ViPipe])
    {
        bFirstFps[ViPipe] = HI_FALSE;
        pstSnsState->au32FL[1] = pstSnsState->au32FL[0];
    }
    return;
}

// not support slow frameRate in WDR mode
static HI_VOID cmos_slow_framerate_set(VI_PIPE ViPipe, HI_U32 u32FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{

     printf("cmos_slow_framerate_set \r\n");
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    
    u32FullLines = (u32FullLines > IMX123_FULL_LINES_MAX) ? IMX123_FULL_LINES_MAX : u32FullLines;
    pstSnsState->au32FL[0] = (u32FullLines >> 1) << 1;
    pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];
    printf("cmos_slow_framerate_set pstSnsState->au32FL[0] %d \r\n",pstSnsState->au32FL[0]);
    
 		
    pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = IMX123_FORCAR_LOW_8BITS(u32FullLines);   //VMAX
    pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = IMX123_FORCAR_MID_8BITS(u32FullLines);
    pstSnsState->astRegsInfo[0].astI2cData[7].u32Data = IMX123_FORCAR_HIG_4BITS(u32FullLines);
   
    pstAeSnsDft->u32MaxIntTime = pstSnsState->au32FL[0] - 4;
  

    return;
}


/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(VI_PIPE ViPipe, HI_U32 u32IntTime)
{
    static HI_BOOL bFirst = HI_TRUE;
   //  printf("cmos_inttime_update \r\n");
    //static HI_U32 u32LastShortIntTime = 0;
    HI_U32 u32Value = 0;

    static HI_U32 u32ShortIntTime;
    static HI_U32 u32LongIntTime;

    HI_U32 u32SHS1;
    HI_U32 u32SHS2;
    //static HI_U32 u32RHS1Pre = 0;

    //HI_U32 u32delta = 0;
    //HI_U16 u16ShortExpMod4 = 0;
    //HI_U16 u16ShortExpMod8 = 0;
    //HI_U32 LinearExpTime = 0;
    //HI_U32 SGain = 0;
    //HI_U32 SGainDb = 0;
    //HI_U32 i = 0;
    //HI_U32 u32RHS1Limit = 0;

    ISP_SNS_STATE_S *pstSnsState = HI_NULL;
    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

     
    u32Value = pstSnsState->au32FL[0] - u32IntTime;
    pstSnsState->astRegsInfo[0].astI2cData[0].u32Data = IMX123_FORCAR_LOW_8BITS(u32Value);   //SHR0
    pstSnsState->astRegsInfo[0].astI2cData[1].u32Data = IMX123_FORCAR_MID_8BITS(u32Value);
    pstSnsState->astRegsInfo[0].astI2cData[2].u32Data = IMX123_FORCAR_HIG_4BITS(u32Value);
    bFirst = HI_TRUE;

    return;
}

/*Again and Dgain use the same table, Step is 0.3db*/
static HI_U32 ad_gain_table[631] =
{
   1024,   1036,   1048,   1060,   1072,   1085,   1097,   1110,   1123,   1136,   1149,   1162,   1176,   1189,
   1203,   1217,   1231,   1245,   1260,   1274,   1289,   1304,   1319,   1334,   1350,   1366,   1381,   1397,
   1414,   1430,   1446,   1463,   1480,   1497,   1515,   1532,   1550,   1568,   1586,   1604,   1623,   1642,
   1661,   1680,   1699,   1719,   1739,   1759,   1780,   1800,   1821,   1842,   1863,   1885,   1907,   1929,
   1951,   1974,   1997,   2020,   2043,   2067,   2091,   2115,   2139,   2164,   2189,   2215,   2240,   2266,
   2292,   2319,   2346,   2373,   2400,   2428,   2456,   2485,   2514,   2543,   2572,   2602,   2632,   2663,
   2693,   2725,   2756,   2788,   2820,   2853,   2886,   2919,   2953,   2987,   3022,   3057,   3092,   3128,
   3164,   3201,   3238,   3276,   3314,   3352,   3391,   3430,   3470,   3510,   3551,   3592,   3633,   3675,
   3718,   3761,   3805,   3849,   3893,   3938,   3984,   4030,   4077,   4124,   4172,   4220,   4269,   4318,
   4368,   4419,   4470,   4522,   4574,   4627,   4681,   4735,   4790,   4845,   4901,   4958,   5015,   5073,
   5132,   5192,   5252,   5313,   5374,   5436,   5499,   5563,   5627,   5692,   5758,   5825,   5893,   5961,
   6030,   6100,   6170,   6242,   6314,   6387,   6461,   6536,   6611,   6688,   6766,   6844,   6923,   7003,
   7084,   7166,   7249,   7333,   7418,   7504,   7591,   7679,   7768,   7858,   7949,   8041,   8134,   8228,
   8323,   8420,   8517,   8616,   8716,   8817,   8919,   9022,   9126,   9232,   9339,   9447,   9557,   9667,
   9779,   9892,  10007,  10123,  10240,  10359,  10479,  10600,  10723,  10847,  10972,  11099,  11228,  11358,
  11489,  11623,  11757,  11893,  12031,  12170,  12311,  12454,  12598,  12744,  12891,  13041,  13192,  13344,
  13499,  13655,  13813,  13973,  14135,  14299,  14464,  14632,  14801,  14973,  15146,  15321,  15499,  15678,
  15860,  16044,  16229,  16417,  16607,  16800,  16994,  17191,  17390,  17591,  17795,  18001,  18210,  18420,
  18634,  18850,  19068,  19289,  19512,  19738,  19966,  20198,  20431,  20668,  20907,  21149,  21394,  21642,
  21893,  22146,  22403,  22662,  22925,  23190,  23458,  23730,  24005,  24283,  24564,  24848,  25136,  25427,
  25722,  26020,  26321,  26626,  26934,  27246,  27561,  27880,  28203,  28530,  28860,  29194,  29532,  29874,
  30220,  30570,  30924,  31282,  31645,  32011,  32382,  32757,  33136,  33520,  33908,  34300,  34698,  35099,
  35506,  35917,  36333,  36754,  37179,  37610,  38045,  38486,  38931,  39382,  39838,  40300,  40766,  41238,
  41716,  42199,  42687,  43182,  43682,  44188,  44699,  45217,  45740,  46270,  46806,  47348,  47896,  48451,
  49012,  49579,  50153,  50734,  51322,  51916,  52517,  53125,  53740,  54363,  54992,  55629,  56273,  56925,
  57584,  58251,  58925,  59607,  60298,  60996,  61702,  62417,  63139,  63870,  64610,  65358,  66115,  66881,
  67655,  68438,  69231,  70033,  70843,  71664,  72494,  73333,  74182,  75041,  75910,  76789,  77678,  78578,
  79488,  80408,  81339,  82281,  83234,  84198,  85173,  86159,  87157,  88166,  89187,  90219,  91264,  92321,
  93390,  94471,  95565,  96672,  97791,  98924, 100069, 101228, 102400, 103586, 104785, 105999, 107226, 108468,
 109724, 110994, 112279, 113580, 114895, 116225, 117571, 118932, 120310, 121703, 123112, 124537, 125980, 127438,
 128914, 130407, 131917, 133444, 134989, 136553, 138134, 139733, 141351, 142988, 144644, 146319, 148013, 149727,
 151461, 153215, 154989, 156783, 158599, 160435, 162293, 164172, 166073, 167996, 169942, 171910, 173900, 175914,
 177951, 180011, 182096, 184204, 186337, 188495, 190678, 192886, 195119, 197379, 199664, 201976, 204315, 206681,
 209074, 211495, 213944, 216421, 218927, 221462, 224027, 226621, 229245, 231900, 234585, 237301, 240049, 242829,
 245640, 248485, 251362, 254273, 257217, 260196, 263209, 266256, 269339, 272458, 275613, 278805, 282033, 285299,
 288602, 291944, 295325, 298745, 302204, 305703, 309243, 312824, 316446, 320111, 323817, 327567, 331360, 335197,
 339078, 343005, 346976, 350994, 355059, 359170, 363329, 367536, 371792, 376097, 380452, 384858, 389314, 393822,
 398382, 402995, 407662, 412382, 417157, 421988, 426874, 431817, 436817, 441876, 446992, 452168, 457404, 462700,
 468058, 473478, 478961, 484507, 490117, 495793, 501534, 507341, 513216, 519158, 525170, 531251, 537403, 543626,
 549921, 556288, 562730, 569246, 575838, 582505, 589250, 596074, 602976, 609958, 617021, 624166, 631393, 638704,
 646100, 653582, 661150, 668806, 676550, 684384, 692309, 700326, 708435, 716638, 724936, 733331, 741822, 750412,
 759102, 767892, 776783, 785778, 794877, 804081, 813392, 822811, 832338, 841976, 851726, 861589, 871565, 881658,
 891867, 902194, 912641, 923209, 933899, 944713, 955652, 966718, 977912, 989236,1000691,1012278,1024000,1035857,
1047852,1059986,1072260,1084676,1097236,1109941,1122794,1135795,1148947,1162251,1175709,1189323,1203095,1217026,
1231119,1245374,1259795,1274383,1289140,1304067,1319168,1334443,1349895,1365526,1381338,1397333,1413513,1429881,
1446438

};


static HI_VOID cmos_again_calc_table(VI_PIPE ViPipe, HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
     	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);
    int i;
    HI_U32 u32GainTableMax;

        u32GainTableMax = 630;
    

    CMOS_CHECK_POINTER_VOID(pu32AgainLin);
    CMOS_CHECK_POINTER_VOID(pu32AgainDb);

    if (*pu32AgainLin >= ad_gain_table[u32GainTableMax])
    {
        *pu32AgainLin = ad_gain_table[u32GainTableMax];
        *pu32AgainDb = u32GainTableMax;
         return ;
    }
    
    for (i = 1; i <= u32GainTableMax; i++)
        {
            if (*pu32AgainLin < ad_gain_table[i])
            {
                *pu32AgainLin = ad_gain_table[i - 1];
                *pu32AgainDb = i - 1;
                break;
            }
        }
    return;
}

static HI_VOID cmos_gains_update(VI_PIPE ViPipe, HI_U32 u32Again, HI_U32 u32Dgain)
{
  //   printf("cmos_gains_update \r\n");

	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

     pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = IMX123_FORCAR_LOW_8BITS(u32Again);
     pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = IMX123_FORCAR_MID_8BITS(u32Again);

    return;
}


/*
    Linear:    SHR0 [9, VMAX-1] ;    Initime = VMAX - SHR0  Initime [1,VMAX -9]
    WDR2t1:    SHS1 4n+2 (n=0,1,2....)  and 18 <= SHS1 <= RHS1-4
               RHS1 8n+2 (n=0,1,2....)  and (SHR1+5)<= RHS1<=(SHR0-18) and RHS1<(BRL*2)
               SHR0 4n(n=0,1,2.....) and (RHS1+18) <= SHR0<=(2*VMAX-4)  VMAX :2n (n=0,1,2.....)
*/

//static HI_VOID cmos_get_inttime_max(VI_PIPE ViPipe, HI_U32 u32Ratio, HI_U32 *pu32IntTimeMax)
static HI_VOID cmos_get_inttime_max(VI_PIPE ViPipe, HI_U16 u16ManRatioEnable, HI_U32 *au32Ratio, HI_U32 *au32IntTimeMax, HI_U32 *au32IntTimeMin, HI_U32 *pu32LFMaxIntTime)
{
    printf("cmos_get_inttime_max \r\n");
    HI_U32  u32ShortIntTimeMax=0;
    HI_U32 u32IntTimeMaxTmp = 0;
   // HI_U32 u32RatioTmp = 0x40;
    //HI_U32 u32ShortTimeMinLimit = 25;
   // HI_U32 u32delta;
    //HI_U32 u32BRLCalTemp = 0;
    //static HI_U32 u32LastIntTimeMaxTmp = 25;
    //printf("Ratio = %d\n", au32Ratio[0]);

    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER_VOID(au32Ratio);
    CMOS_CHECK_POINTER_VOID(au32IntTimeMax);
    CMOS_CHECK_POINTER_VOID(au32IntTimeMin);
    CMOS_CHECK_POINTER_VOID(pu32LFMaxIntTime);
    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);


    return;
}


/* Only used in FSWDR mode */
static HI_VOID cmos_ae_fswdr_attr_set(VI_PIPE ViPipe, AE_FSWDR_ATTR_S *pstAeFSWDRAttr)
{
     printf("cmos_ae_fswdr_attr_set \r\n");
    CMOS_CHECK_POINTER_VOID(pstAeFSWDRAttr);
    genFSWDRMode      = pstAeFSWDRAttr->enFSWDRMode;
    gu32MaxTimeGetCnt = 0;
}


static HI_S32 cmos_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
     printf("cmos_init_ae_exp_function \r\n");
    CMOS_CHECK_POINTER(pstExpFuncs);

    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
    pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
    pstExpFuncs->pfn_cmos_slow_framerate_set= cmos_slow_framerate_set;
    pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
    pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
    pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
    pstExpFuncs->pfn_cmos_dgain_calc_table  = NULL;
    pstExpFuncs->pfn_cmos_get_inttime_max   = cmos_get_inttime_max;
    pstExpFuncs->pfn_cmos_ae_fswdr_attr_set = cmos_ae_fswdr_attr_set;

    printf("Manaul ExpRatio is too large!\n");
    return HI_SUCCESS;
}




/* Rgain and Bgain of the golden sample */
#define GOLDEN_RGAIN 0
#define GOLDEN_BGAIN 0

static HI_S32 cmos_get_awb_default(VI_PIPE ViPipe, AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    printf("cmos_get_awb_default \r\n");
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER(pstAwbSnsDft);
    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = 4850;

    pstAwbSnsDft->au16GainOffset[0] = 0x1C2;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x1F3;

    pstAwbSnsDft->as32WbPara[0] = 80;
    pstAwbSnsDft->as32WbPara[1] = 5;
    pstAwbSnsDft->as32WbPara[2] = -171;
    pstAwbSnsDft->as32WbPara[3] = 181935;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -136460;

    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));

    switch (pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:
            memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));
            break;

    }

    pstAwbSnsDft->u16SampleRgain = g_au16SampleRgain[ViPipe];
    pstAwbSnsDft->u16SampleBgain = g_au16SampleBgain[ViPipe];
    pstAwbSnsDft->u16InitRgain = g_au16InitWBGain[ViPipe][0];
    pstAwbSnsDft->u16InitGgain = g_au16InitWBGain[ViPipe][1];
    pstAwbSnsDft->u16InitBgain = g_au16InitWBGain[ViPipe][2];

    return HI_SUCCESS;
}

static HI_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
     printf("cmos_init_awb_exp_function \r\n");
    CMOS_CHECK_POINTER(pstExpFuncs);

    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;

    return HI_SUCCESS;
}


static ISP_CMOS_DNG_COLORPARAM_S g_stDngColorParam =
{
    {378, 256, 430},
    {439, 256, 439}
};

static HI_S32 cmos_get_isp_default(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
     printf("cmos_get_isp_default \r\n");
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER(pstDef);
    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
#ifdef CONFIG_HI_ISP_CA_SUPPORT
    pstDef->unKey.bit1Ca       = 1;
    pstDef->pstCa              = &g_stIspCA;
#endif
    pstDef->unKey.bit1Clut     = 1;
    pstDef->pstClut            = &g_stIspCLUT;

    pstDef->unKey.bit1Dpc      = 1;
    pstDef->pstDpc             = &g_stCmosDpc;

    pstDef->unKey.bit1Wdr      = 1;
    pstDef->pstWdr             = &g_stIspWDR;
#ifdef CONFIG_HI_ISP_HLC_SUPPORT
    pstDef->unKey.bit1Hlc      = 0;
    pstDef->pstHlc             = &g_stIspHlc;
#endif
    pstDef->unKey.bit1Lsc      = 1;
    pstDef->pstLsc             = &g_stCmosLsc;
#ifdef CONFIG_HI_ISP_EDGEMARK_SUPPORT
    pstDef->unKey.bit1EdgeMark = 0;
    pstDef->pstEdgeMark        = &g_stIspEdgeMark;
#endif
#ifdef CONFIG_HI_ISP_CR_SUPPORT
    pstDef->unKey.bit1Ge       = 1;
    pstDef->pstGe              = &g_stIspGe;
#endif
    pstDef->unKey.bit1Detail   = 1;
    pstDef->pstDetail          = &g_stIspDetail;

    switch (pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:
            pstDef->unKey.bit1Demosaic       = 1;
            pstDef->pstDemosaic              = &g_stIspDemosaic;
            pstDef->unKey.bit1Sharpen        = 1;
            pstDef->pstSharpen               = &g_stIspYuvSharpen;
            pstDef->unKey.bit1Drc            = 1;
            pstDef->pstDrc                   = &g_stIspDRC;
            pstDef->unKey.bit1Gamma          = 1;
            pstDef->pstGamma                 = &g_stIspGamma;
            pstDef->unKey.bit1BayerNr        = 1;
            pstDef->pstBayerNr               = &g_stIspBayerNr;
            pstDef->unKey.bit1AntiFalseColor = 1;
            pstDef->pstAntiFalseColor        = &g_stIspAntiFalseColor;
            pstDef->unKey.bit1Ldci           = 1;
            pstDef->pstLdci                  = &g_stIspLdci;
            pstDef->unKey.bit1Dehaze         = 1;
            pstDef->pstDehaze                = &g_stIspDehaze;
            memcpy(&pstDef->stNoiseCalibration, &g_stIspNoiseCalibration, sizeof(ISP_CMOS_NOISE_CALIBRATION_S));
            break;


    }


    memcpy(&pstDef->stDngColorParam, &g_stDngColorParam, sizeof(ISP_CMOS_DNG_COLORPARAM_S));

    printf("cmos_get_isp_default!  pstSnsState->u8ImgMode  %d\n",pstSnsState->u8ImgMode);
    switch (pstSnsState->u8ImgMode)
    {
        default:
        case IMX123_forcar_1080P_60FPS_MODE:
            printf("IMX123_forcar_1080P_60FPS_MODE \n");
            pstDef->stSensorMode.stDngRawFormat.u8BitsPerSample = 12;
            pstDef->stSensorMode.stDngRawFormat.u32WhiteLevel = 1920;
            break;
        case IMX123_forcar_1080P_30FPS_MODE:

            printf("IMX123_forcar_1080P_30FPS_MODE \n");
            pstDef->stSensorMode.stDngRawFormat.u8BitsPerSample = 12;
            pstDef->stSensorMode.stDngRawFormat.u32WhiteLevel = 1920;
            break;
        case IMX123_forcar_QXGA_30FPS_MODE:

            printf("IMX123_forcar_QXGA_30FPS_MODE \n");
            pstDef->stSensorMode.stDngRawFormat.u8BitsPerSample = 12;
            pstDef->stSensorMode.stDngRawFormat.u32WhiteLevel = 2048;
            break;

    }

    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleH.u32Denominator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleH.u32Numerator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleV.u32Denominator = 1;
    pstDef->stSensorMode.stDngRawFormat.stDefaultScale.stDefaultScaleV.u32Numerator = 1;
    pstDef->stSensorMode.stDngRawFormat.stCfaRepeatPatternDim.u16RepeatPatternDimRows = 2;
    pstDef->stSensorMode.stDngRawFormat.stCfaRepeatPatternDim.u16RepeatPatternDimCols = 2;
    pstDef->stSensorMode.stDngRawFormat.stBlcRepeatDim.u16BlcRepeatRows = 2;
    pstDef->stSensorMode.stDngRawFormat.stBlcRepeatDim.u16BlcRepeatCols = 2;
    pstDef->stSensorMode.stDngRawFormat.enCfaLayout = CFALAYOUT_TYPE_RECTANGULAR;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[0] = 0;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[1] = 1;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPlaneColor[2] = 2;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[0] = 0;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[1] = 1;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[2] = 1;
    pstDef->stSensorMode.stDngRawFormat.au8CfaPattern[3] = 2;
    pstDef->stSensorMode.bValidDngRawFormat = HI_TRUE;

  // pstDef->stSensorMaxResolution.u32MaxWidth  = 2592;
  //  pstDef->stSensorMaxResolution.u32MaxHeight = 1520;
    pstDef->stSensorMode.u32SensorID = IMX123_ID;
    pstDef->stSensorMode.u8SensorMode = pstSnsState->u8ImgMode;
    return HI_SUCCESS;
}


static HI_S32 cmos_get_isp_black_level(VI_PIPE ViPipe, ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel)
{
     printf("cmos_get_isp_black_level \r\n");
    HI_S32  i;

    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    if (HI_NULL == pstBlackLevel)
    {
        printf("null pointer when get isp black level value!\n");
        return -1;
    }

    /* Don't need to update black level when iso change */
    pstBlackLevel->bUpdate = HI_FALSE;

          
        for (i=0; i<4; i++)
        {
            pstBlackLevel->au16BlackLevel[i] = 0xEF;
        }

    return HI_SUCCESS;    
}

static HI_VOID cmos_set_pixel_detect(VI_PIPE ViPipe, HI_BOOL bEnable)
{
     printf("cmos_set_pixel_detect \r\n");
    HI_U32 u32FullLines_5Fps;
    HI_U32 u32MaxIntTime_5Fps = 0;

    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    if (WDR_MODE_2To1_LINE == pstSnsState->enWDRMode)
    {   
        return;
    }
    else
    {
        if (IMX123_forcar_1080P_60FPS_MODE == pstSnsState->u8ImgMode)
        {
            u32FullLines_5Fps = (IMX123_VMAX_1080P_60FPS_LINEAR * 60) / 5;
        }
        else  if (IMX123_forcar_QXGA_30FPS_MODE == pstSnsState->u8ImgMode)
        {       
            u32FullLines_5Fps = (IMX123_VMAX_QXGA_30FPS_LINEAR* 30) / 5;
        }
        else
        {
            return;
        }
    }

    printf("u32FullLines_5Fps %d\n",u32FullLines_5Fps);
    u32FullLines_5Fps = (u32FullLines_5Fps > IMX123_FULL_LINES_MAX) ? IMX123_FULL_LINES_MAX : u32FullLines_5Fps;
    printf("u32FullLines_5Fps %d\n",u32FullLines_5Fps);

    u32MaxIntTime_5Fps = u32FullLines_5Fps - 8;
    printf("u32MaxIntTime_5Fps %d\n",u32MaxIntTime_5Fps);
    printf("bEnable %d\n",bEnable);

    if (bEnable) /* setup for ISP pixel calibration mode */
    {
        IMX123_forcar_write_register(ViPipe, IMX123_GAIN_LONG_LOW, 0x00); //gain
        IMX123_forcar_write_register(ViPipe, IMX123_GAIN_LONG_HIGH, 0x00);

        IMX123_forcar_write_register(ViPipe, IMX123_VMAX_ADDR_L, IMX123_FORCAR_LOW_8BITS(u32FullLines_5Fps));    //VMAX
        IMX123_forcar_write_register(ViPipe, IMX123_VMAX_ADDR_M, IMX123_FORCAR_MID_8BITS(u32FullLines_5Fps));
        IMX123_forcar_write_register(ViPipe, IMX123_VMAX_ADDR_H, IMX123_FORCAR_HIG_4BITS(u32FullLines_5Fps));

        IMX123_forcar_write_register(ViPipe, IMX123_SHR0_LOW, 0x4);
        IMX123_forcar_write_register(ViPipe, IMX123_SHR0_MIDDLE, 0x0);
        IMX123_forcar_write_register(ViPipe, IMX123_SHR0_HIGH, 0x0);
    }
    else /* setup for ISP 'normal mode' */
    {
        pstSnsState->u32FLStd = (pstSnsState->u32FLStd > IMX123_FULL_LINES_MAX) ? IMX123_FULL_LINES_MAX : pstSnsState->u32FLStd;
        pstSnsState->au32FL[0] = pstSnsState->u32FLStd ;
        IMX123_forcar_write_register (ViPipe, IMX123_VMAX_ADDR_L, IMX123_FORCAR_LOW_8BITS(pstSnsState->au32FL[0]));
        IMX123_forcar_write_register (ViPipe, IMX123_VMAX_ADDR_M, IMX123_FORCAR_MID_8BITS(pstSnsState->au32FL[0]));
        IMX123_forcar_write_register (ViPipe, IMX123_VMAX_ADDR_H, IMX123_FORCAR_HIG_4BITS(pstSnsState->au32FL[0]));
        pstSnsState->bSyncInit = HI_FALSE ;
        bInit = HI_FALSE;
    }

    return;
}


static HI_S32 cmos_set_wdr_mode(VI_PIPE ViPipe, HI_U8 u8Mode)
{
     printf("cmos_set_wdr_mode \r\n");
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    pstSnsState->bSyncInit = HI_FALSE;

    switch (u8Mode& 0x3F)
    {
        case WDR_MODE_NONE:
            pstSnsState->enWDRMode = WDR_MODE_NONE;
        printf("cmos_set_wdr_mode WDR_MODE_NONE \n");
            
            if(IMX123_forcar_QXGA_30FPS_MODE == pstSnsState->u8ImgMode)
        {
                pstSnsState->u32FLStd  = IMX123_VMAX_QXGA_30FPS_LINEAR;
        }
            else if(IMX123_forcar_1080P_60FPS_MODE== pstSnsState->u8ImgMode)
            {
                pstSnsState->u32FLStd  = IMX123_VMAX_1080P_60FPS_LINEAR;
            }
            else
            {
            
        }
            printf("linear mode\n");
            break;
        
        default:
            printf("NOT support this mode!\n");
            return HI_FAILURE;
        break;
    }


   /// pstSnsState->u8ImgMode = u8SensorImageMode;
    pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
    pstSnsState->au32FL[1] = pstSnsState->au32FL[0];
    memset(pstSnsState->au32WDRIntTime, 0, sizeof(pstSnsState->au32WDRIntTime));
  //  printf(" pstSnsState->u8ImgMode =%d\n", u8SensorImageMode);
  //  printf("pstSnsState->au32FL[0] =%d\n",pstSnsState->u32FLStd);
  //  printf(" pstSnsState->au32FL[1]=%d \n", pstSnsState->au32FL[0]);

    return HI_SUCCESS;
}

static HI_S32 cmos_get_sns_regs_info(VI_PIPE ViPipe, ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    // printf("cmos_get_sns_regs_info \r\n");
    HI_S32 i;
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER(pstSnsRegsInfo);
    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    if ((HI_FALSE == pstSnsState->bSyncInit) || (HI_FALSE == pstSnsRegsInfo->bConfig))
    {
        pstSnsState->astRegsInfo[0].enSnsType = ISP_SNS_I2C_TYPE;
        pstSnsState->astRegsInfo[0].unComBus.s8I2cDev = g_aunImx123_forcar_BusInfo[ViPipe].s8I2cDev;
        pstSnsState->astRegsInfo[0].u8Cfg2ValidDelayMax = 2;
        pstSnsState->astRegsInfo[0].u32RegNum = 8;


        
         
        for (i = 0; i < pstSnsState->astRegsInfo[0].u32RegNum; i++)
        {    
            pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = HI_TRUE;
            pstSnsState->astRegsInfo[0].astI2cData[i].u8DevAddr = imx123_forcar_i2c_addr;
            pstSnsState->astRegsInfo[0].astI2cData[i].u32AddrByteNum = imx123_forcar_addr_byte;
            pstSnsState->astRegsInfo[0].astI2cData[i].u32DataByteNum = imx123_forcar_data_byte;
        }

        //shutter related
        pstSnsState->astRegsInfo[0].astI2cData[0].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[0].u32RegAddr = IMX123_SHR0_LOW;        //SHR0
        pstSnsState->astRegsInfo[0].astI2cData[1].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[1].u32RegAddr = IMX123_SHR0_MIDDLE;
        pstSnsState->astRegsInfo[0].astI2cData[2].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[2].u32RegAddr = IMX123_SHR0_HIGH;

        pstSnsState->astRegsInfo[0].astI2cData[3].u8DelayFrmNum = 0;        //Long Gain
        pstSnsState->astRegsInfo[0].astI2cData[3].u32RegAddr = IMX123_GAIN_LONG_LOW;
        pstSnsState->astRegsInfo[0].astI2cData[4].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[4].u32RegAddr = IMX123_GAIN_LONG_HIGH;

        //Vmax
        pstSnsState->astRegsInfo[0].astI2cData[5].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[5].u32RegAddr = IMX123_VMAX_ADDR_L;
        pstSnsState->astRegsInfo[0].astI2cData[6].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[6].u32RegAddr = IMX123_VMAX_ADDR_M;
        pstSnsState->astRegsInfo[0].astI2cData[7].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[7].u32RegAddr = IMX123_VMAX_ADDR_H;

        pstSnsState->bSyncInit = HI_TRUE;
        bInit = HI_TRUE;
    }
    else
    {
        for (i=0; i<pstSnsState->astRegsInfo[0].u32RegNum; i++)
        {
            if (pstSnsState->astRegsInfo[0].astI2cData[i].u32Data == pstSnsState->astRegsInfo[1].astI2cData[i].u32Data)
            {
                pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = HI_FALSE;
            }
            else
            {
                pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = HI_TRUE;
            }
        }

    }
    
    if (HI_NULL == pstSnsRegsInfo)
    {
        printf("null pointer when get sns reg info!\n");
        return -1;
    }

    pstSnsRegsInfo->bConfig = HI_FALSE;
    memcpy(pstSnsRegsInfo, &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));
    memcpy(&pstSnsState->astRegsInfo[1], &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));

    pstSnsState->au32FL[1] = pstSnsState->au32FL[0];
    //printf(" pstSnsState->u8ImgMode =%d\n", u8SensorImageMode);
   // printf("pstSnsState->au32FL[0] =%d\n",pstSnsState->u32FLStd);
  //  printf(" pstSnsState->au32FL[1]=%d \n", pstSnsState->au32FL[0]);

    return HI_SUCCESS;
    
}

#define IMX123_FOR_CAR_ERR_MODE_PRINT(pstSensorImageMode,pstSnsState)\
    do{\
        ISP_TRACE(HI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",\
                  pstSensorImageMode->u16Width,  \
                  pstSensorImageMode->u16Height, \
                  pstSensorImageMode->f32Fps,    \
                  pstSnsState->enWDRMode);\
    }while(0)

static HI_S32 cmos_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
    
    bInit = HI_FALSE;    

    printf("cmos_set_image_mode\n");
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER(pstSensorImageMode);
    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    HI_U8 u8SensorImageMode = pstSnsState->u8ImgMode;

    if (HI_NULL == pstSensorImageMode)
    {
        printf("null pointer when set image mode\n");
        return HI_FAILURE;
    }
    //u8SensorImageMode = pstSnsState->u8ImgMode;
    pstSnsState->bSyncInit = HI_FALSE;

    printf("pstSnsState->enWDRMode  %d \n",pstSnsState->enWDRMode);
    printf("pstSensorImageMode->u16Width  %d \n",pstSensorImageMode->u16Width);
    printf("pstSensorImageMode->u16Height  %d \n",pstSensorImageMode->u16Height);
    printf("pstSensorImageMode->f32Fps  %d \n",pstSensorImageMode->f32Fps);

    if((pstSensorImageMode->u16Width <= 1920)&&(pstSensorImageMode->u16Height <= 1080))
    {
        if (pstSensorImageMode->f32Fps <= 60)
        {
            u8SensorImageMode = IMX123_forcar_1080P_60FPS_MODE;
            pstSnsState->u32FLStd  = IMX123_VMAX_1080P_60FPS_LINEAR;
        }
        else
        {
            printf("Not support! Width:%d, Height:%d, Fps:%f\n", 
            pstSensorImageMode->u16Width, 
            pstSensorImageMode->u16Height,
            pstSensorImageMode->f32Fps);
            return HI_FAILURE;
        }
    }
    else if((pstSensorImageMode->u16Width <= 2048)&&(pstSensorImageMode->u16Height <= 1536))
    {
        if (pstSensorImageMode->f32Fps <= 60)
        {
            u8SensorImageMode = IMX123_forcar_QXGA_30FPS_MODE;
            pstSnsState->u32FLStd  = IMX123_VMAX_QXGA_30FPS_LINEAR;
        }
        else
        {
            printf("Not support! Width:%d, Height:%d, Fps:%f\n", 
            pstSensorImageMode->u16Width, 
            pstSensorImageMode->u16Height,
            pstSensorImageMode->f32Fps);
            return HI_FAILURE;
        }
    }
    else
    {
        IMX123_FOR_CAR_ERR_MODE_PRINT(pstSensorImageMode, pstSnsState);
        printf("Not support! Width:%d, Height:%d, Fps:%f\n", 
        pstSensorImageMode->u16Width, 
        pstSensorImageMode->u16Height,
        pstSensorImageMode->f32Fps);
        return HI_FAILURE;
    }
    /* Sensor first init */
    if (HI_FALSE == bSensorInit)
    {
        pstSnsState->u8ImgMode = u8SensorImageMode;
        return HI_SUCCESS;
    }

    /* Switch SensorImageMode */
    if (u8SensorImageMode == pstSnsState->u8ImgMode)
    {
        /* Don't need to switch SensorImageMode */
        return ISP_DO_NOT_NEED_SWITCH_IMAGEMODE;
    }

   /* if ((HI_TRUE == pstSnsState->bInit) && (u8SensorImageMode == pstSnsState->u8ImgMode))
    {

        printf("Don't need to switch SensorImageMode \n");
        // Don't need to switch SensorImageMode 
        return ISP_DO_NOT_NEED_SWITCH_IMAGEMODE;
    }*/

    pstSnsState->u8ImgMode = u8SensorImageMode;
    pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
    pstSnsState->au32FL[1] = pstSnsState->au32FL[0];
    memset(pstSnsState->au32WDRIntTime, 0, sizeof(pstSnsState->au32WDRIntTime));
    printf(" pstSnsState->u8ImgMode =%d\n", u8SensorImageMode);
    printf(" pstSnsState->au32FL[0] =%d\n",pstSnsState->u32FLStd);
    printf(" pstSnsState->au32FL[1]=%d \n", pstSnsState->au32FL[0]);

    return HI_SUCCESS;
    
}

static HI_VOID sensor_global_init(VI_PIPE ViPipe)
{
     printf("sensor_global_init \r\n");
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    pstSnsState->bInit = HI_FALSE;
    pstSnsState->bSyncInit = HI_FALSE;
    pstSnsState->u8ImgMode = IMX123_forcar_QXGA_30FPS_MODE;
    pstSnsState->enWDRMode = WDR_MODE_NONE;
    pstSnsState->u32FLStd = IMX123_VMAX_QXGA_30FPS_LINEAR;
    pstSnsState->au32FL[0] = IMX123_VMAX_QXGA_30FPS_LINEAR;
    pstSnsState->au32FL[1] = IMX123_VMAX_QXGA_30FPS_LINEAR;
    gu32RHS1 = 0xe6;

    genFSWDRMode = ISP_FSWDR_NORMAL_MODE;
    
    memset(&pstSnsState->astRegsInfo[0], 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&pstSnsState->astRegsInfo[1], 0, sizeof(ISP_SNS_REGS_INFO_S));
   
}

static HI_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
     printf("cmos_init_sensor_exp_function \r\n");
    CMOS_CHECK_POINTER(pstSensorExpFunc);

    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init         = IMX123_forcar_init;
    pstSensorExpFunc->pfn_cmos_sensor_exit         = IMX123_forcar_exit;
    pstSensorExpFunc->pfn_cmos_sensor_global_init  = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_set_image_mode      = cmos_set_image_mode;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode        = cmos_set_wdr_mode;
    
    pstSensorExpFunc->pfn_cmos_get_isp_default     = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_get_isp_black_level = cmos_get_isp_black_level;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect    = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_get_sns_reg_info    = cmos_get_sns_regs_info;

    return HI_SUCCESS;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

static HI_S32 sensor_ctx_init(VI_PIPE ViPipe)
{
     printf("sensor_ctx_init \r\n");
    ISP_SNS_STATE_S *pastSnsStateCtx = HI_NULL;

    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

    if (HI_NULL == pastSnsStateCtx)
    {
        pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
        if (HI_NULL == pastSnsStateCtx)
        {
            ISP_TRACE(HI_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
            return HI_ERR_ISP_NOMEM;
        }
    }

    memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));

    IMX123_FORCAR_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

    return HI_SUCCESS;
}

static HI_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
     printf("sensor_ctx_exit \r\n");
    ISP_SNS_STATE_S *pastSnsStateCtx = HI_NULL;
    bFirstFps[ViPipe] = HI_TRUE;
    IMX123_FORCAR_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
    SENSOR_FREE(pastSnsStateCtx);
    IMX123_FORCAR_SENSOR_RESET_CTX(ViPipe);
}

static HI_S32 sensor_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
     printf("sensor_register_callback \r\n");
    HI_S32 s32Ret;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;
    ISP_SNS_ATTR_INFO_S   stSnsAttrInfo;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);
    ISP_TRACE(HI_DBG_ERR, "sensor_register_callback\n");
    s32Ret = sensor_ctx_init(ViPipe);

    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    stSnsAttrInfo.eSensorId = IMX123_ID;

    s32Ret  = cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret |= HI_MPI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);

    if (HI_SUCCESS != s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }

    s32Ret  = cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret |= HI_MPI_AE_SensorRegCallBack(ViPipe, pstAeLib, &stSnsAttrInfo, &stAeRegister);

    if (HI_SUCCESS != s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret  = cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret |= HI_MPI_AWB_SensorRegCallBack(ViPipe, pstAwbLib, &stSnsAttrInfo, &stAwbRegister);

    if (HI_SUCCESS != s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }
    
    return HI_SUCCESS;
}


static HI_S32 sensor_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{    
     printf("sensor_unregister_callback \r\n");
    HI_S32 s32Ret;

    CMOS_CHECK_POINTER(pstAeLib);
    CMOS_CHECK_POINTER(pstAwbLib);

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(ViPipe, IMX123_ID);

    if (HI_SUCCESS != s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }

    s32Ret = HI_MPI_AE_SensorUnRegCallBack(ViPipe, pstAeLib, IMX123_ID);

    if (HI_SUCCESS != s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(ViPipe, pstAwbLib, IMX123_ID);

    if (HI_SUCCESS != s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    sensor_ctx_exit(ViPipe);

    return HI_SUCCESS;
}

static HI_S32 sensor_set_init(VI_PIPE ViPipe, ISP_INIT_ATTR_S *pstInitAttr)
{

     printf("sensor_set_init \r\n");
    CMOS_CHECK_POINTER(pstInitAttr);

    g_au32InitExposure[ViPipe]  = pstInitAttr->u32Exposure;
    g_au32LinesPer500ms[ViPipe] = pstInitAttr->u32LinesPer500ms;
    g_au16InitWBGain[ViPipe][0] = pstInitAttr->u16WBRgain;
    g_au16InitWBGain[ViPipe][1] = pstInitAttr->u16WBGgain;
    g_au16InitWBGain[ViPipe][2] = pstInitAttr->u16WBBgain;
    g_au16SampleRgain[ViPipe]   = pstInitAttr->u16SampleRgain;
    g_au16SampleBgain[ViPipe]   = pstInitAttr->u16SampleBgain;

    return HI_SUCCESS;
}

static HI_S32 IMX123_forcar_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
    printf("IMX123_forcar_set_bus_info \r\n");
    g_aunImx123_forcar_BusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;
    return HI_SUCCESS;
}


void IMX123_forcar_standby(VI_PIPE ViPipe)
{
	// TODO:
	return;
}

void IMX123_forcar_restart(VI_PIPE ViPipe)
{
	// TODO:
	return;
}

ISP_SNS_OBJ_S stSnsImx123Obj =
{
    .pfnRegisterCallback    = sensor_register_callback,
    .pfnUnRegisterCallback  = sensor_unregister_callback,
    .pfnStandby             = IMX123_forcar_standby,
    .pfnRestart             = IMX123_forcar_restart,
    .pfnMirrorFlip          = HI_NULL,
    .pfnWriteReg            = IMX123_forcar_write_register,
    .pfnReadReg             = IMX123_forcar_read_register,
    .pfnSetBusInfo          = IMX123_forcar_set_bus_info,
    .pfnSetInit             = sensor_set_init
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __IMX123_FORCAR_CMOS_H_ */

