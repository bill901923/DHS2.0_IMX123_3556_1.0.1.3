/*
 * dhs_isp.c
 *
 *  Created on: 2018年8月19日
 *      Author: xll
 */


#include "hi_type.h"
#include "dhs_isp.h"
#include "sample_comm.h"
#include "hi_product_scene.h"

#define ISP_DEV  0
static int AWB_MODE = -1;
static int AE_MODE = -1;
static HI_PDT_SCENE_PIPE_PARAM_S *DHS_Set_pstScenePipeParam = HI_NULL;
// AE 曝光控制类型为手动时，若曝光参数设置超出最大(小)值，将使用 sensor 支 持的最大(小)值代替。//
//无论是自动曝光还是手动曝光，曝光时间的单位为微秒(us)，曝光增益的单位为 10bit 精度的倍数，即 1024 代表 1 倍，2048 代表 2 倍等。
int DHS_ISP_RESET(){
	AWB_MODE = -1;
	AE_MODE = -1;
	return 0;

}

uint8_t AWB_TYPE=1;

int DHS_DEFAULT_ISP_3559v200() {
	//曝光时间 DHS_PDT_SCENE_Set_Exp

	VI_PIPE ViPipe = 0;
	ISP_EXPOSURE_ATTR_S stExpAttr; 
	HI_MPI_ISP_GetExposureAttr(ViPipe, &stExpAttr);
	stExpAttr.bByPass = HI_FALSE; 
	stExpAttr.enOpType = OP_TYPE_MANUAL; 
	stExpAttr.stManual.enAGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enISPDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.u32AGain = 0x400;
	stExpAttr.stManual.u32DGain = 0x400;
	stExpAttr.stManual.u32ISPDGain = 0x400;
	stExpAttr.stManual.u32ExpTime= 0x40000; 
	DHS_PDT_SCENE_Set_Exp(ViPipe, &stExpAttr);
	return 0;
	//对比度
}
int DHS_DEFAULT_ISP() {
	//曝光时间 DHS_PDT_SCENE_Set_Exp

	ISP_EXPOSURE_ATTR_S stExpAttr;

	int r = HI_MPI_ISP_GetExposureAttr(ISP_DEV, &stExpAttr);
	stExpAttr.bByPass = HI_FALSE;
	stExpAttr.enOpType = OP_TYPE_MANUAL;
	stExpAttr.u8AERunInterval = 1;
	stExpAttr.stManual.enAGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enISPDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.u32AGain = 1450;
	stExpAttr.stManual.u32DGain = 1450;
	stExpAttr.stManual.u32ISPDGain = 2000;    //手1552->100  手 //1244->88 //1144
	stExpAttr.stManual.u32ExpTime = 50000;
	int r1 = DHS_PDT_SCENE_Set_Exp(ISP_DEV, &stExpAttr);

	printf("DHS_PDT_SCENE_Set_Exp:%d==%d\n", r, r1);

	return 0;
	//对比度
}
int DHS_Set_ISP(int exptime,int ispgain,int again,int dgain){
    ISP_EXPOSURE_ATTR_S stExpAttr;
 //   int r = HI_MPI_ISP_GetExposureAttr(ISP_DEV, &stExpAttr);
	stExpAttr.bByPass = HI_FALSE;
	stExpAttr.enOpType = OP_TYPE_MANUAL;
	stExpAttr.u8AERunInterval = 1;
	stExpAttr.stManual.enAGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enISPDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.u32AGain = again;
	stExpAttr.stManual.u32DGain = dgain;
	stExpAttr.stManual.u32ISPDGain = ispgain;    //手1552->100  手 //1244->88 //1144
	stExpAttr.stManual.u32ExpTime = exptime;
	
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>ispgain:%d \n",ispgain);
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>exptime:%d \n",exptime);
	/* DHS_ISP_SET_S Set_SceneMode;
   
		Set_SceneMode.item=1;
        Set_SceneMode.again=again;
        Set_SceneMode.dgain=dgain;
        Set_SceneMode.ispgain=ispgain;
        Set_SceneMode.exptime=exptime;

	DHS_Set_config_for_ISP(&Set_SceneMode);*/

//	int r1 = 
	DHS_PDT_SCENE_Set_Exp(ISP_DEV, &stExpAttr);
//	printf("DHS_PDT_SCENE_Set_Exp:%d==%d\n", r, r1);
	printf(" ISP command: {exptime:%d;ispgain:%d;dgain:%d;again:%d}\n",exptime,ispgain,dgain,again);
	return 0;
}


