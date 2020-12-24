
#ifndef __DHS_ISP_H__
#define __DHS_ISP_H__

#include <pthread.h>
#include <errno.h>
#include <sys/prctl.h>
#include "dhs_app.h"
#include "sample_comm.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "hi_comm_isp.h"
#include "hi_product_scene_setparam.h"
#include "product_scene_setparam_inner.h"
#include "hi_product_scene.h"
#include "hi_ipcmsg.h"
#include "hi_appcomm_msg_client.h"
//amp/a7_linux/mpp/include/hi_comm_isp.h
//\reference\dashcam\modules\scene\core\server\product_scene_setparam_inner.h
#ifdef __cplusplus
extern "C" {
#endif
int DHS_ISP_RESET();
int DHS_Set_AWB(int rgain,int grgain,int gbgain,int abgain);
int DHS_Set_ISP(int exptime,int ispgain,int again,int dgain);
int DHS_AUTO_AWB();
int DHS_AUTO_ISP();
#ifdef __cplusplus
}
#endif
#endif //__DHS_ISP_H__

