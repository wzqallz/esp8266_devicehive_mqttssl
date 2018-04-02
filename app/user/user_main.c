/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"
#include "user_config.h"
#include "protocol.h"
#include "uart.h"
#include "espconn.h"

UART_HandleTypeDef         				UART_HandleStruct;
Pro_Wait_AckTypeDef           	  Wait_AckStruct;
Device_WirteTypeDef   					  Device_WirteStruct;    		
Device_ReadTypeDef                Device_ReadStruct; 
Pro_M2W_ReturnInfoTypeDef  				Pro_M2W_ReturnInfoStruct;
Pro_CommonCmdTypeDef      			 	Pro_CommonCmdStruct;
Pro_W2D_WifiStatusTypeDef     	 	Pro_W2D_WifiStatusStruct;     
Pro_CommonCmdTypeDef     	 				Pro_M2WResetCmdStruct;    		
Pro_D2W_ConfigWifiTypeDef       	Pro_D2WConfigCmdStruct;   		
Pro_D2W_ReportDevStatusTypeDef   	Pro_D2W_ReportStatusStruct;
KEY_StatusTypeDef 								KEY_Status;
uint8		SN;    
uint32  SystemTimeCount = 0;
uint32  ReportTimeCount = 0;
uint8   Set_LedStatus = 0;
void wifiStatusInit(void){	
	printf("UART_HandleStruct Init...\r\n");
	memset(&UART_HandleStruct, 0, sizeof(UART_HandleStruct));	
	memset(&Pro_CommonCmdStruct, 0, sizeof(Pro_CommonCmdStruct));	
	Pro_CommonCmdStruct.Pro_HeadPart.Head[0] = 0XFF;	
	Pro_CommonCmdStruct.Pro_HeadPart.Head[1] = 0XFF;			
	printf("Pro_CommonCmdStruct Init...\r\n");	
	memset(&Pro_CommonCmdStruct, 0, sizeof(Pro_CommonCmdStruct));	
	Pro_CommonCmdStruct.Pro_HeadPart.Head[0] = 0XFF;	
	Pro_CommonCmdStruct.Pro_HeadPart.Head[1] = 0XFF;				
	memset(&Pro_M2W_ReturnInfoStruct, 0, sizeof(Pro_M2W_ReturnInfoStruct));	
	Pro_M2W_ReturnInfoStruct.Pro_HeadPart.Head[0] = 0XFF; 	
	Pro_M2W_ReturnInfoStruct.Pro_HeadPart.Head[1] = 0XFF;	
	Pro_M2W_ReturnInfoStruct.Pro_HeadPart.Len = exchangeBytes(sizeof(Pro_M2W_ReturnInfoStruct) - 4);	
	Pro_M2W_ReturnInfoStruct.Pro_HeadPart.Cmd = Pro_D2W__GetDeviceInfo_Ack_Cmd;	
	memcpy(Pro_M2W_ReturnInfoStruct.Pro_ver, PRO_VER, strlen(PRO_VER));	
	memcpy(Pro_M2W_ReturnInfoStruct.P0_ver, P0_VER, strlen(P0_VER));	
	memcpy(Pro_M2W_ReturnInfoStruct.Hard_ver, HARD_VER, strlen(HARD_VER));	
	memcpy(Pro_M2W_ReturnInfoStruct.Soft_ver, SOFT_VER, strlen(SOFT_VER));	
	memcpy(Pro_M2W_ReturnInfoStruct.Product_Key, PRODUCT_KEY, strlen(PRODUCT_KEY));	
	Pro_M2W_ReturnInfoStruct.Binable_Time = exchangeBytes(0);;				
	printf("Pro_D2W_ReportStatusStruct Init...\r\n");	
	memset(&Pro_D2W_ReportStatusStruct, 0, sizeof(Pro_D2W_ReportStatusStruct));	
	Pro_D2W_ReportStatusStruct.Pro_HeadPartP0Cmd.Pro_HeadPart.Head[0] = 0XFF;	
	Pro_D2W_ReportStatusStruct.Pro_HeadPartP0Cmd.Pro_HeadPart.Head[1] = 0XFF;            	
	Pro_D2W_ReportStatusStruct.Pro_HeadPartP0Cmd.Pro_HeadPart.Len = exchangeBytes(sizeof(Pro_D2W_ReportStatusStruct) - 4);	
	Pro_D2W_ReportStatusStruct.Pro_HeadPartP0Cmd.Pro_HeadPart.Cmd = Pro_D2W_P0_Cmd;		
	printf("Pro_M2WResetCmdStruct Init...\r\n");	
	memset(&Pro_M2WResetCmdStruct, 0, sizeof(Pro_M2WResetCmdStruct));
	Pro_M2WResetCmdStruct.Pro_HeadPart.Head[0] = 0XFF;	
	Pro_M2WResetCmdStruct.Pro_HeadPart.Head[1] = 0XFF;	
	Pro_M2WResetCmdStruct.Pro_HeadPart.Len = exchangeBytes(sizeof(Pro_M2WResetCmdStruct) - 4);	
	Pro_M2WResetCmdStruct.Pro_HeadPart.Cmd = Pro_D2W_ResetWifi_Cmd;		
	printf("Pro_D2WConfigCmdStruct Init...\r\n");	
	memset(&Pro_D2WConfigCmdStruct, 0, sizeof(Pro_D2WConfigCmdStruct));	
	Pro_D2WConfigCmdStruct.Pro_HeadPart.Head[0] = 0XFF;	
	Pro_D2WConfigCmdStruct.Pro_HeadPart.Head[1] = 0XFF;	
	Pro_D2WConfigCmdStruct.Pro_HeadPart.Len = exchangeBytes(sizeof(Pro_D2WConfigCmdStruct) - 4);	
	Pro_D2WConfigCmdStruct.Pro_HeadPart.Cmd = Pro_D2W_ControlWifi_Config_Cmd;		
	printf("Device_WirteStruct Init...\r\n");	
	memset(&Device_WirteStruct, 0, sizeof(Device_WirteStruct));	
	Device_WirteStruct.Motor = exchangeBytes(0x05);   //关闭电机		
	printf("Device_ReadStruct Init...\r\n");	
	memset(&Device_ReadStruct, 0, sizeof(Device_ReadStruct));		
	printf("KEY_Status Init...\r\n");	
	memset(&KEY_Status, 0, sizeof(KEY_Status));	
}


