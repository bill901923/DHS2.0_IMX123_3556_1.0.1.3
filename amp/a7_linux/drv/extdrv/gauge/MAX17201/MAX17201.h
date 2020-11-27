#ifndef __MAX17201_H__
#define __MAX17201_H__
//#include <stdint.h>
//#include "MAX17201_config.h"
/*
|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|   REGISTER TYPE   |    LSB SIZE       |   MINIMUM VALUE   |    MAXIMUM VALUE     |                                           NOTES                                       | 
|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|     Capacity      |  5.0μVh/RSENSE    |       0.0μVh      |  327.675mVh/RSENSE   |                     Equivalent to 0.5mA with a 0.010Ω sense resistor                  |
|     Percentage    |     1/256%        |       0.0%        |       255.9961%      |                     1% LSb when reading only the upper byte.                          |
|     Voltage       |   0.078125mV      |       0.0V        |       5.11992V       |                                                —                                      |
|     Current       | 1.5625μV/RSENSE   |   -51.2mV/RSENSE  |   51.1984mV/RSENSE   |   Signed 2's complement format. Equivalent to 156.25μA with a 0.010Ω sense resistor   |
|     Temperature   |     1/256°C       |      -128.0°C     |       127.996°C      |       Signed 2's complement format. 1°C LSb when reading only the upper byte.         |
|     Resistance    |     1/4096Ω       |       0.0 Ω       |       15.99976Ω      |                                                —                                      |
|     Time          |     5.625s        |        0.0s       |       102.3984h      |                                                —                                      |
|     Special       |        —          |         —         |         —            |                Format details are included with theregister description.              |
|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
*/


//SLAVE  ADDRESS      
//6Ch	00h–FFh
//16h	80h–FFh



//#define MAX1720x_SLAVE_ADDRESS_1 0x36
//#define MAX1720x_SLAVE_ADDRESS_2 0x0B


//060H COMMAND 
#define MAX1720x_COMMAND 0x060
/*
CommStat
D2 		D1
NVError NVBusy

NVError 该只读位跟踪非易失性存储器是忙还是闲。
NVBusy  该位指示发送到命令寄存器的上一个SHA-256或非易失性存储器相关命令的结果。
*/
#define Read_CommStat 0x061 

/*
Lock
LOCK1: Locks register pages 1Ah, 1Bh
LOCK2: Locks register pages 01h, 02h, 03h, 04h, 0Bh, 0Dh
LOCK3: Locks register pages 18h, 19h
LOCK4: Locks register pages 1Ch
LOCK5: Locks register pages 1Dh
*/
#define Read_Lock_State 0x07Fh //Lock 读锁状态

/*
ODSCTh
D12 D11 D10 D9 D8			D4 D3 D2 D1 D0
SCTH						ODTH

SCTH 短路阈值设置						
ODTH 过度放电阈值设置
*/
#define Set_ODSCTh 0x0F2

/*
ODSCCfg
D15 D14		D13 D12 	D11 D10 D9 D8 	D7 	D6 		D5 	D4 	D3 D2 D1 D0
SCi SCen 	X 	X	 	SCDLY 			ODi ODen 	X 	X 	ODDLY

SCi 短路指示器
SCen 短路警報使能
SCDLY 设置短路的锁存和延迟 2μs  917us
ODi 过度放电指示器
ODen 过度放电警報使能
ODDLY 设置过度放电的锁存和延迟 μs2μs ~ 14.657ms
*/
#define Set_ODSCCfg 0x0F3

/*
MODELGAUGE m5 REGISTER VFOCV 由电压电量计确定的计算出的电池开路电压。unit:V
*/
#define Read_VFOCV 0x0FB 

/*MODELGAUGE m5 REGISTER VFSOC 根据电压电量表保存计算出的当前电池充电状态。unit:%
*/
#define Read_VFSOC 0x0FF 

