/*
 * FocalTech FT6236 TouchScreen driver.
 *
 * Copyright (c) 2010  Focal tech Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/property.h>
#include <asm/irq.h>


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
//#include <mach/io.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/i2c.h>
#include "i2cdev.h"
#include "hi_gpio.h"
#include "MAX17201.h"
#define print_info(fmt, arg...)   \
    do{  \
        printk("fun:%s,%d "fmt, __func__,__LINE__, ##arg);  \
    }while(0)
//#define SENSITIVITY             _IOWR('w', 4, unsigned int)
#define GAUGE_VOLTGE        _IOWR('r', 5, unsigned int)
#define GAUGE_CAPACITY      _IOWR('r', 6, unsigned int)
#define GAUGE_CONFIG_Read   _IOWR('r', 7, unsigned int)
#define GAUGE_ChargeState   _IOWR('r', 8, unsigned int)
#define GAUGE_CURRENT       _IOWR('r', 9, unsigned int)
#define GAUGE_OPEN_VOLTGE   _IOWR('r',10, unsigned int)
#define GAUGE_FULL_TIME     _IOWR('r',11, unsigned int)
#define GAUGE_EMPTF_TIME    _IOWR('r',12, unsigned int)

//#define GAUGE_CHARGING   _IOWR('r', 6, unsigned int)
#define CWFG_ENABLE_LOG 1 //CHANGE   Customer need to change this for enable/disable log
#define CWFG_I2C_BUSNUM 2 //CHANGE   Customer need to change this number according to the principle of hardware
#define DOUBLE_SERIES_BATTERY 0


#define MODE_SLEEP_MASK         (0x3<<6)
#define MODE_SLEEP              (0x3<<6)
#define MODE_NORMAL             (0x0<<6)
#define MODE_QUICK_START        (0x3<<4)
#define MODE_RESTART            (0xf<<0)

#define CONFIG_UPDATE_FLG       (0x1<<1)
#define ATHD                    (0x0<<3)        // ATHD = 0%

#define queue_delayed_work_time  8000
#define BATTERY_CAPACITY_ERROR  40*1000
#define BATTERY_CHARGING_ZERO   1800*1000

#define UI_FULL 100
#define DECIMAL_MAX 70
#define DECIMAL_MIN 30

#define CHARGING_ON 1
#define NO_CHARGING 0
#define CWFG_NAME "MAX17201"
#define SIZE_BATINFO    64

#define BAT_MODEL 1 /*0: 1200 1:3450*/



int charger_mode;
int capacity;
int voltage;
int status;
int change;
int bat_Current;
int g_MAX17201_charger_mode = 0;
int g_MAX17201_capacity = 0;
int g_MAX17201_vol = 0;
//int byVCheangCheck = 0;
//static unsigned int  reg_virtual_addr= 0;
//static unsigned int  reg_virtual_addr2= 0;
#define GPIO_MAX17201_REG_READ(addr,ret)                   (ret =(*(volatile unsigned int *)(addr)))
#define GPIO_MAX17201_REG_WRITE(addr,value)   			 ((*(volatile unsigned int *)(addr)) = (value))

static int Maxim_get_voltage(unsigned int Vreg);
static int Maxim_get_capacity(void);
static void Maxim_update_vol(void);
static void Maxim_update_capacity(void);
static void Maxim_update_Current(void);
 unsigned int  Maxim_Voltage( unsigned char M_val_H, unsigned char M_val_L )  //mV
{
	unsigned int outdata;
	unsigned int checkdata=((M_val_H*256)+M_val_L);

	outdata=checkdata;

    return outdata;
}

unsigned int  Maxim_Percentage( unsigned char M_val_H, unsigned char M_val_L,unsigned char Decimal)  //1*(0.1^Decimal)%  Decimal 0~2
{
	unsigned int outdata;
    unsigned int u16Decimal=1;
	unsigned int checkdata=(M_val_H*256)+M_val_L;
    int i;
	for(i=0;i<Decimal;i++)
	{
		u16Decimal*=10;
	}
	checkdata/=256;
	checkdata*=u16Decimal;	
	
	outdata=checkdata;
/*	if(((checkdata-outdata)*10)>=5)
	{
		outdata++;
	}*/
    return outdata;
}

