/**
 * @file    filemng_repairer.c
 * @brief   file manager repairer function.
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/19
 * @version   1.0

 */
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "hi_mp4_format.h"
#include "hi_eventhub.h"

#include "hi_appcomm_util.h"
#include "filemng_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define FILEMNG_REPAIRER_LAST_FILE_NAME ".LastFileName" /**<This file will save the last video name recorded */
static HI_BOOL s_bParseExtraMdat;                       /**<Parse extra Mdat or not */
static HI_FILEMNG_REPAIR_OPERATION_S g_opts = { NULL, NULL, NULL, NULL };

#ifndef SUPPORT_CUSTOM_BACKUP_FILE
static HI_S32 REPAIRER_AddFileName(const HI_CHAR *filePath, HI_VOID *argv)
{
#define FILEMNG_REPAIRER_MAX_BACKUP_NUM (4)
    HI_CHAR szFilePath[FILEMNG_REPAIRER_MAX_BACKUP_NUM][HI_APPCOMM_MAX_PATH_LEN] = { 0 };
    HI_U32 u32BackupCnt = 0;
    HI_S32 i = 0;
    HI_BOOL bCover = HI_FALSE;
    HI_CHAR *lastFileName = (HI_CHAR *)argv;
    FILE *fp = fopen(lastFileName, "ab+");
    if (NULL == fp) {
        MLOGE("fopen[%s] Error:%s\n", lastFileName, strerror(errno));
        return HI_FILEMNG_EINTER;
    }
    fseek(fp, 0L, SEEK_SET);
    while (NULL != fgets(szFilePath[u32BackupCnt], HI_APPCOMM_MAX_PATH_LEN, fp)) {
        u32BackupCnt++;
        if (u32BackupCnt >= FILEMNG_REPAIRER_MAX_BACKUP_NUM) {
            bCover = HI_TRUE;
            fclose(fp);
            fp = fopen(lastFileName, "wb");
            if (NULL == fp) {
                MLOGE("fopen[%s] Error:%s\n", lastFileName, strerror(errno));
                return HI_FILEMNG_EINTER;
            }
            break;
        }
    }
    if (bCover) {
        fseek(fp, 0L, SEEK_SET);
        for (i = 1; i < FILEMNG_REPAIRER_MAX_BACKUP_NUM; i++) {
            if (0 > fputs(szFilePath[i], fp)) {
                MLOGE("save LastFileName fail!%s \n", strerror(errno));
                fclose(fp);
                return HI_FAILURE;
            }
        }
    } else {
        fseek(fp, 0L, SEEK_END);
    }
    if (0 > fputs(filePath, fp)) {
        MLOGE("save LastFileName fail!%s \n", strerror(errno));
        fclose(fp);
        return HI_FAILURE;
    }
    fwrite("\n", 1, 1, fp);
    fflush(fp);
#ifndef __LITEOS__
    fsync(fileno(fp));
#endif
    fclose(fp);
    return HI_SUCCESS;
}

static HI_S32 REPAIRER_GetFileName(HI_U32 fileIndex, HI_CHAR *filePath, HI_U32 maxFilePathLen, HI_VOID *argv)
{
    HI_CHAR *lastFileName = (HI_CHAR *)argv;
    HI_U32 index = 0;
    HI_CHAR szFilePath[HI_APPCOMM_MAX_PATH_LEN] = { 0 };
    /**get last file name */
    FILE *fp = fopen(lastFileName, "rb");
    if (NULL == fp) {
        MLOGD("fopen[%s] Error:%s, won't repair any file.\n", lastFileName, strerror(errno));
        return HI_FAILURE;
    }
    while (NULL != fgets(szFilePath, HI_APPCOMM_MAX_PATH_LEN, fp)) {
        szFilePath[strnlen(szFilePath, HI_APPCOMM_MAX_PATH_LEN) - 1] = '\0';
        if (index == fileIndex) {
            snprintf(filePath, maxFilePathLen, "%s", szFilePath);
            return HI_SUCCESS;
        }
        index++;
    }
    return HI_FAILURE;
}

static HI_S32 REPAIRER_GetFileNameCnt(HI_U32 *cnt, HI_VOID *argv)
{
    HI_CHAR *lastFileName = (HI_CHAR *)argv;
    HI_CHAR filePath[HI_APPCOMM_MAX_PATH_LEN] = { 0 };
    HI_U32 fileNameCnt = 0;
    /**get last file name */
    FILE *fp = fopen(lastFileName, "rb");
    if (NULL == fp) {
        MLOGD("fopen[%s] Error:%s, won't repair any file.\n", lastFileName, strerror(errno));
        *cnt = 0;
        return HI_SUCCESS;
    }
    while (NULL != fgets(filePath, HI_APPCOMM_MAX_PATH_LEN, fp)) {
        fileNameCnt++;
    }
    fclose(fp);
    *cnt = fileNameCnt;
    return HI_SUCCESS;
}
#endif

static HI_S32 REPAIRER_CheckMP4(const HI_CHAR *pszFilePath)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_MW_PTR Handle = NULL;
    HI_U64 u64Duration = 0;
    HI_MP4_CONFIG_S stMP4Cfg = { 0 };
    snprintf(stMP4Cfg.aszFileName, HI_MP4_MAX_FILE_NAME, pszFilePath);
    stMP4Cfg.enConfigType = HI_MP4_CONFIG_DEMUXER;
    stMP4Cfg.stDemuxerConfig.u32VBufSize = 1 << 20;
    s32Ret = HI_MP4_Create(&Handle, &stMP4Cfg);
    if (HI_SUCCESS != s32Ret) {
        return s32Ret;
    }
    HI_MP4_FILE_INFO_S stMP4Info;
    s32Ret = HI_MP4_GetFileInfo(Handle, &stMP4Info);
    HI_MP4_Destroy(Handle, &u64Duration);
    return s32Ret;
}

