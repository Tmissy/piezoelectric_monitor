#include "timer_config.h"
#include "math.h"
#include "stdbool.h"
#include "gpio_config.h"

#define PI (3.1415926)

extern bool atReceiveFinish;
extern bool uartReceiveFlag;

static uint16_t mSecondCnt = 0;
static uint16_t waitmSecond = 10;

void timer_plus_output_gpio_config(){
	//gpio ʱ������
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	//PWM1��������
	gpio_out_config(POWER_PWM1_PORT,POWER_PWM1_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	gpio_af_set(POWER_PWM1_PORT, GPIO_AF_1, POWER_PWM1_PIN);
	//PWM2��������
	gpio_out_config(POWER_PWM2_PORT,POWER_PWM2_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	gpio_af_set(POWER_PWM2_PORT, GPIO_AF_2, POWER_PWM2_PIN);
	
}


//��ʱ��3Ϊ����ʱ��������50hz���ڼ���
//��ʱ��0Ϊ��ʱ��3�Ӷ�ʱ������ʱ��3���������¼�����ʱ��0ͬʱ���������õ�����ģʽ���ظ�����60�Σ��رն�ʱ��3
//��ʱ��1Ϊ��ʱ��3�ĴӶ�ʱ������ʱ3ms,�رն�ʱ��3������DMAת��
//��ʱ��2Ϊ��ʱ��1�ĴӶ�ʱ������ʱ��1��������ʱ�俪����ʱ��2�����õ�����ģʽ���ظ�����0
//��ʱ��4Ϊ��ʱ��1�ĴӶ�ʱ������ʱ��1���������¼�������ʱ��4������1ms��ʱ�������ۼӣ��رն�ʱ��

/****************************************�ɼ�����***************************************************
**1. ������ʱ��3��ʱ��******************************************************************************
**2. ��ʱ��3���������¼���ʱ��0����������PWM1����***************************************************
**3. ��ʱ��3���������¼���ͬʱ������ʱ��1����ʱʱ��3ms**********************************************
**4. ��ʱ��1���������¼�****************************************************************************
					a.������ʱ��2��ʹ�ܶ�ʱ��2�жϣ����䵥���壬�����жϺ�رն�ʱ��2*************************
					b. ����DCIʹ�ܣ�����AD�ɼ�ʱ�ӣ�ʹ��DMA **************************************************
**5. ��ʱ��1���������¼��ģ�������ʱ��4����ʱʱ��1ms,ʹ�ܶ�ʱ��4�ж�********************************
**6. ��ʱ��4���������¼�****************************************************************************
					a. �ر�DCI���ر�DMA����������DMA**********************************************************
					b. ��ʱ�������¼������󣬹رն�ʱ��1******************************************************
**7. �ɼ���ɣ����붨ʱ��3�жϣ��ر����еĶ�ʱ�����ر�DCI���ر�DMA**********************************
***************************************************************************************************/


/*!
    \brief     ��ʱ����Ϊ����ʱ����
    \param[in]  none
    \param[out] none
    \retval     none
*/
  /* ----------------------------------------------------------------------------
    TIMER3 Configuration: 
    TIMER2CLK = SystemCoreClock/1999 = 100KHz, the period is 1s(10000/2000 = 50).
		�ظ�Ƶ��50hz 
    ---------------------------------------------------------------------------- */
void timer3_master_config(void)
{
    timer_parameter_struct timer_initpara;
	
    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER3);
	  rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);//���ö�ʱ��ʱ�ӣ����൱���Ǳ�Ƶ4��50M*4=200M

    /* deinit a TIMER */
    timer_deinit(TIMER3);
    /* TIMER3 configuration */
    timer_initpara.prescaler         = 1999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 1999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER3, &timer_initpara);

    /* enable the TIMER interrupt */
		timer_interrupt_flag_clear(TIMER3, TIMER_INT_UP);
    timer_interrupt_enable(TIMER3, TIMER_INT_UP);
		
		/*���ö�ʱ��0�Ͷ�ʱ��3��������ʱ��0Ϊ��ģʽ��ʹ�ܶ�ʱ��0������ʱ��3*/
		 /* select the master slave mode */
		 
    timer_master_slave_mode_config(TIMER3, TIMER_MASTER_SLAVE_MODE_ENABLE);
		//����ʹ���ź���Ϊ���������
		timer_master_output_trigger_source_select(TIMER3, TIMER_TRI_OUT_SRC_UPDATE);
 /* enable a TIMER */  
		timer_enable(TIMER3);
}


