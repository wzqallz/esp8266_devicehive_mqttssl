/**
  ******************************************************************************
  * @file    Protocol.c
  * @author  jason
	* @Tel     18938045680
	* @QQ      570526723 
  * @version V2.0
  * @date    12/05/2014
  * @brief   1��רҵǶ��ʽ�����豸��������
						 2��רҵ�������߹�Ӧ�̣�http://wenjieteam.taobao.com/
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, jason SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2014 jason</center></h2>
  */ 
	
#include "espconn.h"
#include "esp_common.h"
#include "protocol.h"
#include "uart.h"
extern UART_HandleTypeDef  				UART_HandleStruct;


extern Pro_M2W_ReturnInfoTypeDef  Pro_M2W_ReturnInfoStruct;
extern Pro_HeadPartTypeDef        Pro_HeadPartStruct;
extern Pro_CommonCmdTypeDef 			Pro_CommonCmdStruct;
//extern Pro_M2W_ReturnInfoTypeDef  Pro_M2W_ReturnInfoStruct;
//extern Pro_P0_ControlTypeDef      Pro_P0_ControlStruct;
extern Pro_Wait_AckTypeDef            Wait_AckStruct;
extern Pro_D2W_ReportDevStatusTypeDef   Pro_D2W_ReportStatusStruct;
//extern Pro_P0_ControlTypeDef      Pro_P0_ControlStruct;
extern Device_WirteTypeDef   			Device_WirteStruct;
extern Device_ReadTypeDef         Device_ReadStruct; 

extern Pro_W2D_WifiStatusTypeDef      Pro_W2D_WifiStatusStruct;
//extern Pro_D2W_ReportDevStatusTypeDef   Pro_D2W_ReportStatusStruct;
extern uint8									     SN; 
extern uint8 Set_LedStatus;




LOCAL esp_udp phoneConnUdp;
LOCAL struct espconn phoneConn;
#define DEFAULT_LAN_PORT 	12476
uint8  deviceid_buf[200];
uint16 deviceid_len;

LOCAL void ICACHE_FLASH_ATTR
udpclient_recv(void *arg, char *pdata, unsigned short len)
{
	//���յ���Ϣ��ͨ�����ں�����ӿ����
    //ͨ��wifi������Ϣ
    //����һ����Ӧ���紫��Ľṹ�壻��������Ҫ���͵����ݣ���������Ҫ�������ݵĳ���
 //   espconn_send(&phoneConn, "Receive message!", os_strlen("Receive message!"));
    os_printf("Receive message:%s\n",pdata);
	
}
LOCAL void ICACHE_FLASH_ATTR
udpclient_conn(void *arg)
{
	//udp���ӳɹ�������device-id���ֻ�

	sint16 ret ;
	ret = espconn_sendto(&phoneConn, deviceid_buf, deviceid_len);
	if (ret != 0) {
		os_printf("UDP send error!");
	}
	
}

LOCAL void ICACHE_FLASH_ATTR
udpclient_reconn(void *arg, sint8 err)
{
	//udp����ʧ��
}

