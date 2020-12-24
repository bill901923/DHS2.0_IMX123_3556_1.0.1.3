#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
//#include<linux/msg.h>

#include "dhs_isp.h"
#include "dhs_led.h"
#include "dhs_app.h"
#include "dhs_batt.h"
#include "dhs_socket.h"
#include "dhs_ble.h"
#include "dhs_type.h"
#include "dhs_stream.h"
#include "../wifi/dhs_wifi_service.h"
#include "dhs_microcirculation.h"
#include "utils.h"
#include "sample_comm.h"
#include "hi_hal_gauge.h"
#include "dictionary.h"
#include "iniparser.h"

BLEService * ble_service;
StreamService *stream_service;
WifiService* wifi_service;
SocketService* socket_service;
MicrocirculationService* blood_service;
BattService * batt_service;
static DEV_STATE app_run = DEV_SLEEP;
struct timeval tv;
Appcontext *app_context;

BLE_STATE BLE_STATE_temp = BLE_CON_STOP;

void ble_process(char *data);
void ble_chage_state(BLE_STATE bs);

void app_sleep();
void app_work();

UI_shm_memory *UIshmdata;
void set_config_SIG(char *iten ,char *iten2,char* val);
void DHSUI_shm( int item,int cast)
{
    int shmid;
	void *shm = NULL;
	key_t key = ftok(UISHM_NAME, 'k');


	shmid = shmget(key, sizeof(UI_shm_memory), 0666|IPC_CREAT);
    if(shmid == -1)
	{printf( "DHS_shm shmget failed. ");exit(-1);}
	shm = shmat(shmid, 0, 0);
	if(shm == (void *)-1)
	{
		printf( "DHS_shm shmat failed. ");
		exit(-1);
	}
	//printf( "DHS_shm UI_shm_memory at 0x%X \r\n",(unsigned int) shm);
    UIshmdata = (UI_shm_memory *)shm;
    
 
   // printf( "UDHSUI_shm %d %d\r\n",item,cast);
    switch(item)
    {
        case 0:
            UIshmdata->MSG_START=cast;
        break;

        case 1:
            UIshmdata->MSG_ICON_p2p=cast;
        break;

        case 2:
            UIshmdata->MSG_ICON_LEDState=cast;
        break;
    }
    
    

	//printf( "UIshmdata->MSG_START %d %d\r\n",UIshmdata->MSG_START,cast);
    
    if(shmdt(shm) == -1)
	{
		printf( "shmdt failed. ");
		exit(-1);
	}
}
void DHSUI_shm_check( )
{
    int shmid;
	void *shm = NULL;
	key_t key = ftok(UISHM_NAME, 'k');


	shmid = shmget(key, sizeof(UI_shm_memory), 0666|IPC_CREAT);
    if(shmid == -1)
	{printf( "DHS_shm shmget failed. ");exit(-1);}
	shm = shmat(shmid, 0, 0);
	if(shm == (void *)-1)
	{
		printf( "DHS_shm shmat failed. ");
		exit(-1);
	}
	//printf( "DHS_shm UI_shm_memory at 0x%X \r\n",(unsigned int) shm);
    UIshmdata = (UI_shm_memory *)shm;
    
 
   // printf( "UDHSUI_shm %d %d\r\n",item,cast);
     if(UIshmdata->MSG_ICON_LEDState>0)
        {
            led_all_close();
            DHS_AUTO_AWB();
            DHS_AUTO_ISP();
            DHSUI_shm(2,0);
        }
    

	//printf( "UIshmdata->MSG_START %d %d\r\n",UIshmdata->MSG_START,cast);
    
    if(shmdt(shm) == -1)
	{
		printf( "shmdt failed. ");
		exit(-1);
	}
}
void PowerOff(char* strdata)
{
  printf("PowerOff by %s\n",strdata);
  //      himm(0x12090000,0x00010162);
}
 int str_match(const char *a, const char *b)
{
    return strncmp(a, b, strlen(b)) == 0;
}
//电池服务
int init_batt_service(Appcontext *c){
    int ret = DHS_FAILURE;
    int timeflag=0;
    batt_service = getBattService();

    while((ret == DHS_FAILURE)&(timeflag<3))
    {   
        timeflag++;
        ret = batt_service->open();
        if(ret!=DHS_SUCCESS)
        {
            msSleep(10000);
        }
    }
    
    
    if(ret!=DHS_SUCCESS){
        //关机/重启
        PowerOff("init_batt_service");
    }
    printf("bat_vol:%d; chargeState=%d; level:%d; \n",batt_service->vol,batt_service->chargeState,batt_service->level);
    if(batt_service->chargeState == DHS_FALSE ){
        if((batt_service->isLowState || batt_service->vol<3.4) && !batt_service->chargeState){
            //电量过低 关机
            PowerOff("init_batt_service Low power");
            return -1;
        }
    }
    return 0;
}
int wifiChengeStateCallBack(WIFI_STATE wifiState){
    printf("wifi chenge state call back ....................\n");
    if(wifiState==WIFI_STATE_ENABLED){
        //开启ble广播
        ble_service->work();
        if(app_context->cli_info->type==ANDROID){
            wifi_service->wifiP2pManager->p2pFind();
        }
       
    }else{
        //重启

         printf("reboot by wifiChengeStateCallBack fail \r\n");
        system("reboot");
    }
    return 0;
}
int netChengeStateCallBack(NET_STATE wifiState){

    if(wifiState==UNKNOWN){
        ble_service->close();
         printf("reboot by netChengeStateCallBack fail \r\n");
        system("reboot");
        return 0;
    }

    if(app_context->cli_info->status != NET_CONNING){
        return 0;
    }
    if(app_context->cli_info->type == ANDROID){
        if(wifiState == COMPLETED){
            //设置ＩＰ
            char ip[17], dns[17];
            WIFI_BOOL ret = wifi_service->setIP(ip,dns);
            if(ret == WIFI_FALSE){
                app_context->cli_info->status = NET_CONNED;
                //IP设置失败
                ble_service->sendMsg("P2PE=6\r\n",8,0);
            }else{
                //彻底连接成功
                strcpy(app_context->cli_info->ip, dns);
                strcpy(app_context->dev_info->ip, ip);
                strcpy(app_context->dev_info->dns, dns);
                char buf[30];
                sprintf(buf, "P2PC=%s\r\n", ip);
                app_context->cli_info->status = NET_CONNED;
                ble_service->sendMsg(buf,strlen(buf),0);

            }
        }else if(wifiState == DISCONNECTED){
            //app_context->cli_info->status = NET_CONNED;
            //p2p链接断开
            //ble_service->sendMsg("P2PE=1\r\n",8,0);

        }else if(wifiState == FAILED){
            app_context->cli_info->status = NET_CONNED;
            //p2p拒绝链接
            ble_service->sendMsg("P2PE=7\r\n",8,0);

        }else if(wifiState == UNAVAILABLE){
            app_context->cli_info->status = NET_CONNED;
            //p2p构建组失败/认证失败
            printf("UNAVAILABLE \r");
            ble_service->sendMsg("P2PE=8\r\n",8,0);
        }else if(wifiState == AUTHENTICATTIMEOUT){
            app_context->cli_info->status = NET_CONNED;
            //链接等待超时
            printf("AUTHENTICATTIMEOUT \r");
           ble_service->sendMsg("P2PE=8\r\n",8,0);
            
        }
        
    }else if(app_context->cli_info->type == IOS){

    }
    
    return 0;
}
void socket_state_chenge(SOCK_STATE sock_state){
    if(sock_state == SOCK_CON_SUCCESS){
        app_work();
    }
    return;
}
int init_socket_service(Appcontext *c){
    socket_service = getSocketService();
    SockState *ss = malloc(sizeof(SockState));
    ss->change_state = socket_state_chenge;
    socket_service->open(ss);
    return 0;
}