/*MAX17201 init function, Often called during initialization*/
static int check_i2c_write(unsigned int reg_addr,unsigned int reg_addr_num, unsigned int data, unsigned int data_byte_num,unsigned check_falg)
{
    int ret=1;
    unsigned char reg_val[2]={0,0};
    unsigned int checkdata=0;
      ret = hi_i2c_write(reg_addr,reg_addr_num,data,data_byte_num);
            if (ret < 0){
	            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!1.Maxim_init hi_i2c_write 0x%04x => 0x%04x fail \n",reg_addr,data);
                return 1;
            }
            if(check_falg)
            {
                ret = hi_i2c_read(reg_addr,reg_addr_num, reg_val, data_byte_num);
                if (ret < 0){
                    dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!Maxim_init hi_i2c_read Status fail \n");
                    return 1;
                }
            // dev_err(NULL,"checkdata 0x%04x => reg_val[1]=0x%04x reg_val=0x%04x\n",reg_addr,data,checkdata);
                checkdata=(((reg_val[1]<<8)&0xFF00)+reg_val[0]);

                if(checkdata!=data)
                {
                    dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!Maxim_init Set fail 0x%04x => 0x%04x <> 0x%04x\n",reg_addr,data,checkdata);
                    return 1;
                }
            }
     return 0;
}

static int Maxim_init(void)
{
    int ret;
    int ui;
    unsigned int checkdata=0;
    unsigned char reg_val[2];

    hi_gpio_groupbit_info stIntInfo;
    stIntInfo.group_num = 5;
    stIntInfo.bit_num = 7;
    stIntInfo.value = 0;

    hi_gpio_set_dir(&stIntInfo);

    stIntInfo.group_num = 2; //himm 0x120D2400 0x0    himm 0x120D2008
    stIntInfo.bit_num = 1;
    stIntInfo.value = 0;
    hi_gpio_set_dir(&stIntInfo);

    
    //6c =>0 16=>1

	dev_err(NULL,"!!!!!!!!! Maxim_init\n");
    ret = hi_i2c_read(Status,1, reg_val, 2);
     if (ret < 0){
        dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!Maxim_init hi_i2c_read Status fail \n");
        return ret;
    }
    if((reg_val[0]&0x02)==0x02) //POR check
    {
        //step 1: write model to memory		
        for( ui=0;ui<MAX17201_Set_len;ui++)
        {
           ret = check_i2c_write(MAX17201_Set[ui][0],1,MAX17201_Set[ui][1],2,MAX17201_Set[ui][2]);
            if (ret >0){
	            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!2.Maxim_init hi_i2c_write 0x%04x => 0x%04x fail \n",MAX17201_Set[ui][0],MAX17201_Set[ui][1]);
                return ret;
            }
        }
	
	//copy model to max17205	
    
        ret = check_i2c_write(Read_CommStat,1,0x0000,2,1);	
        if (ret < 0){
            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!copy model to max17205 \n");
            return ret;
        }

        ret = check_i2c_write(MAX1720x_COMMAND,1,0xE904,2,1);	
        if (ret < 0){
            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!copy model to max17205 \n");
            return ret;
        }	
	    //delay 3 seconds
        mdelay(3000);
       /* ret = check_i2c_write(0x062,1,0x0059,2,1);	
        if (ret < 0){
            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!copy model to max17205 \n");
            return ret;
        }	
		 ret = check_i2c_write(0x063,1,0x00C4,2,1);	
        if (ret < 0){
            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!copy model to max17205 \n");
            return ret;
        }	
		 ret = check_i2c_write(0x080,1,0x0000,2,1);	
        if (ret < 0){
            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!copy model to max17205 \n");
            return ret;
        }	
        
        */
       ret = check_i2c_write(MAX1720x_COMMAND,1,0x000f,2,1);	
        if (ret < 0){
            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!copy model to max17205 \n");
            return ret;
        }	
	    //delay 2 seconds	
        mdelay(2000);	
       ret = check_i2c_write(MAX1720x_COMMAND,1,0xe001,2,1);	
        if (ret < 0){
            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!copy model to max17205 \n");
            return ret;
        }	
				
	    //verify memory again	
    		
        ret = check_i2c_write(verify_memory[0],1,verify_memory[1],2,verify_memory[2]);
        if (ret >0){
            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!verify memory fail 0x%04x => 0x%04x fail \n",verify_memory[0],verify_memory[1]);
            return ret;
        }
	
	    //delay 300ms	
        mdelay(300);	
		
         ret = hi_i2c_read(Status,1, reg_val, 2);
        if (ret < 0){
            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!Maxim_init hi_i2c_read Status fail \n");
            return ret;
        }

        reg_val[0]&=0xFD;
        checkdata=((reg_val[1]<<8)&0xFF00)+reg_val[0];

        ret = check_i2c_write(Status,1,checkdata,2,1);
        if (ret >0){
            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!3.Maxim_init hi_i2c_write 0x%04x => 0x%04x fail \n",Status,checkdata);
                return ret;
        }
       
        ret = hi_i2c_read(Status,1, reg_val, 2);
        if (ret < 0){
            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!Maxim_init hi_i2c_read Status fail \n");
            return ret;
        }
        if((reg_val[0]&0x02)!=0x00) //POR check
        {
            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!Maxim_init hi_i2c_read Status fail \n");
            return ret;
        }

    }

    Maxim_update_vol();
    Maxim_update_capacity();
    Maxim_update_Current();
	dev_err(NULL,"MAX17201 init success!\n");
	dev_err(NULL,"voltage =%d capacity=%d change=%d Current=0x%04x\n",voltage,capacity,change,bat_Current);
	print_info("MAX17201 init success!\n");

    return 0;
}
static int Maxim_get_time(unsigned int Vreg)
{    
    int ret;
    unsigned char reg_val[2];
    u16 value_time;
    int voltage;
    

    ret = hi_i2c_read(Vreg,1,reg_val,2);
    if(ret < 0) {
          return ret;
    }
    value_time = ((reg_val[1]*256)+reg_val[0]);


    voltage = value_time * 45 / 8;

    //dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!MAX17201 Maxim_get_voltage %d \n", voltage);
	//if(DOUBLE_SERIES_BATTERY)
	//	voltage = voltage * 2;
    return voltage;
}

