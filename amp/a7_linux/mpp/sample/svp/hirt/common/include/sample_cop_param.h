#ifndef __SAMPLE_COP_PARAM_H
#define __SAMPLE_COP_PARAM_H
#include "hi_nnie.h"
#include "hi_runtime_comm.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct hiNodePlugin_Param_S
{
    HI_U32 u32SrcWidth;
    HI_U32 u32SrcHeight;
    SVP_NNIE_NET_TYPE_E enNetType;
    HI_U32 u32NumRatioAnchors;
    HI_U32 u32NumScaleAnchors;
    HI_FLOAT* pfRatio;
    HI_FLOAT* pfScales;
    HI_U32 u32MaxRoiFrameCnt;
    HI_U32 u32MinSize;
    HI_FLOAT fSpatialScale;
    HI_FLOAT fNmsThresh;
    HI_FLOAT fFilterThresh;
    HI_U32 u32NumBeforeNms;
    HI_FLOAT fConfThresh;
    HI_FLOAT fValidNmsThresh;
    HI_U32 u32ClassSize;
} HI_NODEPlugin_Param_S;

typedef enum hiSAMPLE_WK_DETECT_NET_FASTER_RCNN_TYPE_E
{
    SAMPLE_WK_DETECT_NET_FASTER_RCNN_ALEX = 0x0,        //fasterrcnn_alexnet
    SAMPLE_WK_DETECT_NET_FASTER_RCNN_VGG16,             //fasterrcnn_vgg16
    SAMPLE_WK_DETECT_NET_FASTER_RCNN_RES18,             //fasterrcnn_res18
    SAMPLE_WK_DETECT_NET_FASTER_RCNN_RES34,             //fasterrcnn_res34
    SAMPLE_WK_DETECT_NET_FASTER_RCNN_PVANET,            //fasterrcnn_pvanet

    SAMPLE_WK_DETECT_NET_FASTER_RCNN_TYPE_BUTT,
}SAMPLE_WK_DETECT_NET_FASTER_RCNN_TYPE_E;

typedef struct hiProposal_param_s
{
    HI_RUNTIME_MEM_S stBachorMemInfo;
    HI_NODEPlugin_Param_S stNodePluginParam;
} HI_PROPOSAL_Param_S;

HI_S32 createFasterrcnnCopParam(HI_U32 u32ParamNum, HI_RUNTIME_COP_ATTR_S* pCopParam, HI_PROPOSAL_Param_S* pPluginParam, HI_U8 u8NetType);
HI_S32 createRFCNCopParam(HI_U32 u32ParamNum, HI_RUNTIME_COP_ATTR_S* pCopParam, HI_PROPOSAL_Param_S* pPluginParam);
HI_VOID releaseRfcnAndFrcnnCopParam(HI_U32 u32ParamNum, HI_PROPOSAL_Param_S* pCopParam);

#ifdef __cplusplus
}
#endif

#endif