int init_wifi_service(Appcontext *c){
    WifiContext *w_context = (WifiContext*)malloc(sizeof(WifiContext));
    w_context->wifiChengeStateCallBack = wifiChengeStateCallBack;
    w_context->netChengeStateCallBack = netChengeStateCallBack;
    int ret = WIFI_TRUE;
    wifi_service = getWifiService(w_context);
    printf("app_context->dev_info->key_mod  %d \n",app_context->dev_info->key_mod);
    printf("app_context->conf_info->wifimod  %d \n",app_context->conf_info->wifimod);
    if(app_context->dev_info->key_mod == ANDROID){
        app_context->cli_info->type = ANDROID;
       if(app_context->conf_info->wifimod>0)
       {
            ret = wifi_service->open(P2P);
        }
        printf("********app work mode is android********\n");
    }else{
        ret = app_context->cli_info->type = IOS;
        wifi_service->open(AP);
        printf("********app work mode is ios********\n");
    }
    if(ret !=WIFI_TRUE){
        //关机
        PowerOff("init_wifi_service");
    }
    return 0;
}
int ble_test(){
    ble_service->sendMsg("ble testing... \r\n",18,1);
    return 0;
}
int init_ble_service(Appcontext *c){
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
* function : sys_service 系统服务器
******************************************************************************/
static int work_led = 0;
static void *sys_service(void *para){
 //   Appcontext *p = (Appcontext *)para;
    int con_s = 0;
 //   int Log_s = 0;
 //   int wait_s = 0;
    //BLE_Msg bm;
  //  bm.msgType = 3;

    int shmid;
	void *shm = NULL;
	share_memory *shmdata;
	key_t key = ftok(SHM_NAME, 'k');
	shmid = shmget(key, sizeof(share_memory), 0666|IPC_CREAT);
	if(shmid == -1)
	{printf( "sys_service shmget failed. %d \r\n ",key);exit(-1);}
	shm = shmat(shmid, 0, 0);
	if(shm == (void *)-1)
	{
		printf( "sys_service shmat failed. \r\n");
		exit(-1);
	}
	printf( "sys_service key=%d sharememory at 0x%X \r\n",key,(unsigned int) shm);
	shmdata = (share_memory *)shm;
	shmdata->sockflag = 0; 
    shmdata->read_sendflag=0;
	shmdata->Sock_open = 0;   

	printf( "sys_service 1\r\n");

    int VolTemp=0;
    HI_U32 GPIOvalue;
    HI_U32 GPIOvalue_old=0;
    himd(0x120D9400, &GPIOvalue);  // 9_1 INPUT 10_6 outPUT
    if ((GPIOvalue & 0x00000002) != 0x00000000)
    {
        GPIOvalue &= 0xFFFFFFFD;
        himm(0x120D9400, GPIOvalue);
    }
    int sockState_old=0;
    while (1)
    {
        //检测电池
    /******************/
        //DHSUI_shm();
    /*******************/
        if(++con_s>12) {
            
            con_s=0; 
            //电池电量
            char buf[10];
            sprintf(buf, "BAT=%d\r\n", batt_service->level);
            if(VolTemp!=batt_service->level)
            {
                printf("%s",buf);
            }
            ble_service->sendMsg(buf,strlen(buf),0);
            //ble_service->sendMsg(buf,strlen(buf));
            if(batt_service->isLowState && !batt_service->chargeState){
                //电量过低 关机
                PowerOff("sys_service Lowpower");
            //    return -1;
            }
        }        
        VolTemp=batt_service->level;
        ++con_s;
        //系统状态 客户端状态 处理
        //==============获取设备是否链接--->更新工作指示灯状态=================

        if(sockState_old!=socket_service->sockState)
        {
            sockState_old=socket_service->sockState;
            switch(sockState_old)
            {
            case SOCK_BROADCAST  :        //开始广播
                printf("sockState SOCK_BROADCAST\r\n");
            break;
            case SOCK_CON_SUCCESS  :      //有链接
                printf("sockState SOCK_CON_SUCCESS\r\n");
            break;
            case SOCK_CON_STOP   :        //链接断开
                printf("sockState SOCK_CON_STOP\r\n");
            break;
            case SOCK_ENABLED     :       //开启
                printf("sockState SOCK_ENABLED\r\n");
            break;
            case SOCK_DISENABLED   :      //关闭
                printf("sockState SOCK_DISENABLED\r\n");
            break;
            }
        }

        if(socket_service->sockState == SOCK_CON_SUCCESS){
            

            if(shmdata->sockflag>0)
            {
                DHSUI_shm(0,(int)DHS_PICTURE_GETTING);
            }
            else if(shmdata->Sock_open==1)
            {
                DHSUI_shm(0,(int)DHS_CLIENT_CONNECTING);
            }
            DHSUI_shm(1,1);
            //指示灯亮起 
            //SAMPLE_SYS_SetReg(0x12145004,0);
            //app_work();  
        }else{

            DHSUI_shm(0,(int)DHS_WAITING_CONNECTION);
            DHSUI_shm(1,0);
            app_sleep();
            if(work_led){
                work_led = 0;
                //SAMPLE_SYS_SetReg(0x12145004,0);
            }else{
                work_led = 1;
               // SAMPLE_SYS_SetReg(0x12145004,1);
            }
            if(app_context->conf_info->wifimod>0)
            {
                DHSUI_shm_check();   
            }       
           

        }

        himd(0x120D9008, &GPIOvalue);//GPIO9_1
       if(((GPIOvalue & 0x00000002)==0)&&(shmdata->Sock_open==1)&&(shmdata->sockflag==0))
        {
            printf("Send to BT PIC\r\n");
            ble_service->sendMsg("PIC\r\n",5,0);
        }

        if(GPIOvalue_old!=GPIOvalue)
        {
            printf("GPIOvalue POTO  0x%02x\r\n",GPIOvalue);
            GPIOvalue_old=GPIOvalue;
        }



        sleep(1);
    }

    if(shmdt(shm) == -1)
	{
		printf( "shmdt failed. ");
		exit(-1);
	}
/*	if(shmctl(shmid, IPC_RMID, 0) == -1)
	{
		printf( "shmdt failed. ");
		exit(-1);
	}*/
    return NULL;
}
int init_blood_service(){
    blood_service = getMicrocirculationService();
    return 0;

}

int init_stream_service(Appcontext *c){
  //  StreamState *sState = malloc(sizeof(StreamState));
    stream_service = getStreamService();
    stream_service->setAWB(app_context->conf_info->wl_awb_rgain,app_context->conf_info->wl_awb_grgain,app_context->conf_info->wl_awb_gbgain,app_context->conf_info->wl_awb_bgain);
    return 0;
}
/******************************************************************************
* function : init_service 初始化服务
******************************************************************************/
int init_service(Appcontext *c){
    printf("================ DHS1500 SERVICE INIT ===================\n");
    DHSUI_shm(0,(int)DHS_DEVICE_INIT);
    app_context = c;
    int ret;
    ret = init_batt_service(c);
    ret = init_ble_service(c);
    if(ret !=DHS_SUCCESS)  
    {
    printf("init_ble_service Fail\n");
        goto ERR;
    }

    ret =init_socket_service(c);
    if(ret !=DHS_SUCCESS)
    {
    printf("init_socket_service Fail\n");
        goto ERR;
    }

    ret = init_wifi_service(c);
    if(ret !=DHS_SUCCESS)
    {
    printf("init_wifi_service Fail\n");
        goto ERR;
    }
    pthread_t sys_service_pid;
    if (pthread_create(&sys_service_pid, NULL, sys_service, (void *)c) != 0)
    {
        perror("pthread_create");
    }
    init_stream_service(c);
    init_blood_service(c);
    //pthread_join(sys_service_pid,NULL);
    while (1);
    return 0;
ERR:
    ble_service->close();
   // system("reboot");
    return 0;
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
            if(app_context->cli_info->type==ANDROID){
                if(app_context->conf_info->wifimod>0)
            {
                wifi_service->wifiP2pManager->p2pFind();
            }
            }
            
        }else if(bs == BLE_CON_STOP){

            printf("bs BLE_CON_STOP\n");
            app_context->cli_info->status = OFF_LINE;
        }
   //     BLE_STATE_temp=bs;
  //  }
    return;
}
/******************************************************************************
* function : p2p_connect p2p 链接
******************************************************************************/
static void *p2p_connect(void *p){
    char *pp = (char *)p;
    if(wifi_service->wifiP2pManager->p2p_net_state !=SCANNING){
	    wifi_service->wifiP2pManager->p2pFind();
	}
    
    int time_out_num = 0;
    //int status;
	while (1)
    {
        //5s 超时
        if(++time_out_num > 20){
            //没有搜索到这个mac地址
            printf("!!!!!!!!!!!!!! not scan mac/name:%s !!!!!!!!!!!!!!!!!!!!!!\n",pp);
            //设备查到到但不可连接 需要手机端刷新p2p
            ble_service->sendMsg("P2PE=3\r\n",8,0);
            app_context->cli_info->status = NET_CONNED;
            wifi_service->wifiP2pManager->p2pFind();
            msSleep(500000);
            return NULL;
        }
        smartlist_t * st = wifi_service->wifiP2pManager->P2PDeviceList;
        int st_len = smartlist_len(st);
        int i = 0;
        for(i=0;i<st_len;i++){
            P2PDeviceInfo *pi = (P2PDeviceInfo *)smartlist_get(st,i);
            printf("=========================================== %s == %s == %s \n",pp,pi->p2p_dev_addr,pi->name);
            if(str_match(pi->name, pp) ||str_match(pi->p2p_dev_addr, pp)){
                WIFI_BOOL ret = wifi_service->wifiP2pManager->p2pConnectWPA(pi->p2p_dev_addr);
                if(ret == WIFI_TRUE)  {
                    msSleep(500000);
                    return NULL;
                }
            }else{
                free(pi);
                smartlist_del(st,i);
            }
        }
        msSleep(1000000);
    }
    return NULL;
}

