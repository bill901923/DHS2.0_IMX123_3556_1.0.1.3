#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>
//#include<linux/msg.h>

#include "dhs_ble.h"
#include "dhs_uart.h"
#include "dhs_type.h"
#include "smartlist.h"
#include "sample_comm.h"
#include "hi_hal_common.h"
#include "hi_appcomm_util.h"

static BLEService *bLEService = NULL;
static BleState *bleState = NULL;
static smartlist_t *ble_msg_smartlist;
static int ble_fd = 0;

#define BT_WAKE_UP_IO_ADR 0x120DA100 
#define BT_WAKE_UP_IO_H 0x120DA100 
#define BT_WAKE_UP_IO_L 0x120DA100 

#define BT_CTRL_IO_ADR 0x120DA200 


static int str_match(const char *a, const char *b)
{
    return strncmp(a, b, strlen(b)) == 0;
}

static int check_ble(){
    char buff[512];
    int fd = ble_fd;
    fd_set fds;
    int nread;
    struct timeval timeout;
    const char * buf_response = "AT+OK\r\n";
    const char * w_buf = "AT\r\n";
    if (writeData(fd, w_buf, 4) < 0) {
        printf(">>>BLE uart write failed!\n");
		return DHS_FAILURE;
	}
    while (1)
    {
        timeout.tv_sec = 2; //等待1秒
        timeout.tv_usec = 0;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        switch (select(fd + 1, &fds, NULL, NULL, &timeout))
        {
        case -1:
            printf(">>>LBE check select error\n");
            break;
        case 0:
            printf(">>>LBE check Overtime\n");
            printf("Overtime\n");
            break;
        default:
            while ((nread = read(fd, buff, 512)) > 0)
            {
                printf("Uart Receive:");
                buff[nread + 1] = '\0';
                printf("%s\n", buff);
                if (!memcmp(buff, buf_response, 7))
                {
                    return DHS_SUCCESS;
                }
                return DHS_SUCCESS;
            }
            break;
        }
        printf(">>>Receive failed,please check the device connection.\n");
        return DHS_FAILURE;
    }
}

