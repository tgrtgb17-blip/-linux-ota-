#include "stm32f10x.h"
#include "delay.h"
#include <string.h>


#define USART1_RX_BUF_SIZE    1024
static volatile uint8_t  USART1_RX_BUF[USART1_RX_BUF_SIZE];
static volatile uint16_t USART1_RX_HEAD = 0;
static volatile uint16_t USART1_RX_TAIL = 0;

void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_USART1,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength =USART_WordLength_8b;
    USART_InitStructure.USART_StopBits =USART_StopBits_1;
    USART_InitStructure.USART_Parity =USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode =USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
	
    USART_Init(USART2, &USART_InitStructure);
	
    USART1_RX_HEAD = 0;
    USART1_RX_TAIL = 0;

    memset((void *)USART1_RX_BUF, 0, USART1_RX_BUF_SIZE);
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel =USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
    USART_Cmd(USART2, ENABLE);
}
void USART1_IRQHandler(void)
{
    uint8_t data;
    uint16_t next_head;

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        data = (uint8_t)USART_ReceiveData(USART1);

        next_head = USART1_RX_HEAD + 1;

        if(next_head >= USART1_RX_BUF_SIZE)
        {
            next_head = 0;
        }
        if(next_head != USART1_RX_TAIL)
        {
            USART1_RX_BUF[USART1_RX_HEAD] = data;
            USART1_RX_HEAD = next_head;
        }
    }

    if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
    {
        volatile uint16_t temp;

        temp = USART1->SR;
        temp = USART1->DR;
        (void)temp;
    }
}

uint8_t USART1_ReadByte_NonBlock(uint8_t *data)
{
    uint16_t tail;

    if(USART1_RX_HEAD == USART1_RX_TAIL)
    {
        return 0;
    }
    tail = USART1_RX_TAIL;
    *data = USART1_RX_BUF[tail];

    tail++;
    if(tail >= USART1_RX_BUF_SIZE)
    {
        tail = 0;
    }

    USART1_RX_TAIL = tail;

    return 1;
}

uint8_t USART1_ReadByte(uint8_t *data)
{
    while(1)
    {
        if(USART1_ReadByte_NonBlock(data))
        {
            return 1;
        }
    }
}

void USART1_ClearRxBuffer(void)
{
    uint8_t data;

    while(USART1_ReadByte_NonBlock(&data))
    {
        ;
    }

    USART1_RX_HEAD = 0;
    USART1_RX_TAIL = 0;
}

void USART1_ReadHalfWord(uint16_t *data)
{
    uint8_t low;
    uint8_t high;
    USART1_ReadByte(&low);
    USART1_ReadByte(&high);
    *data =((uint16_t)low) |((uint16_t)high << 8);
}

void USART1_receiveWord(uint32_t *data)
{
    uint16_t low;
    uint16_t high;
    USART1_ReadHalfWord(&low);
    USART1_ReadHalfWord(&high);
    *data =((uint32_t)low) |((uint32_t)high << 16);
}

void USART_Send(uint8_t data)
{
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);

    USART_SendData(USART1, data);

    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
}

void USART_sendhalfword(uint16_t data)
{
    uint8_t high_byte;

    USART_Send((uint8_t)(data & 0xFF));

    high_byte = (uint8_t)(data >> 8);

    USART_Send(high_byte);
}

void USART_sendword(uint32_t data)
{
    uint16_t high_halfword;
    USART_sendhalfword((uint16_t)(data & 0xFFFF));
    high_halfword =(uint16_t)(data >> 16);
    USART_sendhalfword(high_halfword);
}

void USART1_ReadBuffer(uint8_t *buf, uint16_t len)
{
    uint16_t i;
    i = 0;
    while(i < len)
    {
        if(USART1_ReadByte_NonBlock(&buf[i]))
        {
            i++;
        }
    }
}

void Usart_SendString(char *str)
{
    while(*str)
    {
        while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
        USART_SendData(USART1,(uint16_t)(*str));
        str++;
    }
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
}

uint8_t USART_receive_string(char *buffer, uint16_t len)
{
    uint16_t index;
    uint16_t timeout_ms;
    uint8_t ch;


    index = 0;
    timeout_ms = 100;
    memset(buffer, 0, len);
    while(timeout_ms > 0)
    {

        if(USART1_ReadByte_NonBlock(&ch))
        {
            if(ch == '\n')
            {
                buffer[index] = '\0';

                return 1;
            }
            if(ch != '\r')
            {
                if(index < len - 1)
                {
                    buffer[index++] = (char)ch;
                }
            }
        }

        Delay_ms(1);
        timeout_ms--;
    }

    buffer[index] = '\0';

    return 0;
}

void Usart2_SendString(char *str)
{
    while(*str)
    {
        while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);

        USART_SendData(USART2,(uint16_t)(*str));

        str++;
    }
    while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
}


uint8_t USART1_ReadByte_Sync(uint8_t *ch,uint32_t timeout_ms)
{
    while(timeout_ms--)
    {
        if(USART1_ReadByte_NonBlock(ch))
        {
            return 1;
        }
    }
    return 0;
}

void USART1_StopOTA(void)
{
    volatile uint32_t temp;
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, DISABLE);
    DMA_Cmd(DMA1_Channel5, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_GL5 |DMA1_FLAG_TC5 |DMA1_FLAG_HT5 |DMA1_FLAG_TE5);
    temp = USART1->SR;
    temp = USART1->DR;

    NVIC_ClearPendingIRQ(USART1_IRQn);
}