/******************************************************************************
* function : ble_process 处理蓝牙信息
******************************************************************************/
void ble_process(char *data){
    printf("ble test read data:%s \n",data);
    //BLE_Msg bm;
    //bm.msgType = 3;
    if (str_match(data, "INIT")){

        if(app_context->cli_info->type==ANDROID && app_context->cli_info->status == NET_CONNED){
            //移除p2p
           // wifi_service->wifiP2pManager->p2pRemovedGroup();
        }

        if(app_context->cli_info->type == ANDROID){
             wifi_service->wifiP2pManager->p2pFind();
        }
        //返回设备信息
        char buf[48];
        sprintf(buf, "INIT=%s|%s|%s|%d|%d\r\n",
                app_context->conf_info->dev_version, app_context->conf_info->dev_serial,
                app_context->conf_info->dev_name, app_context->cli_info->type, batt_service->level);
        
        ble_service->sendMsg(buf,strlen(buf),0);
        return ;
    }else if (str_match(data, "BAT"))
    {
        //电池电量
        char buf[10];
        sprintf(buf, "BAT=%d\r\n", batt_service->level);
        ble_service->sendMsg(buf,strlen(buf),0);
        return ;
       // ble_service->sendMsg(buf,strlen(buf));

    }

    else if (str_match(data, "LED"))
    {
         //stream_service->open();
        if (str_match(data, "LED=1")) 
        {
           if(app_context->conf_info->WL_AUTO==0)
           {
                DHS_Set_AWB(app_context->conf_info->wl_awb_rgain,app_context->conf_info->wl_awb_grgain,app_context->conf_info->wl_awb_gbgain,app_context->conf_info->wl_awb_bgain);
            }
            DHS_Set_ISP(app_context->conf_info->wl_ae_exptime,app_context->conf_info->wl_ae_ispgain,app_context->conf_info->wl_ae_again,app_context->conf_info->wl_ae_dgain);
            cutA_close();
            cutB_close();
            
            led_wl2_open(app_context->conf_info->wl_led);
           // led_wl1_open(app_context->conf_info->pl_led);

            ble_service->sendMsg("LED=OK\r\n",8,0);
             DHSUI_shm(2,1);
        }
        else if (str_match(data, "LED=2"))
        {
          if(app_context->conf_info->PL_AUTO==0)
           {
            DHS_Set_AWB(app_context->conf_info->pl_awb_rgain,app_context->conf_info->pl_awb_grgain,app_context->conf_info->pl_awb_gbgain,app_context->conf_info->pl_awb_bgain);
           }
            DHS_Set_ISP(app_context->conf_info->pl_ae_exptime,app_context->conf_info->pl_ae_ispgain,app_context->conf_info->pl_ae_again,app_context->conf_info->pl_ae_dgain); 
            cutA_open();
            cutB_close();
          //  led_wl2_open(app_context->conf_info->wl_led);
            led_wl1_open(app_context->conf_info->pl_led);
            ble_service->sendMsg("LED=OK\r\n",8,0);
             DHSUI_shm(2,2);
        }
        else if (str_match(data, "LED=3"))
        {
           if(app_context->conf_info->CPL_AUTO==0)
           {
            DHS_Set_AWB(app_context->conf_info->cpl_awb_rgain,app_context->conf_info->cpl_awb_grgain,app_context->conf_info->cpl_awb_gbgain,app_context->conf_info->cpl_awb_bgain);
           }
            DHS_Set_ISP(app_context->conf_info->cpl_ae_exptime,app_context->conf_info->cpl_ae_ispgain,app_context->conf_info->cpl_ae_again,app_context->conf_info->cpl_ae_dgain);
            cutA_close();
            cutB_open();
           // led_wl2_open(app_context->conf_info->wl_led);
            led_wl1_open(app_context->conf_info->cpl_led);
            ble_service->sendMsg("LED=OK\r\n",8,0);
             DHSUI_shm(2,3);
        }
        else if (str_match(data, "LED=4"))
        {
           if(app_context->conf_info->UV_AUTO==0)
           {
            DHS_Set_AWB(app_context->conf_info->uv_awb_rgain,app_context->conf_info->uv_awb_grgain,app_context->conf_info->uv_awb_gbgain,app_context->conf_info->uv_awb_bgain);
           }
            DHS_Set_ISP(app_context->conf_info->uv_ae_exptime, app_context->conf_info->uv_ae_ispgain, app_context->conf_info->uv_ae_again, app_context->conf_info->uv_ae_dgain);
            cutA_close();
            cutB_close();
            led_uv_open(app_context->conf_info->uv_led);
            ble_service->sendMsg("LED=OK\r\n",8,0);
             DHSUI_shm(2,4);
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
             DHSUI_shm(2,0);
        }
        else
        {
            printf("ble_process Get %s \r\n",data);
        }
        
        
        return;
    }
    else if (str_match(data, "BSO"))
    {
        blood_service->open();
        if (blood_open() == DHS_FAILURE)
        {
             ble_service->sendMsg("BSO=FAIL\r\n",10,0);
            
        }
        else
        {
             ble_service->sendMsg("BSO=OK\r\n",8,0);
          
        }
        return;
    }
    else if (str_match(data, "BSC"))
    {
        blood_close();
        ble_service->sendMsg("BSC=OK\r\n",8,0);
       
    }
    else if (str_match(data, "SN"))
    {

    }
    else if (str_match(data, "NETSTA"))
    {

    }
    else if (str_match(data, "NETAP"))
    {
    }
    else if (str_match(data, "NETP2P"))
    {

    }
    else if (str_match(data, "WFAPC"))
    {
        //WFAPC=MAC=\r\n
    }
    else if (str_match(data, "P2PC"))
    {
        app_context->cli_info->status = NET_CONNING;
        wifi_service->wifiP2pManager->p2pFind();
        const char *delim = "=";
       // char *p = NULL;
        char *saveptr = NULL;
        //p = 
        strtok_r(data, delim, &saveptr);
        char *p1 = NULL;
        char *saveptr1 = NULL;
        p1 = strtok_r(saveptr, delim, &saveptr1);
        //strncpy(app_context->cli_info->mac, p1, strlen(p1));
        printf("Client mac addr/name is:%s \n", p1);
        WifiStatus ws;
        int ret = wifi_service->getWifiStatus(&ws);
        //获取设备p2p状态
        if(ret == WIFI_TRUE){
            if(ws._wpa_state == COMPLETED){
                if(str_match(p1, app_context->cli_info->mac)){
                    //直接返回ＩＰ地址
                    char buf[30];
                    sprintf(buf, "P2PC=%s\r\n", app_context->dev_info->ip);
                    app_context->cli_info->status = NET_CONNED;
                    ble_service->sendMsg(buf,strlen(buf),0);
                    return;
                }else{
                   //移除p2p
                    wifi_service->wifiP2pManager->p2pRemovedGroup(); 
                    wifi_service->wifiP2pManager->p2pFind();
                    ble_service->sendMsg("P2PE=1\r\n",8,0);

                }
            }
        }else{
            wifi_service->wifiP2pManager->p2pReset();
        }
        strcpy(app_context->cli_info->mac, p1);
        //去链接P2P
        pthread_t p2p_pid;
        if (pthread_create(&p2p_pid,NULL,p2p_connect,(void *) app_context->cli_info->mac) != 0){
            perror("pthread_create");
            //未知错误
            ble_service->sendMsg("P2PE=8\r\n", 8,0);
            app_context->cli_info->status = NET_CONNED;
            return;
        }
        return;
    }
    else if (str_match(data, "UVCC"))
    {
        //UVC连接
        //关闭电源充电
        //关闭WiFi
        //启动uvc
        //返回成功
    }
    else if (str_match(data, "APPEXIT"))
    {
    }
    else if (str_match(data, "UPGRADE"))
    {
    }
    else if (str_match(data, "GETLOGS"))
    {

    }
    else if (str_match(data, "PICTURE"))
    {

    }
    else if (str_match(data, "SET"))
    {

        if (str_match(data, "SETSN")) 
        {
            //"[INFO]\n"
            //"\n"
            //"serial     =   20_0009\n"
            char *p; 
            p = strtok(data, "=");
            p = strtok(NULL, "=");
            char temp[30]={0};
            memset(temp,0,30);
            sprintf(temp,"20_%s",p);
            set_config_SIG("INFO","serial",temp);
        }
    }
    return;
}

