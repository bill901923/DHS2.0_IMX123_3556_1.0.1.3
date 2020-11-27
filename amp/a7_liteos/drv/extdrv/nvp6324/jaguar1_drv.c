/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: jaguar1_drv.c
 *  Description	:
 *  Author		:
 *  Date        :
 *  Version		: Version 1.0
 *
 ********************************************************************************
 *  History     :
 *
 *
 ********************************************************************************/

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
//#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <linux/list.h>
#include <asm/delay.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/poll.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>
#include <linux/i2c.h>
#ifndef __HuaweiLite__
#include <linux/i2c-dev.h>
#include <linux/kthread.h>
#endif

#include "jaguar1_common.h"
#include "jaguar1_video.h"
#include "jaguar1_coax_protocol.h"
#include "jaguar1_motion.h"
#include "jaguar1_ioctl.h"
#include "jaguar1_video_eq.h"
#include "jaguar1_mipi.h"

#define I2C_0       (0)
#define I2C_1       (1)
#define I2C_2       (2)
#define I2C_3       (3)

#define JAGUAR1_4PORT_R0_ID 0xB0
#define JAGUAR1_2PORT_R0_ID 0xA0
#define JAGUAR1_1PORT_R0_ID 0xA2
#define AFE_NVP6134E_R0_ID  0x80

#define JAGUAR1_4PORT_REV_ID 0x00
#define JAGUAR1_2PORT_REV_ID 0x00
#define JAGUAR1_1PORT_REV_ID 0x00
#define MAX_VIDEO_CH_CNT 4


#ifdef __HuaweiLite__
#include "i2c.h"
static unsigned int open_cnt = 0;
static unsigned int i2c_dev = 0;     //0:i2c_0 1:i2c_1 2:i2c_2
#endif

#define JAGUAR1_INVALID_DEVID(devnum)  ((devnum) >= 4)

int chip_id[4];
int rev_id[4];
static unsigned char jaguar1_cnt;
unsigned char jaguar1_i2c_addr[4] = {0x60, 0x62, 0x64, 0x66};
unsigned char mclk = 3;  //0:756 1:594 2:378 3:1242
unsigned char lane = 4;  //2 or 4
unsigned char chn = 4;   //Number of channels to enable " 1 ~ 4 "
unsigned char init = 1;  //Whether to initialize the channel when the mount is driven
unsigned char fmt = 2;   //Video format :0->960H, 1->720P, 2->1080P_30, 3->1080P_25
unsigned char ntpal = 0; //0 ntsc, 1 pal
unsigned char yuv = 2;   //0 disable, 1: yuv422, 2: yuv420, 3: yuv420 legacy
unsigned int bit8 = 1;   //acp protocol 1:8bit or 0:16bit.

struct semaphore jaguar1_lock;
struct i2c_client* jaguar1_client;
static struct i2c_board_info hi_info =
{
    I2C_BOARD_INFO("jaguar1", 0x60),
};
decoder_get_information_str decoder_inform;

/*******************************************************************************
 *function name: change_uv
 input parameter:chn ,piont which input camera
                 value 1 mean uv
                 value 4 mean vu
 *******************************************************************************/
void change_uv(unsigned int chn,unsigned int value)
{
    int dev_num = 0,reg_value;
    reg_value = (value <<4) | 0x8;
    gpio_i2c_write(jaguar1_i2c_addr[dev_num], 0xFF, 0x00);
    gpio_i2c_write(jaguar1_i2c_addr[dev_num], 0x1c+chn, reg_value);
    return ;
}


/*******************************************************************************
 ** Description     : Check ID
 *  Argurments      : dec(slave address)
 *  Return value    : Device ID
 *  Modify          :
 *  warning         :
 *******************************************************************************/
