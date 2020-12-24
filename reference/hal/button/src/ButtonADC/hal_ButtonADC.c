/**
* @file    hi_hal_Button_ADC.c
* @brief   hal Button_ADC implemention
*
* Copyright (c) 2017 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date      2017/12/17
* @version   1.0

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pthread.h>


#include "hi_type.h"
#include "hi_hal_common_inner.h"
#include "hal_ButtonADC.h"
#include "hi_hal_common.h"
#include "hi_appcomm_log.h"
#include "hi_appcomm_util.h"
#include "hi_hal_common.h"
#include "hi_hal_adc_inner.h"
//#include "hi_gpio.h"
#include "hi_adc.h"


#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif  /* End of #ifdef __cplusplus */

//GPIO3_1_KEY_MENU LSADC_CH1/GPIO10_4

#define ADC_CH1_IOCFG_REG                (0x111F0034)
#define ADC_GPIO                 (0x120DA400) //10_4

#define HAL_ADC_MAXCNT                         (4)
#define HAL_ADC_SINGLE_MODE                    (0)
#define HAL_ADC_CONTINOUS_MODE                 (1)

static HI_S32 s_s32ButADCfd = HAL_FD_INITIALIZATION_VAL;
static pthread_mutex_t s_ButADCMutex = PTHREAD_MUTEX_INITIALIZER;

//#define HAL_Button_ADC_DEV    ("/dev/B_adc")
#define HAL_Button_ADC_DEV    ("/dev/hi_lsadc")


typedef struct tagHAL_ADC_STATE_S
{
    HI_BOOL bHALADCInitState;
} HAL_ADC_STATE_S;

typedef struct tagHAL_ADC_CTX_S
{
    HI_U32 u32InitHdlCnt;
    HAL_ADC_STATE_S astAdcState[HAL_ADC_MAXCNT];
} HAL_ADC_CTX_S;
static HAL_ADC_CTX_S s_sBUTAdcCtx;


/** macro define */
HI_S32 Button_ADC_Init(HI_HANDLE AdcHdl )
{
    HI_S32 s32Model = HAL_ADC_CONTINOUS_MODE;
    int ret =-1;


    HI_U32 t;
    himd(ADC_GPIO, &t);
    printf("ADC_GPIO 0x%x =>0x%x.\n",ADC_CH1_IOCFG_REG,t);
    writereg(0x0, 0x10, ADC_GPIO);/*GPIO 10_4*/
    himd(ADC_GPIO, &t);
    printf("ADC_GPIO 0x%x =>0x%x.\n",ADC_CH1_IOCFG_REG,t);
    writereg(0x0, 0xf, ADC_CH1_IOCFG_REG);/*LSADC_CH1*/

    
    printf("Button_ADC_Init %d.\n",AdcHdl);
    HI_MUTEX_LOCK(s_ButADCMutex);

    if (HI_TRUE == s_sBUTAdcCtx.astAdcState[AdcHdl].bHALADCInitState)
    {
        printf("ADC[%d] has Already Init.\n",AdcHdl);
        HI_MUTEX_UNLOCK(s_ButADCMutex);
        return HI_SUCCESS;
    }
   // himm(0x111F0034,0x430);



    /**Open ADC device once*/
    if (HAL_FD_INITIALIZATION_VAL == s_s32ButADCfd)
    {
        /** Initial ADC Device */
        s_s32ButADCfd = open(HAL_Button_ADC_DEV, O_RDWR);

        if (HAL_FD_INITIALIZATION_VAL == s_s32ButADCfd)
        {
            printf("[Error] open adcdev failed\n");
            HI_MUTEX_UNLOCK(s_ButADCMutex);
            return HI_HAL_EINVOKESYS;
        }

        if(ioctl(s_s32ButADCfd, LSADC_IOC_MODEL_SEL, &s32Model) <0)
        {
            printf("[Error] Set LSADC Mode failed\n");
            close(s_s32ButADCfd);
            HI_MUTEX_UNLOCK(s_ButADCMutex);
            return HI_HAL_EINVOKESYS;
        }
            printf("Enalbe LSADC Mode[%d] Pass\n",s32Model);
    }
   

    /**Enable Chn*/
    ret=ioctl(s_s32ButADCfd, LSADC_IOC_CHN_ENABLE, &AdcHdl);
    if(ret <0)
    {
        printf("[Error] Enalbe LSADC Chn[%d] failed\n",AdcHdl);
        close(s_s32ButADCfd);
        HI_MUTEX_UNLOCK(s_ButADCMutex);
        return HI_HAL_EINVOKESYS;
    }
    else
    {
        
        printf("Enalbe LSADC Chn[%d] Pass\n",AdcHdl);
    }
    

    /**Start Chn*/

    ret=-1;
    ret=ioctl(s_s32ButADCfd, LSADC_IOC_START, &AdcHdl);
    if(ret <0)
    {
        printf("[Error] Start LSADC failed\n");
        close(s_s32ButADCfd);
        HI_MUTEX_UNLOCK(s_ButADCMutex);
        return HI_HAL_EINVOKESYS;
    }
    else
    {
        
        printf("Start LSADC Chn[%d] Pass\n",AdcHdl);
    }

    s_sBUTAdcCtx.u32InitHdlCnt++;
    s_sBUTAdcCtx.astAdcState[AdcHdl].bHALADCInitState = HI_TRUE;
    HI_MUTEX_UNLOCK(s_ButADCMutex);
    return HI_SUCCESS;
}

