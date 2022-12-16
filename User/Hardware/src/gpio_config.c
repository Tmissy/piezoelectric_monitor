#include "gpio_config.h"
#include "timer_config.h"
#include "spi_config.h"
#include "usart_config.h"
#include "adc_config.h"
#include "dci_config.h"

#define LORA_NB_SELECT  gpio_input_bit_get(NB_LORA_SEL_GPIO_PROT,NB_LORA_SEL_PIN)

/**
*@brief		gpio ≈‰÷√
*@param   Œﬁ
*@return	Œﬁ
*/

void gpio_config(){
	// ±÷”≈‰÷√
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_GPIOE);
	
		/*POWER CONTORL µÁ‘¥øÿ÷∆“˝Ω≈≈‰÷√*/
	gpio_out_config(POWER_SYSTEM_EN_PROT, POWER_SYSTEM_EN_PIN,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	
	/*NB_LORA SEL PROT  “˝Ω≈≈‰÷√*/
	gpio_in_config(NB_LORA_SEL_GPIO_PROT,NB_LORA_SEL_PIN,GPIO_MODE_INPUT,GPIO_PUPD_NONE);

	if(LORA_NB_SELECT){
		 /* configure SPI1 GPIO */
    gpio_af_set(GPIOB, GPIO_AF_5, LORA_SPI_SCK_PIN | LORA_SPI_MISO_PIN | LORA_SPI_MOSI_PIN);
		gpio_out_config(GPIOB, LORA_SPI_SCK_PIN | LORA_SPI_MOSI_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
		gpio_in_config(GPIOB,LORA_SPI_MISO_PIN,GPIO_MODE_INPUT,GPIO_PUPD_NONE);
		//lora ∏¥Œª“˝Ω≈
		gpio_out_config(LORA_RST_PROT,LORA_RST_PIN,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
		//NSS “˝Ω≈
		gpio_out_config(LORA_SPI_NSS_PORT,LORA_SPI_NSS_PIN,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
		//NB lora —°‘Ò“˝Ω≈
		gpio_in_config(NB_LORA_SEL_GPIO_PROT,NB_LORA_SEL_PIN,GPIO_MODE_INPUT,GPIO_PUPD_NONE);
		debug_printf("LORA  SPI INIT \r\n");
		
				/*LORA SPI ≈‰÷√ */
		lora_spi_config();
	}else{
		/*NB USART ≈‰÷√ */
//		nb_usart_init();
	}
	/*MAX31865 SPI ≈‰÷√*/
//	thermocouple_spi_config();
	


	/*µÁ—πºÏ≤‚ADC≈‰÷√*/
//	adc_config();
	/*AD DCI ≈‰÷√*/
	gpio_out_config(DCI_DATA_VALIDITE_PORT, DCI_DATA_VALIDITE_PIN,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	dci_config();
	dci_dma_config();
	
	/*MS9280 “˝Ω≈≈‰÷√*/
	gpio_out_config(ADC_STDBY_H_PORT, ADC_STDBY_H_PIN,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	
	/*AD8330 IO ≈‰÷√ */
	gpio_out_config(VGA_EN_PORT, VGA_EN_PIN,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	gpio_out_config(GAIN_CONTROL_PROT,GAIN_CONTROL_PIN,GPIO_MODE_ANALOG,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
		debug_printf("NB port %d\r\n",LORA_NB_SELECT);
	
	/*∑¢…‰ PWM IO ≈‰÷√*/
	timer_plus_output_gpio_config();
	
	gpio_bit_set(DCI_DATA_VALIDITE_PORT,DCI_DATA_VALIDITE_PIN);
	
}


/******************◊‘∂Ø≈‰÷√GPIO*********************/
/*!
    \brief      set GPIO output mode
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,E,F,G,H,I) 
    \param[in]  pin: GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \param[in]  pull_up_down: GPIO pin with pull-up or pull-down resistor
      \arg        GPIO_PUPD_NONE: without weak pull-up and pull-down resistors
      \arg        GPIO_PUPD_PULLUP: with weak pull-up resistor
      \arg        GPIO_PUPD_PULLDOWN:with weak pull-down resistor
    \param[in]  otype: GPIO pin output mode
      \arg        GPIO_OTYPE_PP: push pull mode
      \arg        GPIO_OTYPE_OD: open drain mode
    \param[in]  speed: GPIO pin output max speed
      \arg        GPIO_OSPEED_2MHZ: output max speed 2M 
      \arg        GPIO_OSPEED_25MHZ: output max speed 25M 
      \arg        GPIO_OSPEED_50MHZ: output max speed 50M
      \arg        GPIO_OSPEED_200MHZ: output max speed 200M
    \param[out] none
    \retval     none
*/
void gpio_out_config(uint32_t gpio_periph,uint32_t pin,uint32_t mode,uint32_t pull_up_down,uint8_t otype,uint32_t speed)
{
//	if(gpio_periph == GPIOA)
//		rcu_periph_clock_enable(RCU_GPIOA);
//	if(gpio_periph == GPIOB)
//		rcu_periph_clock_enable(RCU_GPIOB);
//	if(gpio_periph == GPIOC)
//		rcu_periph_clock_enable(RCU_GPIOC);
//	if(gpio_periph == GPIOD)
//		rcu_periph_clock_enable(RCU_GPIOD);
//	if(gpio_periph == GPIOE)
//		rcu_periph_clock_enable(RCU_GPIOE);
//	if(gpio_periph == GPIOF)
//		rcu_periph_clock_enable(RCU_GPIOF);
//	if(gpio_periph == GPIOG)
//		rcu_periph_clock_enable(RCU_GPIOG);
//	if(gpio_periph == GPIOH)
//		rcu_periph_clock_enable(RCU_GPIOH);
//	if(gpio_periph == GPIOI)
//		rcu_periph_clock_enable(RCU_GPIOI);
	gpio_mode_set(gpio_periph, mode, pull_up_down, pin);
	gpio_output_options_set(gpio_periph, otype, speed, pin);
}


/*!
    \brief      set GPIO output mode
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,E,F,G,H,I) 
    \param[in]  pin: GPIO_PIN_x(x=0..15), GPIO_PIN_ALL
    \param[in]  mode: GPIO pin mode
      \arg        GPIO_MODE_INPUT: input mode
      \arg        GPIO_MODE_OUTPUT: output mode
    \param[in]  pull_up_down: GPIO pin with pull-up or pull-down resistor
      \arg        GPIO_PUPD_NONE: without weak pull-up and pull-down resistors
      \arg        GPIO_PUPD_PULLUP: with weak pull-up resistor
      \arg        GPIO_PUPD_PULLDOWN:with weak pull-down resistor
    \param[out] none
    \retval     none
*/
void gpio_in_config(uint32_t gpio_periph,uint32_t pin,uint32_t mode,uint32_t pull_up_down)
{
	gpio_mode_set(gpio_periph, mode, pull_up_down, pin);
}




void powerOff(){
	
//	gpio_bit_reset(POWER_SYSTEM_EN_PROT,POWER_SYSTEM_EN_PIN);
//	gpio_bit_reset(VGA_EN_PORT,VGA_EN_PIN);
//	gpio_bit_set(ADC_STDBY_H_PORT,ADC_STDBY_H_PIN);

}

void powerOn(){

	gpio_bit_set(POWER_SYSTEM_EN_PROT,POWER_SYSTEM_EN_PIN);
  gpio_bit_set(VGA_EN_PORT,VGA_EN_PIN);
	gpio_bit_set(DCI_DATA_VALIDITE_PORT,DCI_DATA_VALIDITE_PIN);
	gpio_bit_reset(ADC_STDBY_H_PORT,ADC_STDBY_H_PIN);
	
}






