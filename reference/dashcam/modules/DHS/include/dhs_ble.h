
#ifndef __DHS_BLE_H__
#define __DHS_BLE_H__

//#include "uart.h"
#include "dhs_uart.h"
#include "dhs_type.h"
#include "dhs_app.h"
#include "sample_comm.h"
#include "utils.h"

#define BLE_UART_DEV  "/dev/ttyAMA4"
//#define BLE_UART_DEV  "dev/uartdev-1"
#define BLE_REC_DATA_LEN  256
#define DHS_GIPO_FOR_BTWORK  2              //蓝牙工作io脚
#ifdef __cplusplus
extern "C" {
#endif

#define set_BT_State_USE himm(0x112F0004,0x500)	
#define set_BT_CTRL_USE himm(0x112F0000,0x500) 	 
#define set_BT_CTRL_On himm(0x120DA100,0x40) 	//GPIO10_6
#define set_BT_CTRL_Off himm(0x120DA100,0x00) 	//GPIO10_6

typedef enum
{
    BLE_BROADCAST = 30,          //开始广播
    BLE_CON_SUCCESS = 31,        //有链接
    BLE_CON_STOP = 32,           //链接断开
    BLE_ENABLED = 33,            //开启
    BLE_DISENABLED = 34,         //关闭
} BLE_STATE;

# define MAX_MSG_LEN 128
# define MSQ_KEY 21012
 
typedef struct MSGQUEUE
{
	long msgType;
    int len;
	char msgText[MAX_MSG_LEN];
}BLE_Msg;

typedef struct dhs_BleState{
    void (*process)(char * data);
    void (*changeState)(BLE_STATE ble_state);
}BleState;

typedef struct dhs_BLEService{
    BLE_STATE ble_state;
    char* name;
    int isSetName;
    int (*open)(BleState *state,_config_info *cf);
    int (*work)();
    int (*close)();
    int (*reset)();
    int (*disClient)();
    int (*sendMsg)(const char * data,int len,int type);
} BLEService;

BLEService* getBleService();
int check_str(char *s, char *c);
#ifdef __cplusplus
}
#endif
#endif //__DHS_BLE_H__

