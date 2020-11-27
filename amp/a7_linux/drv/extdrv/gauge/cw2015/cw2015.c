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
#define print_info(fmt, arg...)   \
    do{  \
        printk("fun:%s,%d "fmt, __func__,__LINE__, ##arg);  \
    }while(0)
//#define SENSITIVITY             _IOWR('w', 4, unsigned int)
#define GAUGE_VOLTGE        _IOWR('r', 5, unsigned int)
#define GAUGE_CAPACITY      _IOWR('r', 6, unsigned int)
#define GAUGE_CONFIG_Read   _IOWR('r', 7, unsigned int)
#define GAUGE_ChargeState   _IOWR('r', 8, unsigned int)
//#define GAUGE_CHARGING   _IOWR('r', 6, unsigned int)
#define CWFG_ENABLE_LOG 1 //CHANGE   Customer need to change this for enable/disable log
#define CWFG_I2C_BUSNUM 2 //CHANGE   Customer need to change this number according to the principle of hardware
#define DOUBLE_SERIES_BATTERY 0

#define REG_VERSION             0x0
#define REG_VCELL               0x2
#define REG_SOC                 0x4
#define REG_RRT_ALERT           0x6
#define REG_CONFIG              0x8
#define REG_MODE                0xA
#define REG_VTEMPL              0xC
#define REG_VTEMPH              0xD
#define REG_BATINFO             0x10


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
#define CWFG_NAME "cw2015"
#define SIZE_BATINFO    64

#define BAT_MODEL 1 /*0: 1200 1:3450*/

#if (BAT_MODEL==0)
static unsigned char config_info[SIZE_BATINFO] = {
0x15  ,0x7E  ,0x7E  ,0x56  ,0x43  ,0x2C  ,0x1F  ,
0x17  ,0x1F  ,0x32  ,0x44  ,0x47  ,0x44  ,0x42  ,
0x41  ,0x45  ,0x3E  ,0x36  ,0x38  ,0x42  ,0x4A  ,
0x57  ,0x5A  ,0x54  ,0x4E  ,0x4B  ,0x0E  ,0x15  ,
0x76  ,0xA6  ,0xA4  ,0xA4  ,0xA6  ,0xA5  ,0xA3  ,
0xA3  ,0x35  ,0x07  ,0x7A  ,0x27  ,0x09  ,0xA9  ,
0x14  ,0x4C  ,0x80  ,0x93  ,0x9D  ,0x33  ,0x54  ,
0x79  ,0x9E  ,0xC7  ,0x80  ,0xF6  ,0xDC  ,0xCB  ,
0x2F  ,0x00  ,0x64  ,0xA5  ,0xB5  ,0x16  ,0x68  ,
0x09
};
#else
static unsigned char config_info[SIZE_BATINFO] = {
0x15  ,0x7E  ,0x68  ,0x4B  ,0x2E  ,0x29  ,0x33  ,
0x45  ,0x4D  ,0x51  ,0x4E  ,0x4A  ,0x45  ,0x42  ,
0x44  ,0x47  ,0x45  ,0x4C  ,0x51  ,0x4B  ,0x49  ,
0x45  ,0x42  ,0x3B  ,0x3A  ,0x37  ,0x0E  ,0x15  ,
0x3B  ,0x5B  ,0x5A  ,0x58  ,0x58  ,0x57  ,0x56  ,
0x55  ,0x2B  ,0x04  ,0xAE  ,0x20  ,0x07  ,0x70  ,
0x22  ,0x5A  ,0x85  ,0x93  ,0x9A  ,0x46  ,0x60  ,
0x84  ,0x98  ,0xA3  ,0x80  ,0xFF  ,0xFF  ,0xCB  ,
0x2F  ,0x00  ,0x64  ,0xA5  ,0xB5  ,0x16  ,0x68  ,
0x11
};
#endif

int charger_mode;
int capacity;
int voltage;
int status;
int change;
int g_cw2015_charger_mode = 0;
int g_cw2015_capacity = 0;
int g_cw2015_vol = 0;
//int byVCheangCheck = 0;
//static unsigned int  reg_virtual_addr= 0;
//static unsigned int  reg_virtual_addr2= 0;
#define GPIO_CW2015_REG_READ(addr,ret)                   (ret =(*(volatile unsigned int *)(addr)))
#define GPIO_CW2015_REG_WRITE(addr,value)   			 ((*(volatile unsigned int *)(addr)) = (value))

static int cw_get_voltage(void);
static int cw_get_capacity(void);	

/*CW2015 update profile function, Often called during initialization*/
int cw_update_config_info(void)
{
    int ret;
    unsigned char reg_val;
    int i;
    unsigned char reset_val;

    print_info("\n");
    print_info("[FGADC] test config_info = 0x%x\n",config_info[0]);

    // make sure no in sleep mode
    ret = hi_i2c_read(REG_MODE,1, &reg_val,1);
    if(ret < 0) {
        return ret;
    }

    reset_val = reg_val;
    if((reg_val & MODE_SLEEP_MASK) == MODE_SLEEP) {
        return -1;
    }

    // update new battery info
    for (i = 0; i < SIZE_BATINFO; i++) {

        ret = hi_i2c_write(REG_BATINFO + i, 1,config_info[i],1);
        if(ret < 0) 
			return ret;
    }

	reg_val = 0x00;
    reg_val |= CONFIG_UPDATE_FLG;   // set UPDATE_FLAG
    reg_val &= 0x07;                // clear ATHD
    reg_val |= ATHD;                // set ATHD
    ret = hi_i2c_write(REG_CONFIG,1,(unsigned int)reg_val,1);
    if(ret < 0) 
		return ret;

	msleep(50);
    // reset
    reg_val = 0x00;
    reset_val &= ~(MODE_RESTART);
    reg_val = reset_val | MODE_RESTART;
    
    ret = hi_i2c_write(REG_MODE,1,(unsigned int)reg_val,1);
    if(ret < 0) return ret;

    msleep(10);
    
    ret =hi_i2c_write(REG_MODE,1,(unsigned int)reg_val,1);
    if(ret < 0) return ret;
	
    msleep(100);
	print_info("cw2015 update config success!\n");
	
    return 0;
}
/*CW2015 init function, Often called during initialization*/
static int cw_init(void)
{
    int ret;
    int i;
    //unsigned int data=0;
    unsigned char reg_val = MODE_SLEEP;

    hi_gpio_groupbit_info stIntInfo;
    stIntInfo.group_num = 5;
    stIntInfo.bit_num = 7;
    stIntInfo.value = 0;

    hi_gpio_set_dir(&stIntInfo);

    stIntInfo.group_num = 2; //himm 0x120D2400 0x0    himm 0x120D2008
    stIntInfo.bit_num = 1;
    stIntInfo.value = 0;
    hi_gpio_set_dir(&stIntInfo);

    

	dev_err(NULL,"!!!!!!!!! cw_init\n");
    if ((reg_val & MODE_SLEEP_MASK) == MODE_SLEEP) {
        reg_val = MODE_NORMAL;
        ret =  hi_i2c_write(REG_MODE, 1, (unsigned int)reg_val, 1);
        if (ret < 0) 
        {

	        dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!cw_init hi_i2c_write fail \n");
            return ret;
        }
    }
    ret = hi_i2c_read(REG_CONFIG, 1, &reg_val, 1);
    if (ret < 0)
    {
	    dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!cw_init hi_i2c_read fail \n");
    	return ret;
    }

    if ((reg_val & 0xf8) != ATHD) {
        reg_val &= 0x07;    /* clear ATHD */
        reg_val |= ATHD;    /* set ATHD */
        ret = hi_i2c_write(REG_CONFIG, 1, (unsigned int)reg_val, 1);
        if (ret < 0)
        {
	        dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!cw_init hi_i2c_write REG_CONFIG fail \n");
            return ret;
        }
    }
        
    ret = hi_i2c_read(REG_CONFIG,1, &reg_val, 1);
        if (ret < 0)
        {
	        dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!cw_init hi_i2c_read REG_CONFIG fail \n");
            return ret;
        }

    if (!(reg_val & CONFIG_UPDATE_FLG)) {
		print_info("update config flg is true, need update config\n");
        ret = cw_update_config_info();
        if (ret < 0) {
			print_info("%s : update config fail\n", __func__);
            return ret;
        }
    } else {
    	for(i = 0; i < SIZE_BATINFO; i++) { 
	        ret = hi_i2c_read((REG_BATINFO + i),1, &reg_val, 1);
	        if (ret < 0)
            {
	            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!cw_init hi_i2c_read REG_BATINFO fail \n");
	        	return ret;
             }
	        
	        if (config_info[i] != reg_val)
            {
	            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!cw_init config_info(%02X) != reg_val(%02X) fail \n",config_info[i],reg_val);
	            break;
            }
        }
        if (i != SIZE_BATINFO) {
			print_info("config didn't match, need update config\n");
        	ret = cw_update_config_info();
            if (ret < 0){
	            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!cw_init cw_update_config_info  fail \n");
                return ret;
            }
        }
    }
	
	msleep(10);
    for (i = 0; i < 30; i++) {
        ret = hi_i2c_read(REG_SOC,1, &reg_val, 1);
            if (ret < 0){
	            dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!cw_init hi_i2c_read REG_SOC fail \n");
                return ret;
            }
        else if (reg_val <= 0x64) 
            break;
        msleep(120);
    }
	
    if (i >= 30 ){
    	 reg_val = MODE_SLEEP;
         ret = hi_i2c_write(REG_MODE,1,(unsigned int)reg_val,1);
         print_info("cw2015 input unvalid power error, cw2015 join sleep mode\n");
         return -1;
    } 

	print_info("cw2015 init success!\n");	
    return 0;
}
/*This function called when get voltage from cw2015*/
static int cw_get_voltage(void)
{    
    int ret;
    unsigned char reg_val[2];
    u16 value16, value16_1, value16_2, value16_3;
    int voltage;
    
    ret = hi_i2c_read(REG_VCELL,1,reg_val,2);
    if(ret < 0) {
        return ret;
    }
    value16 = (reg_val[0] << 8) + reg_val[1];

    ret = hi_i2c_read(REG_VCELL,1,reg_val,2);
    if(ret < 0) {
          return ret;
    }
    value16_1 = (reg_val[0] << 8) + reg_val[1];

    ret = hi_i2c_read(REG_VCELL,1,reg_val,2);
    if(ret < 0) {
        return ret;
    }
    value16_2 = (reg_val[0] << 8) + reg_val[1];

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

    voltage = value16_1 * 625 / 2048;

    //dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!W2015 cw_get_voltage %d \n", voltage);
	//if(DOUBLE_SERIES_BATTERY)
	//	voltage = voltage * 2;
    return voltage;
}

static void cw_update_vol(void)
{
    int ret;
    ret = cw_get_voltage();
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
   // BAT_STAT  I2C2_SCL/GPIO5_7
   
    //GPIO_CW2015_REG_READ(reg_virtual_addr2,data); //GPIO5_7
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
    //int cwVol=0;
    int cwcap=0;
    // cwVol=cw_get_voltage();
     cwcap=cw_get_capacity();
	//dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!get_charge_state =%d \n",IO_data.value);

    
    //    dev_err(NULL,"IO_data.value = %d cwVol =%d  g_cw2015_vol =%d\n",IO_data.value,cwVol,g_cw2015_vol);
    if(IO_data.value==0)
    {
        // g_cw2015_vol=cwVol;
        if(charger_mode != CHARGING_ON)
        {
            dev_err(NULL,"by charge_state %d\n",IO_data.value);
        }
        return CHARGING_ON;
    }
    else if((USB_data.value==1)&&(cwcap<100))
    {
        // g_cw2015_vol=cwVol;
        if(charger_mode != CHARGING_ON)
        {
            dev_err(NULL,"by USBstate %d\n",USB_data.value);
            
        }
        return CHARGING_ON;
    }
  /*  else if((g_cw2015_vol+100<cwVol)&&(cwcap<100))
    {
        byVCheangCheck=1;
        g_cw2015_vol=cwVol;
        return CHARGING_ON;
    }
    else if((byVCheangCheck)&&(cwcap<100))
    {
        if((g_cw2015_vol-100>cwVol))
        {
            byVCheangCheck=0;
            return NO_CHARGING;
        }
        else
        {
            g_cw2015_vol=cwVol;
            return CHARGING_ON;
        }
        
    }   */ 
    else 
    {
      //  byVCheangCheck=0;
      //  g_cw2015_vol=cwVol;
        return NO_CHARGING;
    }
   
        
}
static void cw_update_charge_status(void)
{
	int cw_charger_mode;
	cw_charger_mode = get_charge_state();
	if(charger_mode != cw_charger_mode){
        charger_mode = cw_charger_mode;
	
	}
}
/*static void cw_update_status(void)
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
static int cw_init_data(void)
{
	unsigned char reg_SOC[2];
	int real_SOC = 0;
	int digit_SOC = 0;
	int ret;
	int UI_SOC = 0;
	ret = hi_i2c_read(REG_SOC,1,reg_SOC,2);
	//dev_err(NULL,"!!!!!!!!!!!!!!!!!!!!cw_init_data ret value = %d \n",ret);
	if (ret < 0)
		return ret;

	real_SOC = reg_SOC[0];
	digit_SOC = reg_SOC[1];
	UI_SOC = ((real_SOC * 256 + digit_SOC) * 100)/ (UI_FULL*256);
	//dev_err(NULL,"real_SOC = %d, digit_SOC = %d\n",real_SOC, digit_SOC);
	
	
	
	cw_update_vol();
	cw_update_charge_status();
	capacity = UI_SOC;
    	cw_update_status();
	return 0;
}*/
static int cw_por(void)
{
	int ret;
	unsigned int reset_val;
	
	reset_val = MODE_SLEEP; 
    			  
	ret = hi_i2c_write(REG_MODE,1,reset_val,1);
	if (ret < 0)
		return ret;
	reset_val = MODE_NORMAL;
	msleep(10);
	ret = hi_i2c_write(REG_MODE,1,reset_val,1);
	if (ret < 0)
		return ret;
	ret = cw_init();
	if (ret) 
		return ret;
	return 0;
}
static int cw_get_capacity(void)
{
	int ui_100 = UI_FULL;
	int remainder = 0;
	int real_SOC = 0;
	int digit_SOC = 0;
	int UI_SOC = 0;
	//int cw_capacity;
	int ret;
	unsigned char reg_val[2];
	//unsigned char reg_0x4f;
	//unsigned char temp_value;
	static int reset_loop = 0;
	static int charging_zero_loop = 0;
	
//	ret = hi_i2c_write(REG_SOC,1,reg_val,2);
    ret = hi_i2c_read(REG_SOC,1,reg_val,2);
	if (ret < 0)
    {
        
	dev_err(NULL,"hi_i2c_read error \n");
		return ret;
}

	/*
	ret = cw_read(client, REG_LAST_FULL_CHARGE, &reg_0x4f);
	if (ret < 0)
		return ret;
	*/


	real_SOC = reg_val[0];
	digit_SOC = reg_val[1];
	//print_info("CW2015[%d]: real_SOC = %d, digit_SOC = %d\n", __LINE__, real_SOC, digit_SOC);
	//dev_err(NULL,"CW2015[%d]: real_SOC = %d, digit_SOC = %d\n", __LINE__, real_SOC, digit_SOC);
	
	/*case 1 : avoid IC error, read SOC > 100*/
	if ((real_SOC < 0) || (real_SOC > 100)) {
		print_info("Error:  real_SOC = %d\n", real_SOC);
		reset_loop++;			
		if (reset_loop > (BATTERY_CAPACITY_ERROR / queue_delayed_work_time)){ 
			cw_por();
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
			cw_por();
			charging_zero_loop = 0;
		}
	}else if(charging_zero_loop != 0){
		charging_zero_loop = 0;
	}

	
	UI_SOC = ((real_SOC * 256 + digit_SOC) * 100)/ (ui_100*256);
	remainder = (((real_SOC * 256 + digit_SOC) * 100 * 100) / (ui_100*256)) % 100;
	//dev_err(NULL,"CW2015[%d]: ui_100 = %d, UI_SOC = %d, remainder = %d\n", __LINE__, ui_100, UI_SOC, remainder);
	/*case 3 : aviod swing*/
	if(UI_SOC >= 100){
	//	dev_err(NULL,"CW2015[%d]: UI_SOC = %d larger 100!!!!\n", __LINE__, UI_SOC);
		UI_SOC = 100;
	}else{
		if((remainder > 70 || remainder < 30) && UI_SOC >= capacity - 1 && UI_SOC <= capacity + 1)
		{
			UI_SOC = capacity;
	//		dev_err(NULL,"CW2015[%d]: UI_SOC = %d, capacity = %d\n", __LINE__, UI_SOC, capacity);
		}
	}

	return UI_SOC;
}


static void cw_update_capacity(void)
{
    int cw_capacity;
    cw_capacity = cw_get_capacity();

    if ((cw_capacity >= 0) && (cw_capacity <= 100) && (capacity != cw_capacity)) {
      /*  if(cw_capacity>capacity)
        {
		    change = 1;
        }*/				
        capacity = cw_capacity;
    }
    cw_update_charge_status();
}
/*
static void cw_bat_work(void)
{
    int ret;
    unsigned char reg_val;
    //u16 value16;
	int i = 0;
    
    ret = hi_i2c_read(REG_MODE,1, &reg_val,1);
	if(ret < 0){
		//battery is out , you can send new battery capacity vol here what you want set
		//for example
		capacity = 100;
		voltage = 4200;
		change = 1;
	}else{
		if((reg_val & MODE_SLEEP_MASK) == MODE_SLEEP){
			for(i = 0; i < 5; i++){
				if(cw_por() == 0)
					break;
			}
		}
		cw_update_vol();
		cw_update_charge_status();
		cw_update_capacity();
		cw_update_status();
	}
	print_info("charger_mod = %d\n", charger_mode);
	print_info("status = %d\n", status);
	print_info("capacity = %d\n", capacity);
	print_info("voltage = %d\n", voltage);

	
	g_cw2015_capacity = capacity;
    g_cw2015_vol = voltage;
	
	//queue_delayed_work(cwfg_workqueue, &battery_delay_work, msecs_to_jiffies(queue_delayed_work_time));
}*/

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
        case _IOC_NR(GAUGE_VOLTGE):
        
                cw_update_vol();   
                value= voltage;           
                ret = copy_to_user((unsigned int*)arg,&value,  sizeof(unsigned int));
            break;
        case _IOC_NR(GAUGE_CAPACITY):
                cw_update_capacity(); 
                value= capacity;

                if(capacity<10)
                {
                    cw_update_vol(); 
                    dev_err(NULL,"GAUGE_CAPACITY = %d  %d   %d\n",value,charger_mode,voltage); 
                }  
                else if(g_cw2015_capacity!=   capacity)
                {
                    dev_err(NULL,"GAUGE_CAPACITY = %d  %d   %d\n",value,charger_mode,voltage); 
                }         
                g_cw2015_capacity=   capacity;    
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
                if(g_cw2015_charger_mode!=charger_mode)
                {
                    dev_err(NULL,"GAUGE_ChargeState = %d \n",value); 
                    g_cw2015_charger_mode=charger_mode;
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
    ret = cw_init();
    if (ret) {
         dev_err(NULL, " cw2015 init fail!\n");
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
MODULE_DESCRIPTION("Gauge cw2015 driver");
MODULE_LICENSE("GPL");
#endif
