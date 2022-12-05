#include "adc_config.h"
#include "systick.h"
#include "fmc_flash.h"
#include "usart_config.h"
#include "gpio_config.h"

#define BAT_MAX_VOL  5.5
#define BAT_MIN_VOL  3.5


#define CLECT_LORA  gpio_input_bit_get(NB_LORA_SEL_GPIO_PROT,NB_LORA_SEL_PIN)
static uint8_t bat_per = 0;
static float current_voltage = 0;



union{
 float bat_capacity_flaot ;
 uint32_t bat_capacity_32t;
}temp;



static  nb_current_t  nb_current = {
																.stop_current_ua =  6,
																.net_connect_current_ma = 100,
																.interactive_current_ma = 100,
																.upload_data_current_ma = 120,
																.sample_current_ma = 180,
																};
nb_wakeup_stage_time_t nb_wakeup_stage_time = {0,0,0,0};
nb_capacity_of_the_consumption_t nb_capacity_of_the_consumption = {0,0,0,0};


void adc_config(void)
{
		 rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
		adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
	
    /* ADC mode config 独立模式*/
		adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1U);
    
    /* ADC trigger config */
		adc_software_trigger_enable(ADC0 , ADC_REGULAR_CHANNEL);

    /* enable ADC interface */
    adc_enable(ADC0);
    delay_1ms(1U);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
}

/*!
    \brief      ADC channel sample
    \param[in]  none
    \param[out] none
    \retval     none
*/
uint16_t adc_channel_sample(uint32_t adc_periph,uint8_t channel)
{
    /* ADC regular channel config */
    adc_regular_channel_config(adc_periph, 0U, channel, ADC_SAMPLETIME_28);
    /* ADC software trigger enable */
    adc_software_trigger_enable(adc_periph, ADC_REGULAR_CHANNEL);

    /* wait the end of conversion flag */
    while(!adc_flag_get(adc_periph, ADC_FLAG_EOC));
    /* clear the end of conversion flag */
    adc_flag_clear(adc_periph, ADC_FLAG_EOC);
    /* return regular channel sample value */
    return (adc_regular_data_read(adc_periph));
}



/* USER CODE BEGIN 1 */
//  电压：5V~6V  <=>   电量：0~100%
uint16_t  acqBat(uint32_t adc_periph,uint8_t channel){
	
	uint16_t batAdc;
	batAdc = adc_channel_sample(adc_periph,channel);
	debug_printf("batAdc is %d value\r\n",batAdc);
	return batAdc;
}
//电压计算
float batVolt(uint16_t adcVaule){
	float retVolt = 0;
	retVolt = (float)(adcVaule*3.3)/4096;
	//4370工作后大概有0.1V的压降
	debug_printf("vol is %f v\r\n",retVolt);
	return retVolt;
}
//电量百分比计算
uint8_t batPrecent(float batVolt){
	uint8_t ret = 0;
	if((batVolt - BAT_MAX_VOL)>=0){
		ret = 100;
	}else if(batVolt <= BAT_MIN_VOL){
		ret = 0;
	}else{
		
		ret = (batVolt-BAT_MIN_VOL)*100.0 / (BAT_MAX_VOL - BAT_MIN_VOL) ;
	}

	return ret;
}

uint8_t BatGetPrecent()
{
	float bat_sum = 0;
	float bat_avg = 0;
	uint32_t time = 0;
	uint8_t i = 10;
	while(i-->0){
		bat_sum +=batVolt(acqBat(ADC0,ADC_CHANNEL_10));
	}
	bat_avg = 	bat_sum/10;
//	bat_per = batPrecent(3*bat_avg);
	current_voltage = 3*bat_avg;
	if(CLECT_LORA){
		if(current_voltage> BAT_MAX_VOL){
			WriteFlashBatteryCapacity(LORA_COLLECT_DATA_MAX_COUNTER);
			bat_per = 100;
		}else{
			time =  *(__IO uint32_t *)BATTERY_CAPACITY_BASE;
			time -=1;
			WriteFlashBatteryCapacity(time);
			debug_printf("collect  timer  %d\r\n ",time);
			bat_per = time*100/LORA_COLLECT_DATA_MAX_COUNTER;
		}
	}else{
		temp.bat_capacity_32t =  *(__IO uint32_t *)BATTERY_CAPACITY_BASE;
		bat_per  = temp.bat_capacity_flaot*100/ BATTERY_FULL_CAPACITY;
		debug_printf("current_voltage is %f v\r\n",current_voltage);
		if(current_voltage> BAT_MAX_VOL){
		bat_per = 100;
		}
	}

	return bat_per;
}

