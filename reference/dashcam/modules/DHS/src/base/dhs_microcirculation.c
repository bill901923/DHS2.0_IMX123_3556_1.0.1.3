#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>
#include <sys/types.h>
//#include <linux/msg.h>
#include "../../include/dhs_microcirculation.h"

//#define BSOPEN  

static  MicrocirculationService *microcirculationService = NULL;
static int mcl_fd = 0;
static pthread_t blood_read_pid;
static int start_num = 0;

static void *blood_read_thread(void * para){
    printf("Start collection blood uart data >>>>>\n");   
    char my_rec_data[REC_DATA_LEN];
    int node = 0;
    while (microcirculationService->mcl_state == MCL_ENABLED) {
        if(++start_num >6){
            start_num=0;
            getBleService()->sendMsg("BSE\r\n",5,1);
            microcirculationService->close();
            return NULL; 
        }

        #ifndef BSOPEN
            fprintf(stderr,"=================Blood NOT DO =================\n\n");
            //发送信息到ble消息队列
            char msg_buf[12];

            sprintf(msg_buf, "BSD%02X %02X %02X\r\n",0,0,0);
            getBleService()->sendMsg(msg_buf,12,2);
        
            msSleep(200000);
            blood_close(); 
            return NULL;
        #endif
        if(readData(mcl_fd,my_rec_data, REC_DATA_LEN) > 0){
            start_num=0;
            unsigned char   head = 0xFF;
            unsigned char   bk;               //微循环 50-99  [67]
            unsigned char   spo2;             //血氧 70-99    [66]
            unsigned char   heartrate;        //心率 30-200   [65]
           // int i=0;
            if(head == my_rec_data[0]){

                heartrate   = my_rec_data[65];
                spo2        = my_rec_data[66];
                bk          = my_rec_data[67];

                //发送信息到ble消息队列
                char msg_buf[81];
                sprintf(msg_buf, "BSOK%s\r", my_rec_data);
                //mclState->process(msg_buf);
                getBleService()->sendMsg(msg_buf,81,2);
                
            }else{
                continue;
            }

            //计算血样 微循环  心率 是否符合要求
            
            char str[1],str1[1],str2[1];
            sprintf(str,"%d",bk);
            sprintf(str1,"%d",spo2);
            sprintf(str2,"%d",heartrate);
            int _bk         = atoi(str);
            int _spo2       = atoi(str1);
            int _heartrate  = atoi(str2);
            fprintf(stderr,"\n*******************************************************************\n");
            fprintf(stderr,"Blood microcirculation: %d == %02X\n",_bk,bk);
            fprintf(stderr,"Blood oxygen:           %d == %02X\n",_spo2,spo2);
            fprintf(stderr,"Heart rate:             %d == %02X\n",_heartrate,heartrate);

                if((49<_bk)&&(69<_spo2)&&(29<_heartrate))
                {
                    fprintf(stderr,"=================Blood completion success=================\n\n");
                    if(node<4){
                        continue;
                    }
                    //发送信息到ble消息队列
                    char msg_buf[12];
                    sprintf(msg_buf, "BSD%02X %02X %02X\r\n",heartrate,spo2,bk);
                    getBleService()->sendMsg(msg_buf,12,2);
                
                    msSleep(200000);
                    blood_close(); 
                return NULL;
                    //关闭血氧关闭线程
                }
            
            node ++;
            // //格式化打印
            // fprintf(stderr,"read from client :\n");
            // while(i<76){
            //     fprintf(stderr,"%02X ",my_rec_data[i++]);
            //     if((i%8)==0) fprintf(stderr,"\n\n");
            // }
            // fprintf(stderr,"\n\n");
            // fprintf(stderr,"===========================================================\n\n");

        }
        msSleep(1200000);
    }
    pthread_exit(NULL);
    return NULL;
}
int blood_open(){
   printf("\n!!!!!!Openging blood senser !!!!!\n");
   if(microcirculationService->mcl_state == MCL_ENABLED){
       return DHS_SUCCESS;
   }
    //传感器上电
    HI_U32 value;
    himd(0x120D0400, &value);  
    printf("\nvalue =0x%x\n",value);
    if ((value & 0x00000001) != 0x00000001)
    {
        value &= 0xFFFFFFFE;
    printf("\nvalue =0x%x\n",value);
        value |= 0x00000001;
    printf("\nvalue =0x%x\n",value);
        himm(0x120D0400, value);
    }

    himm(0x120D0004,0x01 );
    //SAMPLE_SYS_SetReg(0x12145010,0x00000004);//himm 0x12145010 0x00000004
    //打开串口
    if(mcl_fd > 0){
        return DHS_SUCCESS;
    }

    mcl_fd = openPort(BLOOD_UART_DEV);
    if(mcl_fd<0) {
		printf(">>>Open blood serial port failed!\n");
		return DHS_FAILURE;
	}

    printf("Open blood serial(%s) port %d Success\n",BLOOD_UART_DEV, mcl_fd);
    setPara(mcl_fd, 4 ,8 ,1 ,0);
    msSleep(500000);
    
    //写入0x8A 打开传感器
    const char open[1] = {0x8A};
    if (writeData(mcl_fd,open, 1) < 0) {
		printf("Blood serial write Data Fail!\n");
		return DHS_FAILURE;
	}
    microcirculationService->mcl_state = MCL_ENABLED;
    //开启接受数据线程
    if(pthread_create(&blood_read_pid, 0, blood_read_thread,(void *)mcl_fd) == -1){
        return DHS_FAILURE;
    }
    printf("!!!!!!DHS2.0 blood init success!!!!!\n");
    return DHS_SUCCESS;
}
int blood_close(){
    printf("!!!!!!Close blood senser !!!!!\n");
    if (microcirculationService->mcl_state == MCL_DISENABLED){
       return DHS_SUCCESS;
    }
    microcirculationService->mcl_state = MCL_DISENABLED;

    //发送关闭命令
    //写入0x88 关闭传感器
    const char str_close[1] = {0x88};
    writeData(mcl_fd,str_close, 1);
    //传感器断电
    closePort(mcl_fd);
    mcl_fd = -1;
    //拉低 电源
    //SAMPLE_SYS_SetReg(0x12145010,0);
    printf("!!!!!!Close blood senser success !!!!!\n");
    return DHS_SUCCESS;
}

int blood_reset(){
    return DHS_SUCCESS;
}



MicrocirculationService* getMicrocirculationService(){
    //omp_set_lock(&lock);
    if(microcirculationService != NULL) {
        //omp_unset_lock(&lock);
        return microcirculationService;
    } else {
        microcirculationService = (MicrocirculationService*)malloc(sizeof(MicrocirculationService));
		microcirculationService->open = blood_open;
		microcirculationService->close = blood_close;
        microcirculationService->mcl_state = MCL_DISENABLED;
        assert(microcirculationService != NULL);
        //omp_unset_lock(&lock);
        return microcirculationService;
    }
}

