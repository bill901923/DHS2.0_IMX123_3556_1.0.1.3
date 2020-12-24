/*
 * jsimddct.h
 *
 * Copyright 2009 Pierre Ossman <ossman@cendio.se> for Cendio AB
 *
 * Based on the x86 SIMD extension for IJG JPEG library,
 * Copyright (C) 1999-2006, MIYASAKA Masaru.
 * For conditions of distribution and use, see copyright notice in jsimdext.inc
 *
 */

EXTERN_JPEG(int) jsimd_can_convsamp (void);
EXTERN_JPEG(int) jsimd_can_convsamp_float (void);

EXTERN_JPEG(void) jsimd_convsamp (JSAMPARRAY sample_data, JDIMENSION start_col,
                             DCTELEM *workspace);
EXTERN_JPEG(void) jsimd_convsamp_float (JSAMPARRAY sample_data,
                                   JDIMENSION start_col,
                                   FAST_FLOAT *workspace);

EXTERN_JPEG(int) jsimd_can_fdct_islow (void);
EXTERN_JPEG(int) jsimd_can_fdct_ifast (void);
EXTERN_JPEG(int) jsimd_can_fdct_float (void);

EXTERN_JPEG(void) jsimd_fdct_islow (DCTELEM *data);
EXTERN_JPEG(void) jsimd_fdct_ifast (DCTELEM *data);
EXTERN_JPEG(void) jsimd_fdct_float (FAST_FLOAT *data);

EXTERN_JPEG(int) jsimd_can_quantize (void);
EXTERN_JPEG(int) jsimd_can_quantize_float (void);

EXTERN_JPEG(void) jsimd_quantize (JCOEFPTR coef_block, DCTELEM *divisors,
                             DCTELEM *workspace);
EXTERN_JPEG(void) jsimd_quantize_float (JCOEFPTR coef_block, FAST_FLOAT *divisors,
                                   FAST_FLOAT *workspace);

EXTERN_JPEG(int) jsimd_can_idct_2x2 (void);
EXTERN_JPEG(int) jsimd_can_idct_4x4 (void);
EXTERN_JPEG(int) jsimd_can_idct_6x6 (void);
EXTERN_JPEG(int) jsimd_can_idct_12x12 (void);

EXTERN_JPEG(void) jsimd_idct_2x2 (j_decompress_ptr cinfo,
                             jpeg_component_info *compptr,
                             JCOEFPTR coef_block, JSAMPARRAY output_buf,
                             JDIMENSION output_col);
EXTERN_JPEG(void) jsimd_idct_4x4 (j_decompress_ptr cinfo,
                             jpeg_component_info *compptr,
                             JCOEFPTR coef_block, JSAMPARRAY output_buf,
                             JDIMENSION output_col);
EXTERN_JPEG(void) jsimd_idct_6x6 (j_decompress_ptr cinfo,
                             jpeg_component_info *compptr,
                             JCOEFPTR coef_block, JSAMPARRAY output_buf,
                             JDIMENSION output_col);
EXTERN_JPEG(void) jsimd_idct_12x12 (j_decompress_ptr cinfo,
                               jpeg_component_info *compptr,
                               JCOEFPTR coef_block, JSAMPARRAY output_buf,
                               JDIMENSION output_col);

EXTERN_JPEG(int) jsimd_can_idct_islow (void);
EXTERN_JPEG(int) jsimd_can_idct_ifast (void);
EXTERN_JPEG(int) jsimd_can_idct_float (void);

EXTERN_JPEG(void) jsimd_idct_islow (j_decompress_ptr cinfo,
                               jpeg_component_info *compptr,
                               JCOEFPTR coef_block, JSAMPARRAY output_buf,
                               JDIMENSION output_col);
EXTERN_JPEG(void) jsimd_idct_ifast (j_decompress_ptr cinfo,
                               jpeg_component_info *compptr,
                               JCOEFPTR coef_block, JSAMPARRAY output_buf,
                               JDIMENSION output_col);
EXTERN_JPEG(void) jsimd_idct_float (j_decompress_ptr cinfo,
                               jpeg_component_info *compptr,
                               JCOEFPTR coef_block, JSAMPARRAY output_buf,
                               JDIMENSION output_col);