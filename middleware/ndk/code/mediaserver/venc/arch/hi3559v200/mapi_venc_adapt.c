/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    mapi_venc_adapt.c
 * @brief   venc adapt module function
 * @author  HiMobileCam NDK develop team
 * @date  2019-3-26
 */
#include "mpi_venc.h"

#include "hi_mapi_comm_define.h"
#include "hi_mapi_venc_define.h"
#include "mapi_venc_inner.h"
#include "mapi_comm_inner.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

HI_S32 HAL_MAPI_VENC_ConfigH265CarTrans(HI_HANDLE VencHdl, HI_MAPI_PAYLOAD_TYPE_E enType,
                                        HI_MAPI_VENC_SCENE_MODE_E enSceneMode)
{
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_H265_TRANS_S stH265Trans;

    if ((enType == HI_MAPI_PAYLOAD_TYPE_H265) && (enSceneMode == HI_MAPI_VENC_SCENE_MODE_CAR)) {
        s32Ret = HI_MPI_VENC_GetH265Trans(VencHdl, &stH265Trans);
        if (s32Ret != HI_SUCCESS) {
            MAPI_ERR_TRACE(HI_MAPI_MOD_VENC, "HI_MPI_VENC_GetH265Trans fail,Ret:%x\n", s32Ret);
            return s32Ret;
        }

        stH265Trans.bScalingListEnabled = HI_TRUE;
        s32Ret = HI_MPI_VENC_SetH265Trans(VencHdl, &stH265Trans);
        if (s32Ret != HI_SUCCESS) {
            MAPI_ERR_TRACE(HI_MAPI_MOD_VENC, "HI_MPI_VENC_SetH265Trans fail,Ret:%x\n", s32Ret);
            return s32Ret;
        }
    }

    return HI_SUCCESS;
}

HI_S32 HAL_MAPI_VENC_ConfigH265Attr(const HI_MAPI_VENC_RC_ATTR_S *pstRcAttr,
                                    VENC_CHN_ATTR_S *pstVencChnAttr)
{
    HI_S32 s32Ret;
    CHECK_MAPI_VENC_NULL_PTR_RET(pstRcAttr);
    CHECK_MAPI_VENC_NULL_PTR_RET(pstVencChnAttr);

    switch (pstRcAttr->enRcMode) {
        case HI_MAPI_VENC_RC_MODE_CBR: {
            pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
            s32Ret = memcpy_s(&pstVencChnAttr->stRcAttr.stH265Cbr, sizeof(VENC_H265_CBR_S), &pstRcAttr->stAttrCbr,
                sizeof(HI_MAPI_VENC_ATTR_CBR_S));
            MAPI_CHECK_RET(HI_MAPI_MOD_VENC, "memcpy_s", s32Ret);
            break;
        }

        case HI_MAPI_VENC_RC_MODE_VBR: {
            pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
            s32Ret = memcpy_s(&pstVencChnAttr->stRcAttr.stH265Vbr, sizeof(VENC_H265_VBR_S), &pstRcAttr->stAttrVbr,
                sizeof(HI_MAPI_VENC_ATTR_VBR_S));
            MAPI_CHECK_RET(HI_MAPI_MOD_VENC, "memcpy_s", s32Ret);
            break;
        }

        case HI_MAPI_VENC_RC_MODE_QVBR: {
            pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265QVBR;
            s32Ret = memcpy_s(&pstVencChnAttr->stRcAttr.stH265QVbr, sizeof(VENC_H265_QVBR_S), &pstRcAttr->stAttrQVbr,
                sizeof(HI_MAPI_VENC_ATTR_QVBR_S));
            MAPI_CHECK_RET(HI_MAPI_MOD_VENC, "memcpy_s", s32Ret);
            break;
        }

        case HI_MAPI_VENC_RC_MODE_CVBR: {
            pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265CVBR;
            s32Ret = memcpy_s(&pstVencChnAttr->stRcAttr.stH265CVbr, sizeof(VENC_H265_CVBR_S), &pstRcAttr->stAttrCVbr,
                sizeof(HI_MAPI_VENC_ATTR_CVBR_S));
            MAPI_CHECK_RET(HI_MAPI_MOD_VENC, "memcpy_s", s32Ret);
            break;
        }

        case HI_MAPI_VENC_RC_MODE_AVBR: {
            MAPI_ERR_TRACE(HI_MAPI_MOD_VENC, "AVBR is not support!\n");;
            return HI_MAPI_VENC_EILLEGAL_PARAM;
        }

        default: {
            pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
            s32Ret = memcpy_s(&pstVencChnAttr->stRcAttr.stH265Cbr, sizeof(VENC_H265_CBR_S), &pstRcAttr->stAttrCbr,
                sizeof(HI_MAPI_VENC_ATTR_CBR_S));
            MAPI_CHECK_RET(HI_MAPI_MOD_VENC, "memcpy_s", s32Ret);
            MAPI_ERR_TRACE(HI_MAPI_MOD_VENC,
                "RC mode only support CBR or VBR or QVBR or CVBR or AVBR, default use CBR!\n");
            break;
        }
    }

    pstVencChnAttr->stVencAttr.stAttrH265e.bRcnRefShareBuf = HI_TRUE;

    return HI_SUCCESS;
}

