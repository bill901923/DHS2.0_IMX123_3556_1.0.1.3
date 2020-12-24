
#ifndef __DHS_LED_H__
#define __DHS_LED_H__

#include "dhs_app.h"
#ifdef __cplusplus
extern "C" {
#endif

//DCDC for PWM EN
#define set_PWM_EN_IO_USE himm(0x114F0058,0x600)
//#define set_PWM_IO himm(0x120D3400,0x43)
#define open_PWM_EN himm(0x120D3100,0x40)
#define clean_PWM_EN himm(0x120D3100,0)

//LED_PWM1
#define set_PWM2_IO_USE himm(0x111F0028,0x631)	  //iocfg_reg61 PWM1/GPIO6_7 0x631
#define set_PWM2_Period(x) himm(0x12070020,x)	  //iocfg_reg61 PWM1/GPIO6_7 0x631
#define set_PWM2_Duty(x) himm(0x12070024,x)	//PWM0_CFG0 PWM0 的配置 0  50M/14k = 3571
#define PWM2_Run himm(0x1207002C,0x05)
#define PWM2_Stop himm(0x1207002C,0x00)

//LED_PWM2
#define set_PWM1_IO_USE himm(0x111F0024,0x631)	  //iocfg_reg61 PWM1/GPIO6_7 0x631
#define set_PWM1_Period(x) himm(0x12070000,x)	  //iocfg_reg61 PWM1/GPIO6_7 0x631
#define set_PWM1_Duty(x) himm(0x12070004,x)	//PWM0_CFG0 PWM0 的配置 0  50M/14k = 3571
#define PWM1_Run himm(0x1207000C,0x05)
#define PWM1_Stop himm(0x1207000C,0x00)

//LED_OPTION		
#define set_LED_OP_IO_USE himm 0x114F0064 0x0600  //iocfg_reg43   VI_HS/GPIO4_7/SENSOR1_RSTN
//#define set_LED_OP_DR himm(0x120D4400,0x80)
#define set_LED_OP_ch1 himm(0x120D4200,0x0)
#define set_LED_OP_ch2 himm(0x120D4200,0x80)


//LED_C_1/LED_PWM_OE
//#define himm 0x114F0044 0x0  //iocfg_reg35		   VI_DATA1/GPIO3_1/VOU_DATA1
#define set_LED_OE_IO_USE himm(0x120D3400,0x43)
#define set_LED_OE_open   himm(0x120D3008,0x00)
#define set_LED_OE_clean  himm(0x120D3008,0x02)

//Cut
#define  CUTA_IO_USE himm(0x114F0068,0x400) //iocfg_reg44  VI_CLK/GPIO2_3/VOU_CLK
#define  CUTB_IO_USE himm(0x114F0040,0x400)  //iocfg_reg34  VI_DATA0/GPIO3_0/VOU_DATA0

//#define  CUTA_IO_OUT_SET himm(0x120D2400,0x08) //IO_OUT_SET
//#define  CUTB_IO_OUT_SET himm(0x120D3400,0x41)

#define  CUTA_open himm(0x120D2020,0x08)
#define  CUTA_clean himm(0x120D2020,0x0)
#define  CUTB_open himm(0x120D3004,0x01)
#define  CUTB_clean himm(0x120D3004,0x0)




int cutA_open();
int cutB_open();
int cutA_close();
int cutB_close();

int led_init();
int led_all_close();
int led_uv_open(int v);
int led_wl2_open(int v);
int led_wl1_open(int v);
#ifdef __cplusplus
}
#endif
#endif //__DHS_LED_H__

