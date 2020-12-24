#ifndef __DHS_BATT_H__
#define __DHS_BATT_H__


#define GAUGE_VALUE_MAX              (100)
#define GAUGE_VALUE_LOW              (10)
#define GAUGE_VALUE_MIN              (2)
#define GAUGE_CHENGE_TIMER           (5)

#define REC_DATA_LEN  512

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dhs_BattService{
    int (*open)();
    int level;
    int vol;
    int chargeState; 
    int isLowState;  
} BattService;

BattService* getBattService();

void bat_timer_signal_handler(int m);
#ifdef __cplusplus
}
#endif
#endif //__DHS_BATT_H__