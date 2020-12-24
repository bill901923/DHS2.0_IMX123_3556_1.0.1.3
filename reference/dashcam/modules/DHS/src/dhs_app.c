#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "sample_comm.h"
#include "dhs_app.h"
#include "dictionary.h"
#include "iniparser.h"
#include "dhs_microcirculation.h"
#include "dhs_service.h"
static int work_mode = -1;
static Appcontext *app_context = NULL;

int  check_num(char *data);
void DHS_Do_set(char type ,char len ,const char *argv[]);
void DHS_Do_Set_PWM(char len ,const char *argv[]);
void set_config(char type ,int cunt,int* item,int* val);

void DHS_DEV_Usage(char *sPrgNm)
{
    printf("Usage : %s <param>\n", sPrgNm);
    printf("param:\n");
    printf("\t -h        --for help.\n");
    printf("\t -android --use android mode.\n");
    printf("\t -ios --use ios mode.\n");
    printf("\t -uvc --use uvc mode.\n");
    printf("\t -test --use test mode.\n");
    printf("\t -updatecheck --use updatecheck.\n");
    //   printf("\t -uac      --use uac.\n");
    return;
}


void DHS_Set_Usage()
{
    printf("Usage : Set config <param>\n");
    printf("param:\n");
    printf("\t 1. WL OR UV\n");
    printf("\t 2. e :exptime.\n");
    printf("\t 3. i :ispgain.\n");
    printf("\t 4. a :again.\n");
    printf("\t 5. d :dgain\n");
    printf("\t 6. r :rgain\n");
    printf("\t 7. gb:gbgain\n");
    printf("\t 8. gr:grgain\n");
    printf("\t 9. b :bgain=\n");
    printf("\t ex: ./DHS_app Set UV 50000 2000 2000 2000 900 500 500 500\n");
    printf("\t ex: ./DHS_app Set WL e 10000 r 1000\n");
    
    //   printf("\t -uac      --use uac.\n");
    return;
}

