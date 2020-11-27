/*  extdrv/interface/ssp/hi_ssp.c
 *
 * Copyright (c) 2006 Hisilicon Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 * History:
 *      21-April-2006 create this file
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/poll.h>

#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>

#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>

#include "hi_ssp_ili9342.h"


#ifdef __HuaweiLite__
#include "asm/io.h"
#endif


#ifdef __HuaweiLite__
#else
#include <mach/io.h>/* for IO_ADDRESS */
#endif

#define SSP_BASE    0x120c2000

#define SSP_SIZE    0x1000          // 4KB
#define DEFAULT_MD_LEN (128)

#ifdef __HuaweiLite__
#define IO_ADDRESS_VERIFY(x) (x)
#else
void __iomem *reg_ssp_base_va;
#define IO_ADDRESS_VERIFY(x) (reg_ssp_base_va + ((x)-(SSP_BASE)))
#endif

/* SSP register definition .*/
#define SSP_CR0              IO_ADDRESS_VERIFY(SSP_BASE + 0x00)
#define SSP_CR1              IO_ADDRESS_VERIFY(SSP_BASE + 0x04)
#define SSP_DR               IO_ADDRESS_VERIFY(SSP_BASE + 0x08)
#define SSP_SR               IO_ADDRESS_VERIFY(SSP_BASE + 0x0C)
#define SSP_CPSR             IO_ADDRESS_VERIFY(SSP_BASE + 0x10)
#define SSP_IMSC             IO_ADDRESS_VERIFY(SSP_BASE + 0x14)
#define SSP_RIS              IO_ADDRESS_VERIFY(SSP_BASE + 0x18)
#define SSP_MIS              IO_ADDRESS_VERIFY(SSP_BASE + 0x1C)
#define SSP_ICR              IO_ADDRESS_VERIFY(SSP_BASE + 0x20)
#define SSP_DMACR            IO_ADDRESS_VERIFY(SSP_BASE + 0x24)

#define SPI_SR_BSY        (0x1 << 4)/* spi busy flag */
#define SPI_SR_TFE        (0x1 << 0)/* Whether to send fifo is empty */
#define SPI_DATA_WIDTH    (9)
#define SPI_SPO           (1)
#define SPI_SPH           (1)
#define SPI_SCR           (8)
#define SPI_CPSDVSR       (8)
#define SPI_FRAMEMODE     (0)

#define MAX_WAIT 10000

#define  ssp_readw(addr,ret)            (ret =(*(volatile unsigned int *)(addr)))
#define  ssp_writew(addr,value)            ((*(volatile unsigned int *)(addr)) = (value))

static int ssp_set_reg(unsigned int Addr, unsigned int Value)
{
#ifdef __HuaweiLite__
    (*(volatile unsigned int *)(Addr)) = Value;
#else
    void* pmem = ioremap_nocache(Addr, DEFAULT_MD_LEN);
    if (pmem == NULL)
    {
        return -1;
    }

    *(unsigned int*)pmem = Value;
    iounmap(pmem);
#endif
    return 0;
}

static int ssp_read_reg(unsigned int Addr, unsigned int* pValue)
{
#ifdef __HuaweiLite__
    (*pValue) = (*(volatile unsigned int *)(Addr));
#else
    void* pmem = ioremap_nocache(Addr, DEFAULT_MD_LEN);
    if (pmem == NULL)
    {
        return -1;
    }

    * pValue = *(unsigned int*)pmem;
    iounmap(pmem);
#endif
    return 0;
}

static int hi_spi_check_timeout(void)
{
    unsigned int value =  0;
    unsigned int tmp = 0;
    while (1)
    {
        ssp_readw(SSP_SR,value);
        if ((value & SPI_SR_TFE) && (!(value & SPI_SR_BSY)))
        {
            break;
        }

        if (tmp++ > MAX_WAIT)
        {
            printk("Kernel: spi transfer wait timeout!\n");
        return -1;
        }
        udelay(1);
    }
    return 0;
}



void hi_ssp_writeOnly(int bWriteOnly)
{
    int ret = 0;

    bWriteOnly = 0;

    ssp_readw(SSP_CR1,ret);

    if (bWriteOnly)
    {
        ret = ret | (0x1 << 5);
    }
    else
    {
        ret = ret & (~(0x1 << 5));
    }

    ssp_writew(SSP_CR1,ret);
}


