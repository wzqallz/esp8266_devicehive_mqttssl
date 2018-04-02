#ifndef _PROTOCOL_H
#define _PROTOCOL_H





#define		PRO_VER																"00000004"
#define		P0_VER																"00000004"
#define		HARD_VER															"00000001"
#define		SOFT_VER															"00000001"
#define		PRODUCT_KEY														"6f3074fe43894547a4f1314bd7e3ae0b"


 	typedef struct	
{
	uint8    	KEY1;
	uint8     KEY2;
	uint8			KEY3;
}KEY_StatusTypeDef;


#define Max_UartBuf             256

//设备串口通信
 	typedef struct	
{
	uint8							Package_Flag;    				 //判断是否接收到一个完整的串口数据包         
	uint8							UART_Flag1;
	uint8							UART_Flag2;
	uint16 						UART_Count;								//串口缓冲区计算数据长度	
	uint8						 	UART_Cmd_len;							//指令长度	
	uint8 						UART_Buf[Max_UartBuf]; 		//串口缓冲区
	uint8             Message_Buf[Max_UartBuf]; //处理接收到指令的Buf
	uint8             Message_Len;	            //处理信息长度
	
}UART_HandleTypeDef;




//所有协议指令集合
typedef enum
{
  Pro_W2D_GetDeviceInfo_Cmd 					= 0x01,
  Pro_D2W__GetDeviceInfo_Ack_Cmd			= 0x02,
	
	Pro_W2D_P0_Cmd 											= 0x03,
  Pro_D2W_P0_Ack_Cmd									= 0x04,
	
	Pro_D2W_P0_Cmd 											= 0x05,
  Pro_W2D_P0_Ack_Cmd									= 0x06,
	
	Pro_W2D_Heartbeat_Cmd 							= 0x07,
  Pro_D2W_heartbeatAck_Cmd						= 0x08,
	
	Pro_D2W_ControlWifi_Config_Cmd 			= 0x09,
  Pro_W2D_ControlWifi_Config_Ack_Cmd	= 0x0A,
	
	Pro_D2W_ResetWifi_Cmd 							= 0x0B,
  Pro_W2D_ResetWifi_Ack_Cmd						= 0x0C,
	
	Pro_W2D_ReportWifiStatus_Cmd 				= 0x0D,
  Pro_D2W_ReportWifiStatus_Ack_Cmd		= 0x0E,
	
	Pro_D2W_ReportWifiReset_Cmd 				= 0x0F,
  Pro_W2D_ReportWifiReset_Ack_Cmd			= 0x10,
	
	Pro_W2D_ErrorPackage_Cmd 						= 0x11,
  Pro_D2W_ErrorPackage_Ack_Cmd				= 0x12,
	
}Pro_CmdTypeDef;


typedef enum
{
  SetLED_OnOff    = 0x01,
	SetLED_Color    = 0x02,
	SetLED_R       	= 0x04,   
	SetLED_G    		= 0x08,     
	SetLED_B  		 	= 0x10,     
	SetMotor			 	= 0x20,	
  
}Attr_FlagsTypeDef;

typedef enum
{
  LED_OnOff         = 0x00,
	LED_OnOn          = 0x01,
	LED_Costom        = 0x00,
	LED_Yellow    		= 0x02,     
	LED_Purple  		 	= 0x04,     
	LED_Pink			 	  = 0x06,	
  
}LED_ColorTypeDef;
/******************************************************
* 只读设备列表，以后要添加只读设备都添加到这里
********************************************************/
 	typedef struct	
{

	bool                Infrared;
	uint8             Temperature;
	uint8             Humidity;
	
	                 
}Device_ReadTypeDef;  



/******************************************************
*可写设备列表，以后要添加可写设备都添加到这里
********************************************************/
 	typedef struct	
{
	uint8							LED_Cmd;
	uint8							LED_R;
	uint8							LED_G;  
	uint8							LED_B;
	uint16						Motor;
	
	
}Device_WirteTypeDef;


/******************************************************
*设备报警，故障等信息
*******************************************************/
 	typedef struct	
{
	uint8							Alert;
	uint8							Fault;

	
}Device_RestsTypeDef;
/******************************************************
*包含所有设备的结构体
********************************************************/
 	typedef struct	
{
	Device_WirteTypeDef Device_Wirte;
	Device_ReadTypeDef Device_Read;
	Device_RestsTypeDef Device_Rests;
}Device_AllTypeDef;


#define Send_MaxTime   200    
#define Send_MaxNum    3

/******************************************************
* ACK 回复参数
* SendTime 
********************************************************/
 	typedef struct	
{
	uint8			SendTime;
	uint8			SendNum;
	
}Pro_Wait_AckTypeDef;



/******************************************************
* 协议的公用部分
********************************************************/
 	typedef struct	
{
	uint8							Head[2];
	uint16						Len;
	uint8							Cmd;
	uint8							SN;
	uint8							Flags[2];
	
}Pro_HeadPartTypeDef;


/******************************************************
* 4.1  WiFi模组请求设备信息
********************************************************/
 	typedef struct	
{
	Pro_HeadPartTypeDef  		Pro_HeadPart;
	uint8									Pro_ver[8];
	uint8									P0_ver[8];
	uint8									Hard_ver[8];
	uint8									Soft_ver[8];
	uint8									Product_Key[32];
	uint16								Binable_Time;
	uint8									Sum;
	
}Pro_M2W_ReturnInfoTypeDef;

