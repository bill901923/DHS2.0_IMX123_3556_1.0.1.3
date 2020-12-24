/**
 * @file      upgrade_partition.c
 * @brief     product upgrade interface implementatio
 *
 * Copyright (c) 2019 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/04/26
 * @version   1.0
 */
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include "upgrade_partition.h"
#include "hi_flash.h"
#include "crc.h"
#include "hi_eventhub.h"
#include "hi_math.h"
#include "hi_appcomm_util.h"
#include "hi_appcomm.h"
#include "hi_upgrade.h"
#include "upgrade_ext4_format.h"

#define FS_STRING_JFFS2                           "jffs2"
#define FS_STRING_EXT4                            "ext4"
#define FS_STRING_UBIFS                           "ubi"

#if defined (CFG_SPI_NAND_FLASH) || defined (CFG_NAND_FLASH)
    #define FLASH_TYPE  HI_FLASH_TYPE_NAND_0
    #define MEDIA_NAME  "hinand:"
#elif defined (CFG_EMMC_FLASH)
    #define FLASH_TYPE HI_FLASH_TYPE_EMMC_0
    #define MEDIA_NAME "mmcblk0:"
    #define DEV_MMCBLK_PREFIX     "/dev/mmcblk0"
#else
    #define FLASH_TYPE HI_FLASH_TYPE_SPI_0
    #define MEDIA_NAME "hi_sfc:"
#endif
#define DEV_MTD_PREFIX          "/dev/mtd"
#define DEV_ROOT                "/dev/root"
#define MOUNT_UBIFS_PREFIX         "ubi"

enum FileSystemType{INVALID_FS=0, FS_JFFS2, FS_EXT4, FS_UBIFS};
typedef struct {
    HI_BOOL status;/* 0: if not exist */
    HI_BOOL isMounted;/* 0:not mounted, 1:mounted */
    HI_BOOL isRootfs;
    HI_CHAR rwFlag[3];/* rw or ro */
    HI_CHAR dev[HI_APPCOMM_COMM_STR_LEN];/* /dev/mtd0 /dev/mtd1 ... */
    HI_CHAR mountDev[HI_APPCOMM_COMM_STR_LEN];/* /dev/mtdblock0 /dev/mtdblock1 /dev/mmcblk0p0 ...*/
    HI_CHAR mountDir[HI_APPCOMM_COMM_STR_LEN];
    HI_CHAR fsTypeStr[HI_APPCOMM_COMM_STR_LEN];
    HI_U32 offset;/* offset of the flash */
    HI_U32 size;/* size of the partition */
    HI_U32 eraseSize;
    HI_CHAR name[HI_APPCOMM_COMM_STR_LEN];/* mtd name */
}UPGRADE_PartitionInfo;

typedef struct {
    HI_U32 curPercent;
    HI_U32 offset;
    HI_U32 totalSize;
}UPGRADE_Progress;

typedef struct{
    UPGRADE_Progress progress;
    enum FileSystemType rootfsType;/* jffs2/ubi/... */
    HI_U32 flashSize;
    UPGRADE_PartitionInfo partsInfo[HI_UPGRADE_MAX_PART_CNT];
}UPGRADE_PartitionCtx;

static UPGRADE_PartitionCtx g_upgradePartitionCtx;

static HI_S32 UPGRADE_ReadFile(FILE *fp, HI_U8 *buffer, HI_U32 bufferSize)
{
    HI_U32 readSize = 0;
    HI_U32 ret =0;
    HI_U32 remainSize = bufferSize;

    do {
        ret = fread(&buffer[readSize], sizeof(HI_U8), remainSize, fp);
        if (ret == 0) {
            if (errno == EINTR || errno == EAGAIN) {
                MLOGW("read file interuptted, retry\n");
                HI_usleep(2 * 1000);
                continue;
            }
            MLOGE("fread:%s\n", strerror(errno));
            return HI_FAILURE;
        } else if (ret < remainSize && !feof(fp)) {
            remainSize -= ret;
            readSize += ret;
            continue;
        } else {
            readSize += ret;
            break;
        }
    }while (1);

    return readSize;
}