/*!
    \brief     ��ʱ��0Ϊ��ʱ�����ĴӶ�ʱ����������������ظ�����60
							 TIMER0 Configuration: 
							 TIMER2CLK = SystemCoreClock/2 = 100MHZ,һ��ʱ������10ns,
							 60KhzƵ�������16660ns ,Prescaler cnt ����Ϊ1666
    -----------------
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer0_pwm1_output_config(void)
{
	    /* enable the peripherals clock */
		rcu_periph_clock_enable(RCU_TIMER0);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);//���ö�ʱ��ʱ�ӣ����൱���Ǳ�Ƶ4��50M*4=200M
		timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    /* deinit a TIMER */
    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 1665;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 50;
    timer_init(TIMER0,&timer_initpara);

     /* CH1,CH2 and CH3 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_DISABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER0,TIMER_CH_0,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_0,833);
    timer_channel_output_mode_config(TIMER0,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER0,ENABLE);
		timer_single_pulse_mode_config(TIMER0,TIMER_SP_MODE_SINGLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_disable(TIMER0);
		
		/*���ö�ʱ��2��Ϊ��ʱ��0�Ĵ�ģʽ����ʱ��0��������ʱ��2Ҳ����*/
				 /* select the master slave mode */
    timer_master_slave_mode_config(TIMER0, TIMER_MASTER_SLAVE_MODE_ENABLE);
				//���ö�ʱ���Ĵ���Դ���Զ�ʱ��0
		timer_input_trigger_source_select(TIMER0,TIMER_SMCFG_TRGSEL_ITI3);
				 /* slave mode selection: TIMER0  �¼�ģʽ*/
    timer_slave_mode_select(TIMER0,TIMER_SLAVE_MODE_EVENT);

}



/*!
    \brief     ��ʱ��1Ϊ��ʱ��3�ĴӶ�ʱ��������3ms���������¼�
							 TIMER0 Configuration: 
							 TIMER2CLK = SystemCoreClock/24000 = 10KHz, the period is 1s(10000/2000 = 50).
							 �ظ�Ƶ��50hz 
    -----------------
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer1_master3_slave1_config(void)
{
  
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;
	
    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER1);
	  rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);//���ö�ʱ��ʱ�ӣ����൱���Ǳ�Ƶ4��50M*4=200M

    /* deinit a TIMER */
    timer_deinit(TIMER1);
    /* TIMER3 configuration */
    timer_initpara.prescaler         = 199;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 2999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER1, &timer_initpara);

    /* enable the TIMER interrupt */
		timer_interrupt_flag_clear(TIMER1, TIMER_INT_UP);
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
		
		    /* auto-reload preload enable */
    timer_auto_reload_shadow_disable(TIMER1);
		
				/*���ö�ʱ��2��Ϊ��ʱ��0�Ĵ�ģʽ����ʱ��0��������ʱ��2Ҳ����*/
				 /* select the master slave mode */
    timer_master_slave_mode_config(TIMER1, TIMER_MASTER_SLAVE_MODE_ENABLE);
						//���ö�ʱ���Ĵ���Դ���Զ�ʱ��0
		timer_input_trigger_source_select(TIMER1,TIMER_SMCFG_TRGSEL_ITI3);
						//����ʹ���ź���Ϊ���������
		timer_master_output_trigger_source_select(TIMER1, TIMER_TRI_OUT_SRC_UPDATE);
				 /* slave mode selection: TIMER0  �¼�ģʽ*/
    timer_slave_mode_select(TIMER1,TIMER_SLAVE_MODE_EVENT);
		
//    /* channel DMA request source selection */
//    timer_channel_dma_request_source_select(TIMER1,TIMER_DMAREQUEST_UPDATEEVENT);
//    /* TIMER0 update DMA request enable */
//    timer_dma_enable(TIMER1,TIMER_DMA_UPD);

}


