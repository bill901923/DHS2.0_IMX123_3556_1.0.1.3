/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: Jaguar1 Device Driver
 *  Description	: coax_table.h
 *  Author		:
 *  Date         :
 *  Version		: Version 1.0
 *
 ********************************************************************************
 *  History      :
 *
 *
 ********************************************************************************/
#ifndef _JAGUAR1_VIDEO_TABLE_
#define _JAGUAR1_VIDEO_TABLE_

#include "jaguar1_video.h"

#define EOD (-1)

/*=============================================================
 * Video Initialize Structure
 ==============================================================*/
typedef struct NC_VD_VI_Init_STR
{
    char* name;

    // decoder_afe_fmtdef
    unsigned char powerdown;
    unsigned char gain;
    unsigned char spd;
    unsigned char ctrlreg;
    unsigned char ctrlibs;
    unsigned char adcspd;
    unsigned char clplevel;
    unsigned char eq_band;
    unsigned char lpf_front_band;
    unsigned char clpmode;
    unsigned char f_lpf_bypass;
    unsigned char clproff;
    unsigned char b_lpf_bypass;
    unsigned char duty;
    unsigned char ref_vol;
    unsigned char lpf_back_band;
    unsigned char clk_sel;
    unsigned char eq_gainsel;

    // decoder_color_fmtdef
    unsigned char brightnees;		//B0		0x0c/0x0d/0x0e/0x0f
    unsigned char contrast;			//B0 		0x10/0x11/0x12/0x13
    unsigned char hue;				//B0		0x40/0x41/0x42/0x43
    unsigned char u_gain;         	//B0		0x44/0x45/0x46/0x47
    unsigned char v_gain;         	//B0		0x48/0x49/0x4a/0x4b
    unsigned char u_offset;       	//B0		0x4c/0x4d/0x4e/0x4f
    unsigned char v_offset;       	//B0		0x50/0x51/0x52/0x53
    unsigned char saturation_a;   	//B0		0x3c/0x3d/0x3e/0x3f
    unsigned char saturation_b;		//B5/6/7/8	0x2b
    unsigned char black_level;  	//B5/6/7/8	0x20
    unsigned char burst_dec_a;		//B5/6/7/8	0x24
    unsigned char burst_dec_b;      //B5/6/7/8	0x5f
    unsigned char burst_dec_c;    	//B5/6/7/8	0xd1
    unsigned char new_wpd_sel;		//B5/6/7/8 	0x75				[0]

    // decoder_basic_vfmt_fmtdef
    unsigned char video_format;		//B0		0x08/0x09/0x0a/0x0b
    unsigned char sd_mode;			//B0 		0x81/0x82/0x83/0x84
    unsigned char ahd_mode;
    unsigned char spl_mode;			//B0		0x85/0x86/0x87/0x88
    unsigned char sd_freq_sel;   	//B5/6/7/8	0x69[0]

    // decoder_basic_chroma_fmtdef
    unsigned char	pal_cm_off;		//B0		0x21/0x25/0x29/0x2d
    unsigned char	s_point;		//B5/6/7/8 	0x28
    unsigned char	fsc_lock_mode;	//B5/6/7/8	0x25
    unsigned char	comb_mode;    	//B5/6/7/8	0x90

    // decoder_basic_timing_fmtdef
    unsigned char	sync_rs;		//B5/6/7/8	0x47
    unsigned char	h_delay_lsb;	//B0 		0x58/0x59/0x5a/0x5b
    unsigned char	h_mask_on;		//B0		0x89/0x8a/0x8b/0x8c
    unsigned char	h_mask_sel;     //B0		0x8e/0x8f/0x90/0x91
    unsigned char 	mem_rdp;		//B5/6/7/8	0x64
    unsigned char	v_blk_end_b;	//B5/6/7/8	0xA9
    unsigned char	y_delay;		//B0		0xA0/0xA1/0xA2/0xA3
    unsigned char	fld_inv;		//B0		0x54[4]/[5]/[6]/[7]
    unsigned char	v_blk_end_a;	//B0		0x64/0x65/0x66/0x67

    // decoder_basic_hscaler_fmtdef
    unsigned char	h_down_scaler;	//B9		0x96/0xb6/0xd6/0xf6
    unsigned char	h_scaler_mode;	//B9	 	0x97/0xb7/0xd7/0xf7
    unsigned char	ref_base_lsb;	//B9		0x98/0xb8/0xd8/0xf8
    unsigned char	ref_base_msb;  	//B9		0x99/0xb9/0xd9/0xf9
    unsigned char	line_mem_mode;	//B0		0x34/0x35/0x36/0x37

    // decoder_basic_hpll_fmtdef
    unsigned char	hpll_mask_on;	//B5/6/7/8		0x50
    unsigned char	hafc_byp_th_e;	//B5/6/7/8	 	0xbb
    unsigned char	hafc_byp_th_s;	//B5/6/7/8		0xb7
    unsigned char	hafc_op_md;  	//B5/6/7/8		0xb8

    // clock
    unsigned char clk_adc;
    unsigned char clk_pre;
    unsigned char clk_post;

    // Reserved
    unsigned char h_dly_msb;;
    unsigned char vblk_end_sel;
    unsigned char vblk_end_ext;
    unsigned char h_scaler_active;
    unsigned char cml_mode;
    unsigned char agc_op;
    unsigned char g_sel;
    unsigned char sync_sel;
    unsigned char reserved9;
    unsigned char reserved10;

} NC_VD_VI_Init_STR;


typedef struct NC_VD_VO_Init_STR
{
    char* name;
    unsigned char vport_oclk_sel;   // 1xcc[6:4]
    unsigned char mux_yc_merge;     // 1xec[0]
    unsigned char port_seq_ch01[4]; // 1xc0[7:0]
    unsigned char port_seq_ch23[4]; // 1xc1[7:0]
    unsigned char chid_vin;         // 0x14[4:0]
    unsigned char vport_out_sel;    // 1xc8[5:0]  0x30 [low bit 0 1mux, 2 2mux, 8 4mux]

    unsigned char manual_ahd;       // 13x31[5:0]
    unsigned char manual_cvi;       // 13x32[5:0]
    unsigned char manual_tvi;       // 13x33[5:0]

} NC_VD_VO_Init_STR;


/*======================================================
 * This is a variable structure added in Jaguar1.
 *======================================================= */
typedef struct NC_VD_Jaguar1_Init_STR
{
    char* name;

    unsigned char vadc_clk_sel;
    unsigned char vafe_clp_level;
    unsigned char vafe_f_lpf_sel;
    unsigned char clp_mode_sel;
    unsigned char vafe_duty;      // 0x66 > 5x5b

    unsigned char novid_det_b;
    unsigned char h_dly_msb;
    unsigned char vblk_end;

    unsigned char vport_oclk_sel;
    unsigned char new_c_peak_mode;
    unsigned char auto_gnos_mode;
    unsigned char y_temp_sel;
    unsigned char no_mem_path;

    unsigned char a_cmp_pw_mode;
    unsigned char eoh_rst_dly;

    unsigned char hafc_lpf_sel;

    unsigned char tm_clk_sel;   // 1xa0

} NC_VD_Jaguar1_Init_STR;