HI_S32 HAL_MAPI_VENC_ConfigH264Attr(const HI_MAPI_VENC_RC_ATTR_S *pstRcAttr,
                                    VENC_CHN_ATTR_S *pstVencChnAttr)
{
    HI_S32 s32Ret;
    CHECK_MAPI_VENC_NULL_PTR_RET(pstRcAttr);
    CHECK_MAPI_VENC_NULL_PTR_RET(pstVencChnAttr);

    switch (pstRcAttr->enRcMode) {
        case HI_MAPI_VENC_RC_MODE_CBR: {
            pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
            s32Ret = memcpy_s(&pstVencChnAttr->stRcAttr.stH264Cbr, sizeof(VENC_H264_CBR_S), &pstRcAttr->stAttrCbr,
                sizeof(HI_MAPI_VENC_ATTR_CBR_S));
            MAPI_CHECK_RET(HI_MAPI_MOD_VENC, "memcpy_s", s32Ret);
            break;
        }

        case HI_MAPI_VENC_RC_MODE_VBR: {
            pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
            s32Ret = memcpy_s(&pstVencChnAttr->stRcAttr.stH264Vbr, sizeof(VENC_H264_VBR_S), &pstRcAttr->stAttrVbr,
                sizeof(HI_MAPI_VENC_ATTR_VBR_S));
            MAPI_CHECK_RET(HI_MAPI_MOD_VENC, "memcpy_s", s32Ret);
            break;
        }

        case HI_MAPI_VENC_RC_MODE_QVBR: {
            pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264QVBR;
            s32Ret = memcpy_s(&pstVencChnAttr->stRcAttr.stH264QVbr, sizeof(VENC_H264_QVBR_S), &pstRcAttr->stAttrQVbr,
                sizeof(HI_MAPI_VENC_ATTR_QVBR_S));
            MAPI_CHECK_RET(HI_MAPI_MOD_VENC, "memcpy_s", s32Ret);
            break;
        }

        case HI_MAPI_VENC_RC_MODE_CVBR: {
            pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264CVBR;
            s32Ret = memcpy_s(&pstVencChnAttr->stRcAttr.stH264CVbr, sizeof(VENC_H264_CVBR_S), &pstRcAttr->stAttrCVbr,
                sizeof(HI_MAPI_VENC_ATTR_CVBR_S));
            MAPI_CHECK_RET(HI_MAPI_MOD_VENC, "memcpy_s", s32Ret);
            break;
        }

        case HI_MAPI_VENC_RC_MODE_AVBR: {
            MAPI_ERR_TRACE(HI_MAPI_MOD_VENC, "AVBR is not support!\n");;
            return HI_MAPI_VENC_EILLEGAL_PARAM;
        }

        default: {
            pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
            s32Ret = memcpy_s(&pstVencChnAttr->stRcAttr.stH264Cbr, sizeof(VENC_H264_CBR_S), &pstRcAttr->stAttrCbr,
                sizeof(HI_MAPI_VENC_ATTR_CBR_S));
            MAPI_CHECK_RET(HI_MAPI_MOD_VENC, "memcpy_s", s32Ret);
            MAPI_ERR_TRACE(HI_MAPI_MOD_VENC,
                "RC mode only support CBR or VBR or QVBR or CVBR or AVBR, default use CBR!\n");
            break;
        }
    }

    pstVencChnAttr->stVencAttr.stAttrH264e.bRcnRefShareBuf = HI_TRUE;

    return HI_SUCCESS;
}

