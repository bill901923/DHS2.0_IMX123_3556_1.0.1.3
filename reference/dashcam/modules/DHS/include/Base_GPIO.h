
#ifndef __BASE_GPIO_H__
#define __BASE_GPIO_H__

#include "dhs_app.h"
#include "hi_hal_common.h"
#include "hi_appcomm_util.h"
#define GPIO_base_adr 0x120D0000 

#ifdef __cplusplus
extern "C" {
#endif


void GPIO_SetIO(HI_U8 gpio_port, HI_U8 gpio_num,HI_U8 set_out);
void GPIO_SetReg(HI_U8 gpio_port, HI_U8 gpio_num,HI_U8 set_out);
HI_U32 GPIO_GetReg(HI_U64 u64Addr);
#ifdef __cplusplus
}
#endif
#endif //__BASE_GPIO_H__

