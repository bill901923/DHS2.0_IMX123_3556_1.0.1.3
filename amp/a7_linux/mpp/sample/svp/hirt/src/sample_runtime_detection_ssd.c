#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hi_runtime_api.h"
#include "sample_memory_ops.h"
#include "sample_log.h"
#include "sample_runtime_define.h"
#ifdef ON_BOARD
#include "mpi_sys.h"
#include "mpi_vb.h"
#else
#include "hi_comm_svp.h"
#include "hi_nnie.h"
#include "mpi_nnie.h"
#endif
#include "math.h"
#include "sample_save_blob.h"
#include "sample_resize_roi.h"
#include "sample_data_utils.h"
#include "sample_runtime_define.h"
#include "sample_runtime_detection_ssd.h"
#include "sample_model_ssd.h"

HI_S32 SAMPLE_RUNTIME_LoadModelGroup_SSD(
        const HI_CHAR* pcModelFileSSD,
        HI_RUNTIME_WK_INFO_S *pstWKInfo,
        HI_RUNTIME_GROUP_HANDLE* phGroupHandle)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_RUNTIME_GROUP_INFO_S stGroupInfo;
    HI_CHAR *pacConfig = NULL;

    memset(&stGroupInfo, 0, sizeof(HI_RUNTIME_GROUP_INFO_S));

    stGroupInfo.stWKsInfo.u32WKNum = 1;
    strncpy(pstWKInfo->acModelName, "ssd", MAX_NAME_LEN);
    s32Ret = SAMPLE_RUNTIME_LoadModelFile(pcModelFileSSD, &pstWKInfo->stWKMemory);
    SAMPLE_CHK_RET(HI_SUCCESS != s32Ret, HI_FAILURE, "LoadFile fail");
    stGroupInfo.stWKsInfo.pstAttrs = pstWKInfo;

    SAMPLE_RUNTIME_ReadConfig(CONFIG_DIR"ssd.modelgroup", &pacConfig);
    SAMPLE_CHK_GOTO((NULL == pacConfig), FAIL, "HI_SVPRT_RUNTIME_ReadConfig error\n");

    s32Ret = HI_SVPRT_RUNTIME_LoadModelGroup(pacConfig, &stGroupInfo, phGroupHandle);
    SAMPLE_CHK_GOTO((HI_SUCCESS != s32Ret), FAIL,
                                   "HI_SVPRT_RUNTIME_LoadModelGroup error\n");

    SAMPLE_LOG_INFO("LoadGroup succ, group handle[%p]\n", phGroupHandle);
    SAMPLE_FREE(pacConfig);
    return HI_SUCCESS;

FAIL:
    SAMPLE_FREE(pacConfig);
    SAMPLE_FreeMem(&pstWKInfo->stWKMemory);
    return HI_FAILURE;
}

HI_S32 SAMPLE_RUNTIME_SetGroupSrc_SSD(const HI_CHAR* pcSrcFile, HI_RUNTIME_BLOB_S* pastSrcSSDBlobs)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_RUNTIME_MEM_S stMem;

    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastSrcSSDBlobs[0], HI_RUNTIME_BLOB_TYPE_U8, 1, 300, 300, 3, ALIGN_16);
    SAMPLE_CHK_RET(HI_SUCCESS != s32Ret, HI_FAILURE, "SAMPLE_RUNTIME_SetBlob ssd failed!\n");

    s32Ret = SAMPLE_RUNTIME_ReadSrcFile(pcSrcFile, &(pastSrcSSDBlobs[0]), (HI_U32)strlen(pcSrcFile));
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_ReadSrcFile failed!\n");

    return HI_SUCCESS;
FAIL_0:
    stMem.u64PhyAddr = pastSrcSSDBlobs[0].u64PhyAddr;
    stMem.u64VirAddr = pastSrcSSDBlobs[0].u64VirAddr;
    SAMPLE_FreeMem(&stMem);
    return HI_FAILURE;
}

