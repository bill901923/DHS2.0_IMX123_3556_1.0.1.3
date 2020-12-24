#include <stdio.h>
#include <stdlib.h>
#include "Base_GPIO.h"
#include "sample_comm.h"

void GPIO_SetIO(HI_U8 gpio_port, HI_U8 gpio_num,HI_U8 set_out)
{
    HI_U32 value=0;
    HI_U32 check_value=set_out<<gpio_num;
    HI_U32 g_Port=0x1000*gpio_port;
    HI_U32 Addr =GPIO_base_adr + g_Port +0x400;

    himd(Addr, &value);  // 10_7 INPUT 10_6 outPUT
   if ((value & check_value) != check_value)
    {
        value &=(~check_value);
        value |= check_value;
        printf("GPIO_SetIO 0x%08x 0x%02x\r\n",Addr,value);
        himm(Addr, value);
    }

}

void GPIO_SetReg(HI_U8 gpio_port, HI_U8 gpio_num,HI_U8 set_out)
{
    HI_U32 value=set_out<<gpio_num;
    HI_U32 g_Port=0x1000*gpio_port;
    HI_U32 g_Num=1<<(2+gpio_num);
    HI_U32 Addr =GPIO_base_adr + g_Port + g_Num ;

    printf("GPIO_SetReg 0x%08x 0x%02x\r\n",Addr,value);
    //himd(u64Addr, &value);  // 10_7 INPUT 10_6 outPUT
   /* if ((value & u32Value) != u32Value)
    {
        value &= 0xFFFFFCFF;
        value |= 0x00000100;*/
        himm(Addr, value);
   // }

}
HI_U32 GPIO_GetReg(HI_U64 u64Addr)
{
    HI_U32 value;
    himd(u64Addr, &value);  // 10_7 INPUT 10_6 outPUT
   return value;
}