/**
  ******************************************************************************
  * @file    Protocol.c
  * @author  jason
	* @Tel     18938045680
	* @QQ      570526723 
  * @version V2.0
  * @date    12/05/2014
  * @brief   1，专业嵌入式智能设备方案定制
						 2，专业开发工具供应商：http://wenjieteam.taobao.com/
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
	//接收到消息后，通过串口和网络接口输出
    //通过wifi发送消息
    //参数一：对应网络传输的结构体；参数二：要发送的内容；参数三：要发送内容的长度
 //   espconn_send(&phoneConn, "Receive message!", os_strlen("Receive message!"));
    os_printf("Receive message:%s\n",pdata);
	
}
LOCAL void ICACHE_FLASH_ATTR
udpclient_conn(void *arg)
{
	//udp连接成功，发送device-id到手机

	sint16 ret ;
	ret = espconn_sendto(&phoneConn, deviceid_buf, deviceid_len);
	if (ret != 0) {
		os_printf("UDP send error!");
	}
	
}

LOCAL void ICACHE_FLASH_ATTR
udpclient_reconn(void *arg, sint8 err)
{
	//udp连接失败
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
				//mqtt 连接
				int ret;
				ret = mqtt_conn_init();
				
				 if (ret == pdPASS)  {
					 //向云端注册设备
					
					
				}
				
				
				//udp 发送device-id到手机端
				phoneConnUdp.local_port = DEFAULT_LAN_PORT;
				phoneConnUdp.remote_port = DEFAULT_LAN_PORT;
			//	phoneConnUdp.remote_ip = phone_ip;
				phoneConn.type = ESPCONN_UDP;
				phoneConn.proto.udp = &(phoneConnUdp);
				
				
				phoneConn.proto.udp->remote_ip[0] = phone_ip[0];
				phoneConn.proto.udp->remote_ip[1] = phone_ip[1];
				phoneConn.proto.udp->remote_ip[2] = phone_ip[2];
				phoneConn.proto.udp->remote_ip[3] = phone_ip[3];

				
				//注册UDP数据包接收回调
				//参数一：对应网络传输结构体；参数二：成功接收数据后的回调函数
				espconn_regist_recvcb(&phoneConn, udpclient_recv); 
				espconn_regist_connectcb(&phoneConn,udpclient_conn);
				espconn_regist_reconcb(&phoneConn,udpclient_reconn);
				espconn_create(&phoneConn);//建立UDP传输
				
				
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
	Pro_W2D_CommonCmdHandle();
	printf("UART_HandleStruct.Message_Buf[] length  = %d \r\n",UART_HandleStruct.Message_Len);
	uint8 peiwan_mode = UART_HandleStruct.Message_Buf[8];
	printf("peiwan_mode = %d \r\n",peiwan_mode);
	if(peiwan_mode ==1){  //softap
		wifi_set_opmode(SOFTAP_MODE);
	}

	if(peiwan_mode ==2){  //smartconfig

		wifi_set_opmode(STATION_MODE);
		xTaskCreate(smartconfig_task, "smartconfig_task", 256, NULL, 2, NULL);
	}

}





/*******************************************************************************
* Function Name  : Pro_W2D_CommonCmdHandle
* Description    : 1，WiFi模组与设备MCU的心跳(4.2)
									 2，设备MCU通知WiFi模组进入配置模式(4.3)
									 3，设备MCU重置WiFi模组(4.4)
									 4, WiFi模组请求重启MCU(4.6)
                   5, WiFi模组请求重启MCU ( 4.9 WiFi模组主动上报当前的状态)
                   6，设备MCU回复 (WiFi模组控制设备)
* 4.6 	WiFi模组请求重启MCU
* 4.9 	Wifi模组回复
* 4.10  设备MCU回复										
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void Pro_W2D_CommonCmdHandle(void)
{
	memcpy(&Pro_CommonCmdStruct, UART_HandleStruct.Message_Buf, sizeof(Pro_CommonCmdStruct));	
	Pro_CommonCmdStruct.Pro_HeadPart.Cmd = Pro_CommonCmdStruct.Pro_HeadPart.Cmd + 1;
	Pro_CommonCmdStruct.Pro_HeadPart.Len = 0X05;
	Pro_CommonCmdStruct.Sum = CheckSum((uint8_t *)&Pro_CommonCmdStruct, sizeof(Pro_CommonCmdStruct));	
	Pro_UART_SendBuf((uint8_t *)&Pro_CommonCmdStruct,sizeof(Pro_CommonCmdStruct), 0);	
	memset(&Pro_CommonCmdStruct, 0, sizeof(Pro_CommonCmdStruct));
}







/*******************************************************************************
* Function Name  : Pro_D2W_ErrorCmdHandle
* Description    : MCU发送非法信息通知，WIFI回复ACK，并执行相应的动作
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/

void Pro_D2W_ErrorCmdHandle(uint8 errorType)
{
	Pro_ErrorCmdTypeDef           	 Pro_ErrorCmdStruct;      // //4.7 非法消息通知
	memcpy(&Pro_ErrorCmdStruct, UART_HandleStruct.Message_Buf, sizeof(Pro_ErrorCmdStruct));
	
	Pro_ErrorCmdStruct.Pro_HeadPart.Cmd = Pro_W2D_ErrorPackage_Cmd;
	Pro_ErrorCmdStruct.Error_Packets = errorType;
	Pro_ErrorCmdStruct.Pro_HeadPart.Len = 0X06;
	Pro_ErrorCmdStruct.Pro_HeadPart.Flags[0] = 0X00;
	Pro_ErrorCmdStruct.Pro_HeadPart.Flags[1] = 0X00;
	Pro_ErrorCmdStruct.Sum = CheckSum((uint8 *)&Pro_ErrorCmdStruct, sizeof(Pro_ErrorCmdStruct));
	Pro_UART_SendBuf((uint8 *)&Pro_CommonCmdStruct,sizeof(Pro_CommonCmdStruct), 0);	
	/*************************在这里添加故障处理函数*****************************/
	switch (Pro_ErrorCmdStruct.Error_Packets)
	{
		case Error_AckSum:
			printf("D2W Error Command ->Error_AckSum\r\n");
			break;
		case Error_Cmd:
			printf("D2W Error Command ->Error_Cmd\r\n");
			break;
		case Error_Other:
			printf("D2W Error Command ->Error_Other\r\n");
			break;
		default:
			break;
	}	
