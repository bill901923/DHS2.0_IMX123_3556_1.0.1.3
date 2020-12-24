

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <netinet/tcp.h>
#include <assert.h>

#include <pthread.h>
#include <sys/time.h>
#include "dhs_socket.h"
#include "dhs_type.h"
#include "dhs_stream.h"
#include "utils.h"
#include "dhs_picture.h"

static SocketService *sockService = NULL;
static SockState *sock_State = NULL;
static int client_sock = -1;
static pthread_t client_sockpid;
/*
static int SetNonBlock(int iSock)
{
    int iFlags;

    iFlags = fcntl(iSock, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    iFlags |= O_NDELAY;
    int ret = fcntl(iSock, F_SETFL, iFlags);
    return ret;
}*/
static void wait_delaytime_ms(int cunt)
{
    while (cunt--)
    {
        hi_usleep(1000);
    }   
}
/*static int str_match(const char *a, const char *b)
{
    return strncmp(a, b, strlen(b)) == 0;
}*/
static int str_match_have(const char *a, const char *b)
{
    return (strstr(a, b) != NULL);
}


static void *accept_request(void *arg)
{
    int tem_client_sock = (int)(intptr_t)arg;
    char data_recv[512];
 //   const char *data_send = "Server has received your request!\n";
    int i_recvBytes;
    int times=0; 
    int times2=0; 
    int shmid;
	void *shm = NULL;
	share_memory *shmdata;
	key_t key = ftok(SHM_NAME, 'k');
	shmid = shmget(key, sizeof(share_memory), 0666|IPC_CREAT);
	if(shmid == -1)
	{
		printf( "shmget failed. ");
		exit(-1);
	}
	shm = shmat(shmid, 0, 0);
	if(shm == (void *)-1)
	{
		printf( "shmat failed. ");
		exit(-1);
	}
	printf( "accept_request sharememory at 0x%X ", (unsigned int)shm);
	shmdata = (share_memory *)shm;
	shmdata->Sock_open = 1;   
    shmdata->sockflag=0;     
    int ret=DHS_FAILURE ;
    while (tem_client_sock>-1)
    {
        printf("waiting for request...\n");
        
        sockService->sockState = SOCK_CON_SUCCESS;
        sock_State->change_state(SOCK_CON_SUCCESS);
        //Reset data.
        memset(data_recv, 0, 512);
      //  printf("4\n");
        i_recvBytes = read(tem_client_sock, data_recv, 512);
      //  printf("5\n");
        
        printf("Git %d %d %s\n",i_recvBytes,strlen(data_recv),data_recv);
        if(i_recvBytes>0)
        {
            printf("Git %d %s\n",strlen(data_recv),data_recv);
            if(strlen(data_recv)==0)
            {
             printf("Git %d : ",i_recvBytes);
                for(int Fori=0;Fori<i_recvBytes;Fori++)
                {

             printf(" 0x%02x",data_recv[Fori]);
                }
             printf("\r\n");
            }
            times = 0;
            if (strcmp(data_recv, "quit") == 0)
            {
                printf("Quit command!\n");
                break; //Break the while loop.
            }
            if((str_match_have(data_recv, "pic"))||(str_match_have(data_recv, "PIC"))||
               (str_match_have(data_recv, "pic2"))||(str_match_have(data_recv, "PIC2"))
            )
            {
                
                int type=0;
                if((str_match_have(data_recv, "pic2"))||(str_match_have(data_recv, "PIC2")))
                {
                     type=1;       
                }

                shmdata->sockflag=1;  
                shmdata->getcon=1;  
                printf("pic command!\n");
                printf("shmdata->sockflag %d\n",shmdata->sockflag);
                printf("shmdata->getcon  %d\n",shmdata->getcon);
                //发送 拍照指令
                //int ret = takePhotoOfOnline(tem_client_sock);            
                int out_fail=0;
                int wait_cun=100;
                while((shmdata->read_sendflagend==0)&(out_fail<wait_cun))
                {
                    out_fail++;
                    if(shmdata->read_sendflag)
                    {
                         ret = takePhotoOfFIFO(tem_client_sock,type);  
                         out_fail=0;
                    }  
                    printf("wait %d s %d\n",out_fail,shmdata->read_sendflag);
                    if(shmdata->read_sendflagend==0)
                    {
                        wait_delaytime_ms(50);  
                    }
                                   
                }
                
                if(shmdata->read_sendflagend==1)
                {
                    ret=DHS_SUCCESS;
                    shmdata->sockflag=0;   
                    shmdata->read_sendflag=0;
                    shmdata->read_sendflagend=0;
                    
                }
                if(ret == DHS_SUCCESS){
                    if(type==0)
                    {
                        send(tem_client_sock, "JCGFOK", 6, 0);
                        printf("send to client image success!!! \n");
                    }
                    else
                    {
                        char tempvuff[50];
                        memset(tempvuff,0,50);
                        sprintf(tempvuff,"{\"DATA_LEN\":%d}",sendPhoto_len());
                        send(tem_client_sock, tempvuff, strlen(tempvuff), 0);
                        printf("Get to Buff image success!!! (%s)\n",tempvuff);
                    }
                    
                }else
                {
                    send(tem_client_sock, "ERR", 3, 0);
                }
                //shmdata->sockflag=0;
               // takePhotoOfOnline(client_sock);
                continue; //Break the while loop.
            }
            else if((str_match_have(data_recv, "getp"))||(str_match_have(data_recv, "GETP")))
            {
                  ret = sendPhoto(tem_client_sock); 
                if(ret >0){
                      //  send(tem_client_sock, "JCGFOK", 6, 0);
                        printf("send to client image success!!! \n");
                    
                }else
                {
                    send(tem_client_sock, "ERR", 3, 0);
                }
            }            
            else
            {   
                printf("++++++++++++++++++++++++++%s\n",data_recv);
            }
        }
		else if((i_recvBytes==-1 && (errno==EAGAIN)) )
		{
			printf("EAGAIN %dth sleep 1s\n",times++);
			sleep(1);
            if(times == 3){
                //超时
                printf("socket time out!!!!!!!!!!!!!!!!!!!!! \n");
                break;
            }
		}
		else if((errno==ETIMEDOUT) )
		{
			printf("ETIMEDOUT %d th sleep 1s\n",times2++);
            if(times2 == 1000){
                printf("socket time out!!!!!!!!!!!!!!!!!!!!! \n");
                break;
            }
		}
		else
		{
			printf("count=%d,errno=%d\n",i_recvBytes,errno);
			break;
		}

        usleep(1000);
    }
    //Clear
    close(tem_client_sock);
    sockService->sockState = SOCK_CON_STOP;
    printf("terminating current client_connection.......................\n");
    return NULL;
}

