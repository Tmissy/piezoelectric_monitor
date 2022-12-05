#include "communicate.h"
#include "gpio_config.h"
#include "sx1278.h"
#include "lora.h"
#include "systick.h"
#include "dev_sim7020.h"
#include "spi_config.h"
#include "net_protocol.h"
#include "fmc_flash.h"
#include "usart_config.h"
#include "service_logic.h"
#include "rtc_config.h"
#include "adc_config.h"

static bool lsUsedLora = false;
extern bool ls_con_net;
extern Tx_ORIUTG_Data_t  oriutg_data ;

static uint8_t debug_req_buf[64];
static uint16_t send_size = 0;

#define TWELVE_HOURS_TIME   (43200)
#define MAX_WAKEUP_TIME     (65535)
/**
*@brief		ͨ��ģ���ʼ��
*@param     ��
*@note      ��
*@return	true: ��ʼ���ɹ�
*			false: ��ʼ��ʧ��
*/
bool communication_init()
{
	uint8_t err = 0;
	if (lsUsedLora)
	{
		lora_ctrl_init(&SX1278_hw, &SX1278,&lora_ctrl, SPI0);
		SX1278_LORA_CAD(&SX1278);
		delay_1ms(3);

	}
	else
	{
		while(err < 3){
			err++;
			sim7020_reboot();
			if(!sim7020_init())continue;
			return true;
		}
		return false;
	}
	
	return true;
}

/**
*@brief		ͨ��ģ������
*@param     ��
*@note      ��
*@return	true: �����ɹ�
*			false: ����ʧ��
*/
bool communication_reboot()
{
	lsUsedLora = gpio_input_bit_get(NB_LORA_SEL_GPIO_PROT,NB_LORA_SEL_PIN);

	if (lsUsedLora)
	{
		debug_printf("lora init\r\n");
		return true;
	}
	else
	{
		debug_printf("nb init\r\n");
		nb_usart_init();
		ReLoadSersorId();
		net_get_debug_data((char*)debug_req_buf, &send_size);
		at_parse_init();				
		return sim7020_reboot();
	}
	
}


/**
*@brief		ͨ��ģ������
*@param     ��
*@note      ��
*@return	true: �����ɹ�
*			false: ����ʧ��
*/
bool communication_sleep()
{
	if (lsUsedLora)
	{
		 SX1278_sleep(lora_ctrl.pSX1278);
	}
	else
	{
		return sim7020_sleep(0);
//		gpio_bit_set(POWER_SYS_EN_GPIO_PROT,POWER_SYS_EN_PIN);
	}
	return true;
}

/**
*@brief		��ʱ�����ʽֵת��Ϊ��
*@param     hms_time: Ҫ�����ʱ��ṹ�����
*@return	�볤��
*/
static uint32_t hms_to_sec(sleep_time_t* hms_time)
{
	return (hms_time->hour * 3600) + (hms_time->min * 60) + hms_time->sec;
}

/**
*@brief		����ͨ��ģ������ʱ��
*@param     ��
*@note      ��
*			false: ����ʧ��
*/
void set_Communication_RTC_Time()
{
	uint32_t sample_sec = 0;
	if (lsUsedLora)
	{
		 rtc_configuration(1);
	}
	else
	{
			uint32_t * temp_sample_time = NULL;	
			temp_sample_time = readFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t));
			memcpy((uint8_t*)&sample_time,(uint8_t*)temp_sample_time,sizeof(sleep_time_t));
			debug_printf(" sample_time.flag %x\r\n",sample_time.flag);
			debug_printf(" sample_time.wakeup_count %x\r\n",sample_time.wakeup_count);
			debug_printf(" sample_time.hour %d\r\n",sample_time.hour);
			debug_printf(" sample_time.min  %d\r\n",sample_time.min);
			debug_printf(" sample_time.sec  %d\r\n",sample_time.sec);
			sample_sec = hms_to_sec(&sample_time);
			if(sample_sec >= MAX_WAKEUP_TIME){
				sample_sec -= sample_time.wakeup_count*MAX_WAKEUP_TIME;
			}
			if(sample_time.flag != 0xaaaa){
				writeFlash_X_Word(FWDGT_RESET_FLAG,1,0);
				rtc_configuration(TWELVE_HOURS_TIME);
				debug_printf ("sample_time.flag = aaaa\r\n");
			}else{
			if(sample_sec >MAX_WAKEUP_TIME){
				sample_time.wakeup_count++;
				fmc_erase_pages(RTC_STANDBY_TIME_ADDR);	
				writeFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t),(uint32_t*) &sample_time);
				rtc_configuration(MAX_WAKEUP_TIME);
				debug_printf ("MAX_WAKEUP_TIME s\r\n");
			}
			else{
				writeFlash_X_Word(FWDGT_RESET_FLAG,1,0);
				sample_time.wakeup_count = 0;
				fmc_erase_pages(RTC_STANDBY_TIME_ADDR);	
				writeFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t),(uint32_t*) &sample_time);
				rtc_configuration(sample_sec);
				debug_printf ("sample_time.flag  wake up \r\n");
			}
		}
	}
}

