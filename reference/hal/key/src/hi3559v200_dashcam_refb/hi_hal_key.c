/**
* @file    hi_hal_key.c
* @brief   HAL key implemention
*
* Copyright (c) 2017 Huawei Tech.Co.,Ltd
*
* @author    HiMobileCam Reference Develop Team
* @date      2017/12/16
* @version   1.0

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#include "hi_hal_common_inner.h"
#include "hi_hal_gpio_inner.h"

#include "hi_hal_key.h"
#include "hi_hal_common.h"
#include "hi_appcomm_log.h"
#include "hi_appcomm_util.h"

#include "hi_hal_adc_inner.h"


#include <sys/ioctl.h>
#include "hi_adc.h"
/*
#include "dictionary.h"
#include "iniparser.h"
*/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif  /* End of #ifdef __cplusplus */

/** macro define */
#define HAL_KEY_MEM_DEV  "/dev/mem"
#define HAL_KEY_POWER_CTRL_BASE  (0x12090000) /**< power control base register address */
#define HAL_KEY_PWR_CTRL0_OFFSET (0x0008)     /**< power control0 offset */


#ifdef CONFIG_ADC_ON

#define HAL_KEY_KEY3_ADC_VALUEMIN (85)
#define HAL_KEY_KEY3_ADC_VALUEMAX (100)
#define HAL_KEY_KEY2_ADC_VALUEMIN (160)
#define HAL_KEY_KEY2_ADC_VALUEMAX (175)
#define HAL_KEY_ADC_CHN           (0)
#define HAL_KEY_ADC2_CHN          (1)

// 294 238 181 117 173 279


  

#define CONFIG_Key_NAME "app/key_conf.ini"

#define HAL_KEY_KEY4_1_ADC_VALUEMIN_H (914) // 2.94 
#define HAL_KEY_KEY4_1_ADC_VALUEMIN_L (890) // 2.94 

#define HAL_KEY_KEY4_2_ADC_VALUEMIN_H (880) // 2.79 USE
#define HAL_KEY_KEY4_2_ADC_VALUEMIN_L (830) // 2.79 USE

#define HAL_KEY_KEY4_3_ADC_VALUEMIN_H (750) // 2.38  defual 739 user_set 718  USE
#define HAL_KEY_KEY4_3_ADC_VALUEMIN_L (710) // 2.38  defual 739 user_set 718  USE

#define HAL_KEY_KEY4_4_ADC_VALUEMIN_H (680) // 1.81  defual 562
#define HAL_KEY_KEY4_4_ADC_VALUEMIN_L (630) // 1.81       

#define HAL_KEY_KEY4_5_ADC_VALUEMIN_H (560) // 1.73 USE defual 537
#define HAL_KEY_KEY4_5_ADC_VALUEMIN_L (510) // 1.73 USE

#define HAL_KEY_KEY4_6_ADC_VALUEMIN_H (400) // 1.17 USE defual 383
#define HAL_KEY_KEY4_6_ADC_VALUEMIN_L (360) // 1.17 USE



#else

#define HAL_KEY_KEY2_GRP_NUM (4)     /**< key2 gpio group number */
#define HAL_KEY_KEY2_BIT_NUM (5)       /**< key2 gpio bit number */

#endif

#define HAL_KEY_READ_REG(addr)             (*(volatile HI_U32 *)(addr))

#define HAL_KEY_KEY1_GRP_NUM (0)     /**< key1 gpio group number */
#define HAL_KEY_KEY1_BIT_NUM (0)     /**< key1 gpio bit number */


HI_U32  s_u32HALKEYPwrCtrlBase;  /**< address mmap from power control base register address */
HI_U32* s_pu32HALKEYPwrCtrlBase; /**< address mmap from power control base register address */

static HI_S32 s_s32HALKEYfd = HAL_FD_INITIALIZATION_VAL;
static HI_S32 s_s32HALKEYMEMfd = HAL_FD_INITIALIZATION_VAL;


static HI_S32 cuntchack=0;
static HI_S32 button_num=0;
static HI_BOOL button_flag=HI_FALSE;

