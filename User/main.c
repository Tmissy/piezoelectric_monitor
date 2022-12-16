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
#include "fmc_flash.h"
#include "dac_config.h"
u8 flag_uart_out = 0;			//串口发出标志位：0--不发出，1--发出

u16 dci_average_count = 1024;		//设置多少次平均

#define Sampling_time 60		//DCI数据采集的延时时间

extern u16 battery_DCI_value_arry[Sampling_number];

u16 dci_average_value[Sampling_number] = {0};	//DCI的平均值所存放的数组


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
    \brief      串口发送DCI获取的电磁探测数据
    \param[in]  none
    \param[out] none
    \retval     none
*/



int main(void)
{
 	systick_config();			//1_ systick config and init
	

	gd_debug_com_init();
	gpio_config();
	nvic_irq_enable(TIMER3_IRQn, 0, 1);
	nvic_irq_enable(TIMER1_IRQn, 0, 2);
	nvic_irq_enable(TIMER7_UP_TIMER12_IRQn, 0, 1);
	printf("hello\r\n");
	debug_printf("system clock: %d\r\n",rcu_clock_freq_get(CK_AHB));
  timer_config();
	write_Flash_double_Word(SENSOR_ID_ADDR, (uint64_t)10010001202212090001);
	write_network_id_to_flash(SENSOR_PARAMETER_ADDR,5134);
	debug_printf("sensorID: %llu\r\n",read_sensorid_form_flash(SENSOR_ID_ADDR));
	communication_reboot();
	communication_init();
	powerOn();
	while(1)
	{
//		loraTxData(&lora_ctrl,'a','R');
//		delay_1ms(1000);
	//通信模块连接网络，限制120秒
		tick = Get_Tick();	
//		debug_printf("communication connect use : %d ms!\r\n",net_c);	
//	if (net_wait_con(tick/1000.0, 120))
//	{		
//		net_con_time = (Get_Tick() - tick);
//		debug_printf("communication connect use : %d ms!\r\n",net_con_time);		
//		//执行debug请求
		if(!communication_req())	
		{
			//请求失败并且采样时间到达，此时执行自动采样上传
			//			sample_wakeup = 0;		
			tick = Get_Tick();
//			StartSampleUpload(3, allChannelList, 1);	
			interactive	= Get_Tick() - tick;
		}		
//	}
//	else{
////		debug_printf("1\r\n");
//	}
	}
}