HI_S32 SAMPLE_RUNTIME_SetGroupDst_SSD(HI_RUNTIME_BLOB_S* pastDstSSDBlobs, const HI_U32 u32Length)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_U32 u32DstIndex = 0;
    if(0 == u32Length)
    {
        SAMPLE_LOG_INFO("pastDstSSDBlobs is zero length!");
        return HI_FAILURE;
    }
    //conv4_3_norm_mbox_loc_perm
    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastDstSSDBlobs[u32DstIndex++], HI_RUNTIME_BLOB_TYPE_S32, 1, 16, 38, 38, ALIGN_16);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_SetBlob ssd dst conv4_3_norm_mbox_loc_perm failed!\n");
    //conv4_3_norm_mbox_conf_perm
    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastDstSSDBlobs[u32DstIndex++], HI_RUNTIME_BLOB_TYPE_S32, 1, 84, 38, 38, ALIGN_16);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_SetBlob ssd dst conv4_3_norm_mbox_conf_perm failed!\n");
    //fc7_mbox_loc_perm
    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastDstSSDBlobs[u32DstIndex++], HI_RUNTIME_BLOB_TYPE_S32, 1, 24, 19, 19, ALIGN_16);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_SetBlob ssd dst fc7_mbox_loc_perm failed!\n");
    //fc7_mbox_conf_perm
    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastDstSSDBlobs[u32DstIndex++], HI_RUNTIME_BLOB_TYPE_S32, 1, 126, 19, 19, ALIGN_16);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_SetBlob ssd dst fc7_mbox_conf_perm failed!\n");
    //conv6_2_mbox_loc_perm
    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastDstSSDBlobs[u32DstIndex++], HI_RUNTIME_BLOB_TYPE_S32, 1, 24, 10, 10, ALIGN_16);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_SetBlob ssd dst conv6_2_mbox_loc_perm failed!\n");
    //conv6_2_mbox_conf_perm
    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastDstSSDBlobs[u32DstIndex++], HI_RUNTIME_BLOB_TYPE_S32, 1, 126, 10, 10, ALIGN_16);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_SetBlob ssd dst conv6_2_mbox_conf_perm failed!\n");
    //conv7_2_mbox_loc_perm
    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastDstSSDBlobs[u32DstIndex++], HI_RUNTIME_BLOB_TYPE_S32, 1, 24, 5, 5, ALIGN_16);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_SetBlob ssd dst conv7_2_mbox_loc_perm failed!\n");
    //conv7_2_mbox_conf_perm
    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastDstSSDBlobs[u32DstIndex++], HI_RUNTIME_BLOB_TYPE_S32, 1, 126, 5, 5, ALIGN_16);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_SetBlob ssd dst conv7_2_mbox_conf_perm failed!\n");
    //conv8_2_mbox_loc_perm
    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastDstSSDBlobs[u32DstIndex++], HI_RUNTIME_BLOB_TYPE_S32, 1, 16, 3, 3, ALIGN_16);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_SetBlob ssd dst conv8_2_mbox_loc_perm failed!\n");
    //conv8_2_mbox_conf_perm
    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastDstSSDBlobs[u32DstIndex++], HI_RUNTIME_BLOB_TYPE_S32, 1, 84, 3, 3, ALIGN_16);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_SetBlob ssd dst conv8_2_mbox_conf_perm failed!\n");
    //conv9_2_mbox_loc_perm
    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastDstSSDBlobs[u32DstIndex++], HI_RUNTIME_BLOB_TYPE_S32, 1, 16, 1, 1, ALIGN_16);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_SetBlob ssd dst conv9_2_mbox_loc_perm failed!\n");
    //conv9_2_mbox_conf_perm
    s32Ret = SAMPLE_RUNTIME_SetBlob((HI_RUNTIME_BLOB_S*)&pastDstSSDBlobs[u32DstIndex++], HI_RUNTIME_BLOB_TYPE_S32, 1, 84, 1, 1, ALIGN_16);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_SetBlob ssd dst conv9_2_mbox_conf_perm failed!\n");

    return HI_SUCCESS;
