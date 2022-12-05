#ifndef ADC_CONFIG_H
#define ADC_CONFIG_H

#include "gd32f4xx.h"
#include "systick.h"


#define LORA_COLLECT_DATA_MAX_COUNTER   (3000)
#define BATTERY_FULL_CAPACITY (4500)

typedef struct {
	float   stop_current_ua ;
	float   net_connect_current_ma;
	float   interactive_current_ma;
	float   upload_data_current_ma;
	float   sample_current_ma;

}nb_current_t;

typedef struct {
	uint32_t   stop_current_time_s ;
	uint16_t   net_connect_current_time_s;
	uint16_t   interactive_current_time_s;
	uint16_t   upload_data_time_s;
	uint16_t   sample_current_time_s;
}nb_wakeup_stage_time_t;

typedef struct {
	float   system_init_mah;
	float   stop_current_mah ;
	float   net_connect_current_mah;
	float   interactive_current_mah;
	float   upload_data_current_mah;
	float   sample_current_mah;
}nb_capacity_of_the_consumption_t;


extern nb_wakeup_stage_time_t nb_wakeup_stage_time ;
extern nb_capacity_of_the_consumption_t nb_capacity_of_the_consumption ;
void wakeup_to_stop_capacity_consume_mah(nb_wakeup_stage_time_t time);


void adc_config(void);
uint16_t adc_channel_sample(uint32_t adc_periph,uint8_t channel);
float batVolt(uint16_t adcVaule);
uint8_t batPrecent(float batVolt);
uint8_t BatGetPrecent(void);
uint8_t BatLastPer(void);
#endif 

