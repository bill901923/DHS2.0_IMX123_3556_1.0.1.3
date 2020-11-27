/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: Jaguar1 Device Driver
 *  Description	: common.h
 *  Author		:
 *  Date         :
 *  Version		: Version 1.0
 *
 ********************************************************************************
 *  History      :
 *
 *
 ********************************************************************************/
#ifndef __COMMON_H__
#define __COMMON_H__

#include <linux/kernel.h>

unsigned char __I2CReadByte8(unsigned char devaddress, unsigned char address);
void __I2CWriteByte8(unsigned char devaddress, unsigned char address, unsigned char data);
#define  gpio_i2c_read   __I2CReadByte8
#define  gpio_i2c_write  __I2CWriteByte8

#define DRIVER_VER "1.1.1"

#define JAGUAR1_MAX_CHAN_CNT 4

extern unsigned char jaguar1_i2c_addr[4];

#define HI_CHIPID_BASE 0x12050000
#define HI_CHIPID0 IO_ADDRESS(HI_CHIPID_BASE + 0xEEC)
#define HI_CHIPID1 IO_ADDRESS(HI_CHIPID_BASE + 0xEE8)
#define HI_CHIPID2 IO_ADDRESS(HI_CHIPID_BASE + 0xEE4)
#define HI_CHIPID3 IO_ADDRESS(HI_CHIPID_BASE + 0xEE0)
//#define HW_REG(reg) *((volatile unsigned int *)(reg))

#define _SET_BIT(data,bit) ((data)|=(1<<(bit)))
#define _CLE_BIT(data,bit) ((data)&=(~(1<<(bit))))

#define JAGUAR1_BANK_CHANGE(bank) gpio_i2c_write(jaguar1_i2c_addr[0], 0xFF, bank );

#define PORTA		0x00
#define PORTB		0x01
#define PORTC		0x02
#define PORTD		0x03
#define PORTAB		0x04
#define PORTCD		0x05

#define FUNC_ON		0x01
#define FUNC_OFF	0x00

#define BANK_0  0x00
#define BANK_1  0x01
#define BANK_2  0x02
#define BANK_3  0x03
#define BANK_4  0x04
#define BANK_5  0x05
#define BANK_A  0x0A
#define BANK_B  0x0B
#define BANK_11 0x11
#define BANK_13 0x13
#define BANK_20 0x20
#define BANK_21 0x21
#define BANK_22 0x22


typedef struct _decoder_get_information_str
{
    unsigned char chip_id[4];
    unsigned char chip_rev[4];
    unsigned char chip_addr[4];

    unsigned char Total_Port_Num;
    unsigned char Total_Chip_Cnt;

} decoder_get_information_str;

typedef struct _decoder_dev_ch_info_s
{
    unsigned char ch;
    unsigned char devnum;
    unsigned char fmt_def;
} decoder_dev_ch_info_s;


typedef enum NC_FORMAT_FPS
{
    FMT_FPS_UNKNOWN = 0,
    FMT_NT = 1,
    FMT_PAL,
    FMT_12_5P,
    FMT_7_5P,
    FMT_30P,
    FMT_25P,
    FMT_50P,
    FMT_60P,
    FMT_15P,
    FMT_18P,
    FMT_18_75P,
    FMT_20P,

    FMT_FPS_MAX,

} NC_FORMAT_FPS;

//#define FMT_AUTO (-1)

typedef enum NC_FORMAT_STANDARD
{
    FMT_STD_UNKNOWN = 0,
    FMT_SD,
    FMT_AHD20,
    FMT_AHD30,
    FMT_TVI,
    FMT_CVI,

    FMT_AUTO,		// FIXME

    FMT_STD_MAX,

} NC_FORMAT_STANDARD;


typedef enum NC_FORMAT_RESOLUTION
{
    FMT_RESOL_UNKNOWN = 0,
    FMT_SH720,
    FMT_H960,
    FMT_H1280,
    FMT_H1440,
    FMT_H960_EX,
    FMT_H960_2EX,
    FMT_H960_Btype_2EX,
    FMT_720P,
    FMT_720P_EX,
    FMT_720P_Btype,
    FMT_720P_Btype_EX,
    FMT_1080P,
    FMT_3M,
    FMT_4M,
    FMT_5M,
    FMT_5_3M,
    FMT_6M,
    FMT_8M_X,
    FMT_8M,
    FMT_960P,

    FMT_H960_Btype_2EX_SP,
    FMT_720P_Btype_EX_SP,

    FMT_RESOL_MAX,
} NC_FORMAT_RESOLUTION;