FAIL_0:
    return HI_FAILURE;
}

HI_S32 SAMPLE_RUNTIME_ForwardGroup_SSD(const HI_CHAR* pcSrcFile, HI_RUNTIME_GROUP_HANDLE hGroupHandle)
{
    HI_S32 s32Ret = HI_FAILURE;
#if DEBUG
    HI_CHAR* pcOutName = HI_NULL;
#endif
    HI_S32 as32ResultROI[200 * SVP_WK_PROPOSAL_WIDTH] = { 0 };
    HI_U32 u32ResultROICnt = 0;
    HI_RUNTIME_MEM_S stMem;
    HI_RUNTIME_GROUP_SRC_BLOB_ARRAY_S astSrcGroupBlobArray;
    HI_RUNTIME_GROUP_DST_BLOB_ARRAY_S astDstGroupBlobArray;

    HI_RUNTIME_GROUP_BLOB_S astInputBlob[1];
    HI_RUNTIME_GROUP_BLOB_S astOutputBlob[12];
    HI_RUNTIME_BLOB_S astSrc[1];
    HI_RUNTIME_BLOB_S astDst[12];

    memset(&astInputBlob[0], 0, sizeof(astInputBlob));
    memset(&astOutputBlob[0], 0, sizeof(astOutputBlob));

    astSrcGroupBlobArray.u32BlobNum = 1;
    astDstGroupBlobArray.u32BlobNum = 12;
    astSrcGroupBlobArray.pstBlobs = astInputBlob;
    astDstGroupBlobArray.pstBlobs = astOutputBlob;

    strncpy(astInputBlob[0].acOwnerName, "", MAX_NAME_LEN);
    strncpy(astInputBlob[0].acBlobName, "data", MAX_NAME_LEN);
    astInputBlob[0].pstBlob = astSrc;

    strncpy(astOutputBlob[0].acOwnerName, "ssd", MAX_NAME_LEN);
    strncpy(astOutputBlob[0].acBlobName, "conv4_3_norm_mbox_loc_perm", MAX_NAME_LEN);
    astOutputBlob[0].pstBlob = &astDst[0];
    strncpy(astOutputBlob[1].acOwnerName, "ssd", MAX_NAME_LEN);
    strncpy(astOutputBlob[1].acBlobName, "conv4_3_norm_mbox_conf_perm", MAX_NAME_LEN);
    astOutputBlob[1].pstBlob = &astDst[1];

    strncpy(astOutputBlob[2].acOwnerName, "ssd", MAX_NAME_LEN);
    strncpy(astOutputBlob[2].acBlobName, "fc7_mbox_loc_perm", MAX_NAME_LEN);
    astOutputBlob[2].pstBlob = &astDst[2];
    strncpy(astOutputBlob[3].acOwnerName, "ssd", MAX_NAME_LEN);
    strncpy(astOutputBlob[3].acBlobName, "fc7_mbox_conf_perm", MAX_NAME_LEN);
    astOutputBlob[3].pstBlob = &astDst[3];

    strncpy(astOutputBlob[4].acOwnerName, "ssd", MAX_NAME_LEN);
    strncpy(astOutputBlob[4].acBlobName, "conv6_2_mbox_loc_perm", MAX_NAME_LEN);
    astOutputBlob[4].pstBlob = &astDst[4];
    strncpy(astOutputBlob[5].acOwnerName, "ssd", MAX_NAME_LEN);
    strncpy(astOutputBlob[5].acBlobName, "conv6_2_mbox_conf_perm", MAX_NAME_LEN);
    astOutputBlob[5].pstBlob = &astDst[5];

    strncpy(astOutputBlob[6].acOwnerName, "ssd", MAX_NAME_LEN);
    strncpy(astOutputBlob[6].acBlobName, "conv7_2_mbox_loc_perm", MAX_NAME_LEN);
    astOutputBlob[6].pstBlob = &astDst[6];
    strncpy(astOutputBlob[7].acOwnerName, "ssd", MAX_NAME_LEN);
    strncpy(astOutputBlob[7].acBlobName, "conv7_2_mbox_conf_perm", MAX_NAME_LEN);
    astOutputBlob[7].pstBlob = &astDst[7];

    strncpy(astOutputBlob[8].acOwnerName, "ssd", MAX_NAME_LEN);
    strncpy(astOutputBlob[8].acBlobName, "conv8_2_mbox_loc_perm", MAX_NAME_LEN);
    astOutputBlob[8].pstBlob = &astDst[8];
    strncpy(astOutputBlob[9].acOwnerName, "ssd", MAX_NAME_LEN);
    strncpy(astOutputBlob[9].acBlobName, "conv8_2_mbox_conf_perm", MAX_NAME_LEN);
    astOutputBlob[9].pstBlob = &astDst[9];

    strncpy(astOutputBlob[10].acOwnerName, "ssd", MAX_NAME_LEN);
    strncpy(astOutputBlob[10].acBlobName, "conv9_2_mbox_loc_perm", MAX_NAME_LEN);
    astOutputBlob[10].pstBlob = &astDst[10];
    strncpy(astOutputBlob[11].acOwnerName, "ssd", MAX_NAME_LEN);
    strncpy(astOutputBlob[11].acBlobName, "conv9_2_mbox_conf_perm", MAX_NAME_LEN);
    astOutputBlob[11].pstBlob = &astDst[11];

    memset(astSrc, 0, sizeof(astSrc));
    memset(astDst, 0, sizeof(astDst));

    s32Ret = SAMPLE_RUNTIME_SetGroupSrc_SSD(pcSrcFile, astSrc);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "SAMPLE_RUNTIME_SetGroupSrc_FrcnnAlex failed!\n");

    s32Ret = SAMPLE_RUNTIME_SetGroupDst_SSD(astDst,sizeof(astDst));
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "SAMPLE_RUNTIME_SetGroupDst_FrcnnAlex failed!\n");
#if PERFORMANCE_TEST
    long spend;
    struct timespec start, end;
    clock_gettime(0, &start);
