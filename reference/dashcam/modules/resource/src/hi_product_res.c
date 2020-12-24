/**
 * @file      hi_product_res.c
 * @brief     resource api
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/6/10
 * @version   1.0

 */
#include "hi_product_res.h"
#include "hi_appcomm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/** rename module name */
#ifdef HI_MODULE
#undef HI_MODULE
#endif
#define HI_MODULE "RES"

#define PDT_RES_MAX_CNT     (5)

/** res magic start macro */
#define PDT_RES_MAGIC_START (0x72657373)
/** res magic end macro */
#define PDT_RES_MAGIC_END   (0x72657364)

#define BIG_LITTLE_ENDIAN_TRANSFER(x) (((x&0xff)<<24)|((x&0xff00)<<8)|((x&0xff0000)>>8)|((x&0xff000000)>>24))

/** res info */
typedef struct {
    HI_PDT_RES_Purpose purpose;
    HI_U32 size;    /*unit:Byte*/
    HI_U32 offset;
} PDT_RES_Info;

/** res head info */
typedef struct {
    HI_U32          magicStart; /**<res head start */
    PDT_RES_Info    res[PDT_RES_MAX_CNT];
    HI_U32          magicEnd;   /**<res head end */
} PDT_RES_Head;

#if defined (CFG_MEM_RES_BASE)
static PDT_RES_Head *g_res = (PDT_RES_Head *)CFG_MEM_RES_BASE;
#endif

HI_S32 HI_PDT_RES_Load(HI_VOID)
{
#if defined (CFG_MEM_RES_BASE)
    HI_BOOL needTransfer = HI_FALSE;
    MLOGD("magicStart:0x%x,magicEnd:0x%x\n", g_res->magicStart, g_res->magicEnd);
    if (g_res->magicStart == PDT_RES_MAGIC_START && g_res->magicEnd == PDT_RES_MAGIC_END) {
        MLOGD("ResHead is Little Endian,no need to transfer\n");
    } else if (BIG_LITTLE_ENDIAN_TRANSFER(g_res->magicStart) == PDT_RES_MAGIC_START &&
               BIG_LITTLE_ENDIAN_TRANSFER(g_res->magicEnd) == PDT_RES_MAGIC_END) {
        MLOGD("ResHead is Big Endian,need to transfer\n");
        needTransfer = HI_TRUE;
    } else {
        g_res = NULL;
        MLOGE("Res load failed!!! Please check the res data\n");
        return HI_FAILURE;
    }
    HI_S32 i = 0;
    for (i = 0; i < PDT_RES_MAX_CNT; i++) {
        if (needTransfer) {
            g_res->res[i].purpose = BIG_LITTLE_ENDIAN_TRANSFER(g_res->res[i].purpose);
            g_res->res[i].size = BIG_LITTLE_ENDIAN_TRANSFER(g_res->res[i].size);
            g_res->res[i].offset = BIG_LITTLE_ENDIAN_TRANSFER(g_res->res[i].offset);
        }
        MLOGD("res[%d]:purpose=%x,size=%x,offset=%x\n", i, g_res->res[i].purpose, g_res->res[i].size, g_res->res[i].offset);
    }
    return HI_SUCCESS;
#else
    MLOGW("CFG_MEM_RES_BASE is undefined.\n");
    return HI_FAILURE;
#endif
}

HI_S32 HI_PDT_RES_GetData(HI_PDT_RES_Purpose purpose, HI_U8 **memAddr, HI_U32 *dataLen)
{
#if defined (CFG_MEM_RES_BASE)
    HI_S32 i = 0;
    if (g_res == NULL) {
        MLOGE("Res not Load.\n");
        return HI_FAILURE;
    }
    for (i = 0; i < PDT_RES_MAX_CNT; i++) {
        if (g_res->res[i].purpose == purpose && g_res->res[i].size > 0 && g_res->res[i].offset > 0) {
#if defined (CFG_MEM_RES_SIZE)
            if ((g_res->res[i].size + sizeof(PDT_RES_Head)) > CFG_MEM_RES_SIZE ||
                    g_res->res[i].offset < sizeof(PDT_RES_Head) ||
                    g_res->res[i].offset > CFG_MEM_RES_SIZE  ||
                    (g_res->res[i].offset + g_res->res[i].size) > CFG_MEM_RES_SIZE) {
                MLOGE("Res[%d]purpose=%d (0x%08X, 0x%08X) beyond legal address (0x%08X, 0x%08X)\n",
                      i, g_res->res[i].purpose,
                      (CFG_MEM_RES_BASE + g_res->res[i].offset),
                      (CFG_MEM_RES_BASE + g_res->res[i].offset + g_res->res[i].size),
                      CFG_MEM_RES_BASE, (CFG_MEM_RES_BASE + CFG_MEM_RES_SIZE));
                return HI_FAILURE;
            }
#endif
            if (memAddr != NULL) {
                *memAddr = (HI_U8 *)CFG_MEM_RES_BASE + g_res->res[i].offset;
            }
            if (dataLen != NULL) {
                *dataLen = g_res->res[i].size;
            }
            return HI_SUCCESS;
        }
    }
    MLOGE("Res not found,purpose=%d\n", purpose);
#else
    MLOGW("CFG_MEM_RES_BASE is undefined.\n");
#endif
    return HI_FAILURE;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