/*
Status   部分與 Config 連動
D15 D14 D13 D12 D11 D10 D9 	D8 	D7 		D6	D3 	D2 	D1 
Br 	Smx Tmx Vmx Bi 	Smn Tmn Vmn dSOCi 	Imx Bst Imn POR 
POR：上电复位。
当设备检测到发生软件或硬件POR事件时，此位设置为1。该位必须由系统软件清除，以检测下一个POR事件。
def:1。

Imn：超过最小電流警报阈值。自动清除。def:0。

Bst：电池状态。当系统中有电池时，此位设置为0，而当电池不存在时，此位设置为1。def:0。

Imx：超过最大電流警报阈值。该位自动清除。def:0。

dSOCi：充电状态1％更改警报。每当RepSOC寄存器越过整数百分比的边界（例如50.0％，51.0％等）时，将其设置为1。必须由主机软件清除。def:0。

Vmn：超过最低电压警报阈值。def:0。

Tmn：超过最低温度警报阈值。def:0。

Smn：超出最低SOC警报阈值。 def:0。

Bi：插入电池。当器件通过监视AIN1引脚检测到电池已插入系统时，此位设置为1。该位必须由系统软件清除	def:0。

Vmx：超过最大电压警报阈值。def:0。

Tmx：超过最大温度警报阈值。def:0。

Smx：超过最大SOC警报阈值。def:0。

Br：卸下电池。当系统检测到电池已从系统中卸下时，此位设置为1。该位必须由系统软件清除，def:0。

*/ 
#define Status       		0X000	 
/*
Status2   部分與 Config2 連動
D1 	D0
Hib PORSkip

PORSkip：重置期间跳过IC RAM的初始化时，此位置1。
这发生在复位之前RAM值已经有效的情况下，例如，通过软件将固件重启命令发送到IC时。

Hib：休眠状态。
当设备处于休眠模式时，此位设置为1；当设备处于活动模式时，此位设置为0。 上电时Hib设置为0。
*/
#define Status2			    0X0B0	

/*
VAlrtTh		电壓阈值	1:20mV
TAlrtTh		溫度阈值	1:2's complement with1度 正負值
SAlrtTh		充電电阈值	1:1%
IAlrtTh		电流阈值	1:2's complement with 400μV 正負值

D15 D14 D13 D12 D11 D10 D9 D8 	D7 D6 D5 D4 D3 D2 D1 D0
MAX 							MIN

*/
#define VAlrtTh      		0X001	 
#define TAlrtTh      		0X002	 
#define SAlrtTh      		0X003	
#define IAlrtTh   			0X0B4	
/*
Config 部分與 Status 連動
D14 D13 D12 D11 	D10 	D9 	D8 	D7 		D6 		D4 		D3 		D2 	D1 	D0
SS 	TS 	VS 	ALRTp 	AINSH 	Ten Tex SHDN 	COMMSH	ETHRM 	FTHRM 	Aen Bei Ber

Ber：启用电池移除警报。当Ber = 1时，通过AIN引脚电压检测到电池取出状态将触发警报。请注意，如果此位设置为1，则ALSH位应设置为0，以防止警报条件导致设备进入关闭模式。
Bei：启用电池插入警报。当Bei = 1时，由AIN引脚电压检测到的电池插入情况会触发警报。请注意，如果此位设置为1，则ALSH位应设置为0，以防止警报条件导致设备进入关闭模式。
Aen：启用电量计输出警报。1:如果电流，电压或SOC超过任何警报阈值寄存器值，则会触发警报。该位仅影响ALRT1引脚的操作。。请注意，如果此位设置为1，则ALSH位应设置为0，以防止警报条件导致设备进入关闭模式。

FTHRM：强制热敏电阻偏置开关。这使主机可以控制热敏电阻开关的偏置或实现对电池取出的快速检测。设置FTHRM = 1始终使能热敏电阻偏置开关。使用标准的10kΩ热敏电阻，这会为电路的电流消耗增加约200μA的电流。
ETHRM：启用热敏电阻。1:启用自动THRM输出偏置和AIN1 / AIN2测量.COMMSH：通信关闭。如果将SDA和SCL都保持低电平（MAX1720x）或DQ保持低电平（MAX1721x）超过ShdnTimer的超时时间，则设置为逻辑1以强制器件进入关闭模式。
寄存器。
这也将设备配置为在任何通信的上升沿唤醒。请注意，如果COMMSH和AINSH都设置为0，则器件将唤醒DQ / SDA或OD / SCL引脚中任何一个的边沿。请参阅“操作模式”部分。

SHDN：Shutdown。1: ShdnTimer寄存器超时（默认为45s延迟）后强制关闭设备。上电时和退出关闭模式时，SHDN会重置为0。为了在45秒内命令关闭，首先写入HibCFG = 0x0000进入活动模式。

Tex：外部温度。

Ten：启用温度通道。设置为1并将ETHRM或FTHRM设置为1以启用 PackCfg寄存器中定义的温度测量。

AINSH：AIN1 功能開啟。1:卸下电池后关闭设备。如果AIN1引脚保持高电平（AIN1> VTHRM-VDET）的时间超过ShdnTimer寄存器的超时时间，进入关断状态。这也将设备配置为在电池上将AIN1拉低时唤醒
插入。请注意，如果COMMSH和AINSH都设置为0，则器件将唤醒DQ / SDA或OD / SCL引脚中任何一个的边沿。

ALRTp：ALRT1 Pin Polarity。 0:ALRT1 pin is active low  	1:ALRT1 pin is active high.
VS：电压 ALRT1 Sticky。1:手動清除温度警报。0:自动清除温度警报。
TS：温度 ALRT1 Sticky。1:手動清除温度警报。0:自动清除温度警报。
SS：SOC ALRT1 Sticky。1:手動清除温度警报。0:自动清除温度警报。

*/
#define Config       		0X01D	