static void* ble_read_thread(void* p)
{
    //BleState *bs = (BleState *)p;
    char buffer[50]; 
    char my_rec_data[BLE_REC_DATA_LEN];
    int buflen=0;
    memset(buffer,0,50);
    HI_U32 value;
    while (1) 
    {
        himd(0x120DA200, &value);  // 10_7 INPUT 10_6 outPUT
        if(readData(ble_fd,my_rec_data, BLE_REC_DATA_LEN)> 0)
        {          
            printf("value %02x  %d Get BLE %d - %s\n",value,bLEService->ble_state,strlen(my_rec_data),my_rec_data);
            for(int ui=0;ui<strlen(my_rec_data);ui++)
            {
                buffer[buflen++]=my_rec_data[ui];
                if(((buffer[buflen-1]==0x0D)&&(buffer[buflen-2]==0x0A))||
                   ((buffer[buflen-1]==0x0A)&&(buffer[buflen-2]==0x0D))) 
                {  

                     printf("Do BLE CMD %d(%d) - %s\n",buflen,strlen(buffer),buffer);
                    if (str_match(buffer, "AT+CON")){
                        const char *delim = "=";
                        //char *p = NULL;
                        char *saveptr = NULL;
                        p = strtok_r(buffer, delim, &saveptr);
                        if (str_match(saveptr, "SUCCESS")){

                            printf(">>>By COMD SUCCESS\n");
                            bLEService->ble_state = BLE_CON_SUCCESS;
                            bleState->changeState(BLE_CON_SUCCESS);
                        }
                        else if (str_match(saveptr, "STOP")){
                            printf(">>>By COMD STOP\n");
                            bLEService->ble_state = BLE_CON_STOP;
                            bleState->changeState(BLE_CON_STOP);
                        }                
                    } else if (check_str(buffer,"INIT")==1)
                    {
                        bleState->process("INIT\r\n");
                    }else
                    {
                        bleState->process(buffer);
                    }

                    memset(buffer,0,50);
                    buflen=0;
                }
            }
            memset(my_rec_data,0,BLE_REC_DATA_LEN);
        }
        else if(((value & 0x00000080)==0)&&(bLEService->ble_state != BLE_CON_SUCCESS))
        {
                printf(">>>By IO SUCCESS\n");
            bLEService->ble_state = BLE_CON_SUCCESS;
            bleState->changeState(BLE_CON_SUCCESS);
        }
        else if(((value & 0x00000080)==0x80)&&(bLEService->ble_state != BLE_CON_STOP))
        {
                printf(">>>By IO STOP\n");
            bLEService->ble_state = BLE_CON_STOP;
            bleState->changeState(BLE_CON_STOP);
        }
        
        
        usleep(10000);
    }
    return NULL;
}
static void* ble_write_thread(void* p)
{
    msSleep(200000);
    //清除所有链接的设备
    writeData(ble_fd,"AT+DISA\r\n",9);
    while (1)
    {
        if(smartlist_len(ble_msg_smartlist) >0){
            BLE_Msg *bm = (BLE_Msg *) smartlist_get(ble_msg_smartlist,0);
            writeData(ble_fd,bm->msgText,bm->len);
            free(bm);
            smartlist_del(ble_msg_smartlist,0);
            msSleep(500000);
        }
       	
        usleep(100000);
    }

    return NULL;
}
int ble_open(BleState *state,_config_info *cf){
     bleState = state;
     HI_U32 value;
    if(ble_fd>0){
        return DHS_SUCCESS;
    }
/*    printf("uart init ...\n");
    if (uart_dev_init() != 0)
    {
        printf("uart_dev_init failed");
    }*/

    ble_fd = openPort(BLE_UART_DEV);
    if(ble_fd<0) {
		printf(">>>Open ble serial port failed!\n");
		return DHS_FAILURE;
	}
    printf("Open ble serial %s port %d Success\n",BLE_UART_DEV, ble_fd);
 
    setPara(ble_fd, 6 ,8 ,1 ,0);
    if(check_ble() == DHS_FAILURE){

        return DHS_FAILURE;
    }
    if(cf->global_init == DHS_TRUE){
        //修改蓝牙名称 AT+NAME=XXXX
        char w_buf[30];
        sprintf(w_buf, "AT+NAME=%s#%s\r\n",cf->dev_name,cf->dev_serial);
        printf("BT NAME=%s#%s\r\n",cf->dev_name,cf->dev_serial);
        if (writeData(ble_fd, w_buf, strlen(w_buf)) < 0) {
            printf("BLE uart write name failed!\n");
            return DHS_FAILURE;
        }
    }
   
   
    ble_msg_smartlist = smartlist_new();
    pthread_t ble_read_pid , ble_write_pid;
    pthread_create(&ble_read_pid, 0, ble_read_thread, (void *)bleState);
    pthread_create(&ble_write_pid, 0, ble_write_thread, (void *)ble_msg_smartlist);
    set_BT_State_USE ;
    set_BT_CTRL_USE ; 	 
   // #define set_BT_CTRL_On himm(0x120DA080,0x40) 	//GPIO10_6
   // #define set_BT_CTRL_Off himm(0x120DA080,0x00) 	//GPIO10_6
   // Set IO
    himd(0x120DA400, &value);  // 10_7 INPUT 10_6 outPUT
    if ((value & 0x000000C0) != 0x00000040)
    {
        value &= 0xFFFFFF3F;
        value |= 0x00000040;
        himm(0x120DA400, value);
    }
    set_BT_CTRL_On;
    //开启广播；
    return DHS_SUCCESS;
}
int ble_disClient(){
    //清除所有链接的设备
    writeData(ble_fd,"AT+DISA\r\n",9);
    return 0;
}
int ble_work(){
    bLEService->ble_state = BLE_BROADCAST;
    //SAMPLE_SYS_SetReg(BT_WAKE_UP_IO_ADR,0);

    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>ble broadcast start<<<<<<<<<<<<<<<<<<<<<<< \n");
    return DHS_SUCCESS;
}
int ble_close(){
    //SAMPLE_SYS_SetReg(BT_WAKE_UP_IO_ADR,0x04);
    return DHS_SUCCESS;
}

int ble_reset(){
    return DHS_SUCCESS;
}

int ble_sendMsg(const char * data,int len, int type){
    if(bLEService->ble_state == BLE_CON_SUCCESS){
        BLE_Msg *bm = (BLE_Msg*) malloc(sizeof(BLE_Msg));
        bm->msgType = type;
        bm->msgText[0] = '\0';
        bm->len = len;
        strncpy(bm->msgText,data,len);
        smartlist_add(ble_msg_smartlist,bm);   
    }
 
    return 0;
}


BLEService *getBleService(BleState *state){
    //omp_set_lock(&lock);
    if(bLEService != NULL) {
        //omp_unset_lock(&lock);
        return bLEService;
    } else {
 
        bLEService = (BLEService*)malloc(sizeof(BLEService));
		bLEService->open = ble_open;
        bLEService->work = ble_work;
		bLEService->close = ble_close;
		bLEService->reset = ble_reset;
        bLEService->disClient = ble_disClient;
        bLEService->ble_state = BLE_DISENABLED;
		bLEService->sendMsg = ble_sendMsg;	
        assert(bLEService != NULL);
        //omp_unset_lock(&lock);
        return bLEService;
    }
}

int check_str(char *s, char *c)
{
    int i=0,j=0,flag=-1;
    while(i<strlen(s) && j<strlen(c)){
        if(s[i]==c[j]){//如果字符相同则两个字符都增加
            i++;
            j++;
        }else{
            i=i-j+1; //主串字符回到比较最开始比较的后一个字符
            j=0;     //字串字符重新开始
        }
        if(j==strlen(c)){ //如果匹配成功
            flag=1;  //字串出现
            break;
        }
    }
    return flag;
}