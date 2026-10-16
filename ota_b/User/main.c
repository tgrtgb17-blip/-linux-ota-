#include "stm32f10x.h"
#include "serial.h"
#include "ota.h"
#include "jump.h"
#include "LED.h"
#include "esp8266.h"
#include "esp_at.h"
#include "my_flash.h"


#define INFO_ADDR 0x08008BF4
#define wait_OTA_flag 0x0800F800
#define Cerror_ota 0x0800FC10
#define NUM_ADDR 0x0800F814
#define APP_ADDR 0x08002800
#define APP_bkp 0x08008C00
#define INFO_v2_ADDR 0x0800F7F4

uint32_t wait_ota = 0x12344321;	


int main(void)
{
	SystemInit();
	USART1_Init();
	SCB->VTOR = 0x08000000;
	uint16_t flag,flag_1,NUM_head;
	uint32_t info;
	uint32_t version;
	uint32_t OTA_flag;
	version = *(volatile uint32_t *)INFO_ADDR;
	OTA_flag =*(volatile uint32_t *)(wait_OTA_flag);
	NUM_head =  *(volatile uint32_t *)NUM_ADDR;
	flag=BKP_ReadBackupRegister(BKP_DR1);
	uint32_t flag_v1 = *(volatile uint32_t *)wait_OTA_flag;
	uint32_t old_app_len = *(volatile uint32_t *)(INFO_v2_ADDR+4);
	uint16_t watch_dog = *(volatile uint16_t *)Cerror_ota;
	uint8_t is_watchdog_reset = (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET);
    RCC_ClearFlag();
	esp8266_init();
   
	if(OTA_flag == wait_ota)
	{	
		
		if(NUM_head != 0x1122)
		{	
			OTA_Update('0');
		}
		else 
		{
			OTA_Update('1');
		}
			
	}
	else if(OTA_flag != wait_ota)	
	{	
		if(watch_dog == 0x6666)
		{	
			flash_earse_1(Cerror_ota);
			flash_earse_8(APP_ADDR);
			flash_earse_1(INFO_ADDR);
			flash_earse_1(wait_OTA_flag);
            Flash_ReadProgram(APP_bkp,APP_ADDR,old_app_len);
            Flash_ReadProgram(INFO_v2_ADDR,INFO_ADDR,12);
			NVIC_SystemReset();
		}
		jump_boot_app();
	}
	while(1);
}

