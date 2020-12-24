#ifndef __DHS_H__
#define __DHS_H__
#include <stdio.h>
#include <stdlib.h>
#include "dhs_type.h"
#include "Base_GPIO.h"
#include "dhs_led.h"
#include "dhs_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "hi_hal_gauge.h"
#include "hi_product_scene_setparam.h"
#include "product_scene_setparam_inner.h"
#include "hi_product_scene.h"
#include "hi_ipcmsg.h"
#include "hi_appcomm_msg_client.h"

#ifdef __cplusplus
extern "C" {
#endif
#define CONFIG_ADDR "./conf.ini"
#define CONFIG_NAME "conf.ini"
#define APP_SRC "/root/app/"
#define DHS_TEST_MODE 0

typedef enum
{
    ANDROID = 0,
    IOS = 1,
    PAD = 2,
    PC = 3,
    USB = 4,
} CLIENT_TYPE;
typedef enum
{
    DEV_SLEEP = 0,
    DEV_WORK  =1,
    DEV_INIT = 2,
} DEV_STATE;
typedef enum
{
    ON_LINE = 0,                //在线
    OFF_LINE = 1,               //离线
    NET_CONNING = 2,            //链接网络中
    NET_CONNED = 3,             //网络链接完成
} CLIENT_STATUS;


typedef struct dhs_client_info
{
    char name[24];
    char ip[17];
    CLIENT_STATUS status;        //0:在线；1:离线； 3:未知
    int socket_status;          //0:在线；1:离线；2:未知
    char mac[17];
    int net_status; //0:已连接；1:未连接;2在连接中...3上一次连接错误(客户端:未连接) 4断开连接 5连接超时 6:获取ip失败 7客户端拒绝连接 8未知错误 9连接错误,p2p连接操作频繁
    int state_processing; //连接状态是否需要处理 0:需要处理 1:不需要处理
    int wifi_refuse;   //p2p/wifi 连接时是否被拒绝
    int ble_dch;
    DHS_BOOL isFirst;       //主要是针对IOS
    CLIENT_TYPE type;
} client_info;

struct event_arg
{
	int flag;
	int level;
};

typedef struct dhs_config_info{
    int global_init;
    char dev_name[8];
    char dev_serial[8];
    char dev_version[8];
    //[LIGHT]
    int wl_led;

    int pl_led;
    int cpl_led;

    int uv_led;
    //[ISP]
    int wl_ae_exptime;
    int wl_ae_ispgain;
    int wl_ae_again;
    int wl_ae_dgain;

    int wl_awb_rgain;
    int wl_awb_gbgain;
    int wl_awb_grgain;
    int wl_awb_bgain;

    int pl_ae_exptime;
    int pl_ae_ispgain;
    int pl_ae_again;
    int pl_ae_dgain;

    int pl_awb_rgain;
    int pl_awb_gbgain;
    int pl_awb_grgain;
    int pl_awb_bgain;

    int cpl_ae_exptime;
    int cpl_ae_ispgain;
    int cpl_ae_again;
    int cpl_ae_dgain;

    int cpl_awb_rgain;
    int cpl_awb_gbgain;
    int cpl_awb_grgain;
    int cpl_awb_bgain;

    int uv_ae_exptime;
    int uv_ae_ispgain;
    int uv_ae_again;
    int uv_ae_dgain;

    int uv_awb_rgain;
    int uv_awb_gbgain;
    int uv_awb_grgain;
    int uv_awb_bgain;
    int wifimod; //1=open p2p 0 =close P2P
    int WL_AUTO; 
    int PL_AUTO; 
    int CPL_AUTO; 
    int UV_AUTO; 
}_config_info;

typedef struct dhs_device_info
{
    DEV_STATE dev_status; //0:在线；1:离线； 3:未知
    int work_mode;  //运行模式  0:android  1:ios 2:uvc 3:test 4:update
    int key_mod;    //按键模式
    char ip[17];
    char dns[17];
} device_info;

typedef struct dhs_context{
    _config_info *conf_info;
    device_info *dev_info;
    client_info *cli_info;
}Appcontext;



#ifdef __cplusplus
}
#endif
#endif //__DHS_H__
