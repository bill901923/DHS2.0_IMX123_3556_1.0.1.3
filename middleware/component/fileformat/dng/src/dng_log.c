#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "dng_log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static DngLogLevelE g_senDngLogLevel = DNG_LOG_LEVEL_ERR;
static DNG_OUTPUT_FUNC g_spfnDngOutput = HI_NULL;

HI_S32 DNG_LOG_SetEnabledLevel(DngLogLevelE level)
{
    g_senDngLogLevel = level;
    return HI_SUCCESS;
}

HI_S32 DNG_LOG_SetOutputFunc(DNG_OUTPUT_FUNC pFunc)
{
    g_spfnDngOutput = pFunc;
    return HI_SUCCESS;
}

HI_S32 DNG_LOG_Printf(const HI_CHAR *modName, DngLogLevelE level, const HI_CHAR *fmt, ...)
{
    if (level < g_senDngLogLevel) {
        return HI_SUCCESS;
    }

    if (!modName || !fmt) {
        return HI_FAILURE;
    }

    va_list args;
    if (!g_spfnDngOutput) {
        printf("[%s] ", modName);
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    } else {
        g_spfnDngOutput("[%s] ", modName);
        va_start(args, fmt);
        g_spfnDngOutput(fmt, args);
        va_end(args);
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
