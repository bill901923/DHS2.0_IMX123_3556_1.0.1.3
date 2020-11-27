/********************************************************************************
 *
 *  Copyright (C) 2017 	NEXTCHIP Inc. All rights reserved.
 *  Module		: video_input.c
 *  Description	:
 *  Author		:
 *  Date         :
 *  Version		: Version 1.0
 *
 ********************************************************************************
 *  History      :
 *
 *
 ********************************************************************************/
#ifndef _JAGUAR1_VIDEO_
#define _JAGUAR1_VIDEO_

#include "jaguar1_common.h"


/* ===============================================
 * APP -> DRV
 * =============================================== */

typedef struct _video_output_init
{
    unsigned char format;
    unsigned char port;
    unsigned char out_ch;
    unsigned char ch_mipiout_en;
} video_output_init;

typedef struct _video_video_loss_s
{
    unsigned char devnum;
    unsigned char videoloss;
    unsigned char reserve2;
} video_video_loss_s;

void vd_jaguar1_init_set( void* p_param);
void vd_jaguar1_vo_ch_seq_set( void* p_param);
void vd_jaguar1_eq_set( void* p_param );
void vd_jaguar1_sw_reset( void* p_param );
void vd_jaguar1_get_novideo( video_video_loss_s* vidloss );

void current_bank_set( unsigned char bank );
unsigned char current_bank_get( void );
void vd_register_set( int dev, unsigned char bank, unsigned char addr, unsigned char val, unsigned int pos, int size );
void reg_val_print_flag_set( int set );
void vd_jaguar1_stop_chnl( unsigned char devnum, unsigned char ch);
void vd_jaguar1_start_chnl( unsigned char devnum, unsigned char ch);

#endif
/********************************************************************
 *  End of file
 ********************************************************************/