/*
Config2 部分與 Status2 連動

D7 		D6 			D0
dSOCen 	TAlrtEn 	POR_CMD


POR_CMD：固件重启。将此位设置为1可重新启动IC固件操作，而无需将非易失性存储器重新调用到RAM中。这允许在不更改非易失性存储器设置的情况下测试不同的IC配置。def:0，并在固件重启后自动清除。请参阅重置命令。

TAlrten：温度警报启用 def:1。

dSOCen：1％ SOC更改警报启用。def:0。
*/
#define Config2		        0X0BB	 
 
 
 

#define RepCap       		0X005	// 當前容量 mAh	
#define AvCap        		0X01F	
#define MixCap       		0X00F
#define FullCap      		0X010 	//该寄存器根据ModelGauge m5算法 保存计算出的电池满容量。 
#define FullCapRep   		0X035   // 全部容量

// ((MSB*256)+LSB)/256
#define RepSOC       		0X006	// 充电百分比输出状态。 is a filtered version of the AvSOC register
#define AvSOC        		0X00E	// 根据ModelGauge m5算法 後充电百分比输出状态
#define MixSOC       		0X00D	//補償前 充电百分比
 
/*理论剩余时间或容量。*/ 
#define AtRate       		0X004	//负载电流   當電流為負值時 以下有效
#define AtAvCap		        0X0DF	//估计剩余容量。 µVh/0.001(感测电阻值)
#define AtAvSOC		        0X0DE	//保存电池的理论电荷状态  0.0039％ 
#define AtQResidual		    0X0DC	//剩余容量
#define AtTTE		        0X0DD	//清空时间(剩餘時間)

 /*溫度*/
 
#define Temp         		0X008	 //根据nPackCfg寄存器的设置，選擇讀取的溫度值 
#define AvgTA        		0X016	 // 平均溫度
#define MaxMinTemp   		0X01A	//最大和最小溫度值。
 
 /*電流*/
#define Current      		0X00A	// (數值*156.25uA)	      
#define AvgCurrent   		0X00B  // 平均電流
#define MaxMinCurr   		0X01C	//最大和最小電流值。


 /*電壓*/
#define VCell        		0X009	   
#define AvgVCell     		0X019   // 平均
#define MaxMinVolt  		0X01B	//最大和最小電壓值。
 
 
 /*時間*/ 
#define TTE          		0X011	//剩餘時間
#define TTF          		0X020	//充滿時間
 
