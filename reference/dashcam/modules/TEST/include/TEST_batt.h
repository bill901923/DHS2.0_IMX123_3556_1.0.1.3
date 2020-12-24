#ifndef __TEST_BATT_H__
#define __TEST_BATT_H__


#include "hi_type.h"
#define GAUGE_VALUE_MAX              (100)
#define GAUGE_VALUE_LOW              (10)
#define GAUGE_VALUE_MIN              (2)
#define GAUGE_CHENGE_TIMER           (5)

#define REC_DATA_LEN  512

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _TEST_time
{
    uint time_h;
    uint time_m;
    uint time_s;
}test_time;


int BAT_open();
int BAT_getVOL();
int BAT_getLevel(); 
int BAT_getChargeState();
HI_FLOAT BAT_getCURRENT();
test_time BAT_getTTF();
test_time BAT_getTTE();
int BAT_getOPVOL();
#ifdef __cplusplus
}
#endif
#endif //__DHS_BATT_H__