int DHS_AUTO_AWB(){
	 DHS_AWB_SET_S Set_SceneMode;
    	Set_SceneMode.DHS_AUTO=1;
		Set_SceneMode.DHS_type=OP_TYPE_AUTO;
		Set_SceneMode.item=1;
       Set_SceneMode.rgain = 256;
       Set_SceneMode.grgain = 256;
       Set_SceneMode.gbgain = 256;
       Set_SceneMode.bgain = 256;
    DHS_Set_config_for_AWB(&Set_SceneMode);
	//int r2 = DHS_PDT_SCENE_Set_Exp(ISP_DEV, pstWBAttr);
	AWB_MODE = 1;
	return 0;
}
int DHS_DEFAULT_AWB(){
//	ISP_WB_ATTR_S pstWBAttr;
	//int r1 = HI_MPI_ISP_GetWBAttr(ISP_DEV, pstWBAttr);
	//HI_MPI_ISP_GetWBAttr(ISP_DEV, pstWBAttr);
/*
	pstWBAttr.bByPass = HI_FALSE;
	pstWBAttr.enOpType = OP_TYPE_MANUAL;
	pstWBAttr.stManual.u16Rgain = 650;
	pstWBAttr.stManual.u16Grgain = 320;
	pstWBAttr.stManual.u16Gbgain = 320;
	pstWBAttr.stManual.u16Bgain = 600;
*/
	//int r2 
	 //DHS_PDT_SCENE_Set_Exp(ISP_DEV, pstWBAttr);

	return 0;
}

int DHS_Set_AWB(int rgain,int grgain,int gbgain,int bgain) {
/*	if (AWB_MODE)
	{
		return 0;
	}*/
/*	printf("set DHS_UV_AWB \r\n");ISP_WB_ATTR_S pstWBAttr;nt r1 = HI_MPI_ISP_GetWBAttr(ISP_DEV, pstWBAttr);pstWBAttr.bByPass = HI_FALSE;pstWBAttr.enOpType = OP_TYPE_MANUAL;pstWBAttr.u8AWBRunInterval = 4;pstWBAttr.stManual.u16Rgain  = rgain;pstWBAttr.stManual.u16Gbgain = gbgain;pstWBAttr.stManual.u16Grgain = grgain;pstWBAttr.stManual.u16Bgain = bgain;*/
	 DHS_AWB_SET_S Set_SceneMode;
    	Set_SceneMode.DHS_AUTO=0;
		Set_SceneMode.item=1;
		if(AWB_TYPE)
		{
			Set_SceneMode.DHS_type=OP_TYPE_AUTO;

			Set_SceneMode.StaticWB[0] = rgain;
			Set_SceneMode.StaticWB[1] = grgain;
			Set_SceneMode.StaticWB[2] = gbgain;
			Set_SceneMode.StaticWB[3] = bgain;
		}
		else
		{
			Set_SceneMode.DHS_type=OP_TYPE_MANUAL;
			Set_SceneMode.rgain = rgain;
			Set_SceneMode.grgain = grgain;
			Set_SceneMode.gbgain = gbgain;
			Set_SceneMode.bgain = bgain;
		}
		

    DHS_Set_config_for_AWB(&Set_SceneMode);
	//int r2 = DHS_PDT_SCENE_Set_Exp(ISP_DEV, pstWBAttr);
	AWB_MODE = 1;
	return 0;
}
int DHS_AUTO_ISP() {
	ISP_EXPOSURE_ATTR_S stExpAttr;
	//int r = 
	//HI_MPI_ISP_GetExposureAttr(ISP_DEV, &stExpAttr);
	stExpAttr.bByPass = HI_FALSE;
	stExpAttr.enOpType = OP_TYPE_AUTO;
	stExpAttr.stManual.enAGainOpType = OP_TYPE_AUTO;
	stExpAttr.stManual.enDGainOpType = OP_TYPE_AUTO;
	stExpAttr.stManual.enISPDGainOpType = OP_TYPE_AUTO;
	stExpAttr.stManual.enExpTimeOpType = OP_TYPE_AUTO;
	stExpAttr.stManual.u32AGain = 1024;
	stExpAttr.stManual.u32DGain = 1024;
	stExpAttr.stManual.u32ISPDGain = 1024;
	stExpAttr.stManual.u32ExpTime = 16384;
	
	  
	//int r1 = 
	DHS_PDT_SCENE_Set_Exp(ISP_DEV, &stExpAttr);

	return 0;
}