#define Age          		0X007	 //当前电池容量与其预期容量的百分比计算值 	 
#define QResidual    		0X00C	// mAH 计算出的电荷量
#define FullSocThr   		0X013	//nFullSOCThr寄存器控制充电结束的检测。推荐的nFullSOCThr寄存器设置为95％。 对于EZ Performance应用程序，推荐为80％（0x5005）。
#define RCell		   		0X014	 //內電阻
#define RFast        		0X015	 
#define Cycles       		0X017	//电池充电/放电周期总数。    
#define DesignCap    		0X018	  
#define IChgTerm     		0X01E	  
#define DevName      		0X021	 //DevName寄存器保存设备类型和固件版本信息   
#define FullCapNom  		0X023		
#define AIN0         		0X027	  
#define LearnCfg     		0X028	  
#define FilterCfg    		0X029	  
#define RelaxCfg     		0X02A	  
#define MiscCfg      		0X02B	  
#define TGain        		0X02C	  
#define TOff         		0X02D	  
#define CGain        		0X02E	  
#define COff         		0X02F		     
#define IAvgEmpty    		0X036			
#define RComp0       		0X038	     
#define TempCo       		0X039	     
#define VEmpty     			0X03A	
#define FStat        		0X03D	     
#define Timer        		0X03E	     
#define ShdnTimer    		0X03F	
#define dQAcc       		0X045	
#define dPAcc 				0X046	
#define VFRemCap 			0X04A	
#define QH  			    0X04D	
#define VShdnCfg    		0X0B8	
#define AgeForecast 		0X0B9	
#define HibCfg      		0X0BA	
#define VRipple     		0X0BC	
#define PackCfg     		0X0BD	
#define TimerH 			    0X0BE	//跟踪电池的寿命。


 		
		
#define QRTable00    		0X012	
#define QRTable10    		0X022	 
#define QRTable20    		0X032	
#define QRTable30  			0X042	

/*测量或计算的电池堆中每个电池的电压*/
#define Cell4		        0X0D5	
#define Cell3		        0X0D6	
#define Cell2		        0X0D7	
#define Cell1		        0X0D8	
#define CellX		        0X0D9	

#define AvgCell4		    0X0D1	
#define AvgCell3		    0X0D2	
#define AvgCell2		    0X0D3	
#define AvgCell1		    0X0D4	
/**/

#define Batt		        0X0DA	//2S至4S配置报告


#define nDesignCap      0x1B3

enum{
  MAX_Data =0,
  MAX_Config =1
};

