#ifndef _MY_FLASH_H
#define _MY_FLASH_H

void flash_earse(void);
uint8_t Flash_Write(uint32_t addr, uint8_t *data, uint16_t len);
void  Flash_WritehalfWord_ota(uint32_t addr, uint16_t data);
void flash_earse_1(uint32_t adrr);


#endif
