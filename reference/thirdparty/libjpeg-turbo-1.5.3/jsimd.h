/*
 * jsimd.h
 *
 * Copyright 2009 Pierre Ossman <ossman@cendio.se> for Cendio AB
 * Copyright (C) 2011, 2014, D. R. Commander.
 * Copyright (C) 2015, Matthieu Darbois.
 *
 * Based on the x86 SIMD extension for IJG JPEG library,
 * Copyright (C) 1999-2006, MIYASAKA Masaru.
 * For conditions of distribution and use, see copyright notice in jsimdext.inc
 *
 */

#include "jchuff.h"             /* Declarations shared with jcphuff.c */

EXTERN_JPEG(int) jsimd_can_rgb_ycc (void);
EXTERN_JPEG(int) jsimd_can_rgb_gray (void);
EXTERN_JPEG(int) jsimd_can_ycc_rgb (void);
EXTERN_JPEG(int) jsimd_can_ycc_rgb565 (void);
EXTERN_JPEG(int) jsimd_c_can_null_convert (void);

EXTERN_JPEG(void) jsimd_rgb_ycc_convert
        (j_compress_ptr cinfo, JSAMPARRAY input_buf, JSAMPIMAGE output_buf,
         JDIMENSION output_row, int num_rows);
EXTERN_JPEG(void) jsimd_rgb_gray_convert
        (j_compress_ptr cinfo, JSAMPARRAY input_buf, JSAMPIMAGE output_buf,
         JDIMENSION output_row, int num_rows);
EXTERN_JPEG(void) jsimd_ycc_rgb_convert
        (j_decompress_ptr cinfo, JSAMPIMAGE input_buf, JDIMENSION input_row,
         JSAMPARRAY output_buf, int num_rows);
EXTERN_JPEG(void) jsimd_ycc_rgb565_convert
        (j_decompress_ptr cinfo, JSAMPIMAGE input_buf, JDIMENSION input_row,
         JSAMPARRAY output_buf, int num_rows);
EXTERN_JPEG(void) jsimd_c_null_convert
        (j_compress_ptr cinfo, JSAMPARRAY input_buf, JSAMPIMAGE output_buf,
         JDIMENSION output_row, int num_rows);

EXTERN_JPEG(int) jsimd_can_h2v2_downsample (void);
EXTERN_JPEG(int) jsimd_can_h2v1_downsample (void);

EXTERN_JPEG(void) jsimd_h2v2_downsample
        (j_compress_ptr cinfo, jpeg_component_info *compptr,
         JSAMPARRAY input_data, JSAMPARRAY output_data);

EXTERN_JPEG(int) jsimd_can_h2v2_smooth_downsample (void);

EXTERN_JPEG(void) jsimd_h2v2_smooth_downsample
        (j_compress_ptr cinfo, jpeg_component_info *compptr,
         JSAMPARRAY input_data, JSAMPARRAY output_data);

EXTERN_JPEG(void) jsimd_h2v1_downsample
        (j_compress_ptr cinfo, jpeg_component_info *compptr,
        JSAMPARRAY input_data, JSAMPARRAY output_data);

EXTERN_JPEG(int) jsimd_can_h2v2_upsample (void);
EXTERN_JPEG(int) jsimd_can_h2v1_upsample (void);
EXTERN_JPEG(int) jsimd_can_int_upsample (void);

EXTERN_JPEG(void) jsimd_h2v2_upsample
        (j_decompress_ptr cinfo, jpeg_component_info *compptr,
         JSAMPARRAY input_data, JSAMPARRAY *output_data_ptr);
EXTERN_JPEG(void) jsimd_h2v1_upsample
        (j_decompress_ptr cinfo, jpeg_component_info *compptr,
         JSAMPARRAY input_data, JSAMPARRAY *output_data_ptr);
EXTERN_JPEG(void) jsimd_int_upsample
        (j_decompress_ptr cinfo, jpeg_component_info *compptr,
         JSAMPARRAY input_data, JSAMPARRAY *output_data_ptr);

EXTERN_JPEG(int) jsimd_can_h2v2_fancy_upsample (void);
EXTERN_JPEG(int) jsimd_can_h2v1_fancy_upsample (void);

EXTERN_JPEG(void) jsimd_h2v2_fancy_upsample
        (j_decompress_ptr cinfo, jpeg_component_info *compptr,
         JSAMPARRAY input_data, JSAMPARRAY *output_data_ptr);
EXTERN_JPEG(void) jsimd_h2v1_fancy_upsample
        (j_decompress_ptr cinfo, jpeg_component_info *compptr,
         JSAMPARRAY input_data, JSAMPARRAY *output_data_ptr);

EXTERN_JPEG(int) jsimd_can_h2v2_merged_upsample (void);
EXTERN_JPEG(int) jsimd_can_h2v1_merged_upsample (void);

EXTERN_JPEG(void) jsimd_h2v2_merged_upsample
        (j_decompress_ptr cinfo, JSAMPIMAGE input_buf,
         JDIMENSION in_row_group_ctr, JSAMPARRAY output_buf);
EXTERN_JPEG(void) jsimd_h2v1_merged_upsample
        (j_decompress_ptr cinfo, JSAMPIMAGE input_buf,
         JDIMENSION in_row_group_ctr, JSAMPARRAY output_buf);

EXTERN_JPEG(int) jsimd_can_huff_encode_one_block (void);

EXTERN_JPEG(JOCTET*) jsimd_huff_encode_one_block
        (void *state, JOCTET *buffer, JCOEFPTR block, int last_dc_val,
         c_derived_tbl *dctbl, c_derived_tbl *actbl);