#endif
    s32Ret = HI_SVPRT_RUNTIME_ForwardGroupSync(hGroupHandle, &astSrcGroupBlobArray, &astDstGroupBlobArray, 0);
#if PERFORMANCE_TEST
    clock_gettime(0, &end);
    spend = (end.tv_sec - start.tv_sec) * 1000 * 1000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("\n\n[Forward]===== TIME SPEND: %ldms, %ldus =====\n\n", spend/1000, spend);
#endif
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0, "HI_SVPRT_RUNTIME_ForwardGroupSync failed!\n");

    SAMPLE_LOG_INFO("Pic: %s\n", pcSrcFile);
    SAMPLE_Ssd_GetResult(astSrc, astDst, as32ResultROI, &u32ResultROICnt, sizeof(astDst));
    SAMPLE_LOG_INFO("roi cnt: %d\n", u32ResultROICnt);

    drawImageRect("ssd", &astSrc[0], as32ResultROI, u32ResultROICnt, 6);

#if DEBUG
    for (int i = 0; i < 12; i++)
    {
        switch(i) {
            case 0:
                pcOutName = (HI_CHAR*)"conv4_3_norm_mbox_loc_perm";
                break;
            case 1:
                pcOutName = (HI_CHAR*)"conv4_3_norm_mbox_conf_perm";
                break;
            case 2:
                pcOutName = (HI_CHAR*)"fc7_mbox_loc_perm";
                break;
            case 3:
                pcOutName = (HI_CHAR*)"fc7_mbox_conf_perm";
                break;
            case 4:
                pcOutName = (HI_CHAR*)"conv6_2_mbox_loc_perm";
                break;
            case 5:
                pcOutName = (HI_CHAR*)"conv6_2_mbox_conf_perm";
                break;
            case 6:
                pcOutName = (HI_CHAR*)"conv7_2_mbox_loc_perm";
                break;
            case 7:
                pcOutName = (HI_CHAR*)"conv7_2_mbox_conf_perm";
                break;
            case 8:
                pcOutName = (HI_CHAR*)"conv8_2_mbox_loc_perm";
                break;
            case 9:
                pcOutName = (HI_CHAR*)"conv8_2_mbox_conf_perm";
                break;
            case 10:
                pcOutName = (HI_CHAR*)"conv9_2_mbox_loc_perm";
                break;
            case 11:
                pcOutName = (HI_CHAR*)"conv9_2_mbox_conf_perm";
                break;
            default:
                break;
        }
        printDebugData(pcOutName, astDst[i].u64VirAddr, 10);
    }

