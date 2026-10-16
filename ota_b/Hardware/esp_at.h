#ifndef _esp_at_h
#define _esp_at_h

uint8_t esp_at(uint8_t *out_buffer);
//uint8_t at_compare(char *str1,char*str2);
uint8_t esp_compare(char * str);
uint8_t esp_at_sync(uint8_t *out_buffer);
#endif