void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata)
{
    switch(status) {
        case SC_STATUS_WAIT:
            printf("SC_STATUS_WAIT\n");
            break;
        case SC_STATUS_FIND_CHANNEL:
            printf("SC_STATUS_FIND_CHANNEL\n");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            printf("SC_STATUS_GETTING_SSID_PSWD\n");
            sc_type *type = pdata;
            if (*type == SC_TYPE_ESPTOUCH) {
                printf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
            } else {
                printf("SC_TYPE:SC_TYPE_AIRKISS\n");
            }
            break;
        case SC_STATUS_LINK:
            printf("SC_STATUS_LINK\n");
            struct station_config *sta_conf = pdata;
	
	        wifi_station_set_config(sta_conf);
	        wifi_station_disconnect();
	        wifi_station_connect();
            break;
        case SC_STATUS_LINK_OVER:
            printf("SC_STATUS_LINK_OVER\n");
            if (pdata != NULL) {
				//SC_TYPE_ESPTOUCH
                uint8 phone_ip[4] = {0};

                memcpy(phone_ip, (uint8*)pdata, 4);
                printf("Phone ip: %d.%d.%d.%d\n",phone_ip[0],phone_ip[1],phone_ip[2],phone_ip[3]);
				//mqtt ����
				int ret;
				ret = mqtt_conn_init();
				
				 if (ret == pdPASS)  {
					 //���ƶ�ע���豸
					
					
				}
				
				
				//udp ����device-id���ֻ���
				phoneConnUdp.local_port = DEFAULT_LAN_PORT;
				phoneConnUdp.remote_port = DEFAULT_LAN_PORT;
			//	phoneConnUdp.remote_ip = phone_ip;
				phoneConn.type = ESPCONN_UDP;
				phoneConn.proto.udp = &(phoneConnUdp);
				
				
				phoneConn.proto.udp->remote_ip[0] = phone_ip[0];
				phoneConn.proto.udp->remote_ip[1] = phone_ip[1];
				phoneConn.proto.udp->remote_ip[2] = phone_ip[2];
				phoneConn.proto.udp->remote_ip[3] = phone_ip[3];

				
				//ע��UDP���ݰ����ջص�
				//����һ����Ӧ���紫��ṹ�壻���������ɹ��������ݺ�Ļص�����
				espconn_regist_recvcb(&phoneConn, udpclient_recv); 
				espconn_regist_connectcb(&phoneConn,udpclient_conn);
				espconn_regist_reconcb(&phoneConn,udpclient_reconn);
				espconn_create(&phoneConn);//����UDP����
				
				
            } else {
            	//SC_TYPE_AIRKISS - support airkiss v2.0
			//	airkiss_start_discover();
			}
            smartconfig_stop();
            break;
    }
	
}


void ICACHE_FLASH_ATTR
smartconfig_task(void *pvParameters)
{
    smartconfig_start(smartconfig_done);
    
    vTaskDelete(NULL);
}

void pro_D2W_ControlWifi_Config_CmdHandle(void)
{
	wifi_set_opmode(STATION_MODE);	 
	xTaskCreate(smartconfig_task, "smartconfig_task", 256, NULL, 2, NULL);


}

/*******************************************************************************
* Function Name  : Pro_GetMcuInfo
* Description    : WiFiģ�������豸��Ϣ
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void Pro_W2D_GetMcuInfo(void)
{
	memcpy(&Pro_CommonCmdStruct, UART_HandleStruct.Message_Buf, sizeof(Pro_CommonCmdStruct));	
	Pro_M2W_ReturnInfoStruct.Pro_HeadPart.SN = Pro_CommonCmdStruct.Pro_HeadPart.SN;
	Pro_M2W_ReturnInfoStruct.Sum = CheckSum((uint8_t *)&Pro_M2W_ReturnInfoStruct, sizeof(Pro_M2W_ReturnInfoStruct));
	Pro_UART_SendBuf((uint8_t *)&Pro_M2W_ReturnInfoStruct,sizeof(Pro_M2W_ReturnInfoStruct), 0);
	
	Log_UART_SendBuf((uint8_t *)&Pro_M2W_ReturnInfoStruct,sizeof(Pro_M2W_ReturnInfoStruct));
	
	
 /******************************�����־*********************************************/	
	printf("W2D_GetMcuInfo...\r\n");
	printf("PRO_VER:"); 		printf(PRO_VER); 			printf("\r\n");
	printf("P0_VER:");			printf(P0_VER);				printf("\r\n");
	printf("P0_VER:");      printf(HARD_VER);			printf("\r\n");
	printf("SOFT_VER:");    printf(SOFT_VER);			printf("\r\n");
	printf("PRODUCT_KEY:"); printf(PRODUCT_KEY);  printf("\r\n");
/***********************************************************************************/	   
	
	
	
}



