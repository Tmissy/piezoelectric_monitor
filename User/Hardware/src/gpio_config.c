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
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOA);
	
	
	/*NB_LORA SEL PROT  “˝Ω≈≈‰÷√*/
	gpio_in_config(NB_LORA_SEL_GPIO_PROT,NB_LORA_SEL_PIN,GPIO_MODE_INPUT,GPIO_PUPD_NONE);
	
	if(LORA_NB_SELECT){
		/*LORA SPI ≈‰÷√ */
		lora_spi_config();
		gpio_out_config(LORA_RST_PROT,LORA_RST_PIN,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
		gpio_in_config(NB_LORA_SEL_GPIO_PROT,NB_LORA_SEL_PIN,GPIO_MODE_INPUT,GPIO_PUPD_NONE);
	}else{
		/*NB USART ≈‰÷√ */
//		nb_usart_init();
	}
	/*MAX31865 SPI ≈‰÷√*/
//	thermocouple_spi_config();
	
	/*µÁ—πºÏ≤‚ADC≈‰÷√*/
//	adc_config();
	/*AD DCI ≈‰÷√*/
//	dci_config();
	
	/*AD8330 IO ≈‰÷√ */
	
	/*∑¢…‰ PWM IO ≈‰÷√*/
	timer_plus_output_gpio_config();
	
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
	gpio_mode_set(gpio_periph, mode, GPIO_PUPD_PULLDOWN, pin);
}




void powerOff(){


}

void powerOn(){


}






