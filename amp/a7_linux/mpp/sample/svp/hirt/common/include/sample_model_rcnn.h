#ifndef SAMPLE_MODEL_RCNN_H
#define SAMPLE_MODEL_RCNN_H
#include "hi_runtime_comm.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum SAMPLE_RUNTIME_MODEL_TYPE_E
{
    SAMPLE_RUNTIME_MODEL_TYPE_FRCNN = 1,
    SAMPLE_RUNTIME_MODEL_TYPE_RFCN = 2,
} SAMPLE_RUNTIME_MODEL_TYPE_E;

HI_S32 SAMPLE_DATA_GetRoiResultFromOriginSize(SAMPLE_RUNTIME_MODEL_TYPE_E enType,
                               HI_RUNTIME_BLOB_S* pstScoreBlob,
                               HI_RUNTIME_BLOB_S* pstBBoxBlob,
                               HI_RUNTIME_BLOB_S* pstProposalBlob,
                               HI_U32 u32Width,
                               HI_U32 u32Height,
                               HI_S32* ps32ResultROI,
                               HI_U32* pu32ResultROICnt);
HI_S32 SAMPLE_DATA_GetRoiResult(SAMPLE_RUNTIME_MODEL_TYPE_E enType,
                               HI_RUNTIME_BLOB_S* pstScoreBlob,
                               HI_RUNTIME_BLOB_S* pstBBoxBlob,
                               HI_RUNTIME_BLOB_S* pstProposalBlob,
                               HI_RUNTIME_BLOB_S* pstDataBlob,
                               HI_S32* ps32ResultROI,
                               HI_U32* pu32ResultROICnt);

#ifdef __cplusplus
}
#endif
#endif
