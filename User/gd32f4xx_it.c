/*!
    \file    gd32f4xx_it.c
    \brief   interrupt service routines
    
    \version 2016-08-15, V1.0.0, firmware for GD32F4xx
    \version 2018-12-12, V2.0.0, firmware for GD32F4xx
    \version 2020-09-30, V2.1.0, firmware for GD32F4xx
    \version 2022-03-09, V3.0.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

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

#include "gd32f4xx_it.h"
#include "systick.h"
#include "usart_config.h"
#include "dci_config.h"
#include "service_logic.h"
#include "gpio_config.h"

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1){
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}


static  volatile uint16_t sample_count = 0;
/*!
    \brief      this function handles TIMER2 interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER3_IRQHandler(void)
{
		static uint8_t firstPowerFlag = 0;
    if(SET == timer_interrupt_flag_get(TIMER3, TIMER_INT_UP)){
			if(get_sample_flag()){
				sample_count++;
				if(sample_count > get_sample_count()){
				debug_printf("sample_count%d\r\n",sample_count);
					debug_printf("get_sample_count()%d\r\n",get_sample_count());
					sample_flag_clear();
					timer_disable(TIMER3);
					sample_count = 0;
				}
			}
			
			dci_disable();						//ʧ��DCI
			dma_channel_disable(DMA1,DMA_CH1);	//ʹ��DMA��CH1
			dci_config();
			dci_dma_config();
			
//			dci_enable();						//ʹ��DCI
//			dma_channel_enable(DMA1,DMA_CH1);	//ʹ��DMA��CH1
			timer_interrupt_flag_clear(TIMER3, TIMER_INT_UP);
    }
}

/*!
    \brief      this function handles TIMER2 interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER1_IRQHandler(void)
{
		static uint8_t firstPowerFlag = 0;
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_UP)){
			timer_disable(TIMER1);
			dci_enable();						//ʹ��DCI
			dma_channel_enable(DMA1,DMA_CH1);	//ʹ��DMA��CH1
			gpio_bit_reset(DCI_DATA_VALIDITE_PORT,DCI_DATA_VALIDITE_PIN);
			timer_interrupt_flag_clear(TIMER1, TIMER_INT_UP);
    }
}


/*!
    \brief      this function handles TIMER2 interrupt request.
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER7_UP_TIMER12_IRQHandler(void)
{
		static uint16_t count = 0;
		static uint8_t firstPowerFlag = 0;
    if(SET == timer_interrupt_flag_get(TIMER7, TIMER_INT_UP)){

			timer_disable(TIMER1);
			timer_disable(TIMER7);
			gpio_bit_set(DCI_DATA_VALIDITE_PORT,DCI_DATA_VALIDITE_PIN);
			acumulator_sensor_data(dci_sampling_value_buffer);
			dci_disable();						//ʧ��DCI
			dma_channel_disable(DMA1,DMA_CH1);	//ʹ��DMA��CH1
			dci_config();
			dci_dma_config();
			
			timer_interrupt_flag_clear(TIMER7, TIMER_INT_UP);
    }
}


/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    delay_decrement();
		Inc_Tick();
}