void DHS_Do_Get()
{
    printf("DHS_Do_Get \n");
    dictionary *ini         = NULL;
 
    //加载文件
    ini = iniparser_load(CONFIG_ADDR);  
  
    printf("---------------Config set--------------- \r\n");
    printf("wl_ae_exptime = %d  \r\n",iniparser_getint(ini, "ISP:wl_ae_exptime",0));
    printf("wl_ae_ispgain = %d  \r\n",iniparser_getint(ini, "ISP:wl_ae_ispgain",0));
    printf("wl_ae_again   = %d  \r\n",iniparser_getint(ini, "ISP:wl_ae_again",0));
    printf("wl_ae_dgain   = %d  \r\n",iniparser_getint(ini, "ISP:wl_ae_dgain",0));
    printf("wl_awb_rgain  = %d  \r\n",iniparser_getint(ini, "ISP:wl_awb_rgain",0));
    printf("wl_awb_gbgain = %d  \r\n",iniparser_getint(ini, "ISP:wl_awb_gbgain",0));
    printf("wl_awb_grgain = %d  \r\n",iniparser_getint(ini, "ISP:wl_awb_grgain",0));
    printf("wl_awb_bgain  = %d  \r\n",iniparser_getint(ini, "ISP:wl_awb_bgain",0));


    printf("pl_ae_exptime = %d  \r\n",iniparser_getint(ini, "ISP:pl_ae_exptime",0));
    printf("pl_ae_ispgain = %d  \r\n",iniparser_getint(ini, "ISP:pl_ae_ispgain",0));
    printf("pl_ae_again   = %d  \r\n",iniparser_getint(ini, "ISP:pl_ae_again",0));
    printf("pl_ae_dgain   = %d  \r\n",iniparser_getint(ini, "ISP:pl_ae_dgain",0));
    printf("pl_awb_rgain  = %d  \r\n",iniparser_getint(ini, "ISP:pl_awb_rgain",0));
    printf("pl_awb_gbgain = %d  \r\n",iniparser_getint(ini, "ISP:pl_awb_gbgain",0));
    printf("pl_awb_grgain = %d  \r\n",iniparser_getint(ini, "ISP:pl_awb_grgain",0));
    printf("pl_awb_bgain  = %d  \r\n",iniparser_getint(ini, "ISP:pl_awb_bgain",0));

    printf("uv_ae_exptime = %d  \r\n",iniparser_getint(ini, "ISP:uv_ae_exptime",0));
    printf("uv_ae_ispgain = %d  \r\n",iniparser_getint(ini, "ISP:uv_ae_ispgain",0));
    printf("uv_ae_again   = %d  \r\n",iniparser_getint(ini, "ISP:uv_ae_again",0));
    printf("uv_ae_dgain   = %d  \r\n",iniparser_getint(ini, "ISP:uv_ae_dgain",0));  
    printf("uv_awb_rgain  = %d  \r\n",iniparser_getint(ini, "ISP:uv_awb_rgain",0));
    printf("uv_awb_gbgain = %d  \r\n",iniparser_getint(ini, "ISP:uv_awb_gbgain",0));
    printf("uv_awb_grgain = %d  \r\n",iniparser_getint(ini, "ISP:uv_awb_grgain",0));
    printf("uv_awb_bgain  = %d  \r\n",iniparser_getint(ini, "ISP:uv_awb_bgain",0));

    iniparser_freedict(ini);



    printf("---------------now set--------------- \r\n");
    
	   	DHS_AWB_SET_S Set_AWB;
        DHS_ISP_SET_S Set_ISP;
    DHS_Get_config_for_AWB(&Set_AWB);
    DHS_Get_config_for_ISP(&Set_ISP);
   
        
    printf("exptime = %d  \r\n",Set_ISP.exptime);
    printf("ispgain = %d  \r\n",Set_ISP.ispgain);
    printf("again   = %d  \r\n",Set_ISP.again);
    printf("dgain   = %d  \r\n",Set_ISP.dgain);  

    printf("rgain   = %d  \r\n",Set_AWB.rgain);
    printf("gbgain  = %d  \r\n",Set_AWB.gbgain);
    printf("grgain  = %d  \r\n",Set_AWB.grgain);
    printf("bgain   = %d  \r\n",Set_AWB.bgain);

    printf("StaticWB[0]   = %d  \r\n",Set_AWB.StaticWB[0]);
    printf("StaticWB[1]   = %d  \r\n",Set_AWB.StaticWB[1]);
    printf("StaticWB[2]   = %d  \r\n",Set_AWB.StaticWB[2]);
    printf("StaticWB[3]   = %d  \r\n",Set_AWB.StaticWB[3]);

}