/*
void creat_KEY_inifile(char *name)
{
    dictionary *ini         = NULL;

    iniparser_set(ini,"Key1:KEY_H","914");
    iniparser_set(ini,"Key1:KEY_L","890");

    iniparser_set(ini,"Key2:KEY_H","880");
    iniparser_set(ini,"Key2:KEY_L","830");

    iniparser_set(ini,"Key3:KEY_H","750");
    iniparser_set(ini,"Key3:KEY_L","710");

    iniparser_set(ini,"Key4:KEY_H","680");
    iniparser_set(ini,"Key4:KEY_L","630");

    iniparser_set(ini,"Key5:KEY_H","560");
    iniparser_set(ini,"Key5:KEY_L","510");

    iniparser_set(ini,"Key6:KEY_H","400");
    iniparser_set(ini,"Key6:KEY_L","360");
    //   printf("fopen\n");
    FILE * f = fopen(name,"w");    
    //   printf("iniparser_dump_ini\n");
    iniparser_dump_ini(ini, f);
    //    printf("fclose\n");
    fclose(f);
    //    printf("iniparser_freedict\n");
    iniparser_freedict(ini);
    return;
}

int init_KEY_config(){
    
    dictionary *ini         = NULL;
 
    //加载文件
    ini = iniparser_load(CONFIG_Key_NAME);
    if ( ini == NULL ) {
        printf("creat_inifile!\n");
        creat_KEY_inifile(CONFIG_Key_NAME);
        ini = iniparser_load(CONFIG_Key_NAME);
        if ( ini == NULL ) {
            printf("stone:iniparser_load error!\n");
            exit(-1);
        }
    }
    iniparser_freedict(ini);

    printf("init_KEY_config PASS \n");
    return 0;
}
*/
HI_S32 HI_HAL_KEY_Init()
{
    HI_S32 s32Ret = HI_SUCCESS;
    //init_KEY_config();

    if (HAL_FD_INITIALIZATION_VAL == s_s32HALKEYfd)
    {
        /** Initial GPIO Device */
        s32Ret = HAL_GPIO_Init(&s_s32HALKEYfd);

        if (HI_FAILURE == s32Ret)
        {
            MLOGE("[Error] open gpiodev failed,errno(%d)\n",errno);
            return HI_HAL_EINTER;
        }

        /** Initial Power Ctrl */
        s_s32HALKEYMEMfd = open(HAL_KEY_MEM_DEV, O_RDWR | O_SYNC, 00777);

        if (s_s32HALKEYMEMfd < 0)
        {
            MLOGE("[Error] open memdev failed\n");
            HAL_GPIO_Deinit(s_s32HALKEYfd);
            s_s32HALKEYfd = HAL_FD_INITIALIZATION_VAL;
            return HI_HAL_EINVOKESYS;
        }
        himm(0x114f0080, 0x400);
#ifdef CONFIG_ADC_ON

        MLOGD("Set ADC pin to GPIO to enable Key.\n\n");
        s32Ret = HAL_ADC_Init(HAL_KEY_ADC_CHN);
        if (HI_SUCCESS != s32Ret)
        {
            MLOGE("[Error]ADC[%d] Init Failed.\n\n",HAL_KEY_ADC_CHN);
            close(s_s32HALKEYMEMfd);
            s_s32HALKEYMEMfd = HAL_FD_INITIALIZATION_VAL;
            HAL_GPIO_Deinit(s_s32HALKEYfd);
            s_s32HALKEYfd = HAL_FD_INITIALIZATION_VAL;
        }
        s32Ret = HAL_ADC_Init(HAL_KEY_ADC2_CHN);
        if (HI_SUCCESS != s32Ret)
        {

            MLOGE("[Error]ADC[%d] Init Failed.\n\n",HAL_KEY_ADC2_CHN);
        }
        
#else
        /**Set ADC pin to GPIO to enable Key*/

        MLOGD("Set ADC pin to GPIO to enable Key.\n\n");
        himm(0x111f0030, 0x401);
        himm(0x111f0034, 0x401);
#endif

        s_pu32HALKEYPwrCtrlBase = (HI_U32*)mmap(NULL, 0x50, \
                                                PROT_READ | PROT_WRITE, MAP_SHARED, s_s32HALKEYMEMfd, HAL_KEY_POWER_CTRL_BASE);

        if (s_pu32HALKEYPwrCtrlBase == MAP_FAILED)
        {
            MLOGE("[Error] power_ctrl mmap failed,errno(%d)\n",errno);
            close(s_s32HALKEYMEMfd);
            s_s32HALKEYMEMfd = HAL_FD_INITIALIZATION_VAL;
            HAL_GPIO_Deinit(s_s32HALKEYfd);
            s_s32HALKEYfd = HAL_FD_INITIALIZATION_VAL;

            return HI_HAL_EINVOKESYS;
        }


        s_u32HALKEYPwrCtrlBase = (HI_U32)s_pu32HALKEYPwrCtrlBase;
    }
    else
    {
        MLOGE("gpio already be opened\n");
        return HI_HAL_EINITIALIZED;
    }

    return HI_SUCCESS;
}


