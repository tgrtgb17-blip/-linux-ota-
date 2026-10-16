#include "stm32f10x.h"
#include "serial.h"
#include "OTA.h"
#include "esp8266.h"
#include "LED.h"
#include "OLED.h"
#include "my_flash.h"
#include "delay.h"
#include <string.h>

#define INFO_ADDR 0x08008BF4
#define Cerror_ota 0x0800FC10

#define usart_len 100
volatile uint8_t usart_IT_flag = 0;
volatile uint8_t temp;
uint8_t USART_buffer[100];
void delay(uint32_t t)
{
    while(t--)
    {
        for(int i=0;i<8000;i++);
    }
}
void LED1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
}
int main(void)
{	
	__enable_irq();
	uint8_t flag;
    SystemInit();
	SCB->VTOR = 0x08002800;
    LED1_Init();
	USART1_Init(USART_buffer);
	uint8_t data;

	uint32_t magic = 0x12344321;
	uint32_t version = 0x00000000;
	uint16_t flag_1 = 0xAA55;
	static char buffer[128];
	char *upgrade = "upgrade";
	IWDG_ReloadCounter();
	
	flash_earse_1(Cerror_ota);
	
    while(1)
    {
		IWDG_ReloadCounter();
        GPIO_ResetBits(GPIOC,GPIO_Pin_13);
        delay(1000);
        GPIO_SetBits(GPIOC,GPIO_Pin_13);
        delay(1000);

		 if(usart_IT_flag)
        {	
			usart_IT_flag = 0;
			USART_buffer[temp] = '\0';
			DMA_Cmd(DMA1_Channel5, ENABLE); 
            if(strstr((char *)USART_buffer, "upgrade") != NULL)
            {
                OTA_register();
            }
		}
	}
}

void  USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        volatile uint32_t tmp;
        tmp = USART1->SR;
        tmp = USART1->DR;
		
        DMA_Cmd(DMA1_Channel5, DISABLE);
        temp = usart_len- DMA_GetCurrDataCounter(DMA1_Channel5);
        usart_IT_flag = 1;
		DMA1_Channel5->CMAR = (uint32_t)USART_buffer;
        DMA_SetCurrDataCounter(DMA1_Channel5, usart_len);
//	DMA_Cmd(DMA1_Channel5, ENABLE);
    }
}