/*This function called when get voltage from MAX17201*/
static int Maxim_get_voltage(unsigned int Vreg)
{    
    int ret;
    unsigned char reg_val[2];
    u16  value16_1;
   // u16 value16,value16_2, value16_3;
    int voltage;
    
 /*   ret = hi_i2c_read(Vreg,1,reg_val,2);
    if(ret < 0) {
        return ret;
    }
    value16 = Maxim_Voltage(reg_val[1],reg_val[0]);*/

    ret = hi_i2c_read(Vreg,1,reg_val,2);
    if(ret < 0) {
          return ret;
    }
    value16_1 = Maxim_Voltage(reg_val[1],reg_val[0]);

 /*   ret = hi_i2c_read(Vreg,1,reg_val,2);
    if(ret < 0) {
        return ret;
    }
    value16_2 = Maxim_Voltage(reg_val[1],reg_val[0]);*/
/*
    if(value16 > value16_1) {     
        value16_3 = value16;
        value16 = value16_1;
        value16_1 = value16_3;
    }

    if(value16_1 > value16_2) {
    value16_3 =value16_1;
    value16_1 =value16_2;
    value16_2 =value16_3;
    }

    if(value16 >value16_1) {     
    value16_3 =value16;
    value16 =value16_1;
    value16_1 =value16_3;
    }            
*/

    voltage = value16_1 * 5 / 64;

    //dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!MAX17201 Maxim_get_voltage %d \n", voltage);
	//if(DOUBLE_SERIES_BATTERY)
	//	voltage = voltage * 2;
    return voltage;
}