/*******************************************************************************
* Function Name  : Pro_Pro_W2D_Heartbeat
* Description    : 1��WiFiģ�����豸MCU������(4.2)
									 2���豸MCU֪ͨWiFiģ���������ģʽ(4.3)
									 3���豸MCU����WiFiģ��(4.4)
									 4, WiFiģ����������MCU(4.6)
                   5, WiFiģ����������MCU ( 4.9 WiFiģ�������ϱ���ǰ��״̬)
                   6���豸MCU�ظ� (WiFiģ������豸)
* 4.6 	WiFiģ����������MCU
* 4.9 	Wifiģ��ظ�
* 4.10  �豸MCU�ظ�										
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void Pro_W2D_CommonCmdHandle(void)
{
	memcpy(&Pro_CommonCmdStruct, UART_HandleStruct.Message_Buf, sizeof(Pro_CommonCmdStruct));	
	Pro_CommonCmdStruct.Pro_HeadPart.Cmd = Pro_CommonCmdStruct.Pro_HeadPart.Cmd + 1;
	Pro_CommonCmdStruct.Sum = CheckSum((uint8_t *)&Pro_CommonCmdStruct, sizeof(Pro_CommonCmdStruct));	
	Pro_UART_SendBuf((uint8_t *)&Pro_CommonCmdStruct,sizeof(Pro_CommonCmdStruct), 0);	
	memset(&Pro_CommonCmdStruct, 0, sizeof(Pro_CommonCmdStruct));
}

/*******************************************************************************
* Function Name  : Pro_W2D_WifiStatusHandle
* Description    : ��WiFi��״̬���浽 Pro_W2D_WifiStatusStruct�С����ظ�ACK
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/			
void Pro_W2D_WifiStatusHandle(void)
{
	memcpy(&Pro_W2D_WifiStatusStruct, UART_HandleStruct.Message_Buf, sizeof(Pro_W2D_WifiStatusStruct));	
	Pro_W2D_CommonCmdHandle();
	switch (Pro_W2D_WifiStatusStruct.Wifi_Status)
	{
		case Wifi_SoftAPMode:
			printf("W2M->Wifi_SoftAPMode\r\n");
			break;
		case Wifi_StationMode:
			printf("W2M->Wifi_StationMode\r\n");
			break;
		case Wifi_ConfigMode:
			printf("W2M->Wifi_ConfigMode\r\n");
			break;
		case Wifi_BindingMode:
			printf("W2M->Wifi_BindingMode\r\n");
			break;
		case Wifi_ConnRouter:
			printf("W2M->Wifi_ConnRouter\r\n");
			break;
		case Wifi_ConnClouds:
			printf("W2M->Wifi_ConnClouds\r\n");
			break;
		default:
			break;
	}
} 


/*******************************************************************************
* Function Name  : Pr0_W2D_RequestResetDeviceHandle
* Description    : WiFiģ������λ�豸MCU��MCU�ظ�ACK����ִ���豸��λ
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/	
void Pr0_W2D_RequestResetDeviceHandle(void)
{
	Pro_W2D_CommonCmdHandle();
	printf("W2D_RequestResetDevice...\r\n");
	
/****************************����������豸��λ����****************************/	
//	__set_FAULTMASK(1); // �ر������ж�
//	NVIC_SystemReset(); // ϵͳ��λ
	
/******************************************************************************/
}


/*******************************************************************************
* Function Name  : Pro_W2D_ErrorCmdHandle
* Description    : WiFi�����յ��Ƿ���Ϣ֪ͨ���豸MCU�ظ�ACK����ִ����Ӧ�Ķ���
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/

void Pro_W2D_ErrorCmdHandle(void)
{
	Pro_ErrorCmdTypeDef           	 Pro_ErrorCmdStruct;      // //4.7 �Ƿ���Ϣ֪ͨ
	memcpy(&Pro_ErrorCmdStruct, UART_HandleStruct.Message_Buf, sizeof(Pro_ErrorCmdStruct));
	
	Pro_ErrorCmdStruct.Pro_HeadPart.Cmd = Pro_ErrorCmdStruct.Pro_HeadPart.Cmd;
	Pro_ErrorCmdStruct.Sum = CheckSum((uint8 *)&Pro_CommonCmdStruct, sizeof(Pro_CommonCmdStruct));	
	Pro_UART_SendBuf((uint8 *)&Pro_CommonCmdStruct,sizeof(Pro_CommonCmdStruct), 0);	
	/*************************��������ӹ��ϴ�����*****************************/
	switch (Pro_ErrorCmdStruct.Error_Packets)
	{
		case Error_AckSum:
			printf("W2D Error Command ->Error_AckSum\r\n");
			break;
		case Error_Cmd:
			printf("W2D Error Command ->Error_Cmd\r\n");
			break;
		case Error_Other:
			printf("W2D Error Command ->Error_Other\r\n");
			break;
		default:
			break;
	}	
/******************************************************************************/	
}	



