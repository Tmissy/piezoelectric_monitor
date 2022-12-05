#ifndef _PORT_H_
#define _PORT_H_

#include "sys.h"

/*	正逻辑LED灯（PC13）		*/
#define LED PCout(13)

/*	下降沿按键（PA0）		*/
#define Button PAin(0)

/*	CK_OUT1(PC9)			*/

/*	DCI硬件同步使能控制端	*/
#define DCI_V_En	PBout(0)		//MCU模拟V
#define DCI_H_En	PAout(15)		//MCU模拟H

/*	DCI的HSYNC、VSYNC使能	*/
#define DCI_H_V_En	PCout(9)

/*	Power_PWM1(PE2)			*/
#define Power_PWM1 PEout(2)

/*	Power_PWM2(PE3)			*/
#define Power_PWM2 PEout(3)

/*	PWM_EN(PE4)				*/
#define PWM_EN PEout(4)

/*TP17焊盘脱落（PE5）		*/

/*	T_PWDN(PE6)				*/
#define T_PWDN PEout(6)




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
void GPIO_Out_config(uint32_t gpio_periph,uint32_t pin,uint32_t mode,uint32_t pull_up_down,uint8_t otype,uint32_t speed);

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
void GPIO_In_config(uint32_t gpio_periph,uint32_t pin,uint32_t mode,uint32_t pull_up_down);

//所有输入输出的GPIO口【配置】
void GPIO_IO_config(void);

#endif