int DHS_UV_ISP() {
	ISP_EXPOSURE_ATTR_S stExpAttr;
	//int r = 
	HI_MPI_ISP_GetExposureAttr(ISP_DEV, &stExpAttr);
	stExpAttr.bByPass = HI_FALSE;
	stExpAttr.enOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enAGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enISPDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.u32AGain = 3000;
	stExpAttr.stManual.u32DGain = 3000;
	stExpAttr.stManual.u32ISPDGain = 2000;
	stExpAttr.stManual.u32ExpTime = 50000;
	
	  
	//int r1 = 
	DHS_PDT_SCENE_Set_Exp(ISP_DEV, &stExpAttr);

	return 0;
}



int DHS_NORMAL_ISP() {
	ISP_EXPOSURE_ATTR_S stExpAttr;

	int r = HI_MPI_ISP_GetExposureAttr(ISP_DEV, &stExpAttr);
	stExpAttr.bByPass = HI_FALSE;
	stExpAttr.enOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enAGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enISPDGainOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
	stExpAttr.stManual.u32AGain = 1024;
	stExpAttr.stManual.u32DGain = 1024;
	stExpAttr.stManual.u32ISPDGain = 1024;
	stExpAttr.stManual.u32ExpTime = 30384;
	int r1 = DHS_PDT_SCENE_Set_Exp(ISP_DEV, &stExpAttr);

	printf("22DHS_PDT_SCENE_Set_Exp:%d==%d\n", r, r1);
	return 0;
}

int DHS_COMTRAST_ISP_1() {
	//HI_S32 HI_MPI_ISP_SetDCFInfo(const ISP_DCF_INFO_S *pstIspDCF);
	ISP_DCF_INFO_S stIspDCF;
	//printf("11111111111111111111->u8Contrast[%d]\n",stIspDCF.u8Contrast);

	//int r3 = 
	HI_MPI_ISP_GetDCFInfo(ISP_DEV,&stIspDCF);


	//int r =
	 HI_MPI_ISP_SetDCFInfo(ISP_DEV,&stIspDCF);
	//int r1 = 
	HI_MPI_ISP_GetDCFInfo(ISP_DEV,&stIspDCF);

	//pstIspDCF.u8Contrast = 5;
	return 0;
}

int DHS_COMTRAST_ISP_2() {
	ISP_DCF_INFO_S stIspDCF;
	//stIspDCF.u8Contrast=5;
	//int r = 
		HI_MPI_ISP_SetDCFInfo(ISP_DEV,&stIspDCF);
	//int r1 = 
	HI_MPI_ISP_GetDCFInfo(ISP_DEV,&stIspDCF);

	//pstIspDCF.u8Contrast = 5;

	return 0;

}

int DHS_UV_CCM(){
	ISP_COLORMATRIX_ATTR_S pstCCMAttr;
	//int r = 
	HI_MPI_ISP_GetCCMAttr(ISP_DEV, &pstCCMAttr);
	pstCCMAttr.enOpType = OP_TYPE_MANUAL;
	pstCCMAttr.stManual.bSatEn = HI_FALSE;
	pstCCMAttr.stAuto.bISOActEn = HI_FALSE; //if enabled, CCM will bypass in low light
	pstCCMAttr.stAuto.bTempActEn = HI_TRUE; //if enabled, CCM will bypass when color temperate is larger than 10K or less than 2500K

	ISP_COLOR_TONE_ATTR_S colorTone;
	HI_MPI_ISP_SetColorToneAttr(ISP_DEV, &colorTone);
	colorTone.u16RedCastGain = 256;
	colorTone.u16GreenCastGain = 384;
	colorTone.u16BlueCastGain = 384;
	HI_MPI_ISP_GetColorToneAttr(ISP_DEV, &colorTone);
	HI_MPI_ISP_SetCCMAttr(ISP_DEV, &pstCCMAttr);

	return 0;
}


