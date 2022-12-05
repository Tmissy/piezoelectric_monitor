#include "gd32f4xx.h"


#include "sys.h"
#include "systick.h"
#include "port.h"
#include "dci_adc.h"
#include "my_usart.h"
#include "gpio_config.h"
#include "spi_config.h"
#include "timer_config.h"
#include "usart_config.h"
#include "lora.h"
#include "communicate.h"
u8 flag_uart_out = 0;			//���ڷ�����־λ��0--��������1--����

u16 dci_average_count = 1024;		//���ö��ٴ�ƽ��

#define Sampling_time 60		//DCI���ݲɼ�����ʱʱ��

extern u16 battery_DCI_value_arry[Sampling_number];

u16 dci_average_value[Sampling_number] = {0};	//DCI��ƽ��ֵ����ŵ�����


static uint32_t tick = 0;
uint32_t net_con_time = 0;

uint16_t adcVoltTest = 0;
Lora_Control_Def lora_ctrl={0};
SX1278_hw_t SX1278_hw;
SX1278_t SX1278;

uint8_t test[10] = {0,0x5a,0x5a};
uint32_t* ptest = NULL;

uint8_t len = 0;
uint32_t interactive = 0;

/*!
    \brief      ���ڷ���DCI��ȡ�ĵ��̽������
    \param[in]  none
    \param[out] none
    \retval     none
*/



int main(void)
{
	systick_config();			//1_ systick config and init
	
	nvic_irq_enable(TIMER3_IRQn, 1, 1);
	nvic_irq_enable(TIMER1_IRQn, 2, 2);
	gpio_config();
	gd_debug_com_init();
	printf("hello\r\n");
	debug_printf("system clock: %d\r\n",rcu_clock_freq_get(CK_AHB));
  timer_config();

	communication_init();
	while(1)
	{

	}
}



