#include "stm32f10x.h"                  // Device header


uint16_t CRC_data(uint8_t* data,uint16_t len)
{
	uint16_t crc = 0xFFFF;
	
	for(uint16_t i=0;i<len;i++)
	{
		crc ^=data[i];
		for(uint8_t j = 0;j<8;j++)
		{
			if(crc & 1)
			{
				crc>>= 1;
				crc ^= 0xA001;
			}
			else 
			{
				crc>>= 1;	
			}
		}	
	}
	return crc;
}	