int DHS_DEFAUL_CCM(){
	ISP_COLORMATRIX_ATTR_S pstCCMAttr;
	
		int r = HI_MPI_ISP_GetCCMAttr(ISP_DEV, &pstCCMAttr);
		pstCCMAttr.enOpType = OP_TYPE_AUTO;
		pstCCMAttr.stManual.bSatEn = HI_FALSE;
		pstCCMAttr.stAuto.bISOActEn = HI_FALSE; //if enabled, CCM will bypass in low light
		pstCCMAttr.stAuto.bTempActEn = HI_TRUE; //if enabled, CCM will bypass when color temperate is larger than 10K or less than 2500K
	//	PDT_CHECK_SCENE_RET(r);
	
		
	//	HI_U16 hCCM[9] = {0x1a4,0x8096,0x800e,0x8046,0x173,0x802d,0x9,0x80a6,0x19d};
	//	HI_U16 mCCM[9] = {0x19f,0x8087,0x8018,0x8060,0x17b,0x801b,0x11,0x80bf,0x1ae};
		HI_U16 lCCM[9] = {0x1a7,0x8091,0x8016,0x8084,0x18f,0x800b,0x1b,0x8168,0x24d};

		int i,j;
		for (i = 0; i < 10; i++) {
			pstCCMAttr.stAuto.astCCMTab[0].au16CCM[i]=lCCM[i];
	//		pstCCMAttr.stAuto.au16HighCCM[i] = hCCM[i];
	//		pstCCMAttr.stAuto.au16MidCCM[i] = mCCM[i];
	//		pstCCMAttr.stAuto.au16LowCCM[i] = lCCM[i];
		}
				DHS_Set_pstScenePipeParam= PDT_SCENE_GetParam();
		pstCCMAttr.stAuto.u16CCMTabNum = (HI_U16)DHS_Set_pstScenePipeParam[0].stStaticCCM.u32TotalNum;
		for (i = 0; i < DHS_Set_pstScenePipeParam[0].stStaticCCM.u32TotalNum; i++) {
			pstCCMAttr.stAuto.astCCMTab[i].u16ColorTemp = DHS_Set_pstScenePipeParam[0].stStaticCCM.au16AutoColorTemp[i];

	      printf("DHS_DEFAUL_CCM pstCCMAttr.stAuto.astCCMTab[%d] %d\n",i,pstCCMAttr.stAuto.astCCMTab[i].u16ColorTemp);
			for (j = 0; j < CCM_MATRIX_SIZE; j++) {

	      printf("DHS_DEFAUL_CCM pstCCMAttr.stAuto.astCCMTab[%d].au16CCM[%d] %d\n",i,j,pstCCMAttr.stAuto.astCCMTab[i].au16CCM[j]);
				pstCCMAttr.stAuto.astCCMTab[i].au16CCM[j] = DHS_Set_pstScenePipeParam[0].stStaticCCM.au16AutoCCM[i][j];
			}
		}
		
		//pstCCMAttr.stAuto.u16HighColorTemp = 4900; /*RW,  Range: <=10000*/
		//pstCCMAttr.stAuto.u16MidColorTemp = 3770; /*RW,  the MidColorTemp should be at least 400 smaller than HighColorTemp*/
		//pstCCMAttr.stAuto.u16LowColorTemp = 2640; /*RW,  the LowColorTemp should be at least 400 smaller than u16MidColorTemp, >= 2000*/

		ISP_COLOR_TONE_ATTR_S colorTone;
		HI_MPI_ISP_SetColorToneAttr(ISP_DEV, &colorTone);
		colorTone.u16RedCastGain = 256;
		colorTone.u16GreenCastGain = 256;
		colorTone.u16BlueCastGain = 256;
		HI_MPI_ISP_GetColorToneAttr(ISP_DEV, &colorTone);

		int r1 = HI_MPI_ISP_SetCCMAttr(ISP_DEV, &pstCCMAttr);
		printf("HI_MPI_ISP_SetCCMAttr:%d==%d\n", r, r1);
	return 0;
}
int DHS_CCM_1() {
	ISP_COLORMATRIX_ATTR_S pstCCMAttr;
	int r = HI_MPI_ISP_GetCCMAttr(ISP_DEV, &pstCCMAttr);
	pstCCMAttr.enOpType = OP_TYPE_AUTO;
	pstCCMAttr.stManual.bSatEn = HI_FALSE;
	pstCCMAttr.stAuto.bISOActEn = HI_FALSE; //if enabled, CCM will bypass in low light
	pstCCMAttr.stAuto.bTempActEn = HI_TRUE; //if enabled, CCM will bypass when color temperate is larger than 10K or less than 2500K
	//	HI_U16 hCCM[9] = {0x1a4,0x8096,0x800e,0x8046,0x173,0x802d,0x9,0x80a6,0x19d};
	//	HI_U16 mCCM[9] = {0x1a4,0x8096,0x800e,0x8046,0x173,0x802d,0x9,0x80a6,0x19d};
	HI_U16 lCCM[9] = {0x1a4,0x8096,0x800e,0x8046,0x173,0x802d,0x9,0x80a6,0x19d};


	int i,j;
	for (i = 0; i < 10; i++) {
			pstCCMAttr.stAuto.astCCMTab[0].au16CCM[i]=lCCM[i];
	//	pstCCMAttr.stAuto.au16HighCCM[i] = hCCM[i];
	//	pstCCMAttr.stAuto.au16MidCCM[i] = mCCM[i];
	//	pstCCMAttr.stAuto.au16LowCCM[i] = lCCM[i];
	}
	DHS_Set_pstScenePipeParam= PDT_SCENE_GetParam();
		pstCCMAttr.stAuto.u16CCMTabNum = (HI_U16)DHS_Set_pstScenePipeParam[0].stStaticCCM.u32TotalNum;
		for (i = 0; i < DHS_Set_pstScenePipeParam[0].stStaticCCM.u32TotalNum; i++) {
			pstCCMAttr.stAuto.astCCMTab[i].u16ColorTemp = DHS_Set_pstScenePipeParam[0].stStaticCCM.au16AutoColorTemp[i];
			printf("DHS_CCM_1 pstCCMAttr.stAuto.astCCMTab[%d] %d\n",i,pstCCMAttr.stAuto.astCCMTab[i].u16ColorTemp);
			for (j = 0; j < CCM_MATRIX_SIZE; j++) {

	      printf("DHS_CCM_1 pstCCMAttr.stAuto.astCCMTab[%d].au16CCM[%d] %d\n",i,j,pstCCMAttr.stAuto.astCCMTab[i].au16CCM[j]);
				pstCCMAttr.stAuto.astCCMTab[i].au16CCM[j] = DHS_Set_pstScenePipeParam[0].stStaticCCM.au16AutoCCM[i][j];
			}
		}

	//pstCCMAttr.stAuto.u16HighColorTemp = 4900; /*RW,  Range: <=10000*/
	//pstCCMAttr.stAuto.u16MidColorTemp = 3770; /*RW,  the MidColorTemp should be at least 400 smaller than HighColorTemp*/
	//pstCCMAttr.stAuto.u16LowColorTemp = 2640; /*RW,  the LowColorTemp should be at least 400 smaller than u16MidColorTemp, >= 2000*/

	int r1 = HI_MPI_ISP_SetCCMAttr(ISP_DEV, &pstCCMAttr);
	printf("HI_MPI_ISP_GetCCMAttr:%d==%d\n", r, r1);
	return 0;
}

