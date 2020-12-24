 /*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: liteplayer ipcmsg function.
 * Author: HiMobileCam middleware develop team
 * Create: 2019.05.18
 */

#ifndef HI_AVPLAY_IPCMSG_SERVER_H
#define HI_AVPLAY_IPCMSG_SERVER_H

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

HI_S32 HI_AVPLAY_IPCMSG_SVR_Init(const HI_CHAR *pszFFLibName);

HI_S32 HI_AVPLAY_IPCMSG_SVR_Deinit();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HI_AVPLAYER_IPCMSG_SERVER_H__ */
