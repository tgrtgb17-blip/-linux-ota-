#include "stm32f10x.h"
#include <string.h>


void USART1_Init(uint8_t *buffer)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_USART1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 |GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 |GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits =USART_StopBits_1;
    USART_InitStructure.USART_Parity =USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode =USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART1,&USART_InitStructure);
	USART_Init(USART2,&USART_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef USART_NVIC_USART;
	USART_NVIC_USART.NVIC_IRQChannel = USART1_IRQn;
	USART_NVIC_USART.NVIC_IRQChannelCmd = ENABLE;
	USART_NVIC_USART.NVIC_IRQChannelPreemptionPriority = 1;
	USART_NVIC_USART.NVIC_IRQChannelSubPriority = 1;

	NVIC_Init(&USART_NVIC_USART);	
	
	DMA_InitTypeDef USART_DMA;
	USART_DMA.DMA_BufferSize = 100;
	USART_DMA.DMA_DIR = DMA_DIR_PeripheralSRC;
	USART_DMA.DMA_M2M = DMA_M2M_Disable;
	USART_DMA.DMA_MemoryBaseAddr = (uint32_t)buffer;
	USART_DMA.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	USART_DMA.DMA_MemoryInc = DMA_MemoryInc_Enable;
	USART_DMA.DMA_Mode = DMA_Mode_Normal;
	USART_DMA.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	USART_DMA.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	USART_DMA.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	USART_DMA.DMA_Priority = DMA_Priority_Medium;
	
	DMA_Init(DMA1_Channel5,&USART_DMA);
	DMA_Cmd(DMA1_Channel5,ENABLE);
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	
    USART_Cmd(USART1,ENABLE);
	USART_Cmd(USART2,ENABLE);
}

//uint8_t USART1_ReadByte(uint8_t *data)
//{
//    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)==SET)
//    {
//        *data =USART_ReceiveData(USART1);
//        return 1;
//    }
//    return 0;
//}
void Usart_SendString(char *str)
{
    while(*str)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, *str);
        str++;
    }
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}
//uint8_t USART_receive_string(char *buffer, uint16_t len)
//{
//    static uint16_t index = 0;
//    uint8_t ch;
//    if (buffer == NULL || len < 2)
//    {
//        return 0;
//    }
//    while (USART1_ReadByte(&ch))
//    {
//        if (ch == '\n')
//        {
//            buffer[index] = '\0';

//            index = 0;

//            return 1;
//        }
//        if (ch == '\r')
//        {
//            continue;
//        }
//        if (index < len - 1)
//        {
//            buffer[index++] = ch;
//        }
//        else
//        {
//            index = 0;
//        }
//    }
//    return 0;
//}
void Usart2_SendString(char *str)
{
    while(*str)
    {
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
        USART_SendData(USART2, *str);
        str++;
    }
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}
