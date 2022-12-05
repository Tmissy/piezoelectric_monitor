#include "dac_config.h"


#define GAIN_RANGE  48
#define GAIN_1V_VALUE   1241
//#define GAIN_1V5_VALUE   1862

/*!
    \brief      configure the DAC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_config(void){
		
		rcu_periph_clock_enable(RCU_DAC);
	
    dac_deinit();
		rcu_periph_clock_enable(RCU_DAC);
    /* configure the DAC_OUT0 */
    dac_trigger_disable(DAC0);
    dac_wave_mode_config(DAC0, DAC_WAVE_DISABLE);
    dac_output_buffer_enable(DAC0);

		dac_data_set(DAC0, DAC_ALIGN_12B_R, GAIN_1V_VALUE);                                  //ÉèÖÃDAC0 Êý¾Ý
		dac_enable(DAC0);

}


void set_gain(uint8_t gain){
	if(gain>GAIN_RANGE){
		return ;
	}
	uint16_t convert_gain = gain *(GAIN_1V_VALUE)/GAIN_RANGE;
	dac_data_set(DAC0, DAC_ALIGN_12B_R, convert_gain); 
	
}