static void vd_set_all(video_init_all* param)
{

    int i, dev_num = 0;
    video_input_init  video_val[4];
    yuv  = param->mipi_yuv_type;
    lane = param->mipi_lane_num;
    mclk = param->mipi_clk;
    mipi_datatype_set(yuv); // to do  VD_DATA_TYPE_YUV422
    mipi_tx_init(dev_num);

    for ( i = 0 ; i < 4 ; i++)
    {
        video_val[i].ch = param->ch_param[i].ch;
        video_val[i].format = param->ch_param[i].format;
        video_val[i].input = param->ch_param[i].input;

        if (i < chn)
        { video_val[i].ch_mipiout_en = param->ch_param[i].ch_mipiout_en; }
        else
        { video_val[i].ch_mipiout_en = DISABLE; }

        vd_jaguar1_init_set(&video_val[i]);
        mipi_video_format_set(&video_val[i]);
        change_uv(i,param->ch_param[i].uv_seq);
    }

    arb_init(dev_num);
    disable_parallel(dev_num);
}

/*******************************************************************************
 *	Description		: Check ID
 *	Argurments		: dec(slave address)
 *	Return value	: Device ID
 *	Modify			:
 *	warning			:
 *******************************************************************************/
int check_id(unsigned int dec)
{
    int ret;
    gpio_i2c_write(dec, 0xFF, 0x00);
    ret = gpio_i2c_read(dec, 0xf4);
    return ret;
}

/*******************************************************************************
 * Description      : Get rev ID
 * Argurments       : dec(slave address)
 * Return value     :
 * Modify           :
 * warning          :
 *******************************************************************************/
int check_rev(unsigned int dec)
{
    int ret;
    gpio_i2c_write(dec, 0xFF, 0x00);
    ret = gpio_i2c_read(dec, 0xf5);
    return ret;
}

/*******************************************************************************
 * Description      : Check decoder count
 * Argurments       : void
 * Return value     : (total chip count - 1) or -1(not found any chip)
 * Modify           :
 * warning          :
 *******************************************************************************/
int check_decoder_count(void)
{
    unsigned int chip = 0;
    unsigned int i = 0;
    int ret = -1;

    for (chip = 0; chip < 4; chip++)
    {
        chip_id[chip] = check_id(jaguar1_i2c_addr[chip]);
        rev_id[chip]  = check_rev(jaguar1_i2c_addr[chip]);

        if ( (chip_id[chip] != JAGUAR1_4PORT_R0_ID )  &&
             (chip_id[chip] != JAGUAR1_2PORT_R0_ID)   &&
             (chip_id[chip] != JAGUAR1_1PORT_R0_ID)   &&
             (chip_id[chip] != AFE_NVP6134E_R0_ID)
           )
        {
            printk("Device ID Error... %x, Chip Count:[%d]\n", chip_id[chip], chip);
            jaguar1_i2c_addr[chip] = 0xFF;
            chip_id[chip] = 0xFF;
        }
        else
        {
            printk("Device (0x%x) ID OK... %x , Chip Count:[%d]\n", jaguar1_i2c_addr[chip], chip_id[chip], chip);
            printk("Device (0x%x) REV %x\n", jaguar1_i2c_addr[chip], rev_id[chip]);
            jaguar1_i2c_addr[jaguar1_cnt] = jaguar1_i2c_addr[chip];

            if (jaguar1_cnt < chip)
            {
                jaguar1_i2c_addr[chip] = 0xFF;
            }

            chip_id[jaguar1_cnt] = chip_id[chip];
            rev_id[jaguar1_cnt]  = rev_id[chip];

            jaguar1_cnt++;
            #ifdef __HuaweiLite__
            break;
            #endif
        }

        if ((chip == 3) && (jaguar1_cnt < chip))
        {
            for (i = jaguar1_cnt; i < 4; i++)
            {
                chip_id[i] = 0xff;
                rev_id[i]  = 0xff;
            }
        }
    }

    printk("Chip Count = %d\n", jaguar1_cnt);
    printk("Address [0x%x][0x%x][0x%x][0x%x]\n", jaguar1_i2c_addr[0], jaguar1_i2c_addr[1], jaguar1_i2c_addr[2], jaguar1_i2c_addr[3]);
    printk("Chip Id [0x%x][0x%x][0x%x][0x%x]\n", chip_id[0], chip_id[1], chip_id[2], chip_id[3]);
    printk("Rev Id [0x%x][0x%x][0x%x][0x%x]\n", rev_id[0], rev_id[1], rev_id[2], rev_id[3]);

    for ( i = 0; i < 4; i++ )
    {
        decoder_inform.chip_id[i] = chip_id[i];
        decoder_inform.chip_rev[i] = rev_id[i];
        decoder_inform.chip_addr[i] = jaguar1_i2c_addr[i];
    }

    decoder_inform.Total_Chip_Cnt = jaguar1_cnt;
    ret = jaguar1_cnt;

    return ret;
}

