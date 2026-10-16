#ifndef _OTA_H
#define _OTA_H
#include <stdint.h>

void OTA_register(void);
void OTA_Update(uint8_t pan);
void OTA_kill(void);

typedef struct
{		
	uint8_t number;
	uint32_t address;
	uint8_t len;
	uint8_t buffer[256];
	uint16_t crc;
}ota_buffer;

typedef struct{
	uint32_t magic;
	uint32_t version;
	uint32_t num;
}info_struct;

typedef struct{
	uint16_t magic;
	uint16_t pack_num;
	uint32_t address;
}otaResumeCtx;

typedef struct{
	uint16_t pack;
	uint32_t addr;
}mqtt_add;

#endif
