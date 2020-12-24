/**
 * @file      hi_product_res.h
 * @brief     resource api
 *
 * Copyright (c) 2017 Huawei Tech.Co.,Ltd
 *
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/6/10
 * @version   1.0

 */
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/** res purpose */
typedef enum {
    HI_PDT_RES_PURP_NONE            = 0,
    HI_PDT_RES_PURP_BOOTLOGO        = 1,
    HI_PDT_RES_PURP_BOOTSOUND       = 2,
    HI_PDT_RES_PURP_OSDFONT         = 3,
    HI_PDT_RES_PURP_OSDLOGO_MAIN    = 4,
    HI_PDT_RES_PURP_OSDLOGO_SUB     = 5,
    HI_PDT_RES_PURP_BUTT
} HI_PDT_RES_Purpose;

/** function interface */
/**
 * @brief    load resource.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/6/10
 */
HI_S32 HI_PDT_RES_Load(HI_VOID);

/**
 * @brief    get resource mem addr or data lenght.
 * @param[in] purpose:purpose of resource.
 * @param[out] memAddr:memory address of resource if not empty.
 * @param[out] dataLen:lenght of resource data if not empty.
 * @return 0 success,non-zero error code.
 * @exception    None
 * @author    HiMobileCam Reference Develop Team
 * @date      2019/6/10
 */
HI_S32 HI_PDT_RES_GetData(HI_PDT_RES_Purpose purpose, HI_U8 **memAddr, HI_U32 *dataLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

