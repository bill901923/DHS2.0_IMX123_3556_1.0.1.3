#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/signal.h>
#include <signal.h>
#include <string.h>
//#include <omp.h>
#include "wpa_ctrl.h"
#include "dhs_wifi_service.h"
#include <sys/time.h>
#include <signal.h>

int wsSleep(long ms) 
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = ms;

    return select(0, NULL, NULL, NULL, &tv);
}
static wpa_cli *wci = NULL;
static WifiService * _wifiService = NULL;
static WifiContext *_context = NULL;
//static omp_lock_t lock;
pthread_t wpa_pid;
WIFI_BOOL p2p_wps_pbc();

static void *receiveMsgs(void *para);
static int wpa_cli_open_connection(wpa_cli *wci);
//static void wpa_cli_close_connection(wpa_cli *wci);
static int str_match(const char *a, const char *b)
{
    return strncmp(a, b, strlen(b)) == 0;
}
/*static void chenge_wifi_state(){

}*/
 
static void *open_thread(void *para)
{
	WifiService *p = (WifiService *)para;
	p->wifiState = WIFI_STATE_ENABLING;
	
		WIFI_LOG("open_thread %d\n", p->wifiMode);
	if(p->wifiMode == AP){
		// system("ifconfig wlan0 192.168.0.1");
		// system(UDHCPD_SYSTEM);
		// FILE * fstream = popen(AP_WPA_SYSTEM, "r");
		// if (!fstream)
		// {
		// 	fprintf(stderr, "execute command failed: %s", strerror(errno));
		// 	return WIFI_FALSE;
		// }
		// char buff[256];		
		// while (NULL != fgets(buff, sizeof(buff), fstream))
		// {
		// 	fprintf(stderr,"==========:%s \n",buff);
		// }

		// pclose(fstream);
		//wpa_cli_open_connection(wci);
		//strcpy(com,AP_WPA_SYSTEM);
		_wifiService->wifiState = WIFI_STATE_ENABLED;
		_context->wifiChengeStateCallBack(_wifiService->wifiState);
	}else if(p->wifiMode ==STA){
		WIFI_LOG("!!!!!!!!!!!!!!!!!!STA_WPA_SYSTEM!!!!!!!!!!!!!!!!!!!!!!!!\n");
		system(STA_WPA_SYSTEM);
		wpa_cli_open_connection(wci);
	}else{
		WIFI_LOG("!!!!!!!!!!!!!!!!!!P2P_WPA_SYSTEM!!!!!!!!!!!!!!!!!!!!!!!!\n");
		system(P2P_WPA_SYSTEM);
		wpa_cli_open_connection(wci);
		//p2p_wps_pbc();
	}

	WIFI_LOG("open wifi thread close!!!! \n");
	return NULL;
}
int wifi_open(WIFI_MODE wm){
	if(_wifiService->isWifiEnabled){
		return WIFI_TRUE;
	}

	_wifiService->wifiMode = wm;
	wci = malloc(sizeof(wpa_cli));
	wci->g_wpa__cli_initialized = 0;
	wci->ctrl_ifname = strdup(WIFI_IFNAME);

		WIFI_LOG("_wifiService->wifiMode %d \n",_wifiService->wifiMode);
	pthread_t wifi_open_pid;
	if (pthread_create(&wifi_open_pid, NULL, open_thread, (void *)_wifiService) != 0)
	{
		WIFI_LOG("creat wifi_open failed \n");
		return WIFI_FALSE;
	}
		WIFI_LOG("creat wifi_open pass \n");
	return WIFI_TRUE;
}

WIFI_BOOL wifi_close(){
	
	return WIFI_TRUE;
}

WIFI_BOOL wifi_reset(){
	
	return WIFI_TRUE;
}