/*=============================================================
 * Video Initialize Table
 ==============================================================*/

NC_VD_VI_Init_STR vd_vi_init_list[] =
{
    /* =====================================================
     * SD
     * =====================================================*/
    [ AHD20_SD_H960_NT ] = {

    },

    [ AHD20_SD_H960_PAL ] = {

    },

    [ AHD20_SD_SH720_NT ] = {
        .name = "AHD20_SD_SH720_NT",
        .powerdown  	= 0x00,
        .gain  			= 0x01,
        .spd  			= 0x0D,
        .ctrlreg  		= 0x01,
        .ctrlibs  		= 0x02,
        .adcspd  		= 0x00,
        .clplevel  		= 0x02,
        .eq_band  		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode  	   	= 0x00,
        .f_lpf_bypass  	= 0x01,
        .clproff  	   	= 0x00,
        .b_lpf_bypass  	= 0x00,
        .duty  		   	= 0x04,
        .ref_vol  	   	= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel  	   	= 0x01,
        .eq_gainsel    	= 0x00,

        .new_wpd_sel  	= 0x00,
        .brightnees 	= 0x00,
        .contrast  		= 0x8c,
        .black_level  	= 0x80,
        .hue  			= 0x00,
        .u_gain  		= 0x10,
        .v_gain  		= 0x10,
        .u_offset  		= 0xF8,
        .v_offset  	  	= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a  	= 0x2A,
        .burst_dec_b  	= 0x00,
        .burst_dec_c  	= 0x00,

        .video_format  	= 0xa0,
        .sd_mode  		= 0x04,
        .ahd_mode  		= 0x00,
        .spl_mode  		= 0x00,
        .sd_freq_sel  	= 0x0,

        .pal_cm_off  	= 0x82,
        .s_point  		= 0xd0,
        .fsc_lock_mode  = 0xDC,
        .comb_mode  	= 0x1,

        .sync_rs  		= 0x04,
        .h_delay_lsb  	= 0x80,
        .h_mask_on  	= 0x1,
        .h_mask_sel  	= 0xd,
        .mem_rdp  		= 0x01,
        .v_blk_end_b  	= 0x1c,
        .y_delay  		= 0x10,
        .fld_inv  		= 0x0,
        .v_blk_end_a  	= 0x80,

        .h_down_scaler  = 0x10,
        .h_scaler_mode  = 0x10,
        .ref_base_lsb  	= 0x0,
        .ref_base_msb  	= 0x0,
        .line_mem_mode  = 0x00,

        .hpll_mask_on  	= 0x80,
        .hafc_byp_th_e  = 0xF,
        .hafc_byp_th_s  = 0xFC,
        .hafc_op_md  	= 0x39,

        .clk_adc  		= 0x06,
        .clk_pre  		= 0x07,
        .clk_post 		= 0x01,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x20,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_SD_SH720_PAL ] = {
        .name = "AHD20_SD_SH720_PAL",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel 	= 0x00,
        .brightnees 	= 0x00,
        .contrast		= 0x86,
        .black_level 	= 0x80,
        .hue 			= 0x00,
        .u_gain 		= 0x00,
        .v_gain 		= 0x00,
        .u_offset 		= 0xF8,
        .v_offset 		= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0x00,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0xdd,
        .sd_mode 		= 0x05,
        .ahd_mode 		= 0x00,
        .spl_mode 		= 0x00,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off  	= 0x02,
        .s_point  		= 0xd0,
        .fsc_lock_mode  = 0xcc,
        .comb_mode  	= 0x0d,

        .sync_rs  		= 0x04,
        .h_delay_lsb  	= 0xa0,
        .h_mask_on  	= 0x1,
        .h_mask_sel  	= 0xd,
        .mem_rdp  		= 0x01,
        .v_blk_end_b  	= 0x0a,
        .y_delay  		= 0x10,
        .fld_inv  		= 0x1,
        .v_blk_end_a  	= 0x00,

        .h_down_scaler 	= 0x10,
        .h_scaler_mode 	= 0x10,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0x80,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0xB9,

        .clk_adc 		= 0x06,
        .clk_pre 		= 0x07,
        .clk_post 		= 0x01,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x20,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_SD_H1280_NT ] = {

    },

    [ AHD20_SD_H1280_PAL ] = {

    },

    [ AHD20_SD_H1440_NT ] = {

    },

    [ AHD20_SD_H1440_PAL ] = {

    },

    [ AHD20_SD_H960_EX_NT ] = {

    },

    [ AHD20_SD_H960_EX_PAL ] = {

    },

    [ AHD20_SD_H960_2EX_NT ] = {

    },

    [ AHD20_SD_H960_2EX_PAL ] = {

    },

    [ AHD20_SD_H960_2EX_Btype_NT ] = {
        .name = "AHD20_SD_H960_2EX_Btype_NT",
        .powerdown  	= 0x00,
        .gain  			= 0x01,
        .spd  			= 0x0D,
        .ctrlreg  		= 0x01,
        .ctrlibs  		= 0x02,
        .adcspd  		= 0x00,
        .clplevel  		= 0x02,
        .eq_band  		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode  	   	= 0x00,
        .f_lpf_bypass  	= 0x01,
        .clproff  	   	= 0x00,
        .b_lpf_bypass  	= 0x00,
        .duty  		   	= 0x04,
        .ref_vol  	   	= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel  	   	= 0x01,
        .eq_gainsel    	= 0x00,

        .new_wpd_sel  	= 0x00,
        .brightnees 	= 0x00,
        .contrast  		= 0x8c,
        .black_level  	= 0x80,
        .hue  			= 0x00,
        .u_gain  		= 0x10,
        .v_gain  		= 0x10,
        .u_offset  		= 0xF8,
        .v_offset  	  	= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a  	= 0x2A,
        .burst_dec_b  	= 0x00,
        .burst_dec_c  	= 0x00,

        .video_format  	= 0xa0,
        .sd_mode  		= 0x0e,
        .ahd_mode  		= 0x00,
        .spl_mode  		= 0x00,
        .sd_freq_sel  	= 0x0,

        .pal_cm_off  	= 0x8b,
        .s_point  		= 0xd0,
        .fsc_lock_mode  = 0xDC,
        .comb_mode  	= 0x1,

        .sync_rs  		= 0x04,
        .h_delay_lsb  	= 0x80,
        .h_mask_on  	= 0x0,
        .h_mask_sel  	= 0x0,
        .mem_rdp  		= 0x01,
        .v_blk_end_b  	= 0x0,
        .y_delay  		= 0x10,
        .fld_inv  		= 0x1,
        .v_blk_end_a  	= 0x80,

        .h_down_scaler  = 0x10,
        .h_scaler_mode  = 0x10,
        .ref_base_lsb  	= 0x0,
        .ref_base_msb  	= 0x0,
        .line_mem_mode  = 0x01,

        .hpll_mask_on  	= 0x84,
        .hafc_byp_th_e  = 0xF,
        .hafc_byp_th_s  = 0xFC,
        .hafc_op_md  	= 0xB9,

        .clk_adc  		= 0x44,
        .clk_pre  		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x20,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_SD_H960_2EX_Btype_PAL ] = {
        .name = "AHD20_SD_H960_2EX_Btype_PAL",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel 	= 0x00,
        .brightnees 	= 0x00,
        .contrast		= 0x86,
        .black_level 	= 0x80,
        .hue 			= 0x00,
        .u_gain 		= 0x00,
        .v_gain 		= 0x00,
        .u_offset 		= 0xF8,
        .v_offset 		= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0x00,
        .burst_dec_c 	= 0x00,

        .video_format 	= 0xdd,
        .sd_mode 		= 0x0F,
        .ahd_mode 		= 0x00,
        .spl_mode 		= 0x00,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off  	= 0x0b,
        .s_point  		= 0xd0,
        .fsc_lock_mode  = 0xcc,
        .comb_mode  	= 0x0d,

        .sync_rs  		= 0xee,
        .h_delay_lsb  	= 0x60,
        .h_mask_on  	= 0x0,
        .h_mask_sel  	= 0x0,
        .mem_rdp  		= 0x1,
        .v_blk_end_b  	= 0x0,
        .y_delay  		= 0x10,
        .fld_inv  		= 0x0,
        .v_blk_end_a  	= 0x00,

        .h_down_scaler 	= 0x10,
        .h_scaler_mode 	= 0x10,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x1,

        .hpll_mask_on 	= 0xc6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0xB9,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x20,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    /* =====================================================
     * AHD
     * =====================================================*/
    [ AHD20_1080P_30P ] = {
        .name = "AHD20_1080P_30P",
        .powerdown 		= 0x0,
        .gain 			=  0x1,
        .spd 			= 0xD,
        .ctrlreg		= 0x1,
        .ctrlibs		= 0x2,
        .adcspd 		= 0x0,
        .clplevel 		= 0x2,
        .eq_band 		= 0x0,
        .lpf_front_band = 0x7,
        .clpmode 		= 0x0,
        .f_lpf_bypass 	= 0x1,
        .clproff 		= 0x0,
        .b_lpf_bypass 	= 0x0,
        .duty 			= 0x4,
        .ref_vol 		= 0x1,
        .lpf_back_band 	= 0x7,
        .clk_sel 		= 0x1,
        .eq_gainsel 	= 0x0,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x86,
        .black_level 	= 0x80,
        .hue 			= 0,
        .u_gain 		= 0,
        .v_gain 		= 0,
        .u_offset 		= 0xF8,
        .v_offset 		= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x02,
        .spl_mode 		= 0x0,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x1,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x48,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x04,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_1080P_25P ] = {
        .name = "AHD20_1080P_25P",
        .powerdown 		= 0x0,
        .gain 			=  0x1,
        .spd 			= 0xD,
        .ctrlreg		= 0x1,
        .ctrlibs		= 0x2,
        .adcspd 		= 0x0,
        .clplevel 		= 0x2,
        .eq_band 		= 0x0,
        .lpf_front_band = 0x7,
        .clpmode 		= 0x0,
        .f_lpf_bypass 	= 0x1,
        .clproff 		= 0x0,
        .b_lpf_bypass 	= 0x0,
        .duty 			= 0x4,
        .ref_vol 		= 0x1,
        .lpf_back_band 	= 0x7,
        .clk_sel 		= 0x1,
        .eq_gainsel 	= 0x0,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x86,
        .black_level 	= 0x80,
        .hue 			= 0,
        .u_gain 		= 0,
        .v_gain 		= 0,
        .u_offset 		= 0xF8,
        .v_offset 		= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x03,
        .spl_mode 		= 0x0,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x1,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x48,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x03,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_720P_60P ] = {
        .name = "AHD20_720P_60P",
        .powerdown 		= 0x0,
        .gain 			=  0x1,
        .spd 			= 0xD,
        .ctrlreg		= 0x1,
        .ctrlibs		= 0x2,
        .adcspd 		= 0x0,
        .clplevel 		= 0x2,
        .eq_band 		= 0x0,
        .lpf_front_band = 0x7,
        .clpmode 		= 0x0,
        .f_lpf_bypass 	= 0x1,
        .clproff 		= 0x0,
        .b_lpf_bypass 	= 0x0,
        .duty 			= 0x4,
        .ref_vol 		= 0x1,
        .lpf_back_band 	= 0x7,
        .clk_sel 		= 0x1,
        .eq_gainsel 	= 0x0,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x86,
        .black_level 	= 0x80,
        .hue 			= 0,
        .u_gain 		= 0,
        .v_gain 		= 0,
        .u_offset 		= 0xF8,
        .v_offset 		= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x04,
        .spl_mode 		= 0x0,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x1,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x48,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x04,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
		.vblk_end_sel 	= 0x10,
		.vblk_end_ext 	= 0x1B,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_720P_50P ] = {
        .name = "AHD20_720P_50P",
        .powerdown 		= 0x0,
        .gain 			=  0x1,
        .spd 			= 0xD,
        .ctrlreg		= 0x1,
        .ctrlibs		= 0x2,
        .adcspd 		= 0x0,
        .clplevel 		= 0x2,
        .eq_band 		= 0x0,
        .lpf_front_band = 0x7,
        .clpmode 		= 0x0,
        .f_lpf_bypass 	= 0x1,
        .clproff 		= 0x0,
        .b_lpf_bypass 	= 0x0,
        .duty 			= 0x4,
        .ref_vol 		= 0x1,
        .lpf_back_band 	= 0x7,
        .clk_sel 		= 0x1,
        .eq_gainsel 	= 0x0,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x86,
        .black_level 	= 0x80,
        .hue 			= 0,
        .u_gain 		= 0,
        .v_gain 		= 0,
        .u_offset 		= 0xF8,
        .v_offset 		= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x05,
        .spl_mode	 	= 0x0,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x1,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x48,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x03,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_720P_30P ] = {
        .name = "AHD20_720P_30P",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x86,
        .black_level 	= 0x80,
        .hue 			= 0,
        .u_gain 		= 0,
        .v_gain 		= 0,
        .u_offset 		= 0xF8,
        .v_offset 		= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 30,

        .video_format 	= 0x0,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x06,
        .spl_mode 		= 0x00,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x1,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x80,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_720P_25P ] = {
        .name = "AHD20_720P_25P",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x0d,
        .spl_mode 		= 0x00,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x1,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x80,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_720P_30P_EX ] = {
        .name = "AHD20_720P_30P_EX",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .video_format 	= 0x0,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x0A,
        .spl_mode 		= 0x0,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x1,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x80,
        .h_mask_on 		= 0x00,
        .h_mask_sel 	= 0x00,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x00,
        .clk_post 		= 0x04,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_720P_25P_EX ] = {
        .name = "AHD20_720P_25P_EX",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .video_format 	= 0x0,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x0B,
        .spl_mode 		= 0x0,
        .sd_freq_sel	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x1,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x80,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x00,
        .clk_post 		= 0x04,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_720P_30P_EX_Btype ] = {
        .name = "AHD20_720P_30P_EX_Btype",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x86,
        .black_level 	= 0x80,
        .hue 			= 0,
        .u_gain 		= 0,
        .v_gain 		= 0,
        .u_offset 		= 0xF8,
        .v_offset 		= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x0c,
        .spl_mode 		= 0x00,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x1,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x80,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x5,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
		.vblk_end_sel 	= 0x10,
		.vblk_end_ext 	= 0x1C,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_720P_25P_EX_Btype ] = {
        .name = "AHD20_720P_25P_EX_Btype",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x86,
        .black_level 	= 0x80,
        .hue 			= 0,
        .u_gain 		= 0,
        .v_gain 		= 0,
        .u_offset 		= 0xF8,
        .v_offset 		= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x0d,
        .spl_mode 		= 0x00,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x1,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x80,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x5,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_720P_960P_30P ] = {
        .name = "AHD20_720P_960P_30P",
        .powerdown 		= 0x0,
        .gain 			=  0x1,
        .spd 			= 0xD,
        .ctrlreg		= 0x1,
        .ctrlibs		= 0x2,
        .adcspd 		= 0x0,
        .clplevel 		= 0x2,
        .eq_band 		= 0x0,
        .lpf_front_band = 0x7,
        .clpmode 		= 0x0,
        .f_lpf_bypass 	= 0x1,
        .clproff 		= 0x0,
        .b_lpf_bypass 	= 0x0,
        .duty 			= 0x4,
        .ref_vol 		= 0x1,
        .lpf_back_band 	= 0x7,
        .clk_sel 		= 0x1,
        .eq_gainsel 	= 0x0,

        .new_wpd_sel 	= 0x0,
        .brightnees 	= 0x0,
        .contrast		= 0x86,
        .black_level 	= 0x80,
        .hue 			= 0x0,
        .u_gain 		= 0x0,
        .v_gain 		= 0x0,
        .u_offset 		= 0xF8,
        .v_offset 		= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0x0,
        .burst_dec_c 	= 0x00,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x02,
        .spl_mode 		= 0x0,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x1,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x48,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x04,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ AHD20_720P_960P_25P ] = {
        .name = "AHD20_720P_960P_25P",
        .powerdown 		= 0x0,
        .gain 			= 0x1,
        .spd 			= 0xD,
        .ctrlreg		= 0x1,
        .ctrlibs		= 0x2,
        .adcspd 		= 0x0,
        .clplevel 		= 0x2,
        .eq_band 		= 0x0,
        .lpf_front_band = 0x7,
        .clpmode 		= 0x0,
        .f_lpf_bypass 	= 0x1,
        .clproff 		= 0x0,
        .b_lpf_bypass 	= 0x0,
        .duty 			= 0x4,
        .ref_vol 		= 0x1,
        .lpf_back_band 	= 0x7,
        .clk_sel 		= 0x1,
        .eq_gainsel 	= 0x0,

        .new_wpd_sel 	= 0x0,
        .brightnees 	= 0x0,
        .contrast		= 0x86,
        .black_level 	= 0x80,
        .hue 			= 0x0,
        .u_gain 		= 0x0,
        .v_gain 		= 0x0,
        .u_offset 		= 0xF8,
        .v_offset 		= 0xF8,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0x0,
        .burst_dec_c 	= 0x0,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x03,
        .spl_mode 		= 0x0,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x1,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x48,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x03,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },


    /* =====================================================
     * TVI
     * =====================================================*/
    [ TVI_FHD_30P ] = {
        .name = "TVI_FHD_30P",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0xD,
        .ctrlreg		= 0x01,
        .ctrlibs		= 2,
        .adcspd 		= 0,
        .clplevel 		= 2,
        .eq_band 		= 0,
        .lpf_front_band = 7,
        .clpmode 		= 0,
        .f_lpf_bypass 	= 1,
        .clproff 		= 0,
        .b_lpf_bypass 	= 0,
        .duty 			= 4,
        .ref_vol 		= 1,
        .lpf_back_band 	= 7,
        .clk_sel 		= 1,
        .eq_gainsel 	= 0,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0x00,
        .contrast   	= 0x8c,
        .black_level 	= 0x84,
        .hue 			= 0x00,
        .u_gain 		= 0x10,
        .v_gain 		= 0x10,
        .u_offset 		= 0xF6,
        .v_offset 		= 0xF4,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x02,
        .spl_mode 		= 0x1,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x68,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x2,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x08,
        .clk_pre 		= 0x02,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ TVI_FHD_25P ] = {
        .name = "TVI_FHD_25P",
        .powerdown 		= 0,
        .gain 			= 1,
        .spd 			= 0xD,
        .ctrlreg		= 1,
        .ctrlibs		= 2,
        .adcspd 		= 0,
        .clplevel 		= 2,
        .eq_band 		= 0,
        .lpf_front_band = 7,
        .clpmode 		= 0,
        .f_lpf_bypass 	= 1,
        .clproff 		= 0,
        .b_lpf_bypass 	= 0,
        .duty 			= 4,
        .ref_vol 		= 1,
        .lpf_back_band 	= 7,
        .clk_sel 		= 1,
        .eq_gainsel 	= 0,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x75,
        .black_level 	= 0x84,
        .hue 			= 0x3,
        .u_gain 		= 0xC,
        .v_gain 		= 0x1A,
        .u_offset 		= 0xFA,
        .v_offset 		= 0xFA,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x03,
        .spl_mode 		= 0x1,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x60,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x08,
        .clk_pre 		= 0x02,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ TVI_HD_60P ] = {
        .name = "TVI_HD_60P",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x75,
        .black_level 	= 0x84,
        .hue 			= 0x00,
        .u_gain 		= 0xC,
        .v_gain 		= 0x1A,
        .u_offset 		= 0xFA,
        .v_offset 		= 0xFA,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x04,
        .spl_mode 		= 0x1,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x98,
        .h_mask_on 		= 0x66,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x00,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x08,
        .clk_pre 		= 0x02,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ TVI_HD_50P ] = {
        .name = "TVI_HD_50P",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x75,
        .black_level 	= 0x84,
        .hue 			= 0x00,
        .u_gain 		= 0xC,
        .v_gain 		= 0x1A,
        .u_offset 		= 0xFA,
        .v_offset 		= 0xFA,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x05,
        .spl_mode 		= 0x1,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x66,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x00,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x08,
        .clk_pre 		= 0x02,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ TVI_HD_30P ] = {
        .name = "TVI_HD_30P",
        .powerdown  	= 0x00,
        .gain  			= 0x01,
        .spd  			= 0x0D,
        .ctrlreg  		= 0x01,
        .ctrlibs  		= 0x02,
        .adcspd  		= 0x00,
        .clplevel  		= 0x02,
        .eq_band  		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode  		= 0x00,
        .f_lpf_bypass  	= 0x01,
        .clproff  		= 0x00,
        .b_lpf_bypass  	= 0x00,
        .duty  			= 0x04,
        .ref_vol  		= 0x03,
        .lpf_back_band  = 0x07,
        .clk_sel  		= 0x01,
        .eq_gainsel  	= 0x00,

        .new_wpd_sel  	= 0,
        .brightnees  	= 0,
        .contrast  		= 0x8c,
        .black_level  	= 0x80,
        .hue  			= 0x00,
        .u_gain  		= 0x10,
        .v_gain  		= 0x10,
        .u_offset  		= 0xF6,
        .v_offset  		= 0xF4,
        .saturation_a  	= 0x80,
        .saturation_b  	= 0xA8,
        .burst_dec_a  	= 0x2A,
        .burst_dec_b  	= 0x30,
        .burst_dec_c  	= 0x30,

        .video_format  	= 0x0,
        .sd_mode  		= 0x00,
        .ahd_mode  		= 0x0A,
        .spl_mode  		= 0x01,
        .sd_freq_sel  	= 0x0,

        .pal_cm_off  	= 0x82,
        .s_point  		= 0x90,
        .fsc_lock_mode  = 0xDC,
        .comb_mode  	= 0x5,

        .sync_rs  		= 0xEE,
        .h_delay_lsb  	= 0x70,
        .h_mask_on  	= 0x01,
        .h_mask_sel  	= 0x2,
        .mem_rdp  		= 0x0,
        .v_blk_end_b  	= 0x0,
        .y_delay  		= 0x10,
        .fld_inv  		= 0x0,
        .v_blk_end_a  	= 0x80,

        .h_down_scaler  = 0x00,
        .h_scaler_mode  = 0x59,
        .ref_base_lsb  	= 0x0,
        .ref_base_msb  	= 0x0,
        .line_mem_mode  = 0x0,

        .hpll_mask_on  	= 0xC6,
        .hafc_byp_th_e  = 0xF,
        .hafc_byp_th_s  = 0xFC,
        .hafc_op_md  	= 0x39,

        .clk_adc  		= 0x08,
        .clk_pre  		= 0x02,
        .clk_post  		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x80,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ TVI_HD_25P ] = {
        .name = "TVI_HD_25P",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x8c,
        .black_level 	= 0x84,
        .hue 			= 0x00,
        .u_gain 		= 0x10,
        .v_gain 		= 0x10,
        .u_offset 		= 0xF6,
        .v_offset 		= 0xF4,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x0,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x0B,
        .spl_mode 		= 0x1,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x70,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x59,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x08,
        .clk_pre 		= 0x02,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x80,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ TVI_HD_30P_EX ] = {
        .name = "TVI_HD_30P_EX",
        .powerdown  	= 0x00,
        .gain  			= 0x01,
        .spd  			= 0x0D,
        .ctrlreg  		= 0x01,
        .ctrlibs  		= 0x02,
        .adcspd  		= 0x00,
        .clplevel  		= 0x02,
        .eq_band  		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode  		= 0x00,
        .f_lpf_bypass  	= 0x01,
        .clproff  		= 0x00,
        .b_lpf_bypass  	= 0x00,
        .duty  			= 0x04,
        .ref_vol  		= 0x03,
        .lpf_back_band  = 0x07,
        .clk_sel  		= 0x01,
        .eq_gainsel  	= 0x00,

        .new_wpd_sel  	= 0,
        .brightnees  	= 0,
        .contrast  		= 0x8c,
        .black_level  	= 0x80,
        .hue  			= 0x00,
        .u_gain  		= 0x10,
        .v_gain  		= 0x10,
        .u_offset  		= 0xF6,
        .v_offset  		= 0xF4,
        .saturation_a  	= 0x80,
        .saturation_b  	= 0xA8,
        .burst_dec_a  	= 0x2A,
        .burst_dec_b  	= 0x30,
        .burst_dec_c  	= 0x30,

        .video_format  	= 0x0,
        .sd_mode  		= 0x00,
        .ahd_mode  		= 0x0A,
        .spl_mode  		= 0x01,
        .sd_freq_sel  	= 0x0,

        .pal_cm_off  	= 0x82,
        .s_point  		= 0x90,
        .fsc_lock_mode  = 0xDC,
        .comb_mode  	= 0x5,

        .sync_rs  		= 0xEE,
        .h_delay_lsb  	= 0x70,
        .h_mask_on  	= 0x01,
        .h_mask_sel  	= 0x2,
        .mem_rdp  		= 0x0,
        .v_blk_end_b  	= 0x0,
        .y_delay  		= 0x10,
        .fld_inv  		= 0x0,
        .v_blk_end_a  	= 0x80,

        .h_down_scaler  = 0x00,
        .h_scaler_mode  = 0x59,
        .ref_base_lsb  	= 0x0,
        .ref_base_msb  	= 0x0,
        .line_mem_mode  = 0x0,

        .hpll_mask_on  	= 0xC6,
        .hafc_byp_th_e  = 0xF,
        .hafc_byp_th_s  = 0xFC,
        .hafc_op_md  	= 0x39,

        .clk_adc  		= 0x08,
        .clk_pre  		= 0x02,
        .clk_post  		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x80,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ TVI_HD_25P_EX ] = {
        .name = "TVI_HD_25P_EX",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x8c,
        .black_level 	= 0x84,
        .hue 			= 0x00,
        .u_gain 		= 0x10,
        .v_gain 		= 0x10,
        .u_offset 		= 0xF6,
        .v_offset 		= 0xF4,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xA8,
        .burst_dec_a 	= 0x2A,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x0,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x0B,
        .spl_mode 		= 0x1,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x70,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x59,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x08,
        .clk_pre 		= 0x02,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x80,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ TVI_HD_B_30P ] = {
        .name = "TVI_HD_B_30P",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel  	= 0,
        .brightnees  	= 0,
        .contrast  		= 0x8c,
        .black_level  	= 0x80,
        .hue  			= 0x00,
        .u_gain  		= 0x10,
        .v_gain  		= 0x10,
        .u_offset  		= 0xF6,
        .v_offset  		= 0xF4,
        .saturation_a  	= 0x80,
        .saturation_b  	= 0xA8,
        .burst_dec_a  	= 0x2A,
        .burst_dec_b  	= 0x30,
        .burst_dec_c  	= 0x30,

        .video_format 	= 0x0,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x06,
        .spl_mode 		= 0x3,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x80,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x10,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ TVI_HD_B_25P ] = {
        .name = "TVI_HD_B_25P",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel  	= 0,
        .brightnees  	= 0,
        .contrast  		= 0x8c,
        .black_level  	= 0x80,
        .hue  			= 0x00,
        .u_gain  		= 0x10,
        .v_gain  		= 0x10,
        .u_offset  		= 0xF6,
        .v_offset  		= 0xF4,
        .saturation_a  	= 0x80,
        .saturation_b  	= 0xA8,
        .burst_dec_a  	= 0x2A,
        .burst_dec_b  	= 0x30,
        .burst_dec_c  	= 0x30,

        .video_format 	= 0x0,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x07,
        .spl_mode 		= 0x3,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off  	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x80,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ TVI_HD_B_30P_EX ] = {
        .name = "TVI_HD_B_30P_EX",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel  	= 0,
        .brightnees  	= 0,
        .contrast  		= 0x8c,
        .black_level  	= 0x80,
        .hue  			= 0x00,
        .u_gain  		= 0x10,
        .v_gain  		= 0x10,
        .u_offset  		= 0xF6,
        .v_offset  		= 0xF4,
        .saturation_a  	= 0x80,
        .saturation_b  	= 0xA8,
        .burst_dec_a  	= 0x2A,
        .burst_dec_b  	= 0x30,
        .burst_dec_c  	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x0A,
        .spl_mode 		= 0x3,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x70,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ TVI_HD_B_25P_EX ] = {
        .name = "TVI_HD_B_25P_EX",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel  	= 0,
        .brightnees  	= 0,
        .contrast  		= 0x8c,
        .black_level  	= 0x80,
        .hue  			= 0x00,
        .u_gain  		= 0x10,
        .v_gain  		= 0x10,
        .u_offset  		= 0xF6,
        .v_offset  		= 0xF4,
        .saturation_a  	= 0x80,
        .saturation_b  	= 0xA8,
        .burst_dec_a  	= 0x2A,
        .burst_dec_b  	= 0x30,
        .burst_dec_c  	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x0B,
        .spl_mode 		= 0x03,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x60,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x0,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    /* =====================================================
     * CVI
     * =====================================================*/
    [ CVI_FHD_30P ] = {
        .name = "CVI_FHD_30P",
        .powerdown 		= 0,
        .gain 			= 1,
        .spd 			= 0xD,
        .ctrlreg		= 1,
        .ctrlibs		= 2,
        .adcspd 		= 0,
        .clplevel 		= 2,
        .eq_band 		= 0,
        .lpf_front_band = 7,
        .clpmode 		= 0,
        .f_lpf_bypass 	= 1,
        .clproff 		= 0,
        .b_lpf_bypass 	= 0,
        .duty 			= 4,
        .ref_vol 		= 1,
        .lpf_back_band 	= 7,
        .clk_sel 		= 1,
        .eq_gainsel 	= 0,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x98,
        .black_level 	= 0x86,
        .hue 			= 0x3,
        .u_gain 		= 0xC,
        .v_gain 		= 0x1A,
        .u_offset 		= 0xFA,
        .v_offset 		= 0xFA,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xa8,
        .burst_dec_a 	= 0x2a,
        .burst_dec_b 	= 0,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x02,
        .spl_mode		= 0x02,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x00,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x4,
        .mem_rdp 		= 0x21,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x39,
        .ref_base_lsb 	= 0x78,
        .ref_base_msb 	= 0x1,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x08,
        .clk_pre 		= 0x02,
        .clk_post		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x80,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ CVI_FHD_25P ] = {
        .name = "CVI_FHD_25P",
        .powerdown 		= 0,
        .gain 			= 1,
        .spd 			= 0xD,
        .ctrlreg		= 1,
        .ctrlibs		= 2,
        .adcspd 		= 0,
        .clplevel 		= 2,
        .eq_band 		= 0,
        .lpf_front_band = 7,
        .clpmode 		= 0,
        .f_lpf_bypass 	= 1,
        .clproff 		= 0,
        .b_lpf_bypass 	= 0,
        .duty 			= 4,
        .ref_vol 		= 1,
        .lpf_back_band 	= 7,
        .clk_sel 		= 1,
        .eq_gainsel 	= 0,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x98,
        .black_level 	= 0x86,
        .hue 			= 0x00,
        .u_gain 		= 0xC,
        .v_gain 		= 0x1A,
        .u_offset 		= 0xFA,
        .v_offset 		= 0xFA,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xa8,
        .burst_dec_a 	= 0x2a,
        .burst_dec_b 	= 0x30,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x03,
        .spl_mode 		= 0x02,
        .sd_freq_sel 	= 0x00,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x80,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x2,
        .mem_rdp 		= 0x22,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x49,
        .ref_base_lsb 	= 0x4f,
        .ref_base_msb 	= 0x2,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x08,
        .clk_pre 		= 0x02,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x00,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ CVI_HD_60P ] = {
        .name = "CVI_HD_60P",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x8c,
        .black_level 	= 0x86,
        .hue 			= 0x00,
        .u_gain 		= 0x10,
        .v_gain 		= 0x10,
        .u_offset 		= 0xF6,
        .v_offset 		= 0xFA,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xa8,
        .burst_dec_a 	= 0x2a,
        .burst_dec_b 	= 0x30,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x04,
        .spl_mode 		= 0x02,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0xa8,
        .h_mask_on 		= 0x00,
        .h_mask_sel 	= 0x00,
        .mem_rdp 		= 0x09,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x00,
        .h_scaler_mode 	= 0x19,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x08,
        .clk_pre 		= 0x02,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x80,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ CVI_HD_50P ] = {
        .name = "CVI_HD_50P",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel	= 0,
        .brightnees 	= 0,
        .contrast		= 0x8c,
        .black_level 	= 0x86,
        .hue 			= 0x00,
        .u_gain 		= 0x10,
        .v_gain 		= 0x10,
        .u_offset 		= 0xF6,
        .v_offset 		= 0xFA,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xa8,
        .burst_dec_a 	= 0x2a,
        .burst_dec_b 	= 0x30,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x05,
        .spl_mode 		= 0x02,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x60,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x00,
        .h_scaler_mode 	= 0x29,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x08,
        .clk_pre 		= 0x02,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x80,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ CVI_HD_30P ] = {
        .name = "CVI_HD_30P",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .video_format 	= 0x0,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x06,
        .spl_mode 		= 0x2,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x80,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x1,
        .h_scaler_mode 	= 0x19,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x1,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x04,
        .clk_pre 		= 0x04,
        .clk_post 		= 0x04,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x80,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ CVI_HD_25P ] = {
        .name = "CVI_HD_25P",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .video_format 	= 0x0,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x07,
        .spl_mode 		= 0x2,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x80,
        .h_mask_on 		= 0x0,
        .h_mask_sel 	= 0x0,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x1,
        .h_scaler_mode 	= 0x29,
        .ref_base_lsb 	= 0xc0,
        .ref_base_msb 	= 0x1,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x04,
        .clk_pre 		= 0x04,
        .clk_post 		= 0x04,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x80,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ CVI_HD_30P_EX ] = {
        .name = "CVI_HD_30P_EX",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x8c,
        .black_level 	= 0x80,
        .hue 			= 0x00,
        .u_gain 		= 0x10,
        .v_gain 		= 0x10,
        .u_offset 		= 0xF6,
        .v_offset 		= 0xFA,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xa8,
        .burst_dec_a 	= 0x2a,
        .burst_dec_b 	= 0x30,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x0A,
        .spl_mode 		= 0x02,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x82,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0xa0,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x5,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x19,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x1,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e 	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x80,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },

    [ CVI_HD_25P_EX ] = {
        .name = "CVI_HD_25P_EX",
        .powerdown 		= 0x00,
        .gain 			= 0x01,
        .spd 			= 0x0D,
        .ctrlreg 		= 0x01,
        .ctrlibs 		= 0x02,
        .adcspd 		= 0x00,
        .clplevel 		= 0x02,
        .eq_band 		= 0x00,
        .lpf_front_band = 0x07,
        .clpmode 		= 0x00,
        .f_lpf_bypass 	= 0x01,
        .clproff 		= 0x00,
        .b_lpf_bypass 	= 0x00,
        .duty 			= 0x04,
        .ref_vol 		= 0x03,
        .lpf_back_band 	= 0x07,
        .clk_sel 		= 0x01,
        .eq_gainsel 	= 0x00,

        .new_wpd_sel 	= 0,
        .brightnees 	= 0,
        .contrast		= 0x8c,
        .black_level 	= 0x80,
        .hue 			= 0x00,
        .u_gain 		= 0x10,
        .v_gain 		= 0x10,
        .u_offset 		= 0xF6,
        .v_offset 		= 0xFA,
        .saturation_a 	= 0x80,
        .saturation_b 	= 0xa8,
        .burst_dec_a 	= 0x2a,
        .burst_dec_b 	= 0x30,
        .burst_dec_c 	= 0x30,

        .video_format 	= 0x20,
        .sd_mode 		= 0x00,
        .ahd_mode 		= 0x0B,
        .spl_mode 		= 0x02,
        .sd_freq_sel 	= 0x0,

        .pal_cm_off 	= 0x83,
        .s_point 		= 0x90,
        .fsc_lock_mode 	= 0xDC,
        .comb_mode 		= 0x5,

        .sync_rs 		= 0xEE,
        .h_delay_lsb 	= 0x60,
        .h_mask_on 		= 0x01,
        .h_mask_sel 	= 0x1,
        .mem_rdp 		= 0x0,
        .v_blk_end_b 	= 0x0,
        .y_delay 		= 0x10,
        .fld_inv 		= 0x0,
        .v_blk_end_a 	= 0x80,

        .h_down_scaler 	= 0x0,
        .h_scaler_mode 	= 0x29,
        .ref_base_lsb 	= 0x0,
        .ref_base_msb 	= 0x0,
        .line_mem_mode 	= 0x0,

        .hpll_mask_on 	= 0xC6,
        .hafc_byp_th_e	= 0xF,
        .hafc_byp_th_s 	= 0xFC,
        .hafc_op_md 	= 0x39,

        .clk_adc 		= 0x44,
        .clk_pre 		= 0x01,
        .clk_post 		= 0x02,

        .h_dly_msb 		= 0x00,
        .vblk_end_sel 	= 0x00,
        .vblk_end_ext 	= 0x00,
        .h_scaler_active = 0x80,
        .cml_mode 		= 0x2c,
        .agc_op 		= 0x24,
        .g_sel 			= 0x0c,
        .sync_sel       = 0x20,
    },
};


NC_VD_VO_Init_STR vd_vo_init_list[] =
{
    [ AHD20_1080P_30P ] = {
        .name = "AHD20_1080P_30P",

        .vport_oclk_sel   = 0x40,
        .mux_yc_merge     = 0x00,
        .port_seq_ch01    = {0x00, 0x11, 0x22, 0x33},
        .port_seq_ch23    = {0x00, 0x11, 0x22, 0x33},
        .chid_vin       = 0x00,
        .vport_out_sel    = 0x30,

        .manual_ahd  = 0x00,
        .manual_cvi  = 0x00,
        .manual_tvi  = 0x00,
    },

    [ AHD20_1080P_25P ] = {
        .name = "AHD20_1080P_25P",

        .vport_oclk_sel = 0x40,
        .mux_yc_merge   = 0x00,
        .port_seq_ch01  = {0x00, 0x11, 0x22, 0x33},
        .port_seq_ch23  = {0x00, 0x11, 0x22, 0x33},
        .chid_vin       = 0x00,
        .vport_out_sel  = 0x30,

        .manual_ahd  = 0x00,
        .manual_cvi  = 0x00,
        .manual_tvi  = 0x00,
    },

    [ AHD20_720P_30P_EX_Btype ] = {
        .name = "AHD20_720P_30P_EX_Btype",

        .vport_oclk_sel = 0x40,
        .mux_yc_merge   = 0x00,
        .port_seq_ch01  = {0x00, 0x11, 0x22, 0x33},
        .port_seq_ch23  = {0x00, 0x11, 0x22, 0x33},
        .chid_vin       = 0x00,
        .vport_out_sel  = 0x30,

        .manual_ahd  = 0x00,
        .manual_cvi  = 0x00,
        .manual_tvi  = 0x00,
    },

    [ AHD20_720P_25P_EX_Btype ] = {
        .name = "AHD20_720P_25P_EX_Btype",

        .vport_oclk_sel = 0x40,
        .mux_yc_merge   = 0x00,
        .port_seq_ch01  = {0x00, 0x11, 0x22, 0x33},
        .port_seq_ch23  = {0x00, 0x11, 0x22, 0x33},
        .chid_vin       = 0x00,
        .vport_out_sel  = 0x30,

        .manual_ahd  = 0x00,
        .manual_cvi  = 0x00,
        .manual_tvi  = 0x00,
    },

    [ CVI_FHD_30P ] = {
        .name = "CVI_FHD_30P",

        .vport_oclk_sel = 0x40,
        .mux_yc_merge   = 0x00,
        .port_seq_ch01  = {0x00, 0x11, 0x22, 0x33},
        .port_seq_ch23  = {0x00, 0x11, 0x22, 0x33},
        .chid_vin       = 0x00,
        .vport_out_sel  = 0x30,

        .manual_ahd  = 0x00,
        .manual_cvi  = 0x00,
        .manual_tvi  = 0x00,
    },

    [ CVI_FHD_25P ] = {
        .name = "CVI_FHD_25P",

        .vport_oclk_sel = 0x40,
        .mux_yc_merge   = 0x00,
        .port_seq_ch01  = {0x00, 0x11, 0x22, 0x33},
        .port_seq_ch23  = {0x00, 0x11, 0x22, 0x33},
        .chid_vin       = 0x00,
        .vport_out_sel  = 0x30,

        .manual_ahd  = 0x00,
        .manual_cvi  = 0x00,
        .manual_tvi  = 0x00,
    },

    [ CVI_HD_60P ] = {
        .name = "CVI_HD_60P",

        .vport_oclk_sel = 0x40,
        .mux_yc_merge   = 0x00,
        .port_seq_ch01  = {0x00, 0x11, 0x22, 0x33},
        .port_seq_ch23  = {0x00, 0x11, 0x22, 0x33},
        .chid_vin       = 0x00,
        .vport_out_sel  = 0x30,

        .manual_ahd  = 0x00,
        .manual_cvi  = 0x00,
        .manual_tvi  = 0x00,
    },

    [ CVI_HD_50P ] = {
        .name = "CVI_HD_50P",

        .vport_oclk_sel = 0x40,
        .mux_yc_merge   = 0x00,
        .port_seq_ch01  = {0x00, 0x11, 0x22, 0x33},
        .port_seq_ch23  = {0x00, 0x11, 0x22, 0x33},
        .chid_vin       = 0x00,
        .vport_out_sel  = 0x30,

        .manual_ahd  = 0x00,
        .manual_cvi  = 0x00,
        .manual_tvi  = 0x00,
    },

    [ CVI_HD_30P_EX ] = {
        .name = "CVI_HD_30P_EX",

        .vport_oclk_sel = 0x40,
        .mux_yc_merge   = 0x00,
        .port_seq_ch01  = {0x00, 0x11, 0x22, 0x33},
        .port_seq_ch23  = {0x00, 0x11, 0x22, 0x33},
        .chid_vin       = 0x00,
        .vport_out_sel  = 0x30,

        .manual_ahd  = 0x00,
        .manual_cvi  = 0x00,
        .manual_tvi  = 0x00,
    },

    [ CVI_HD_25P_EX ] = {
        .name = "CVI_HD_30P_EX",

        .vport_oclk_sel = 0x40,
        .mux_yc_merge   = 0x00,
        .port_seq_ch01  = {0x00, 0x11, 0x22, 0x33},
        .port_seq_ch23  = {0x00, 0x11, 0x22, 0x33},
        .chid_vin       = 0x00,
        .vport_out_sel  = 0x30,

        .manual_ahd  = 0x00,
        .manual_cvi  = 0x00,
        .manual_tvi  = 0x00,
    },
};


NC_VD_Jaguar1_Init_STR vd_jaguar1_init_list[] =
{
    [ AHD20_SD_H960_2EX_Btype_NT ] = {
        .name = "AHD20_SD_H960_2EX_Btype_NT",
        .vadc_clk_sel  =  0x80,
        .vafe_clp_level  =  0x03,
        .vafe_f_lpf_sel  =  0x03,
        .clp_mode_sel  =  0x00,
        .vafe_duty  =  0x01,

        .novid_det_b  =  0x13,
        .h_dly_msb  =  0x00,
        .vblk_end  =  0x21,

        .vport_oclk_sel  =  0x60,
        .new_c_peak_mode  =  0x00,
        .auto_gnos_mode  =  0x00,
        .y_temp_sel  =  0xc6,
        .no_mem_path  =  0x01,

        .a_cmp_pw_mode  =  0xc6,
        .eoh_rst_dly  =  0x10,

        .hafc_lpf_sel  =  0x72,
        .tm_clk_sel  =  0x10,
    },

    [ AHD20_SD_H960_2EX_Btype_PAL ] = {
        .name = "AHD20_SD_H960_2EX_Btype_PAL",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01,

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x60,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x01,

        .a_cmp_pw_mode   = 0xc6,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },


    [ AHD20_1080P_30P ] = {
        .name = "AHD20_1080P_30P",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01,

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x00,

        .a_cmp_pw_mode   = 0x00,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ AHD20_1080P_25P ] = {
        .name = "AHD20_1080P_25P",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x00,

        .a_cmp_pw_mode   = 0x00,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ AHD20_720P_30P ] = {
        .name = "AHD20_720P_30P_EX_Btype",
        .vadc_clk_sel   = 0x80,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x20,

        .vport_oclk_sel  = 0x60,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x01,

        .a_cmp_pw_mode   = 0xc0,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ AHD20_720P_25P ] = {
        .name = "AHD20_720P_25P_EX_Btype",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x01,

        .a_cmp_pw_mode   = 0xc0,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ AHD20_720P_30P_EX_Btype ] = {
        .name = "AHD20_720P_30P_EX_Btype",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x00,

        .a_cmp_pw_mode   = 0x00,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ AHD20_720P_25P_EX_Btype ] = {
        .name = "AHD20_720P_25P_EX_Btype",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x01,

        .a_cmp_pw_mode   = 0xc0,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ TVI_FHD_30P ] = {
        .name = "TVI_FHD_30P",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x01,

        .a_cmp_pw_mode   = 0xc0,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ TVI_FHD_25P ] = {
        .name = "TVI_FHD_25P",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x01,

        .a_cmp_pw_mode   = 0xc0,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ TVI_HD_60P ] = {
        .name = "TVI_HD_60P",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x01,

        .a_cmp_pw_mode   = 0xc0,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ TVI_HD_50P ] = {
        .name = "TVI_HD_50P",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x01,

        .a_cmp_pw_mode   = 0xc0,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ TVI_HD_30P_EX ] = {
        .name = "TVI_HD_30P_EX",
        .vadc_clk_sel  =  0x80,
        .vafe_clp_level  =  0x03,
        .vafe_f_lpf_sel  =  0x03,
        .clp_mode_sel  =  0x00,
        .vafe_duty  =  0x01,

        .novid_det_b  =  0x13,
        .h_dly_msb  =  0x00,
        .vblk_end  =  0x20,

        .vport_oclk_sel  =  0x60,
        .new_c_peak_mode  =  0x00,
        .auto_gnos_mode  =  0x00,
        .y_temp_sel  =  0xc6,
        .no_mem_path  =  0x01,

        .a_cmp_pw_mode  =  0xc0,
        .eoh_rst_dly  =  0x10,

        .hafc_lpf_sel  =  0x72,
        .tm_clk_sel  =  0x10,

    },

    [ TVI_HD_25P_EX ] = {
        .name = "TVI_HD_25P_EX",
        .vadc_clk_sel   = 0x80,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x40,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x01,

        .a_cmp_pw_mode   = 0xc0,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ CVI_FHD_30P ] = {
        .name = "CVI_FHD_30P",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x00,

        .a_cmp_pw_mode   = 0x00,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ CVI_FHD_25P ] = {
        .name = "CVI_FHD_25P",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x00,

        .a_cmp_pw_mode   = 0x00,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ CVI_HD_60P ] = {
        .name = "CVI_HD_60P",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01, // 0x66 > 5x5b

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x00,

        .a_cmp_pw_mode   = 0x00,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ CVI_HD_50P ] = {
        .name = "CVI_HD_50P",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01,

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x00,

        .a_cmp_pw_mode   = 0x00,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ CVI_HD_30P_EX ] = {
        .name = "CVI_HD_30P_EX",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01,

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x00,

        .a_cmp_pw_mode   = 0x00,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },

    [ CVI_HD_25P_EX ] = {
        .name = "CVI_HD_30P_EX",
        .vadc_clk_sel   = 0x00,
        .vafe_clp_level = 0x03,
        .vafe_f_lpf_sel = 0x03,
        .clp_mode_sel   = 0x00,
        .vafe_duty      = 0x01,

        .novid_det_b     = 0x13,
        .h_dly_msb       = 0x00,
        .vblk_end        = 0x22,

        .vport_oclk_sel  = 0x70,
        .new_c_peak_mode = 0x00,
        .auto_gnos_mode = 0x00,
        .y_temp_sel      = 0xc6,
        .no_mem_path 	 = 0x00,

        .a_cmp_pw_mode   = 0x00,
        .eoh_rst_dly 	 = 0x10,

        .hafc_lpf_sel 	 = 0x72,
        .tm_clk_sel  =  0x10,
    },
};

/********************************************************************
 *  End of file
 ********************************************************************/

#endif
