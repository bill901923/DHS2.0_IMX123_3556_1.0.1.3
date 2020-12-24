/**
 * @file      upgrade_ext4_format.h
 * @brief     product upgrade interface implementatio
 *
 * Copyright (c) 2019 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/05/28
 * @version   1.0
 */
#ifndef __UPGRADE_EXT4_FORMAT_H
#define __UPGRADE_EXT4_FORMAT_H

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct {
    HI_U32 magic; /* 0xED26FF3A */
    HI_U16 majorVersion;
    HI_U16 minorVersion;
    HI_U16 fileHeaderSize;
    HI_U16 chunkHeaderSize;
    HI_U32 blockSize;
    HI_U32 totalBlocks;
    HI_U32 totalChunks;
    HI_U32 imageCheckSum;
}UPGRADE_EXT4Header;

#define UPGRADE_SPARSE_HEADER_MAGIC     0xed26ff3a
#define UPGRADE_UBI_MAJOR_VERSION       1

#define UPGRADE_CHUNK_TYPE_RAW          0xCAC1
#define UPGRADE_CHUNK_TYPE_FILL         0xCAC2
#define UPGRADE_CHUNK_TYPE_DONT_CARE    0xCAC3
#define UPGRADE_CHUNK_TYPE_CRC32        0xCAC4

typedef struct {
    HI_U16 chunkType;/* refter to CHUNK_TYPE_* */
    HI_U16 reserved0;
    HI_U32 chunkSize;
    HI_U32 totalSize;
} UPGRADE_EXT4ChunkHeader;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif

