#ifndef __SAMPLE_DATA_UTILS_H
#define __SAMPLE_DATA_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "string.h"
#include "hi_runtime_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ALIGN_16
#define ALIGN_16 16
#define ALIGN16(u32Num) ((u32Num + ALIGN_16-1) / ALIGN_16 * ALIGN_16)
#endif
#ifndef ALIGN_32
#define ALIGN_32 32
#define ALIGN32(u32Num) ((u32Num + ALIGN_32-1) / ALIGN_32 * ALIGN_32)
#endif

#define SVP_WK_QUANT_BASE (0x1000)
#define MAX_STACK_DEPTH (50000)
#define SVP_WK_PROPOSAL_WIDTH (6)
#define RPN_SUPPRESS_FALSE (0)
#define RPN_SUPPRESS_TRUE (1)
#define SVP_WK_COORDI_NUM (4)
#define SAMPLE_SVP_NNIE_HALF 0.5f
#ifndef max
#define max(x,y) (x>y?x:y)
#endif
#ifndef min
#define min(x,y) (x<y?x:y)
#endif

#define SAMPLE_IVE_RESIZE_BATCH_MAX (64)

typedef struct hiNNIE_STACK
{
    HI_S32 s32Min;
    HI_S32 s32Max;
} NNIE_STACK_S;

typedef struct tagTRACKER_LOCATION
{
    HI_S32 x1;
    HI_S32 y1;
    HI_S32 x2;
    HI_S32 y2;
}TRACKER_LOCATION_S;

typedef struct tagBondingBox
{
    HI_DOUBLE x1;
    HI_DOUBLE y1;
    HI_DOUBLE x2;
    HI_DOUBLE y2;
}BondingBox_s;

HI_S32 NonRecursiveArgQuickSort(HI_S32* aResultArray,
                                       HI_S32 s32Low, HI_S32 s32High, NNIE_STACK_S* pstStack, HI_U32 u32MaxNum);
HI_S32 NonMaxSuppression(HI_S32* pu32Proposals, HI_U32 u32NumAnchors, HI_U32 u32NmsThresh);
HI_VOID SAMPLE_DATA_GetStride(HI_RUNTIME_BLOB_TYPE_E type, HI_U32 width, HI_U32 align, HI_U32* pStride);
HI_U32 SAMPLE_DATA_GetBlobSize(HI_U32 stride, HI_U32 num, HI_U32 height, HI_U32 chn);
HI_S32 SAMPLE_RUNTIME_HiMemAlloc(HI_RUNTIME_MEM_S* pstMem, HI_BOOL bCached);
HI_S32 SAMPLE_RUNTIME_LoadModelFile(const HI_CHAR* pcModelFile, HI_RUNTIME_MEM_S* pstMemInfo);
HI_S32 SAMPLE_RUNTIME_HiBlobAlloc(HI_RUNTIME_BLOB_S* pstBlob, HI_U32 u32BlobSize, HI_BOOL bCached);
HI_S32 SAMPLE_RUNTIME_SetBlob(HI_RUNTIME_BLOB_S* pstBlob,
                              HI_RUNTIME_BLOB_TYPE_E enType,
                              HI_U32 u32Num,
                              HI_U32 u32Width,
                              HI_U32 u32Height,
                              HI_U32 u32Chn,
                              HI_U32 u32Align);
HI_S32 SAMPLE_RUNTIME_ReadSrcFile(const HI_CHAR* pcSrcFile, HI_RUNTIME_BLOB_S* pstSrcBlob, const HI_U32 u32Length);
HI_S32 SAMPLE_RUNTIME_ReadConfig(const HI_CHAR* pcConfigFile, HI_CHAR **acBuff);
HI_DOUBLE compute_output_w(HI_DOUBLE x1, HI_DOUBLE x2);
HI_DOUBLE compute_output_h(HI_DOUBLE y1, HI_DOUBLE y2);
HI_VOID computeCropLocation(const BondingBox_s *pstTightBbox, HI_DOUBLE dWidth, HI_DOUBLE dHeight, BondingBox_s* pstLocationBbox);
HI_DOUBLE compute_edge_x(HI_DOUBLE x1, HI_DOUBLE x2);
HI_DOUBLE compute_edge_y(HI_DOUBLE y1, HI_DOUBLE y2);

#ifdef __cplusplus
}
#endif

#endif