HI_S32 HAL_MAPI_VENC_ConfigReEncodeTimes(HI_HANDLE VencHdl, const HI_MAPI_PAYLOAD_TYPE_E enType,
    const HI_MAPI_VENC_RC_MODE_E enRcMode)
{
    HI_S32 s32Ret;
    VENC_RC_PARAM_S stVencRcPara;

    // only H264 or H265 support config
    if ((enType != HI_MAPI_PAYLOAD_TYPE_H264) && (enType != HI_MAPI_PAYLOAD_TYPE_H265)) {
        return HI_SUCCESS;
    }

    s32Ret = HI_MPI_VENC_GetRcParam(VencHdl, &stVencRcPara);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VENC, "HI_MPI_VENC_GetRcParam err 0x%x\n", s32Ret);
        return s32Ret;
    }

    if (enType == HI_MAPI_PAYLOAD_TYPE_H264) {
        switch (enRcMode) {
            case HI_MAPI_VENC_RC_MODE_CBR: {
                stVencRcPara.stParamH264Cbr.s32MaxReEncodeTimes = 0;
                break;
            }

            case HI_MAPI_VENC_RC_MODE_VBR: {
                stVencRcPara.stParamH264Vbr.s32MaxReEncodeTimes = 0;
                break;
            }

            case HI_MAPI_VENC_RC_MODE_QVBR: {
                stVencRcPara.stParamH264QVbr.s32MaxReEncodeTimes = 0;
                break;
            }

            case HI_MAPI_VENC_RC_MODE_CVBR: {
                stVencRcPara.stParamH264CVbr.s32MaxReEncodeTimes = 0;
                break;
            }

            case HI_MAPI_VENC_RC_MODE_AVBR: {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VENC, "AVBR is not support!\n");;
                return HI_MAPI_VENC_EILLEGAL_PARAM;
            }

            default: {
                stVencRcPara.stParamH264Cbr.s32MaxReEncodeTimes = 0;
                MAPI_ERR_TRACE(HI_MAPI_MOD_VENC,
                    "RC mode only support CBR or VBR or QVBR or CVBR or AVBR, default use CBR!\n");
                break;
            }
        }
    } else if (enType == HI_MAPI_PAYLOAD_TYPE_H265) {

        switch (enRcMode) {
            case HI_MAPI_VENC_RC_MODE_CBR: {
                stVencRcPara.stParamH265Cbr.s32MaxReEncodeTimes = 0;
                break;
            }

            case HI_MAPI_VENC_RC_MODE_VBR: {
                stVencRcPara.stParamH265Vbr.s32MaxReEncodeTimes = 0;
                break;
            }

            case HI_MAPI_VENC_RC_MODE_QVBR: {
                stVencRcPara.stParamH265QVbr.s32MaxReEncodeTimes = 0;
                break;
            }

            case HI_MAPI_VENC_RC_MODE_CVBR: {
                stVencRcPara.stParamH265CVbr.s32MaxReEncodeTimes = 0;
                break;
            }

            case HI_MAPI_VENC_RC_MODE_AVBR: {
                MAPI_ERR_TRACE(HI_MAPI_MOD_VENC, "AVBR is not support!\n");;
                return HI_MAPI_VENC_EILLEGAL_PARAM;
            }

            default: {
                stVencRcPara.stParamH265Cbr.s32MaxReEncodeTimes = 0;
                MAPI_ERR_TRACE(HI_MAPI_MOD_VENC,
                    "RC mode only support CBR or VBR or QVBR or CVBR or AVBR, default use CBR!\n");
                break;
            }
        }
    }

    s32Ret = HI_MPI_VENC_SetRcParam(VencHdl, &stVencRcPara);
    if (s32Ret != HI_SUCCESS) {
        MAPI_ERR_TRACE(HI_MAPI_MOD_VENC, "HI_MPI_VENC_SetRcParam err 0x%x\n", s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