void creat_inifile(char *name)
{
    FILE    *ini ;
    ini = fopen(name, "w");
    fprintf(ini,
    
    "\n"
    "[INIT]\n"
    "\n"
    "init       =   1\n"
    "\n"
    "[INFO]\n"
    "\n"
    "serial     =   20_0009\n"
    "name       =   DHS\n"
    "version    =   v2.0.01\n"
    "\n"
    "[LIGHT]\n"
    "\n"
    "wl_led    =   1100\n"
    "pl_led    =   1100\n"
    "cpl_led    =   1100\n"
    "uv_led     =   3000\n"
    "\n"
    "[ISP]\n"
    "\n"
    "wl_ae_exptime=19000\n"
    "wl_ae_ispgain=1024\n"
    "wl_ae_again=1024\n"
    "wl_ae_dgain=1024\n"

    "wl_awb_rgain=470\n"
    "wl_awb_gbgain=256\n"
    "wl_awb_grgain=256\n"
    "wl_awb_bgain=400\n"

    "pl_ae_exptime=19000\n"
    "pl_ae_ispgain=1024\n"
    "pl_ae_again=1024\n"
    "pl_ae_dgain=1024\n"

    "pl_awb_rgain=470\n"
    "pl_awb_gbgain=256\n"
    "pl_awb_grgain=256\n"
    "pl_awb_bgain=400\n"

    "cpl_ae_exptime=30000\n"
    "cpl_ae_ispgain=1024\n"
    "cpl_ae_again=1024\n"
    "cpl_ae_dgain=1024\n"

    "cpl_awb_rgain=470\n"
    "cpl_awb_gbgain=256\n"
    "cpl_awb_grgain=256\n"
    "cpl_awb_bgain=400\n"

    "uv_ae_exptime=40000\n"
    "uv_ae_ispgain=3000\n"
    "uv_ae_again=3000\n"
    "uv_ae_dgain=3000\n"
  
    "uv_awb_rgain=470\n"
    "uv_awb_gbgain=256\n"
    "uv_awb_grgain=256\n"
    "uv_awb_bgain=400\n"  
    "\n"
    "[WIFI]\n"
    "\n" 
    "WIFIMOD=1\n"  
    "\n"
    "[SET]\n"
    "\n" 
    "WL_AUTO=0\n"
    "PL_AUTO=0\n"
    "CPL_AUTO=0\n"
    "UV_AUTO=0\n"
    "\n" 
    "\n");
    fclose(ini);

    return;
}
int init_config(){
    
    dictionary *ini         = NULL;
 
    //加载文件
    ini = iniparser_load(CONFIG_ADDR);
    if ( ini == NULL ) {
        printf("creat_inifile!\n");
        creat_inifile(CONFIG_NAME);
        ini = iniparser_load(CONFIG_ADDR);
        if ( ini == NULL ) {
            printf("stone:iniparser_load error!\n");
            exit(-1);
        }
    }
    app_context->conf_info->global_init = iniparser_getint(ini, "INIT:init",0);
   
    strcat(app_context->conf_info->dev_serial,iniparser_getstring(ini, "INFO:serial", "null"));
    strcat(app_context->conf_info->dev_name,iniparser_getstring(ini, "INFO:name", "null"));
    strcat(app_context->conf_info->dev_version,iniparser_getstring(ini, "INFO:version", "null"));
    printf("serial: %d \n", strlen(app_context->conf_info->dev_serial));
    printf("DHS->dev_name: %s \n", app_context->conf_info->dev_name);
    printf("DHS->dev_version: %s \n", app_context->conf_info->dev_version);
    //[LIGHT]
    app_context->conf_info->wl_led = iniparser_getint(ini, "LIGHT:wl_led",0);
    app_context->conf_info->pl_led = iniparser_getint(ini, "LIGHT:pl_led",0);
    app_context->conf_info->cpl_led = iniparser_getint(ini, "LIGHT:cpl_led",0);
    app_context->conf_info->uv_led = iniparser_getint(ini, "LIGHT:uv_led",0);
    //[ISP]
    app_context->conf_info->wl_ae_exptime = iniparser_getint(ini, "ISP:wl_ae_exptime",0);
    app_context->conf_info->wl_ae_ispgain = iniparser_getint(ini, "ISP:wl_ae_ispgain",0);
    app_context->conf_info->wl_ae_again = iniparser_getint(ini, "ISP:wl_ae_again",0);
    app_context->conf_info->wl_ae_dgain = iniparser_getint(ini, "ISP:wl_ae_dgain",0);

    app_context->conf_info->wl_awb_rgain = iniparser_getint(ini, "ISP:wl_awb_rgain",0);
    app_context->conf_info->wl_awb_gbgain = iniparser_getint(ini, "ISP:wl_awb_gbgain",0);
    app_context->conf_info->wl_awb_grgain = iniparser_getint(ini, "ISP:wl_awb_grgain",0);
    app_context->conf_info->wl_awb_bgain = iniparser_getint(ini, "ISP:wl_awb_bgain",0);


    app_context->conf_info->pl_ae_exptime = iniparser_getint(ini, "ISP:pl_ae_exptime",0);
    app_context->conf_info->pl_ae_ispgain = iniparser_getint(ini, "ISP:pl_ae_ispgain",0);
    app_context->conf_info->pl_ae_again = iniparser_getint(ini, "ISP:pl_ae_again",0);
    app_context->conf_info->pl_ae_dgain = iniparser_getint(ini, "ISP:pl_ae_dgain",0);

    app_context->conf_info->pl_awb_rgain = iniparser_getint(ini, "ISP:pl_awb_rgain",0);
    app_context->conf_info->pl_awb_gbgain = iniparser_getint(ini, "ISP:pl_awb_gbgain",0);
    app_context->conf_info->pl_awb_grgain = iniparser_getint(ini, "ISP:pl_awb_grgain",0);
    app_context->conf_info->pl_awb_bgain = iniparser_getint(ini, "ISP:pl_awb_bgain",0);


    app_context->conf_info->cpl_ae_exptime = iniparser_getint(ini, "ISP:cpl_ae_exptime",0);
    app_context->conf_info->cpl_ae_ispgain = iniparser_getint(ini, "ISP:cpl_ae_ispgain",0);
    app_context->conf_info->cpl_ae_again = iniparser_getint(ini, "ISP:cpl_ae_again",0);
    app_context->conf_info->cpl_ae_dgain = iniparser_getint(ini, "ISP:cpl_ae_dgain",0);

    app_context->conf_info->cpl_awb_rgain = iniparser_getint(ini, "ISP:cpl_awb_rgain",0);
    app_context->conf_info->cpl_awb_gbgain = iniparser_getint(ini, "ISP:cpl_awb_gbgain",0);
    app_context->conf_info->cpl_awb_grgain = iniparser_getint(ini, "ISP:cpl_awb_grgain",0);
    app_context->conf_info->cpl_awb_bgain = iniparser_getint(ini, "ISP:cpl_awb_bgain",0);

    app_context->conf_info->uv_ae_exptime = iniparser_getint(ini, "ISP:uv_ae_exptime",0);
    app_context->conf_info->uv_ae_ispgain = iniparser_getint(ini, "ISP:uv_ae_ispgain",0);
    app_context->conf_info->uv_ae_again = iniparser_getint(ini, "ISP:uv_ae_again",0);
    app_context->conf_info->uv_ae_dgain = iniparser_getint(ini, "ISP:uv_ae_dgain",0);
  
    app_context->conf_info->uv_awb_rgain = iniparser_getint(ini, "ISP:uv_awb_rgain",0);
    app_context->conf_info->uv_awb_gbgain = iniparser_getint(ini, "ISP:uv_awb_gbgain",0);
    app_context->conf_info->uv_awb_grgain = iniparser_getint(ini, "ISP:uv_awb_grgain",0);
    app_context->conf_info->uv_awb_bgain = iniparser_getint(ini, "ISP:uv_awb_bgain",0);
    app_context->conf_info->wifimod=iniparser_getint(ini, "WIFI:WIFIMOD",1);
    app_context->conf_info->WL_AUTO=iniparser_getint(ini, "SET:WL_AUTO",0);
    app_context->conf_info->PL_AUTO=iniparser_getint(ini, "SET:PL_AUTO",0);
    app_context->conf_info->CPL_AUTO=iniparser_getint(ini, "SET:CPL_AUTO",0);
    app_context->conf_info->UV_AUTO=iniparser_getint(ini, "SET:UV_AUTO",0);

    if(app_context->conf_info->global_init){
        printf("!!!!!!!!!!!!!! dhs is init up app!!!!!!!!!!!!!!!!!!!!\n");
        iniparser_set(ini, "INIT:init", "0");
    }
    iniparser_freedict(ini);

    printf("init_config PASS \n");
    return 0;
}
void init_pin_mux(){
    himm(0x10FF0040,0x530);
    //BLIGHT_CONTROL/GPIO8_4
    himm(0x112F0030,0x504);
    himm(0x120D8400,0x10);
    himm(0x120D8040,0x10);
GPIO_SetIO(8,4,1);
GPIO_SetReg(8,4,1);
    led_init();
}
int init_device(){
    printf("init_device \n");
    init_pin_mux();
    app_context->dev_info = malloc(sizeof(device_info));
    app_context->cli_info = malloc(sizeof(client_info));
    
    app_context->dev_info->work_mode = work_mode;
    app_context->dev_info->dev_status = DEV_INIT;
  //  unsigned int work_mode_key = 0;
  //  SAMPLE_SYS_GetReg(0x12146008,&work_mode_key);
  //  if(work_mode_key == 0){
        app_context->dev_info->key_mod = ANDROID;
   /* }else
    {
        app_context->dev_info->key_mod = IOS;
    }
    */
    return 0;
}
int init_drive(){
    return 0;
}

