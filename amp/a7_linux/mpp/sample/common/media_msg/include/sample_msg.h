#ifndef __SAMPLE_MSG_H__
#define __SAMPLE_MSG_H__

#include "hi_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define SAMPLE_SEND_MSG_TIMEOUT 900000

typedef enum hiSAMPLE_MOD_ID_E
{
    SAMPLE_COMM_MOD_VI = 0,
    SAMPLE_COMM_MOD_VENC,

    SAMPLE_MOD_VI,

    SAMPLE_MOD_BUTT,
} SAMPLE_MOD_ID_E;

HI_S32 Media_Msg_Init(HI_VOID);
HI_S32 Media_Msg_Deinit(HI_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_MSG_H__*/