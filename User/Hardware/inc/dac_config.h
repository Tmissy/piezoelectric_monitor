#ifndef DAC_CONFIG_H
#define DAC_CONFIG_H

#include "gd32f4xx.h"

#define CHANNLE_NUM       (4)
#define CHANNEL_GAIN_SIZIOF  (sizeof(vga_gain_t))
	
#define CHANNLE_GAIN_BUFFER  (CHANNLE_NUM*CHANNEL_GAIN_SIZIOF)

typedef struct{
	uint32_t flag;
	uint32_t gain;
}vga_gain_t;

void dac_config(void);
void set_gain(uint8_t gain);

#endif 

