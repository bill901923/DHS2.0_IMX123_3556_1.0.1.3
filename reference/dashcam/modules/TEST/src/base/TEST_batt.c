#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <signal.h>
#include <string.h>
//#include <omp.h>

#include "dhs_batt.h"
#include "TEST_batt.h"
#include "dhs_type.h"
#include "dhs_app.h"
#include "sample_comm.h"

#include "hi_system.h"
#include "hi_hal_common_inner.h"

#include "hi_type.h"
#include "hi_hal_common.h"

//#define SENSITIVITY             _IOWR('w', 4, unsigned int)
//#define GAUGE_CAPACITY   _IOWR('r', 5, unsigned int)
//#define GAUGE_CHARGING   _IOWR('r', 6, unsigned int)
//#define GAUGE_VOL   _IOWR('r', 7, unsigned int)


#define GAUGE_VOLTGE        _IOWR('r', 5, unsigned int)
#define GAUGE_CAPACITY      _IOWR('r', 6, unsigned int)
#define GAUGE_CONFIG_Read   _IOWR('r', 7, unsigned int)
#define GAUGE_ChargeState   _IOWR('r', 8, unsigned int)
#define GAUGE_CURRENT       _IOWR('r', 9, unsigned int)
#define GAUGE_OPEN_VOLTGE   _IOWR('r',10, unsigned int)
#define GAUGE_FULL_TIME     _IOWR('r',11, unsigned int)
#define GAUGE_EMPTF_TIME    _IOWR('r',12, unsigned int)

#define GAUGE_DEV    ("/dev/gauge")

static int gauge_fd = 0;
struct itimerval itv;
//static int authening =0;
//static int isCs = 0;


void bat_uninit_timer()  
{   
    itv.it_value.tv_sec = 0;  
    itv.it_value.tv_usec = 0;  
    itv.it_interval = itv.it_value;  
    setitimer(ITIMER_REAL, &itv, NULL);  
}

int bat_timer()
{
	signal(SIGALRM, bat_timer_signal_handler);
	
	//itv.it_interval.tv_sec = GAUGE_CHENGE_TIMER;
	//itv.it_interval.tv_usec = 0;
	//itv.it_value.tv_sec = GAUGE_CHENGE_TIMER;
	//itv.it_value.tv_usec = 0;
	//itv_num = 0;
	//setitimer(ITIMER_REAL, &itv, NULL);

    alarm(5);
	return 0;
}

 int BAT_getChargeState(){
    U32 s32Level = 0;
    int ret = ioctl(gauge_fd, GAUGE_ChargeState, &s32Level);
    if(DHS_FAILURE == ret)
    {
        printf("get charge error\n");
        return DHS_FAILURE;
    }
    return s32Level;
}


HI_FLOAT BAT_getCURRENT() /* percent*/
{
    U32 s32CURRENT = 0;
    HI_FLOAT F_CURRENT=0;
    int ret = ioctl(gauge_fd, GAUGE_CURRENT, &s32CURRENT);
    if(DHS_FAILURE == ret)
    {
        printf("get CURRENT error\n");
        return DHS_FAILURE;
    }
   //mA  1bit= 156.25 uA 2'
   // printf("s32CURRENT:0x%04x \n",s32CURRENT);
    if(s32CURRENT>=0x8000)
    {
        F_CURRENT=0xFFFF-s32CURRENT;
        F_CURRENT*=-5;
        F_CURRENT/=32;
    }
    else
    {
        F_CURRENT=s32CURRENT;
        F_CURRENT*=5;
        F_CURRENT/=32;
    }

    return F_CURRENT;   
}

 int BAT_getLevel(){
    U32 s32Level = 0;
    int ret = ioctl(gauge_fd, GAUGE_CAPACITY, &s32Level);
    if(DHS_FAILURE == ret)
    {
        printf("get Level error\n");
        return DHS_FAILURE;
    }

    return s32Level;
}
 int BAT_getVOL(){
    U32 s32Level = 0;
    int ret = ioctl(gauge_fd, GAUGE_VOLTGE, &s32Level);
    if(DHS_FAILURE == ret)
    {
        printf("get VOL error\n");
        return DHS_FAILURE;
    }
    return s32Level;
}
 test_time BAT_getTTF(){
    U32 s32Level = 0;
    test_time temp_time;
    int ret = ioctl(gauge_fd, GAUGE_FULL_TIME, &s32Level);
    if(DHS_FAILURE == ret)
    {
        printf("get VOL error\n");
    }

    temp_time.time_s=s32Level%60;
    temp_time.time_m=(s32Level/60)%60;
    temp_time.time_h=(s32Level/3600);
    return temp_time;
}
 test_time BAT_getTTE(){
    U32 s32Level = 0;
    test_time temp_time;
    int ret = ioctl(gauge_fd, GAUGE_EMPTF_TIME, &s32Level);
    if(DHS_FAILURE == ret)
    {
        printf("get BAT_getTTE error\n");
    }

    temp_time.time_s=s32Level%60;
    temp_time.time_m=(s32Level/60)%60;
     temp_time.time_h=(s32Level/3600);
    return temp_time;
}

 int BAT_getOPVOL(){
    U32 s32Level = 0;
    int ret = ioctl(gauge_fd, GAUGE_OPEN_VOLTGE, &s32Level);
    if(DHS_FAILURE == ret)
    {
        printf("get VOL error\n");
        return DHS_FAILURE;
    }
    return s32Level;
}

int BAT_open(){
    HI_system("/app/komod");
    HI_system("insmod /app/komod/MAX_17201.ko");
    printf("!!!!!!!!!!!!!!!gauge opening!!!!!!!!!! \n");

    gauge_fd = open(GAUGE_DEV, O_RDWR);
    if (gauge_fd == 0)
    {
        printf("open gauge failed\n");
        return DHS_FAILURE;
    }

    printf("bat_vol  :%d; chargeState=%d; level:%d \n",BAT_getVOL(),BAT_getChargeState(),BAT_getLevel());
    

    return 0;
}


