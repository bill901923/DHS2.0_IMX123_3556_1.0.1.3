#include <unistd.h>
#include <string.h>

#include "mpi_awb.h"

typedef struct hiPQT_AWB_NATURA_S
{
    HI_S32  (*pfnSetSpecAwbAttr)(VI_PIPE ViPipe, const ISP_SPECAWB_ATTR_S *pstSpecAwbAttr);
    HI_S32  (*pfnSetCaaControlAttr)(VI_PIPE ViPipe, const ISP_SPECAWB_CAA_CONTROl_S *pstSpecAwbCaaAttr);
    HI_S32  (*pfnGetCaaControlAttr)(VI_PIPE ViPipe, ISP_SPECAWB_CAA_CONTROl_S *pstSpecAwbCaaAttr);
    HI_S32  (*pfnGetSpecAwbAttr)(VI_PIPE ViPipe, ISP_SPECAWB_ATTR_S *pstSpecAwbAttr);
} PQT_AWB_NATURA_S;


#ifdef __NO_USE_SPECAWB__
PQT_AWB_NATURA_S g_stAwbNatura = { NULL };
#else
PQT_AWB_NATURA_S g_stAwbNatura = {HI_MPI_ISP_SetSpecAwbAttr, HI_MPI_ISP_SetCaaControlAttr,\
    HI_MPI_ISP_GetCaaControlAttr, HI_MPI_ISP_GetSpecAwbAttr};
#endif


extern int pq_control_main(int argc, char **argv);
char g_acUserWorkingPath[128] = "/sharefs";

#define ARGS_SIZE_T 10
#define ARG_BUF_LEN_T 256
static char *ptask_args_control[ARGS_SIZE_T];
static char args_buf_control[ARG_BUF_LEN_T];

static unsigned int taskid_control = -1;

void pq_control(unsigned long p0, unsigned long p1, unsigned long p2, unsigned long p3)
{
    int i = 0;
    unsigned int argc = p0;
    char** argv = (char**)p1;
    //Set_Interupt(0);
    dprintf("\ninput command:\n");

    for (i = 0; i < argc; i++)
    {
        dprintf("%s ", argv[i]);
    }

    dprintf("\n");
    pq_control_main(argc, argv);
    dprintf("\nmain out\n");
    dprintf("[END]:app_test finish!\n");
    taskid_control = -1;
}

void app_control(int argc, char **argv )
{
    int i = 0;
    int len = 0;
    char* pch = NULL;
    TSK_INIT_PARAM_S stappTask;

    if (argc < 1)
    {
        dprintf("illegal parameter!\n");
    }

    if (taskid_control != -1)
    {
        dprintf("There's a app_main task existed.");
    }

    memset(&stappTask, 0, sizeof(TSK_INIT_PARAM_S));
    memset(args_buf_control, 0, sizeof(args_buf_control));
    pch = args_buf_control;

    for (i = 0; i < ARGS_SIZE_T; i++)
    {
        ptask_args_control[i] = NULL;
    }

    argc++;
    ptask_args_control[0] = "pq_control";

    for (i = 1; i < argc; i++)
    {
        len = strlen(argv[i - 1]);
        memcpy(pch, argv[i - 1], len);
        ptask_args_control[i] = pch;
        //keep a '\0' at the end of a string.
        pch = pch + len + 1;

        if (pch >= args_buf_control + ARG_BUF_LEN_T)
        {
            dprintf("args out of range!\n");
            break;
        }
    }
	
	if(NULL != argv[0] && strcmp(argv[0], "stop"))		
	{
		chdir("/");
		memset(g_acUserWorkingPath, 0, 128);
		memcpy(g_acUserWorkingPath, argv[0], strlen(argv[0])); 		
	}

    memset(&stappTask, 0, sizeof(TSK_INIT_PARAM_S));
    stappTask.pfnTaskEntry = (TSK_ENTRY_FUNC)pq_control;
    stappTask.uwStackSize = 0x10000;
    stappTask.pcName = "pq_control";
    stappTask.usTaskPrio = 10;
    stappTask.uwResved = LOS_TASK_STATUS_DETACHED;
    stappTask.auwArgs[0] = argc;
    stappTask.auwArgs[1] = (unsigned long)ptask_args_control;
    (void)LOS_TaskCreate(&taskid_control, &stappTask);

    dprintf("Control_Task %d\n", taskid_control);

    //chdir("/sd0");
    chdir(g_acUserWorkingPath);

}
