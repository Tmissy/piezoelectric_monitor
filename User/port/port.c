#include "port.h"


/************************GPIO简化代码 V2.0*****************************

________________________手动配置GPIO【输出】________________________

//1、GPIO时钟使能
rcu_periph_clock_enable(RCU_GPIOC);
//2、		设置端口、设置输出、		设置输出、		设置管脚号
gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_13);
//3、				  设置端口、设置类型、		设置速度、		设置管脚号
gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_13);

________________________手动配置GPIO【输入】________________________
//				设置端口、设置端口输入		设置端口结构		设置端口号
gpio_mode_set(gpio_periph, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, pin);

*****************************************************/





/******************自动配置GPIO*********************/
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
void GPIO_Out_config(uint32_t gpio_periph,uint32_t pin,uint32_t mode,uint32_t pull_up_down,uint8_t otype,uint32_t speed)
{
	if(gpio_periph == GPIOA)
		rcu_periph_clock_enable(RCU_GPIOA);
	if(gpio_periph == GPIOB)
		rcu_periph_clock_enable(RCU_GPIOB);
	if(gpio_periph == GPIOC)
		rcu_periph_clock_enable(RCU_GPIOC);
	if(gpio_periph == GPIOD)
		rcu_periph_clock_enable(RCU_GPIOD);
	if(gpio_periph == GPIOE)
		rcu_periph_clock_enable(RCU_GPIOE);
	if(gpio_periph == GPIOF)
		rcu_periph_clock_enable(RCU_GPIOF);
	if(gpio_periph == GPIOG)
		rcu_periph_clock_enable(RCU_GPIOG);
	if(gpio_periph == GPIOH)
		rcu_periph_clock_enable(RCU_GPIOH);
	if(gpio_periph == GPIOI)
		rcu_periph_clock_enable(RCU_GPIOI);
	
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
void GPIO_In_config(uint32_t gpio_periph,uint32_t pin,uint32_t mode,uint32_t pull_up_down)
{
	if(gpio_periph == GPIOA)
		rcu_periph_clock_enable(RCU_GPIOA);
	if(gpio_periph == GPIOB)
		rcu_periph_clock_enable(RCU_GPIOB);
	if(gpio_periph == GPIOC)
		rcu_periph_clock_enable(RCU_GPIOC);
	if(gpio_periph == GPIOD)
		rcu_periph_clock_enable(RCU_GPIOD);
	if(gpio_periph == GPIOE)
		rcu_periph_clock_enable(RCU_GPIOE);
	if(gpio_periph == GPIOF)
		rcu_periph_clock_enable(RCU_GPIOF);
	if(gpio_periph == GPIOG)
		rcu_periph_clock_enable(RCU_GPIOG);
	if(gpio_periph == GPIOH)
		rcu_periph_clock_enable(RCU_GPIOH);
	if(gpio_periph == GPIOI)
		rcu_periph_clock_enable(RCU_GPIOI);
	
	gpio_mode_set(gpio_periph, mode, GPIO_PUPD_PULLDOWN, pin);
}


void GPIO_IO_config(void)
{
	/*	LED（PC13）灯使能	*/
	GPIO_Out_config(GPIOC,GPIO_PIN_13,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_2MHZ);
	
	/*	Button(PA0)按键使能	*/
	GPIO_In_config(GPIOA,GPIO_PIN_0,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP);
	
	/*	CK_OUT1(PC9)				*/
	GPIO_Out_config(GPIOC,GPIO_PIN_9,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);
	
	/*	DCI硬件同步使能控制端		*/
	GPIO_Out_config(GPIOB,GPIO_PIN_0,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_2MHZ);
	GPIO_Out_config(GPIOA,GPIO_PIN_15,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_2MHZ);
	
	/*	DCI的HSYNC、VSYNC使能		*/
	GPIO_Out_config(GPIOC,GPIO_PIN_9,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	
	
	/*	Power_PWM1(PE2)				*/
	GPIO_Out_config(GPIOE,GPIO_PIN_2,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	
	
	/*	Power_PWM2(PE3)				*/
	GPIO_Out_config(GPIOE,GPIO_PIN_3,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);
	
	/*	PWM_EN(PE4)*/
	GPIO_Out_config(GPIOE,GPIO_PIN_4,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);
	
	/*	TP17焊盘脱落（PE5）			*/
	
	/*	T_PWDN(PE6)					*/
	GPIO_Out_config(GPIOE,GPIO_PIN_6,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);
	

	


}

