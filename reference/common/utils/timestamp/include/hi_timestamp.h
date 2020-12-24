/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: timestamp log header file.
 * Author: Camera Reference Develop Team
 * Create: 2017-12-18
 * History: 2017-12-18 create this file
 *          2019-06-28 add HI_TimeStamp_Init, adapt timer cksel
 */

#ifndef __HI_TIMESTAMP_H__
#define __HI_TIMESTAMP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/** performance tiem stamp */

/** \addtogroup     TimeStamp */
/** @{ *//** <!-- [TimeStamp] */

/** TimeStamp log funtions */
HI_S32 HI_TimeStamp_Init(HI_U64 phyMemAddr, HI_BOOL cleanMem);
HI_S32 HI_TimeStamp_Mark(const HI_CHAR *func, HI_U32 line, HI_U32 type);
HI_S32 HI_TimeStamp_Clean(HI_VOID);
HI_S32 HI_TimeStamp_Print(HI_U32 type);
HI_U32 HI_TimeStamp_Getms(HI_VOID);

/** TimeStamp log macro define */
#ifdef CFG_TIME_STAMP_ON
#define HI_TIME_STAMP HI_TimeStamp_Mark(__func__, __LINE__, 0);
#define HI_TIME_CLEAN HI_TimeStamp_Clean();
#define HI_TIME_PRINT HI_TimeStamp_Print(0);
#else
#define HI_TIME_STAMP
#define HI_TIME_CLEAN
#define HI_TIME_PRINT
#endif
#ifdef CFG_TIME_MEASURE_ON
#define HI_PrintBootTime(string) \
    do { \
        if (string != NULL){   \
            MLOGI(YELLOW "%s: %u ms" NONE "\n", string, HI_TimeStamp_Getms()); \
        } else {   \
            MLOGI(YELLOW "%u ms" NONE "\n", HI_TimeStamp_Getms()); \
        } \
    }while(0)

#else
#define HI_PrintBootTime(string)
#endif


#ifdef CFG_PERFORMANCE_TIME_STAMP
#define HI_PERFORMANCE_TIME_STAMP HI_TIME_STAMP
#else
#define HI_PERFORMANCE_TIME_STAMP
#endif

/** @} *//** <!-- ==== TimeStamp End ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef __HI_TIMESTAMP_H__ */