int init_app(){
    printf("init_app \n");
    app_context = malloc(sizeof(Appcontext));
    app_context->conf_info = malloc(sizeof(_config_info));
    init_config();
    init_device();
    init_drive();
    return 0;
}
int main(int argc, const char *argv[])
{
    printf("================ DHS1500 DEVICE APP STARTING ===================\n\n");
    //判断客户端信息
    int i = argc;
        char OUTst=1;
        
    int ret = 0;
    ret = init_app();
    int get_flag=0;
    printf("argc :%d\n",argc);
    printf("Get1 %s\n",argv[1]);
    printf("Get2 %s\n",argv[2]);
    printf("Get3 %s\n",argv[3]);
    if (strcmp(argv[1], "test")==0)
    {             
        if (strcmp(argv[2], "SB")==0)
        { 
            while (OUTst)
            {                   
                MicrocirculationService *SB =getMicrocirculationService();            
                if (strcmp(argv[3], "open")==0)
                {
                     printf("Go  SB open\n");
                    SB->open();
                }
                else if (strcmp(argv[3], "close")==0)
                {
                 printf("Go  SB close\n");
                    SB->close();
                    OUTst=0;
                }
                else if (strcmp(argv[3], "reset")==0)
                {
                    SB->reset();
                }
                argv[2]="\n";
                argv[3]="\n";
                if (SB->mcl_state == MCL_DISENABLED){
                    
                    printf("MCL_DISENABLED\n");
                    OUTst=0;
                }
            }
            printf("exit SB \n");
        }
        else if (strcmp(argv[2], "cut")==0)
        {
                if (strcmp(argv[3], "openA")==0)
                {
                    cutA_open();
                }
                else if  (strcmp(argv[3], "openB")==0)
                {
                    cutB_open();
                }
                else if  (strcmp(argv[3], "open")==0)
                {
                    cutB_open();
                    cutA_open();
                }
                else if (strcmp(argv[3], "closeA")==0)
                {
                    cutA_close();
                }
                else if (strcmp(argv[3], "closeB")==0)
                {
                    cutB_close();
                }
                else if (strcmp(argv[3], "close")==0)
                {
                    cutA_close();
                    cutB_close();
                }
                argv[2]="\n";
                argv[3]="\n";
        }
        else if (strcmp(argv[2], "Led")==0)
        {
            printf("Get4 %s\n",argv[4]);
            int Val=0;
           
            if (strcmp(argv[3], "open_uv")==0)
            {
                 if(strlen(argv[4])>0)
            {
                    Val=atoi(argv[4]);
            }
                    led_init();
                    led_uv_open(Val);
            }                
            else if  (strcmp(argv[3], "open_w")==0)
            {
                 if(strlen(argv[4])>0)
            {
                    Val=atoi(argv[4]);
            }
                led_init();
                led_wl1_open(Val);
            }
            else if (strcmp(argv[3], "close")==0)
            {
                led_all_close();
            }
            argv[2]="\n";
            argv[3]="\n";
            argv[4]="\n";
        }
        else if (strcmp(argv[2], "BT")==0)
        {
                if (strcmp(argv[3], "open")==0)
                {
                    cutA_open();
                }
                else if  (strcmp(argv[3], "openB")==0)
                {
                    cutB_open();
                }
                else if  (strcmp(argv[3], "open")==0)
                {
                    cutB_open();
                    cutA_open();
                }
                else if (strcmp(argv[3], "closeA")==0)
                {
                    cutA_close();
                }
                else if (strcmp(argv[3], "closeB")==0)
                {
                    cutB_close();
                }
                else if (strcmp(argv[3], "close")==0)
                {
                    cutA_close();
                    cutB_close();
                }
                argv[2]="\n";
                argv[3]="\n";
        }       
       
    }
    else if (strcmp(argv[1], "Set")==0)
    {
        
         printf("in to  %s\n",argv[1]);
        if (strcmp(argv[2], "UV")==0)
        {
            DHS_Do_set(0,argc,argv);
        }
        else if (strcmp(argv[2], "WL")==0)
        { 
            DHS_Do_set(2,argc,argv);
        } 
        else if (strcmp(argv[2], "PL")==0)
        { 
            DHS_Do_set(1,argc,argv);
        }         
        else if (strcmp(argv[2], "Get")==0)
        { 
            DHS_Do_Get();
        }       
        else if (strcmp(argv[2], "PWM")==0)
        { 
            DHS_Do_Set_PWM(argc,argv);
        }
        else if (strcmp(argv[2], "-h")==0)
        {
            DHS_Set_Usage();
        }
    }
    else
    {    
        while (i > 1)
        {
            if (strcmp(argv[i - 1], "-android") == 0)
            {
                work_mode = 0;
                get_flag=1;
            }
            else if (strcmp(argv[i - 1], "-ios") == 0)
            {
                work_mode = 1;
                get_flag=1;
            }
            else if (strcmp(argv[i - 1], "-uvc") == 0)
            {
                work_mode = 2;
                get_flag=1;
            }
            else if (strcmp(argv[i - 1], "-test") == 0)
            {
                work_mode = 3;
                get_flag=1;
            }
            if (strcmp(argv[i - 1], "-updatecheck") == 0)
            {
                printf("dong");
                return 0;
            }
            if (strcmp(argv[i - 1], "-h") == 0)
            {
                DHS_DEV_Usage((char*)argv[0]);
            }
            i--;
        }
        if(get_flag)
        {
            ret = init_service(app_context);
            if (ret!=0)
            {

                printf("reboot by init_service fail \r\n");
               system("reboot");
            }
        }
    }
    return 0;
}
/*
int  check_num(char *data)
{
    int len =strlen(data);
    int start=1;
    for(char i=0;i<len;i++)
    {
        if((data[i]<48)||(data[i]>57))
        {
               start= 0;
              i=len+1;
        }
       
    }

    return start;
}*/

