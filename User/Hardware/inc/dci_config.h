#ifndef DCI_CONFIG_H
#define DCI_CONFIG_H

#include "gd32f4xx.h"

#define SAMPLING_DEPTH (2048)
#define DCI_DR_ADDRESS     (0x50050028U)		//DCI的4字节数据存储器

#define SENSOR_DATA_LEN (2048)
#define ORIUTG_DATA_LEN (sizeof(ORIUTG_Data_t))
#define LORA_TX_DATA_LEN  (sizeof(ORIUTG_Data_t))
typedef struct{ //4128
	uint64_t sensorId;
	uint8_t head[8];
	uint16_t length;
	int16_t temperate;
	uint8_t  average;
	uint8_t  gain;
	uint8_t  ADSampleDiv;
	uint8_t  Battery;
	uint32_t networkId;
	uint32_t channel_num;
	uint16_t Data[SENSOR_DATA_LEN];
	uint8_t tail[8];
}ORIUTG_Data_t;



typedef struct loradata_t
{
	union{
	ORIUTG_Data_t loraTxData;
	uint8_t tx_buf[LORA_TX_DATA_LEN];
	};
}Tx_ORIUTG_Data_t;

extern uint32_t  dci_sampling_value_buffer[SAMPLING_DEPTH];

void dci_config(void);
void dci_dma_config(void);
#endif 

