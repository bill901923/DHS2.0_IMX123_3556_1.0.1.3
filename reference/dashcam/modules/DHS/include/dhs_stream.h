#ifndef __DHS_STREAM_H__
#define __DHS_STREAM_H__


#include "dhs_isp.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef enum
{
    ST_INIT,
    ST_CON_SUCCESS,        //有链接
    ST_CON_STOP,           //链接断开
    ST_ENABLED,            //开启
    ST_DISENABLED,         //关闭
    ST_ENABLEDING,         //开启中...
    ST_DISENABLEDING,      //关闭中....

}STREAM_STATE;

typedef struct dhs_StreamState{
    void (*process)(char * data);
    void (*change_state)(STREAM_STATE st_state);
}StreamState;

typedef struct dhs_StreamService{
    int (*open)();
    int (*close)();
    int (*setAWB)(int rgain,int grgain,int gbgain,int abgain);
    STREAM_STATE state;
} StreamService;

StreamService* getStreamService();
#ifdef __cplusplus
}
#endif
#endif //__DHS_STREAM_H__