/*******************************************************************************
* Function Name  : Pro_W2D_P0
* Description    : WiFi ʹ��P0Э����豸�����������
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void Pro_W2D_P0CmdHandle(void)
{
	Pro_P0_ControlTypeDef      			 Pro_P0_ControlStruct; 			 //WiFiģ������豸
	
	memcpy(&Pro_P0_ControlStruct, UART_HandleStruct.Message_Buf, sizeof(Pro_P0_ControlStruct));
	switch (Pro_P0_ControlStruct.Pro_HeadPartP0Cmd.Action)
	{
		case P0_W2D_Control_Devce_Action:
			Pro_W2D_Control_DevceHandle();
			break;
		case P0_W2D_ReadDevStatus_Action:
			Pro_W2D_ReadDevStatusHandle();
			break;
		case P0_D2W_ReportDevStatus_Action:
			break;
		
		default:
			break;
	}
	

}


/*******************************************************************************
* Function Name  : Pro_W2D_Control_DevceHandle
* Description    : WiFi ʹ��P0Э����豸�����������
* Input          : uint8_t *Buf:���յ���ָ��
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void Pro_W2D_Control_DevceHandle(void)
{
	Pro_P0_ControlTypeDef      			 Pro_P0_ControlStruct; 			 //4WiFiģ������豸
	Pro_W2D_CommonCmdHandle();
	memcpy(&Pro_P0_ControlStruct, UART_HandleStruct.Message_Buf, sizeof(Pro_P0_ControlStruct));
	switch (Pro_P0_ControlStruct.Attr_Flags)
	{
		case SetLED_OnOff:
			Device_WirteStruct.LED_Cmd = Pro_P0_ControlStruct.Device_Wirte.LED_Cmd ;
			if(Pro_P0_ControlStruct.Device_Wirte.LED_Cmd == LED_OnOff)
			{
				//LED_RGB_Control(0,0,0);
				printf("SetLED_Off \r\n");
			}
			if(Pro_P0_ControlStruct.Device_Wirte.LED_Cmd == LED_OnOn)
			{
			//	LED_RGB_Control(254,0,0);
				printf("SetLED_On \r\n");
			}
			break;			
			case SetLED_Color:	
				Device_WirteStruct.LED_Cmd = Pro_P0_ControlStruct.Device_Wirte.LED_Cmd ;
				if(Pro_P0_ControlStruct.Device_Wirte.LED_Cmd == LED_Costom)
				{
					Set_LedStatus = 0;
					printf("SetLED LED_Costom \r\n");
				}
				if(Pro_P0_ControlStruct.Device_Wirte.LED_Cmd == LED_Yellow)
				{
					Set_LedStatus = 1;
				//	LED_RGB_Control(254, 0, 70);
					printf("SetLED LED_Yellow \r\n");
				}
					
				if(Pro_P0_ControlStruct.Device_Wirte.LED_Cmd == LED_Purple)
				{
					Set_LedStatus = 1;
			//		LED_RGB_Control(254, 70, 0);	
					printf("SetLED LED_Purple \r\n");
				}
				if(Pro_P0_ControlStruct.Device_Wirte.LED_Cmd == LED_Pink)
				{
					Set_LedStatus = 1;
			//		LED_RGB_Control(238, 30, 30);
					printf("SetLED LED_Pink \r\n");
				}
			break;
		case SetLED_R:
			if(Set_LedStatus == 1) break;
			Device_WirteStruct.LED_R = Pro_P0_ControlStruct.Device_Wirte.LED_R;
			printf("W2D Control LED_R = %d \r\n",Device_WirteStruct.LED_R);
		//	LED_RGB_Control(Device_WirteStruct.LED_R,Device_WirteStruct.LED_B,Device_WirteStruct.LED_G);
		  
			break;
		case SetLED_G:
			if(Set_LedStatus == 1) break;
			Device_WirteStruct.LED_G = Pro_P0_ControlStruct.Device_Wirte.LED_G;
			printf("W2D Control LED_G = %d \r\n",Device_WirteStruct.LED_G);
		//	LED_RGB_Control(Device_WirteStruct.LED_R,Device_WirteStruct.LED_B,Device_WirteStruct.LED_G);
			
			break;
		case SetLED_B:
			if(Set_LedStatus == 1) break;
			Device_WirteStruct.LED_B = Pro_P0_ControlStruct.Device_Wirte.LED_B;
			printf("W2D Control LED_B = %d \r\n",Device_WirteStruct.LED_B);
		//	LED_RGB_Control(Device_WirteStruct.LED_R,Device_WirteStruct.LED_B,Device_WirteStruct.LED_G);
			
			break;
		case SetMotor:
			Device_WirteStruct.Motor = Pro_P0_ControlStruct.Device_Wirte.Motor;

			printf("W2D Control Motor = %d \r\n",exchangeBytes(Device_WirteStruct.Motor));
		//	Motor_status(exchangeBytes(Device_WirteStruct.Motor));
			break;
		
	
/*************************************��Ӹ����д�豸**********************************************/