int wpa_cli_ctrlRequest(struct wpa_ctrl *ctrl, const char *cmd, char *buf, size_t *buflen)
{
	int ret;
	if (ctrl == NULL)
		return -3;
	ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, buflen, NULL);
	if (ret == -2)
		WIFI_LOG("'%s' command timed out.\n", cmd);
	else if (ret < 0)
		WIFI_LOG("'%s' command failed.\n", cmd);
	return ret;
}

WIFI_BOOL p2p_wps_pbc()
{
    char buf[64];
    size_t buf_len;
    buf_len = sizeof(buf) - 1;
    int result = wpa_cli_ctrlRequest(wci->ctrl_conn, "WPS_PBC any", buf, &buf_len);
    if (result < 0 || (memcmp(buf, "FAIL", 4) == 0))
    {
		WIFI_LOG(">>>WPS_PBC any Error: %s \n", buf);
        return WIFI_FALSE;
    }
	printf(">>>>>>>>>>>>>p2p_wps_pbc<<<<<<<<<<<\n");
    return WIFI_TRUE;
}

WIFI_BOOL p2pFind(){
	
	if(_wifiService->wifiP2pManager->p2p_state == SCANNING) return WIFI_TRUE;
	p2p_wps_pbc();
	char buf[64];
    size_t buf_len;
    buf_len = sizeof(buf) - 1;
    int result = wpa_cli_ctrlRequest(wci->ctrl_conn, "P2P_FIND 5000s", buf, &buf_len);
    WIFI_LOG(">>>p2p_find: %s \n", buf);
    if (result < 0)
    {
		_wifiService->wifiP2pManager->p2p_state = P2P_STATE_UNKNOWN;
        return WIFI_FALSE;
    }
    if (strncasecmp(buf, "OK", 2) != 0)
    {
		_wifiService->wifiP2pManager->p2p_state = P2P_STATE_UNKNOWN;
        return WIFI_FALSE;
    }
	_wifiService->wifiP2pManager->p2p_state = P2P_STATE_ENABLED;
	_wifiService->wifiP2pManager->p2p_net_state = SCANNING;

	return WIFI_TRUE;
}


struct itimerval itv;
static int itv_num = 0;
static int authening =0;
void p2p_uninit_time()  
{   
    itv.it_value.tv_sec = 0;  
    itv.it_value.tv_usec = 0;  
    itv.it_interval = itv.it_value;  
    setitimer(ITIMER_REAL, &itv, NULL);  
}
void p2p_timeout_signal_handler(int m)
{
	
	p2p_uninit_time();
	if(!authening){
		WIFI_LOG("authenticat time out!!!! \n");
		_wifiService->wifiP2pManager->p2p_net_state=AUTHENTICATTIMEOUT;
		_context->netChengeStateCallBack(_wifiService->wifiP2pManager->p2p_net_state);
	}

}
int p2p_timeout()
{
	signal(SIGALRM, p2p_timeout_signal_handler);
	
	itv.it_interval.tv_sec = 30;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 30;
	itv.it_value.tv_usec = 0;
	itv_num = 0;
	setitimer(ITIMER_REAL, &itv, NULL);
	return 0;
}

