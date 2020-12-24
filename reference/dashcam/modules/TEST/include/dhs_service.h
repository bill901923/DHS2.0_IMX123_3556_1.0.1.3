
#ifndef __DHS_SERVICE_H__
#define __DHS_SERVICE_H__
#include "dhs_app.h"
#ifdef __cplusplus
extern "C" {
#endif
int init_service(Appcontext *c);
void  DHSUI_shm( int item,int cast);
int str_match(const char *a, const char *b);
#ifdef __cplusplus
}
#endif
#endif //__DHS_SERVICE_H__

