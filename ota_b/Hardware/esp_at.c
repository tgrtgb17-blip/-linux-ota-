#include "stm32f10x.h"
#include "serial.h"
#include "OLED.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ESP_MAX_BUF 512


uint16_t esp_at(uint8_t *out_buffer)
{
    uint8_t ch;
    uint32_t timeout = 5000000;

    char head[] = "+MQTTSUBRECV:";
    uint8_t head_idx = 0;

    char len_str[10];
    uint16_t i;
    uint16_t data_len;
	
	memset(out_buffer, 0, 512);	
	
    while(timeout--)
    {
        if(!USART1_ReadByte_NonBlock(&ch))
        {
            continue;
        }

        if(ch == head[head_idx])
        {
            head_idx++;

            if(head[head_idx] == '\0')
            {
                break;
            }
        }
        else
        {
            head_idx = 0;

            if(ch == '+')
            {
                head_idx = 1;
            }
        }
    }

    if(head[head_idx] != '\0')
    {
        return 0;
    }
	
	
	head_idx = 0; 

    while(1)
    {
        if(USART1_ReadByte_NonBlock(&ch))
        {
            if(ch == ',')
            {
                break;
            }
        }

        IWDG_ReloadCounter();
    }

    while(1)
    {
        if(USART1_ReadByte_NonBlock(&ch))
        {
            if(ch == '"')
            {
                break;
            }
        }

        IWDG_ReloadCounter();
    }

    while(1)
    {
        if(USART1_ReadByte_NonBlock(&ch))
        {
            if(ch == '"')
            {
                break;
            }
        }

        IWDG_ReloadCounter();
    }

    while(1)
    {
        if(USART1_ReadByte_NonBlock(&ch))
        {
            if(ch == ',')
            {
                break;
            }
        }

        IWDG_ReloadCounter();
    }

    i = 0;

    while(1)
    {
        if(USART1_ReadByte_NonBlock(&ch))
        {
            if(ch == ',')
            {
                len_str[i] = '\0';
                break;
            }

            if(i < sizeof(len_str) - 1)
            {
                len_str[i++] = ch;
            }
        }

        IWDG_ReloadCounter();
    }

    data_len = atoi(len_str);

    if(data_len >= ESP_MAX_BUF)
    {
        return 0;
    }

    i = 0;

    while(i < data_len)
    {
        if(USART1_ReadByte_NonBlock(&ch))
        {
            out_buffer[i++] = ch;
        }

        IWDG_ReloadCounter();
    }

    return data_len;
}

uint8_t esp_compare(char *str)
{
    static uint8_t my_buffer[ESP_MAX_BUF];

    uint16_t len;

    len = esp_at(my_buffer);

    if(len == 0)
    {
        return 0;
    }

    my_buffer[len] = '\0';

    if(strstr((char *)my_buffer, str) != NULL)
    {
        return 1;
    }

    return 0;
}