WIFI_BOOL p2pConnectWPA(char* dev_addr){
	_wifiService->wifiP2pManager->p2p_net_state=AUTHENTICATING;
	//P2P_CONNECT 02:ec:0a:64:a6:7b pbc persistent go_intent=0
	//76:d2:1d:91:de:db 02:ec:0a:64:a6:7b  P2P_CONNECT 76:d2:1d:91:de:db pbc persistent
	
    char com[128];
    sprintf(com, "P2P_CONNECT %s pbc persistent go_intent=0", dev_addr);
	printf("---------------------------------- :%s \n" ,com);
    char buf[32];
    size_t buf_len;
    buf_len = sizeof(buf) - 1;
    int result = wpa_cli_ctrlRequest(wci->ctrl_conn, com, buf, &buf_len);
    if (result < 0)
    {
        return -2;
    }
    if (strncasecmp(buf, "OK", 2) != 0)
    {
        return WIFI_FALSE;
    }
	authening = 0;
	p2p_timeout();
    return WIFI_TRUE;
}
//P2P_PEER 76:d2:1d:91:de:db
int p2pFindPeer(char *dev_addr){
	char com[64];
    sprintf(com, "P2P_PEER %s", dev_addr);
    char buf[1024];
	size_t buf_len;
    buf_len = sizeof(buf) - 1;
    int result = wpa_cli_ctrlRequest(wci->ctrl_conn, com, buf, &buf_len);
    if (result < 0)
    {
        return WIFI_FALSE;
		
    }
    const char *delim = "\n";
    char *p = NULL;
    char *saveptr = NULL;
    p = strtok_r(buf, delim, &saveptr);
    while (p != NULL)
    {
        const char *delim1 = "=";
        char *p1 = NULL;
        char *saveptr1 = NULL;
        p1 = strtok_r(p, delim1, &saveptr1);
        if (strcmp(p1, "status") == 0)
        {
            return atoi(saveptr1);
        }
        p = strtok_r(NULL, delim, &saveptr);
    }
	return WIFI_TRUE;
}
WIFI_BOOL p2pRemovedGroup(){
	_wifiService->wifiP2pManager->p2p_state = P2P_STATE_DISABLING;
	char buf[64];
    size_t buf_len;
    buf_len = sizeof(buf) - 1;
    int result = wpa_cli_ctrlRequest(wci->ctrl_conn, "P2P_GROUP_REMOVE wlan0", buf, &buf_len);
    WIFI_LOG(">>>>P2P_GROUP_REMOVE wlan0: %s \n", buf);
    if (result < 0 || (memcmp(buf, "FAIL", 4) == 0))
    {
        return WIFI_FALSE;
    }
	_wifiService->wifiP2pManager->p2p_state = P2P_STATE_DISABLED;
	_wifiService->wifiP2pManager->p2p_net_state = AVAILABLE;
    return WIFI_TRUE;
}
int p2pPeerStatus(char *dev_addr,int sta){
	char com[64];
    sprintf(com, "P2P_PEER %s", dev_addr);
    char buf[1024];
	size_t buf_len;
    buf_len = sizeof(buf) - 1;
    int result = wpa_cli_ctrlRequest(wci->ctrl_conn, com, buf, &buf_len);
    if (result < 0)
    {
        return WIFI_FALSE;
		
    }
	if (strncasecmp(buf, "FAIL", 4) == 0)
    {
        return WIFI_FALSE;
    }
	printf("^^^^^^^^^^^^^^^^^^^^^ :%s \n",buf);
    const char *delim = "\n";
    char *p = NULL;
    char *saveptr = NULL;
    p = strtok_r(buf, delim, &saveptr);
    while (p != NULL)
    {
        const char *delim1 = "=";
        char *p1 = NULL;
        char *saveptr1 = NULL;
        p1 = strtok_r(p, delim1, &saveptr1);
        if (strcmp(p1, "status") == 0)
        {
            sta = atoi(saveptr1);
			return WIFI_TRUE;
        }
        p = strtok_r(NULL, delim, &saveptr);
    }
    return WIFI_FALSE;
}

WIFI_BOOL p2pClose(){
	_wifiService->wifiP2pManager->p2p_state = P2P_STATE_DISABLING;
	
	char buf[64];
    size_t buf_len;
    buf_len = sizeof(buf) - 1;
    int result = wpa_cli_ctrlRequest(wci->ctrl_conn, "P2P_FLUSH", buf, &buf_len);
    WIFI_LOG(">>>>p2p_flush: %s \n", buf);
    if (result < 0 || (memcmp(buf, "FAIL", 4) == 0))
    {
        return WIFI_FALSE;
    }
	_wifiService->wifiP2pManager->p2p_state = P2P_STATE_DISABLED;
	_wifiService->wifiP2pManager->p2p_net_state = AVAILABLE;
    return WIFI_TRUE;
}
WIFI_BOOL p2pReset(){
	p2pClose();
	p2pFind();
	return WIFI_TRUE;
}
WIFI_BOOL ap_findByBssid(char * bssid){
	char com[64];
    sprintf(com, "STA %s",bssid);
    char buf[256];
    size_t buf_len;
    buf_len = sizeof(buf) - 1;
    int result = wpa_cli_ctrlRequest(wci->ctrl_conn, com, buf, &buf_len);
    printf(">>>>>result:%d ; STA: %s \n", result, buf);
    if (result < 0 || (memcmp(buf, "FAIL", 4) == 0))
    {
        return WIFI_FALSE;
    }

    return WIFI_TRUE;
}