int  check_num(char *data)
{
    int i = atoi(data);
    char temp[10] ;
    sprintf(temp,"%d",i);
   // printf("%s  = %s \r\n",temp,data);
    if (strlen(temp) == strlen(data))
    {
        
        return 1 ;
    }
    else
    {
        printf("Not Num %d %d\r\n",strlen(temp),strlen(data));
        return 0;
    }
}
void DHS_Do_Set_PWM(char len ,const char *argv[])
{
    int i=3;
    int dataget=0;
    int temp1[3]={0,0,0};
    int temp2[3]={0,0,0};
    while (len >i)
    {
        if (strcmp(argv[i], "UV")==0)
        {
            temp1[dataget]=1;
        }
        else if (strcmp(argv[i], "WL")==0)
        { 
            temp1[dataget]=2;
        } 
        else if (strcmp(argv[i], "PL")==0)
        {
            temp1[dataget]=3;
        }
        else if(check_num((char *)argv[i]))
        {   
            if(temp1[dataget]==0)
            {
                if(dataget)
                {
                    temp1[dataget]=temp1[dataget-1]+1;
                }
                else
                {
                    temp1[dataget]=1;
                }
            }
       //     printf("Set %s\r\n",(char *)argv[i]);
            temp2[dataget++]=atoi((char *)argv[i]);
        }
        else
        {
            printf("\t Wrong input\n");
        }         
        i++;
    }
    if(dataget)
    {        
            set_config(3,dataget,temp1,(int *)temp2);
    }
    memset(temp1,0,3);
    memset(temp2,0,3);
}
void DHS_Do_set(char type ,char len ,const char *argv[])
{
    int i=3;
    int dataget=0;
    int temp1[9]={0};
    int temp2[9]={0};
    printf("DHS_Do_set\r\n");
    while (len >i)
    {
        
   // printf("%d %s\r\n",i,(char *)argv[i]);
        if (strcmp(argv[i], "e")==0)
        {
            temp1[dataget]=1;
        }
        else if(strcmp(argv[i], "i")==0)
        {
            temp1[dataget]=2;
        }
        else if(strcmp(argv[i], "a")==0)
        {
            temp1[dataget]=3;
        }
        else if(strcmp(argv[i], "d")==0)
        {
            temp1[dataget]=4;
        }
        else if(strcmp(argv[i], "r")==0)
        {
            temp1[dataget]=5;
        }
        else if(strcmp(argv[i], "gb")==0)
        {
            temp1[dataget]=6;
        }
        else if(strcmp(argv[i], "gr")==0)
        {
            temp1[dataget]=7;
        }
        else if(strcmp(argv[i], "b")==0)
        {
            temp1[dataget]=8;
        }
        else if(check_num((char *)argv[i]))
        {
            if(temp1[dataget]==0)
            {
                if(dataget)
                {
                    temp1[dataget]=temp1[dataget-1]+1;
                }
                else
                {
                    temp1[dataget]=1;
                }
            }
       //     printf("Set %s\r\n",(char *)argv[i]);
            temp2[dataget++]=atoi((char *)argv[i]);
        }
        else
        {
            printf("\t Wrong input\n");
        }
        i++;
    }
    if(dataget)
    {        
            set_config(type,dataget,temp1,(int *)temp2);
    }
    memset(temp1,0,9);
    memset(temp2,0,9);
}