static void *listener_client(void *p)
{
    int server_sock = sockService->listenfd;
    struct sockaddr_in client_name;
    char addr_p[INET_ADDRSTRLEN];
    listen(server_sock, 3);
    printf("socket listener_client........\n");
    while (1)
    {
        printf("waiting client link........\n");
        //这边要为socklen_t类型
        socklen_t client_name_len = sizeof(client_name);
        int tem_client_sock = -1;
        tem_client_sock = accept(server_sock,(struct sockaddr *)&client_name,&client_name_len);
        //只允许一个socket 客户端
        if(sockService->sockState == SOCK_CON_SUCCESS){
            //关闭 client_sock
           shutdown(client_sock,SHUT_RDWR);
        }
        client_sock = tem_client_sock;
        if (tem_client_sock == -1){
            perror("accept");
        }

        inet_ntop(AF_INET, &client_name.sin_addr, addr_p, sizeof(addr_p));
        printf("client IP is %s, port is %d\n", addr_p, ntohs(client_name.sin_port));
        
        strcpy(sockService->client_ip,addr_p);

        //把client_sock转成地址作为参数传入pthread_create
        if (pthread_create(&client_sockpid, NULL, accept_request, (void *)(intptr_t)client_sock) != 0){
            perror("pthread_create");
        }
        
        printf("socket texting!!!!! \n");
        usleep(100000);
    }

    printf("socket close........\n");
    close(server_sock);
    return NULL;
}

int sock_open(SockState *ss){
    
    printf(" sock_open \n");
    sock_State = ss;
    struct sockaddr_in servaddr;
    if((sockService->listenfd = socket(AF_INET , SOCK_STREAM , 0)) < 0)
	{
		perror("socket error");
		return DHS_FAILURE;
	}
    bzero(&servaddr , sizeof(servaddr));
    servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //任意IP地址
	servaddr.sin_port = htons(DHS_SOCK_PORT);
    
    //printf(" servaddr.sin_addr.s_addr =%s\n",servaddr.sin_addr.s_addr);
    if(bind(sockService->listenfd , (struct sockaddr*)&servaddr , sizeof(servaddr)) < 0)
	{
		perror("bind error");
		exit(1);
	}

    int keepAlive = 1; // 开启keepalive属性
    int keepIdle = 30; // 如该连接在60秒内没有任何数据往来,则进行探测 
    int keepInterval = 10; // 探测时发包的时间间隔为5 秒
    int keepCount = 10; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.
    int nSize = 1024;
    int nZero = 0;

    setsockopt(sockService->listenfd,SOL_SOCKET,SO_RCVBUF,(char*)&nSize,sizeof(int));//接受缓存区
    setsockopt(sockService->listenfd,SOL_SOCKET,nZero,(char*)&nSize,sizeof(int));//发送缓冲区

    setsockopt(sockService->listenfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
    setsockopt(sockService->listenfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
    setsockopt(sockService->listenfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
    setsockopt(sockService->listenfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));

    pthread_t sockpid;

    if (pthread_create(&sockpid, NULL, listener_client, (void *)&sockService->listenfd) != 0)
    {
        perror("pthread_create");
        return DHS_NULL;
    }

    sockService->sockState = SOCK_ENABLED;
    return DHS_SUCCESS;
}
int sock_close(){
    close(sockService->listenfd);
    return DHS_SUCCESS;
}

int  sock_sendMsg (char * data,int len){
    return DHS_SUCCESS;
}

SocketService *getSocketService(){
    //omp_set_lock(&lock);
    if(sockService != NULL) {
        //omp_unset_lock(&lock);
        return sockService;
    } else {

        sockService = (SocketService*)malloc(sizeof(SocketService));
        sockService->listenfd = 0;
		sockService->open = sock_open;
		sockService->close = sock_close;
        sockService->sendMsg = sock_sendMsg;
        sockService->sockState = SOCK_DISENABLED;
        assert(sockService != NULL);
        //omp_unset_lock(&lock);
        return sockService;
    }
}