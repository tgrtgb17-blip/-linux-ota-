#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"
#include <stdint.h>

void USART1_Init(void);
uint8_t USART1_ReadByte(uint8_t *data);
uint8_t USART1_ReadByte_NonBlock(uint8_t *data);
uint8_t USART1_ReadByte_Sync(uint8_t *ch,uint32_t timeout_ms);
void USART1_ReadHalfWord(uint16_t *data);
void USART1_receiveWord(uint32_t *data);
void USART1_ReadBuffer(uint8_t *buf,uint16_t len);
void USART_Send(uint8_t data);
void USART_sendhalfword(uint16_t data);
void USART_sendword(uint32_t data);
void Usart_SendString(char *str);
uint8_t USART_receive_string(char *buffer,uint16_t len);
void Usart2_SendString(char *str);
void USART1_StopOTA(void);

#endif
	
	