void hi_ssp_enable(void)
{
    int ret = 0;
    ssp_readw(SSP_CR1,ret);
    ret = (ret & 0xFFFD) | 0x2;

    ret = ret | (0x1 << 4); /* big/little end, 1: little, 0: big */

    ret = ret | (0x1 << 15); /* wait en */

    ssp_writew(SSP_CR1,ret);

    hi_ssp_writeOnly(0);
}


void hi_ssp_disable(void)
{
    int ret = 0;
    ssp_readw(SSP_CR1,ret);
    ret = ret & (~(0x1 << 1));
    ssp_writew(SSP_CR1,ret);
}

int hi_ssp_set_frameform(unsigned char framemode,unsigned char spo,unsigned char sph,unsigned char datawidth)
{
    int ret = 0;
    ssp_readw(SSP_CR0,ret);
    if(framemode > 3)
    {
        printk("Kernel: set frame parameter err.\n");
        return -1;
    }
    ret = (ret & 0xFFCF) | (framemode << 4);
    if((ret & 0x30) == 0)
    {
        if(spo > 1)
        {
            printk("Kernel: set spo parameter err.\n");
            return -1;
        }
        if(sph > 1)
        {
            printk("Kernel: set sph parameter err.\n");
            return -1;
        }
        ret = (ret & 0xFF3F) | (sph << 7) | (spo << 6);
    }
    if((datawidth > 16) || (datawidth < 4))
    {
        printk("Kernel: set datawidth parameter err.\n");
        return -1;
    }
    ret = (ret & 0xFFF0) | (datawidth -1);
    ssp_writew(SSP_CR0,ret);
    return 0;
}


int hi_ssp_set_serialclock(unsigned char scr,unsigned char cpsdvsr)
{
    int ret = 0;
    ssp_readw(SSP_CR0,ret);
    ret = (ret & 0xFF) | (scr << 8);
    ssp_writew(SSP_CR0,ret);
    if((cpsdvsr & 0x1))
    {
        printk("Kernel: set cpsdvsr parameter err.\n");
        return -1;
    }
    ssp_writew(SSP_CPSR,cpsdvsr);
    return 0;
}

int hi_ssp_alt_mode_set(int enable)
{
    int ret = 0;

    ssp_readw(SSP_CR1,ret);
    if (enable)
    {
        ret = ret & (~0x40);
    }
    else
    {
        ret = (ret & 0xFF) | 0x40;
    }
    ssp_writew(SSP_CR1,ret);

    return 0;
}


static void spi_enable(void)
{
    ssp_writew(SSP_CR1, 0x42);
}

static void spi_disable(void)
{
    ssp_writew(SSP_CR1, 0x40);
}

void spi_write_a9byte(unsigned char cmd_dat,unsigned char dat)
{
    unsigned short spi_data = 0;
    int ret = 0;
    if(cmd_dat)
    {
        spi_data = 1 << 8;
    }
    else
    {
        spi_data = 0 << 8;
    }
     //  printk("Kernel: !!!!!!!!!!!!!!!!!!spi_send cmd_dat DATA:0x%x ; dat:0x%x\n",cmd_dat,dat);
    spi_data = spi_data | dat;
    spi_enable();
    ssp_writew(SSP_DR,spi_data);
    ret =  hi_spi_check_timeout();
    if(ret != 0)
    {
        printk("Kernel: !!!!!!!!!!!!!!!!!!spi_send timeout\n");
    }
    spi_disable();
}

void spi_write_a16byte(unsigned short spi_dat)
{
    spi_enable();
    //hi_ssp_writedata(spi_data);
    ssp_writew(SSP_DR,spi_dat);
    printk("Kernel: spi_data:0x%x !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",spi_dat);
    msleep(1);
    spi_disable();
}


