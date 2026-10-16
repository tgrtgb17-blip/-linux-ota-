#ifndef _MY_FLASH_H
#define _MY_FLASH_H

void flash_earse_8(uint32_t adrr);
void flash_earse_1(uint32_t adrr);
uint8_t Flash_Write(uint32_t addr, uint8_t *data, uint16_t len);
void  Flash_WriteWord_ota(uint32_t addr, uint32_t data);
void Flash_WriteStruct(uint32_t addr, void *data, uint16_t size);
void Flash_read_struct(uint32_t addr, void *data);
void Flash_ReadProgram(uint32_t addr,uint32_t add, uint32_t size);
void  Flash_WritehalfWord_ota(uint32_t addr, uint16_t data);
#endif