static int wpa_cli_open_connection(wpa_cli *wci)
{
	WIFI_LOG("==============================\n");
	if (wci->g_wpa__cli_initialized == 1)
	{
		if (wci->ctrl_conn)
		{
			wpa_ctrl_close(wci->ctrl_conn);
			wci->ctrl_conn = NULL;
		}
		if (wci->mon_conn)
		{
			wpa_ctrl_detach(wci->mon_conn);
			wpa_ctrl_close(wci->mon_conn);
			wci->mon_conn = NULL;
		}
	}

	//扫描 wlan0 socket节点文件
	int con_num = 0;
	
	while (1)
	{
		wci->ctrl_conn = wpa_ctrl_open(wci->ctrl_ifname);
		if(wci->ctrl_conn !=NULL)break;
		if(++con_num>6){
			//打开网卡失败
			_wifiService->wifiState = WIFI_STATE_UNKNOWN;
			_context->wifiChengeStateCallBack(_wifiService->wifiState);
			WIFI_LOG("!!!!wifi ctrl ifname open failed!!!!\n");
			break;
		}
		sleep(1);
	}
	
	if (wci->ctrl_conn == NULL)
	{
		_wifiService->wifiState = WIFI_STATE_UNKNOWN;
		return WIFI_FALSE;
	}
	wci->mon_conn = wpa_ctrl_open(wci->ctrl_ifname);
	
	if (wci->mon_conn == NULL)
	{
		WIFI_LOG("Failed to wci->mon_conn to wpa_supplicant\n");
		wpa_ctrl_close(wci->ctrl_conn);
		_wifiService->wifiState = WIFI_STATE_UNKNOWN;
		_context->wifiChengeStateCallBack(_wifiService->wifiState);
		return WIFI_FALSE;
	}

	if (wpa_ctrl_attach(wci->mon_conn))
	{
		WIFI_LOG("Failed to attach to wpa_supplicant\n");
		wpa_ctrl_close(wci->mon_conn);
		wci->mon_conn = NULL;
		wpa_ctrl_close(wci->ctrl_conn);
		wci->ctrl_conn = NULL;
		_wifiService->wifiState = WIFI_STATE_UNKNOWN;
		_context->wifiChengeStateCallBack(_wifiService->wifiState);
		return WIFI_FALSE;
	}
	wpa_ctrl_get_fd(wci->mon_conn);

	WIFI_LOG("Success open control connection to wpa_supplicant.\n");
	wci->g_wpa__cli_initialized = 1;

	if (pthread_create(&wpa_pid, NULL, receiveMsgs, (void *)wci) != 0)
	{
		WIFI_LOG("creat Receive_Thread failed \n");
		_wifiService->wifiState = WIFI_STATE_UNKNOWN;
		_context->wifiChengeStateCallBack(_wifiService->wifiState);
		return WIFI_FALSE;
	}
	_wifiService->wifiState = WIFI_STATE_ENABLED;
	//system("ifconfig wlan0 192.168.0.111");
	//mount -v -t nfs -o nolock 192.168.0.12://home/xll/workspace/DHS_DEV_HI3516/Hi3516CV300_SDK_V1.0.5.0/mpp/sample/dhs1400_dev_v4  /mnt
	_context->wifiChengeStateCallBack(_wifiService->wifiState);
	return WIFI_TRUE;
}
/*
static void wpa_cli_close_connection(wpa_cli *wci)
{
	if (wci->ctrl_conn == NULL)
	{
		return;
	}
	wpa_ctrl_close(wci->ctrl_conn);
	wci->ctrl_conn = NULL;
	if (wci->mon_conn == NULL)
	{
		return;
	}
	wpa_ctrl_close(wci->mon_conn);
	wci->mon_conn = NULL;
}
*/
NET_STATE wpaStateTranslate(char *state)
{
	if (!strcmp(state, "DISCONNECTED"))
		return DISCONNECTED;
	else if (!strcmp(state, "INACTIVE"))
		return INACTIVE;
	else if (!strcmp(state, "SCANNING"))
		return SCANNING;
	else if (!strcmp(state, "AUTHENTICATING"))
		return AUTHENTICATING;
	else if (!strcmp(state, "ASSOCIATING"))
		return ASSOCIATING;
	else if (!strcmp(state, "ASSOCIATED"))
		return ASSOCIATED;
	else if (!strcmp(state, "4WAY_HANDSHAKE"))
		return WAY_HANDSHAKE;
	else if (!strcmp(state, "GROUP_HANDSHAKE"))
		return GROUP_HANDSHAKE;
	else if (!strcmp(state, "COMPLETED"))
		return COMPLETED;
	else if (!strcmp(state, "INVITED"))
		return INVITED;
	else if (!strcmp(state, "FAILED"))
		return FAILED;
	else
		return UNKNOWN;
}