typedef enum NC_VIVO_CH_FORMATDEF
{
    NC_VIVO_CH_FORMATDEF_UNKNOWN = 0,
    NC_VIVO_CH_FORMATDEF_AUTO,

    AHD20_SD_H960_NT,
    AHD20_SD_H960_PAL,
    AHD20_SD_SH720_NT,
    AHD20_SD_SH720_PAL,
    AHD20_SD_H1280_NT,
    AHD20_SD_H1280_PAL,
    AHD20_SD_H1440_NT,
    AHD20_SD_H1440_PAL,
    AHD20_SD_H960_EX_NT,
    AHD20_SD_H960_EX_PAL,
    AHD20_SD_H960_2EX_NT,
    AHD20_SD_H960_2EX_PAL,
    AHD20_SD_H960_2EX_Btype_NT,    //960x480i
    AHD20_SD_H960_2EX_Btype_PAL,   //960x576i
    AHD20_1080P_60P, // For Test
    AHD20_1080P_50P, // For Test
    AHD20_1080P_30P,              //1920x1080@30fps
    AHD20_1080P_25P,              //1920x1080@25fps
    AHD20_720P_60P,
    AHD20_720P_50P,
    AHD20_720P_30P,
    AHD20_720P_25P,
    AHD20_720P_30P_EX,
    AHD20_720P_25P_EX,
    AHD20_720P_30P_EX_Btype,      //1280x720@30fps
    AHD20_720P_25P_EX_Btype,      //1280x720@25fps
    AHD20_720P_960P_30P,          //1280x960@30fps
    AHD20_720P_960P_25P,          //1280x960@25fps

    AHD30_4M_30P,
    AHD30_4M_25P,
    AHD30_4M_15P,
    AHD30_3M_30P,
    AHD30_3M_25P,
    AHD30_3M_18P,
    AHD30_5M_12_5P,
    AHD30_5M_20P,
    AHD30_5_3M_20P,
    AHD30_6M_18P,
    AHD30_6M_20P,
    AHD30_8M_X_30P,
    AHD30_8M_X_25P,
    AHD30_8M_7_5P,
    AHD30_8M_12_5P,
    AHD30_8M_15P,

    TVI_FHD_30P,
    TVI_FHD_25P,
    TVI_HD_60P,
    TVI_HD_50P,
    TVI_HD_30P,
    TVI_HD_25P,
    TVI_HD_30P_EX,
    TVI_HD_25P_EX,
    TVI_HD_B_30P,
    TVI_HD_B_25P,
    TVI_HD_B_30P_EX,
    TVI_HD_B_25P_EX,
    TVI_3M_18P,
    TVI_5M_12_5P,
    TVI_4M_30P,
    TVI_4M_25P,
    TVI_4M_15P,

    CVI_FHD_30P,
    CVI_FHD_25P,
    CVI_HD_60P,
    CVI_HD_50P,
    CVI_HD_30P,
    CVI_HD_25P,
    CVI_HD_30P_EX,
    CVI_HD_25P_EX,
    CVI_4M_30P,
    CVI_4M_25P,
    CVI_8M_15P,
    CVI_8M_12_5P,

    AHD20_SD_H960_2EX_Btype_SP_NT,
    AHD20_SD_H960_2EX_Btype_SP_PAL,

    AHD20_720P_30P_EX_Btype_SP,
    AHD20_720P_25P_EX_Btype_SP,

    NC_VIVO_CH_FORMATDEF_MAX,

} NC_VIVO_CH_FORMATDEF;

typedef enum NC_OUTPUT_MUX_MODE
{
    NC_MX_MUX1 = 0,
    NC_MX_MUX2,
    NC_MX_MUX4,
} NC_OUTPUT_MUX_MODE;