/**
*@brief		����ͨ��ģ������ʱ��
*@param     ��
*@note      ��
*			false: ����ʧ��
*/
void wake_Up_Count()
{
	uint32_t sample_sec = 0;
	if (lsUsedLora)
	{
		
	}
	else
	{
		uint32_t * temp_sample_time = NULL;	
		temp_sample_time = readFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t));	
		memcpy((uint8_t*)&sample_time,(uint8_t*)temp_sample_time,sizeof(sleep_time_t));
		debug_printf(" sample_time.flag %x\r\n",sample_time.flag);
		debug_printf(" sample_time.wakeup_count %x\r\n",sample_time.wakeup_count);
		debug_printf(" sample_time.hour %d\r\n",sample_time.hour);
		debug_printf(" sample_time.min  %d\r\n",sample_time.min);
		debug_printf(" sample_time.sec  %d\r\n",sample_time.sec);
		sample_sec = hms_to_sec(&sample_time);
		debug_printf(" sample_sec  %d\r\n",sample_sec);
		if(sample_time.flag != 0xaaaa){
			sample_wakeup = true;
		}else{
			if(sample_time.wakeup_count == 0){
				sample_wakeup = true;
			}else{
				sample_wakeup = false;
			}
		}
		
	}
}


/**
*@brief		���ͨ��ģ���Ƿ�����
*@param     ��
*@note      ��
*@return	true: ������
*			false: δ����
*/
bool communication_ls_con()
{
	if (lsUsedLora)
	{
		 return (SX1278_SPIRead(&SX1278, LR_RegIrqFlags) == 0x05);
	}
	else
	{
		return sim7020_check_net();
	}
	
}

/**
*@brief		���ͨ��ģ�����ڵ���ģʽ
*@param     ��
*@note      ��
*@return	true: ������
*			false: δ�ڵ���״̬
*/

bool communication_ls_debug()
{
	if (lsUsedLora)
	{
		 return false;
	}
	else
	{
		return (net_get_debug_status() != DEBUG_MODE_DONE);
	}
	
}

/**
*@brief		�ȴ�ͨ��ģ������
*@param     timeout���ȴ�ʱ�䣬��λ��s
*@note      ��ʹ��loraģ�飬��ֻ�Ǽ��һ�Σ���ʱʱ��δʹ��
*@return	true: �����ɹ�
*			false: ������ʱ
*/

bool net_wait_con(uint16_t start_sec, uint16_t timeout)
{
	uint16_t timeout_cnt = 0;
	if (lsUsedLora)
	{
		return (SX1278_SPIRead(&SX1278, LR_RegIrqFlags) == 0x05);
	}
	else
	{
		while(timeout_cnt < timeout)
		{
			 if(!communication_ls_con())
			 {
				 timeout_cnt = Get_Tick()/1000.0 - start_sec;
				 printf("wait net contect %d sec\r\n",timeout_cnt);
				 continue;
			 }
			 else
			 {
				 return true;
			 }
		}
		
	}
	return ls_con_net;
}


/**
*@brief		ͨ��ģ���������
*@param     ��
*@note      ��
*@return	true: ���Գɹ�
*			false: ����ʧ�ܣ���δ�������
*/

bool communication_req()
{
	uint8_t retry_cnt =0;
	
	if (lsUsedLora)
	{
		debug_printf("detect\r\n");
		loraSetMode(&lora_ctrl, LORA_SLAVER);
		lora_passivity_communication(&lora_ctrl,&oriutg_data);
		SX1278_sleep(lora_ctrl.pSX1278);
		return true;
	}
	else
	{
		debug_printf("communication_req\r\n");
		net_send_udp_tcp((char*)debug_req_buf, send_size);
		while(retry_cnt < 1)
		{
			sim7020_handle();
			retry_cnt++;
			if (communication_ls_debug())break;
		}
		if (!communication_ls_debug())
		{
			return false;
		}
		else
		{
			do
			{
				sim7020_handle();
				if (LONG_SLEEP_READY == sleep_step)
				{
					net_retra_handle();					
				}
				printf("network wait recv form server...\r\n");
				if (sim7020_get_idel() > 40)//400��				
				{
					net_set_close_sta(true);
					return true;
				}
			}
			while(communication_ls_debug());
			return true;
		}
	}
	
}


/**
*@brief		����Ƿ�ʹ��Lora�豸
*@param     ��
*@note      ��
*@return	true: ʹ��Lora�豸
*			false: δʹ��Lora�豸������NB�豸
*/

bool communication_ls_lora()
{
	return lsUsedLora;
}




/**
*@brief		����ͨ�ſ��Ʋ�����������ID����������������ʱ�䣬����ʱ��
*@param     ��
*@note      ��
*@return	true: ȫ�����سɹ�
*			false: ���ش���ʧ��
*/

bool communication_load_param()
{
	bool suc = true;
	if(!lsUsedLora)
	{
		debug_printf("communication_load_param\r\n");
		if(!ReLoadSersorId())suc = false;
		if(!ReLoad_Ip_Port())suc = false;
		//��һ���ϵ���ص�ص���
		if(*(__IO uint32_t *)BATTERY_CAPACITY_FLAG != 0xaaaaaaaa){
			WriteFlashBatteryCapacity(BATTERY_FULL_CAPACITY);
		}
	}else{
		//��һ���ϵ���ص�ص���
		if(*(__IO uint32_t *)BATTERY_CAPACITY_FLAG != 0xaaaaaaaa){
			WriteFlashBatteryCapacity(LORA_COLLECT_DATA_MAX_COUNTER);
		}
		reload_lora_parameter();
	}
	return suc;
}