/*!
    \brief     ��ʱ��0Ϊ��ʱ�����ĴӶ�ʱ����������������ظ�����60
							 TIMER0 Configuration: 
							 TIMER2CLK = SystemCoreClock/24000 = 10KHz, the period is 1s(10000/2000 = 50).
							 �ظ�Ƶ��50hz 
    -----------------
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer2_slave1_config(void)
{
	
			    /* enable the peripherals clock */
		rcu_periph_clock_enable(RCU_TIMER2);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL2);//���ö�ʱ��ʱ�ӣ����൱���Ǳ�Ƶ4��50M*4=200M
		timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    /* deinit a TIMER */
    timer_deinit(TIMER2);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 0;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 499;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2,&timer_initpara);

     /* CH1,CH2 and CH3 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_LOW;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_HIGH;
    timer_ocintpara.ocnidlestate = TIMER_OC_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER2,TIMER_CH_2,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_2,99);
    timer_channel_output_mode_config(TIMER2,TIMER_CH_2,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER2,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER2,ENABLE);
		timer_single_pulse_mode_config(TIMER2,TIMER_SP_MODE_SINGLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_disable(TIMER2);
		
		/*���ö�ʱ��2��Ϊ��ʱ��0�Ĵ�ģʽ����ʱ��0��������ʱ��2Ҳ����*/
				 /* select the master slave mode */
    timer_master_slave_mode_config(TIMER2, TIMER_MASTER_SLAVE_MODE_ENABLE);
				//���ö�ʱ���Ĵ���Դ���Զ�ʱ��0
		timer_input_trigger_source_select(TIMER2,TIMER_SMCFG_TRGSEL_ITI1);
				 /* slave mode selection: TIMER0  �¼�ģʽ*/
    timer_slave_mode_select(TIMER2,TIMER_SLAVE_MODE_EVENT);

}
void timer7_slave1_config(void)
{
				    /* enable the peripherals clock */
		rcu_periph_clock_enable(RCU_TIMER7);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);//���ö�ʱ��ʱ�ӣ����൱���Ǳ�Ƶ4��50M*4=200M
		timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    /* deinit a TIMER */
    timer_deinit(TIMER7);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 199;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER7,&timer_initpara);

	    /* enable the TIMER interrupt */
		timer_interrupt_flag_clear(TIMER7, TIMER_INT_UP);
    timer_interrupt_enable(TIMER7, TIMER_INT_UP);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_disable(TIMER7);
		
		/*���ö�ʱ��2��Ϊ��ʱ��0�Ĵ�ģʽ����ʱ��0��������ʱ��2Ҳ����*/
				 /* select the master slave mode */
    timer_master_slave_mode_config(TIMER7, TIMER_MASTER_SLAVE_MODE_ENABLE);
				//���ö�ʱ���Ĵ���Դ���Զ�ʱ��0
		timer_input_trigger_source_select(TIMER7,TIMER_SMCFG_TRGSEL_ITI1);
				 /* slave mode selection: TIMER0  �¼�ģʽ*/
    timer_slave_mode_select(TIMER7,TIMER_SLAVE_MODE_EVENT);
	
	
	
//    /* ----------------------------------------------------------------------------
//    TIMER2 Configuration: 
//    TIMER2CLK = SystemCoreClock/18000 = 10KHz, the period is 1s(10/10000 = 1ms).
//    ---------------------------------------------------------------------------- */
//	
//		/* enable the peripherals clock */
//		rcu_periph_clock_enable(RCU_TIMER4);
//    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);//���ö�ʱ��ʱ�ӣ����൱���Ǳ�Ƶ4��50M*4=200M
//	
//    timer_oc_parameter_struct timer_ocinitpara;
//    timer_parameter_struct timer_initpara;

//    /* deinit a TIMER */
//    timer_deinit(TIMER4);

//    /* TIMER2 configuration */
//    timer_initpara.prescaler         = 199;
//    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
//    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
//    timer_initpara.period            = 999;
//    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
//    timer_init(TIMER4, &timer_initpara);

//    /* enable the TIMER interrupt */
//		timer_interrupt_flag_clear(TIMER4, TIMER_INT_UP);
//    timer_interrupt_enable(TIMER4, TIMER_INT_UP);
//		
//		 /* auto-reload preload enable */
//    timer_auto_reload_shadow_enable(TIMER4);

//				/*���ö�ʱ��2��Ϊ��ʱ��0�Ĵ�ģʽ����ʱ��0��������ʱ��2Ҳ����*/
//				 /* select the master slave mode */
//    timer_master_slave_mode_config(TIMER4, TIMER_MASTER_SLAVE_MODE_ENABLE);
//				//���ö�ʱ���Ĵ���Դ���Զ�ʱ��0
//		timer_input_trigger_source_select(TIMER4,TIMER_SMCFG_TRGSEL_ITI1);
//				 /* slave mode selection: TIMER0  �¼�ģʽ*/
//    timer_slave_mode_select(TIMER4,TIMER_SLAVE_MODE_EVENT);
}

void enable_tiemr3_50hz(){
    /* enable a TIMER */  
		timer_enable(TIMER3);
}

void timer_config(){
	timer0_pwm1_output_config();
	timer1_master3_slave1_config();
	timer2_slave1_config();
	timer3_master_config();
	timer7_slave1_config();
}




void TIM_Reset_Timeout(void)
{
	mSecondCnt = 0;
}


