#ifndef SAMPLE_MODEL_CLASSIFICATION_H
#define SAMPLE_MODEL_CLASSIFICATION_H
#include "hi_runtime_comm.h"

#ifdef __cplusplus
extern "C"
{
#endif

HI_S32 SAMPLE_RUNTIME_Cnn_TopN_Output(HI_RUNTIME_BLOB_S* pstDst, HI_U32 u32TopN);

#ifdef __cplusplus
}
#endif

#endif
