/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * @file    msg_adec.h
 * @brief   msg adec header
 * @author  HiMobileCam NDK develop team
 * @date  2019-6-196
 */
#ifndef __MSG_SERVER_ADEC_H__
#define __MSG_SERVER_ADEC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/** \addtogroup     MSG */
/** @{ */ /** <!-- [MSG] */


typedef enum tagMSG_ACAP_CMD_E {
    MSG_CMD_ADEC_INIT,
    MSG_CMD_ADEC_DEINIT,
    MSG_CMD_ADEC_SEND_STREAM,
    MSG_CMD_ADEC_GET_FRAME,
    MSG_CMD_ADEC_RELEASE_FRAME,
    MSG_CMD_ADEC_SEND_ENDOFSTREAM,
    MSG_CMD_ADEC_BUTT
} MSG_ACAP_CMD_E;


/** @} */ /** <!-- ==== MSG End ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __MSG_SERVER_ADEC_H__ */
