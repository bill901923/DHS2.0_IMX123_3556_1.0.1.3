/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    mapi_sys.c
 * @brief   NDK sys server functions
 * @author  HiMobileCam NDK develop team
 * @date  2018-2-6
 */

#include "hi_type.h"
#include "hi_comm_vb.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "hi_mapi_sys.h"
#include "hi_mapi_comm_define.h"
#include "hi_mapi_log.h"
#include "mapi_vcap_inner.h"
#include "mapi_vproc_inner.h"
#include "mapi_venc_inner.h"
#include "mapi_disp_inner.h"
#include "mapi_acap_inner.h"
#include "mapi_aenc_inner.h"
#include "mapi_ao_inner.h"
#include "mapi_comm_inner.h"
#include "mapi_sys_arch.h"
#include "mapi_sys_os.h"
#include "mapi_acap_arch.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CHECK_MAPI_SYS_NULL_PTR_RET(paraname, ptr)                                 \
    do {                                                                       \
        if ((ptr) == NULL) {                                                   \
            MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "%s is NULL pointer\n", paraname); \
            return HI_MAPI_SYS_ENULL_PTR;                                      \
        }                                                                      \
    } while (0)

static HI_BOOL g_s32MediaInit = HI_FALSE;

// VQEModule Register state
static HI_BOOL g_s32RegisterVQEModule = HI_FALSE;

static MAPI_RgnHdlInfo_S *g_apstRgnHdlInfo[RGN_HANDLE_MAX] = { HI_NULL };
pthread_mutex_t g_rgnLock;

HI_S32 MAPI_AddRgnHdl(HI_HANDLE GrpHdl, HI_HANDLE ChnHdl, HI_HANDLE OsdHdl, MOD_ID_E enModId)
{
    HI_U32 i;
    MAPI_RgnHdlInfo_S *pstRgnHdlInfo;

    MUTEX_LOCK(g_rgnLock);

    for (i = 0; i < RGN_HANDLE_MAX; i++) {
        if (g_apstRgnHdlInfo[i] == HI_NULL) {
            pstRgnHdlInfo = (MAPI_RgnHdlInfo_S *)malloc(sizeof(MAPI_RgnHdlInfo_S));

            if (pstRgnHdlInfo == HI_NULL) {
                MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_AddRgnHdl malloc failed.\n");
                MUTEX_UNLOCK(g_rgnLock);
                return HI_MAPI_SYS_ENOMEM;
            }

            pstRgnHdlInfo->GrpHdl = GrpHdl;
            pstRgnHdlInfo->ChnHdl = ChnHdl;
            pstRgnHdlInfo->OsdHdl = OsdHdl;
            pstRgnHdlInfo->enModId = enModId;
            g_apstRgnHdlInfo[i] = pstRgnHdlInfo;

            MUTEX_UNLOCK(g_rgnLock);
            return HI_SUCCESS;
        }
    }

    MUTEX_UNLOCK(g_rgnLock);
    return HI_FAILURE;
}

HI_U32 MAPI_GetRgnHdl(HI_HANDLE GrpHdl, HI_HANDLE ChnHdl, HI_HANDLE OsdHdl, MOD_ID_E enModId)
{
    HI_U32 i;

    MUTEX_LOCK(g_rgnLock);

    for (i = 0; i < RGN_HANDLE_MAX; i++) {
        if (g_apstRgnHdlInfo[i] != HI_NULL) {
            if ((g_apstRgnHdlInfo[i]->GrpHdl == GrpHdl) && (g_apstRgnHdlInfo[i]->ChnHdl == ChnHdl) &&
                (g_apstRgnHdlInfo[i]->OsdHdl == OsdHdl) && (g_apstRgnHdlInfo[i]->enModId == enModId)) {
                MUTEX_UNLOCK(g_rgnLock);
                return i;
            }
        }
    }

    MUTEX_UNLOCK(g_rgnLock);
    return HI_FAILURE;
}

