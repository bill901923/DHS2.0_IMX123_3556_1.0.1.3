#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>        //pause()
#include <signal.h>        //signal()
#include "sample_comm.h"
#include "TEST_app.h"
#include "TEST_batt.h"
#include "dictionary.h"
#include "iniparser.h"
#include "utils.h"
#include "hi_system.h"

static Appcontext *app_context = NULL;
void ble_process(char *data);
void ble_chage_state(BLE_STATE bs);
int init_batt_service();
int init_ble_service();
int init_service();
int init_TEST_app();
int  check_num(char *data);
void signalHandler(int signo);

static int str_match(const char *a, const char *b);

time_t start, now;
BLEService * ble_service;

int  check_num(char *data)
{
    int i = atoi(data);
    char temp[10] ;
    sprintf(temp,"%d",i);
   // printf("%s  = %s \r\n",temp,data);
    for(int j=0;j<strlen(data);j++)
    {

        if((data[j]==0x0a)||(data[j]==0x0D))
        {
            data[j]=0;
        }

    }

    if (strlen(temp) == strlen(data))
    {
        return 1 ;
    }
    else
    {
        printf("Not Num %d %d\r\n",strlen(temp),strlen(data));
        return 0;
    }
}

static int str_match(const char *a, const char *b)
{
    return strncmp(a, b, strlen(b)) == 0;
}
int init_batt_service(){
    BAT_open();  
    time(&start);
   /* if(ret!=DHS_SUCCESS){
        //关机/重启
        PowerOff("init_batt_service");
    }*/
    signal(SIGALRM, signalHandler);

    struct itimerval new_value, old_value;
    new_value.it_value.tv_sec = 2;
    new_value.it_value.tv_usec = 0;
    new_value.it_interval.tv_sec = 3;
    new_value.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &new_value, &old_value);

    return 1;
}

void signalHandler(int signo)
{
            char timedata[20]={0};
            char timeTTE[20]={0};
            char timeTTF[20]={0};
            int time_h=0,time_m=0,time_s=0;   
            test_time temp_time;
           // printf("start =%ld\r\n",start);
    switch (signo){
        case SIGALRM:         
            time(&now);
           // printf("now =%ld\r\n",now);
            now=now-start;
            //printf("now =%ld\r\n",now);
            time_s=now%60;
            time_m=(now/60)%60;
            time_h=(now/3600);
            sprintf(timedata,"%02d:%02d:%02d",time_h,time_m,time_s);
            printf("%s :bat_vol:%d; chargeState=%d; level:%d CURRENT:%f mA\n",timedata,BAT_getVOL(),BAT_getChargeState(),BAT_getLevel(),BAT_getCURRENT());
            temp_time=BAT_getTTF();

            sprintf(timeTTF,"%02d:%02d:%02d",temp_time.time_h,temp_time.time_m,temp_time.time_s);
            temp_time=BAT_getTTE();
            sprintf(timeTTE,"%02d:%02d:%02d",temp_time.time_h,temp_time.time_m,temp_time.time_s);
            printf("%s :OPbat_vol:%d; FULL=%s; Empty:%s \n",timedata,BAT_getOPVOL(),timeTTF,timeTTE);
            

            break;
   }
}
int init_ble_service(){
     printf("********init_ble_service********\n");
    BleState *bleState = malloc(sizeof(BleState));
    bleState->process=ble_process;
    bleState->changeState = ble_chage_state;
    ble_service = getBleService();
    ble_service->isSetName = app_context->conf_info->global_init;
    char w_buf[30];
    sprintf(w_buf, "%s#%s\r\n",app_context->conf_info->dev_name,app_context->conf_info->dev_serial);
    ble_service->name = (char *)malloc(sizeof(strlen(w_buf)));
    strcpy(ble_service->name,w_buf);
    return ble_service->open(bleState,app_context->conf_info);
}


/******************************************************************************
* function : ble_chage_state ble状态回调
******************************************************************************/
void ble_chage_state(BLE_STATE bs){
  //  if(BLE_STATE_temp!=bs)
  //  {

        if(bs == BLE_CON_SUCCESS){

            printf("bs BLE_CON_SUCCESS\n");
            app_context->cli_info->status = ON_LINE;
            hi_usleep(1000000);
            ble_service->sendMsg("SUCCESS\r\n",9,0);      
            
        }else if(bs == BLE_CON_STOP){

            printf("bs BLE_CON_STOP\n");
            app_context->cli_info->status = OFF_LINE;
        }
   //     BLE_STATE_temp=bs;
  //  }
    return;
}
int init_service(){
    printf("================ DHS1500 SERVICE INIT ===================\n");
    int ret;
    //ret = init_batt_service();
    ret = init_ble_service();
    if(ret !=DHS_SUCCESS)  
    {
    printf("init_ble_service Fail\n");
    }

    //pthread_join(sys_service_pid,NULL);
    while (1);
   // system("reboot");
    return 0;
}

