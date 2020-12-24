#ifndef __DHS_WIFI_SERVICE_H__
#define __DHS_WIFI_SERVICE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "smartlist.h"
#define P2P_WPA_SYSTEM "wpa_supplicant -iwlan0 -Dnl80211 -B -c/app/wireless/p2p_supplicant.conf &"
#define STA_WPA_SYSTEM "wpa_supplicant -iwlan0 -Dnl80211 -B -c/app/wireless/wpa_supplicant.conf &"
#define AP_WPA_SYSTEM "hostapd -e /app/wireless/entropy.bin /app/wireless/hostapd.conf &"
#define UDHCPD_SYSTEM "udhcpd /app/wireless/udhcpd.conf &"
#define AP_ENTROPY_DRC  "/etc/wireless/entropy.bin"
#define WIFI_IFNAME "/var/run/wpa_supplicant/wlan0"

#define WIFI_LOG(format, ...) fprintf(stderr, "\033[;31m" format "\033[0m\n", ## __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    WIFI_FALSE = 0,
    WIFI_TRUE = 1,
}WIFI_BOOL;
typedef enum
{
    WIFI_STATE_DISABLED = 110,        //WIFI网卡关闭
    WIFI_STATE_DISABLING = 111,       //WIFI网卡正在关闭
    WIFI_STATE_ENABLED = 112,         //WIFI网卡可用
    WIFI_STATE_ENABLING = 113,        //WIFI网卡正在打开
    WIFI_STATE_UNKNOWN = 114,         //WIFI网卡状态不可知
} WIFI_STATE;
typedef enum
{
    P2P_STATE_DISABLED = 20,        //P2P关闭
    P2P_STATE_DISABLING = 21,       //P2P正在关闭
    P2P_STATE_ENABLED = 22,         //P2P可用
    P2P_STATE_ENABLING = 23,        //P2P正在打开
    P2P_STATE_UNKNOWN = 24,         //P2P状态不可知
} P2P_STATE;

typedef enum{
    P2P_AVAILABLE = 33,                  //可用=3；
    P2P_CONNECTED = 30,                  //连接=0；
    P2P_FAILED = 32,                     //失败=2；
    P2P_INVITED = 31,                    //邀请(正在连接)=1；
    P2P_UNAVAILABLE = 34,                //不可用=4；
}P2P_STATUS;

typedef enum {
    COMPLETED       = 40,
    INVITED         = 41,            //邀请链接
    FAILED          = 42,            //链接失败
    AVAILABLE       = 43,            //可用 空闲的
    UNAVAILABLE     = 44,            //不可用
    DISCONNECTED    = 45,            //没有链接
    SCANNING        = 46,            //搜索网络中
    AUTHENTICATING  = 47,            //认证中            
    ASSOCIATING     = 48,            //同意链接中
    ASSOCIATED      = 49,            
    WAY_HANDSHAKE  = 410,
    GROUP_HANDSHAKE = 411,
    UNKNOWN         = 412,           //未知
    INACTIVE         = 413,          //不活动
    AUTHENTICATTIMEOUT  =414,        //认证超时
}NET_STATE;


typedef enum{
    STA = 0,
    AP  = 1,
    P2P = 2,
} WIFI_MODE;

typedef struct dhs_WifiContext{
    int (*wifiChengeStateCallBack)(WIFI_STATE wifiState);
    int (*netChengeStateCallBack)(NET_STATE wifiState);
}WifiContext;

typedef struct dhs_WifiStatus{
    char * bssid;                              //=c4:36:55:74:70:36
    char * ssid;                                //=JCGF_TEST-5G
    int id;                                     //=0
    WIFI_MODE mode;
    char *pairwise_cipher;                      //=CCMP
    char *group_cipher;                         //=CCMP
    char *key_mgmt;                             //=WPA2-PSK
    NET_STATE _wpa_state;                       //=COMPLETED
    char *ip_address;                           //=192.168.0.59
    char *p2p_device_address;                   //=0c:9a:42:08:da:e9
    char *address;                              //=0c:9a:42:08:da:e9
}WifiStatus;

typedef struct dhs_P2PDevice{
    char p2p_dev_addr[17];                              //=c4:36:55:74:70:36
    char pri_dev_type[20];                               //=10-0050F204-5
    char  name[17];
    int config_methods;
    int dev_capab;
    int group_capab;
    int vendor_elems; 
}P2PDeviceInfo;

typedef struct dhs_WifiConfiguration
{
    char * BSSID;
    char * SSID;
    char * PASSWORD;
    int PASS_TYPE;
}WifiConfiguration;