HI_S32 MAPI_DelRgnHdl(HI_HANDLE GrpHdl, HI_HANDLE ChnHdl, HI_HANDLE OsdHdl, MOD_ID_E enModId)
{
    HI_U32 i;

    MUTEX_LOCK(g_rgnLock);

    for (i = 0; i < RGN_HANDLE_MAX; i++) {
        if (g_apstRgnHdlInfo[i] != HI_NULL) {
            if ((g_apstRgnHdlInfo[i]->GrpHdl == GrpHdl) && (g_apstRgnHdlInfo[i]->ChnHdl == ChnHdl) &&
                (g_apstRgnHdlInfo[i]->OsdHdl == OsdHdl) && (g_apstRgnHdlInfo[i]->enModId == enModId)) {
                free(g_apstRgnHdlInfo[i]);
                g_apstRgnHdlInfo[i] = HI_NULL;
                MUTEX_UNLOCK(g_rgnLock);
                return HI_SUCCESS;
            }
        }
    }

    MUTEX_UNLOCK(g_rgnLock);
    return HI_FAILURE;
}

static HI_S32 MAPI_MEDIA_SYS_Init(const HI_MAPI_MEDIA_CONFIG_S *pstMediaConfig)
{
    HI_S32 s32Ret;
    MPP_SYS_CONFIG_S stSysConf;
    VB_SUPPLEMENT_CONFIG_S stSupplementConf;

    (HI_VOID)HI_MPI_SYS_Exit();
    (HI_VOID)HI_MPI_VB_Exit();

    s32Ret = HI_MPI_VB_SetConfig(&pstMediaConfig->stVbConfig);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HI_MPI_VB_SetConf failed:%x \n", s32Ret);
        return s32Ret;
    }

    // This place will open DCF by default
    s32Ret = memset_s(&stSupplementConf, sizeof(VB_SUPPLEMENT_CONFIG_S), 0, sizeof(VB_SUPPLEMENT_CONFIG_S));
    MAPI_CHECK_RET(HI_MAPI_MOD_SYS, "memset_s", s32Ret);

    HAL_MAPI_VB_SetSupplementConfig(&stSupplementConf);

    s32Ret = HI_MPI_VB_SetSupplementConfig(&stSupplementConf);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HI_MPI_VB_SetSupplementConf failed:%x \n", s32Ret);
        return s32Ret;
    }

    s32Ret = HI_MPI_VB_Init();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HI_MPI_VB_Init failed:%x \n", s32Ret);
        return s32Ret;
    }

    s32Ret = memset_s(&stSysConf, sizeof(stSysConf), 0, sizeof(stSysConf));
    MAPI_CHECK_RET(HI_MAPI_MOD_SYS, "memset_s", s32Ret);
    stSysConf.u32Align = 16;
    s32Ret = HI_MPI_SYS_SetConfig(&stSysConf);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HI_MPI_SYS_SetConf failed:%x \n", s32Ret);
        return s32Ret;
    }

    s32Ret = HI_MPI_SYS_Init();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HI_MPI_SYS_Init failed:%x \n", s32Ret);
        return s32Ret;
    }

    VI_VPSS_MODE_S stVIVPSSMode;
    s32Ret = memcpy_s(&stVIVPSSMode, sizeof(VI_VPSS_MODE_S), &pstMediaConfig->stVIVPSSMode, sizeof(VI_VPSS_MODE_S));
    MAPI_CHECK_RET(HI_MAPI_MOD_SYS, "memcpy_s", s32Ret);
    s32Ret = HI_MPI_SYS_SetVIVPSSMode(&stVIVPSSMode);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HI_MPI_SYS_SetVIVPSSMode failed:%x \n", s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

static HI_S32 MAPI_MEDIA_SYS_Deinit(HI_VOID)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_SYS_Exit();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HI_MPI_SYS_Exit failed:%x \n", s32Ret);
    }

    s32Ret = HI_MPI_VB_Exit();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HI_MPI_VB_Exit failed:%x \n", s32Ret);
    }

    return HI_SUCCESS;
}