/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;
        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void wifi_event_handler_cb(System_Event_t *event)
{
    if (event == NULL) {
        return;
    }

    switch (event->event_id) {
        case EVENT_STAMODE_GOT_IP:
            os_printf("sta got ip ,create task and free heap size is %d\n", system_get_free_heap_size());
            user_conn_init();
            break;

        case EVENT_STAMODE_CONNECTED:
            os_printf("sta connected\n");
            break;

        case EVENT_STAMODE_DISCONNECTED:
            wifi_station_connect();
            break;

        default:
            break;
    }
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/

LOCAL os_timer_t uart_message_handle;
void user_init(void)
{
   uint32 system_free_size = 0;   
   wifiStatusInit();
   wifi_station_set_auto_connect(1);    
   wifi_set_sleep_type(NONE_SLEEP_T);//set none sleep mode    
  // espconn_tcp_set_max_con(10);
   uart_init_new();
   //创建定时任务调用 protocol.c里的uartMessageHandle方法，读取串口发送的数据
    os_timer_disarm(&uart_message_handle); //启动定时器前先取消定时器定时
    os_timer_setfn(&uart_message_handle, (os_timer_func_t *) uartMessageHandle,NULL);  //设置定时器回调函数
    os_timer_arm(&uart_message_handle, 100, 1);   //启动定时器 100ms
//加入内容测试
	
}