void spi_read_a16byte(unsigned short spi_dat)
{
    spi_enable();
    //hi_ssp_writedata(spi_data);
    unsigned int value =  0;
    unsigned int tmp = 0;
     while (1)
    {
        ssp_readw(SSP_SR,value);
        printk("Kernel:ssp_readw value------- spi_data:0x%x !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",value);
        if (tmp++ > MAX_WAIT)
        {
            printk("Kernel: !!!!!!!!!!FUCK FUCK spi transfer wait timeout!!!!\n");
        return -1;
        }
        udelay(1);
    }
    printk("Kernel:ssp_readw------- spi_data:0x%x !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",spi_dat);
    msleep(1);
    spi_disable();
}

void ssp_write_dat(unsigned char dat)
{
    spi_write_a9byte(1,dat);
}

void ssp_write_cmd(unsigned char dat)
{
    spi_write_a9byte(0,dat);
}

long ssp_lcd_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    //printk(KERN_EMERG " cmd = 0x%x,cmd&0xf000  = 0x%x\n",cmd,cmd&0xf000);
    if((cmd&0xf000 )== 0xf000)
    {
        printk("Kernel: cmd = 0x%x,0x%x,0x%x\n",cmd,((cmd&(0x1<<8))>>8),cmd&0xff);
        spi_write_a9byte(((cmd&(0x1<<8))>>8),cmd&0xff);
        return 0;
    }
    switch(cmd)
    {
        case SSP_LCD_SET_BRIGHTNESS:

        break;
        case SSP_LCD_SET_BACKLIGHT:

        break;
        case SSP_LCD_SET_DISPLAY:
 #if 0
        ssp_gpio_dir_config(9,6,1);
        if(arg)
        {
            ssp_write_cmd(0x11);
            ssp_gpio_write(9,6,1);
        }
        else
        {
            ssp_write_cmd(0x10);
            ssp_gpio_write(9,6,0);
        }
  #endif
        break;
        default:
        {
            printk("Kernel: No such ssp command %#x!\n", cmd);
            break;
        }
    }

    return 0;
}

int ssp_lcd_open(struct inode * inode, struct file * file)
{
        return 0;
}


int ssp_lcd_close(struct inode * inode, struct file * file)
{
    return 0;
}

void ssp_set(void)
{
    spi_disable();
    hi_ssp_set_frameform(SPI_FRAMEMODE, SPI_SPO, SPI_SPH, SPI_DATA_WIDTH);
    hi_ssp_set_serialclock(SPI_SCR, SPI_CPSDVSR);
    hi_ssp_alt_mode_set(1);
    hi_ssp_enable();
}