HI_S32 Button_ADC_VAl(HI_HANDLE AdcHdl,HI_S32* ps32vel)
{

   printf("Button_ADC_VAl chn %d\n",AdcHdl);
   if (HI_NULL == ps32vel)
    {
        printf("[Error]Null pointer.\n");
        return HI_HAL_EINVAL;
    }

    if (HI_TRUE != s_sBUTAdcCtx.astAdcState[AdcHdl].bHALADCInitState)
    {
        printf("ADC[%d] has not Init.\n",AdcHdl);
        return HI_HAL_ENOINIT;
    }

    if (HAL_FD_INITIALIZATION_VAL == s_s32ButADCfd)
    {
        printf("Dev has not been opened.\n");
        return HI_HAL_EINTER;
    }
    
    HI_MUTEX_LOCK(s_ButADCMutex);
    *ps32vel = ioctl(s_s32ButADCfd, LSADC_IOC_GET_CHNVAL, &AdcHdl);

    printf("Button_ADC_VAl Data %d\n",*ps32vel);
    HI_MUTEX_UNLOCK(s_ButADCMutex);
    return HI_SUCCESS;

}

HI_S32 Button_ADC_GetKey(HI_HANDLE AdcHdl,HI_S32* pbkeyNum)
{

    printf("Button_ADC_GetKey chn %d.\n",AdcHdl);
  HI_S32 s32Ret = 0;
    HI_S32 s32val = 0;
    HI_S32 bkeyNum = 22;
    /* parm ps32Level check */
    if (NULL == pbkeyNum)
    {
        printf("pbkeyNum is null\n");
        return HI_HAL_EINVAL;
    }

    
    s32Ret = Button_ADC_VAl(AdcHdl,&s32val);
   //s32Ret = ioctl(s_s32ButADCfd, IOCTL_CMD_READ_CAPACITY, &s32Level);
    if(HI_FAILURE == s32Ret)
    {
        printf("get But_adc_Key error\n");
        return HI_HAL_EINTER;
    }
  
    
    *pbkeyNum = bkeyNum;

    return HI_SUCCESS;
}

HI_S32 Button_ADC_Deinit(HI_HANDLE AdcHdl)
{

    printf("Button_ADC_Deinit %d.\n",AdcHdl);
    HI_MUTEX_LOCK(s_ButADCMutex);
    if (HI_TRUE != s_sBUTAdcCtx.astAdcState[AdcHdl].bHALADCInitState)
    {
        printf("ADC[%d] has not been Init.\n",AdcHdl);
        HI_MUTEX_UNLOCK(s_ButADCMutex);
        return HI_SUCCESS;
    }

    if (HAL_FD_INITIALIZATION_VAL != s_s32ButADCfd &&
        1 == s_sBUTAdcCtx.u32InitHdlCnt)
    {
        /** Deinit ADC Device */
        close(s_s32ButADCfd);
    }
    s_sBUTAdcCtx.u32InitHdlCnt--;
    HI_MUTEX_LOCK(s_ButADCMutex);
    return HI_SUCCESS;
}
#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif  /* End of #ifdef __cplusplus */

