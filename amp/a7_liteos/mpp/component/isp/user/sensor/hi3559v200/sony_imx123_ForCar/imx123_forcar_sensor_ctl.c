
/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : IMX123_forcar_sensor_ctl.c

  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/11/07
  Description   :
  History       :
  1.Date        : 2013/11/07
    Author      :
    Modification: Created file

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "hi_comm_video.h"
#include "hi_sns_ctrl.h"
#include "mpi_sys.h"

#include <ctype.h>
//#include <linux/fb.h>
#include <sys/mman.h>
#include <memory.h>


#ifdef HI_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "hi_i2c.h"
//#include "drv_i2c.h"
#endif

#include "mpi_isp.h"


extern WDR_MODE_E genSensorMode;
extern HI_U8 gu8SensorImageMode;
extern HI_BOOL bSensorInit;

const unsigned char imx123_forcar_i2c_addr = 0x34; // I2C Address of IMX123
const unsigned int  imx123_forcar_addr_byte    =    2;
const unsigned int  imx123_forcar_data_byte    =    1;

void IMX123_forcar_linear_qxga_30_init(VI_PIPE ViPipe);
void IMX123_forcar_linear_1080p60_init(VI_PIPE ViPipe);
void IMX123_forcar_linear_qxga_60_init(VI_PIPE ViPipe);

static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};
extern ISP_SNS_STATE_S   *g_pastImx123_forcar[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U  g_aunImx123_forcar_BusInfo[];

#define IMX123_forcar_1080P_60FPS_MODE   (0)
#define IMX123_forcar_1080P_30FPS_MODE   (1)
#define IMX123_forcar_QXGA_30FPS_MODE    (2)

int IMX123_forcar_i2c_init(VI_PIPE ViPipe)
{
    char acDevFile[16] = {0};
    HI_U8 u8DevNum;

    if (g_fd[ViPipe] >= 0)
    {
        return HI_SUCCESS;
    }

	if (imx123_forcar_addr_byte != 2 || imx123_forcar_data_byte != 1)
	{
		printf("imx123 i2c address error!\n");
		return -1;
	}
#ifdef HI_GPIO_I2C
    int ret;

    g_fd[ViPipe] = open("/dev/gpioi2c_ex", O_RDONLY, S_IRUSR);
    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(HI_DBG_ERR, "Open gpioi2c_ex error!\n");
        return HI_FAILURE;
    }
#else
    int ret;

    u8DevNum = g_aunImx123_forcar_BusInfo[ViPipe].s8I2cDev;
    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%d", u8DevNum);
    printf("%s: Open %s ... \n", __FUNCTION__, acDevFile);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(HI_DBG_ERR, "Open /dev/hi_i2c_drv-%d [%d] error!\n", u8DevNum,u8DevNum);
        return HI_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (imx123_forcar_i2c_addr >> 1));
    if (ret < 0)
    {
        ISP_TRACE(HI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return ret;
    }
#endif

    return HI_SUCCESS;
}

int IMX123_forcar_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

int IMX123_forcar_read_register(VI_PIPE ViPipe, int addr)
{
	if (0 > g_fd[ViPipe])
    return HI_SUCCESS;

	int ret;
	char buf[8] = {
		0,
	};

	ret = ioctl(g_fd[ViPipe], I2C_16BIT_REG, 1);
	ret = ioctl(g_fd[ViPipe], I2C_16BIT_DATA, 0);
	ret = ioctl(g_fd[ViPipe], I2C_TENBIT, 0);

	buf[0] = addr & 0xff;
	buf[1] = (addr >> 8) & 0xff;

	ret = read(g_fd[ViPipe], buf, imx123_forcar_addr_byte + imx123_forcar_data_byte);
	if (ret < 0)
	{
		printf("I2C_READ error!\n");
		return HI_FAILURE;
	}

	return buf[0];
}

