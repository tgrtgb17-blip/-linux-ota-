#include "stm32f10x.h"                  // Device header


#define app_address  0x08002800

void flash_earse(void)
{
	FLASH_Unlock();
	
	for(uint8_t i = 0 ;i < 54; i++ )
	{
		FLASH_ErasePage(app_address+i*0x400);
	}
	FLASH_Lock();
}

uint8_t Flash_Write(uint32_t addr, uint8_t *data, uint16_t len)
{
    FLASH_Unlock();
    for(uint16_t i=0;i<len;i+=2)
    {
        uint16_t temp;
        temp=data[i];
        if(i+1<len)
        {
            temp |= data[i+1]<<8;
        }
        else
        {
            temp |= 0xff00;
        }
        if(FLASH_ProgramHalfWord(addr,temp)!=FLASH_COMPLETE)
		{
		return 1;
		}
        addr+=2;
	
    }
    FLASH_Lock();
	return 2;
	
}
void  Flash_WritehalfWord_ota(uint32_t addr, uint16_t data)
{
    FLASH_Unlock();
	uint8_t i;
    while(FLASH_ProgramHalfWord(addr,data)!=FLASH_COMPLETE)
	{
		i++;
		if(i>10)
		{
			break;
		}
	}	
	
    FLASH_Lock();
}
void flash_earse_1(uint32_t adrr)
{	
	__disable_irq();
	FLASH_Unlock();
	FLASH_ErasePage(adrr);
	FLASH_Lock();
	__enable_irq();
}

