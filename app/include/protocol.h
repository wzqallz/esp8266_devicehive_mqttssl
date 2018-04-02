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

//�豸����ͨ��
 	typedef struct	
{
	uint8							Package_Flag;    				 //�ж��Ƿ���յ�һ�������Ĵ������ݰ�         
	uint8							UART_Flag1;
	uint8							UART_Flag2;
	uint16 						UART_Count;								//���ڻ������������ݳ���	
	uint8						 	UART_Cmd_len;							//ָ���	
	uint8 						UART_Buf[Max_UartBuf]; 		//���ڻ�����
	uint8             Message_Buf[Max_UartBuf]; //������յ�ָ���Buf
	uint8             Message_Len;	            //������Ϣ����
	
}UART_HandleTypeDef;




//����Э��ָ���
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
* ֻ���豸�б��Ժ�Ҫ���ֻ���豸����ӵ�����
********************************************************/
 	typedef struct	
{

	bool                Infrared;
	uint8             Temperature;
	uint8             Humidity;
	
	                 
}Device_ReadTypeDef;  



/******************************************************
*��д�豸�б��Ժ�Ҫ��ӿ�д�豸����ӵ�����
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
*�豸���������ϵ���Ϣ
*******************************************************/
 	typedef struct	
{
	uint8							Alert;
	uint8							Fault;

	
}Device_RestsTypeDef;
/******************************************************
*���������豸�Ľṹ��
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
* ACK �ظ�����
* SendTime 
********************************************************/
 	typedef struct	
{
	uint8			SendTime;
	uint8			SendNum;
	
}Pro_Wait_AckTypeDef;



/******************************************************
* Э��Ĺ��ò���
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
* 4.1  WiFiģ�������豸��Ϣ
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
*       ͨ�����������ack�ȿ��Ը��ô�֡            *
******************************************************
* 4.2 	WiFiģ�����豸MCU������
* 4.4 	�豸MCU����WiFiģ�� 
* 4.6 	WiFiģ����������MCU
* 4.9 	Wifiģ��ظ�
* 4.10  �豸MCU�ظ�
******************************************************/
 	typedef struct	
{
	Pro_HeadPartTypeDef    	Pro_HeadPart;
	uint8							  	Sum;
}Pro_CommonCmdTypeDef;


/******************************************************
* �豸MCUͨ��WiFiģ���������ģʽ
********************************************************/
 	typedef struct	
{
	Pro_HeadPartTypeDef  		Pro_HeadPart;
	uint8                 Config_Method;
	uint8							  	Sum;
}Pro_D2W_ConfigWifiTypeDef; 


/*****************************************************
*WiFi����ģʽ�궨��
******************************************************/
#define 		Wifi_SoftAPMode       			(uint8)(1<<0)    //�Ƿ���SoftAP ģʽ��0:�رգ�1������
#define 		Wifi_StationMode      	    (uint8)(1<<1)    //�Ƿ���Stationģʽ��0:�رգ�1������
#define 		Wifi_ConfigMode       		  (uint8)(1<<2)    //�Ƿ�������ģʽ��0:�رգ�1������
#define 		Wifi_BindingMode     			  (uint8)(1<<3)  	 //�Ƿ�����ģʽ��0:�رգ�1������
#define     Wifi_ConnRouter             (uint8)(1<<4) 	 //WiFiģ���Ƿ�ɹ����ӵ�·������0:�رգ�1������
#define     Wifi_ConnClouds             (uint8)(1<<5)    //WiFiģ���Ƿ�ɹ����ӵ��ƶˣ�0:�رգ�1������
	


/*****************************************************
* 4.5 WiFiģ�����豸MCU֪ͨWiFiģ�鹤��״̬�ı仯
******************************************************/
 	typedef struct	
{
	Pro_HeadPartTypeDef    	Pro_HeadPart;
	uint16                 Wifi_Status;
	uint8							  	Sum;
}Pro_W2D_WifiStatusTypeDef;



/*****************************************************
* �Ƿ���Ϣ֪ͨö���б�
******************************************************/
typedef enum
{
  Error_AckSum = 0x01,      //У��ʹ���      
  Error_Cmd 	 = 0x02,			//�����ʶ��
	Error_Other  = 0x03,      //��������              
}Error_PacketsTypeDef;


/*****************************************************
* 4.7 �Ƿ���Ϣ֪ͨ
******************************************************/
 	typedef struct	
{
	Pro_HeadPartTypeDef  		Pro_HeadPart;
	Error_PacketsTypeDef		Error_Packets;
	uint8									Sum;
}Pro_ErrorCmdTypeDef;


/*****************************************************
* P0 command �б�
******************************************************/
typedef enum
{
	P0_W2D_Control_Devce_Action 		= 0x01,
  P0_W2D_ReadDevStatus_Action 		= 0x02,
	P0_D2W_ReadDevStatus_Action_ACK = 0x03,
	P0_D2W_ReportDevStatus_Action   = 0X04,
	
}P0_ActionTypeDef;


/******************************************************
* ��P0ָ��Ĺ�������
********************************************************/
 	typedef struct	
{
	Pro_HeadPartTypeDef  		Pro_HeadPart;
	P0_ActionTypeDef        Action;
}Pro_HeadPartP0CmdTypeDef;


/*****************************************************
* 4.8 WiFiģ���ȡ�豸�ĵ�ǰ״̬
****************************************************** 
*Wifi ģ�鷢��
******************************************************/
 	typedef struct	
{
	Pro_HeadPartP0CmdTypeDef 			Pro_HeadPartP0Cmd;
	uint8												Sum;
}Pro_W2D_ReadDevStatusTypeDef;



/****************************************************** 
*�豸MCU �ظ� 
*�豸MCU�ϱ���ǰ��״̬
*******************************************************
*4.9 �豸MCU��WiFiZ�����ϱ���ǰ״̬ 
******************************************************/
 	typedef struct	
{
	Pro_HeadPartP0CmdTypeDef 			Pro_HeadPartP0Cmd;
	Device_AllTypeDef       			Device_All;
	uint8												Sum;
}Pro_D2W_ReportDevStatusTypeDef;





/*****************************************************
* 4. 10 WiFiģ������豸
****************************************************** 
* WiFiģ�鷢��
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



