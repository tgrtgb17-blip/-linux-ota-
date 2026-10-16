#include "stm32f10x.h"                  // Device header
#include "ota.h"
#include <string.h>

#define APP_ADDR 0x08002800
#define APP_v2_addr 0x08004800
#define APP_v1_bkp 0x08006800
#define INFO_ADDR 0x0800F800
#define NUM_ADDR 0x0800F814

void flash_earse_8(uint32_t adrr)
{	
	__disable_irq();
	FLASH_Unlock();
	
	for(uint8_t i = 0 ;i < 8; i++ )
	{
		FLASH_ErasePage(adrr+i*0x400);
	}
	FLASH_Lock();
	__enable_irq();
}

void flash_earse_1(uint32_t adrr)
{	
	__disable_irq();
	FLASH_Unlock();
	FLASH_ErasePage(adrr);
	FLASH_Lock();
	__enable_irq();
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
	uint16_t i;
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

void  Flash_WriteWord_ota(uint32_t addr, uint32_t data)
{
    FLASH_Unlock();
	uint8_t i;
    while(FLASH_ProgramWord(addr,data)!=FLASH_COMPLETE)
	{
		i++;
		if(i>10)
		{
			break;
		}
	}	
	
    FLASH_Lock();
}

void Flash_WriteStruct(uint32_t addr, void *data, uint16_t size)
{
    uint8_t *p = (uint8_t *)data;

    FLASH_Unlock();

    for(uint16_t i = 0; i < size; i += 2)
    {
        uint16_t temp = p[i];

        if(i + 1 < size)
        {
            temp |= ((uint16_t)p[i + 1] << 8);
        }
        else
        {
            temp |= 0xFF00;
        }

        FLASH_ProgramHalfWord(addr, temp);

        addr += 2;
    }

    FLASH_Lock();
}

//void Flash_read_struct(uint32_t addr, void *data)
//{
//	
//	otaResumeCtx *pRam = (otaResumeCtx *)data;
//	
//	while(1)
//	{
//		otaResumeCtx *pFlash = (otaResumeCtx *)addr;
//		if(pFlash->magic == 0x1122)
//		{
//		addr +=8;	
//		}
//		else {
//		addr-=8;
//		otaResumeCtx *pFlash = (otaResumeCtx *)addr;
//		*pRam = *pFlash;
//		break;	
//		}
//	}
//}
void Flash_read_struct(uint32_t addr, void *data)
{
    otaResumeCtx *pRam = (otaResumeCtx *)data;
    uint32_t start_addr = 0x0800F800;
    uint32_t end_addr = 0x0800FC00;
    while(1)
    {
        if (addr < start_addr || addr >= end_addr) {
            memset(pRam, 0, sizeof(otaResumeCtx));
            break;
        }
        otaResumeCtx *pFlash = (otaResumeCtx *)addr;
        if(pFlash->magic == 0x1122)
        {
            addr += 8;	
        }
        else {
            addr -= 8;
            if (addr >= start_addr && addr < end_addr) {
                pFlash = (otaResumeCtx *)addr;
                *pRam = *pFlash;
            }
            break;	
        }
    }
}
/*搬整个app*/
void Flash_ReadProgram(uint32_t addr,uint32_t add, uint32_t size)
{
    uint8_t data[64];
    uint32_t i;
    uint32_t offset = 0; 
    while (offset < size)
    {
        uint32_t chunk_size = 64;
        if (offset + 64 > size) {
            chunk_size = size - offset;
        }
        for (uint32_t j = 0; j < chunk_size; j++) {
            data[j] = *(volatile uint8_t *)(addr + offset + j);
        }
        Flash_Write(add + offset, data, chunk_size);
        offset += chunk_size;
    }
}