/**************************************************************************************************/		
		
		default:
			break;
	}
	Pro_D2W_ReportDevStatusHandle();
	memset(&Pro_P0_ControlStruct, 0, sizeof(Pro_P0_ControlStruct));
}



void Pro_W2D_ReadDevStatusHandle(void)
{
	Pro_D2W_ReportDevStatusHandle();
}

void Pro_D2W_ReportDevStatusHandle(void)
{
	Pro_D2W_ReportStatusStruct.Pro_HeadPartP0Cmd.Pro_HeadPart.SN = SN++;	
	Pro_D2W_ReportStatusStruct.Pro_HeadPartP0Cmd.Action = P0_D2W_ReportDevStatus_Action;
	
	Pro_D2W_ReportStatusStruct.Device_All.Device_Wirte.LED_Cmd = Device_WirteStruct.LED_Cmd;
	Pro_D2W_ReportStatusStruct.Device_All.Device_Wirte.LED_R = Device_WirteStruct.LED_R;
	Pro_D2W_ReportStatusStruct.Device_All.Device_Wirte.LED_G = Device_WirteStruct.LED_G;
	Pro_D2W_ReportStatusStruct.Device_All.Device_Wirte.LED_B = Device_WirteStruct.LED_B;
	Pro_D2W_ReportStatusStruct.Device_All.Device_Wirte.Motor = Device_WirteStruct.Motor;	
	Pro_D2W_ReportStatusStruct.Device_All.Device_Read.Temperature = Device_ReadStruct.Temperature;
	Pro_D2W_ReportStatusStruct.Device_All.Device_Read.Humidity = Device_ReadStruct.Humidity;
	Pro_D2W_ReportStatusStruct.Device_All.Device_Read.Infrared = Device_ReadStruct.Infrared;	
	Pro_D2W_ReportStatusStruct.Sum = CheckSum((uint8 *)&Pro_D2W_ReportStatusStruct, sizeof(Pro_D2W_ReportStatusStruct));	
	Pro_UART_SendBuf((uint8 *)&Pro_D2W_ReportStatusStruct,sizeof(Pro_D2W_ReportStatusStruct), 0);
	
	printf("Pro_D2W_ReportDevStatus\r\n");	
}



/*******************************************************************************
* Function Name  : UART_SendBuf
* Description    : �򴮿ڷ�������֡
* Input          : buf:������ʼ��ַ�� packLen:���ݳ��ȣ� tag=0,���ȴ�ACK��tag=1,�ȴ�ACK��
* Output         : None
* Return         : None
* Attention		   : ���ȴ�ACK������Э��ʧ���ط�3�Σ�����������FF�����������55
*******************************************************************************/

void Pro_UART_SendBuf(uint8_t *Buf, uint16_t PackLen, uint8_t Tag)
{
	uint16_t i;
	Pro_HeadPartTypeDef   Send_HeadPart;
	Pro_HeadPartTypeDef   Recv_HeadPart;
	
	for(i=0;i<PackLen;i++){
		uart0_write_char(Buf[i]);
		if(i >=2 && Buf[i] == 0xFF) uart0_write_char(0x55);;		
	}		
	
	if(Tag == 0) return ;
	
	memcpy(&Send_HeadPart, Buf, sizeof(Send_HeadPart));
	memset(&Wait_AckStruct, 0, sizeof(Wait_AckStruct));

	while(Wait_AckStruct.SendNum < Send_MaxNum)
	{
		if(Wait_AckStruct.SendTime < Send_MaxTime)
		{
			if(UART_HandleStruct.Package_Flag)
			{			
				memcpy(&Recv_HeadPart, UART_HandleStruct.UART_Buf, sizeof(Recv_HeadPart));
				UART_HandleStruct.Package_Flag = 0;
				
				if((Send_HeadPart.Cmd == (Recv_HeadPart.Cmd - 1)) && (Send_HeadPart.SN == Recv_HeadPart.SN))
					break;	
			}
	
		}
		else
		{
			Wait_AckStruct.SendTime = 0;
			for(i=0;i<PackLen;i++)
			{
				uart0_write_char(Buf[i]);
				if(i >=2 && Buf[i] == 0xFF) 
					uart0_write_char(0x55);
			}	
			Wait_AckStruct.SendNum++ ;			
		}
	}
	
}
/*******************************************************************************
* Function Name  : UART_SendBuf
* Description    : �򴮿ڷ�������֡
* Input          : buf:������ʼ��ַ�� packLen:���ݳ��ȣ� tag=0,���ȴ�ACK��tag=1,�ȴ�ACK��
* Output         : None
* Return         : None
* Attention		   : ���ȴ�ACK������Э��ʧ���ط�3�Σ�����������FF�����������55
*******************************************************************************/