HI_S32 FILEMNG_REPAIRER_Init(const HI_FILEMNG_REPAIR_CFG_S *pstRepairCfg)
{
#ifdef SUPPORT_CUSTOM_BACKUP_FILE
    g_opts = pstRepairCfg->operation;
#else
    HI_CHAR szTopPath[HI_APPCOMM_MAX_PATH_LEN] = { 0 };
    snprintf(szTopPath, HI_APPCOMM_MAX_PATH_LEN, "%s%s", pstRepairCfg->szRootPath, pstRepairCfg->szTopDirName);
    HI_S32 s32Ret = HI_mkdir(szTopPath, 0777);
    if (HI_SUCCESS != s32Ret) {
        return HI_FILEMNG_EINTER;
    }
    static HI_CHAR lastFileName[HI_APPCOMM_MAX_PATH_LEN];
    snprintf(lastFileName, sizeof(lastFileName), "%s/%s", szTopPath, FILEMNG_REPAIRER_LAST_FILE_NAME);
    g_opts.addRepairFileName = REPAIRER_AddFileName;
    g_opts.getRepairFileName = REPAIRER_GetFileName;
    g_opts.getRepairFileCnt = REPAIRER_GetFileNameCnt;
    g_opts.argv = (HI_VOID *)lastFileName;
#endif
    s_bParseExtraMdat = pstRepairCfg->bParseExtraMdat;
    return HI_SUCCESS;
}

HI_S32 FILEMNG_REPAIRER_Backup(const HI_CHAR *pszSrcFilePath)
{
    if (g_opts.addRepairFileName == NULL) {
        MLOGE("addRepairFileName is null.\n");
        return HI_FILEMNG_EINTER;
    }
    HI_S32 s32Ret = g_opts.addRepairFileName(pszSrcFilePath, g_opts.argv);
    if (HI_SUCCESS != s32Ret) {
        MLOGE("addRepairFileName failed:%d.\n", s32Ret);
    }
    MLOGD("Add %s to repair list success.\n", pszSrcFilePath);
    return s32Ret;
}

HI_S32 FILEMNG_REPAIRER_Repair(HI_VOID)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR szFilePath[HI_APPCOMM_MAX_PATH_LEN] = { 0 };
    HI_BOOL bRepairFlg = HI_FALSE;
    HI_EVENT_S stEvent = { 0 };
    HI_U32 u32RepairFileListCnt = 0;
    HI_U32 u32FileIndex = 0;
    if (g_opts.getRepairFileCnt == NULL) {
        MLOGE("getRepairFileCnt is null.\n");
        return HI_FILEMNG_EINTER;
    }
    s32Ret = g_opts.getRepairFileCnt(&u32RepairFileListCnt, g_opts.argv);
    if (HI_SUCCESS != s32Ret) {
        MLOGE("getRepairFileCnt failed:%x.\n", s32Ret);
        return s32Ret;
    }
    for (u32FileIndex = 0; u32FileIndex < u32RepairFileListCnt; u32FileIndex++) {
        /**get repair file name */
        memset(szFilePath, 0, HI_APPCOMM_MAX_PATH_LEN);
        if (g_opts.getRepairFileName == NULL) {
            MLOGE("getRepairFileName is null.\n");
            return HI_FILEMNG_EINTER;
        }
        s32Ret = g_opts.getRepairFileName(u32FileIndex, szFilePath, HI_APPCOMM_MAX_PATH_LEN, g_opts.argv);
        if (HI_SUCCESS != s32Ret) {
            MLOGE("getRepairFileName failed:%x.\n", s32Ret);
            return s32Ret;
        }
        if (0 != access(szFilePath, F_OK)) {
            MLOGW("Won't repair. No this file:%s.\n", szFilePath);
            continue;
        }
        /**check whether need to be repaired */
        s32Ret = REPAIRER_CheckMP4(szFilePath);
        if (HI_SUCCESS == s32Ret) {
            MLOGD("%s does not need to be repaired or it has already been repaired\n", szFilePath);
        } else {
            MLOGD("%s needs to be repaired.\n", szFilePath);
            if (HI_FALSE == bRepairFlg) {
                stEvent.EventID = HI_EVENT_FILEMNG_REPAIR_BEGIN;
                HI_EVTHUB_Publish(&stEvent);
                bRepairFlg = HI_TRUE;
            }
            s32Ret = HI_MP4_RepairFile(szFilePath, s_bParseExtraMdat);
            if (s32Ret != HI_SUCCESS) {
                stEvent.EventID = HI_EVENT_FILEMNG_REPAIR_FAILED;
                snprintf(stEvent.aszPayload, EVENT_PAYLOAD_LEN, szFilePath);
                HI_EVTHUB_Publish(&stEvent);
                HI_LOG_PrintFuncErr(HI_MP4_RepairFile, s32Ret);
                continue;
            }
            MLOGI("%s has been repaired successfully.\n", szFilePath);
        }
    }
    if (bRepairFlg) {
        sync();
        stEvent.EventID = HI_EVENT_FILEMNG_REPAIR_END;
        HI_EVTHUB_Publish(&stEvent);
    }
    return HI_SUCCESS;
}

HI_S32 FILEMNG_REPAIRER_Deinit(HI_VOID)
{
    memset(&g_opts, 0, sizeof(HI_FILEMNG_REPAIR_OPERATION_S));
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

