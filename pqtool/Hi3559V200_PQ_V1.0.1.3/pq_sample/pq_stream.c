#include <unistd.h>
#include "hi_sns_ctrl.h"




/*
stSnsImx307Obj
stSnsImx307_2l_Obj
stSnsImx123Obj
stSnsImx458Obj
stSnsOs05aObj
stSnsOS08A10Obj
*/
ISP_SNS_OBJ_S *g_pstUserSnsObj[ISP_MAX_PIPE_NUM] = 
{
#if (defined(CONFIG_SNS0_IMX307))
        &stSnsImx307Obj,
#elif (defined(CONFIG_SNS0_IMX123))
        &stSnsImx123Obj,
#elif (defined(CONFIG_SNS0_IMX377))
        &stSnsImx377Obj,
#elif (defined(CONFIG_SNS0_IMX458))
        &stSnsImx458Obj,
#elif (defined(CONFIG_SNS0_OS05A))
        &stSnsOs05aObj,
#elif (defined(CONFIG_SNS0_OS08A10))
        &stSnsOS08A10Obj,
#else
        &stSnsImx307Obj,
#endif

#if (defined(CONFIG_SNS0_IMX307))
        &stSnsImx307Obj,
#elif (defined(CONFIG_SNS0_IMX123))
        &stSnsImx123Obj,
#elif (defined(CONFIG_SNS0_IMX377))
        &stSnsImx377Obj,
#elif (defined(CONFIG_SNS0_IMX458))
        &stSnsImx458Obj,
#elif (defined(CONFIG_SNS0_OS05A))
        &stSnsOs05aObj,
#elif (defined(CONFIG_SNS0_OS08A10))
        &stSnsOS08A10Obj,
#else
        &stSnsImx307Obj,
#endif
    NULL,
    NULL
};



extern int pq_stream_main(int argc, char *argv[]);
extern char g_acUserWorkingPath[128];

#define ARGS_SIZE_T 10
#define ARG_BUF_LEN_T 256
static char *ptask_args_stream[ARGS_SIZE_T];
static char args_buf_stream[ARG_BUF_LEN_T];

static unsigned int taskid_stream = -1;

void pq_stream(unsigned long p0, unsigned long p1, unsigned long p2, unsigned long p3)
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
    pq_stream_main(argc, argv);
    dprintf("\nmain out\n");
    dprintf("[END]:app_test finish!\n");
    taskid_stream = -1;
}

void app_stream(int argc, char **argv )
{
    int i = 0;
    int len = 0;
    char* pch = NULL;
    TSK_INIT_PARAM_S stappTask;

    if (argc < 1)
    {
        dprintf("illegal parameter!\n");
    }

    if (taskid_stream != -1)
    {
        dprintf("There's a app_main task existed.");
    }

    memset(&stappTask, 0, sizeof(TSK_INIT_PARAM_S));
    memset(args_buf_stream, 0, sizeof(args_buf_stream));
    pch = args_buf_stream;

    for (i = 0; i < ARGS_SIZE_T; i++)
    {
        ptask_args_stream[i] = NULL;
    }

    argc++;
    ptask_args_stream[0] = "pq_stream";

    for (i = 1; i < argc; i++)
    {
        len = strlen(argv[i - 1]);
        memcpy(pch, argv[i - 1], len);
        ptask_args_stream[i] = pch;
        //keep a '\0' at the end of a string.
        pch = pch + len + 1;

        if (pch >= args_buf_stream + ARG_BUF_LEN_T)
        {
            dprintf("args out of range!\n");
            break;
        }
    }

	if((NULL != argv[argc-2]) && (2 < argc))
	{
		len = strlen(argv[argc-2]);
		if(7 < len)
		{
			chdir("/");			
			memset(g_acUserWorkingPath, 0, 128);
			memcpy(g_acUserWorkingPath, argv[argc-2], len); 			
		}
	}
	
	
    memset(&stappTask, 0, sizeof(TSK_INIT_PARAM_S));
    stappTask.pfnTaskEntry = (TSK_ENTRY_FUNC)pq_stream;
    stappTask.uwStackSize = 0x10000;
    stappTask.pcName = "pq_stream";
    stappTask.usTaskPrio = 10;
    stappTask.uwResved = LOS_TASK_STATUS_DETACHED;
    stappTask.auwArgs[0] = argc;
    stappTask.auwArgs[1] = (unsigned long)ptask_args_stream;
    (void)LOS_TaskCreate(&taskid_stream, &stappTask);

    dprintf("Stream_Task %d\n", taskid_stream);

    chdir(g_acUserWorkingPath);

}