inline uint8_t BatLastPer()
{
	return bat_per;
}

//计算NB唤醒后各个阶段的电流消耗总和
void wakeup_to_stop_capacity_consume_mah(nb_wakeup_stage_time_t time){
	float capacity_consume_total_mah = 0;
	
	nb_capacity_of_the_consumption.stop_current_mah = time.stop_current_time_s*nb_current.stop_current_ua/1000/3600*12;
	debug_printf("time.stop_current_time_s = %d\r\n" , time.stop_current_time_s);
	debug_printf("stop_current_mah = %f\r\n" , nb_capacity_of_the_consumption.stop_current_mah);
	
	nb_capacity_of_the_consumption.net_connect_current_mah = time.net_connect_current_time_s*nb_current.net_connect_current_ma/3600;
	debug_printf("time.net_connect_current_time_s = %d\r\n" , time.net_connect_current_time_s);
	debug_printf("net_connect_current_mah = %f\r\n" , nb_capacity_of_the_consumption.net_connect_current_mah);
	
	nb_capacity_of_the_consumption.interactive_current_mah = time.interactive_current_time_s*nb_current.interactive_current_ma/3600;
	debug_printf("time.interactive_current_time_s = %d\r\n" , time.interactive_current_time_s);
	debug_printf("interactive_current_mah = %f\r\n" , nb_capacity_of_the_consumption.interactive_current_mah);
	
	nb_capacity_of_the_consumption.sample_current_mah = time.sample_current_time_s*nb_current.sample_current_ma/3600;
	debug_printf("time.sample_current_time_s = %d\r\n" , time.sample_current_time_s);
	debug_printf("sample_current_mah = %f\r\n" , nb_capacity_of_the_consumption.sample_current_mah);
	
	nb_capacity_of_the_consumption.upload_data_current_mah = nb_wakeup_stage_time.upload_data_time_s*nb_current.upload_data_current_ma/3600;
	debug_printf("time.upload_data_time_s = %d\r\n" , time.upload_data_time_s);
	debug_printf("upload_data_current_mah = %f\r\n" , nb_capacity_of_the_consumption.sample_current_mah);
	
	capacity_consume_total_mah = nb_capacity_of_the_consumption.stop_current_mah + \
															 nb_capacity_of_the_consumption.net_connect_current_mah + \
															 nb_capacity_of_the_consumption.interactive_current_mah +  \
															 nb_capacity_of_the_consumption.upload_data_current_mah +  \
	                             nb_capacity_of_the_consumption.sample_current_mah;
	debug_printf("capacity_consume_total_mah = %f\r\n" , capacity_consume_total_mah);
	if(*(__IO uint32_t *)BATTERY_CAPACITY_FLAG == 0xaaaaaaaa){
		temp.bat_capacity_32t =  *(__IO uint32_t *)BATTERY_CAPACITY_BASE;
//		temp.bat_capacity_flaot = 5000;
		debug_printf("bat_capacity_32t = %d\r\n" , temp.bat_capacity_32t);
		debug_printf("bat_capacity = %f\r\n" , temp.bat_capacity_flaot);
	}else{
		temp.bat_capacity_flaot = BATTERY_FULL_CAPACITY;
	}
	temp.bat_capacity_flaot = temp.bat_capacity_flaot - capacity_consume_total_mah;
	if(current_voltage> BAT_MAX_VOL){
		temp.bat_capacity_flaot = BATTERY_FULL_CAPACITY;
	}
	debug_printf("bat is %f v\r\n",current_voltage);
	debug_printf("bat_capacity_32t = %d\r\n" , temp.bat_capacity_32t);
	debug_printf("bat_capacity = %f\r\n" , temp.bat_capacity_flaot);
	WriteFlashBatteryCapacity(temp.bat_capacity_32t);
	debug_printf("bat_capacity = %d\r\n" , *(__IO uint32_t *)BATTERY_CAPACITY_BASE);

}