typedef enum NC_OUTPUT_INTERFACE
{
    NC_OI_BT656 = 0,              /* ITU-R BT.656 YUV4:2:2 */
    NC_OI_BT601,                  /* ITU-R BT.601 YUV4:2:2 */
    NC_OI_DIGITAL_CAMERA,         /* digatal camera mode */
    NC_OI_BT1120_STANDARD,        /* BT.1120 progressive mode */
    NC_OI_BT1120_INTERLEAVED,     /* BT.1120 interstage mode */
} NC_OUTPUT_INTERFACE;

typedef enum NC_OUTPUT_EDGE
{
    NC_OE_SINGLE_UP = 0,         /* single-edge mode and in rising edge */
    NC_OE_SINGLE_DOWN,           /* single-edge mode and in falling edge */
    NC_OE_DOUBLE ,
} NC_OUTPUT_EDGE;



typedef enum NC_ANALOG_INPUT
{
    SINGLE_ENDED = 0,
    DIFFERENTIAL,
} NC_ANALOG_INPUT;

typedef enum NC_CABLE
{
    CABLE_A = 0,
    CABLE_B,
    CABLE_C,
    CABLE_D,
} NC_CABLE;

typedef enum NC_STAGE
{
    STAGE_0 = 0,
    STAGE_1,
    STAGE_2,
    STAGE_3,
    STAGE_4,
    STAGE_5,
} NC_STAGE;

typedef enum NC_JAGUAR1_EQ
{
    NC_EQ_SETTING_FMT_UNKNOWN = 0,

    AHD20_SD_H720_NT_SINGLE_ENDED,
    AHD20_SD_H720_NT_DIFFERENTIAL,
    AHD20_SD_H720_PAL_SINGLE_ENDED,
    AHD20_SD_H720_PAL_DIFFERENTIAL,

    AHD20_SD_H960_2EX_Btype_NT_SINGLE_ENDED,
    AHD20_SD_H960_2EX_Btype_NT_DIFFERENTIAL,
    AHD20_SD_H960_2EX_Btype_PAL_SINGLE_ENDED,
    AHD20_SD_H960_2EX_Btype_PAL_DIFFERENTIAL,

    AHD20_1080P_30P_SINGLE_ENDED,
    AHD20_1080P_30P_DIFFERENTIAL,
    AHD20_1080P_25P_SINGLE_ENDED,
    AHD20_1080P_25P_DIFFERENTIAL,

    AHD20_720P_60P_SINGLE_ENDED,
    AHD20_720P_60P_DIFFERENTIAL,
    AHD20_720P_50P_SINGLE_ENDED,
    AHD20_720P_50P_DIFFERENTIAL,

    AHD20_720P_30P_SINGLE_ENDED,
    AHD20_720P_30P_DIFFERENTIAL,
    AHD20_720P_25P_SINGLE_ENDED,
    AHD20_720P_25P_DIFFERENTIAL,

    AHD20_720P_30P_EX_SINGLE_ENDED,
    AHD20_720P_30P_EX_DIFFERENTIAL,
    AHD20_720P_25P_EX_SINGLE_ENDED,
    AHD20_720P_25P_EX_DIFFERENTIAL,

    AHD20_720P_30P_EX_Btype_SINGLE_ENDED,
    AHD20_720P_30P_EX_Btype_DIFFERENTIAL,
    AHD20_720P_25P_EX_Btype_SINGLE_ENDED,
    AHD20_720P_25P_EX_Btype_DIFFERENTIAL,

    AHD20_960P_30P_SINGLE_ENDED,
    AHD20_960P_30P_DIFFERENTIAL,
    AHD20_960P_25P_SINGLE_ENDED,
    AHD20_960P_25P_DIFFERENTIAL,

    TVI_FHD_30P_SINGLE_ENDED,
    TVI_FHD_30P_DIFFERENTIAL,
    TVI_FHD_25P_SINGLE_ENDED,
    TVI_FHD_25P_DIFFERENTIAL,

    TVI_HD_60P_SINGLE_ENDED,
    TVI_HD_60P_DIFFERENTIAL,
    TVI_HD_50P_SINGLE_ENDED,
    TVI_HD_50P_DIFFERENTIAL,

    TVI_HD_30P_SINGLE_ENDED,
    TVI_HD_30P_DIFFERENTIAL,
    TVI_HD_25P_SINGLE_ENDED,
    TVI_HD_25P_DIFFERENTIAL,

    TVI_HD_30P_EX_SINGLE_ENDED,
    TVI_HD_30P_EX_DIFFERENTIAL,
    TVI_HD_25P_EX_SINGLE_ENDED,
    TVI_HD_25P_EX_DIFFERENTIAL,

    TVI_HD_B_30P_SINGLE_ENDED,
    TVI_HD_B_30P_DIFFERENTIAL,
    TVI_HD_B_25P_SINGLE_ENDED,
    TVI_HD_B_25P_DIFFERENTIAL,

    TVI_HD_B_30P_EX_SINGLE_ENDED,
    TVI_HD_B_30P_EX_DIFFERENTIAL,
    TVI_HD_B_25P_EX_SINGLE_ENDED,
    TVI_HD_B_25P_EX_DIFFERENTIAL,

    CVI_FHD_30P_SINGLE_ENDED,
    CVI_FHD_30P_DIFFERENTIAL,
    CVI_FHD_25P_SINGLE_ENDED,
    CVI_FHD_25P_DIFFERENTIAL,

    CVI_HD_60P_SINGLE_ENDED,
    CVI_HD_60P_DIFFERENTIAL,
    CVI_HD_50P_SINGLE_ENDED,
    CVI_HD_50P_DIFFERENTIAL,

    CVI_HD_30P_SINGLE_ENDED,
    CVI_HD_30P_DIFFERENTIAL,
    CVI_HD_25P_SINGLE_ENDED,
    CVI_HD_25P_DIFFERENTIAL,

    CVI_HD_30P_EX_SINGLE_ENDED,
    CVI_HD_30P_EX_DIFFERENTIAL,
    CVI_HD_25P_EX_SINGLE_ENDED,
    CVI_HD_25P_EX_DIFFERENTIAL,

    NC_EQ_SETTING_FMT_MAX,


} NC_JAGUAR1_EQ;

