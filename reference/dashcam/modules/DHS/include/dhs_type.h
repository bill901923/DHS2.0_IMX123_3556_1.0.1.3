#ifndef __DHS_TYPE_H__
#define __DHS_TYPE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "hi_appcomm.h"
#ifdef __cplusplus
extern "C" {
#endif

#define U32 unsigned int 
#define U16 unsigned short 
#define S32 int 
#define S16 short int 
#define U8  unsigned char 
#define S8  char
typedef enum
{
    DHS_FALSE = 0,
    DHS_TRUE = 1,
} DHS_BOOL;

typedef enum
{
    DHS_DEVICE_INIT = 1,
    DHS_WAITING_CONNECTION = 2,
    DHS_CLIENT_CONNECTING = 3,
    DHS_PICTURE_GETTING = 4,
    DHS_NO_MSG = 5,
} DHS_IS_STEAT;

#define UISHM_NAME "/app/UItemp" 

typedef struct _UI_shm
{
    int MSG_START;
    int MSG_ICON_p2p;
    int MSG_ICON_LEDState; // WL=1 PL=2 CPL=3 UV=4
    int MSG_ICON_set;
}UI_shm_memory;



#define DHS_NULL 0L
#define DHS_SUCCESS 0
#define DHS_FAILURE (-1)

#ifdef __cplusplus
}
#endif
#endif //__DHS_TYPE_H__