void PQTOOLSET(uint8_t set_flag)
{
    HI_BOOL PQFLAG = HI_TRUE;
    HI_BOOL PQFLAG_ck  ;
    HI_PDT_PARAM_Init();
    if(set_flag){   PQFLAG=HI_TRUE; }
    else        {   PQFLAG=HI_FALSE;    }

    HI_PDT_PARAM_GetCommParam(DHS_PARAM_TYPE_PQTOOL, &PQFLAG_ck);
    printf("PQFLAG_ck %d\r\n",PQFLAG_ck);
    HI_PDT_PARAM_SetCommParam(DHS_PARAM_TYPE_PQTOOL, &PQFLAG);
    printf("PQFLAG %d\r\n",PQFLAG);
    HI_PDT_PARAM_GetCommParam(DHS_PARAM_TYPE_PQTOOL, &PQFLAG_ck);
    printf("PQFLAG_ck %d\r\n",PQFLAG_ck);
}

int main(int argc, const char *argv[])
{
    uint8_t do_while=0;
    printf("================ TEST APP ===================\n\n");
    //判断客户端信息
        
    printf("argc :%d\n",argc);
    printf("Get2 %s\n",argv[1]);
    printf("Get3 %s\n",argv[2]);  
    if (strcmp(argv[1], "BAT")==0)
    {    int ret;
        ret = init_batt_service();
        if(ret !=0)  
        {
            printf("init_batt_service Fail\n");
        }
        else
        {
            do_while=1;
        }
    }       
    else if (strcmp(argv[1], "Test")==0)
    {    int ret;
        init_TEST_app();  
        ret = init_service();
        if(ret !=0)  
        {
            printf("init_ble_service Fail\n");
        }
        else
        {
            do_while=1;
        }
    }        
    else if (strcmp(argv[1], "PQ")==0)
    {     
        if (strcmp(argv[2], "open")==0) 
        {
            PQTOOLSET(1);
             HI_system("sed -i \"s;^./main_app;#./main_app;\" /app/bootapp");
             HI_system("reboot");

        }        
        else if (strcmp(argv[2], "close")==0)
        {  
            PQTOOLSET(0);
             HI_system("sed -i \"s;^#./main_app;./main_app;\" /app/bootapp");
             HI_system("reboot");
        }
        else
        {

            printf("open or close\n");m
        }

    }      
    else if (strcmp(argv[1], "OUT")==0)
    {    
       HI_SYSTEM_Poweroff();
    }  

    while (do_while) 
    {
    }
    return 0;
}

/******************************************************************************
* function : ble_process 处理蓝牙信息
******************************************************************************/
void ble_process(char *data){
    printf("ble test read data:%s \n",data); 
    
    if (str_match(data, "LED"))
    {
        printf("data =%s\r\n",data);
         //stream_service->open();
         char *ptmp;
         ptmp = (char *)data;
         char *pch;
	     pch = strsep(&ptmp, ",");
	     pch = strsep(&ptmp, "\\");
         int val=0;
         if(strlen(pch)>2)
         {
             if(check_num(pch))
             {
                val=atoi(pch);
             }
         }
         
        if (str_match(data, "LED=1")) 
        {
            cutA_close();
            cutB_close();
            
            led_wl2_open(val);
           // led_wl1_open(app_context->conf_info->pl_led);
            ble_service->sendMsg("LED=OK\r\n",8,0);
        }
        else if (str_match(data, "LED=2"))
        {
            cutA_open();
            cutB_close();
          //  led_wl2_open(app_context->conf_info->wl_led);
            led_wl1_open(val);
            ble_service->sendMsg("LED=OK\r\n",8,0);
        }
        else if (str_match(data, "LED=3"))
        {
           cutA_close();
            cutB_open();
           // led_wl2_open(app_context->conf_info->wl_led);
            led_wl1_open(val);
            ble_service->sendMsg("LED=OK\r\n",8,0);
        }
        else if (str_match(data, "LED=4"))
        {
            cutA_close();
            cutB_close();
            led_uv_open(val);
            ble_service->sendMsg("LED=OK\r\n",8,0);
        }
       /* else if (str_match(data, "LED=5"))
        {
            DHS_UV_AWB(app_context->conf_info->uv_awb_rgain,app_context->conf_info->uv_awb_grgain,app_context->conf_info->uv_awb_gbgain,app_context->conf_info->uv_awb_bgain);
            DHS_ISP_UV(app_context->conf_info->uv_ae_exptime, app_context->conf_info->uv_ae_ispgain, app_context->conf_info->uv_ae_again, app_context->conf_info->uv_ae_dgain);
            led_uv_open(app_context->conf_info->uv_led);
            ble_service->sendMsg("LED=OK\r\n",8,0);
        }*/
        else if (str_match(data, "LED=0"))
        {
            led_all_close();
            DHS_AUTO_AWB();
            DHS_AUTO_ISP();
           // stream_service->close();
            //关闭led
            ble_service->sendMsg("LEDC=OK\r\n",9,0);
        }
        else
        {
            printf("ble_process Get %s \r\n",data);
        }
        
        
        return;
    }
    return;
}

int init_TEST_app(){
    printf("init_TEST_app \n");
    app_context = malloc(sizeof(Appcontext));
    app_context->conf_info = malloc(sizeof(_config_info));
    app_context->dev_info = malloc(sizeof(device_info));
    app_context->cli_info = malloc(sizeof(client_info));
    //init_config();
    //init_device();
    //init_drive();
    return 0;
}