int DHS_LSC_1() {

   /*ISP_SHADING_ATTR_S*/
   // HI_BOOL bEnable;            /*RW; Range:[0, 1];Format:1.0; HI_TRUE: enable lsc; HI_FALSE: disable lsc*/
   // HI_U16  u16MeshStr;         /*RW; Range:Hi3559V200=[0, 65535];The strength of the mesh shading correction*/
   // HI_U16  u16BlendRatio;      /*RW; Range:[0, 256];Format:9.0; the blendratio of the two mesh gain lookup-table*/
	ISP_SHADING_ATTR_S pstShadingAttr;
	HI_MPI_ISP_GetMeshShadingAttr(ISP_DEV, &pstShadingAttr);
	pstShadingAttr.bEnable = HI_TRUE;
	
	//pstShadingAttr.enOpType = OP_TYPE_AUTO;
   // Mesh Grid Info: Width 
	//	int GW[8] = { 60, 60, 60, 60, 60, 60, 60, 60 };
	//	int GH[8] = { 34, 34, 34, 34, 34, 34, 33, 33 };
	// Mesh Grid Info: Height 
	//	int i;
	/*	for (i = 0; i < 8; i++) {
		pstShadingAttr.au32XGridWidth[i] = GW[i];
		pstShadingAttr.au32YGridWidth[i] = GH[i];
	}*/
	

	/*	int au32RGain[289] = {	1355, 1299, 1227, 1154, 1104, 1081, 1066, 1061, 1045, 1036, 1027, 1043, 1058, 1080, 1128, 1178, 1243,
			            	1350, 1275, 1203, 1133, 1106, 1076, 1071, 1052, 1046, 1034, 1024, 1028, 1053, 1072, 1114, 1164, 1236,
			            1322, 1261, 1186, 1135, 1101, 1099, 1082, 1071, 1062, 1054, 1053, 1053, 1055, 1081, 1105, 1159, 1232,
			            1316, 1254, 1186, 1127, 1106, 1096, 1091, 1076, 1075, 1065, 1064, 1069, 1069, 1090, 1119, 1167, 1270,
			            1324, 1260, 1171, 1132, 1104, 1100, 1100, 1096, 1085, 1087, 1080, 1073, 1083, 1095, 1112, 1160, 1225,
			            1317, 1251, 1182, 1131, 1101, 1110, 1116, 1111, 1108, 1126, 1099, 1088, 1090, 1095, 1122, 1159, 1221,
			            1325, 1247, 1185, 1131, 1114, 1116, 1115, 1136, 1124, 1124, 1109, 1110, 1106, 1108, 1142, 1163, 1221,
			            1310, 1244, 1171, 1145, 1132, 1118, 1132, 1140, 1134, 1137, 1131, 1121, 1123, 1118, 1139, 1180, 1232,
			            1318, 1237, 1179, 1154, 1135, 1128, 1138, 1153, 1154, 1152, 1145, 1136, 1125, 1113, 1148, 1186, 1244,
			            1313, 1250, 1204, 1171, 1145, 1142, 1159, 1154, 1164, 1160, 1174, 1151, 1131, 1133, 1160, 1188, 1252,
			            1316, 1257, 1214, 1169, 1159, 1156, 1158, 1162, 1178, 1176, 1177, 1160, 1148, 1146, 1168, 1220, 1261,
			            1322, 1272, 1222, 1194, 1171, 1159, 1143, 1167, 1178, 1175, 1179, 1179, 1167, 1172, 1179, 1214, 1261,
			            1325, 1280, 1238, 1201, 1180, 1163, 1155, 1171, 1191, 1194, 1185, 1173, 1170, 1176, 1189, 1219, 1273,
			            1335, 1296, 1257, 1212, 1195, 1170, 1181, 1178, 1200, 1187, 1186, 1172, 1174, 1194, 1202, 1241, 1290,
			            1331, 1311, 1271, 1226, 1204, 1185, 1171, 1180, 1189, 1193, 1188, 1176, 1196, 1203, 1226, 1263, 1296,
			            1344, 1306, 1294, 1251, 1213, 1191, 1194, 1169, 1181, 1196, 1186, 1190, 1210, 1220, 1253, 1290, 1318,
			            1358, 1327, 1321, 1268, 1231, 1208, 1207, 1178, 1186, 1208, 1200, 1213, 1209, 1247, 1279, 1314, 1320
	};
	*/
	/* Channel Gr */
	/*	int au32GrGain[289] = {
			 1409, 1333, 1243, 1156, 1100, 1075, 1062, 1059, 1041, 1033, 1028, 1042, 1056, 1083, 1139, 1203, 1283,
			            1398, 1300, 1210, 1133, 1101, 1076, 1075, 1052, 1048, 1031, 1024, 1030, 1054, 1079, 1127, 1180, 1274,
			            1367, 1289, 1192, 1137, 1099, 1104, 1086, 1076, 1065, 1059, 1062, 1059, 1061, 1087, 1115, 1181, 1268,

			            1359, 1280, 1197, 1132, 1111, 1102, 1100, 1085, 1085, 1075, 1075, 1081, 1077, 1109, 1137, 1193, 1320,

			            1365, 1289, 1184, 1137, 1108, 1111, 1115, 1110, 1100, 1106, 1099, 1091, 1098, 1118, 1127, 1186, 1257,

			            1365, 1280, 1195, 1140, 1110, 1123, 1137, 1132, 1132, 1157, 1124, 1113, 1112, 1114, 1143, 1180, 1254,

			            1378, 1277, 1204, 1142, 1127, 1135, 1139, 1161, 1151, 1158, 1137, 1136, 1130, 1130, 1170, 1190, 1262,

			            1361, 1271, 1185, 1161, 1146, 1140, 1159, 1169, 1165, 1171, 1164, 1152, 1149, 1142, 1167, 1210, 1271,

			            1365, 1263, 1196, 1168, 1152, 1151, 1163, 1187, 1189, 1189, 1178, 1170, 1152, 1136, 1176, 1222, 1287,

			            1366, 1278, 1226, 1190, 1165, 1166, 1190, 1189, 1201, 1201, 1214, 1191, 1161, 1160, 1188, 1225, 1298,

			            1377, 1287, 1235, 1190, 1181, 1182, 1192, 1195, 1216, 1219, 1218, 1200, 1181, 1170, 1198, 1256, 1305,

			            1380, 1307, 1249, 1219, 1196, 1188, 1173, 1204, 1222, 1212, 1219, 1219, 1201, 1202, 1208, 1251, 1305,

			            1389, 1324, 1269, 1229, 1209, 1192, 1185, 1204, 1236, 1236, 1227, 1210, 1205, 1210, 1220, 1258, 1321,

			            1403, 1344, 1294, 1238, 1220, 1199, 1214, 1214, 1240, 1227, 1223, 1202, 1205, 1229, 1238, 1285, 1350,

			            1397, 1363, 1312, 1250, 1231, 1212, 1197, 1211, 1223, 1232, 1228, 1207, 1233, 1239, 1262, 1313, 1359,

			            1420, 1357, 1334, 1283, 1239, 1213, 1217, 1191, 1207, 1228, 1216, 1219, 1245, 1253, 1288, 1343, 1391,

			            1434, 1383, 1366, 1299, 1257, 1230, 1231, 1195, 1209, 1236, 1223, 1249, 1240, 1289, 1322, 1371, 1388



	};
	*/
	/* Channel Gb */
	/*	int au32GbGain[289] = {

			 1412, 1330, 1241, 1152, 1099, 1074, 1060, 1056, 1039, 1032, 1026, 1039, 1055, 1081, 1137, 1200, 1275,

			            1395, 1298, 1211, 1132, 1099, 1076, 1072, 1051, 1047, 1031, 1024, 1026, 1055, 1076, 1126, 1178, 1271,

			            1370, 1288, 1193, 1134, 1097, 1101, 1086, 1076, 1066, 1057, 1061, 1058, 1060, 1089, 1116, 1180, 1266,

			            1360, 1276, 1197, 1130, 1111, 1101, 1099, 1081, 1086, 1073, 1074, 1080, 1074, 1104, 1137, 1189, 1311,

			            1366, 1289, 1182, 1135, 1107, 1108, 1114, 1109, 1100, 1105, 1097, 1088, 1099, 1114, 1128, 1183, 1254,

			            1365, 1281, 1191, 1140, 1110, 1124, 1134, 1131, 1133, 1156, 1125, 1112, 1110, 1114, 1141, 1179, 1260,

			            1386, 1276, 1203, 1144, 1122, 1134, 1137, 1163, 1150, 1160, 1139, 1138, 1130, 1129, 1169, 1190, 1257,

			            1361, 1271, 1184, 1162, 1145, 1139, 1159, 1166, 1165, 1167, 1164, 1151, 1150, 1142, 1165, 1209, 1271,

			            1369, 1261, 1196, 1169, 1152, 1148, 1164, 1185, 1187, 1186, 1178, 1170, 1151, 1134, 1174, 1218, 1286,

			            1365, 1279, 1227, 1193, 1166, 1165, 1187, 1187, 1198, 1198, 1212, 1188, 1162, 1159, 1188, 1224, 1295,

			            1374, 1286, 1234, 1190, 1178, 1182, 1188, 1193, 1215, 1217, 1214, 1199, 1180, 1171, 1197, 1257, 1302,

			            1382, 1307, 1250, 1221, 1193, 1185, 1173, 1201, 1220, 1212, 1219, 1219, 1201, 1200, 1210, 1251, 1309,

			            1385, 1322, 1269, 1227, 1207, 1191, 1186, 1204, 1237, 1237, 1228, 1210, 1202, 1206, 1219, 1258, 1319,

			            1400, 1344, 1294, 1239, 1222, 1200, 1214, 1211, 1239, 1225, 1225, 1203, 1206, 1230, 1236, 1286, 1348,

			            1393, 1362, 1308, 1252, 1231, 1213, 1197, 1208, 1219, 1228, 1225, 1206, 1228, 1238, 1263, 1311, 1358,

			            1422, 1356, 1335, 1283, 1238, 1214, 1216, 1188, 1208, 1228, 1215, 1219, 1241, 1253, 1288, 1340, 1385,

			            1426, 1385, 1364, 1298, 1259, 1227, 1224, 1192, 1202, 1229, 1222, 1246, 1234, 1283, 1321, 1373, 1388

	};
	*/
	/* Channel B */
	/*	int au32BGain[289] = {
			 1356, 1288, 1212, 1148, 1100, 1073, 1057, 1057, 1043, 1032, 1024, 1034, 1055, 1077, 1134, 1183, 1248,

			            1345, 1263, 1184, 1125, 1093, 1077, 1069, 1047, 1046, 1029, 1025, 1028, 1056, 1076, 1125, 1163, 1240,

			            1307, 1261, 1171, 1127, 1095, 1099, 1082, 1072, 1060, 1057, 1056, 1053, 1062, 1084, 1118, 1165, 1234,

			            1299, 1257, 1181, 1129, 1102, 1094, 1099, 1081, 1090, 1085, 1078, 1078, 1073, 1101, 1137, 1175, 1273,

			            1311, 1268, 1176, 1132, 1104, 1103, 1112, 1115, 1110, 1130, 1115, 1087, 1095, 1115, 1134, 1175, 1220,

			            1316, 1256, 1181, 1135, 1103, 1111, 1129, 1140, 1150, 1175, 1140, 1117, 1110, 1113, 1141, 1169, 1219,

			            1336, 1246, 1187, 1135, 1117, 1123, 1141, 1170, 1167, 1180, 1157, 1139, 1122, 1128, 1158, 1181, 1227,

			            1321, 1231, 1168, 1151, 1130, 1135, 1163, 1182, 1180, 1189, 1175, 1150, 1134, 1137, 1161, 1197, 1236,

			            1318, 1215, 1174, 1159, 1143, 1142, 1166, 1196, 1197, 1203, 1196, 1167, 1139, 1130, 1164, 1211, 1248,

			            1320, 1234, 1200, 1174, 1153, 1161, 1181, 1202, 1208, 1206, 1216, 1193, 1159, 1145, 1172, 1209, 1262,

			            1334, 1243, 1206, 1174, 1169, 1169, 1184, 1202, 1220, 1219, 1215, 1200, 1172, 1159, 1175, 1233, 1269,

			            1333, 1258, 1218, 1201, 1185, 1172, 1165, 1205, 1225, 1215, 1218, 1221, 1193, 1179, 1182, 1224, 1260,

			            1328, 1274, 1233, 1207, 1194, 1183, 1175, 1207, 1234, 1240, 1228, 1199, 1195, 1190, 1193, 1230, 1282,

			            1342, 1292, 1257, 1219, 1201, 1184, 1198, 1199, 1232, 1225, 1213, 1186, 1191, 1210, 1211, 1246, 1310,

			            1348, 1303, 1265, 1230, 1209, 1199, 1182, 1192, 1206, 1220, 1211, 1190, 1215, 1217, 1236, 1265, 1308,

			            1380, 1305, 1287, 1248, 1221, 1197, 1190, 1168, 1189, 1214, 1190, 1200, 1223, 1231, 1261, 1289, 1332,

			            1371, 1339, 1311, 1263, 1228, 1207, 1204, 1165, 1181, 1208, 1194, 1215, 1217, 1262, 1283, 1312, 1335

	};
	*/
     //Ex by eason
    /*    if (0 == u32ExpLevel || (u32ExpLevel == (s_pstScenePipeParam[u8Index].stDynamicShading.u16ExpCount - 1))) {
            stShadingAttr.u16MeshStr    = s_pstScenePipeParam[u8Index].stDynamicShading.au16MeshStr[u32ExpLevel];
            stShadingAttr.u16BlendRatio = s_pstScenePipeParam[u8Index].stDynamicShading.au16BlendRatio[u32ExpLevel];
        } else {
            stShadingAttr.u16MeshStr    = PDT_SCENE_Interpulate(u64Exposure,
                                          s_pstScenePipeParam[u8Index].stDynamicShading.au32ExpThreshLtoH[u32ExpLevel - 1],
                                          s_pstScenePipeParam[u8Index].stDynamicShading.au16MeshStr[u32ExpLevel - 1],
                                          s_pstScenePipeParam[u8Index].stDynamicShading.au32ExpThreshLtoH[u32ExpLevel],
                                          s_pstScenePipeParam[u8Index].stDynamicShading.au16MeshStr[u32ExpLevel]);
            stShadingAttr.u16BlendRatio    = PDT_SCENE_Interpulate(u64Exposure,
                                             s_pstScenePipeParam[u8Index].stDynamicShading.au32ExpThreshLtoH[u32ExpLevel - 1],
                                             s_pstScenePipeParam[u8Index].stDynamicShading.au16BlendRatio[u32ExpLevel - 1],
                                             s_pstScenePipeParam[u8Index].stDynamicShading.au32ExpThreshLtoH[u32ExpLevel],
                                             s_pstScenePipeParam[u8Index].stDynamicShading.au16BlendRatio[u32ExpLevel]);
        }*/
	int j;
	for (j = 0; j < 289; j++) {
	//	pstShadingAttr.au32RGain[j] = au32RGain[j];
	//	pstShadingAttr.au32GrGain[j] = au32GrGain[j];
	//	pstShadingAttr.au32GbGain[j] = au32GbGain[j];
	//	pstShadingAttr.au32BGain[j] = au32BGain[j];
	}

	//pstShadingAttr.u16MeshStrength = 4095;
	HI_MPI_ISP_SetMeshShadingAttr(ISP_DEV, &pstShadingAttr);

	return 0;
}

