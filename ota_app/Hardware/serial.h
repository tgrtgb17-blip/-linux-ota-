#ifndef _SERIAL_H
#define _SERIAL_H

void USART1_Init(uint8_t *buffer);
uint8_t USART1_ReadByte(uint8_t *data);
void Usart_SendString(char *str);
uint8_t USART_receive_string(char *buffer, uint16_t len);
void Usart2_SendString(char *str);
#endif