/******************************************************************************/	
}	



/*******************************************************************************
* Function Name  : UART_SendBuf
* Description    : 向串口发送数据帧
* Input          : buf:数据起始地址； packLen:数据长度； tag=0,不等待ACK；tag=1,等待ACK；
* Output         : None
* Return         : None
* Attention		   : 若等待ACK，按照协议失败重发3次；数据区出现FF，在其后增加55
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
* Description    : 向串口发送数据帧
* Input          : buf:数据起始地址； packLen:数据长度； tag=0,不等待ACK；tag=1,等待ACK；
* Output         : None
* Return         : None
* Attention		   : 若等待ACK，按照协议失败重发3次；数据区出现FF，在其后增加55
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
* Description    : 模拟的htons 或者 ntohs，如果系统支字节序更改可直接替换成系统函数
* Input          : value
* Output         : None
* Return         : 更改过字节序的short数值
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
* Description    : 校验和算法
* Input          : buf:数据起始地址； packLen:数据长度；
* Output         : None
* Return         : 校验码
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
* Description    : 串口有数据发生了，先检查数据是否合法，再解析数据帧，做相应处理
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
			Pro_D2W_ErrorCmdHandle(Error_AckSum);
			return ;		
		}
		printf("recive cmd  = %d \r\n",Recv_HeadPart.Cmd);
		uint8 i =0;
		while(i<UART_HandleStruct.Message_Len){
			printf("UART_HandleStruct message  = %d \r\n",UART_HandleStruct.Message_Buf[i]);
			i++;
		}

		switch (Recv_HeadPart.Cmd)
		{
			case Pro_D2W__GetDeviceInfo_Ack_Cmd:  //当wifi获取mcu状态时，mcu的回复

				break;
			case Pro_D2W_P0_Ack_Cmd:   //wifi控制mcu时，mcu的回复

				break;
			case Pro_D2W_P0_Cmd:		//mcu主动上报状态
				Pro_W2D_CommonCmdHandle();

				break;						
			case Pro_D2W_heartbeatAck_Cmd: //当wifi发送心跳包，mcu的回复

				break;
			case  Pro_D2W_ControlWifi_Config_Cmd:  //设备发送命令给wifi模块，进入配网模式
				pro_D2W_ControlWifi_Config_CmdHandle();
				break;

			case  Pro_D2W_ResetWifi_Cmd:  //mcu发送消息重置wifi模块，wifi模块进入smartconfig模式

				break;
			case  Pro_D2W_ReportWifiStatus_Ack_Cmd:  //推送wifi模块状态后，mcu的回复

				break;
			case Pro_D2W_ErrorPackage_Ack_Cmd: //wifi模块发送了非法数据到mcu，mcu的回复
				break;
			default:
				Pro_D2W_ErrorCmdHandle(Error_Cmd);  //接收到非法数据
				break;
		}	
	}
		
}






