void Log_UART_SendBuf(uint8 *Buf, uint16_t PackLen)
{
	uint16_t i;
	
	for(i=0;i<PackLen;i++){
		
		uart0_write_char(Buf[i]);
		if(i >=2 && Buf[i] == 0xFF) uart0_write_char(0x55);;		
	}		
}



/*******************************************************************************
* Function Name  : exchangeBytes
* Description    : ģ���htons ���� ntohs�����ϵͳ֧�ֽ�����Ŀ�ֱ���滻��ϵͳ����
* Input          : value
* Output         : None
* Return         : ���Ĺ��ֽ����short��ֵ
* Attention		   : None
*******************************************************************************/
short	exchangeBytes(short	value)
{
	short			tmp_value;
	uint8		*index_1, *index_2;
	
	index_1 = (uint8 *)&tmp_value;
	index_2 = (uint8 *)&value;
	
	*index_1 = *(index_2+1);
	*(index_1+1) = *index_2;
	
	return tmp_value;
}

/*******************************************************************************
* Function Name  : CheckSum
* Description    : У����㷨
* Input          : buf:������ʼ��ַ�� packLen:���ݳ��ȣ�
* Output         : None
* Return         : У����
* Attention		   : None
*******************************************************************************/
uint8_t CheckSum( uint8_t *buf, int packLen )
{
	
  int				i;
	uint8		sum;
	
	if(buf == NULL || packLen <= 0) return 0;
	sum = 0;
	for(i=2; i<packLen-1; i++) 
		sum += buf[i];
	
	return sum;

}


/*******************************************************************************
* Function Name  : MessageHandle
* Description    : ���������ݷ����ˣ��ȼ�������Ƿ�Ϸ����ٽ�������֡������Ӧ����
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void uartMessageHandle(void)
{
	Pro_HeadPartTypeDef   Recv_HeadPart;
	memset(&Recv_HeadPart, 0, sizeof(Recv_HeadPart));
	memset(&UART_HandleStruct.Message_Buf, 0, sizeof(16));
	if(UART_HandleStruct.Package_Flag)
	{
		
		UART_HandleStruct.Message_Len = UART_HandleStruct.UART_Cmd_len + 4;		
		memcpy(&UART_HandleStruct.Message_Buf, UART_HandleStruct.UART_Buf, UART_HandleStruct.Message_Len );					
		memcpy(&Recv_HeadPart, UART_HandleStruct.Message_Buf, sizeof(Recv_HeadPart));
		memset(&UART_HandleStruct.UART_Buf, 0, sizeof(UART_HandleStruct.Message_Buf));		
		UART_HandleStruct.Package_Flag = 0;
		UART_HandleStruct.UART_Count = 0;
		
		if(CheckSum(UART_HandleStruct.Message_Buf, UART_HandleStruct.Message_Len) != UART_HandleStruct.Message_Buf[UART_HandleStruct.Message_Len - 1]) 
		{
			Pro_W2D_ErrorCmdHandle();
			return ;		
		}
		switch (Recv_HeadPart.Cmd)
		{
			case Pro_D2W_ControlWifi_Config_Cmd:  //�豸���������wifiģ�飬��������ģʽ
				pro_D2W_ControlWifi_Config_CmdHandle();
				break;
			case Pro_D2W_P0_Cmd:   //�豸�����ϱ�״̬
			//	Pro_D2W_P0_CmdHandle();		 
				break;
			case Pro_D2W_ResetWifi_Cmd:		//����wifi					
			//	Pro_D2W_ResetWifi_CmdHandle();
				printf("Pro_W2D_Heartbeat ...\r\n");	
				break;						
			case Pro_D2W_ReportWifiReset_Cmd: //����wifiģ��
			//	Pro_D2W_ReportWifiReset_CmdHandle();
				break;
			
			default:
				break;
		}	
	}
		
}






















