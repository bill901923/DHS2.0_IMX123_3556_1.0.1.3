#include <stdio.h>
#include <stdlib.h>
#include "sample_comm.h"
#include "dhs_led.h"



/*
CAT
himm 0x114F0068 0x400  //iocfg_reg44  VI_CLK/GPIO2_3/VOU_CLK
himm 0x114F0040	0x400  //iocfg_reg34  VI_DATA0/GPIO3_0/VOU_DATA0
himm 0x120D2400 0x08
himm 0x120D3400 0x01
himm 0x120D2020 0x08
himm 0x120D2020 0x0
himm 0x120D3004 0x1
himm 0x120D3004 0x0

*/
int cutA_open() //cross
{
    CUTA_open;
    //GPIO_SetIO(3,0,1) ;
    //himm(0x120D3004, 0x01);
    return 0;
}
int cutB_open() //parallel
{
    CUTB_open;
    //GPIO_SetIO(2,3,1) ;
    //himm(0x120D2020, 0x08);
    return 0;
}
int cutA_close()
{
    CUTA_clean;
    //GPIO_SetIO(3,0,0) ;
   // himm(0x120D3004, 0);
    return 0;
}
int cutB_close()
{
    CUTB_clean;
   // GPIO_SetIO(2,3,0) ;
   // himm(0x120D2020, 0);
    return 0;
}

int led_init(){
    printf(">>>>>>>>>>>>>>>>>>>>>>>> led_init \n");
    HI_U32 value;
    himm(0x114F0040,0x400 );
    himm(0x114F0040,0x400 );
    himm(0x114F0058,0x0 );   //DCDC   Set IO mode
    
   // LED_OPTION		
   himm(0x114F0064,0x0 ); //iocfg_reg43   GPIO4_7
   himm(0x120D4400,0x80);
  //  GPIO_SetReg(4,7,1) ;
   GPIO_SetIO(4,7,0) ;


    himm(0x114F0044,0x0 ); // LED_C_1/LED_PWM_OE  GPIO3_1    
    GPIO_SetIO(3,6,0) ;     //LED_P_EN  GPIO3_6

    himm(0x120D3400,0x43);
    GPIO_SetIO(3,6,0) ;     //LED_P_EN  GPIO3_6
    GPIO_SetIO(3,1,0) ;     // LED_C_1/LED_PWM_OE  GPIO3_1

    himd(0x120101BC, &value);  //PERI_CRG111 50M 0x01807883 （3M）   pwm_cken:7 bit
    if ((value & 0x000003C0) != 0x00000180)
    {
        value &= 0xFFFFFC3F;
        value |= 0x00000180;
        himm(0x120101BC, value);
    }

    himm(0x1207000C,0x0 );//PWM0_CTRL PWM0 的控制
    himm(0x1207002C,0x0 );//PWM0_CTRL PWM0 的控制

    himm(0x12070000,3571 );//PWM0_CFG0 PWM0 的配置 0  50M/14k = 3571
    himm(0x12070020,3571 );//PWM0_CFG0 PWM0 的配置 0  50M/14k = 3571

    himm(0x12070024 ,740);
    himm(0x12070004,2000);

    himm(0x111F0024,0x631 );  //PWM0   Set IO PWM    
    himm(0x111F0028,0x631 );  //PWM1   Set IO PWM

    return 0;
}


int led_all_close()
{
    clean_PWM_EN;   //DCDC close
    set_LED_OE_clean;
    PWM1_Stop;    //PWM1
    PWM2_Stop;    //PWM2
    cutA_close();
    cutB_close();
    return 0;
}
int led_close()
{

    set_LED_OE_clean;
    PWM1_Stop;    //PWM1
    PWM2_Stop;    //PWM2
    return 0;
}


int led_uv_open(int v){
    led_close();
    set_LED_OP_ch2;

    int set_v=2000;
    
    if(v>0)  
    {
        set_v=v;
    } 
    set_PWM1_Duty(set_v);//PWM0_CFG1 PWM0 的配置 1

    printf(">>>>>>>>>>>>>>>>>>>>>>>> PWM1 uv:%d \n",v);

    open_PWM_EN;
    set_LED_OE_open;
    //pwm2
    PWM1_Run;//PWM0_CTRL PWM0 的控制
    return 0;
}

int led_wl1_open(int v){ ///
    led_close();
    //GPIO_SetIO(4,7,0) ; //LED_OPTION GPIO4_7
    set_LED_OP_ch1;

    int set_v=800;
    
    if(v>0)  
    {
        set_v=v;
    } 
    set_PWM1_Duty(set_v);//PWM0_CFG1 PWM0 的配置 1

    printf(">>>>>>>>>>>>>>>>>>>>>>>> PWM1 wl:%d \n",v);

    open_PWM_EN;
    set_LED_OE_open;
    //pwm1
    PWM1_Run;
    return 0;
}

int led_wl2_open(int v){
    led_close();
    //GPIO_SetIO(4,7,0) ; //LED_OPTION GPIO4_7
    set_LED_OP_ch1;

    int set_v=800;
    
    if(v>0)  
    {
        set_v=v;
    } 
    set_PWM2_Duty(set_v);//PWM0_CFG1 PWM0 的配置 1

    printf(">>>>>>>>>>>>>>>>>>>>>>>> PWM2 pl:%d \n",v);
    open_PWM_EN;
    set_LED_OE_open;
    //pwm1
    PWM2_Run;
    return 0;
}