struct dhs_wifiInfoList {//定义列表结构体
    int A[1000];
    int n;
};

typedef struct  dhs_WifiInfo
{
    
}WifiInfo;

typedef struct dhs_wpa_cli{
    char *ctrl_ifname;
    struct wpa_ctrl *ctrl_conn;
    struct wpa_ctrl *mon_conn;
    int g_wpa__cli_initialized;
}wpa_cli;

typedef struct dhs_WifiP2pManager
{
    WIFI_BOOL (*p2pFind)();
    WIFI_BOOL (*p2pReset)();
    WIFI_BOOL (*p2pConnectWPA)(char* dev_addr);
    WIFI_BOOL (*p2pClose)();
    WIFI_BOOL (*p2pRemovedGroup)();
    int p2p_state;
    NET_STATE p2p_net_state;
    smartlist_t *P2PDeviceList;
    int (*p2pFindPeer)(char *dev_addr);
    int (*p2pPeerStatus)(char *dev_addr ,int sta);
}WifiP2pManager;

typedef struct dhs_WifiAPManager
{
   WIFI_BOOL (*findByBssid)(char * bssid);
}WifiAPManager;



typedef struct dhs_WifiManager
{
    int (*addNetwork)(WifiConfiguration config);                        //添加一个config描述的WIFI网络，默认情况下，这个WIFI网络是DISABLE状态的。
    int (*calculateSignalLevel)(int rssi , int numLevels);              //计算信号的等级
    int (*compareSignalLevel)(int rssiA, int rssiB);                    //对比网络A和网络B的信号强度
    int (*createWifiLock)(int lockType, char * tag) ;                   //创建一个WIFI 锁，锁定当前的WIFI连接
    int (*disableNetwork)(int netId);                                   //让一个网络连接失效
    int (*disconnect)();                                                //断开当前的WIFI连接
    int (*enableNetwork)(int netId, int disableOthers);                 //连接netId所指的WIFI网络，并是其他的网络都被禁用
    int (*getConfiguredNetworks)();                                     //获取网络连接的状态
    int (*getDhcpInfo)();                                               //获取DHCP 的信息
    struct dhs_wifiInfoList (*getScanResulats)();                       // 获取扫描测试的结果
    WIFI_BOOL (*ressociate)();                                          //重新连接WIFI网络，即使该网络是已经被连接上的
    WIFI_BOOL (*reconnect)();                                           // 重新连接一个未连接上的WIFI网络
    WIFI_BOOL (*removeNetwork)(int netId);                              // 移除某一个网络
    WIFI_BOOL (*saveConfiguration)();                                   //保留一个配置信息
    int (*updateNetwork)(WifiConfiguration config);                     // 更新一个网络连接
    WIFI_BOOL (*startScan)();                                           //开始扫描
}WifiManager;


typedef struct dhs_WifiService{
    int (*open)(WIFI_MODE wm);

    WIFI_BOOL (*close)();

    WIFI_BOOL (*reset)();

    WifiP2pManager *wifiP2pManager;

    WifiAPManager  *wifiAPManager;

    WifiManager    *wifiManager;

    int (*chegeWifiMode)(WIFI_MODE wm);
    int (*getWifiStatus)(WifiStatus *ws);

    int (*setIP)(char *ip,char *dns);

    WIFI_STATE wifiState;                                               //获取当前WIFI设备的状态

    WIFI_BOOL isWifiEnabled;                                            //判断WIFI设备是否打开

    WIFI_MODE wifiMode;                                                //获取当前WIFI设备的模式

    char *(*getIpAddress)();                                            // 获取IP 地址

    WIFI_BOOL (*pingSupplicant)();                                      //ping操作，和PC的ping操作相同作用

    WifiInfo (*getConnectionInfo)();                                    //获取当前连接的信息
    
    WIFI_BOOL (*setWifiEnabled)();                                      //启用或禁用Wi-Fi。

    char * (*getBSSID)();                                               //获取BSSID属性

    int (*getDetailedStateOf)();                                        //获取客户端的连通性

    char *(*getHiddenSSID)();                                           //获取SSID 是否被隐藏

    int (*getLinkSpeed)();                                              //获取连接的速度

    char *(*getMacAddress)();                                           // 获取Mac 地址

    int (*getRssi)();                                                   //获取802.11n 网络的信号

    char *(*getSSID)();                                                 // 获取SSID

    WifiInfo (*getSupplicanState)();                                    //获取具体客户端状态的信息

} WifiService;

WifiService* getWifiService(WifiContext *context);
#ifdef __cplusplus
}
#endif
#endif //__DHS_WIFI_SERVICE_H__