typedef enum NC_D2S_OUTPUT_INTERFACE
{
    DISABLE = 0,
    ENABLE,
} NC_D2S_OUTPUT_INTERFACE;

typedef struct _NC_DEOCDER_SET_STR
{
    NC_VIVO_CH_FORMATDEF FmtDef;
    NC_FORMAT_STANDARD fmt_std;
    NC_FORMAT_RESOLUTION fmt_res;
    NC_FORMAT_FPS fmt_fps;
    NC_ANALOG_INPUT input;
    NC_D2S_OUTPUT_INTERFACE ch_mipiout_en;
} NC_DEOCDER_SET_STR;


#define IOC_VDEC_INIT_ALL                     0xF0
#define IOC_VDEC_START_CHNL                   0xF1
#define IOC_VDEC_STOP_CHNL                    0xF2
#define IOC_VDEC_VIDEO_GET_VIDEO_LOSS         0x18

typedef struct _video_input_init
{
    unsigned char ch;            //channel number.0,1,2 or 3.
    unsigned char format;        //video format.refer to NC_VIVO_CH_FORMATDEF
    unsigned char dist;          //distance.asure = 0.
    unsigned char input;         //0:single end.1:differential
    unsigned char val;           //reserved
    unsigned char uv_seq;     // 1 mean uv, 4 mean vu
    unsigned char ch_mipiout_en;     //disable or enable
} video_input_init;

typedef struct _video_init_all
{
    video_input_init ch_param[4];  //channel parameters
    unsigned char dev_num;          //device number. 0,1,2 or 3
    unsigned char mipi_lane_num;   //mipi lane number.2 or 4
    unsigned char mipi_yuv_type;   //0 disable, 1: yuv422, 2: yuv420, 3: yuv420 legacy
    unsigned char mipi_clk;        //0:756 1:594 2:378 3:1242
} video_init_all;

typedef struct _video_chnl_control
{
    unsigned char devnum;
    unsigned char chn;
} video_chnl_control;


#ifdef __HuaweiLite__
extern int jaguar1_module_init(unsigned int i2c_dev_num);
extern int jaguar1_module_deinit(void);
#endif
#endif   //#ifndef __COMMON_H__
