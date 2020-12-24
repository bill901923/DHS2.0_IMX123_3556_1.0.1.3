/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: Definition of data structure
 * Author: HiMobileCam middleware develop team
 * Create: 2012-12-22
 */

#ifndef MESSAGE_H
#define MESSAGE_H
#include "hi_mw_type.h"

#define MESSAGE_PAYLOAD_LEN        (512)
typedef struct hi_MESSAGE_S{
    HI_S32 what;
    HI_S32 arg1;
    HI_S32 arg2;
    HI_S32 s32Result;
    HI_U64 u64CreateTime;
    HI_CHAR aszPayload[MESSAGE_PAYLOAD_LEN];
}HI_MESSAGE_S;

#endif
