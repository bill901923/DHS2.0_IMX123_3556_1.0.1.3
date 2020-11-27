#ifndef SAMPLE_MODEL_SSD_H
#define SAMPLE_MODEL_SSD_H
#include "hi_runtime_comm.h"

#ifdef __cplusplus
extern "C"
{
#endif

HI_S32 SAMPLE_Ssd_GetResult(HI_RUNTIME_BLOB_S* pstSrcBlob, HI_RUNTIME_BLOB_S* pstDstBlob,
                            HI_S32* ps32ResultROI, HI_U32* pu32ResultROICnt, const HI_U32 u32Length);
#ifdef __cplusplus
}
#endif
#endif
