/**
 * @file      upgrade_partition.h
 * @brief     upgrade interface defination
 *
 * Copyright (c) 2019 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/4/26
 * @version   1.0
 */

#ifndef HIFLASH_TEST_UPGRADE_MTD_H
#define HIFLASH_TEST_UPGRADE_MTD_H
#include "hi_appcomm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/**
 * @brief     Write a bin file to MTD, the srcFileName must match the relevant MTD.
 * @return    0 success,non-zero error code.
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/4/26
 */
HI_S32 UPGRADE_WritePartitionByName(const HI_CHAR *imageFilePath, const HI_CHAR *partitionName);

/**
 * @brief     set the image size of the upgrade package.
 * @return    NA
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/5/20
 */
HI_VOID UPGRADE_SetPktTotalSize(HI_U32 totalSize);

/**
 * @brief     Init with the image size of the upgrade package.
 * @return    NA
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/5/20
 */
HI_S32 UPGRADE_PartitionInit(HI_VOID);
/**
 * @brief     partition deinit
 * @return    NA
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/5/20
 */
HI_S32 UPGRADE_PartitionDeinit(HI_VOID);

/**
 * @brief     Get storage size.
 * @return    0:error, other: real size
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/5/20
 */
HI_S32 UPGRADE_GetFlashSize(HI_VOID);

/**
 * @brief     Get storage size.
 * @return    0:error, other: real size
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/5/20
 */
HI_S32 UPGRADE_GetPartitionSizeByName(const HI_CHAR *partitionName);

/**
 * @brief     Parse partition info from bootArgs, bootArgs could be read from config file, or /proc/cmdline
 * @return    HI_SUCCESS, HI_FAILURE
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/6/19
 */
HI_S32 UPGRADE_ParsePartitionInfo(const HI_CHAR *bootArgs, HI_U32 bootArgsLen);

/**
 * @brief     Update mounts info
 * @return    HI_SUCCESS, HI_FAILURE
 * @exception None
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/6/22
 */
HI_S32 UPGRADE_UpdateMountsInfo(HI_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