static void Maxim_update_vol(void)
{
    int ret;
    ret = Maxim_get_voltage(VCell);
    if ((ret >= 0) && (voltage != ret)) {
        voltage = ret;
		change = 1;
    }
}
static int get_charge_state(void)
{
    
    //unsigned int data=0;
    hi_gpio_groupbit_info IO_data;
    hi_gpio_groupbit_info USB_data;
    //int cwVol=0;
    int cwcap=0;
   // BAT_STAT  I2C2_SCL/GPIO5_7
   
    //GPIO_MAX17201_REG_READ(reg_virtual_addr2,data); //GPIO5_7
    //himm 0x120D5200
    //himm 0x120D5400
    IO_data.group_num=5;
    IO_data.bit_num=7;
    IO_data.value=0;
    hi_gpio_read_bit(&IO_data);

    
    //himm 0x120D2400
    //himm 0x120D2008
    USB_data.group_num=2;
    USB_data.bit_num=1;
    USB_data.value=0;
    hi_gpio_read_bit(&USB_data);
     cwcap=Maxim_get_capacity();
	//dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!get_charge_state =%d \n",IO_data.value);

    
    //   dev_err(NULL,"IO_data.value = %d  g_MAX17201_vol =%d\n",IO_data.value,g_MAX17201_vol);
    if(IO_data.value==0)
    {
        // g_MAX17201_vol=cwVol;
        if(charger_mode != CHARGING_ON)
        {
            dev_err(NULL,"by charge_state %d\n",IO_data.value);
        }
        return CHARGING_ON;
    }
/*   else if((USB_data.value==1)&&(cwcap<100))
    {
        // g_MAX17201_vol=cwVol;
        if(charger_mode != CHARGING_ON)
        {
            dev_err(NULL,"by USBstate %d\n",USB_data.value);
            
        }
        return CHARGING_ON;
    }*/
  /*  else if((g_MAX17201_vol+100<cwVol)&&(cwcap<100))
    {
        byVCheangCheck=1;
        g_MAX17201_vol=cwVol;
        return CHARGING_ON;
    }
    else if((byVCheangCheck)&&(cwcap<100))
    {
        if((g_MAX17201_vol-100>cwVol))
        {
            byVCheangCheck=0;
            return NO_CHARGING;
        }
        else
        {
            g_MAX17201_vol=cwVol;
            return CHARGING_ON;
        }
        
    }   */ 
    else 
    {
      //  byVCheangCheck=0;
      //  g_MAX17201_vol=cwVol;
        return NO_CHARGING;
    }
   
        
}
static void Maxim_update_charge_status(void)
{
	int Maxim_charger_mode;
	Maxim_charger_mode = get_charge_state();
	if(charger_mode != Maxim_charger_mode){
        charger_mode = Maxim_charger_mode;
	
	}
}
/*static void Maxim_update_status(void)
{
    int status_ne;

    if (charger_mode > 0) {
        if (capacity >= 100) 
            status_ne = 0;
        else
            status_ne = 0;
    } else {
        status_ne = 0;
    }

    if (status != status_ne) {
        status = status_ne;
		change = 1;
    } 
}*/
/*
static int Maxim_init_data(void)
{
	unsigned char reg_SOC[2];
	int real_SOC = 0;
	int digit_SOC = 0;
	int ret;
	int UI_SOC = 0;
	ret = hi_i2c_read(RepSOC,1,reg_SOC,2);
	//dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!Maxim_init_data ret value = %d \n",ret);
	if (ret < 0)
		return ret;

	real_SOC = reg_SOC[0];
	digit_SOC = reg_SOC[1];
	UI_SOC = ((real_SOC * 256 + digit_SOC) * 100)/ (UI_FULL*256);
	//dev_err(NULL,"real_SOC = %d, digit_SOC = %d\n",real_SOC, digit_SOC);
	
	
	
	Maxim_update_vol();
	Maxim_update_charge_status();
	capacity = UI_SOC;
    	Maxim_update_status();
	return 0;
}*/
static int Maxim_por(void)
{
	int ret=0;
//	unsigned int reset_val;
	
//	reset_val = MODE_SLEEP; 

	ret = Maxim_init();
	if (ret) 
		return ret;
	return 0;
}
static int Maxim_get_capacity(void)
{
	int ui_100 = UI_FULL;
	int remainder = 0;
	int real_SOC = 0;
	int digit_SOC = 0;
	int UI_SOC = 0;
	//int Maxim_capacity;
	int ret;
	unsigned char reg_val[2];
	//unsigned char reg_0x4f;
	//unsigned char temp_value;
	static int reset_loop = 0;
	static int charging_zero_loop = 0;
	
    ret = hi_i2c_read(RepSOC,1,reg_val,2);
	if (ret < 0)
    {
	dev_err(NULL,"hi_i2c_read error \n");
		return ret;
    }

	real_SOC = reg_val[1];
	digit_SOC = reg_val[0];
	//print_info("MAX17201[%d]: real_SOC = %d, digit_SOC = %d\n", __LINE__, real_SOC, digit_SOC);
	//dev_err(NULL,"MAX17201[%d]: real_SOC = %d, digit_SOC = %d\n", __LINE__, real_SOC, digit_SOC);
	
	/*case 1 : avoid IC error, read SOC > 100*/
	if ((real_SOC < 0) || (real_SOC > 100)) {
		print_info("Error:  real_SOC = %d\n", real_SOC);
		reset_loop++;			
		if (reset_loop > (BATTERY_CAPACITY_ERROR / queue_delayed_work_time)){ 
			Maxim_por();
			reset_loop =0;							 
		}
								 
		return capacity; 
	}else {
		reset_loop =0;
	}

	/*case 2 : avoid IC error, battery SOC is 0% when long time charging*/
	if((charger_mode > 0) &&(real_SOC == 0))
	{
		charging_zero_loop++;
		if (charging_zero_loop > BATTERY_CHARGING_ZERO / queue_delayed_work_time) {
			Maxim_por();
			charging_zero_loop = 0;
		}
	}else if(charging_zero_loop != 0){
		charging_zero_loop = 0;
	}

	
	UI_SOC = ((real_SOC * 256 + digit_SOC) * 100)/ (ui_100*256);
	remainder = (((real_SOC * 256 + digit_SOC) * 100 * 100) / (ui_100*256)) % 100;
	//dev_err(NULL,"MAX17201[%d]: ui_100 = %d, UI_SOC = %d, remainder = %d\n", __LINE__, ui_100, UI_SOC, remainder);
	/*case 3 : aviod swing*/
	if(UI_SOC >= 100){
	//	dev_err(NULL,"MAX17201[%d]: UI_SOC = %d larger 100!!!!\n", __LINE__, UI_SOC);
		UI_SOC = 100;
	}else{
		if((remainder > 70 || remainder < 30) && UI_SOC >= capacity - 1 && UI_SOC <= capacity + 1)
		{
			UI_SOC = capacity;
	//		dev_err(NULL,"MAX17201[%d]: UI_SOC = %d, capacity = %d\n", __LINE__, UI_SOC, capacity);
		}
	}

	return UI_SOC;
}


static void Maxim_update_capacity(void)
{
    int Maxim_capacity;
    Maxim_capacity = Maxim_get_capacity();

    if ((Maxim_capacity >= 0) && (Maxim_capacity <= 100) && (capacity != Maxim_capacity)) {
      /*  if(Maxim_capacity>capacity)
        {
		    change = 1;
        }*/				
        capacity = Maxim_capacity;
    }
    Maxim_update_charge_status();
}

static int Maxim_get_Current(void)
{
	//int Maxim_capacity;
	int ret;
	unsigned char reg_val[2];
	unsigned int checkdata;
    ret = hi_i2c_read(AvgCurrent,1,reg_val,2);
        if (ret < 0)
        {
            dev_err(NULL,"hi_i2c_read error \n");
            return ret;
        }	
    checkdata=((reg_val[1]*256)+reg_val[0]);
	return checkdata;
}


static void Maxim_update_Current(void)
{
    int Maxim_Current;
    Maxim_Current = Maxim_get_Current();
    bat_Current = Maxim_Current;
    
}




#ifndef __LITEOS__
static long gauge_ioctl(struct file* filp, unsigned int cmd, unsigned long arg)
#else
static int gauge_ioctl(struct file* filp, int cmd, unsigned long arg)
#endif
{
    unsigned int value = 0;
    int ret = 0;
     switch (_IOC_NR(cmd))
    {        
        case _IOC_NR(GAUGE_OPEN_VOLTGE):    
                value= Maxim_get_voltage(Read_VFOCV);           
                ret = copy_to_user((unsigned int*)arg,&value,  sizeof(unsigned int));
            break;
        case _IOC_NR(GAUGE_FULL_TIME):    
                value= Maxim_get_time(TTF);           
                ret = copy_to_user((unsigned int*)arg,&value,  sizeof(unsigned int));
            break;
        case _IOC_NR(GAUGE_EMPTF_TIME):    
                value= Maxim_get_time(TTE);           
                ret = copy_to_user((unsigned int*)arg,&value,  sizeof(unsigned int));
            break;

        case _IOC_NR(GAUGE_VOLTGE):
        
                Maxim_update_vol();   
                value= voltage;           
                ret = copy_to_user((unsigned int*)arg,&value,  sizeof(unsigned int));
            break;
        case _IOC_NR(GAUGE_CURRENT):
        
                Maxim_update_Current();
                value= bat_Current;
                ret = copy_to_user((unsigned int*)arg,&value,  sizeof(unsigned int));
                 if (0 != ret)
                {
                    dev_err(NULL,"GAUGE_CURRENT copy_to_user error! %d\n",ret);
                }
            break;

        case _IOC_NR(GAUGE_CAPACITY):
                Maxim_update_capacity(); 
                value= capacity;

                if(capacity<10)
                {
                    Maxim_update_vol(); 
                    dev_err(NULL,"GAUGE_CAPACITY = %d  %d   %d\n",value,charger_mode,voltage); 
                }  
                else if(g_MAX17201_capacity!=   capacity)
                {
                    dev_err(NULL,"GAUGE_CAPACITY = %d  %d   %d\n",value,charger_mode,voltage); 
                }         
                g_MAX17201_capacity=   capacity;    
                ret = copy_to_user((unsigned int*)arg,&value,  sizeof(unsigned int));
                 if (0 != ret)
                {
                    dev_err(NULL,"GAUGE_CAPACITY copy_to_user error! %d\n",ret);
                }
            break;
        case _IOC_NR(GAUGE_CONFIG_Read):
            break;
        case _IOC_NR(GAUGE_ChargeState):
               /* if(capacity>=100)
                {
                    value= NO_CHARGING;     
                }
                else*/
                {
                    value= charger_mode;   
                }
                if(g_MAX17201_charger_mode!=charger_mode)
                {
                    dev_err(NULL,"GAUGE_ChargeState = %d \n",value); 
                    g_MAX17201_charger_mode=charger_mode;
                }
                  
                               
                ret = copy_to_user((unsigned int*)arg,&value,  sizeof(unsigned int));
                 if (0 != ret)
                {
                    dev_err(NULL,"GAUGE_ChargeState copy_to_user error! %d\n",ret);
                }
            break;
     
    }
    return 0;

}