int get_wpa_wifi_status(WifiStatus* ws)
{
	char buf[2048];
	size_t buf_len;
	buf_len = sizeof(buf) - 1;
	int result = wpa_cli_ctrlRequest(wci->ctrl_conn, "STATUS", buf, &buf_len);
	printf(">>>status: %d \n %s \n", result, buf);
	if (result < 0 || (memcmp(buf, "FAIL", 4) == 0))
	{

		return WIFI_FALSE;
	}

	const char *delim = "\n";
	char *p = NULL;
	char *saveptr = NULL;
	p = strtok_r(buf, delim, &saveptr);
	while (p != NULL)
	{
		const char *delim1 = "=";
		char *p1 = NULL;
		char *saveptr1 = NULL;
		p1 = strtok_r(p, delim1, &saveptr1);
		if (strcmp(p1, "wpa_state") == 0)
		{
			ws->_wpa_state = wpaStateTranslate(saveptr1);
		}
		else if (strcmp(p1, "ip_address") == 0)
		{
			ws->ip_address = saveptr1;
		}
		else if (strcmp(p1, "address") == 0)
		{
			ws->address = saveptr1;
		}
		else if (strcmp(p1, "p2p_device_address") == 0)
		{
			ws->p2p_device_address = saveptr1;
		}
		else if (strcmp(p1, "mode") == 0)
		{
			if (!strcmp(saveptr1, "station"))
			{
				ws->mode = STA;
			}
			else
			{
				ws->mode = AP;
			}
		}
		else if (strcmp(p1, "bssid") == 0)
		{
			ws->bssid = saveptr1;
		}
		else if (strcmp(p1, "ssid") == 0)
		{
			ws->ssid = saveptr1;
		}
		else if (strcmp(p1, "key_mgmt") == 0)
		{
			ws->key_mgmt = saveptr1;
		}
		else if (strcmp(p1, "id") == 0)
		{
			ws->id = atoi(saveptr1);
		}
		else
		{
			printf(">>>>>>>>>get_wpa_wifi_status other \n");
		}
		p = strtok_r(NULL, delim, &saveptr);
	}

	return WIFI_TRUE;
}