/*******************************************************************************
 * Description      : video_config_init
 * Argurments       : void
 * Return value     : void
 * Modify           :
 * warning          :
 *******************************************************************************/
void video_decoder_init(void)
{
    int ii = 0;


    // Pad Control Setting
    gpio_i2c_write(jaguar1_i2c_addr[0], 0xff, 0x04);

    for (ii = 0; ii < 36; ii++)
    {
        gpio_i2c_write(jaguar1_i2c_addr[0], 0xa0 + ii , 0x24);
    }


    // Clock Delay Setting
    gpio_i2c_write(jaguar1_i2c_addr[0], 0xff, 0x01);

    for (ii = 0; ii < 4; ii++)
    {
        gpio_i2c_write(jaguar1_i2c_addr[0], 0xcc + ii , 0x64);
    }

    // MIPI_V_REG_OFF
    gpio_i2c_write(jaguar1_i2c_addr[0], 0xff, 0x21);
    gpio_i2c_write(jaguar1_i2c_addr[0], 0x07, 0x80);
    gpio_i2c_write(jaguar1_i2c_addr[0], 0x07, 0x00);

    printk("\nTEST: %s  %d\n", __FUNCTION__, __LINE__);

    // AGC_OFF  08.31
    gpio_i2c_write(jaguar1_i2c_addr[0], 0xff, 0x0A);
    gpio_i2c_write(jaguar1_i2c_addr[0], 0x77, 0x8F);
    gpio_i2c_write(jaguar1_i2c_addr[0], 0xF7, 0x8F);
    gpio_i2c_write(jaguar1_i2c_addr[0], 0xff, 0x0B);
    gpio_i2c_write(jaguar1_i2c_addr[0], 0x77, 0x8F);
    gpio_i2c_write(jaguar1_i2c_addr[0], 0xF7, 0x8F);
}

/*******************************************************************************
 * Description      : Driver open
 * Argurments       : void
 * Return value     : void
 * Modify           :
 * warning          :
 *******************************************************************************/
#ifndef __HuaweiLite__
int jaguar1_open(struct inode* inode, struct file* file)
{
    printk("[DRV] Jaguar1 Driver Open\n");
    printk("[DRV] Jaguar1 Driver Ver::%s\n", DRIVER_VER);
    return 0;
}
#else
int jaguar1_open(struct file* file)
{
    if (0 == open_cnt++)
    {
        printk("[DRV] Jaguar1 Driver Open\n");
        printk("[DRV] Jaguar1 Driver Ver::%s\n", DRIVER_VER);
        return 0;
    }
    printk("[DRV] Jaguar1 Driver you should close the device first!");
    return -1 ;
}
#endif
/*******************************************************************************
 * Description      : Driver clsoe
 * Argurments       :
 * Return value     :
 * Modify           :
 * warning          :
 *******************************************************************************/
#ifndef __HuaweiLite__
int jaguar1_close(struct inode* inode, struct file* file)
{
    printk("[DRV] Jaguar1 Driver Close\n");
    return 0;
}
#else
int jaguar1_close(struct file* file)
{
    open_cnt--;
    printk("[DRV] Jaguar1 Driver Close\n");
    return 0;
}
#endif
/*******************************************************************************
 *	Description		: Driver IOCTL function
 *	Argurments		:
 *	Return value	:
 *	Modify			:
 *	warning			:
 *******************************************************************************/
