#ifndef __NC_VIVO_DEF_H__
#define __NC_VIVO_DEF_H__

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
    AHD20_SD_H960_2EX_Btype_NT,     //960x480i
    AHD20_SD_H960_2EX_Btype_PAL,    //960x576i
    AHD20_1080P_60P, // For Test
    AHD20_1080P_50P, // For Test
    AHD20_1080P_30P,                //1920x1080@30fps
    AHD20_1080P_25P,                //1920x1080@25fps
    AHD20_720P_60P,
    AHD20_720P_50P,
    AHD20_720P_30P,
    AHD20_720P_25P,
    AHD20_720P_30P_EX,
    AHD20_720P_25P_EX,
    AHD20_720P_30P_EX_Btype,        //1280x720@30fps
    AHD20_720P_25P_EX_Btype,        //1280x720@25fps
    AHD20_720P_960P_30P,            //1280x960@30fps
    AHD20_720P_960P_25P,            //1280x960@25fps

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

typedef enum NC_CH
{
    CH1 = 0,
    CH2,
    CH3,
    CH4,
} NC_CH;

typedef enum NC_PORT
{
    PORT1 = 0,
    PORT2,
    PORT3,
    PORT4
} NC_PORT;

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

typedef struct _NC_DEOCDER_SET_STR_ALL
{
    NC_DEOCDER_SET_STR ch_param[4];
} NC_DEOCDER_SET_STR_ALL;

#endif