int IMX123_forcar_write_register(VI_PIPE ViPipe, int addr, int data)
{
    if (0 > g_fd[ViPipe])
    {
        return HI_SUCCESS;
    }
ISP_TRACE(HI_DBG_ERR, "!!!!!!!!!!!!!!!!!!!!!I2C_WRITE error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

#ifdef HI_GPIO_I2C
    i2c_data.dev_addr = imx123_forcar_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = imx123_forcar_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = imx123_forcar_data_byte;

    ret = ioctl(g_fd[ViPipe], GPIO_I2C_WRITE, &i2c_data);

    if (ret)
    {
        printf(HI_DBG_ERR, "GPIO-I2C write faild!\n");
        return ret;
    }
#else
    int idx = 0;
    int ret;
	char buf[8] = {0,};

    if (imx123_forcar_addr_byte == 2)
    {
        buf[idx] = (addr >> 8) & 0xff;
        idx++;
        buf[idx] = addr & 0xff;
        idx++;
    }
    else
    {

    }

    if (imx123_forcar_data_byte == 2)
    {

    }
    else
    {
        buf[idx] = data & 0xff;
        idx++;
    }

    ret = write(g_fd[ViPipe], buf, imx123_forcar_addr_byte + imx123_forcar_data_byte);
    if (ret < 0)
    {
        ISP_TRACE(HI_DBG_ERR, "I2C_WRITE error!\n");
        return HI_FAILURE;
    }

#endif
	int val = IMX123_forcar_read_register(ViPipe, addr);
	//printf("I2C_WRITE [0x%04x]: dst = 0x%02x, curr = 0x%02x, [%s] \n",addr, data, val, data == val ? "OK" : "ERR");

    return HI_SUCCESS;
}

static void delay_ms(int ms)
{
    usleep(ms * 1000);
}

void IMX123_forcar_init(VI_PIPE ViPipe)
{
    WDR_MODE_E       enWDRMode;
    HI_BOOL          bInit;
    HI_U8            u8ImgMode;

    bInit       = g_pastImx123_forcar[ViPipe]->bInit;
    enWDRMode   = g_pastImx123_forcar[ViPipe]->enWDRMode;
    u8ImgMode   = g_pastImx123_forcar[ViPipe]->u8ImgMode;
    bSensorInit = HI_TRUE;
    if (HI_FALSE == bInit)
    {
        /*sensor i2c init */
        //printf("binit false IMX123 i2c init\n");
        IMX123_forcar_i2c_init(ViPipe);
    }

    if (WDR_MODE_NONE == enWDRMode)
    {
        if (IMX123_forcar_1080P_60FPS_MODE == u8ImgMode)
        {
            printf("IMX123_forcar_1080P_60FPS_MODE\n");
             IMX123_forcar_linear_1080p60_init(ViPipe);
        }
        else if ( IMX123_forcar_QXGA_30FPS_MODE== u8ImgMode)
        {
            printf("IMX123_forcar_QXGA_30FPS_MODE\n");
               IMX123_forcar_linear_qxga_30_init(ViPipe);
        }
        else 
        {
             IMX123_forcar_linear_1080p60_init(ViPipe);

        }
    }
    else
    {
        printf("IMX123_init Not support this mode\n");
		bSensorInit = HI_FALSE;
        return;
    }

    g_pastImx123_forcar[ViPipe]->bInit = HI_TRUE;
    return ;
}

void IMX123_forcar_exit(VI_PIPE ViPipe)
{
    IMX123_forcar_i2c_exit(ViPipe);

    return;
}

// 37.125MHz clock input
// 2970Mbps (=371.25Mbps/ch x 8ch)
void IMX123_forcar_linear_qxga_60_init(VI_PIPE ViPipe)
{
	 IMX123_forcar_write_register(ViPipe, 0x3003, 0x01); //soft reset
	//delay_ms(200);

	 IMX123_forcar_write_register(ViPipe, 0x3003, 0x00); //normal operation
	//delay_ms(200);

	 IMX123_forcar_write_register(ViPipe, 0x3000, 0x01); //STANDBY
	//delay_ms(20);

	 IMX123_forcar_write_register(ViPipe, 0x3002, 0x01); // Master mode stop 

	 IMX123_forcar_write_register(ViPipe, 0x3009, 0x00); //org

	 IMX123_forcar_write_register(ViPipe, 0x300c, 0x00);

	 IMX123_forcar_write_register(ViPipe, 0x3012, 0x86);
	 IMX123_forcar_write_register(ViPipe, 0x3013, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x3016, 0x89);
	//VMAX
	 IMX123_forcar_write_register(ViPipe, 0x3018, 0x08); //0x708 = 1800  //org
	 IMX123_forcar_write_register(ViPipe, 0x3019, 0x07);
	//HMAX
	 IMX123_forcar_write_register(ViPipe, 0x301b, 0x3c); //0x303c = 572
	 IMX123_forcar_write_register(ViPipe, 0x301c, 0x02);
	 IMX123_forcar_write_register(ViPipe, 0x301e, 0x08);
	 IMX123_forcar_write_register(ViPipe, 0x3021, 0xf0);
	 IMX123_forcar_write_register(ViPipe, 0x3024, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x302e, 0xda);
	 IMX123_forcar_write_register(ViPipe, 0x3031, 0x1c);
	 IMX123_forcar_write_register(ViPipe, 0x3043, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3044, 0xb5); // dck2en
	 IMX123_forcar_write_register(ViPipe, 0x3046, 0x44); //org
	 IMX123_forcar_write_register(ViPipe, 0x3049, 0x0a); // XVS & XHS output start 

	 IMX123_forcar_write_register(ViPipe, 0x3054, 0x61); //org

	 IMX123_forcar_write_register(ViPipe, 0x3061, 0x81);
	 IMX123_forcar_write_register(ViPipe, 0x3062, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x309d, 0x82);
	 IMX123_forcar_write_register(ViPipe, 0x309f, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x30b3, 0x91);
	 IMX123_forcar_write_register(ViPipe, 0x30c8, 0xbc);
	 IMX123_forcar_write_register(ViPipe, 0x30c9, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x30ca, 0xbc);

	 IMX123_forcar_write_register(ViPipe, 0x30cb, 0x00);

	 IMX123_forcar_write_register(ViPipe, 0x30f0, 0x86);
	 IMX123_forcar_write_register(ViPipe, 0x30f1, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x30f4, 0x86);
	 IMX123_forcar_write_register(ViPipe, 0x30f5, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x30f8, 0x86);
	 IMX123_forcar_write_register(ViPipe, 0x30f9, 0x01);

	 IMX123_forcar_write_register(ViPipe, 0x31103, 0x07);
	 IMX123_forcar_write_register(ViPipe, 0x3104, 0x07);
	 IMX123_forcar_write_register(ViPipe, 0x3106, 0x07);
	 IMX123_forcar_write_register(ViPipe, 0x3107, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3108, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3109, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x310a, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x310b, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3110, 0xf2);
	 IMX123_forcar_write_register(ViPipe, 0x3111, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x3112, 0xeb);
	 IMX123_forcar_write_register(ViPipe, 0x3113, 0x07);
	 IMX123_forcar_write_register(ViPipe, 0x3114, 0xed);
	 IMX123_forcar_write_register(ViPipe, 0x3115, 0x07);
	 IMX123_forcar_write_register(ViPipe, 0x3126, 0x91);
	 IMX123_forcar_write_register(ViPipe, 0x3133, 0x12);
	 IMX123_forcar_write_register(ViPipe, 0x3134, 0x10);
	 IMX123_forcar_write_register(ViPipe, 0x3135, 0x12);
	 IMX123_forcar_write_register(ViPipe, 0x3136, 0x10);
	 IMX123_forcar_write_register(ViPipe, 0x313a, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x313b, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x313c, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x313d, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x3140, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3141, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3144, 0x1e);
	 IMX123_forcar_write_register(ViPipe, 0x3149, 0x55);
	 IMX123_forcar_write_register(ViPipe, 0x314b, 0x99);
	 IMX123_forcar_write_register(ViPipe, 0x314c, 0x99);
	 IMX123_forcar_write_register(ViPipe, 0x3154, 0xe7);
	 IMX123_forcar_write_register(ViPipe, 0x315a, 0x04);
	 IMX123_forcar_write_register(ViPipe, 0x316c, 0x20);
	 IMX123_forcar_write_register(ViPipe, 0x316d, 0x09);
	 IMX123_forcar_write_register(ViPipe, 0x3170, 0x40);
	 IMX123_forcar_write_register(ViPipe, 0x3171, 0x14);
	 IMX123_forcar_write_register(ViPipe, 0x3179, 0x94);
	 IMX123_forcar_write_register(ViPipe, 0x317a, 0x06);
	 IMX123_forcar_write_register(ViPipe, 0x31f6, 0x00); // new added

	 IMX123_forcar_write_register(ViPipe, 0x3201, 0x3c);
	 IMX123_forcar_write_register(ViPipe, 0x3202, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x3203, 0x0e);
	 IMX123_forcar_write_register(ViPipe, 0x3213, 0x05);
	 IMX123_forcar_write_register(ViPipe, 0x321f, 0x05);
	 IMX123_forcar_write_register(ViPipe, 0x325f, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x3269, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x32b6, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x32ba, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x32c4, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x32cb, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x32d9, 0x80);
	 IMX123_forcar_write_register(ViPipe, 0x32dc, 0xb0);
	 IMX123_forcar_write_register(ViPipe, 0x32dd, 0x13);

	 IMX123_forcar_write_register(ViPipe, 0x332a, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332b, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332c, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332d, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332e, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332f, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x3335, 0x50);
	 IMX123_forcar_write_register(ViPipe, 0x3336, 0x80);
	 IMX123_forcar_write_register(ViPipe, 0x3337, 0x1b);
	 IMX123_forcar_write_register(ViPipe, 0x333c, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x333d, 0x03);

	 IMX123_forcar_write_register(ViPipe, 0x3000, 0x00); // Standby cancel 
	delay_ms(20);

	 IMX123_forcar_write_register(ViPipe, 0x3044, 0xf2); // DCKRST = 1 
	 IMX123_forcar_write_register(ViPipe, 0x3044, 0xb2); // DCKRST = 0 
	 IMX123_forcar_write_register(ViPipe, 0x3002, 0x00); // Master mode start 

	printf("-------Sony IMX123 Sensor QXGA 60fps Linear Initial OK!-------\n");
}

void IMX123_forcar_linear_qxga_30_init(VI_PIPE ViPipe)
{
	//------------------------- Chip ID = 02h ------------------------------
 	 IMX123_forcar_write_register(ViPipe, 0x3000, 0x01); // STANDBY
	 IMX123_forcar_write_register(ViPipe, 0x3002, 0x01); // MastEr modE stop

	IMX123_forcar_write_register(ViPipe, 0x3005, 0x02); // ADBIT 12 BIT
	IMX123_forcar_write_register(ViPipe, 0x3007, 0x00); // WINMODE 1080P
	IMX123_forcar_write_register(ViPipe, 0x3009, 0x01); // DRSEL

	 IMX123_forcar_write_register(ViPipe, 0x3012, 0x0E); // FIX
	 IMX123_forcar_write_register(ViPipe, 0x3013, 0x01); // FIX

	 IMX123_forcar_write_register(ViPipe, 0x3018, 0x59); //VMAX L
	 IMX123_forcar_write_register(ViPipe, 0x3019, 0x06); //VMAX H

	IMX123_forcar_write_register(ViPipe, 0x301B, 0x40); //HMAX L
	IMX123_forcar_write_register(ViPipe, 0x301C, 0x06); //HMAX H

	 IMX123_forcar_write_register(ViPipe, 0x301E, 0x08); //SHS1 L
	 IMX123_forcar_write_register(ViPipe, 0x301F, 0x00); //SHS1 H

	 IMX123_forcar_write_register(ViPipe, 0x3044, 0x22); // CSI-2;

	 IMX123_forcar_write_register(ViPipe, 0x3047, 0x04); // XHSLNG

	 IMX123_forcar_write_register(ViPipe, 0x3061, 0x31); // INCKSEL1
	 IMX123_forcar_write_register(ViPipe, 0x3062, 0x00); // INCKSEL2

	 IMX123_forcar_write_register(ViPipe, 0x309d, 0x82);
	 IMX123_forcar_write_register(ViPipe, 0x309f, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x30b3, 0x91);
	 IMX123_forcar_write_register(ViPipe, 0x30f0, 0x86);
	 IMX123_forcar_write_register(ViPipe, 0x30f1, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x30f4, 0x86);
	 IMX123_forcar_write_register(ViPipe, 0x30f5, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x30f8, 0x86);
	 IMX123_forcar_write_register(ViPipe, 0x30f9, 0x01);

	//------------------------- Chip ID = 03h ------------------------------
	 IMX123_forcar_write_register(ViPipe, 0x3103, 0x58);
	 IMX123_forcar_write_register(ViPipe, 0x3104, 0x06);
	 IMX123_forcar_write_register(ViPipe, 0x3106, 0x07);
	 IMX123_forcar_write_register(ViPipe, 0x3107, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3108, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3109, 0x00);
         IMX123_forcar_write_register(ViPipe, 0x310A, 0x00);
         IMX123_forcar_write_register(ViPipe, 0x310B, 0x00);

	 IMX123_forcar_write_register(ViPipe, 0x3110, 0xf2);
	 IMX123_forcar_write_register(ViPipe, 0x3111, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x3112, 0xEb);
	 IMX123_forcar_write_register(ViPipe, 0x3113, 0x07);
	 IMX123_forcar_write_register(ViPipe, 0x3114, 0xEd);
	 IMX123_forcar_write_register(ViPipe, 0x3115, 0x07);

	 IMX123_forcar_write_register(ViPipe, 0x3126, 0x91);

	IMX123_forcar_write_register(ViPipe, 0x3130, 0x4d); // HS_1

	 IMX123_forcar_write_register(ViPipe, 0x3133, 0x12);
	 IMX123_forcar_write_register(ViPipe, 0x3134, 0x10);
	 IMX123_forcar_write_register(ViPipe, 0x3135, 0x12);
	 IMX123_forcar_write_register(ViPipe, 0x3136, 0x10);
	 IMX123_forcar_write_register(ViPipe, 0x313a, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x313b, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x313c, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x313d, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x3140, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3141, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3144, 0x1E);
	 IMX123_forcar_write_register(ViPipe, 0x3149, 0x55);
	 IMX123_forcar_write_register(ViPipe, 0x314b, 0x99);
	 IMX123_forcar_write_register(ViPipe, 0x314c, 0x99);
	 IMX123_forcar_write_register(ViPipe, 0x3154, 0xE7);
	 IMX123_forcar_write_register(ViPipe, 0x315a, 0x04);
	 IMX123_forcar_write_register(ViPipe, 0x316c, 0x2C);
	 IMX123_forcar_write_register(ViPipe, 0x316d, 0x0A);
         IMX123_forcar_write_register(ViPipe, 0x3170, 0x61);
         IMX123_forcar_write_register(ViPipe, 0x3171, 0x1A);
	 IMX123_forcar_write_register(ViPipe, 0x3179, 0x94);
	 IMX123_forcar_write_register(ViPipe, 0x317a, 0x06);
	IMX123_forcar_write_register(ViPipe, 0x31Eb, 0x44); // HS_2
	IMX123_forcar_write_register(ViPipe, 0x31f6, 0x11);

	//------------------------- Chip ID = 04h ------------------------------
	 IMX123_forcar_write_register(ViPipe, 0x3201, 0x3c);
	 IMX123_forcar_write_register(ViPipe, 0x3202, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x3203, 0x0E);
	 IMX123_forcar_write_register(ViPipe, 0x3213, 0x05);
	 IMX123_forcar_write_register(ViPipe, 0x321f, 0x05);
	 IMX123_forcar_write_register(ViPipe, 0x325f, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x3269, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x32b6, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x32ba, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x32c4, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x32cb, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x32d9, 0x80);
	 IMX123_forcar_write_register(ViPipe, 0x32dc, 0xb0);
	 IMX123_forcar_write_register(ViPipe, 0x32dd, 0x13);

	//------------------------- Chip ID = 05h ------------------------------
	 IMX123_forcar_write_register(ViPipe, 0x332a, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332b, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332c, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332d, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332E, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332f, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x3335, 0x50);
	 IMX123_forcar_write_register(ViPipe, 0x3336, 0x80);
	 IMX123_forcar_write_register(ViPipe, 0x3337, 0x1b);
	 IMX123_forcar_write_register(ViPipe, 0x333c, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x333d, 0x03);

	//------------------------- Chip ID = 07h  ------------------------------
	// mipi sEtting
	IMX123_forcar_write_register(ViPipe, 0x3504, 0x0C);
	 IMX123_forcar_write_register(ViPipe, 0x3505, 0x0C);
	IMX123_forcar_write_register(ViPipe, 0x350B, 0x03);
	IMX123_forcar_write_register(ViPipe, 0x3524, 0x57);

	IMX123_forcar_write_register(ViPipe, 0x3528, 0x37);
	IMX123_forcar_write_register(ViPipe, 0x352A, 0x1F);
    IMX123_forcar_write_register(ViPipe, 0x352C, 0x17);
	IMX123_forcar_write_register(ViPipe, 0x352E, 0x1F);
	IMX123_forcar_write_register(ViPipe, 0x3530, 0x77);
	IMX123_forcar_write_register(ViPipe, 0x3532, 0x0F);
	IMX123_forcar_write_register(ViPipe, 0x3536, 0x17);
    IMX123_forcar_write_register(ViPipe, 0x3580, 0x17);
	IMX123_forcar_write_register(ViPipe, 0x3586, 0x37);

	// START SENSOR
	IMX123_forcar_write_register(ViPipe, 0x3000, 0x00); // Standby cancEl
	delay_ms(20);
	IMX123_forcar_write_register(ViPipe, 0x3002, 0x00); // mastEr modE start
	IMX123_forcar_write_register(ViPipe, 0x3049, 0x0a); // XVSOUTSEL XHSOUTSEL
	printf("-------Sony IMX123 sensor QXGA 30fps Linear Initial OK!-------\n");
}
// INCLK= 37.125MHz , QXGA mode
// 2376Mbps Normal mode 30fps
// Serial LVDS 8ch output 12bit 297Mbps/ch
// Vmax = 1650, Hmax = 1500
void IMX123_forcar_linear_qxga_30_init_old(VI_PIPE ViPipe)
{
	 IMX123_forcar_write_register(ViPipe, 0x3003, 0x01); //soft reset
	//delay_ms(200);
	 IMX123_forcar_write_register(ViPipe, 0x3003, 0x00); //normal operation
	//delay_ms(200);

	 IMX123_forcar_write_register(ViPipe, 0x3000, 0x01); //STANDBY
	//delay_ms(200);

	 IMX123_forcar_write_register(ViPipe, 0x3002, 0x01); /* Master mode stop */
	 IMX123_forcar_write_register(ViPipe, 0x3012, 0x86);
	 IMX123_forcar_write_register(ViPipe, 0x3013, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x3016, 0x89);
	 IMX123_forcar_write_register(ViPipe, 0x3018, 0x72); // VMax
	 IMX123_forcar_write_register(ViPipe, 0x301b, 0xdc);
	 IMX123_forcar_write_register(ViPipe, 0x301c, 0x05);
	 IMX123_forcar_write_register(ViPipe, 0x301e, 0x08);
	 IMX123_forcar_write_register(ViPipe, 0x3021, 0x08);
	 IMX123_forcar_write_register(ViPipe, 0x3024, 0x08);
	 IMX123_forcar_write_register(ViPipe, 0x3027, 0x08);
	 IMX123_forcar_write_register(ViPipe, 0x3044, 0xb5); //0x35 //DCK2EN
	 IMX123_forcar_write_register(ViPipe, 0x3049, 0x0a);
	 IMX123_forcar_write_register(ViPipe, 0x3061, 0xa1);
	 IMX123_forcar_write_register(ViPipe, 0x3062, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x309d, 0x82);
	 IMX123_forcar_write_register(ViPipe, 0x309f, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x30b3, 0x91);
	 IMX123_forcar_write_register(ViPipe, 0x30f0, 0x86);
	 IMX123_forcar_write_register(ViPipe, 0x30f1, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x30f4, 0x86);
	 IMX123_forcar_write_register(ViPipe, 0x30f5, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x30f8, 0x86);
	 IMX123_forcar_write_register(ViPipe, 0x30f9, 0x01);

	 IMX123_forcar_write_register(ViPipe, 0x3103, 0x3f);
	 IMX123_forcar_write_register(ViPipe, 0x3104, 0x06);
	 IMX123_forcar_write_register(ViPipe, 0x3106, 0x07);
	 IMX123_forcar_write_register(ViPipe, 0x3107, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3108, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3109, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x310a, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x310b, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3110, 0xf2);
	 IMX123_forcar_write_register(ViPipe, 0x3111, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x3112, 0xeb);
	 IMX123_forcar_write_register(ViPipe, 0x3113, 0x07);
	 IMX123_forcar_write_register(ViPipe, 0x3114, 0xed);
	 IMX123_forcar_write_register(ViPipe, 0x3115, 0x07);
	 IMX123_forcar_write_register(ViPipe, 0x3126, 0x91);
	 IMX123_forcar_write_register(ViPipe, 0x3133, 0x12);
	 IMX123_forcar_write_register(ViPipe, 0x3134, 0x10);
	 IMX123_forcar_write_register(ViPipe, 0x3135, 0x12);
	 IMX123_forcar_write_register(ViPipe, 0x3136, 0x10);
	 IMX123_forcar_write_register(ViPipe, 0x313a, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x313b, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x313c, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x313d, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x3140, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3141, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3144, 0x1e);
	 IMX123_forcar_write_register(ViPipe, 0x3149, 0x55);
	 IMX123_forcar_write_register(ViPipe, 0x314b, 0x99);
	 IMX123_forcar_write_register(ViPipe, 0x314c, 0x99);
	 IMX123_forcar_write_register(ViPipe, 0x3154, 0xe7);
	 IMX123_forcar_write_register(ViPipe, 0x315a, 0x04);
	 IMX123_forcar_write_register(ViPipe, 0x316c, 0x20);
	 IMX123_forcar_write_register(ViPipe, 0x316d, 0x09);
	 IMX123_forcar_write_register(ViPipe, 0x3171, 0x20);
	 IMX123_forcar_write_register(ViPipe, 0x3179, 0x94);
	 IMX123_forcar_write_register(ViPipe, 0x317a, 0x06);
	 IMX123_forcar_write_register(ViPipe, 0x31f6, 0x11);

	 IMX123_forcar_write_register(ViPipe, 0x3201, 0x3c);
	 IMX123_forcar_write_register(ViPipe, 0x3202, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x3203, 0x0e);
	 IMX123_forcar_write_register(ViPipe, 0x3213, 0x05);
	 IMX123_forcar_write_register(ViPipe, 0x321f, 0x05);
	 IMX123_forcar_write_register(ViPipe, 0x325f, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x3269, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x32b6, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x32ba, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x32c4, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x32cb, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x32d9, 0x80);
	 IMX123_forcar_write_register(ViPipe, 0x32dc, 0xb0);
	 IMX123_forcar_write_register(ViPipe, 0x32dd, 0x13);

	 IMX123_forcar_write_register(ViPipe, 0x332a, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332b, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332c, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332d, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332e, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x332f, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x3335, 0x50);
	 IMX123_forcar_write_register(ViPipe, 0x3336, 0x80);
	 IMX123_forcar_write_register(ViPipe, 0x3337, 0x1b);
	 IMX123_forcar_write_register(ViPipe, 0x333c, 0x01);
	 IMX123_forcar_write_register(ViPipe, 0x333d, 0x03);

	 IMX123_forcar_write_register(ViPipe, 0x3000, 0x00); /* Standby cancel */
	delay_ms(20);
	 IMX123_forcar_write_register(ViPipe, 0x3044, 0xf5); /* DCKRST = 1 */
	 IMX123_forcar_write_register(ViPipe, 0x3044, 0xb5); /* DCKRST = 0 */
	 IMX123_forcar_write_register(ViPipe, 0x3002, 0x00); /* Master mode start */

	printf("-------Sony IMX123 Sensor QXGA 30 Linear Initial OK!-------\n");
}

// HD 1080p mode; 1782Mbps Normal mode 60fps
// INCK = 37.125MHz; Vmax = 1125; Hmax = 1100;
void IMX123_forcar_linear_1080p60_init(VI_PIPE ViPipe)
{
	//IMX123_forcar_write_register(ViPipe, 0x3003, 0x01);  //soft reset
	//delay_ms(200);
	//IMX123_forcar_write_register(ViPipe, 0x3003, 0x00);  //normal operation
	//delay_ms(200);

	//------------------------- Chip ID = 02h ------------------------------
	IMX123_forcar_write_register(ViPipe, 0x3000, 0x01); // STANDBY
	IMX123_forcar_write_register(ViPipe, 0x3002, 0x01); // Master mode stop

	IMX123_forcar_write_register(ViPipe, 0x3005, 0x02); // ADBIT 12 BIT
	IMX123_forcar_write_register(ViPipe, 0x3007, 0x10); // WINMODE 1080P
	IMX123_forcar_write_register(ViPipe, 0x3009, 0x01); // DRSEL

	 IMX123_forcar_write_register(ViPipe, 0x3012, 0x0E); // FIX
	 IMX123_forcar_write_register(ViPipe, 0x3013, 0x01); // FIX

	 IMX123_forcar_write_register(ViPipe, 0x3018, 0x65); //VMAX L
	 IMX123_forcar_write_register(ViPipe, 0x3019, 0x04); //VMAX H

	IMX123_forcar_write_register(ViPipe, 0x301B, 0x4c); //HMAX L
	IMX123_forcar_write_register(ViPipe, 0x301C, 0x04); //HMAX H

	 IMX123_forcar_write_register(ViPipe, 0x301E, 0x08); //SHS1 L
	 IMX123_forcar_write_register(ViPipe, 0x301F, 0x00); //SHS1 H

	 IMX123_forcar_write_register(ViPipe, 0x3044, 0x22); // CSI-2;

	 IMX123_forcar_write_register(ViPipe, 0x3047, 0x04); // XHSLNG

	 IMX123_forcar_write_register(ViPipe, 0x3061, 0x81); // INCKSEL1
	 IMX123_forcar_write_register(ViPipe, 0x3062, 0x00); // INCKSEL2

	IMX123_forcar_write_register(ViPipe, 0x309d, 0x82);
	IMX123_forcar_write_register(ViPipe, 0x309f, 0x01);
	IMX123_forcar_write_register(ViPipe, 0x30b3, 0x91);
	IMX123_forcar_write_register(ViPipe, 0x30f0, 0x86);
	IMX123_forcar_write_register(ViPipe, 0x30f1, 0x01);
	IMX123_forcar_write_register(ViPipe, 0x30f4, 0x86);
	IMX123_forcar_write_register(ViPipe, 0x30f5, 0x01);
	IMX123_forcar_write_register(ViPipe, 0x30f8, 0x86);
	IMX123_forcar_write_register(ViPipe, 0x30f9, 0x01);

	//------------------------- Chip ID = 03h ------------------------------
	 IMX123_forcar_write_register(ViPipe, 0x3103, 0x64);
	 IMX123_forcar_write_register(ViPipe, 0x3104, 0x04);
	 IMX123_forcar_write_register(ViPipe, 0x3106, 0x07);
	 IMX123_forcar_write_register(ViPipe, 0x3107, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3108, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3109, 0x00);
	IMX123_forcar_write_register(ViPipe, 0x310a, 0x00);
	IMX123_forcar_write_register(ViPipe, 0x310b, 0x00);

	IMX123_forcar_write_register(ViPipe, 0x3110, 0xf2);
	 IMX123_forcar_write_register(ViPipe, 0x3111, 0x03);
	IMX123_forcar_write_register(ViPipe, 0x3112, 0xeb);
	 IMX123_forcar_write_register(ViPipe, 0x3113, 0x07);
	IMX123_forcar_write_register(ViPipe, 0x3114, 0xed);
	 IMX123_forcar_write_register(ViPipe, 0x3115, 0x07);
	 IMX123_forcar_write_register(ViPipe, 0x3126, 0x91);
	IMX123_forcar_write_register(ViPipe, 0x3130, 0x4d); // HS_1
	 IMX123_forcar_write_register(ViPipe, 0x3133, 0x12);
	 IMX123_forcar_write_register(ViPipe, 0x3134, 0x10);
	 IMX123_forcar_write_register(ViPipe, 0x3135, 0x12);
	 IMX123_forcar_write_register(ViPipe, 0x3136, 0x10);
	IMX123_forcar_write_register(ViPipe, 0x313a, 0x0c);
	IMX123_forcar_write_register(ViPipe, 0x313b, 0x0c);
	IMX123_forcar_write_register(ViPipe, 0x313c, 0x0c);
	IMX123_forcar_write_register(ViPipe, 0x313d, 0x0c);
	 IMX123_forcar_write_register(ViPipe, 0x3140, 0x00);
	 IMX123_forcar_write_register(ViPipe, 0x3141, 0x00);
	IMX123_forcar_write_register(ViPipe, 0x3144, 0x1e);
	 IMX123_forcar_write_register(ViPipe, 0x3149, 0x55);
	IMX123_forcar_write_register(ViPipe, 0x314b, 0x99);
	IMX123_forcar_write_register(ViPipe, 0x314c, 0x99);
	IMX123_forcar_write_register(ViPipe, 0x3154, 0xe7);
	IMX123_forcar_write_register(ViPipe, 0x315a, 0x04);
	IMX123_forcar_write_register(ViPipe, 0x316c, 0x20);
	IMX123_forcar_write_register(ViPipe, 0x316d, 0x09);
	 IMX123_forcar_write_register(ViPipe, 0x3179, 0x94);
	IMX123_forcar_write_register(ViPipe, 0x317a, 0x06);
	IMX123_forcar_write_register(ViPipe, 0x31eb, 0x44); // HS_2
	IMX123_forcar_write_register(ViPipe, 0x31f6, 0x11);

	//------------------------- Chip ID = 04h ------------------------------
	IMX123_forcar_write_register(ViPipe, 0x3201, 0x3c);
	 IMX123_forcar_write_register(ViPipe, 0x3202, 0x01);
	IMX123_forcar_write_register(ViPipe, 0x3203, 0x0e);
	 IMX123_forcar_write_register(ViPipe, 0x3213, 0x05);
	IMX123_forcar_write_register(ViPipe, 0x321f, 0x05);
	IMX123_forcar_write_register(ViPipe, 0x325f, 0x03);
	 IMX123_forcar_write_register(ViPipe, 0x3269, 0x03);
	IMX123_forcar_write_register(ViPipe, 0x32b6, 0x03);
	IMX123_forcar_write_register(ViPipe, 0x32ba, 0x01);
	IMX123_forcar_write_register(ViPipe, 0x32c4, 0x01);
	IMX123_forcar_write_register(ViPipe, 0x32cb, 0x01);
	IMX123_forcar_write_register(ViPipe, 0x32d9, 0x80);
	IMX123_forcar_write_register(ViPipe, 0x32dc, 0xb0);
	IMX123_forcar_write_register(ViPipe, 0x32dd, 0x13);

	//------------------------- Chip ID = 05h ------------------------------
	IMX123_forcar_write_register(ViPipe, 0x332a, 0xff);
	IMX123_forcar_write_register(ViPipe, 0x332b, 0xff);
	IMX123_forcar_write_register(ViPipe, 0x332c, 0xff);
	IMX123_forcar_write_register(ViPipe, 0x332d, 0xff);
	IMX123_forcar_write_register(ViPipe, 0x332e, 0xff);
	IMX123_forcar_write_register(ViPipe, 0x332f, 0xff);
	 IMX123_forcar_write_register(ViPipe, 0x3335, 0x50);
	 IMX123_forcar_write_register(ViPipe, 0x3336, 0x80);
	IMX123_forcar_write_register(ViPipe, 0x3337, 0x1b);
	IMX123_forcar_write_register(ViPipe, 0x333c, 0x01);
	IMX123_forcar_write_register(ViPipe, 0x333d, 0x03);

	//------------------------- Chip ID = 07h  ------------------------------
	// mipi setting
	IMX123_forcar_write_register(ViPipe, 0x3504, 0x0C);
	 IMX123_forcar_write_register(ViPipe, 0x3505, 0x0C);
	IMX123_forcar_write_register(ViPipe, 0x350B, 0x03);
	IMX123_forcar_write_register(ViPipe, 0x3524, 0x57);

	IMX123_forcar_write_register(ViPipe, 0x3528, 0x37);
	IMX123_forcar_write_register(ViPipe, 0x352A, 0x1F);
	IMX123_forcar_write_register(ViPipe, 0x352C, 0x1F);
	IMX123_forcar_write_register(ViPipe, 0x352E, 0x1F);
	IMX123_forcar_write_register(ViPipe, 0x3530, 0x77);
	IMX123_forcar_write_register(ViPipe, 0x3532, 0x0F);
	IMX123_forcar_write_register(ViPipe, 0x3536, 0x17);
	IMX123_forcar_write_register(ViPipe, 0x3580, 0x1f);
	IMX123_forcar_write_register(ViPipe, 0x3586, 0x37);

	// START SENSOR
	IMX123_forcar_write_register(ViPipe, 0x3000, 0x00); // Standby cancel
    delay_ms(20);
	IMX123_forcar_write_register(ViPipe, 0x3002, 0x00); // master mode start
	IMX123_forcar_write_register(ViPipe, 0x3049, 0x0a); // XVSOUTSEL XHSOUTSEL

	//IMX123_forcar_write_register(ViPipe, 0x3044, 0xf5); /* DCKRST = 1 */
	//IMX123_forcar_write_register(ViPipe, 0x3044, 0xb5); /* DCKRST = 0 */
	//IMX123_forcar_write_register(ViPipe, 0x3002, 0x00); /* Master mode start */
	//himm 0x11380130 0x82801 
   // himm 0x11480130 0x82801 
    printf("-------Sony IMX123 Sensor 1080p60 Linear Initial OK!-------\n");
}
