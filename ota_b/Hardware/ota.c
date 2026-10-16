#include "stm32f10x.h"
#include "serial.h"
#include "my_flash.h"
#include "ota.h"
#include "crc.h"
#include "LED.h"
#include "info.h"
#include "jump.h"
#include "esp_at.h"
#include "esp8266.h"
#include "delay.h"
#include "OLED.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define Cerror_ota 0x0800FC10
#define APP_ADDR 0x08002800
#define APP_bkp 0x08008C00
#define INFO_ADDR 0x08008BF4
#define INFO_v2_ADDR 0x0800F7F4
#define NUM_ADDR 0x0800F814

#define wait_OTA_flag 0x0800F800
#define DEFAULT_APP_SIZE 4000

info_struct ota_up;
otaResumeCtx ota_flash,ota_read;
mqtt_add mqtt_ota_s;
uint8_t buffer[512];
uint8_t mqtt_buffer[512];

void OTA_kill(void)
{
    uint32_t value;

    __disable_irq();

    FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
    FLASH_ErasePage(wait_OTA_flag);

    FLASH_Lock();

    __enable_irq();
	
    value = *(volatile uint32_t *)wait_OTA_flag;

    if(value != 0xFFFFFFFF)
    {

        while(1);
    }
    NVIC_SystemReset();

    while(1);
}



void OTA_Update(uint8_t pan)
{
    uint16_t num = 1;
   
    uint16_t payload_len = 0;
	uint16_t crc;
    uint16_t num_become = 8;

    uint32_t c_version;
    uint32_t old_app_size;

    uint32_t addr = APP_ADDR;
    uint32_t num_addr = NUM_ADDR;
    uint32_t read_addr = NUM_ADDR;
	OLED_Init();
	OLED_Clear();
	
	char mqtt_result[64];

    ota_up.magic = 0x12344321;
	
	while(1)
	{
		if(esp_compare("OTA"))
		{
		break;
		}
	}
    if(pan == '0')
    {
        esp_publish("ota/001/result","start");

		payload_len = esp_at(mqtt_buffer);
		mqtt_buffer[payload_len] = '\0';
		
		ota_up.version = atoi((char*)mqtt_buffer);
        c_version = *(volatile uint32_t *)INFO_ADDR;

        if(c_version != 0xFFFFFFFF && c_version >= ota_up.version)
        {
            esp_publish("ota/001/result","same");
            OTA_kill();
            return;
        }
		
		esp_publish("ota/001/result","different");
		
		flash_earse_1(INFO_v2_ADDR);
		flash_earse_1(Cerror_ota);
		
        Flash_ReadProgram(INFO_ADDR,INFO_v2_ADDR,12);
        old_app_size = *(volatile uint32_t *)(INFO_v2_ADDR + 4);
		if(old_app_size == 0xFFFFFFFF)
		{
			old_app_size = 6000;
		}
		
        flash_earse_8(APP_bkp);
        Flash_ReadProgram(APP_ADDR,APP_bkp,old_app_size);
        
		flash_earse_8(APP_ADDR);
		flash_earse_1(INFO_ADDR); 
		esp_at(mqtt_buffer);
		
		ota_up.num = ((uint32_t)mqtt_buffer[0]) | ((uint32_t)mqtt_buffer[1] << 8) | ((uint32_t)mqtt_buffer[2] << 16) | ((uint32_t)mqtt_buffer[3] << 24);

        
    }
    else if(pan == '1')
    {
        esp_publish("ota/001/result","force");

        Flash_read_struct(read_addr,&ota_read);
		mqtt_ota_s.pack = ota_read.pack_num;
		mqtt_ota_s.addr = ota_read.address;
		
		
		payload_len = esp_at(mqtt_buffer);
		mqtt_buffer[payload_len] = '\0';
		ota_up.version = atoi((char*)mqtt_buffer);
		
		sprintf(mqtt_result,"pack=%u;addr=%u",mqtt_ota_s.pack,mqtt_ota_s.addr);
		
		Delay_ms(100);
		
		esp_publish("ota/001/result",mqtt_result);

        num = ota_read.pack_num;
        addr = ota_read.address;
		num_addr = NUM_ADDR + ota_read.pack_num * 8;
		num = num + 1;
		
		esp_at(mqtt_buffer);
		ota_up.num = ((uint32_t)mqtt_buffer[0]) | ((uint32_t)mqtt_buffer[1] << 8) | ((uint32_t)mqtt_buffer[2] << 16) | ((uint32_t)mqtt_buffer[3] << 24);
    }

    ota_buffer ota_struct;

    while(1)
    {
        ota_flash.magic = 0x1122;
        ota_flash.pack_num = 0;
        ota_flash.address = 0;

        if(esp_at(mqtt_buffer) == 0)
		{
			continue;
		}
        ota_struct.number = mqtt_buffer[0];

        ota_flash.pack_num = ota_struct.number;

        if(ota_struct.number == 0)
        {
            esp_publish("ota/001/result","OK");

            LED1_OFF();
			
			Flash_WriteWord_ota(INFO_ADDR,ota_up.version);
			Flash_WriteWord_ota(INFO_ADDR + 4,ota_up.num);
            Flash_WriteWord_ota(INFO_ADDR + 8,ota_up.magic);

            FLASH_ErasePage(NUM_ADDR);
			Flash_WritehalfWord_ota(Cerror_ota,0x6666);
			
            FLASH_ErasePage(wait_OTA_flag);

            OTA_kill();

            break;
        }

        if(ota_struct.number != num)
        {
            esp_publish("ota/001/result","lose");

            old_app_size = *(volatile uint32_t *)(INFO_v2_ADDR + 4);
			
			
			flash_earse_8(APP_ADDR);
			flash_earse_1(INFO_ADDR);
			flash_earse_1(wait_OTA_flag);
            Flash_ReadProgram(APP_bkp,APP_ADDR,old_app_size);
            Flash_ReadProgram(INFO_v2_ADDR,INFO_ADDR,12);

            break;
        }

        ota_struct.len = mqtt_buffer[1];

        memcpy(buffer,&mqtt_buffer[2],ota_struct.len);

        ota_struct.crc = mqtt_buffer[2 + ota_struct.len] |((uint16_t)mqtt_buffer[3 + ota_struct.len] << 8);

        crc = CRC_data(buffer,ota_struct.len);


        if(ota_struct.crc == crc)
        {
            Flash_Write(addr,buffer,ota_struct.len);

            addr += ota_struct.len;

            ota_flash.pack_num = ota_struct.number;
            ota_flash.address = addr;

            esp_publish("ota/001/result","success");
	
			
            Flash_WriteStruct(num_addr,&ota_flash,num_become);

            num_addr += num_become;

            num += 1;
        }
        else
        {
            esp_publish("ota/001/result","error");

            old_app_size = *(volatile uint32_t *)(INFO_v2_ADDR + 4);

            flash_earse_8(APP_ADDR);
			flash_earse_1(INFO_ADDR);
			flash_earse_1(wait_OTA_flag);
            Flash_ReadProgram(APP_bkp,APP_ADDR,old_app_size);
            Flash_ReadProgram(INFO_v2_ADDR,INFO_ADDR,12);;

            break;
        }
    }


}

void OTA_register(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP,ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	BKP_WriteBackupRegister(BKP_DR1,0x55AA);
	NVIC_SystemReset();
}

