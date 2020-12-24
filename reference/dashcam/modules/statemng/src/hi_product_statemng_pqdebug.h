/**
 * @file      hi_product_statemng_pqdebug.h
 * @brief     Describes the data structure definition,the micro definition,
 *            and the function definition inner statemng fucntion module.
 *
 * Copyright (c) 2019 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/06/04
 * @version   1.0
 */
#ifndef __HI_PRODUCT_STATEMNG_PQDEBUG__
#define __HI_PRODUCT_STATEMNG_PQDEBUG__


#include "hi_type.h"
#include "hi_message.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

//#ifdef CONFIG_PQT_SUPPORT_ON

/**
 * @brief        rollback to pq debug
 * @in           PQDebug statues
 * @return       0 success, non-zero error code
 * @exception    None
 * @author       HiMobileCam Reference Develop Team
 * @date         2019/06/04
 */
HI_S32 PDT_STATEMNG_SetPQDebug(HI_BOOL state);


/**
 * @brief        handle PQDebug message
 * @in           message
 * @return       0 success, non-zero error code
 * @exception    None
 * @author       HiMobileCam Reference Develop Team
 * @date         2019/06/04
 */
HI_S32 PDT_STATEMNG_HandlePQDebugMsg(HI_MESSAGE_S* msg);

/**
 * @brief        Set Media For PQDebug
 * @return       0 success, non-zero error code
 * @exception    None
 * @author       HiMobileCam Reference Develop Team
 * @date         2019/06/04
 */
HI_S32 PDT_STATEMNG_AddPQDebugStreams(HI_VOID);
//#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __HI_PRODUCT_STATEMNG_INNER__ */