static HI_VOID UPGRADE_ReportProgress(HI_S32 writtenSize)
{
    HI_EVENT_S event;
    HI_S32 ret = 0;
    HI_U32 offset = g_upgradePartitionCtx.progress.offset;
    HI_U32 totalSize = g_upgradePartitionCtx.progress.totalSize;
    offset += writtenSize;
    memset(&event, 0, sizeof(event));
    event.EventID = HI_EVENT_UPGRADE_PROGRESS;
    event.arg1 = offset*100/totalSize;
    if(event.arg1 > g_upgradePartitionCtx.progress.curPercent) {
        MLOGD("Offset=0x%X, Total=0x%X, event.arg1=%d\n", offset, totalSize, event.arg1);
        ret = HI_EVTHUB_Publish(&event);
        if (ret != HI_SUCCESS) {
            MLOGE("HI Public message failed, offset=%d\n", offset);
        }
    }
    g_upgradePartitionCtx.progress.curPercent = (HI_U32)event.arg1;
    g_upgradePartitionCtx.progress.offset = offset;
}

static inline HI_S32 UPGRADE_IsSparseImage(const UPGRADE_EXT4Header *header)
{
    if (header->magic == UPGRADE_SPARSE_HEADER_MAGIC && header->majorVersion == UPGRADE_UBI_MAJOR_VERSION) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

static HI_S32 UPGRADE_WriteExt4Chunks(HI_HANDLE hiFlash, HI_U32 offset, FILE *fp, const UPGRADE_EXT4Header *ext4Header)
{
    HI_U32 i, chunkOffset;
    HI_U32 chunkLen, readSize;
    HI_S32 ret = HI_SUCCESS;
    UPGRADE_EXT4ChunkHeader chunkHeader;
    HI_U8 *buffer = (HI_U8*)malloc(ext4Header->blockSize);
    HI_APPCOMM_CHECK_POINTER(buffer, HI_FAILURE);

    for (i = 0; i < ext4Header->totalChunks; i++) {
        ret = UPGRADE_ReadFile(fp, (HI_U8*)&chunkHeader, sizeof(chunkHeader));
        if (ret < sizeof(chunkHeader)) {
            MLOGE("read failed\n");
            break;
        }
        UPGRADE_ReportProgress(sizeof(chunkHeader)/2);
        switch (chunkHeader.chunkType) {
            case UPGRADE_CHUNK_TYPE_RAW:
                chunkLen = chunkHeader.chunkSize * ext4Header->blockSize;
                if (chunkHeader.totalSize != (chunkLen + ext4Header->chunkHeaderSize)) {
                    MLOGE("No.%d chunkHeader size error.\n", i);
                    return 1;
                }
                MLOGD("chunkSize=0x%X, blockSize=0x%X, chunkLen=0x%X\n",
                        chunkHeader.chunkSize, ext4Header->blockSize, chunkLen);
                for(chunkOffset=0; chunkOffset<chunkLen; chunkOffset += readSize) {
                    ret = UPGRADE_ReadFile(fp, buffer, ext4Header->blockSize);
                    if(ret < 0) {
                        MLOGE("read file failed\n");
                        break;
                    }
                    readSize = (HI_U32) ret;
                    ret = HI_Flash_Write(hiFlash, offset, buffer, readSize, HI_FLASH_RW_FLAG_RAW);
                    if (ret == HI_FAILURE) {
                        MLOGE("flash write failed\n");
                        break;
                    }
                    UPGRADE_ReportProgress(readSize/2);
                    offset += readSize;
                }
                break;

            case UPGRADE_CHUNK_TYPE_DONT_CARE:
                MLOGD("Chunk type don't care\n");
                break;

            default:
                MLOGE("sparse: unknow chunkHeader type:0x%04x.\n", chunkHeader.chunkType);
                return HI_FAILURE;
        }
    }
    HI_APPCOMM_SAFE_FREE(buffer);

    return ret;
}

static HI_S32 UPGRADE_WriteExt4File(HI_HANDLE hiFlash, const HI_CHAR *fileName, HI_U32 eraseUnit)
{
    HI_S32 ret = 0;
    HI_U32 offset = 0;
    FILE *fp = fopen(fileName, "rb");
    if(fp == NULL) {
        MLOGE("fopen %s failed\n", fileName);
        return HI_FAILURE;
    }

    UPGRADE_EXT4Header ext4Header;
    ret = UPGRADE_ReadFile(fp, (HI_U8*)&ext4Header, sizeof(ext4Header));
    if (ret < sizeof(ext4Header)) {
        MLOGE("read file failed\n");
        fclose(fp);
        return HI_FAILURE;
    }
    if (!UPGRADE_IsSparseImage(&ext4Header)) {
        MLOGE("Invalid sparse format. magic=0x%X, majorVersion=0x%X\n", ext4Header.magic, ext4Header.majorVersion);
        fclose(fp);
        return HI_FAILURE;
    }
    if(ext4Header.blockSize > eraseUnit) {
        MLOGE("invalid blockSize=0x%X, eraseUnit=0x%X\n", ext4Header.blockSize, eraseUnit);
        fclose(fp);
        return HI_FAILURE;
    }
    UPGRADE_ReportProgress(sizeof(ext4Header)/2);
    ret = UPGRADE_WriteExt4Chunks(hiFlash, offset, fp, &ext4Header);
    fclose(fp);

    return (ret>=0)?HI_SUCCESS:HI_FAILURE;
}

static HI_S32 UPGRADE_WriteBinFile(HI_HANDLE hiFlash, const HI_CHAR *fileName,HI_U32 eraseUnit)
{
    HI_S32 ret = HI_FAILURE;
    HI_U8 *dstBuffer = NULL;
    HI_U32 offset = 0;
    HI_U32 readSize, progress;
    struct stat fileStat;

    if (stat(fileName, &fileStat) > 0) {
        MLOGE("stat %s\n", strerror(errno));
        return HI_FAILURE;
    }

    FILE *fp = fopen(fileName, "rb");
    if(fp == NULL) {
        MLOGE("fopen:%s\n", strerror(errno));
        return HI_FAILURE;
    }
    dstBuffer = (HI_U8*)malloc(eraseUnit);

    while(dstBuffer != NULL && offset < fileStat.st_size) {
        memset(dstBuffer, 0xFF, eraseUnit);
        ret = UPGRADE_ReadFile(fp, dstBuffer, eraseUnit);
        if(ret < 0) {
            MLOGE(RED"read file failed"NONE"\n");
            break;
        }
        readSize = (HI_U32)ret;
        progress = (readSize==eraseUnit)?(eraseUnit/2):((1+readSize)/2);
        if(strstr(fileName, g_upgradePartitionCtx.partsInfo[0].name)) {/* u-boot part, needs erase */
            if( HI_Flash_Erase(hiFlash, offset, eraseUnit) == HI_FAILURE) {
                MLOGE("Erase failed, offset=0x%X, readSize=0x%X\n", offset, eraseUnit);
                return ret;
            }
            progress = (readSize==eraseUnit)?(eraseUnit/2):(1+readSize/2);
            UPGRADE_ReportProgress(progress);
        }
#if defined (CFG_SPI_NAND_FLASH) || defined (CFG_NAND_FLASH)
        ret = HI_Flash_Write(hiFlash, offset, dstBuffer, eraseUnit, HI_FLASH_RW_FLAG_RAW);
#else
        ret = HI_Flash_Write(hiFlash, offset, dstBuffer, readSize, HI_FLASH_RW_FLAG_RAW);
#endif
        if(ret < 0) {
            MLOGE("write flash failed\n");
            break;
        }
        offset += readSize;
        UPGRADE_ReportProgress(progress);
        if(offset == fileStat.st_size) {
            MLOGD(GREEN"Finish reading, readSize=%d"NONE"\n", readSize);
            break;
        }
    }

    HI_APPCOMM_SAFE_FREE(dstBuffer);
    fclose(fp);

    return (ret >= 0)? HI_SUCCESS:HI_FAILURE;
}

static HI_S32 UPGRADE_SearchPartitionByName(const HI_CHAR *partitionName)
{
    HI_S32 ret = HI_FAILURE;

    HI_S32 i = 0;
    for(i=0; i<ARRAY_SIZE(g_upgradePartitionCtx.partsInfo); i++) {
        if(!strncmp(partitionName, g_upgradePartitionCtx.partsInfo[i].name,
                strnlen(partitionName, HI_APPCOMM_COMM_STR_LEN)) && g_upgradePartitionCtx.partsInfo[i].status) {
            MLOGI("%s found, i=%d\n", partitionName, i);
            ret = i;
            break;
        }
    }

    return ret;
}

static HI_S32 UPGRADE_ErasePartition(HI_HANDLE hiHandle, HI_U64 partSize, HI_U64 blockSize, const HI_CHAR* imageURL)
{
    HI_S32 ret = HI_SUCCESS;
    HI_U64  offset, progress;
    struct stat fileStat;

    if (stat(imageURL, &fileStat) > 0) {
        MLOGE("stat %s\n", strerror(errno));
        return HI_FAILURE;
    }
    for (offset = 0, progress = 0; offset < partSize;) {
        MLOGD("Erasing at 0x%llX\n", offset);
        ret = HI_Flash_Erase(hiHandle, offset, blockSize);
        if (ret == HI_FAILURE) {
            MLOGE("Erase failed, offset=0x%llX, BlockSize=0x%llX\n", offset, blockSize);
            return HI_FAILURE;
        }
        if(progress < fileStat.st_size) {
            if(progress + blockSize > fileStat.st_size) {
                UPGRADE_ReportProgress((HI_U32)(fileStat.st_size-progress)/2);
            } else {
                UPGRADE_ReportProgress((HI_U32)blockSize/2);
            }
            progress += blockSize;
        }
        offset += blockSize;
    }

    return HI_SUCCESS;
}

static HI_S32 UPGRADE_WritePartitionImage(const HI_CHAR *imageURL, const HI_CHAR *partitionName)
{
    HI_S32 ret = 0;
    HI_HANDLE hiHandle = 0;

    HI_S32 idx = UPGRADE_SearchPartitionByName(partitionName);
    if(idx == HI_FAILURE) {
        MLOGE("%s not found, idx=%d\n", partitionName, idx);
        return HI_FAILURE;
    }
#ifndef __HuaweiLite__
    hiHandle = HI_Flash_OpenByTypeAndName(FLASH_TYPE, g_upgradePartitionCtx.partsInfo[idx].dev);
    if(hiHandle == INVALID_FD) {
        MLOGE("FLASH_TYPE=%d, open %s failed\n", FLASH_TYPE, g_upgradePartitionCtx.partsInfo[idx].dev);
        return HI_FAILURE;
    }
#else
    MLOGI(RED"offset=0x%X, size=0x%X"NONE"\n", g_upgradePartitionCtx.partsInfo[idx].offset,
                                               g_upgradePartitionCtx.partsInfo[idx].size);
    hiHandle = HI_Flash_OpenByTypeAndAddr(FLASH_TYPE, g_upgradePartitionCtx.partsInfo[idx].offset,
                                          g_upgradePartitionCtx.partsInfo[idx].size);
#endif
    MLOGI("Opened %s, hiHandle=%d\n", g_upgradePartitionCtx.partsInfo[idx].dev, hiHandle);

    HI_Flash_InterInfo_S flashInterInfoS;
    ret = HI_Flash_GetInfo(hiHandle, &flashInterInfoS);
    if(ret != HI_SUCCESS) {
        MLOGE("\n");
        return HI_FAILURE;
    }
    if(idx) {/* if not u-boot, erase all of the partition */
        MLOGI("%s needs to erase\n", partitionName);
        ret = UPGRADE_ErasePartition(hiHandle, flashInterInfoS.PartSize, flashInterInfoS.BlockSize, imageURL);
        if(ret != HI_SUCCESS) {
            MLOGE("Erase partition failed\n");
            return HI_FAILURE;
        }
    }
    MLOGI("BlockSize=0x%X, PageSize=0x%X\n", flashInterInfoS.BlockSize, flashInterInfoS.PageSize);
    if(strstr(partitionName, ".ext4")) {/* To upgrade ext4 image */
        ret = UPGRADE_WriteExt4File(hiHandle, imageURL, flashInterInfoS.BlockSize);
    } else {
        ret = UPGRADE_WriteBinFile(hiHandle, imageURL, flashInterInfoS.BlockSize);
    }
    if(HI_Flash_Close(hiHandle) != HI_SUCCESS) {
        MLOGE("Hi flash close\n");
        return HI_FAILURE;
    }

    return ret;
}

HI_S32 UPGRADE_WritePartitionByName(const HI_CHAR *imageFilePath, const HI_CHAR *partitionName)
{
    HI_S32 ret = 0;
    HI_CHAR pktRealPath[HI_UPGRADE_PKT_PATH_MAX_LEN] = {0,};
    HI_CHAR pktURL[HI_UPGRADE_PKT_PATH_MAX_LEN] = {0,};

    if(!realpath(imageFilePath, pktRealPath)){
        return HI_FAILURE;
    }
    snprintf(pktURL, sizeof(pktURL), "%s/%s", pktRealPath, partitionName);
    ret = UPGRADE_SearchPartitionByName(partitionName);
    if (ret == HI_FAILURE) {
        MLOGE("%s not found\n", partitionName);
        return HI_FAILURE;
    }
    MLOGI(RED"partsInfo[%d].rwFlag=%s."NONE"\n", ret, g_upgradePartitionCtx.partsInfo[ret].rwFlag);
    if(g_upgradePartitionCtx.partsInfo[ret].isMounted == HI_TRUE &&
       (g_upgradePartitionCtx.partsInfo[ret].rwFlag[0]=='r' && g_upgradePartitionCtx.partsInfo[ret].rwFlag[1]=='w')) {
        HI_system("sync");
        HI_CHAR cmdBuf[HI_APPCOMM_COMM_STR_LEN] = {};
        snprintf(cmdBuf, sizeof(cmdBuf), "mount -o remount,ro %s %s",g_upgradePartitionCtx.partsInfo[ret].mountDev,
                g_upgradePartitionCtx.partsInfo[ret].mountDir);
        MLOGI("cmdBuf=%s\n", cmdBuf);
        HI_system(cmdBuf);
        MLOGI("imageFilePath=%s, pktRealPath=%s, partitionName=%s\n", imageFilePath, pktRealPath, partitionName);
        ret = UPGRADE_WritePartitionImage(pktURL, partitionName);
        if(ret != HI_SUCCESS) {
            snprintf(cmdBuf, sizeof(cmdBuf), "mount -o remount,rw %s %s", g_upgradePartitionCtx.partsInfo[ret].mountDev,
                     g_upgradePartitionCtx.partsInfo[ret].mountDir);
            MLOGI("cmdBuf=%s\n", cmdBuf);
            HI_system(cmdBuf);
        }
    } else {
        MLOGI("imageFilePath=%s, pktRealPath=%s, partitionName=%s\n", imageFilePath, pktRealPath, partitionName);
        ret = UPGRADE_WritePartitionImage(pktURL, partitionName);
    }

    return ret;
}

HI_U64 UPGRADE_CalculateSize(HI_U32 num, HI_CHAR unit)
{
    HI_U64 ret = 0;

    switch (unit){
        case 'k':
        case 'K':
            ret = (HI_U64)num*1024;
            break;

        case 'm':
        case 'M':
            ret = (HI_U64)num*1024*1024;
            break;

        case 'g':
        case 'G':
            ret = (HI_U64)num*1024*1024*1024;
            break;

        default:
            MLOGE("invalid unit=%c\n", unit);
            break;
    }

    return ret;
}

/* parse n from /dev/mmcblk0pn   /dev/mtdblockn ...*/
static HI_S32 UPGRADE_ParseDevNo(const HI_CHAR *devName)
{
    HI_S32 i = 0;
    HI_BOOL found = HI_FALSE;

    MLOGI("devName:%s\n", devName);

    for(i=strnlen(devName, HI_APPCOMM_COMM_STR_LEN)-1; i>0 ;i--) {
        if(devName[i] > '9' || devName[i] < '0') {
            found = HI_TRUE;
            MLOGD("found, i=%d\n", i);
            break;
        }
    }
    if(found) {
        return strtol(&devName[i+1], NULL, 10);
    }
    return HI_FAILURE;
}

static HI_S32 UPGRADE_ParseRootfsInfo(const HI_CHAR *bootArgs, HI_U32 bootArgsLen)
{
    HI_S32 ret = HI_FAILURE;
    HI_CHAR mountDev[HI_APPCOMM_COMM_STR_LEN];
    HI_CHAR *str = strstr(bootArgs, "root=");

    if(str) {
        memset(mountDev, '\0', sizeof(mountDev));
        if(strstr(bootArgs, "ubi.mtd=")) {
            str = strstr(bootArgs, "ubi.mtd=");
            sscanf(str, "%s", mountDev);
        } else {
            sscanf(str, "%s", mountDev);
        }
        ret = UPGRADE_ParseDevNo(mountDev);
#ifdef CFG_EMMC_FLASH
        ret = ret - 1;
#endif
        if(ret < 0) {
            MLOGE("parse devNo failed\n");
            return HI_FAILURE;
        }
        MLOGI(RED"found rootfs devNo=%d"NONE"\n", ret);
        snprintf(g_upgradePartitionCtx.partsInfo[ret].mountDev, HI_APPCOMM_COMM_STR_LEN, "%s", mountDev);
        snprintf(g_upgradePartitionCtx.partsInfo[ret].mountDir, HI_APPCOMM_COMM_STR_LEN, "/");
        g_upgradePartitionCtx.partsInfo[ret].isMounted = HI_TRUE;
        g_upgradePartitionCtx.partsInfo[ret].isRootfs = HI_TRUE;
        ret = HI_SUCCESS;
    } else {
        MLOGE("can not find root\n");
        return HI_FAILURE;
    }

    if (strstr(bootArgs, FS_STRING_JFFS2)) {
        MLOGI("%s found\n", FS_STRING_JFFS2);
        g_upgradePartitionCtx.rootfsType = FS_JFFS2;
    } else if (strstr(bootArgs, FS_STRING_EXT4)) {
        MLOGI("%s found\n", FS_STRING_EXT4);
        g_upgradePartitionCtx.rootfsType = FS_EXT4;
    } else if (strstr(bootArgs, FS_STRING_UBIFS)) {
        MLOGI("%s found\n", FS_STRING_UBIFS);
        g_upgradePartitionCtx.rootfsType = FS_UBIFS;
    }

    return (g_upgradePartitionCtx.rootfsType != INVALID_FS)?HI_SUCCESS:HI_FAILURE;
}

HI_S32 UPGRADE_ParsePartitionInfo(const HI_CHAR *bootArgs, HI_U32 bootArgsLen)
{
    HI_CHAR *str = NULL;
    HI_U32 i = 0;
    HI_U32 partSize = 0;
    HI_CHAR sizeUnit = 0;
    HI_S32 ret = HI_SUCCESS;
    HI_CHAR partitonName[HI_UPGRADE_MAX_ENV_LEN];
    HI_U32 offset = 0;

    ret = UPGRADE_ParseRootfsInfo(bootArgs, bootArgsLen);
    if(ret != HI_SUCCESS) {
        MLOGE("can't get filesystem type\n");
        return HI_FAILURE;
    }

    str = strstr(bootArgs, MEDIA_NAME);
    if(str == NULL) {
        MLOGE("strstr error:%s, MEDIA_NAME=%s\n", strerror(errno), MEDIA_NAME);
        return HI_FAILURE;
    }
    str += strnlen(MEDIA_NAME, HI_APPCOMM_COMM_STR_LEN);

    for(i=0; i<HI_UPGRADE_MAX_PART_CNT && str<&bootArgs[bootArgsLen]; i++) {
        ret = sscanf(str, "%d%c(%[^)]", &partSize, &sizeUnit, partitonName);
        if(ret <= 0) {
            MLOGD("end\n");
            break;
        }
        snprintf(g_upgradePartitionCtx.partsInfo[i].name, HI_APPCOMM_COMM_STR_LEN, "%s", partitonName);
#ifdef CFG_EMMC_FLASH
        snprintf(g_upgradePartitionCtx.partsInfo[i].dev, HI_APPCOMM_COMM_STR_LEN, "%s", partitonName);
#else
        snprintf(g_upgradePartitionCtx.partsInfo[i].dev, HI_APPCOMM_COMM_STR_LEN, "/dev/mtd%d", i);
        snprintf(g_upgradePartitionCtx.partsInfo[i].mountDev, HI_APPCOMM_COMM_STR_LEN, "/dev/mtdblock%d", i);
#endif
        g_upgradePartitionCtx.partsInfo[i].status = HI_TRUE;
        g_upgradePartitionCtx.partsInfo[i].size = (HI_U32)UPGRADE_CalculateSize(partSize, sizeUnit);
        g_upgradePartitionCtx.partsInfo[i].offset = offset;
        offset += g_upgradePartitionCtx.partsInfo[i].size;

        g_upgradePartitionCtx.flashSize += g_upgradePartitionCtx.partsInfo[i].size;
        MLOGD("partsInfo[%d].name=%s, dev=%s, offset=0x%X, size=0x%X\n", i,
                g_upgradePartitionCtx.partsInfo[i].name, g_upgradePartitionCtx.partsInfo[i].dev,
                g_upgradePartitionCtx.partsInfo[i].offset, g_upgradePartitionCtx.partsInfo[i].size);
        str += strnlen(partitonName, HI_APPCOMM_COMM_STR_LEN);
        str = strstr(str,  "),");
        if(str == NULL) {
            MLOGD("end\n");
            break;
        }
        str += 2;
        if(str[2] == '-') {
            MLOGD("end\n");
            break;
        }
    }

    return HI_SUCCESS;
}

#ifndef __HuaweiLite__
HI_S32 UPGRADE_GetFlashSize(HI_VOID)
{
    MLOGI("flashSize=0x%X\n", g_upgradePartitionCtx.flashSize);
    return g_upgradePartitionCtx.flashSize;
}
#else
HI_S32 UPGRADE_GetFlashSize(HI_VOID)
{
    return 0;
}
#endif

HI_S32 UPGRADE_GetPartitionSizeByName(const HI_CHAR *partitionName)
{
    HI_APPCOMM_CHECK_POINTER(partitionName, HI_FAILURE);
    HI_S32 ret = 0;

    ret = UPGRADE_SearchPartitionByName(partitionName);
    if(ret == HI_FAILURE) {
        MLOGE("%s not found!\n", partitionName);
        return ret;
    }

    return (HI_S32)g_upgradePartitionCtx.partsInfo[ret].size;
}

HI_VOID UPGRADE_SetPktTotalSize(HI_U32 totalSize)
{
    g_upgradePartitionCtx.progress.curPercent = 0;
    g_upgradePartitionCtx.progress.offset = 0;
    g_upgradePartitionCtx.progress.totalSize = totalSize;
}

#ifndef __HuaweiLite__

static HI_S32 UPGRADE_ParseUbiFsMtdNum(const HI_CHAR* ubiName)
{
    HI_S32  ret = 0;
    HI_CHAR sysPath[HI_APPCOMM_COMM_STR_LEN] = {0};
    HI_CHAR mtdNumStr[HI_APPCOMM_COMM_STR_LEN] = {0};

    snprintf(sysPath, sizeof(sysPath), "/sys/class/ubi/%s/mtd_num", ubiName);

    FILE *fp = fopen(sysPath, "r");
    if(fp == NULL) {
        MLOGE("fopen %s failed\n", sysPath);
        return HI_FAILURE;
    }
    while (!fgets(mtdNumStr, sizeof(mtdNumStr) - 1, fp)) {
        break;
    }
    MLOGI("mtdNumStr=%s\n", mtdNumStr);
    ret = strtol(mtdNumStr, NULL, 10);

    return ret;
}

static HI_S32 UPGRADE_ParseProcCmdline(HI_VOID)
{
    HI_S32 ret = 0;

    HI_CHAR procCmdLine[HI_UPGRADE_MAX_ENV_LEN] = {};
    FILE *fp = fopen("/proc/cmdline", "r");
    if(fp == NULL) {
        MLOGE("fopen failed\n");
        return HI_FAILURE;
    }
    while (!fgets(procCmdLine, sizeof(procCmdLine) - 1, fp)) {
        break;
    }
    fclose(fp);
    MLOGI("procCmdLine[%d Bytes]:%s\n", strnlen(procCmdLine, HI_UPGRADE_MAX_ENV_LEN), procCmdLine);
    ret = UPGRADE_ParsePartitionInfo(procCmdLine, strnlen(procCmdLine, HI_UPGRADE_MAX_ENV_LEN));
    return ret;
}

static HI_VOID UPGRADE_UpdateParttitionMountInfo(HI_U32 mtdNo, UPGRADE_PartitionInfo *partitionInfo)
{
    MLOGI("mtdNo=%d, mountDev=%s, mountDir=%s, name=%s, fsTypeStr=%s, isMounted=%d, rwFlag=%s\n",
          mtdNo, partitionInfo->mountDev, partitionInfo->mountDir,
          partitionInfo->name, partitionInfo->fsTypeStr, partitionInfo->isMounted, partitionInfo->rwFlag);

    snprintf(g_upgradePartitionCtx.partsInfo[mtdNo].mountDir, HI_APPCOMM_COMM_STR_LEN, "%s", partitionInfo->mountDir);
    snprintf(g_upgradePartitionCtx.partsInfo[mtdNo].mountDev, HI_APPCOMM_COMM_STR_LEN, "%s", partitionInfo->mountDev);
    snprintf(g_upgradePartitionCtx.partsInfo[mtdNo].fsTypeStr, HI_APPCOMM_COMM_STR_LEN,"%s", partitionInfo->fsTypeStr);
    snprintf(g_upgradePartitionCtx.partsInfo[mtdNo].rwFlag, sizeof(partitionInfo->rwFlag), "%s", partitionInfo->rwFlag);
    g_upgradePartitionCtx.partsInfo[mtdNo].isMounted = partitionInfo->isMounted;
    g_upgradePartitionCtx.partsInfo[mtdNo].isMounted = HI_TRUE;
}

HI_S32 UPGRADE_UpdateMountsInfo(HI_VOID)
{
    HI_S32 mtdNo;
    UPGRADE_PartitionInfo partitionInfo;
    HI_CHAR procMount[HI_UPGRADE_MAX_ENV_LEN];

    FILE *fp = fopen("/proc/mounts", "r");
    if(fp == NULL) {
        MLOGE("fopen failed\n");
        return HI_FAILURE;
    }
    do {
        memset(&partitionInfo, 0, sizeof(partitionInfo));
        memset(partitionInfo.mountDev, '\0', sizeof(partitionInfo.mountDev));
        while (!fgets(procMount, sizeof(procMount), fp)) {
            break;
        }
        if (!memcmp(procMount, DEV_ROOT, strnlen(DEV_ROOT, HI_APPCOMM_COMM_STR_LEN))) {
            sscanf(procMount, "%s %s %s %[%a-zA-Z]", partitionInfo.mountDev, partitionInfo.mountDir, partitionInfo.fsTypeStr, partitionInfo.rwFlag);
            for(mtdNo=0; mtdNo<ARRAY_SIZE(g_upgradePartitionCtx.partsInfo); mtdNo++) {
                if (g_upgradePartitionCtx.partsInfo[mtdNo].isRootfs) {
                    partitionInfo.isMounted = HI_TRUE;
                    UPGRADE_UpdateParttitionMountInfo((HI_U32) mtdNo, &partitionInfo);
                }
            }
            continue;
        }
#ifdef CFG_EMMC_FLASH
        if (!memcmp(procMount, DEV_MMCBLK_PREFIX, strnlen(DEV_MMCBLK_PREFIX, HI_APPCOMM_COMM_STR_LEN))) {
#else
        if (!memcmp(procMount, DEV_MTD_PREFIX, strnlen(DEV_MTD_PREFIX, HI_APPCOMM_COMM_STR_LEN))) {
#endif
            sscanf(procMount, "%s %s %s %[%a-zA-Z]", partitionInfo.mountDev, partitionInfo.mountDir, partitionInfo.fsTypeStr, partitionInfo.rwFlag);
            mtdNo = UPGRADE_ParseDevNo(partitionInfo.mountDev);
#ifdef CFG_EMMC_FLASH
            mtdNo = mtdNo - 1;
#endif
        } else if(!memcmp(procMount, MOUNT_UBIFS_PREFIX, strnlen(MOUNT_UBIFS_PREFIX, HI_APPCOMM_COMM_STR_LEN))){
            sscanf(procMount, "%s %s %s %[%a-zA-Z]", partitionInfo.mountDev, partitionInfo.mountDir, partitionInfo.fsTypeStr, partitionInfo.rwFlag);
            sscanf(partitionInfo.mountDev, "%[a-zA-Z0-9]", partitionInfo.mountDev);
            MLOGI("partitionInfo.mountDev=%s\n",  partitionInfo.mountDev);
            mtdNo = UPGRADE_ParseUbiFsMtdNum(partitionInfo.mountDev);
            snprintf(partitionInfo.mountDev, sizeof(partitionInfo.mountDev), "/dev/mtdblock%d", mtdNo);
        } else {
            continue;
        }
        if(mtdNo < 0) {
            MLOGE("parse dev no failed,mtdNo=%d\n", mtdNo);
            return HI_FAILURE;
        }
        MLOGD("mtdNo=%d, mountDev:%s, mountDir:%s, fsType:%s,rwFlag:%s\n", mtdNo, partitionInfo.mountDev,
              partitionInfo.mountDir, partitionInfo.fsTypeStr, partitionInfo.rwFlag);
        if(mtdNo >= HI_UPGRADE_MAX_PART_CNT) {
            MLOGW("Invalid mtdno %d\n", mtdNo);
        } else if(g_upgradePartitionCtx.partsInfo[mtdNo].status){
            partitionInfo.isMounted = HI_TRUE;
            UPGRADE_UpdateParttitionMountInfo((HI_U32) mtdNo, &partitionInfo);
        }
    }while (!feof(fp));
    fclose(fp);

    return HI_SUCCESS;
}
#endif

HI_S32 UPGRADE_PartitionInit(HI_VOID)
{
    HI_S32 ret = HI_SUCCESS;
#ifndef __HuaweiLite__
    ret = UPGRADE_ParseProcCmdline();
#else
#endif

    return ret;
}

HI_S32 UPGRADE_PartitionDeinit(HI_VOID)
{
    memset(&g_upgradePartitionCtx, 0, sizeof(g_upgradePartitionCtx));
    return HI_SUCCESS;
}