unsigned int verify_memory[3]={0x1B3,0x2580,1};
#define MAX17201_Set_len  96
unsigned int MAX17201_Set[96][3]=
{{0x180,0x0000,1	},//nXTable0 Register
{0x181,0x0000,1	},//nXTable1 Register
{0x182,0x0000,1	},//nXTable2 Register
{0x183,0x0000,1	},//nXTable3 Register
{0x184,0x0000,1	},//nXTable4 Register
{0x185,0x0000,1	},//nXTable5 Register
{0x186,0x0000,1	},//nXTable6 Register
{0x187,0x0000,1	},//nXTable7 Register
{0x188,0x0000,1	},//nXTable8 Register
{0x189,0x0000,1	},//nXTable9 Register
{0x18A,0x0000,1	},//nXTable10 Register
{0x18B,0x0000,1	},//nXTable11 Register
{0x18C,0x0000,1	},//nUser18C Register
{0x18D,0x0000,1	},//nUser18D Register
{0x18E,0x0000,1	},//nODSCTh Register
{0x18F,0x0000,1	},//nODSCCfg Register
{0x190,0x0000,1	},//nOCVTable0 Register
{0x191,0x0000,1	},//nOCVTable1 Register
{0x192,0x0000,1	},//nOCVTable2 Register
{0x193,0x0000,1	},//nOCVTable3 Register
{0x194,0x0000,1	},//nOCVTable4 Register
{0x195,0x0000,1	},//nOCVTable5 Register
{0x196,0x0000,1	},//nOCVTable6 Register
{0x197,0x0000,1	},//nOCVTable7 Register
{0x198,0x0000,1	},//nOCVTable8 Register
{0x199,0x0000,1	},//nOCVTable9 Register
{0x19A,0x0000,1	},//nOCVTable10 Register
{0x19B,0x0000,1	},//nOCVTable11 Register
{0x19C,0x0266,1	},//nIChgTerm Register
{0x19D,0x0000,1	},//nFilterCfg Register
{0x19E,0xAA64,1	},//nVEmpty Register
{0x19F,0x2602,1	},//nLearnCfg Register
{0x1A0,0x3706,1	},//nQRTable00 Register
{0x1A1,0x1A88,1	},//nQRTable10 Register
{0x1A2,0x0C8A,1	},//nQRTable20 Register
{0x1A3,0x0A0B,1	},//nQRTable30 Register
{0x1A4,0x0000,1	},//nCycles Register
{0x1A5,0x2B84,1	},//nFullCapNom Register
{0x1A6,0x1070,1	},//nRComp0 Register
{0x1A7,0x263D,1	},//nTempCo Register
{0x1A8,0xDA80,1	},//nIAvgEmpty Register
{0x1A9,0x2580,1	},//nFullCapRep Register
{0x1AA,0x0000,1	},//nVoltTemp Register
{0x1AB,0x807F,1	},//nMaxMinCurr Register
{0x1AC,0x00FF,1	},//nMaxMinVolt Register
{0x1AD,0x807F,1	},//nMaxMinTemp Register
{0x1AE,0x0000,1	},//nSOC Register
{0x1AF,0x0000,1	},//nTimerH Register
{0x1B0,0x0214,1	},//nConfig Register
{0x1B1,0x0204,1	},//nRippleCfg Register
{0x1B2,0x0000,1	},//nMiscCfg Register
{0x1B3,0x2580,1	},//nDesignCap Register
{0x1B4,0x0000,1	},//nHibCfg Register
{0x1B5,0x0C01,1	},//nPackCfg Register
{0x1B6,0x0000,1	},//nRelaxCfg Register
{0x1B7,0x2241,1	},//nConvgCfg Register
{0x1B8,0x09A0,1	},//nNVCfg0 Register
{0x1B9,0x000E,1	},//nNVCfg1 Register
{0x1BA,0xFF0A,1	},//nNVCfg2 Register
{0x1BB,0x0002,1	},//nSBSCfg Register
{0x1BC,0xE426,0	},//nROMID0 Register
{0x1BD,0x0575,0	},//nROMID1 Register
{0x1BE,0x0010,0	},//nROMID2 Register
{0x1BF,0xE000,0	},//nROMID3 Register
{0x1C0,0xEBA5,1	},//nVAlrtTh Register
{0x1C1,0x7F80,1	},//nTAlrtTh Register
{0x1C2,0x6E00,1	},//nSAlrtTh Register
{0x1C3,0x3280,1	},//nIAlrtTh Register
{0x1C4,0x0000,1	},//nUser1C4 Register
{0x1C5,0x0000,1	},//nUser1C5 Register
{0x1C6,0x0000,1	},//nFullSOCThr Register
{0x1C7,0x0000,1	},//nTTFCfg Register
{0x1C8,0x0000,1	},//nCGain Register
{0x1C9,0x0025,1	},//nTCurve Register
{0x1CA,0x0000,1	},//nTGain Register
{0x1CB,0x0000,1	},//nTOff Register
{0x1CC,0x0000,1	},//nManfctrName0 Register
{0x1CD,0x0000,1	},//nManfctrName1 Register
{0x1CE,0x0000,1	},//nManfctrName2 Register
{0x1CF,0x03E8,1	},//nRSense Register
{0x1D0,0x0000,1	},//nUser1D0 Register
{0x1D1,0x0000,1	},//nUser1D1 Register
{0x1D2,0xD5E3,1	},//nAgeFcCfg Register
{0x1D3,0x0000,1	},//nDesignVoltage Register
{0x1D4,0x0000,1	},//nUser1D4 Register
{0x1D5,0x0000,1	},//nRFastVShdn Register
{0x1D6,0x0000,1	},//nManfctrDate Register
{0x1D7,0x0000,1	},//nFirstUsed Register
{0x1D8,0x0000,1	},//nSerialNumber0 Register
{0x1D9,0x0000,1	},//nSerialNumber1 Register
{0x1DA,0x0000,1	},//nSerialNumber2 Register
{0x1DB,0x0000,1	},//nDeviceName0 Register
{0x1DC,0x0000,1	},//nDeviceName1 Register
{0x1DD,0x0000,1	},//nDeviceName2 Register
{0x1DE,0x0000,1	},//nDeviceName3 Register
{0x1DF,0x0000,1	}//nDeviceName4 Register
};


#endif