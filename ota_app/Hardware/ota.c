#include "stm32f10x.h"                  // Device header
#include "serial.h"
#include "my_flash.h"
#include "ota.h"

#define APP_ADDR 0x08002800
#define wait_OTA_flag 0x0800F800
uint32_t wait_ota = 0x12344321;	

void OTA_register(void)
{
	FLASH_Unlock();
	FLASH_ErasePage(wait_OTA_flag);
	FLASH_ProgramWord(wait_OTA_flag,wait_ota);
	FLASH_Lock();
	
	NVIC_SystemReset();
}


void OTA_flag_1(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP,ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);
	
	BKP_WriteBackupRegister(BKP_DR2,0xAA55);

}
