#include "stm32f10x.h"                  // Device header
#include "serial.h"


#define app_address 0x08002800
typedef void(*app_func)(void);

void jump_boot_app(void)
{
	uint32_t msp_stack = *(uint32_t*)app_address;
	uint32_t app_stack = *(uint32_t*)(app_address+4);
	
	
	
	if((msp_stack & 0x2FFE0000)!=0x20000000)
    {	

        return;
    }
	
	app_func app;
	
	USART1_StopOTA();
	
	__disable_irq();
	RCC->APB1ENR = 0x00000000;
    RCC->APB2ENR = 0x00000000;
	
	SysTick->CTRL = 0;
	SysTick->LOAD =0;
	SysTick->VAL = 0;
	for (int i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
	SCB->VTOR = app_address;
	__set_MSP(msp_stack);
	
	app = (app_func)app_stack;
	
	app();
}	