HI_S32 HI_MAPI_Sys_Init(HI_VOID)
{
    HI_S32 s32Ret;

    s32Ret = HAL_MAPI_SYS_Init();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HAL_MAPI_SYS_Init fail,ret:%x\n", s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_MAPI_Sys_Deinit(HI_VOID)
{
    HI_S32 s32Ret;

    s32Ret = HAL_MAPI_SYS_Deinit();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HAL_MAPI_SYS_Deinit fail,ret:%x\n", s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

HI_S32 HI_MAPI_Sys_GetVprocVencWrapBufferLine(VPSS_VENC_WRAP_PARAM_S *pWrapParam, HI_U32 *pu32BufLine)
{
    // the function be support in the future 56a and 59a version
#if defined(__HI3518EV300__) || defined(__HI3559V200__)

    HI_U32 s32Ret;

    if (pWrapParam == NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "pstDspAttr Null pointer!\n");
        return HI_MAPI_SYS_ENULL_PTR;
    }
    if (pu32BufLine == NULL) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "pstDspAttr Null pointer!\n");
        return HI_MAPI_SYS_ENULL_PTR;
    }

    s32Ret = HI_MPI_SYS_GetVPSSVENCWrapBufferLine(pWrapParam, pu32BufLine);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HI_MPI_SYS_GetVPSSVENCWrapBufferLine failed with %#x!\n", s32Ret);
        return s32Ret;
    }
#else
    MAPI_UNUSED(pWrapParam);
    MAPI_UNUSED(pu32BufLine);

    MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MPI not support GetVprocVencWrapBufferLine!!\n");
    return HI_MAPI_SYS_ENOT_PERM;
#endif

    return HI_SUCCESS;
}

HI_S32 HI_MAPI_Sys_EnableDsp(const HI_MAPI_Sys_DspAttr_S *pstDspAttr)
{
    return HAL_MAPI_SYS_EnableDsp(pstDspAttr);
}

HI_S32 HI_MAPI_Sys_DisableDsp(const HI_MAPI_Sys_DspAttr_S *pstDspAttr)
{
    return HAL_MAPI_SYS_DisableDsp(pstDspAttr);
}

HI_S32 HI_MAPI_Media_Init(const HI_MAPI_MEDIA_ATTR_S *pstMediaAttr)
{
    HI_S32 s32Ret;
    CHECK_MAPI_SYS_NULL_PTR_RET("pstMediaAttr", pstMediaAttr);

    if (g_s32MediaInit == HI_TRUE) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "media has already been inited\n");
        return HI_SUCCESS;
    }

    // system start only need Register VQE once
    if (g_s32RegisterVQEModule == HI_FALSE) {
        s32Ret = HAL_MAPI_ACAP_RegisterVQEModule();

        if (s32Ret != HI_SUCCESS) {
            MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HAL_MAPI_ACAP_RegisterVQEModule fail,ret:%x\n", s32Ret);
            return s32Ret;
        }
        g_s32RegisterVQEModule = HI_TRUE;
    }

    s32Ret = MAPI_MEDIA_SYS_Init(&pstMediaAttr->stMediaConfig);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_MEDIA_SYS_Init fail,ret:%x\n", s32Ret);
        return s32Ret;
    }

    s32Ret = MAPI_VCAP_Init();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_VCap_Init fail,ret:%x\n", s32Ret);
        return s32Ret;
    }

    s32Ret = MAPI_ISPModParam_Init(&pstMediaAttr->stIspModPara);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_ISPModParam_Init fail,ret:%x\n", s32Ret);
        return s32Ret;
    }

    s32Ret = MAPI_VPROC_Init();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_VProc_Init fail,ret:%x\n", s32Ret);
        return s32Ret;
    }

    s32Ret = MAPI_DISP_Init();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_Disp_Init fail,ret:%x\n", s32Ret);
        return s32Ret;
    }

    // add other init fun here
    s32Ret = MAPI_ACAP_Init();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_ACap_Init fail,ret:%x\n", s32Ret);
        return s32Ret;
    }

    s32Ret = MAPI_AENC_Init();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_AEnc_Init err, s32Ret:%x\n", s32Ret);
        return s32Ret;
    }

    s32Ret = MAPI_AO_Init();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_AO_Init err, s32Ret:%x\n", s32Ret);
        return s32Ret;
    }

    s32Ret = MAPI_VENC_Init(&pstMediaAttr->stVencModPara);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_VEnc_Init err, s32Ret:%x\n", s32Ret);
        return s32Ret;
    }

    g_s32MediaInit = HI_TRUE;
    pthread_mutex_init(&g_rgnLock, NULL);

    return HI_SUCCESS;
}

