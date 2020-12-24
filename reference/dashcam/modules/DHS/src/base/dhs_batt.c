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
#include "dhs_type.h"
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

#define GAUGE_DEV    ("/dev/gauge")

static BattService *battService = NULL;
static int gauge_fd = 0;
struct itimerval itv;
static pthread_t gauge_read_pid;
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

static int _getChargeState(int * level){
    U32 s32Level = 0;
    int ret = ioctl(gauge_fd, GAUGE_ChargeState, &s32Level);
    if(DHS_FAILURE == ret)
    {
        printf("get charge error\n");
        return DHS_FAILURE;
    }

    *level = s32Level;
    return DHS_SUCCESS;
}

static int _getLevel(int * level){
    U32 s32Level = 0;
    int ret = ioctl(gauge_fd, GAUGE_CAPACITY, &s32Level);
    if(DHS_FAILURE == ret)
    {
        printf("get Level error\n");
        return DHS_FAILURE;
    }
    *level = s32Level;
    if(s32Level<GAUGE_VALUE_LOW) {
        battService->isLowState = 1;
    }else
    {
        battService->isLowState = 0;
    }

    return DHS_SUCCESS;
}
static int _getVOL(int * level){
    U32 s32Level = 0;
    int ret = ioctl(gauge_fd, GAUGE_VOLTGE, &s32Level);
    if(DHS_FAILURE == ret)
    {
        printf("get VOL error\n");
        return DHS_FAILURE;
    }

    *level = s32Level;
    return DHS_SUCCESS;
}
static void *gauge_read_thread(void * para)
{ 
    int Do_gauge_flag=1; 
    while (Do_gauge_flag) 
    {
            int temp_level=battService->level;
        _getChargeState(&battService->chargeState);
        _getLevel(&battService->level);
        _getVOL(&battService->vol);
 //       printf("battService %d   %d\r\n",battService->level,battService->vol);
        if(temp_level!=battService->level)
        {
            temp_level=battService->level;
            printf("battService %d   %d\r\n",battService->level,battService->vol);
        }
        if(battService->level<5)
        {
            sleep(3);    
            HI_SYSTEM_Poweroff();
        }
        //Delay();
        sleep(5);
    }
     return NULL;
}
int _open(){
    printf("!!!!!!!!!!!!!!!gauge opening!!!!!!!!!! \n");

    gauge_fd = open(GAUGE_DEV, O_RDWR);
    if (gauge_fd == 0)
    {
        printf("open gauge failed\n");
        return DHS_FAILURE;
    }
     _getChargeState(&battService->chargeState);
    _getVOL(&battService->vol);
    _getLevel(&battService->level);
   
  //  bat_timer();
  //开启接受数据线程
    if(pthread_create(&gauge_read_pid, 0, gauge_read_thread,(void *)gauge_fd) == -1){
        return DHS_FAILURE;
   }
    return DHS_SUCCESS;
}


void bat_timer_signal_handler(int m)
{
    int temp_level=battService->level;
    _getChargeState(&battService->chargeState);
    _getLevel(&battService->level);
    _getVOL(&battService->vol);
 //   printf("battService %d   %d\r\n",battService->level,battService->vol);
    if(temp_level!=battService->level)
    {
        temp_level=battService->level;
        printf("battService %d   %d\r\n",battService->level,battService->vol);
    }
    alarm(5);
	// if(isLow && !isCs){
    //     int ret;
    //     SAMPLE_SYS_GetReg(0x12146400,&ret); 
    //     //out
    //     ret &=0xFe;
    //     ret |=0x01;
    //     SAMPLE_SYS_SetReg(0x12146400,ret);

    //     SAMPLE_SYS_SetReg(0x12146004,1);
    //     msSleep(500000);
    //     SAMPLE_SYS_SetReg(0x12146004,0);
    //     msSleep(500000);
    //     ret &=0xFe;
    //     SAMPLE_SYS_SetReg(0x12146400,ret);
    //     //in
    // }
}
BattService *getBattService(){
    //omp_set_lock(&lock);
    if(battService != NULL) {
        //omp_unset_lock(&lock);
        return battService;
    } else {
        battService = (BattService*)malloc(sizeof(BattService));
       
        battService->open = _open;
        battService->vol = 0;
        battService->level = 0;
        battService->chargeState = 0;
        battService->isLowState = 0;
        assert(battService != NULL);
        //omp_unset_lock(&lock);
        return battService;
    }
    
}