#endif

FAIL_0:

    stMem.u64PhyAddr = astSrc[0].u64PhyAddr;
    stMem.u64VirAddr = astSrc[0].u64VirAddr;
    SAMPLE_FreeMem(&stMem);

    for (HI_U32 i = 0; i < 12; ++i) {
        stMem.u64PhyAddr = astDst[i].u64PhyAddr;
        stMem.u64VirAddr = astDst[i].u64VirAddr;
        SAMPLE_FreeMem(&stMem);
    }
    return s32Ret;
}

HI_S32 SAMPLE_SSD()
{
    const HI_CHAR* pcRuntimeModelName = MODEL_SSD_NAME;
    const HI_CHAR* pcSrcFile = IMAGE_SSD_NAME;
    HI_RUNTIME_WK_INFO_S stWKInfo;
    HI_S32 s32Ret = HI_FAILURE;

    HI_RUNTIME_GROUP_HANDLE hGroupHandle;

    memset(&stWKInfo, 0, sizeof(HI_RUNTIME_WK_INFO_S));

    long spend;
    struct timespec start, next, end;
    clock_gettime(0, &start);

    printf("\n============================= ssd net begin ================================\n");
    s32Ret = HI_SVPRT_RUNTIME_Init(CPU_TASK_AFFINITY, NULL);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_1, "HI_SVPRT_RUNTIME_Init failed!\n");

    s32Ret = SAMPLE_RUNTIME_LoadModelGroup_SSD(pcRuntimeModelName, &stWKInfo, &hGroupHandle);
    SAMPLE_CHK_GOTO(HI_SUCCESS != s32Ret, FAIL_0,
                                   "SAMPLE_RUNTIME_LoadModelGroup_SSD failed!\n");

    clock_gettime(0, &end);
    spend = ((long)end.tv_sec - (long)start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
    SAMPLE_LOG_INFO("\n[Load]===== TIME SPEND: %ld ms =====\n", spend);

    s32Ret = SAMPLE_RUNTIME_ForwardGroup_SSD(pcSrcFile, hGroupHandle);
    SAMPLE_CHK_PRINTF((HI_SUCCESS != s32Ret), "SAMPLE_RUNTIME_ForwardGroup_SSD error\n");

    clock_gettime(0, &next);
    spend = ((long)next.tv_sec - (long)end.tv_sec) * 1000 + (next.tv_nsec - end.tv_nsec) / 1000000;
    SAMPLE_LOG_INFO("\n[Forward total]===== TIME SPEND: %ld ms =====\n", spend);

    SAMPLE_CHK_PRINTF((HI_SUCCESS != HI_SVPRT_RUNTIME_UnloadModelGroup(hGroupHandle)), "HI_SVPRT_RUNTIME_UnloadModelGroup error\n");

FAIL_0:
    SAMPLE_FreeMem(&stWKInfo.stWKMemory);
    (HI_VOID)HI_SVPRT_RUNTIME_DeInit();

    clock_gettime(0, &end);
    spend = ((long)end.tv_sec - (long)start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
    SAMPLE_LOG_INFO("\n[Total]===== TIME SPEND: %ld ms =====\n", spend);
FAIL_1:
    SAMPLE_LOG_INFO("SAMPLE_SSD result %d !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", s32Ret);
    return s32Ret;
}