HI_S32 HI_HAL_KEY_GetState(HI_HAL_KEY_IDX_E enKeyIdx, HI_HAL_KEY_STATE_E* penKeyState,HI_U32* Button_NumO)
{
  //  HI_U32 u32GpioGrpNum, u32GpioBitNum;
    HI_U32 u32Val;
   // HI_S32 s32Ret = HI_SUCCESS;
#ifdef CONFIG_ADC_ON
    HI_S32 button_num_temp=0;
    HI_S32 s32AdcValue = 0;
#endif

    /* init check */
    if (HAL_FD_INITIALIZATION_VAL == s_s32HALKEYfd)
    {
        MLOGE("key not initialized\n");
        return HI_HAL_ENOINIT;
    }

    /* parm penKeyState check */
    if (NULL == penKeyState)
    {
        MLOGE("penKeyState is null\n");
        return HI_HAL_EINVAL;
    }

   // MLOGE("enKeyIdx %d\n",enKeyIdx);
    switch (enKeyIdx)
    {
        case HI_HAL_KEY_IDX_0:
           // u32Val = HAL_KEY_READ_REG(s_u32HALKEYPwrCtrlBase + HAL_KEY_PWR_CTRL0_OFFSET);
            //u32Val = (u32Val & (0x01 << 7)) ? 1 : 0;
            break;
        case HI_HAL_KEY_IDX_4:
#ifdef CONFIG_ADC_ON

            
            HAL_ADC_GetVal(HAL_KEY_ADC2_CHN, &s32AdcValue);
           /* if(s32AdcValue<900)
            {
                MLOGE("HAL_ADC_GetVal  HAL_KEY_ADC2_CHN  %d\n",s32AdcValue);}*/

            if ((s32AdcValue > (HAL_KEY_KEY4_1_ADC_VALUEMIN_L ))&&( s32AdcValue < (HAL_KEY_KEY4_1_ADC_VALUEMIN_H)))
            {
                    button_num_temp=1;
             //   MLOGE("HAL_KEY_KEY4_1_ADC_VALUEMIN\n");
            }
            else if ((s32AdcValue > (HAL_KEY_KEY4_2_ADC_VALUEMIN_L) )&&( s32AdcValue < (HAL_KEY_KEY4_2_ADC_VALUEMIN_H)))
            {
                    button_num_temp=2;
             //   MLOGE("HAL_KEY_KEY4_2_ADC_VALUEMIN\n");
            }
            else if ((s32AdcValue > (HAL_KEY_KEY4_3_ADC_VALUEMIN_L) )&&( s32AdcValue < (HAL_KEY_KEY4_3_ADC_VALUEMIN_H)))
            {

                    button_num_temp=3;
             //   MLOGE("HAL_KEY_KEY4_3_ADC_VALUEMIN\n");
            }
            else if ((s32AdcValue > (HAL_KEY_KEY4_4_ADC_VALUEMIN_L) )&&( s32AdcValue < (HAL_KEY_KEY4_4_ADC_VALUEMIN_H)))
            {
                
                    button_num_temp=4;    
              //  MLOGE("HAL_KEY_KEY4_4_ADC_VALUEMIN\n");
            }
            else if ((s32AdcValue > (HAL_KEY_KEY4_5_ADC_VALUEMIN_L) )&&( s32AdcValue < (HAL_KEY_KEY4_5_ADC_VALUEMIN_H)))
            {

                button_num_temp=5;
              //  MLOGE("HAL_KEY_KEY4_5_ADC_VALUEMIN\n");
            }
            else if ((s32AdcValue > (HAL_KEY_KEY4_6_ADC_VALUEMIN_L) )&&( s32AdcValue < (HAL_KEY_KEY4_6_ADC_VALUEMIN_H)))
            {
                
                button_num_temp=6;    
              //  MLOGE("HAL_KEY_KEY4_6_ADC_VALUEMIN\n");
            }
           //if(button_num!=button_num_temp)
           if(button_num_temp>0)
            {
                
                button_flag=HI_FALSE;   
                if(button_num!=button_num_temp)
                {
                                MLOGE("button_num_temp %d ,%d\n",button_num_temp,s32AdcValue);
                }
                button_num=button_num_temp;
                 cuntchack=0;
               // if(button_num_temp==0)
                //{
                        u32Val = 0;
               // }
              //  else
              //  {
                    float V_val=s32AdcValue;
                    V_val*=3.3;
                    V_val/=1000;
              //    u32Val = 1;
                    button_flag=HI_TRUE;
                    *Button_NumO=button_num; 
                    if(button_num!=button_num_temp)
                    {
                                
                        MLOGE("KEY Button %d,%f\n",button_num,V_val);
                    }
              //  }
                    
            }
            else
            {
                u32Val = 1;
            }
            
         /*   else                                         
            {
                if(button_num)
                {   
                     if((cuntchack>=3)&&(button_flag==HI_FALSE))
                     {
                            u32Val = 1;
                            button_flag=HI_TRUE;
                            *Button_NumO=button_num;
                             
                            MLOGE("KEY Button %d\n",button_num);
                     }
                     else
                     {                         
                        cuntchack++;
                     }
                     
                }
            }*/
            
#endif
            break;
        case HI_HAL_KEY_IDX_3:
           /* #ifdef CONFIG_ADC_ON
                        HAL_ADC_GetVal(HAL_KEY_ADC_CHN, &s32AdcValue);
                        if (s32AdcValue > HAL_KEY_KEY3_ADC_VALUEMIN && s32AdcValue < HAL_KEY_KEY3_ADC_VALUEMAX)
                        {
                            u32Val = 0;
                        }
                        else
                        {
                            u32Val = 1;
                        }
            #else*/
                       u32Val = 1;
           // #endif
            break;
        case HI_HAL_KEY_IDX_2:
              /*  #ifdef CONFIG_ADC_ON
                            HAL_ADC_GetVal(HAL_KEY_ADC_CHN, &s32AdcValue);
                            if (s32AdcValue > HAL_KEY_KEY2_ADC_VALUEMIN && s32AdcValue < HAL_KEY_KEY2_ADC_VALUEMAX)
                            {
                                u32Val = 0;
                            }
                            else
                            {*/
                                u32Val = 1;
               /*             }
                #else
                            u32GpioGrpNum = HAL_KEY_KEY2_GRP_NUM;
                            u32GpioBitNum = HAL_KEY_KEY2_BIT_NUM;
                            s32Ret = HAL_GPIO_SetDir(s_s32HALKEYfd, u32GpioGrpNum, u32GpioBitNum, HAL_GPIO_DIR_READ);

                            if (HI_SUCCESS != s32Ret)
                            {
                                MLOGE("[Error]set gpio dir failed\n");
                                return HI_HAL_EGPIO;
                            }

                            s32Ret = HAL_GPIO_GetBitVal(s_s32HALKEYfd, u32GpioGrpNum, u32GpioBitNum, &u32Val);

                            if (HI_SUCCESS != s32Ret)
                            {
                                MLOGE("[Error]read gpio data failed\n");
                                return HI_HAL_EGPIO;
                            }
                #endif*/
                
             break;
        case HI_HAL_KEY_IDX_1:
                /*           u32GpioGrpNum = HAL_KEY_KEY1_GRP_NUM;
                            u32GpioBitNum = HAL_KEY_KEY1_BIT_NUM;
                            s32Ret = HAL_GPIO_SetDir(s_s32HALKEYfd, u32GpioGrpNum, u32GpioBitNum, HAL_GPIO_DIR_READ);

                            if (HI_SUCCESS != s32Ret)
                            {
                                MLOGE("[Error]set gpio dir failed\n");
                                return HI_HAL_EGPIO;
                            }

                            s32Ret = HAL_GPIO_GetBitVal(s_s32HALKEYfd, u32GpioGrpNum, u32GpioBitNum, &u32Val);

                            if (HI_SUCCESS != s32Ret)
                            {
                                MLOGE("[Error]read gpio data failed\n");
                                return HI_HAL_EGPIO;
                            }
                */
                                u32Val = 1;
            break;

        default:
            MLOGE("illeagel enkey(%d) out of range\n",enKeyIdx);
            return HI_HAL_EINVAL;
    }

    *penKeyState = (1 == u32Val) ? HI_HAL_KEY_STATE_UP : HI_HAL_KEY_STATE_DOWN;

    return HI_SUCCESS;
}