void lcd_reset(void)
{
    int tmpvalue = 0;
    //-----------------------------------------------LED RESET----------------------------------------------//
#if defined (HI3556AV100)
    int setvalue = 0x1 << 1;
    int mask = 0x1 << 1 ;
    ssp_set_reg(0x047E0024, 0x1000);
    ssp_read_reg(0x045fC400, &tmpvalue);  /**GPIO_12_1*/
    tmpvalue &= ~(mask);
    tmpvalue |= (setvalue & mask);
    ssp_set_reg(0x045fC400, tmpvalue);

    ssp_set_reg(0x045fC008, 0x2);
    ssp_set_reg(0x045fC008, 0x0);
    udelay(1);
    ssp_set_reg(0x045fC008, 0x2);
    udelay(40 * 1000);
#endif

#if defined (HI3559V200)
    int setvalue = 0x1 << 5;
    int mask = 0x1 << 5 ;
    ssp_read_reg(0x120d0400, &tmpvalue);  /**GPIO_0_5*/
    tmpvalue &= ~(mask);
    tmpvalue |= (setvalue & mask);
    ssp_set_reg(0x120d0400, tmpvalue);

    //ssp_set_reg(0x120d0400, 0x20);       /**GPIO_0_5*/
    ssp_set_reg(0x120d0080, 0x20);
    ssp_set_reg(0x120d0080, 0x0);
    msleep(120);
   printk("Kernel: lcd_reset !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    ssp_set_reg(0x120d0080, 0x20);
    udelay(40 * 1000);
#endif

    //----------------------------------------ST7789S SleepOut Cmd-------------------------------------------//

    //msleep(120);
}

void spi_read_a9byte(unsigned char dat)
{
    unsigned short spi_data = 0;
    int ret = 0;
	
    int retD = 0;
    spi_data = 1 << 8;

    spi_data = spi_data | 0XFF;
    spi_enable();
    ssp_writew(SSP_DR,spi_data);
    ret =  hi_spi_check_timeout();
    if(ret != 0)
    {
        printk("Kernel: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!spi_send timeout\n");
    }
	
    ssp_readw(SSP_DR,retD);
	dat=retD;
    printk("Kernel: yichen !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!SSP_DR %04x\n",retD);

}
void lcd_ili9342c_init_vertical(void){
	printk("Kernel: NEW ----c0-- NEW   ---- lcd_ili9342c_init_vertical!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	    ssp_set();

	    lcd_reset();
	msleep(120);
	//************Start Initial Sequence****************************
	ssp_write_cmd(0xC8);       //Set EXTC
	 ssp_write_dat(0xFF);
	 ssp_write_dat(0x93);
	 ssp_write_dat(0x42);
	  
	ssp_write_cmd(0xb6);  
	 ssp_write_dat(0x0a);
	 ssp_write_dat(0xE0);//SS GS

	ssp_write_cmd(0x36); //Memory Access Control
	 //ssp_write_dat(0xA8); //MY(7),MX(6),MV(5),ML(4),BGR(3),MH(2)
	 ssp_write_dat(0x08); //MY(7),MX(6),MV(5),ML(4),BGR(3),MH(2)
	
    ssp_write_cmd(0x37);  
	 ssp_write_dat(0x00);
	 ssp_write_dat(0x00);




	ssp_write_cmd(0xC0); //Power Control 1
	 ssp_write_dat(0x13); //VRH[5:0]
	 ssp_write_dat(0x0E); //VC[3:0]
	  
	ssp_write_cmd(0xC1);  //Power Control 2
	 ssp_write_dat(0x02); //SAP[2:0],BT[3:0]
	  
	ssp_write_cmd(0xC5); //VCOM
	 ssp_write_dat(0XCA); //C8++
	  
	ssp_write_cmd(0xB1);      
	 ssp_write_dat(0x00);     
	 ssp_write_dat(0x1B);

	ssp_write_cmd(0xB4);      
	 ssp_write_dat(0x02);
	//*****************GAMMA*****************  
	ssp_write_cmd(0xE0);
	 ssp_write_dat(0x00);//P01-VP63   
	 ssp_write_dat(0x01);//P02-VP62   
	 ssp_write_dat(0x04);//P03-VP61   
	 ssp_write_dat(0x00);//P04-VP59   
	 ssp_write_dat(0x11);//P05-VP57   
	 ssp_write_dat(0x08);//P06-VP50   
	 ssp_write_dat(0x35);//P07-VP43   
	 ssp_write_dat(0x79);//P08-VP27,36
	 ssp_write_dat(0x45);//P09-VP20   
	 ssp_write_dat(0x07);//P10-VP13   
	 ssp_write_dat(0x0D);//P11-VP6    
	 ssp_write_dat(0x09);//P12-VP4    
	 ssp_write_dat(0x16);//P13-VP2    
	 ssp_write_dat(0x17);//P14-VP1    
	 ssp_write_dat(0x0F);//P15-VP0    
	  
	ssp_write_cmd(0xE1);
	 ssp_write_dat(0x00);//P01
	 ssp_write_dat(0x28);//P02
	 ssp_write_dat(0x29);//P03
	 ssp_write_dat(0x02);//P04
	 ssp_write_dat(0x0F);//P05
	 ssp_write_dat(0x06);//P06
	 ssp_write_dat(0x3F);//P07
	 ssp_write_dat(0x25);//P08
	 ssp_write_dat(0x55);//P09
	 ssp_write_dat(0x06);//P10
	 ssp_write_dat(0x15);//P11
	 ssp_write_dat(0x0F);//P12
	 ssp_write_dat(0x38);//P13
	 ssp_write_dat(0x38);//P14
	 ssp_write_dat(0x0F);//P15

	//*************RGB interface set**************
	ssp_write_cmd(0x3A); //Pixel Format Set
	ssp_write_dat(0x55); //16BIT

	ssp_write_cmd(0xB0); //此寄存器很重要，重点设置DEN，DOCLK，HSYNC，VSYNC的极性，与客人的主控有很大的联系,客人请自己行根据IC规格书和主控进行调整.
	ssp_write_dat(0xC0);//40,42,60,62,cc。C0客人反映OK，麻点是PCLK的时序没有配合好,如果客人是上升沿的，那么我的为下降沿.
//0x40 //0x42

	ssp_write_cmd(0xf6);//此寄存器设RGB模式或VSYNC模式接口，选择用16或18BIT 数据线.
	ssp_write_dat(0x00);//01
	ssp_write_dat(0x00);
	ssp_write_dat(0x06);//08

	//********Window(窗口/地址)****************
	ssp_write_cmd(0x2A); //320
	ssp_write_dat(0x00);
	ssp_write_dat(0x00);
	ssp_write_dat(0x01);
	ssp_write_dat(0x3F);

	ssp_write_cmd(0x2B); //240
	ssp_write_dat(0x00);
	ssp_write_dat(0x00);
	ssp_write_dat(0x00);
	ssp_write_dat(0xEF);
	//****************************** 


	//********Backlight Control****************
	ssp_write_cmd(0xB8); //Backlight Control1
	ssp_write_dat(0x00);

	ssp_write_cmd(0xB9); //Backlight Control2
	ssp_write_dat(0x00);

	ssp_write_cmd(0xBA); //Backlight Control3
	ssp_write_dat(0x00);

	//****************************** 
	ssp_write_cmd(0x11);//Exit Sleep
	msleep(120);
	ssp_write_cmd(0x29);//Display On
	ssp_write_cmd(0x2c); 	   
    	printk("Kernel: lcd_ili9342c_init_vertical end !!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	return;
}





#ifdef __HuaweiLite__
static const struct file_operations_vfs ssp_lcd_fops =
{
    ssp_lcd_open,   /* open */
    ssp_lcd_close,  /* close */
    0,              /* read */
    0,              /* write */
    0,              /* seek */
    ssp_lcd_ioctl   /* ioctl */
#ifndef CONFIG_DISABLE_POLL
    , 0             /* poll */
#endif
};

int lcd_dev_register(void)
{
    return register_driver("/dev/ssp_lcd", &ssp_lcd_fops, 0666, 0);
}

int lcd_dev_unregister(void)
{
    return unregister_driver("/dev/ssp_lcd");
}

int __init hi_ssp_lcd_init(void)
{
    int ret;
    printk("Kernel: hi_ssp_lcd_init  run here !!!!!!!!!!!!!!!!!!!!!!\n");
    ret = lcd_dev_register();
    if(0 != ret)
    {
        printk("Kernel: lcd_dev_register failed!\n");
        return -1;
    }
    lcd_ili9342c_init_vertical();
    printk("Kernel: ssp_lcd initial ok !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return 0;

}

void __exit hi_ssp_lcd_exit(void)
{
    hi_ssp_disable();
    lcd_dev_unregister();
}

#else

static struct file_operations ssp_lcd_fops = {
    .owner      = THIS_MODULE,
    .unlocked_ioctl = ssp_lcd_ioctl,
    .open       = ssp_lcd_open,
    .release    = ssp_lcd_close
};


static struct miscdevice ssp_lcd_dev = {
   .minor       = MISC_DYNAMIC_MINOR,
   .name        = "ssp_lcd",
   .fops        = &ssp_lcd_fops,
};


static int __init hi_ssp_lcd_init(void)
{
    int ret;

    reg_ssp_base_va = ioremap_nocache((unsigned long)SSP_BASE, (unsigned long)SSP_SIZE);
    if (!reg_ssp_base_va)
    {
        printk("Kernel: ioremap ssp base failed!\n");
        return -ENOMEM;
    }

    ret = misc_register(&ssp_lcd_dev);
    if(0 != ret)
    {
        printk("Kernel: register ssp_0 device failed!\n");
        return -1;
    }
    lcd_ili9342c_init_vertical();
    printk("Kernel: ssp_lcd initial ok!\n");

    return 0;
}

static void __exit hi_ssp_lcd_exit(void)
{

    hi_ssp_disable();
    iounmap((void*)reg_ssp_base_va);
    misc_deregister(&ssp_lcd_dev);
}

module_init(hi_ssp_lcd_init);
module_exit(hi_ssp_lcd_exit);

MODULE_LICENSE("GPL");
#endif