int udhcp_reset_ip(char *ip, char *dns)
{
    FILE *fstream = NULL;
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    //-s /usr/share/udhcpc/default.script  -t N最多发送N个请求数据包 -n如果没有立即获得租约，则退出并失败
    //-q获得租约后退出 -T N尝试获得N秒的租约 -B 表示后台运行
    if (NULL == (fstream = popen("udhcpc -i wlan0 -t12 -T1 -n -q", "r")))
    {
        fprintf(stderr, "execute command failed: %s", strerror(errno));
        return WIFI_FALSE;
    }

    WIFI_LOG("udhcp_reset_ip::::::::");
    WIFI_BOOL Lease_of = WIFI_FALSE;
    WIFI_BOOL Adding_DNS = WIFI_FALSE;
    while (NULL != fgets(buff, sizeof(buff), fstream))
    {
        printf("=============Get buff >%s \n", buff);
		
        //Lease of 192.168.49.163 obtained, lease time 3600
        if (str_match(buff, "Lease of")||str_match(buff, "lease of"))
        {
            const char *delim = " ";
           // char *p = NULL;
            char *p2 = NULL;
            char *saveptr = NULL;
			printf("=============Get Lease of >%s \n", buff);
           // p = 
			strtok_r(buff, delim, &saveptr);
            strtok_r(saveptr, delim, &saveptr);
            p2 = strtok_r(saveptr, delim, &saveptr);
            WIFI_LOG("Lease_of: %s \n", p2);
            strcpy(ip, p2);
            Lease_of = WIFI_TRUE;
        }		
		//udhcpc: lease of 192.168.49.119 obtained, lease time 3600
		//Setting IP address 192.168.49.119 on wlan0
        else if (str_match(buff, "Setting IP"))
		{
            const char *delim = " ";
           // char *p = NULL;
            char *p2 = NULL;
            char *saveptr = NULL;
			
			p2 = strtok_r(buff, delim, &saveptr);
            p2 = strtok_r(saveptr, delim, &saveptr);
            p2 = strtok_r(saveptr, delim, &saveptr);
            p2 = strtok_r(saveptr, delim, &saveptr);
            WIFI_LOG("Lease_of: %s \n", p2);
            strcpy(ip, p2);
            Lease_of = WIFI_TRUE;
		}
        else if (str_match(buff, "Adding router"))
        {
            //Adding router 192.168.49.1
            const char *delim = " ";
          //  char *p = NULL;
            char *p2 = NULL;
            char *saveptr = NULL;
          //  p = 
			strtok_r(buff, delim, &saveptr);
            strtok_r(saveptr, delim, &saveptr);
            p2 = strtok_r(saveptr, delim, &saveptr);
            WIFI_LOG("Adding_DNS: %s \n", p2);
            strcpy(dns, p2);
            Adding_DNS = WIFI_TRUE;
        }
    }

    pclose(fstream);
    if (Lease_of == WIFI_FALSE || Adding_DNS == WIFI_FALSE)
    {
        return WIFI_FALSE;
    }
    return WIFI_TRUE;
}


