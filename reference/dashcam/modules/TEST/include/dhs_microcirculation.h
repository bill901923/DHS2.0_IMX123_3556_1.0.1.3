
#ifndef __DHS_MCL_H__
#define __DHS_MCL_H__

#include "dhs_uart.h"
#include "dhs_type.h"
#include "sample_comm.h"
#include "dhs_ble.h"
#include "utils.h"
#define BLOOD_UART_DEV  "/dev/ttyAMA3"
#define REC_DATA_LEN  512
#define DHS_GIPO_FOR_MCLWORK  2              //
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    MCL_ENABLED,            //开启
    MCL_DISENABLED,         //关闭
} MCL_STATE;

typedef struct dhs_MCLService{
    MCL_STATE mcl_state;
    int (*open)();
    int (*close)();
    int (*reset)();
} MicrocirculationService;

MicrocirculationService* getMicrocirculationService();

int blood_open();
int blood_close();
int blood_reset();
#ifdef __cplusplus
}
#endif
#endif //__DHS_MCL_H__