void set_config(char type ,int cunt,int* item,int* val)
{
    
    dictionary *ini         = NULL;
    char data[20];
    int j;
    //加载文件
    
  //  printf("Set %d num %d CONFIG_ADDR: %s \n",type,cunt, CONFIG_NAME);
    
    ini = iniparser_load(CONFIG_NAME);

 //   printf("iniparser_load\n");
    switch(type)
    {
        case 0:
        for(j=0;j<cunt;j++)
        {
            
          //  printf("Set %d\n",val[j]);
             sprintf(data,"%d",val[j]);
            //itoa(data,val[j],20);
          //  printf("item[%d]=%d Set %s\n",j,item[j],data);
           switch(item[j])
            {
                case 1:
                    iniparser_set(ini,"ISP:uv_ae_exptime",data); 
                    app_context->conf_info->uv_ae_exptime =val[j]; 
                    break;
                case 2:iniparser_set(ini,"ISP:uv_ae_ispgain",data); app_context->conf_info->uv_ae_ispgain =val[j];     break;
                case 3:iniparser_set(ini,"ISP:uv_ae_again",data);   app_context->conf_info->uv_ae_again   =val[j];     break;
                case 4:iniparser_set(ini,"ISP:uv_ae_dgain",data);   app_context->conf_info->uv_ae_dgain   =val[j];    break;
                case 5:iniparser_set(ini,"ISP:uv_awb_rgain",data);  app_context->conf_info->uv_awb_rgain  =val[j];   break;
                case 6:iniparser_set(ini,"ISP:uv_awb_gbgain",data); app_context->conf_info->uv_awb_gbgain =val[j];   break;
                case 7:iniparser_set(ini,"ISP:uv_awb_grgain",data); app_context->conf_info->uv_awb_grgain =val[j];   break;
                case 8:iniparser_set(ini,"ISP:uv_awb_bgain",data);  app_context->conf_info->uv_awb_bgain  =val[j];   break;
            }
            
        }
   
        DHS_Set_AWB(app_context->conf_info->uv_awb_rgain,app_context->conf_info->uv_awb_grgain,app_context->conf_info->uv_awb_gbgain,app_context->conf_info->uv_awb_bgain);
        DHS_Set_ISP(app_context->conf_info->uv_ae_exptime, app_context->conf_info->uv_ae_ispgain, app_context->conf_info->uv_ae_again, app_context->conf_info->uv_ae_dgain);
        break;

        case 1:
        for(j=0;j<cunt;j++)
        {
            
          //  printf("Set %d\n",val[j]);
             sprintf(data,"%d",val[j]);
            //itoa(data,val[j],20);
          //  printf("item[%d]=%d Set %s\n",j,item[j],data);
           switch(item[j])
            {
                case 1:
                    iniparser_set(ini,"ISP:pl_ae_exptime",data); 
                    app_context->conf_info->pl_ae_exptime =val[j]; 
                    break;
                case 2:iniparser_set(ini,"ISP:pl_ae_ispgain",data); app_context->conf_info->pl_ae_ispgain =val[j];     break;
                case 3:iniparser_set(ini,"ISP:pl_ae_again",data);   app_context->conf_info->pl_ae_again   =val[j];     break;
                case 4:iniparser_set(ini,"ISP:pl_ae_dgain",data);   app_context->conf_info->pl_ae_dgain   =val[j];    break;
                case 5:iniparser_set(ini,"ISP:pl_awb_rgain",data);  app_context->conf_info->pl_awb_rgain  =val[j];   break;
                case 6:iniparser_set(ini,"ISP:pl_awb_gbgain",data); app_context->conf_info->pl_awb_gbgain =val[j];   break;
                case 7:iniparser_set(ini,"ISP:pl_awb_grgain",data); app_context->conf_info->pl_awb_grgain =val[j];   break;
                case 8:iniparser_set(ini,"ISP:pl_awb_bgain",data);  app_context->conf_info->pl_awb_bgain  =val[j];   break;
            }
            
        }
   
        DHS_Set_AWB(app_context->conf_info->pl_awb_rgain,app_context->conf_info->pl_awb_grgain,app_context->conf_info->pl_awb_gbgain,app_context->conf_info->pl_awb_bgain);
        DHS_Set_ISP(app_context->conf_info->pl_ae_exptime, app_context->conf_info->pl_ae_ispgain, app_context->conf_info->pl_ae_again, app_context->conf_info->pl_ae_dgain);
        break;
        case 2:
        for(j=0;j<cunt;j++)
        {
            sprintf(data,"%d",val[j]);
            printf("item[%d]=%d SET %s\n",j,item[j],data);
            switch(item[j])
            {
                case 1:iniparser_set(ini,"ISP:wl_ae_exptime",data); app_context->conf_info->wl_ae_exptime =val[j];    break;
                case 2:iniparser_set(ini,"ISP:wl_ae_ispgain",data); app_context->conf_info->wl_ae_ispgain =val[j];    break;
                case 3:iniparser_set(ini,"ISP:wl_ae_again",data);   app_context->conf_info->wl_ae_again   =val[j];    break;
                case 4:iniparser_set(ini,"ISP:wl_ae_dgain",data);   app_context->conf_info->wl_ae_dgain   =val[j];    break;
                case 5:iniparser_set(ini,"ISP:wl_awb_rgain",data);  app_context->conf_info->wl_awb_rgain  =val[j];    break;
                case 6:iniparser_set(ini,"ISP:wl_awb_gbgain",data); app_context->conf_info->wl_awb_gbgain =val[j];    break;
                case 7:iniparser_set(ini,"ISP:wl_awb_grgain",data); app_context->conf_info->wl_awb_grgain =val[j];    break;
                case 8:iniparser_set(ini,"ISP:wl_awb_bgain",data);  app_context->conf_info->wl_awb_bgain  =val[j];   break;
            }
        }
            DHS_Set_AWB(app_context->conf_info->wl_awb_rgain,app_context->conf_info->wl_awb_grgain,app_context->conf_info->wl_awb_gbgain,app_context->conf_info->wl_awb_bgain);
            DHS_Set_ISP(app_context->conf_info->wl_ae_exptime,app_context->conf_info->wl_ae_ispgain,app_context->conf_info->wl_ae_again,app_context->conf_info->wl_ae_dgain);
        break;
        case 3: //SET PWM
        for(j=0;j<cunt;j++)
        {
            
          //  printf("Set %d\n",val[j]);
             sprintf(data,"%d",val[j]);
            //itoa(data,val[j],20);
          //  printf("item[%d]=%d Set %s\n",j,item[j],data);
           switch(item[j])
            {
                case 1:iniparser_set(ini,"LIGHT:uv_led",data); app_context->conf_info->uv_led =val[j];  break;
                case 2:iniparser_set(ini,"LIGHT:wl_led",data); app_context->conf_info->wl_led =val[j];  break;
                case 3:iniparser_set(ini,"LIGHT:pl_led",data);   app_context->conf_info->pl_led   =val[j];  break;
                case 4:iniparser_set(ini,"LIGHT:cpl_led",data);   app_context->conf_info->cpl_led   =val[j];  break;
            }
        }
   
         break;
    }
    
 //   printf("fopen\n");
    FILE * f = fopen(CONFIG_NAME,"w");
    
 //   printf("iniparser_dump_ini\n");

    iniparser_dump_ini(ini, f);
//    printf("fclose\n");
    fclose(f);
//    printf("iniparser_freedict\n");
    iniparser_freedict(ini);
    
}