HI_S32 HI_HAL_KEY_Deinit()
{
    HI_S32 s32Ret = 0;

    if (HAL_FD_INITIALIZATION_VAL == s_s32HALKEYfd)
    {
        MLOGE("key not initialized,no need to close\n");
        return HI_HAL_ENOINIT;
    }

    /* DeInit Power Ctrl */
    if (s_pu32HALKEYPwrCtrlBase)
    {
        s32Ret = munmap(s_pu32HALKEYPwrCtrlBase, HAL_MMAP_LENGTH);

        if (0 != s32Ret)
        {
            MLOGE("munmap failed,errno(%d)\n",errno);
            return HI_HAL_EINVOKESYS;
        }

        s_pu32HALKEYPwrCtrlBase = NULL;
    }

    if (HAL_FD_INITIALIZATION_VAL != s_s32HALKEYMEMfd)
    {
        close(s_s32HALKEYMEMfd);
        s_s32HALKEYMEMfd = HAL_FD_INITIALIZATION_VAL;
    }

    /* Deinitial GPIO Device */
    s32Ret = HAL_GPIO_Deinit(s_s32HALKEYfd);
    if (HI_SUCCESS != s32Ret)
    {
        MLOGE("gpio deinit,errno(%d)\n",errno);
        return HI_HAL_EINVOKESYS;
    }
#ifdef CONFIG_ADC_ON
    HAL_ADC_Deinit(HAL_KEY_ADC_CHN);
#endif
    s_s32HALKEYfd = HAL_FD_INITIALIZATION_VAL;

    return HI_SUCCESS;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