/*****************************************************
*       通用命令，心跳、ack等可以复用此帧            *
******************************************************
* 4.2 	WiFi模组与设备MCU的心跳
* 4.4 	设备MCU重置WiFi模组 
* 4.6 	WiFi模组请求重启MCU
* 4.9 	Wifi模组回复
* 4.10  设备MCU回复
******************************************************/
 	typedef struct	
{
	Pro_HeadPartTypeDef    	Pro_HeadPart;
	uint8							  	Sum;
}Pro_CommonCmdTypeDef;


/******************************************************
* 设备MCU通过WiFi模组进入配置模式
********************************************************/
 	typedef struct	
{
	Pro_HeadPartTypeDef  		Pro_HeadPart;
	uint8                 Config_Method;
	uint8							  	Sum;
}Pro_D2W_ConfigWifiTypeDef; 


/*****************************************************
*WiFi工作模式宏定义
******************************************************/
#define 		Wifi_SoftAPMode       			(uint8)(1<<0)    //是否开启SoftAP 模式，0:关闭，1：开启
#define 		Wifi_StationMode      	    (uint8)(1<<1)    //是否开启Station模式，0:关闭，1：开启
#define 		Wifi_ConfigMode       		  (uint8)(1<<2)    //是否开启配置模式，0:关闭，1：开启
#define 		Wifi_BindingMode     			  (uint8)(1<<3)  	 //是否开启绑定模式，0:关闭，1：开启
#define     Wifi_ConnRouter             (uint8)(1<<4) 	 //WiFi模组是否成功连接到路由器，0:关闭，1：开启
#define     Wifi_ConnClouds             (uint8)(1<<5)    //WiFi模组是否成功连接到云端，0:关闭，1：开启
	


/*****************************************************
* 4.5 WiFi模组向设备MCU通知WiFi模组工作状态的变化
******************************************************/
 	typedef struct	
{
	Pro_HeadPartTypeDef    	Pro_HeadPart;
	uint16                 Wifi_Status;
	uint8							  	Sum;
}Pro_W2D_WifiStatusTypeDef;



/*****************************************************
* 非法信息通知枚举列表
******************************************************/
typedef enum
{
  Error_AckSum = 0x01,      //校验和错误      
  Error_Cmd 	 = 0x02,			//命令不可识别
	Error_Other  = 0x03,      //其他错误              
}Error_PacketsTypeDef;


/*****************************************************
* 4.7 非法消息通知
******************************************************/
 	typedef struct	
{
	Pro_HeadPartTypeDef  		Pro_HeadPart;
	Error_PacketsTypeDef		Error_Packets;
	uint8									Sum;
}Pro_ErrorCmdTypeDef;


/*****************************************************
* P0 command 列表
******************************************************/
typedef enum
{
	P0_W2D_Control_Devce_Action 		= 0x01,
  P0_W2D_ReadDevStatus_Action 		= 0x02,
	P0_D2W_ReadDevStatus_Action_ACK = 0x03,
	P0_D2W_ReportDevStatus_Action   = 0X04,
	
}P0_ActionTypeDef;


/******************************************************
* 带P0指令的公共部分
********************************************************/
 	typedef struct	
{
	Pro_HeadPartTypeDef  		Pro_HeadPart;
	P0_ActionTypeDef        Action;
}Pro_HeadPartP0CmdTypeDef;


/*****************************************************
* 4.8 WiFi模组读取设备的当前状态
****************************************************** 
*Wifi 模组发送
******************************************************/
 	typedef struct	
{
	Pro_HeadPartP0CmdTypeDef 			Pro_HeadPartP0Cmd;
	uint8												Sum;
}Pro_W2D_ReadDevStatusTypeDef;



/****************************************************** 
*设备MCU 回复 
*设备MCU上报当前的状态
*******************************************************
*4.9 设备MCU向WiFiZ主动上报当前状态 
******************************************************/
 	typedef struct	
{
	Pro_HeadPartP0CmdTypeDef 			Pro_HeadPartP0Cmd;
	Device_AllTypeDef       			Device_All;
	uint8												Sum;
}Pro_D2W_ReportDevStatusTypeDef;





/*****************************************************
* 4. 10 WiFi模组控制设备
****************************************************** 
* WiFi模组发送
******************************************************/
 	typedef struct	
{
	
	Pro_HeadPartP0CmdTypeDef 			Pro_HeadPartP0Cmd;
	Attr_FlagsTypeDef             Attr_Flags;
	Device_WirteTypeDef     			Device_Wirte;
	uint8							  				Sum;	
}Pro_P0_ControlTypeDef; 







void uartMessageHandle(void);
void Pro_W2D_GetMcuInfo(void);
void Pro_W2D_CommonCmdHandle(void);
void Pro_W2D_WifiStatusHandle(void);
void Pr0_W2D_RequestResetDeviceHandle(void);
void Pro_W2D_ErrorCmdHandle(void);
void Pro_W2D_P0CmdHandle(void);
void Pro_W2D_Control_DevceHandle(void);
void Pro_W2D_ReadDevStatusHandle(void);

void Pro_D2W_ReportDevStatusHandle(void);

void Pro_UART_SendBuf(uint8 *Buf, uint16 PackLen, uint8 Tag);
void Log_UART_SendBuf(uint8 *Buf, uint16 PackLen);
short	exchangeBytes(short	value);
uint8 CheckSum( uint8 *buf, int packLen );



#endif /*_PROTOCOL_H*/