WifiService *getWifiService(WifiContext *context){
    //omp_set_lock(&lock);
    if(_wifiService != NULL) {
        //omp_unset_lock(&lock);
        return _wifiService;
    } else {
        _wifiService = (WifiService*)malloc(sizeof(WifiService));
		//_context = (WifiContext*)malloc(sizeof(WifiContext));
		_context = context;
		_wifiService->isWifiEnabled = WIFI_FALSE;
		_wifiService->wifiState = WIFI_STATE_DISABLED;
		_wifiService->open = wifi_open;
		_wifiService->close = wifi_close;
		_wifiService->reset = wifi_reset;
		_wifiService->setIP = udhcp_reset_ip;
		_wifiService->getWifiStatus = get_wpa_wifi_status;
		WifiP2pManager * _wifiP2pManager = NULL;
		_wifiP2pManager = (WifiP2pManager*)malloc(sizeof(WifiP2pManager));
		_wifiP2pManager->p2pFindPeer = p2pFindPeer;
		_wifiP2pManager->p2pRemovedGroup = p2pRemovedGroup;
		_wifiP2pManager->p2pClose = p2pClose;
		_wifiP2pManager->p2pFind = p2pFind;
		_wifiP2pManager->p2pReset = p2pReset;
		_wifiP2pManager->p2p_state = P2P_STATE_DISABLED;
		_wifiP2pManager->p2pConnectWPA = p2pConnectWPA;
		_wifiP2pManager->p2pPeerStatus = p2pPeerStatus;
		_wifiP2pManager->P2PDeviceList = smartlist_new();
		_wifiService->wifiP2pManager = _wifiP2pManager;
		WifiAPManager * _wifiAPManager = NULL;
		_wifiAPManager = (WifiAPManager*)malloc(sizeof(WifiAPManager));
		_wifiAPManager->findByBssid = ap_findByBssid;
		WifiManager _wifiManager;
		_wifiService->wifiManager = &_wifiManager;
        assert(_wifiService != NULL);
        //omp_unset_lock(&lock);
        return _wifiService;
    }
}