#ifndef __LITEOS__
static int gauge_open(struct inode* inode, struct file* filp)
#else
static int gauge_open(struct file* filp)
#endif
{
    return 0;
}

#ifndef __LITEOS__
static int gauge_close(struct inode* inode, struct file* filp)
#else
static int gauge_close(struct file* filp)
#endif
{
    return 0;
}

#ifndef __LITEOS__
static struct file_operations gauge_fops = {
    .owner      = THIS_MODULE,
    .unlocked_ioctl = gauge_ioctl,
    .open       = gauge_open,
    .release    = gauge_close
};
static struct miscdevice gauge_dev = {
   .minor       = MISC_DYNAMIC_MINOR,
   .name        = "gauge",
   .fops        = &gauge_fops,
};
#else
static const struct file_operations_vfs gauge_fops =
{
    .open  = gauge_open,       /* open */
    .close = gauge_close,    /* close */
    .ioctl = gauge_ioctl,       /* ioctl */
};
#endif
/*start define of ft*/
static int __init gauge_init(void)
{
    int ret = 0;
    dev_err(NULL,"gauge_init\n");
    ret = i2cdev_init();
    if (ret)
    {
        dev_err(NULL, " i2cdev_init fail!\n");
        goto error_end;
    }
    capacity = 1;
    voltage = 0;
    status = 0;
	charger_mode = NO_CHARGING;
	change = 0;
    ret = Maxim_init();
    if (ret) {
         dev_err(NULL, " MAX17201 init fail!\n");
        return -1;	
    }
#ifndef __LITEOS__
    ret = misc_register(&gauge_dev);
#else
    ret = register_driver("/dev/gauge", &gauge_fops, 0666, 0);
#endif
    if(ret<0)
    {
        print_info("gauge_dev_register fail!\n");
        return -1;
    }
    else
    {
        print_info("gauge_dev_register ok!\n");
    }
    return 0;
error_end:
    return -1;
}

static void __exit gauge_exit(void)
{
    i2cdev_exit();
}

module_init(gauge_init);
module_exit(gauge_exit);

#ifndef __HuaweiLite__
MODULE_AUTHOR("monster");
MODULE_DESCRIPTION("Gauge MAX17201 driver");
MODULE_LICENSE("GPL");
#endif