void app_sleep(){
    if(app_run == DEV_SLEEP) return;
    app_run = DEV_SLEEP;
    if(app_context->cli_info->type==ANDROID){
        //移除p2p
        wifi_service->wifiP2pManager->p2pRemovedGroup();
    }
    ble_service->disClient();
    ble_service->sendMsg("NET=DIS\r\n", 9,0);
    stream_service->close();
    blood_service->close();
    led_all_close();
   
}

void app_work(){
    if(app_run == DEV_WORK) return;
    app_run = DEV_WORK;
    stream_service->open();
    //SAMPLE_SYS_SetReg(0x12145004,0);
}

void set_config_SIG(char *iten ,char *iten2,char* val)
{
    
    dictionary *ini         = NULL;
    char item_d[60]={0};
    //加载文件
    sprintf(item_d,"%s:%s",iten,iten2);

   // Set  conf.ini INFO:serial=20_0002
   // iniparser_set(ini,"LIGHT:uv_led",data);
    printf("Set  %s %s=%s \n",CONFIG_NAME,item_d,val);
    
    ini = iniparser_load(CONFIG_NAME);
    iniparser_set(ini,item_d,val);    
    
 //   printf("fopen\n");
    FILE * f = fopen(CONFIG_NAME,"w");
    iniparser_dump_ini(ini, f);
    fclose(f);
//    printf("iniparser_freedict\n");
    iniparser_freedict(ini);
    
}
