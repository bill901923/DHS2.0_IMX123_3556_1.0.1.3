/**
 * @file    hi_cycle.c
 * @brief   hi_cycle component functions.
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2017/12/28
 * @version   1.0

 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "hi_cycle.h"
#include "hi_appcomm_util.h"
#include "hi_appcomm.h"
#include "hi_common.h"

#ifdef HI_MODULE
#undef HI_MODULE
#endif
#define HI_MODULE "HiCycle"

#define CYCLE_DIVIDE         (3)
#define CYCLE_BYTE_ALIGN     ((HI_U32)16)
#define CYCLE_MAX_PARTITION  (2)
#define CYCLE_MASTER         (0)
#define CYCLE_BACKUP         (1)
#define CYCLE_MAX_CYCLE_SIZE (0x1000000)
#define CYCLE_MAX_INSTANCE   (2)

#define CYCLE_ALIGN_SIZE(u32AlignSize, u32Buffer) (((HI_UL)u32Buffer + (HI_UL)u32AlignSize - 1) & ~((HI_UL)u32AlignSize - 1))

typedef struct tagCYCLE_PARTITION_S {
    HI_HANDLE FlashHdl;
    HI_U8 *pu8MallocBuffer;
    HI_U8 *pu8Buffer;
    HI_U8 *pu8ItemDataAddr;
    HI_U8 *pu8ItemEmptyAddr;
} CYCLE_PARTITION_S;

typedef struct tagCYCLE_INSTANCE_S {
    HI_CYCLE_OPEN_S stOpen;
    CYCLE_PARTITION_S astPTN[CYCLE_MAX_PARTITION];
    HI_BOOL bUsed;
    HI_U32 u32Index;
    HI_U32 u32Magic;
} CYCLE_INSTANCE_S;

typedef struct tagCYCLE_DATA_S {
    HI_U8 *pu8CompressBuffer;
    HI_U32 u32Pagesize;
    HI_U32 u32Blocksize;
    HI_U32 u32TotalSize;
    HI_U32 u32PartSize;
    HI_U32 u32EraseSize;
    HI_U32 u32AlignSize;
    CYCLE_INSTANCE_S astInstance[CYCLE_MAX_INSTANCE];
} CYCLE_DATA_S;

static CYCLE_DATA_S s_stCYCLEData;
static CYCLE_DATA_S *s_pstCYCLEData = HI_NULL;

static HI_CYCLE_INIT_S s_stCYCLEInit;
static HI_CYCLE_INIT_S *s_pstCYCLEInit = HI_NULL;

static pthread_mutex_t s_stCycleMutex = PTHREAD_MUTEX_INITIALIZER;

static HI_S32 CYCLE_GetUnusedInstance(HI_VOID)
{
    HI_S32 i = 0;

    for (i = 0; i < CYCLE_MAX_INSTANCE; i++) {
        if (s_stCYCLEData.astInstance[i].bUsed == HI_FALSE) {
            return i;
        }
    }

    return HI_INVALID_HANDLE;
}

static HI_S32 CYCLE_EraseFlash(HI_U32 u32InstanceIndex, HI_U32 u32PartIndex)
{
    HI_S32 s32Ret = HI_FAILURE;

    HI_APPCOMM_CHECK_EXPR(u32PartIndex < CYCLE_MAX_PARTITION, s32Ret);

    if (s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].FlashHdl < 0) {
        return HI_SUCCESS;
    }

    s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemDataAddr = 0;
    s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemEmptyAddr = 0;

    if (s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8Buffer != HI_NULL
        && s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize > 0
        && s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize < CYCLE_MAX_CYCLE_SIZE) {
        memset(s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8Buffer, 0xff,
               s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize);
    } else {
        return HI_FAILURE;
    }

    if (s_stCYCLEInit.enFlashType == HI_FLASH_TYPE_EMMC_0) {
        HI_U32 u32OffSet = 0;
        HI_U32 u32WriteSize = 0;
        while (u32OffSet < s_pstCYCLEData->u32EraseSize) {
            if ((s_pstCYCLEData->u32EraseSize - u32OffSet) >
                s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize) {
                u32WriteSize = s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize;
            } else {
                u32WriteSize = s_pstCYCLEData->u32EraseSize - u32OffSet;
            }

            s32Ret = HI_Flash_Write(s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].FlashHdl, u32OffSet,
                                    s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8Buffer, u32WriteSize, 0);
            HI_APPCOMM_CHECK_EXPR(s32Ret == s_pstCYCLEData->u32EraseSize, s32Ret);

            u32OffSet += u32WriteSize;
        }
    } else {
        s32Ret = HI_Flash_Erase(s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].FlashHdl, 0,
                                s_pstCYCLEData->u32EraseSize);
        HI_APPCOMM_CHECK_EXPR(s32Ret == s_pstCYCLEData->u32EraseSize, s32Ret);
    }

    return HI_SUCCESS;
}

static HI_S32 CYCLE_InitFlash(HI_U32 u32InstanceIndex, HI_U32 u32PartIndex)
{
    HI_S32 s32Ret = HI_FAILURE;

    if (u32PartIndex >= CYCLE_MAX_PARTITION) {
        return HI_FAILURE;
    }

    if (s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].FlashHdl < 0) {
        return HI_SUCCESS;
    }

    memset(s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8Buffer, 0,
           s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize);
    HI_CYCLE_HEAD_S *pstHead = (HI_CYCLE_HEAD_S *)
                               s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8Buffer;

    pstHead->u32MagicHead = CYCLE_MAGIC_HEAD;
    pstHead->u32Compress = s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32Compress;
    pstHead->u32WriteFlag = HI_CYCLE_WRITE_FLAG_DEFAULT;
    pstHead->u32CycleFlashSize = s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize;
    pstHead->u32AlignSize = s_pstCYCLEData->u32AlignSize;

    s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemEmptyAddr =
        s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8Buffer + sizeof(HI_CYCLE_HEAD_S);
    s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemEmptyAddr = (HI_U8 *)CYCLE_ALIGN_SIZE(s_pstCYCLEData->u32AlignSize, s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemEmptyAddr);

    s32Ret = HI_Flash_Write(s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].FlashHdl, 0,
                            s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8Buffer, sizeof(HI_CYCLE_HEAD_S), 0);
    HI_APPCOMM_CHECK_EXPR(s32Ret == sizeof(HI_CYCLE_HEAD_S), s32Ret);

    return HI_SUCCESS;
}

static HI_U32 CYCLE_GetWriteLen(HI_U32 u32AlignSize, HI_U8 *pu8Data, HI_U32 u32OriginLen, HI_U32 u32Len,
                                HI_U8 **ppu8ItemDataAddr, HI_U8 **ppu8ItemEmptyAddr)
{
    *ppu8ItemDataAddr = *ppu8ItemEmptyAddr;
    HI_U8 *pu8TempData = *ppu8ItemDataAddr;
    HI_CYCLE_ITEM_START_S *pstItem = (HI_CYCLE_ITEM_START_S *)pu8TempData;

    pstItem->u32MagicItemStart = CYCLE_MAGIC_ITEM_START;
    pstItem->u32ItemLen = u32Len;
    pstItem->u32ItemOriginLen = u32OriginLen;

    pu8TempData += sizeof(HI_CYCLE_ITEM_START_S);

    pu8TempData = (HI_U8 *)CYCLE_ALIGN_SIZE(CYCLE_BYTE_ALIGN, pu8TempData);

    if (pu8TempData != HI_NULL
        && pu8Data != HI_NULL
        && u32Len > 0
        && u32Len < CYCLE_MAX_CYCLE_SIZE) {
        memcpy(pu8TempData, pu8Data, u32Len);
    } else {
        return HI_FAILURE;
    }

    pu8TempData += u32Len;
    HI_U32 *pu32End = (HI_U32 *)pu8TempData;
    *pu32End = CYCLE_MAGIC_ITEM_END;

    pu8TempData += sizeof(HI_U32);

    pu8TempData = (HI_U8 *)CYCLE_ALIGN_SIZE(u32AlignSize, pu8TempData);

    *ppu8ItemEmptyAddr = pu8TempData;

    pstItem->u32ItemAllLen = (HI_U32)((HI_UL)pu8TempData - (HI_UL)pstItem);

    return pstItem->u32ItemAllLen;
}

static HI_S32 CYCLE_WriteFlash(HI_U32 u32InstanceIndex, HI_U32 u32PartIndex, HI_U8 *pu8Data, HI_U32 u32OriginLen,
                               HI_U32 u32Len)
{
    HI_S32 s32Ret = HI_FAILURE;

    if (u32PartIndex >= CYCLE_MAX_PARTITION) {
        return HI_FAILURE;
    }

    if (s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].FlashHdl < 0) {
        return HI_SUCCESS;
    }

    HI_U32 u32Temp = u32Len;
    u32Len = (HI_U32)CYCLE_ALIGN_SIZE(CYCLE_BYTE_ALIGN, u32Temp);

    if (s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].FlashHdl >= 0) {
        if (s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemEmptyAddr != 0) {
            HI_U8 *pu8TempData = s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemEmptyAddr;

            pu8TempData += sizeof(HI_CYCLE_ITEM_START_S);

            pu8TempData = (HI_U8 *)CYCLE_ALIGN_SIZE(CYCLE_BYTE_ALIGN, pu8TempData);

            pu8TempData += u32Len + sizeof(HI_U32);

            pu8TempData = (HI_U8 *)CYCLE_ALIGN_SIZE(s_pstCYCLEData->u32AlignSize, pu8TempData);

            if ((pu8TempData - s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8Buffer) >
                s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize) {
                s32Ret = CYCLE_EraseFlash(u32InstanceIndex, u32PartIndex);
                HI_APPCOMM_CHECK_EXPR(s32Ret == HI_SUCCESS, s32Ret);
            }
        }

        if (s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemEmptyAddr == 0) {
            s32Ret = CYCLE_InitFlash(u32InstanceIndex, u32PartIndex);
            HI_APPCOMM_CHECK_EXPR(s32Ret == HI_SUCCESS, s32Ret);
        }

        HI_U32 u32WriteLen = CYCLE_GetWriteLen(s_pstCYCLEData->u32AlignSize, pu8Data, u32OriginLen, u32Len,
                                               &s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemDataAddr,
                                               &s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemEmptyAddr);

        if ((HI_U32)HI_FAILURE == u32WriteLen) {
            goto Write_Error;
        }

        s32Ret = HI_Flash_Write(s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].FlashHdl,
                                s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemDataAddr -
                                s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8Buffer,
                                s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemDataAddr, u32WriteLen, 0);

        if ((HI_U32)s32Ret != u32WriteLen) {
            goto Write_Error;
        }
    }

    return HI_SUCCESS;

Write_Error:
    CYCLE_EraseFlash(u32InstanceIndex, u32PartIndex);

    CYCLE_InitFlash(u32InstanceIndex, u32PartIndex);

    MLOGE("Failed to Write cycle-flash\n");

    return HI_FAILURE;
}

static HI_S32 CYCLE_GetInitFlashData(HI_U8 *pu8Buffer, HI_U32 u32Len,
                                     HI_U8 **ppu8ItemDataAddr,
                                     HI_U8 **ppu8ItemEmptyAddr,
                                     HI_CYCLE_WRITE_FLAG_E *penWriteFlag)
{
    HI_APPCOMM_CHECK_EXPR(u32Len > sizeof(HI_CYCLE_HEAD_S), HI_FAILURE);
    HI_U8 *pcEnd = pu8Buffer + u32Len;
    HI_U32 u32MagicEnd = 0;

    HI_CYCLE_ITEM_START_S *pstItem = HI_NULL;

    HI_CYCLE_HEAD_S *pstHead = (HI_CYCLE_HEAD_S *)pu8Buffer;

    if (pstHead->u32MagicHead != CYCLE_MAGIC_HEAD || pstHead->u32CycleFlashSize == 0 || pstHead->u32AlignSize == 0) {
        MLOGE("data error: cycle Head[%x] cycleFlashSize[0x%x] AlignSize[0x%x]!\n",
              pstHead->u32MagicHead, pstHead->u32CycleFlashSize, pstHead->u32AlignSize);
        return HI_FAILURE;
    }

    *penWriteFlag = (HI_CYCLE_WRITE_FLAG_E)pstHead->u32WriteFlag;

    pu8Buffer += sizeof(HI_CYCLE_HEAD_S);

    pu8Buffer = (HI_U8 *)CYCLE_ALIGN_SIZE(pstHead->u32AlignSize, pu8Buffer);

    pstItem = (HI_CYCLE_ITEM_START_S *)pu8Buffer;

    if (pstItem->u32ItemLen == 0) {
        MLOGE("data error: itemlen=0 !\n");
        return HI_FAILURE;
    }

    u32MagicEnd = *(HI_U32 *)((HI_UL)pu8Buffer + (pstItem->u32ItemLen + sizeof(HI_CYCLE_ITEM_START_S)));

    if ((pstItem->u32MagicItemStart != CYCLE_MAGIC_ITEM_START)
        || (pstItem->u32ItemAllLen >= (u32Len / CYCLE_DIVIDE))
        || (u32MagicEnd != CYCLE_MAGIC_ITEM_END)) {
        MLOGE("data error: ItemStart[%x] ItemAlllen[0x%x] len[0x%x] End[%x]!\n",
              pstItem->u32MagicItemStart, pstItem->u32ItemAllLen, u32Len, u32MagicEnd);
        return HI_FAILURE;
    }

    while (1) {
        *ppu8ItemDataAddr = pu8Buffer;

        pu8Buffer += pstItem->u32ItemAllLen;

        *ppu8ItemEmptyAddr = pu8Buffer;

        pstItem = (HI_CYCLE_ITEM_START_S *)pu8Buffer;

        if (pu8Buffer >= pcEnd) {
            return HI_SUCCESS;
        }

        if (pstItem->u32MagicItemStart != CYCLE_MAGIC_ITEM_START) {
            if (pstItem->u32MagicItemStart == 0xffffffff) {
                return HI_SUCCESS;
            } else {
                MLOGE("data error: ItemStart[%x]!\n", pstItem->u32MagicItemStart);
                return HI_FAILURE;
            }
        }

        u32MagicEnd = *(HI_U32 *)((HI_UL)pu8Buffer + (pstItem->u32ItemLen + sizeof(HI_CYCLE_ITEM_START_S)));

        if ((pstItem->u32ItemLen >= (u32Len / CYCLE_DIVIDE))
            || (u32MagicEnd != CYCLE_MAGIC_ITEM_END)) {
            MLOGE("data error: ItemAlllen[0x%x] len[0x%x] End[%x]!\n",
                  pstItem->u32ItemAllLen, u32Len, u32MagicEnd);
            return HI_FAILURE;
        }
    }

    MLOGE("data error !\n");
    return HI_FAILURE;
}

static HI_S32 CYCLE_Open(HI_U32 u32InstanceIndex)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_S32 s32ArrRet[CYCLE_MAX_PARTITION] = { 0 };
    HI_CYCLE_WRITE_FLAG_E aenArrWriteFlag[CYCLE_MAX_PARTITION] = { 0 };
    HI_CYCLE_ITEM_START_S *pstItem = HI_NULL;
    HI_S32 i = 0;

    for (i = 0; i < CYCLE_MAX_PARTITION; i++) {
        if (s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[i].FlashHdl >= 0) {
            if (((s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize + s_pstCYCLEData->u32AlignSize) > 0)
                && ((s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize + s_pstCYCLEData->u32AlignSize) <
                          CYCLE_MAX_CYCLE_SIZE)) {
                s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[i].pu8MallocBuffer = (HI_U8 *)malloc(s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize + s_pstCYCLEData->u32AlignSize);
                HI_APPCOMM_CHECK_EXPR(s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[i].pu8MallocBuffer != HI_NULL,
                                      HI_FAILURE);
            } else {
                MLOGE("(s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize + s_pstCYCLEData->u32AlignSize) error: 0x%x",
                      (s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize + s_pstCYCLEData->u32AlignSize));
                continue;
            }

            s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[i].pu8Buffer = (HI_U8 *)CYCLE_ALIGN_SIZE(s_pstCYCLEData->u32AlignSize, s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[i].pu8MallocBuffer);

            s32Ret = HI_Flash_Read(s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[i].FlashHdl, 0,
                                   s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[i].pu8Buffer,
                                   s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize, 0);
            HI_APPCOMM_CHECK_EXPR(s32Ret == s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize, s32Ret);

            s32ArrRet[i] = CYCLE_GetInitFlashData(s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[i].pu8Buffer,
                                                  s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize,
                                                  &s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[i].pu8ItemDataAddr,
                                                  &s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[i].pu8ItemEmptyAddr, &aenArrWriteFlag[i]);
        }
    }

    if ((s32ArrRet[CYCLE_MASTER] != HI_SUCCESS)
        || (s32ArrRet[CYCLE_MASTER] != s32ArrRet[CYCLE_BACKUP])
        || (s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[CYCLE_MASTER].pu8ItemEmptyAddr == HI_NULL)
        || (s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[CYCLE_MASTER].pu8ItemEmptyAddr -
                  s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[CYCLE_MASTER].pu8Buffer) !=
        (s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[CYCLE_BACKUP].pu8ItemEmptyAddr -
         s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[CYCLE_BACKUP].pu8Buffer)
        || (aenArrWriteFlag[CYCLE_MASTER] != HI_CYCLE_WRITE_FLAG_DEFAULT)
        || (aenArrWriteFlag[CYCLE_MASTER] != HI_CYCLE_WRITE_FLAG_DEFAULT)) {
        pstItem = HI_NULL;

        for (i = 0; i < CYCLE_MAX_PARTITION; i++) {
            if (s32ArrRet[i] == HI_SUCCESS &&
                s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[i].pu8ItemDataAddr != HI_NULL) {
                pstItem = (HI_CYCLE_ITEM_START_S *)s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[i].pu8ItemDataAddr;
            }
        }

        if (pstItem != HI_NULL) {
            HI_CYCLE_ITEM_START_S *pstTempItem = HI_NULL;
            HI_CYCLE_ITEM_START_S *pstTempItem1 = HI_NULL;

            if (pstItem->u32ItemAllLen > 0 && pstItem->u32ItemAllLen < CYCLE_MAX_CYCLE_SIZE) {
                pstTempItem = (HI_CYCLE_ITEM_START_S *)malloc(pstItem->u32ItemAllLen + CYCLE_BYTE_ALIGN);
                HI_APPCOMM_CHECK_EXPR(pstTempItem != HI_NULL, HI_FAILURE);
            } else {
                return HI_FAILURE;
            }

            pstTempItem1 = (HI_CYCLE_ITEM_START_S *)(HI_U8 *)CYCLE_ALIGN_SIZE(CYCLE_BYTE_ALIGN, pstTempItem);
            memcpy(pstTempItem1, pstItem, pstItem->u32ItemAllLen);

            HI_U8 *pu8Temp = (HI_U8 *)pstTempItem1 + sizeof(HI_CYCLE_ITEM_START_S);

            if (s32ArrRet[CYCLE_BACKUP] != HI_SUCCESS) {
                s32Ret = CYCLE_EraseFlash(u32InstanceIndex, CYCLE_BACKUP);

                s32Ret |= CYCLE_WriteFlash(u32InstanceIndex, CYCLE_BACKUP, pu8Temp, pstTempItem1->u32ItemOriginLen,
                                           pstTempItem1->u32ItemLen);

                s32Ret |= CYCLE_EraseFlash(u32InstanceIndex, CYCLE_MASTER);

                s32Ret |= CYCLE_WriteFlash(u32InstanceIndex, CYCLE_MASTER, pu8Temp, pstTempItem1->u32ItemOriginLen,
                                           pstTempItem1->u32ItemLen);
            } else {
                s32Ret = CYCLE_EraseFlash(u32InstanceIndex, CYCLE_MASTER);

                s32Ret |= CYCLE_WriteFlash(u32InstanceIndex, CYCLE_MASTER, pu8Temp, pstTempItem1->u32ItemOriginLen,
                                           pstTempItem1->u32ItemLen);

                s32Ret |= CYCLE_EraseFlash(u32InstanceIndex, CYCLE_BACKUP);

                s32Ret |= CYCLE_WriteFlash(u32InstanceIndex, CYCLE_BACKUP, pu8Temp, pstTempItem1->u32ItemOriginLen,
                                           pstTempItem1->u32ItemLen);
            }

            HI_APPCOMM_SAFE_FREE(pstTempItem);

            HI_APPCOMM_CHECK_EXPR(s32Ret == HI_SUCCESS, s32Ret);
        } else {
            s32Ret = HI_CYCLE_Clear(&s_pstCYCLEData->astInstance[u32InstanceIndex]);
            HI_APPCOMM_CHECK_EXPR(s32Ret == HI_SUCCESS, s32Ret);
        }
    }

    return HI_SUCCESS;
}

#if defined(__HuaweiLite__)
static HI_S32 CYCLE_Get_ItemData(HI_U32 u32InstanceIndex, HI_U32 u32PartIndex, HI_U8 *pcData, HI_U32 u32Len)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_APPCOMM_CHECK_POINTER(s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemDataAddr,
                             HI_FAILURE);
    HI_CYCLE_ITEM_START_S *pstItem = (HI_CYCLE_ITEM_START_S *)
                                     s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemDataAddr;
    HI_U8 *u8TempAddr = s_pstCYCLEData->astInstance[u32InstanceIndex].astPTN[u32PartIndex].pu8ItemDataAddr + sizeof(HI_CYCLE_ITEM_START_S);
    u8TempAddr = (HI_U8 *)CYCLE_ALIGN_SIZE(CYCLE_BYTE_ALIGN, u8TempAddr);
    HI_U32 u32DecompressLen = s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize;
    if (s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32Compress) {
        HI_MUTEX_LOCK(s_stCycleMutex);

        memset(s_pstCYCLEData->pu8CompressBuffer, 0,
               s_pstCYCLEData->astInstance[u32InstanceIndex].stOpen.u32CycleFlashSize);
        extern HI_S32 HI_Decompress(HI_U8 * pu8zData, HI_U32 nzData, HI_U8 * pu8Data, HI_U32 * pnDataLen);
        s32Ret = HI_Decompress((HI_U8 *)u8TempAddr, (HI_U32)pstItem->u32ItemLen, s_pstCYCLEData->pu8CompressBuffer,
                               &u32DecompressLen);

        if (HI_SUCCESS != s32Ret
            || (HI_U32) u32DecompressLen != u32Len
            || (HI_U32) u32DecompressLen != pstItem->u32ItemOriginLen) {
            MLOGE("decompress err!");
            HI_MUTEX_UNLOCK(s_stCycleMutex);
            return HI_FAILURE;
        }

        memcpy(pcData, s_pstCYCLEData->pu8CompressBuffer, u32Len);

        HI_MUTEX_UNLOCK(s_stCycleMutex);
    } else {
        HI_APPCOMM_CHECK_EXPR(u32Len == pstItem->u32ItemOriginLen, HI_FAILURE);
        memcpy(pcData, u8TempAddr, u32Len);
        s32Ret = HI_SUCCESS;
    }
    return s32Ret;
}
#endif

HI_S32 HI_CYCLE_Init(HI_CYCLE_INIT_S *pstInit)
{
    HI_APPCOMM_CHECK_POINTER(pstInit, HI_FAILURE);

    if (s_pstCYCLEInit != HI_NULL) {
        return HI_SUCCESS;
    }

    if ((pstInit->enFlashType != HI_FLASH_TYPE_SPI_0)
        && (pstInit->enFlashType != HI_FLASH_TYPE_NAND_0)
        && (pstInit->enFlashType != HI_FLASH_TYPE_EMMC_0)) {
        return HI_FAILURE;
    }

    HI_U32 i = 0;

    memset(&s_stCYCLEData, 0, sizeof(s_stCYCLEData));

    for (i = 0; i < CYCLE_MAX_INSTANCE; i++) {
        s_stCYCLEData.astInstance[i].bUsed = HI_FALSE;
        s_stCYCLEData.astInstance[i].u32Index = i;
        s_stCYCLEData.astInstance[i].u32Magic = CYCLE_MAGIC_HEAD + i;
    }

    s_pstCYCLEData = &s_stCYCLEData;

    s_stCYCLEInit = *pstInit;
    s_pstCYCLEInit = &s_stCYCLEInit;

    return HI_SUCCESS;
}

HI_S32 HI_CYCLE_Deinit(HI_VOID)
{
    HI_S32 i = 0;

    if (s_pstCYCLEInit == HI_NULL) {
        return HI_SUCCESS;
    }

    for (i = 0; i < CYCLE_MAX_INSTANCE; i++) {
        if (s_stCYCLEData.astInstance[i].bUsed == HI_TRUE) {
            MLOGE("pls close instance first!");
            return HI_FAILURE;
        }
    }

    if (s_pstCYCLEData->pu8CompressBuffer != HI_NULL) {
        free(s_pstCYCLEData->pu8CompressBuffer);
    }

    memset(&s_stCYCLEData, 0, sizeof(s_stCYCLEData));

    memset(&s_stCYCLEInit, 0, sizeof(s_stCYCLEInit));

    s_pstCYCLEInit = HI_NULL;
    return HI_SUCCESS;
}

HI_S32 HI_CYCLE_Open(HI_VOID **pHandle, HI_CYCLE_OPEN_S *pstOpen)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_APPCOMM_CHECK_POINTER(s_pstCYCLEInit, HI_FAILURE);
    HI_APPCOMM_CHECK_POINTER(pstOpen, HI_FAILURE);
#ifndef __HuaweiLite__
    HI_APPCOMM_CHECK_EXPR(((pstOpen->u32CycleFlashSize > 0) && (pstOpen->pcPartition != HI_NULL)), HI_FAILURE);
#else
    HI_APPCOMM_CHECK_EXPR(((pstOpen->u32CycleFlashSize > 0) && (pstOpen->u64Addr > 0L)), HI_FAILURE);
#endif

    HI_S32 s32Index = CYCLE_GetUnusedInstance();

    if (s32Index == HI_INVALID_HANDLE) {
        MLOGE("Only support %d instance\n", CYCLE_MAX_INSTANCE);
        return HI_FAILURE;
    }

    HI_CYCLE_OPEN_S *pstCurOpen = &s_pstCYCLEData->astInstance[s32Index].stOpen;

    memcpy(pstCurOpen, pstOpen, sizeof(HI_CYCLE_OPEN_S));

    if ((pstCurOpen->u32CycleFlashSize > 0) && (pstCurOpen->u32CycleFlashSize < CYCLE_MAX_CYCLE_SIZE)) {
        if (s_pstCYCLEData->pu8CompressBuffer == HI_NULL) {
            s_pstCYCLEData->pu8CompressBuffer = (HI_U8 *)malloc(pstCurOpen->u32CycleFlashSize);
            HI_APPCOMM_CHECK_POINTER(s_pstCYCLEData->pu8CompressBuffer, HI_FAILURE);
        }
    } else {
        MLOGE("pstCurOpen->u32CycleFlashSize error: 0x%x!", pstCurOpen->u32CycleFlashSize);
        return HI_FAILURE;
    }
#ifndef __HuaweiLite__
    s_pstCYCLEData->astInstance[s32Index].astPTN[CYCLE_MASTER].FlashHdl = HI_Flash_OpenByTypeAndName(s_pstCYCLEInit->enFlashType, pstCurOpen->pcPartition);
#else
    s_pstCYCLEData->astInstance[s32Index].astPTN[CYCLE_MASTER].FlashHdl = HI_Flash_OpenByTypeAndAddr(s_pstCYCLEInit->enFlashType, pstCurOpen->u64Addr, pstCurOpen->u32CycleFlashSize);
#endif
    HI_APPCOMM_CHECK_EXPR(s_pstCYCLEData->astInstance[s32Index].astPTN[CYCLE_MASTER].FlashHdl != HI_FAILURE,
                          HI_FAILURE);

#ifndef __HuaweiLite__
    if (pstOpen->pcPartitionBackup != HI_NULL) {
        s_pstCYCLEData->astInstance[s32Index].astPTN[CYCLE_BACKUP].FlashHdl = HI_Flash_OpenByTypeAndName(s_pstCYCLEInit->enFlashType, pstCurOpen->pcPartitionBackup);

        if (s_pstCYCLEData->astInstance[s32Index].astPTN[CYCLE_BACKUP].FlashHdl < 0) {
            MLOGE("cannot open pstCurOpen->pcParamBackup_MtdName: %s", pstCurOpen->pcPartitionBackup);
            s_pstCYCLEData->astInstance[s32Index].astPTN[CYCLE_BACKUP].FlashHdl = HI_FAILURE;
        }
    }
#else
    if (pstOpen->u64BackupAddr > 0) {
        s_pstCYCLEData->astInstance[s32Index].astPTN[CYCLE_BACKUP].FlashHdl = HI_Flash_OpenByTypeAndAddr(s_pstCYCLEInit->enFlashType, pstCurOpen->u64BackupAddr, pstCurOpen->u32CycleFlashSize);

        if (s_pstCYCLEData->astInstance[s32Index].astPTN[CYCLE_BACKUP].FlashHdl < 0) {
            MLOGE("cannot open pstCurOpen->u64BackupAddr: 0x%08llx", pstCurOpen->u64BackupAddr);
            s_pstCYCLEData->astInstance[s32Index].astPTN[CYCLE_BACKUP].FlashHdl = HI_FAILURE;
        }
    }
#endif
    else {
        s_pstCYCLEData->astInstance[s32Index].astPTN[CYCLE_BACKUP].FlashHdl = HI_FAILURE;
    }

    if (s_pstCYCLEData->u32Pagesize == 0) {
        HI_Flash_InterInfo_S stInfo;
        memset(&stInfo, 0, sizeof(stInfo));

        s32Ret = HI_Flash_GetInfo(s_pstCYCLEData->astInstance[s32Index].astPTN[CYCLE_MASTER].FlashHdl, &stInfo);
        HI_APPCOMM_CHECK_EXPR(s32Ret == HI_SUCCESS, s32Ret);

        s_pstCYCLEData->u32Blocksize = stInfo.BlockSize;
        s_pstCYCLEData->u32Pagesize = stInfo.PageSize;

        if (s_pstCYCLEData->u32Pagesize < CYCLE_BYTE_ALIGN) {
            s_pstCYCLEData->u32AlignSize = CYCLE_BYTE_ALIGN;
        } else {
            s_pstCYCLEData->u32AlignSize = s_pstCYCLEData->u32Pagesize;
        }

        s_pstCYCLEData->u32TotalSize = stInfo.TotalSize;
        s_pstCYCLEData->u32PartSize = stInfo.PartSize;
    }

    if (pstOpen->u32CycleFlashSize < s_pstCYCLEData->u32Blocksize) {
        s_pstCYCLEData->u32EraseSize = s_pstCYCLEData->u32Blocksize;
    } else {
        s_pstCYCLEData->u32EraseSize = (HI_U32)CYCLE_ALIGN_SIZE(s_pstCYCLEData->u32Blocksize, pstOpen->u32CycleFlashSize);
    }

    if (pstOpen->u32CycleFlashSize > s_pstCYCLEData->u32PartSize) {
        HI_CYCLE_Close(&s_pstCYCLEData->astInstance[s32Index]);

        return HI_FAILURE;
    }

    s32Ret = CYCLE_Open(s32Index);

    if (s32Ret != HI_SUCCESS) {
        HI_CYCLE_Close(&s_pstCYCLEData->astInstance[s32Index]);

        return s32Ret;
    } else {
        *pHandle = &s_pstCYCLEData->astInstance[s32Index];
    }

    s_pstCYCLEData->astInstance[s32Index].bUsed = HI_TRUE;

    return HI_SUCCESS;
}

HI_S32 HI_CYCLE_Close(HI_VOID *Handle)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 i = 0;

    HI_APPCOMM_CHECK_EXPR(((HI_NULL != s_pstCYCLEData) && (Handle != HI_NULL)), HI_FAILURE);
    HI_APPCOMM_CHECK_POINTER(s_pstCYCLEInit, HI_FAILURE);

    CYCLE_INSTANCE_S *pstInstance = (CYCLE_INSTANCE_S *)Handle;

    if (pstInstance->u32Magic != (pstInstance->u32Index + CYCLE_MAGIC_HEAD)) {
        MLOGE("HANDLE is wrong!!! 0x%x", (HI_U32)Handle);
        return HI_FAILURE;
    }

    HI_APPCOMM_SAFE_FREE(s_pstCYCLEData->pu8CompressBuffer);

    for (i = 0; i < CYCLE_MAX_PARTITION; i++) {
        if (-1 != s_pstCYCLEData->astInstance[pstInstance->u32Index].astPTN[i].FlashHdl) {
            s32Ret = HI_Flash_Close(s_pstCYCLEData->astInstance[pstInstance->u32Index].astPTN[i].FlashHdl);

            if (HI_SUCCESS != s32Ret) {
                MLOGE("Cannot close flash handle: [i:%d] [flash hande: 0x%x]\n", i,
                      s_pstCYCLEData->astInstance[pstInstance->u32Index].astPTN[i].FlashHdl);
            }

            s_pstCYCLEData->astInstance[pstInstance->u32Index].astPTN[i].FlashHdl = -1;
        }

        if (HI_NULL != s_pstCYCLEData->astInstance[pstInstance->u32Index].astPTN[i].pu8MallocBuffer) {
            free(s_pstCYCLEData->astInstance[pstInstance->u32Index].astPTN[i].pu8MallocBuffer);
            s_pstCYCLEData->astInstance[pstInstance->u32Index].astPTN[i].pu8MallocBuffer = HI_NULL;
            s_pstCYCLEData->astInstance[pstInstance->u32Index].astPTN[i].pu8Buffer = HI_NULL;
        }
    }

    memset(&s_pstCYCLEData->astInstance[pstInstance->u32Index], 0, sizeof(CYCLE_INSTANCE_S));

    return HI_SUCCESS;
}

HI_S32 HI_CYCLE_Write(HI_VOID *Handle, HI_U8 *pu8Data, HI_U32 u32Len)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_APPCOMM_CHECK_POINTER(s_pstCYCLEInit, HI_FAILURE);
    HI_APPCOMM_CHECK_POINTER(pu8Data, HI_FAILURE);
    HI_APPCOMM_CHECK_EXPR(u32Len != 0, HI_FAILURE);
    HI_APPCOMM_CHECK_POINTER(Handle, HI_FAILURE);

    CYCLE_INSTANCE_S *pstInstance = (CYCLE_INSTANCE_S *)Handle;

    if (pstInstance->u32Magic != (pstInstance->u32Index + CYCLE_MAGIC_HEAD)) {
        MLOGE("HANDLE is wrong!!! 0x%x", (HI_U32)Handle);
        return HI_FAILURE;
    }

    HI_U32 u32ZLen = u32Len;

    HI_MUTEX_LOCK(s_stCycleMutex);

    if (pstInstance->stOpen.u32Compress) {
        memset(s_pstCYCLEData->pu8CompressBuffer, 0, pstInstance->stOpen.u32CycleFlashSize);
        extern HI_S32 HI_Compress(HI_U8 * pu8Data, HI_U32 u32DataLen, HI_U8 * pu8ZData, HI_U32 * pu32ZDataLen);
        s32Ret = HI_Compress((HI_U8 *)pu8Data, u32Len, s_pstCYCLEData->pu8CompressBuffer, &u32ZLen);

        if (HI_SUCCESS != s32Ret) {
            MLOGE("compress err!");
            HI_MUTEX_UNLOCK(s_stCycleMutex);
            return HI_FAILURE;
        }

        pu8Data = s_pstCYCLEData->pu8CompressBuffer;
    }

    if (u32ZLen > (pstInstance->stOpen.u32CycleFlashSize / CYCLE_DIVIDE)) {
        MLOGE("pstInstance->stOpen.u32CycleFlashSize is too short!!!");
        HI_MUTEX_UNLOCK(s_stCycleMutex);
        return HI_FAILURE;
    }

    s32Ret = CYCLE_WriteFlash(pstInstance->u32Index, CYCLE_MASTER, pu8Data, u32Len, u32ZLen);
    s32Ret |= CYCLE_WriteFlash(pstInstance->u32Index, CYCLE_BACKUP, pu8Data, u32Len, u32ZLen);

    HI_MUTEX_UNLOCK(s_stCycleMutex);
    return s32Ret;
}

HI_S32 HI_CYCLE_Read(HI_VOID *Handle, HI_U8 *pu8Data, HI_U32 u32Len)
{
    HI_S32 s32Ret = HI_FAILURE;

#if defined(__HuaweiLite__)
    HI_APPCOMM_CHECK_POINTER(s_pstCYCLEInit, HI_FAILURE);
    HI_APPCOMM_CHECK_POINTER(pu8Data, HI_FAILURE);
    HI_APPCOMM_CHECK_EXPR(u32Len != 0, HI_FAILURE);
    HI_APPCOMM_CHECK_POINTER(Handle, HI_FAILURE);

    CYCLE_INSTANCE_S *pstInstance = (CYCLE_INSTANCE_S *)Handle;

    if (pstInstance->u32Magic != (pstInstance->u32Index + CYCLE_MAGIC_HEAD)) {
        MLOGE("HANDLE is wrong!!! 0x%x", (HI_U32)Handle);
        return HI_FAILURE;
    }

    s32Ret = CYCLE_Get_ItemData(pstInstance->u32Index, CYCLE_MASTER, pu8Data, u32Len);
    if (HI_FAILURE == s32Ret) {
        s32Ret = CYCLE_Get_ItemData(pstInstance->u32Index, CYCLE_BACKUP, pu8Data, u32Len);
    }
#else
    MLOGW("cread param in u-boot.\n");
#endif
    return s32Ret;
}

HI_S32 HI_CYCLE_Clear(HI_VOID *Handle)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_APPCOMM_CHECK_POINTER(s_pstCYCLEInit, HI_FAILURE);

    CYCLE_INSTANCE_S *pstInstance = (CYCLE_INSTANCE_S *)Handle;

    if (pstInstance->u32Magic != (pstInstance->u32Index + CYCLE_MAGIC_HEAD)) {
        MLOGE("HANDLE is wrong!!! 0x%x", (HI_U32)Handle);
        return HI_FAILURE;
    }

    s32Ret = CYCLE_EraseFlash(pstInstance->u32Index, CYCLE_MASTER);

    if (s_pstCYCLEData->astInstance[pstInstance->u32Index].astPTN[CYCLE_BACKUP].FlashHdl >= 0) {
        s32Ret = CYCLE_EraseFlash(pstInstance->u32Index, CYCLE_BACKUP);
    }

    return s32Ret;
}