HI_S32 HI_MAPI_Media_Deinit()
{
    HI_S32 s32Ret;

    if (g_s32MediaInit == HI_FALSE) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "Media has already been deinited\n");
        return HI_SUCCESS;
    }

    // add other deinit fun here
    s32Ret = MAPI_VENC_Deinit();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_VEnc_Deinit err, s32Ret:%x\n", s32Ret);
    }

    s32Ret = MAPI_AENC_Deinit();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_AEnc_Deinit err, s32Ret:%x\n", s32Ret);
    }

    s32Ret = MAPI_ACAP_Deinit();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_ACap_Deinit fail,ret:%x\n", s32Ret);
    }

    s32Ret = MAPI_AO_Deinit();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_AO_Deinit fail,ret:%x\n", s32Ret);
    }

    s32Ret = MAPI_VPROC_Deinit();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_VProc_Deinit fail,ret:%x\n", s32Ret);
    }

    s32Ret = MAPI_DISP_Deinit();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_Disp_Deinit fail,ret:%x\n", s32Ret);
    }

    s32Ret = MAPI_VCAP_Deinit();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_VCap_Deinit fail,ret:%x\n", s32Ret);
    }

    s32Ret = MAPI_MEDIA_SYS_Deinit();

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "MAPI_MEDIA_SYS_Deinit fail,ret:%x\n", s32Ret);
    }

    g_s32MediaInit = HI_FALSE;
    pthread_mutex_destroy(&g_rgnLock);

    return HI_SUCCESS;
}

HI_S32 HI_MAPI_Sys_SetDatafifoNodeNum(HI_MAPI_DATAFIFO_CONFIG_S *pstDatafifoCfg)
{
    MAPI_UNUSED(pstDatafifoCfg);

    MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "server don't support SetDatafifoNodeNum!!\n");

    return HI_MAPI_SYS_ENOT_PERM;
}

HI_S32 HI_MAPI_AllocBuffer(HI_U64 *pu64PhyAddr, HI_VOID **ppVirtAddr, HI_U32 u32Len, const HI_CHAR *pstrName)
{
    HI_S32 s32Ret;
    HI_U64 u64PhyBuffer;
    HI_U8 *pu8VirBuffer;
    CHECK_MAPI_SYS_NULL_PTR_RET("pu64PhyAddr", pu64PhyAddr);
    CHECK_MAPI_SYS_NULL_PTR_RET("ppVirtAddr", ppVirtAddr);
    CHECK_MAPI_SYS_NULL_PTR_RET("pstrName", pstrName);

    if (g_s32MediaInit == HI_FALSE) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "media not init yet\n");
        return HI_MAPI_SYS_ENOTREADY;
    }

    if (u32Len <= 0) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "Buffer len can not be less than zero\n");
        return HI_MAPI_SYS_EILLEGAL_PARAM;
    }

    s32Ret = HI_MPI_SYS_MmzAlloc_Cached(&u64PhyBuffer, (HI_VOID **)&pu8VirBuffer, pstrName, "anonymous", u32Len);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HI_MPI_SYS_MmzAlloc_Cached fail,ret:%x\n", s32Ret);
        return s32Ret;
    }

    *pu64PhyAddr = u64PhyBuffer;
    *ppVirtAddr = pu8VirBuffer;
    return HI_SUCCESS;
}

HI_S32 HI_MAPI_FreeBuffer(HI_U64 u64PhyAddr, HI_VOID *pVirtAddr, HI_U32 u32Len)
{
    HI_S32 s32Ret;
    CHECK_MAPI_SYS_NULL_PTR_RET("pVirtAddr", pVirtAddr);
    MAPI_UNUSED(u32Len);

    if (g_s32MediaInit == HI_FALSE) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "media not init yet\n");
        return HI_MAPI_SYS_ENOTREADY;
    }

    s32Ret = HI_MPI_SYS_MmzFree(u64PhyAddr, pVirtAddr);

    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_SYS, "HI_MPI_SYS_MmzFree fail\n");
        return s32Ret;
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
