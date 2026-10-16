#include "stm32f10x.h"
#include "serial.h"
#include "delay.h"
#include "esp8266.h"
#include <string.h>

uint8_t esp8266_init(void)
{
	Usart_SendString("ATE0\r\n");
    Delay_ms(500);
    Usart_SendString("AT+SLEEP=0\r\n");
    Delay_ms(500);
    Usart_SendString("AT+CWMODE=1\r\n");
    Delay_ms(1000);
	IWDG_ReloadCounter();
    Usart_SendString("AT+CWJAP=\"oppoA5\",\"11111111\"\r\n");
    Delay_ms(5000);
	IWDG_ReloadCounter();
    Usart_SendString("AT+MQTTUSERCFG=0,1,\"stm32_001\",\"\",\"\",0,0,\"\"\r\n");
    Delay_ms(1000);

    Usart_SendString("AT+MQTTCONNCFG=0,60,0,\"ota/001/status\",\"offline\",0,0\r\n");
    Delay_ms(500);

    Usart_SendString("AT+MQTTCONN=0,\"192.168.6.212\",1883,1\r\n");
	IWDG_ReloadCounter();
    Delay_ms(5000);
		
    Usart_SendString("AT+MQTTSUB=0,\"ota/001/command\",1\r\n");
	IWDG_ReloadCounter();
    Delay_ms(500);
	
    Usart_SendString("AT+MQTTPUB=0,\"ota/001/status\",\"online\",1,0\r\n");

    return 1;
}
void esp_publish(char *topic,char *payload)
{
    Usart_SendString("AT+MQTTPUB=0,\"");
    Usart_SendString(topic);
    Usart_SendString("\",\"");
    Usart_SendString(payload);
    Usart_SendString("\",1,0\r\n");
}
	
void esp_subcribe(char *topic)
{
    Usart_SendString("AT+MQTTSUB=0,\"");
    Usart_SendString(topic);
    Usart_SendString("\",1\r\n");
}