static void *receiveMsgs(void *para)
{
	wpa_cli *p = (wpa_cli *)para;
	char buf[512];
	while (1)
	{
		if(wpa_ctrl_pending(p->mon_conn) > 0){
			size_t len;
			len = sizeof(buf) - 1;
			if (wpa_ctrl_recv(p->mon_conn, buf, &len) == 0)
			{
				char *pos = buf, *pos2;
			//	int priority = 2;

				if (*pos == '<')
				{
					/* skip priority */
					pos++;
				//	priority = atoi(pos);
					pos = strchr(pos, '>');
					if (pos)
						pos++;
					else
						pos = buf;
				}
				if (strncmp(pos, "CTRL-", 5) == 0)
				{
					pos2 = strchr(pos, str_match(pos, WPA_CTRL_REQ) ? ':' : ' ');
					if (pos2)
						pos2++;
					else
						pos2 = pos;
				}
				else
				{
					pos2 = pos;
				}
				
				buf[len] = '\0';
				//WIFI_LOG(">>>>>>>>>>>>>>>>>>>>>>wifi: %s <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n",buf);
				if (str_match(pos,P2P_EVENT_FIND_STOPPED)){
					WIFI_LOG(">>>>>>>>>>>>>>>>>>>>>wifi:%s\n",buf);
					//P2P 停止搜索
					_wifiService->wifiP2pManager->p2p_state = P2P_STATE_DISABLED;
					_wifiService->wifiP2pManager->p2p_net_state = AVAILABLE;
				}if (str_match(pos,P2P_EVENT_GO_NEG_SUCCESS)){
					WIFI_LOG(">>>>>>>>>>>>>>>>>>>>>wifi:%s\n",buf);
					authening = 1;
					_wifiService->wifiP2pManager->p2p_state = P2P_STATE_DISABLED;
					_wifiService->wifiP2pManager->p2p_net_state = INVITED;
				}
				else if(str_match(pos,P2P_EVENT_GROUP_STARTED))
				{
					WIFI_LOG(">>>>>>>>>>>>>>>>>>>>>wifi:%s\n",buf);
					//p2p链接成功
					_wifiService->wifiP2pManager->p2p_net_state = COMPLETED;
					_context->netChengeStateCallBack(_wifiService->wifiP2pManager->p2p_net_state);
				}else if(str_match(pos,P2P_EVENT_GROUP_REMOVED))
				{
					WIFI_LOG(">>>>>>>>>>>>>>>>>>>>>wifi:%s\n",buf);
					//p2p链接断开
					_wifiService->wifiP2pManager->p2p_net_state = DISCONNECTED;
					_context->netChengeStateCallBack(_wifiService->wifiP2pManager->p2p_net_state);
				}else if(str_match(pos,P2P_EVENT_GO_NEG_FAILURE))
				{
					WIFI_LOG(">>>>>>>>>>>>>>>>>>>>>wifi:%s\n",buf);
					//p2p拒绝链接
					_wifiService->wifiP2pManager->p2p_net_state = FAILED;
					_context->netChengeStateCallBack(_wifiService->wifiP2pManager->p2p_net_state);
				}else if (str_match(pos, P2P_EVENT_DEVICE_LOST))
				{
					WIFI_LOG(">>>>>>>>>>>>>>>>>>>>>wifi:%s\n",buf);
					//P2P-DEVICE-LOST p2p_dev_addr=c6:0e:2a:b5:6b:d3
					//_context->netChengeStateCallBack(_wifiService->wifiP2pManager->p2p_net_state);
					//p2p 某个设备过期
					const char *delim = "=";
					//char *p = NULL;
					char *saveptr = NULL;
					//p = 
					strtok_r(pos, delim, &saveptr);
				
					smartlist_t * st = _wifiService->wifiP2pManager->P2PDeviceList;
					int st_len = smartlist_len(st);
					int i;
					for(i=0;i<st_len;i++){
						P2PDeviceInfo *pi = (P2PDeviceInfo *)smartlist_get(st,i);
						if(str_match(pi->p2p_dev_addr, saveptr)){
							free(pi);
							smartlist_del(st,i);
						}
					}

				}else if (str_match(pos, P2P_EVENT_DEVICE_FOUND))
				{
					printf("pos -> %s \r\n",pos);	
					//p2p 发现设备P2P-DEVICE-FOUND c6:0e:2a:b5:6b:d3 p2p_dev_addr=c6:0e:2a:b5:6b:d3 pri_dev_type=10-0050F204-5 name='LIO-AN00_DHS' config_methods=0x80 dev_capab=0x25 group_capab=0x2a vendor_elems=1 new=0
					P2PDeviceInfo *pi = (P2PDeviceInfo *)malloc(sizeof(P2PDeviceInfo));
					const char *delim = " ";
					char *p = NULL;
					char *saveptr = NULL;
					p = strtok_r(buf, delim, &saveptr);
					while (p != NULL)
					{
						const char *delim1 = "=";
						char *p1 = NULL;
						char *saveptr1 = NULL;
						p1 = strtok_r(p, delim1, &saveptr1);
						if (strcmp(p1, "p2p_dev_addr") == 0)
						{
							strcpy(pi->p2p_dev_addr,saveptr1);
						}
						else if (strcmp(p1, "name") == 0)
						{
							strcpy(pi->name,saveptr1);
						}
						
						p = strtok_r(NULL, delim, &saveptr);
					}

				 	smartlist_add(_wifiService->wifiP2pManager->P2PDeviceList,pi);

				}else if (str_match(pos, P2P_EVENT_GROUP_FORMATION_FAILURE))
				{
					WIFI_LOG(">>>>>>>>>>>>>>>>>>>>>wifi:%s",buf);
					//p2p构建组失败/认证失败 //Authentication with 76:d2:1d:91:5e:db timed out
					WIFI_LOG("p2p authentication fail!!!\n");
					
					_wifiService->wifiP2pManager->p2p_net_state = UNAVAILABLE;
					_context->netChengeStateCallBack(_wifiService->wifiP2pManager->p2p_net_state);
				}else if (str_match(pos, WPS_EVENT_TIMEOUT))
				{
					WIFI_LOG(">>>>>>>>>>>>>>>>>>>>>wifi:%s",buf);
					//链接等待超时
					WIFI_LOG(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>p2p authentication timed out!!!\n");
					
					// _wifiService->wifiP2pManager->p2p_net_state = AUTHENTICATTIMEOUT;
					// _context->netChengeStateCallBack(_wifiService->wifiP2pManager->p2p_net_state);
				}
				//P2P-GO-NEG-FAILURE status=4	
				
				//p->processMsg_callback(buf);
			}
		}
		usleep(100000);
	}
	return NULL;
}