long jaguar1_ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{
    unsigned int __user* argp = (unsigned int __user*)arg;

    /* AllVideo Variable */
    video_init_all all_vd_val;

    /* Video Variable */
    video_input_init  video_val;
    video_output_init vo_seq_set;

    video_equalizer_info_s video_eq;
    video_video_loss_s vidloss;

    /* Coaxial Protocol Variable */
    NC_VD_COAX_STR           coax_val;
    NC_VD_COAX_BANK_DUMP_STR coax_bank_dump;
    FIRMWARE_UP_FILE_INFO    coax_fw_val;
    NC_VD_COAX_TEST_STR      coax_test_val;
    video_chnl_control       chnl_control;
    /* Motion Variable */
    motion_mode motion_set;

    printk("\n %s  %d down jaguar1_lock\n", __FUNCTION__, __LINE__);
    down(&jaguar1_lock);

    switch (cmd)
    {
            /*===============================================================================================
             * Set All - for MIPI Interface
             *===============================================================================================*/
        case IOC_VDEC_INIT_ALL:
            if (copy_from_user(&all_vd_val, argp, sizeof(video_init_all)))
            { printk("IOC_VDEC_INIT_ALL error\n"); }

            vd_set_all(&all_vd_val);
            break;
        case IOC_VDEC_START_CHNL:
            if (copy_from_user(&chnl_control, argp, sizeof(video_chnl_control)))
            { printk("IOC_VDEC_START_CHNL error\n"); }

            vd_jaguar1_start_chnl(chnl_control.devnum, chnl_control.chn);
            break;
        case IOC_VDEC_STOP_CHNL:
            if (copy_from_user(&chnl_control, argp, sizeof(video_chnl_control)))
            { printk("IOC_VDEC_STOP_CHNL error\n"); }

            vd_jaguar1_stop_chnl(chnl_control.devnum, chnl_control.chn);
            break;
            /*===============================================================================================
             * Video Initialize
             *===============================================================================================*/

        case IOC_VDEC_INPUT_INIT:
            if (copy_from_user(&video_val, argp, sizeof(video_input_init)))
            { printk("IOC_VDEC_INPUT_INIT error\n"); }

            vd_jaguar1_init_set(&video_val);
            break;

        case IOC_VDEC_OUTPUT_SEQ_SET:
            if (copy_from_user(&vo_seq_set, argp, sizeof(video_output_init)))
            { printk("IOC_VDEC_OUTPUT_SEQ_SET error\n"); }

            vd_jaguar1_vo_ch_seq_set(&vo_seq_set);
            break;

        case IOC_VDEC_VIDEO_EQ_SET:
            if (copy_from_user(&video_eq, argp, sizeof(video_equalizer_info_s)))
            { printk("IOC_VDEC_VIDEO_EQ_SET error\n"); }

            video_input_eq_val_set(&video_eq);
            break;

        case IOC_VDEC_VIDEO_SW_RESET:
            if (copy_from_user(&video_val, argp, sizeof(video_input_init)))
            { printk("IOC_VDEC_VIDEO_SW_RESET error\n"); }

            vd_jaguar1_sw_reset(&video_val);
            break;

        case IOC_VDEC_VIDEO_EQ_CABLE_SET:
            if (copy_from_user(&video_eq, argp, sizeof(video_equalizer_info_s)))
            { printk("IOC_VDEC_VIDEO_EQ_CABLE_SET error\n"); }

            video_input_eq_cable_set(&video_eq);
            break;

        case IOC_VDEC_VIDEO_EQ_ANALOG_INPUT_SET:
            if (copy_from_user(&video_eq, argp, sizeof(video_equalizer_info_s)))
            { printk("IOC_VDEC_VIDEO_EQ_ANALOG_INPUT_SET error\n"); }

            video_input_eq_analog_input_set(&video_eq);
            break;

        case IOC_VDEC_VIDEO_GET_VIDEO_LOSS:
            if (copy_from_user(&vidloss, argp, sizeof(video_video_loss_s)))
            { printk("IOC_VDEC_VIDEO_GET_VIDEO_LOSS error\n"); }

            vd_jaguar1_get_novideo(&vidloss);
            break;

            /*===============================================================================================
             * Coaxial Protocol
             *===============================================================================================*/
        case IOC_VDEC_COAX_TX_INIT:   //SK_CHANGE 170703
            if (copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
            { printk("IOC_VDEC_COAX_TX_INIT error\n"); }

            coax_tx_init(&coax_val);
            break;

        case IOC_VDEC_COAX_TX_16BIT_INIT:   //SK_CHANGE 170703
            if (copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
            { printk("IOC_VDEC_COAX_TX_INIT error\n"); }

            coax_tx_16bit_init(&coax_val);
            break;

        case IOC_VDEC_COAX_TX_CMD_SEND: //SK_CHANGE 170703
            if (copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
            { printk(" IOC_VDEC_COAX_TX_CMD_SEND error\n"); }

            coax_tx_cmd_send(&coax_val);
            break;

        case IOC_VDEC_COAX_TX_16BIT_CMD_SEND: //SK_CHANGE 170703
            if (copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
            { printk(" IOC_VDEC_COAX_TX_CMD_SEND error\n"); }

            coax_tx_16bit_cmd_send(&coax_val);
            break;

        case IOC_VDEC_COAX_TX_CVI_NEW_CMD_SEND: //SK_CHANGE 170703
            if (copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
            { printk(" IOC_VDEC_COAX_TX_CMD_SEND error\n"); }

            coax_tx_cvi_new_cmd_send(&coax_val);
            break;

        case IOC_VDEC_COAX_RX_INIT:
            if (copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
            { printk(" IOC_VDEC_COAX_RX_INIT error\n"); }

            coax_rx_init(&coax_val);
            break;

        case IOC_VDEC_COAX_RX_DATA_READ:
            if (copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
            { printk(" IOC_VDEC_COAX_RX_DATA_READ error\n"); }

            coax_rx_data_get(&coax_val);
            break;

        case IOC_VDEC_COAX_RX_BUF_CLEAR:
            if (copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
            { printk(" IOC_VDEC_COAX_RX_BUF_CLEAR error\n"); }

            coax_rx_buffer_clear(&coax_val);
            break;

        case IOC_VDEC_COAX_RX_DEINIT:
            if (copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
            { printk("IOC_VDEC_COAX_RX_DEINIT error\n"); }

            coax_rx_deinit(&coax_val);
            break;

        case IOC_VDEC_COAX_BANK_DUMP_GET:
            if (copy_from_user(&coax_bank_dump, argp, sizeof(NC_VD_COAX_BANK_DUMP_STR)))
            { printk("IOC_VDEC_COAX_BANK_DUMP_GET error\n"); }

            coax_test_Bank_dump_get(&coax_bank_dump);
            break;

        case IOC_VDEC_COAX_RX_DETECTION_READ:
            if (copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
            { printk(" IOC_VDEC_COAX_RX_DATA_READ error\n"); }

            coax_acp_rx_detect_get(&coax_val);
            break;

            /*===============================================================================================
             * Coaxial Protocol. Function
             *===============================================================================================*/
        case IOC_VDEC_COAX_RT_NRT_MODE_CHANGE_SET:
            if (copy_from_user(&coax_val, argp, sizeof(NC_VD_COAX_STR)))
            { printk(" IOC_VDEC_COAX_SHOT_SET error\n"); }

            if (JAGUAR1_INVALID_DEVID(coax_val.vd_dev))
            {
                printk("invalid chip_num, need [0,4) %d.\n ", coax_val.vd_dev);
            }
            else
            {
                coax_option_rt_nrt_mode_change_set(&coax_val);
            }
            break;

            /*===============================================================================================
             * Coaxial Protocol FW Update
             *===============================================================================================*/
        case IOC_VDEC_COAX_FW_ACP_HEADER_GET:
            if (copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
            { printk("IOC_VDEC_COAX_FW_READY_CMD_SET error\n"); }

            coax_fw_ready_header_check_from_isp_recv(&coax_fw_val);
            break;

        case IOC_VDEC_COAX_FW_READY_CMD_SET:
            if (copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
            { printk("IOC_VDEC_COAX_FW_READY_CMD_SET error\n"); }

            coax_fw_ready_cmd_to_isp_send(&coax_fw_val);
            break;

        case IOC_VDEC_COAX_FW_READY_ACK_GET:
            if (copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
            { printk("IOC_VDEC_COAX_FW_READY_ISP_STATUS_GET error\n"); }

            coax_fw_ready_cmd_ack_from_isp_recv(&coax_fw_val);
            break;

        case IOC_VDEC_COAX_FW_START_CMD_SET:
            if (copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
            { printk("IOC_VDEC_COAX_FW_START_CMD_SET error\n"); }

            coax_fw_start_cmd_to_isp_send(&coax_fw_val);
            break;

        case IOC_VDEC_COAX_FW_START_ACK_GET:
            if (copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
            { printk("IOC_VDEC_COAX_FW_START_CMD_SET error\n"); }

            coax_fw_start_cmd_ack_from_isp_recv(&coax_fw_val);
            break;

        case IOC_VDEC_COAX_FW_SEND_DATA_SET:
            if (copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
            { printk("IOC_VDEC_COAX_FW_START_CMD_SET error\n"); }

            coax_fw_one_packet_data_to_isp_send(&coax_fw_val);
            break;

        case IOC_VDEC_COAX_FW_SEND_ACK_GET:
            if (copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
            { printk("IOC_VDEC_COAX_FW_START_CMD_SET error\n"); }

            coax_fw_one_packet_data_ack_from_isp_recv(&coax_fw_val);
            break;

        case IOC_VDEC_COAX_FW_END_CMD_SET:
            if (copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
            { printk("IOC_VDEC_COAX_FW_START_CMD_SET error\n"); }

            coax_fw_end_cmd_to_isp_send(&coax_fw_val);
            break;

        case IOC_VDEC_COAX_FW_END_ACK_GET:
            if (copy_from_user(&coax_fw_val, argp, sizeof(FIRMWARE_UP_FILE_INFO)))
            { printk("IOC_VDEC_COAX_FW_START_CMD_SET error\n"); }

            coax_fw_end_cmd_ack_from_isp_recv(&coax_fw_val);
            break;

            /*===============================================================================================
             * Test Function
             *===============================================================================================*/
        case IOC_VDEC_COAX_TEST_TX_INIT_DATA_READ:
            if (copy_from_user(&coax_test_val, argp, sizeof(NC_VD_COAX_TEST_STR)))
            { printk("IOC_VDEC_COAX_INIT_SET error\n"); }

            coax_test_tx_init_read(&coax_test_val);
            break;

        case IOC_VDEC_COAX_TEST_DATA_SET:
            if (copy_from_user(&coax_test_val, argp, sizeof(NC_VD_COAX_TEST_STR)))
            { printk("IOC_VDEC_COAX_TEST_DATA_SET error\n"); }

            if (JAGUAR1_INVALID_DEVID(coax_test_val.chip_num))
            {
                printk("invalid chip_num, need [0,4) %d.\n ", coax_test_val.chip_num);
            }
            else
            {
                coax_test_data_set(&coax_test_val);
            }
            break;

        case IOC_VDEC_COAX_TEST_DATA_READ:
            if (copy_from_user(&coax_test_val, argp, sizeof(NC_VD_COAX_TEST_STR)))
            { printk("IOC_VDEC_COAX_TEST_DATA_SET error\n"); }

            if (JAGUAR1_INVALID_DEVID(coax_test_val.chip_num))
            {
                printk("invalid chip_num, need [0,4) %d.\n ", coax_test_val.chip_num);
            }
            else
            {
                coax_test_data_get(&coax_test_val);
            }
            break;

            /*===============================================================================================
             * Motion
             *===============================================================================================*/
        case IOC_VDEC_MOTION_DETECTION_GET :
            if (copy_from_user(&motion_set, argp, sizeof(motion_set)))
            { printk("IOC_VDEC_MOTION_SET error\n"); }

            if (JAGUAR1_INVALID_DEVID(motion_set.devnum))
            {
                printk("invalid dev num %d.\n ", motion_set.devnum);
            }
            else
            {
                motion_detection_get(&motion_set);
            }
            break;

        case IOC_VDEC_MOTION_SET :
            if (copy_from_user(&motion_set, argp, sizeof(motion_set)))
            { printk("IOC_VDEC_MOTION_SET error\n"); }

            if (JAGUAR1_INVALID_DEVID(motion_set.devnum))
            {
                printk("invalid dev num %d.\n ", motion_set.devnum);
            }
            else
            {
                motion_onoff_set(&motion_set);
            }
            break;

        case IOC_VDEC_MOTION_PIXEL_SET :
            if (copy_from_user(&motion_set, argp, sizeof(motion_set)))
            { printk("IOC_VDEC_MOTION_Pixel_SET error\n"); }

            if (JAGUAR1_INVALID_DEVID(motion_set.devnum))
            {
                printk("invalid dev num %d.\n ", motion_set.devnum);
            }
            else
            {
                motion_pixel_onoff_set(&motion_set);
            }
            break;

        case IOC_VDEC_MOTION_PIXEL_GET :
            if (copy_from_user(&motion_set, argp, sizeof(motion_set)))
            { printk("IOC_VDEC_MOTION_Pixel_SET error\n"); }

            if (JAGUAR1_INVALID_DEVID(motion_set.devnum))
            {
                printk("invalid dev num %d.\n ", motion_set.devnum);
            }
            else
            {
                motion_pixel_onoff_get(&motion_set);
            }
            break;

        case IOC_VDEC_MOTION_ALL_PIXEL_SET :
            if (copy_from_user(&motion_set, argp, sizeof(motion_set)))
            { printk("IOC_VDEC_MOTION_Pixel_SET error\n"); }

            if (JAGUAR1_INVALID_DEVID(motion_set.devnum))
            {
                printk("invalid dev num %d.\n ", motion_set.devnum);
            }
            else
            {
                motion_pixel_all_onoff_set(&motion_set);
            }
            break;

        case IOC_VDEC_MOTION_TSEN_SET :
            if (copy_from_user(&motion_set, argp, sizeof(motion_set)))
            { printk("IOC_VDEC_MOTION_TSEN_SET error\n"); }

            if (JAGUAR1_INVALID_DEVID(motion_set.devnum))
            {
                printk("invalid dev num %d.\n ", motion_set.devnum);
            }
            else
            {
                motion_tsen_set(&motion_set);
            }
            break;

        case IOC_VDEC_MOTION_PSEN_SET :
            if (copy_from_user(&motion_set, argp, sizeof(motion_set)))
            { printk("IOC_VDEC_MOTION_PSEN_SET error\n"); }

            if (JAGUAR1_INVALID_DEVID(motion_set.devnum))
            {
                printk("invalid dev num %d.\n ", motion_set.devnum);
            }
            else
            {
                motion_psen_set(&motion_set);
            }
            break;

        default:
            break;
    }

    up(&jaguar1_lock);
    printk("\n %s  %d up jaguar1_lock\n", __FUNCTION__, __LINE__);

    return 0;
}

/*******************************************************************************
 * Description      : i2c client initial
 * Argurments       : void
 * Return value     : 0
 * Modify           :
 * warning          :
 *******************************************************************************/
#ifndef __HuaweiLite__
static int i2c_client_init(void)
{
    struct i2c_adapter* i2c_adap;

    printk("[DRV] I2C Client Init \n");
    i2c_adap = i2c_get_adapter(I2C_1); /* yjtest I2C_1 -> I2C_0 */

    if (NULL != i2c_adap)
    {
        jaguar1_client = i2c_new_device(i2c_adap, &hi_info);
        i2c_put_adapter(i2c_adap);
    }
    else
    {
        return -1;
    }

    return 0;
}

#else

extern int hi_liteos_i2c_init(unsigned int i2c_num,unsigned int dev_addr);
static int i2c_client_init(void)
{
    int ret = 0;
    ret = hi_liteos_i2c_init(i2c_dev, (jaguar1_i2c_addr[0] >> 1));
    return ret;
}

#endif

/*******************************************************************************
 * Description      : i2c client release
 * Argurments       : void
 * Return value     : void
 * Modify           :
 * warning          :
 *******************************************************************************/
#ifndef __HuaweiLite__

static void i2c_client_exit(void)
{
    if (NULL != jaguar1_client)
    {
        i2c_unregister_device(jaguar1_client);
    }
}
#else

extern int hi_liteos_i2c_exit(void);
static void i2c_client_exit(void)
{
    hi_liteos_i2c_exit();
    return;
}
#endif


#ifdef __HuaweiLite__
const static struct file_operations_vfs jaguar1_fops =
{
    .open = jaguar1_open,
    .close = jaguar1_close,
    .ioctl = jaguar1_ioctl
};

#else
static struct file_operations jaguar1_fops =
{
    .owner      = THIS_MODULE,
    .unlocked_ioctl	= jaguar1_ioctl,
    .open       = jaguar1_open,
    .release    = jaguar1_close
};

static struct miscdevice jaguar1_dev =
{
    .minor		= MISC_DYNAMIC_MINOR,
    .name		= "jaguar1",
    .fops  		= &jaguar1_fops,
};
#endif

/*******************************************************************************
 * Description  : It is called when "insmod jaguar1.ko" command run
 * Argurments   : void
 * Return value :-1(could not register jaguar1 device), 0(success)
 * Modify       :
 * warning      :
 *******************************************************************************/
#ifndef __HuaweiLite__
static int __init jaguar1_module_init(void)
{
    video_init_all sVideoall;
    int ret = 0;
    int ch;
#ifdef FMT_SETTING_SAMPLE
    //int dev_num = 0;
#endif

    ret = misc_register(&jaguar1_dev);

    if (ret)
    {
        printk("ERROR: could not register jaguar1-i2c :%#x \n", ret);
        return -1;
    }

    if(i2c_client_init()<0)
    {
        printk("ERROR: jaguar1 client init failed!");
        misc_deregister(&jaguar1_dev);

        return -1;
    }

    /* decoder count function */
    ret = check_decoder_count();

    if (ret == -1)
    {
        printk("ERROR: could not find jaguar1 devices:%#x \n", ret);
        return ret;
    }
	video_decoder_init();
    printk("\n %s  %d init & down jaguar1_lock\n", __FUNCTION__, __LINE__);
    /* initialize semaphore */
    sema_init(&jaguar1_lock, 1);
    printk("\n %s  %d up jaguar1_lock\n", __FUNCTION__, __LINE__);
    return 0;
}
#else

int jaguar1_module_init(unsigned int i2c_dev_num)
{
    int ret = 0;
    ret = register_driver("/dev/jaguar1", &jaguar1_fops, 0666, 0);
    if (ret) {
        printk(" ERROR: could not register jaguar1 device");
        return -1;
    }
    i2c_dev = i2c_dev_num;
    ret = i2c_client_init();
    if (ret == -1) {
        printk("ERROR: jaguar1  i2c_client_init failed\n");
        unregister_driver("/dev/jaguar1");
        return -1;
    }

    ret = check_decoder_count();
    if (ret == -1)
    {
        printk("ERROR: could not find jaguar1 devices:%#x \n", ret);
        return ret;
    }
	video_decoder_init();
    sema_init(&jaguar1_lock, 1);
    printk(" --->>load jaguar1 success\n");
    return 0;
}
#endif

/*******************************************************************************
 *Description  : It is called when "rmmod nvp61XX_ex.ko" command run
 *Argurments   : void
 *Return value : void
 *Modify       :
 *Warning      :
 *******************************************************************************/
#ifndef __HuaweiLite__

static void __exit jaguar1_module_exit(void)
{
    misc_deregister(&jaguar1_dev);
    i2c_client_exit();

    printk("JAGUAR1 DEVICE DRIVER UNLOAD SUCCESS\n");
}

module_init(jaguar1_module_init);
module_exit(jaguar1_module_exit);

MODULE_LICENSE("GPL");
#else
int jaguar1_module_deinit(void)
{
  unregister_driver("/dev/jaguar1");
  i2c_client_exit();
  printk("JAGUAR1 DEVICE DRIVER UNLOAD SUCCESS\n");
  return 0;
}
#endif
/*******************************************************************************
 *end of file
 *******************************************************************************/
