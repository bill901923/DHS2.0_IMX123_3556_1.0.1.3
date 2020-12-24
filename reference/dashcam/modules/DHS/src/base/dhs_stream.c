#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "dhs_type.h"
#include "dhs_stream.h"
#include "sample_comm.h"

static StreamService *streamService = NULL;
static pthread_t st_pid;

static int rg = 0;
static int grg = 0;
static int gbg = 0;
static int abg = 0;

static void *run_stream(void* p){
    if(streamService->state==ST_DISENABLEDING){
        while (1)
        {
            usleep(900000);
            break;
        }
    }
    streamService->state=ST_ENABLEDING;
    //SAMPLE_SYS_SetReg(0x12040038,1);
    //重置awb
    DHS_ISP_RESET();
    //system("./ittb_stream imx290&");
    while (1)
    {
        usleep(1600000);
        break;
    }
    printf("::set awb-> rg=%d;grg=%d;gbg=%d;abg=%d;\n",rg,grg,gbg,abg);
  //  DHS_AWB_1(rg,grg,gbg,abg);
    streamService->state=ST_ENABLED;
    return NULL;
}
int st_open(){
    if(streamService->state==ST_ENABLED || streamService->state==ST_ENABLEDING) return 0;
    if (pthread_create(&st_pid, NULL, run_stream, NULL) != 0){
        perror("pthread_create");
        return DHS_FAILURE;
    }
    return 0;
}
int st_setAWB(int rgain,int grgain,int gbgain,int abgain){
    rg = rgain;
    grg = grgain;
    gbg = gbgain;
    abg = abgain;
    return 0;
}

static void *close_stream(void* p){
    popen("killall ittb_stream","r");
    while (1)
    {
        sleep(1);
        break;
    }
    //SAMPLE_SYS_SetReg(0x12040038,0);
    //SAMPLE_SYS_SetReg(0x12140400,0x80);
   // SAMPLE_SYS_SetReg(0x12140200,0);
    //SAMPLE_SYS_SetReg(0x12040038,1);
    streamService->state=ST_DISENABLED;
    return NULL;
}

int st_close(){
    if(streamService->state==ST_DISENABLED || streamService->state==ST_DISENABLEDING ) return 0;
    streamService->state=ST_DISENABLEDING;
    static pthread_t st_pid2;
    if (pthread_create(&st_pid2, NULL, close_stream, NULL) != 0){
        perror("pthread_create");
        return DHS_FAILURE;
    }
    return 0;
}
StreamService* getStreamService(){
    //omp_set_lock(&lock);
    if(streamService != NULL) {
        //omp_unset_lock(&lock);
        return streamService;
    } else {
        streamService = (StreamService*)malloc(sizeof(StreamService));
		streamService->open = st_open;
		streamService->close = st_close;
        streamService->setAWB = st_setAWB;
        streamService->state = ST_INIT;
        assert(streamService != NULL);
        //omp_unset_lock(&lock);
        return streamService;
    }
}