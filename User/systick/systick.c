/*!
    \file    systick.c
    \brief   the systick configuration in polling mode

    \version 2020-09-30, V1.0.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f4xx.h"
#include "systick.h"

volatile static float count_1us = 0;
volatile static float count_1ms = 0;
__IO uint32_t uwTick;
volatile static uint32_t delay;

///*!
//    \brief      configure systick
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//void systick_config(void)
//{
//    /* systick clock source is from HCLK/8 内部时钟的108M/8分频 */
//    systick_clksource_set(SYSTICK_CLKSOURCE_HCLK_DIV8);
//	//系统时钟8分频到系统滴答定时器的时钟（200M/8=25M）
//	//参数的实际与理论			理论			实际
//	//						25M/25M	= （1us）	576ns或632ns
//	//						25M/12.5M=（2us）	910ns
//	//							11M				1us
//	//							10M				1.07us
//    count_1us = (float)SystemCoreClock/8000000;//延时1us需要的时钟数量：系统时钟（个人一般取最大200M）/8分频 = 25M
//    count_1ms = (float)count_1us * 1000;//延时1ms需要的时钟数量 108M/8M *1000= 13500 
//}

///*!
//    \brief      delay a time in microseconds in polling mode
//    \param[in]  count: count in microseconds
//    \param[out] none
//    \retval     none
//*/
//void delay_1us(uint32_t count)
//{
//    uint32_t ctl;
//    
//    /* reload the count value */
//    SysTick->LOAD = (uint32_t)(count * count_1us);
//    /* clear the current count value */
//    SysTick->VAL = 0x0000U;
//    /* enable the systick timer */
//    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
//    /* wait for the COUNTFLAG flag set */
//    do{
//        ctl = SysTick->CTRL;
//    }while((ctl&SysTick_CTRL_ENABLE_Msk)&&!(ctl & SysTick_CTRL_COUNTFLAG_Msk));
//    /* disable the systick timer */
//    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
//    /* clear the current count value */
//    SysTick->VAL = 0x0000U;
//}

///*!
//    \brief      delay a time in milliseconds in polling mode
//    \param[in]  count: count in milliseconds
//    \param[out] none
//    \retval     none
//*/
//void delay_1ms(uint32_t count)
//{
//    uint32_t ctl;
//    
//    /* reload the count value */
//    SysTick->LOAD = (uint32_t)(count * count_1ms);
//    /* clear the current count value */
//    SysTick->VAL = 0x0000U;
//    /* enable the systick timer */
//    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
//    /* wait for the COUNTFLAG flag set */
//    do{
//        ctl = SysTick->CTRL;
//    }while((ctl&SysTick_CTRL_ENABLE_Msk)&&!(ctl & SysTick_CTRL_COUNTFLAG_Msk));
//    /* disable the systick timer */
//    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
//    /* clear the current count value */
//    SysTick->VAL = 0x0000U;
//}

/*!
    \brief      configure systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if(SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while(1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

/*!
    \brief      delay a time in milliseconds
    \param[in]  count: count in milliseconds
    \param[out] none
    \retval     none
*/
void delay_1ms(uint32_t count)
{
    delay = count;

    while(0U != delay){
    }
}

void Inc_Tick(void)
{
  uwTick += 1;
}
uint32_t Get_Tick(void){
	return uwTick;
}

/*!
    \brief      delay decrement
    \param[in]  none
    \param[out] none
    \retval     none
*/
void delay_decrement(void)
{
    if(0U != delay){
        delay--;
    }
}
