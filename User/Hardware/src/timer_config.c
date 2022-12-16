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
	//gpio 时钟配置
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	//PWM1引脚配置
	gpio_out_config(POWER_PWM1_PORT,POWER_PWM1_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	gpio_af_set(POWER_PWM1_PORT, GPIO_AF_1, POWER_PWM1_PIN);
	//PWM2引脚配置
	gpio_out_config(POWER_PWM2_PORT,POWER_PWM2_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	gpio_af_set(POWER_PWM2_PORT, GPIO_AF_2, POWER_PWM2_PIN);
	
}


//定时器3为主定时器，进行50hz周期计数
//定时器0为定时器3从定时器，定时器3产生更新事件，定时器0同时启动，配置单脉冲模式，重复计数60次，关闭定时器3
//定时器1为定时器3的从定时器，定时3ms,关闭定时器3，触发DMA转换
//定时器2为定时器1的从定时器，定时器1产生更新时间开启定时器2，配置单脉冲模式，重复计数0
//定时器4为定时器1的从定时器，定时器1产生更新事件开启定时器4，配置1ms定时，数据累加，关闭定时器

/****************************************采集流程***************************************************
**1. 开启定时器3定时；******************************************************************************
**2. 定时器3产生更新事件定时器0启动，发射PWM1波形***************************************************
**3. 定时器3产生更新事件的同时开启定时器1，定时时间3ms**********************************************
**4. 定时器1产生更新事件****************************************************************************
					a.触发定时器2，使能定时器2中断，发射单脉冲，进入中断后关闭定时器2*************************
					b. 开启DCI使能，开启AD采集时钟，使能DMA **************************************************
**5. 定时器1产生更新事件的，触发定时器4，定时时间1ms,使能定时器4中断********************************
**6. 定时器4产生更新事件****************************************************************************
					a. 关闭DCI，关闭DMA，重新配置DMA**********************************************************
					b. 定时器更新事件产生后，关闭定时器1******************************************************
**7. 采集完成，进入定时器3中断，关闭所有的定时器，关闭DCI，关闭DMA**********************************
***************************************************************************************************/


/*!
    \brief     定时器三为主定时器；
    \param[in]  none
    \param[out] none
    \retval     none
*/
  /* ----------------------------------------------------------------------------
    TIMER3 Configuration: 
    TIMER2CLK = SystemCoreClock/1999 = 100KHz, the period is 1s(10000/2000 = 50).
		重复频率50hz 
    ---------------------------------------------------------------------------- */
void timer3_master_config(void)
{
    timer_parameter_struct timer_initpara;
	
    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER3);
	  rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);//配置定时器时钟，就相当于是倍频4，50M*4=200M

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
		
		/*配置定时器0和定时器3级联，定时器0为主模式，使能定时器0启动定时器3*/
		 /* select the master slave mode */
		 
    timer_master_slave_mode_config(TIMER3, TIMER_MASTER_SLAVE_MODE_ENABLE);
		//发送使能信号作为触发的输出
		timer_master_output_trigger_source_select(TIMER3, TIMER_TRI_OUT_SRC_UPDATE);
 /* enable a TIMER */  
		timer_enable(TIMER3);
}


/*!
    \brief     定时器0为定时器三的从定时器；单脉冲输出，重复计数60
							 TIMER0 Configuration: 
							 TIMER2CLK = SystemCoreClock/2 = 100MHZ,一个时钟脉宽10ns,
							 60Khz频率脉宽≈16660ns ,Prescaler cnt 设置为1666
    -----------------
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer0_pwm1_output_config(void)
{
	    /* enable the peripherals clock */
		rcu_periph_clock_enable(RCU_TIMER0);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);//配置定时器时钟，就相当于是倍频4，50M*4=200M
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
		
		/*配置定时器2作为定时器0的从模式，定时器0启动，定时器2也启动*/
				 /* select the master slave mode */
    timer_master_slave_mode_config(TIMER0, TIMER_MASTER_SLAVE_MODE_ENABLE);
				//配置定时器的触发源来自定时器0
		timer_input_trigger_source_select(TIMER0,TIMER_SMCFG_TRGSEL_ITI3);
				 /* slave mode selection: TIMER0  事件模式*/
    timer_slave_mode_select(TIMER0,TIMER_SLAVE_MODE_EVENT);

}



/*!
    \brief     定时器1为定时器3的从定时器，配置3ms产生更新事件
							 TIMER0 Configuration: 
							 TIMER2CLK = SystemCoreClock/24000 = 10KHz, the period is 1s(10000/2000 = 50).
							 重复频率50hz 
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
	  rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);//配置定时器时钟，就相当于是倍频4，50M*4=200M

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
		
				/*配置定时器2作为定时器0的从模式，定时器0启动，定时器2也启动*/
				 /* select the master slave mode */
    timer_master_slave_mode_config(TIMER1, TIMER_MASTER_SLAVE_MODE_ENABLE);
						//配置定时器的触发源来自定时器0
		timer_input_trigger_source_select(TIMER1,TIMER_SMCFG_TRGSEL_ITI3);
						//发送使能信号作为触发的输出
		timer_master_output_trigger_source_select(TIMER1, TIMER_TRI_OUT_SRC_UPDATE);
				 /* slave mode selection: TIMER0  事件模式*/
    timer_slave_mode_select(TIMER1,TIMER_SLAVE_MODE_EVENT);
		
//    /* channel DMA request source selection */
//    timer_channel_dma_request_source_select(TIMER1,TIMER_DMAREQUEST_UPDATEEVENT);
//    /* TIMER0 update DMA request enable */
//    timer_dma_enable(TIMER1,TIMER_DMA_UPD);

}


/*!
    \brief     定时器0为定时器三的从定时器；单脉冲输出，重复计数60
							 TIMER0 Configuration: 
							 TIMER2CLK = SystemCoreClock/24000 = 10KHz, the period is 1s(10000/2000 = 50).
							 重复频率50hz 
    -----------------
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer2_slave1_config(void)
{
	
			    /* enable the peripherals clock */
		rcu_periph_clock_enable(RCU_TIMER2);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL2);//配置定时器时钟，就相当于是倍频4，50M*4=200M
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
		
		/*配置定时器2作为定时器0的从模式，定时器0启动，定时器2也启动*/
				 /* select the master slave mode */
    timer_master_slave_mode_config(TIMER2, TIMER_MASTER_SLAVE_MODE_ENABLE);
				//配置定时器的触发源来自定时器0
		timer_input_trigger_source_select(TIMER2,TIMER_SMCFG_TRGSEL_ITI1);
				 /* slave mode selection: TIMER0  事件模式*/
    timer_slave_mode_select(TIMER2,TIMER_SLAVE_MODE_EVENT);

}
void timer7_slave1_config(void)
{
				    /* enable the peripherals clock */
		rcu_periph_clock_enable(RCU_TIMER7);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);//配置定时器时钟，就相当于是倍频4，50M*4=200M
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
		
		/*配置定时器2作为定时器0的从模式，定时器0启动，定时器2也启动*/
				 /* select the master slave mode */
    timer_master_slave_mode_config(TIMER7, TIMER_MASTER_SLAVE_MODE_ENABLE);
				//配置定时器的触发源来自定时器0
		timer_input_trigger_source_select(TIMER7,TIMER_SMCFG_TRGSEL_ITI1);
				 /* slave mode selection: TIMER0  事件模式*/
    timer_slave_mode_select(TIMER7,TIMER_SLAVE_MODE_EVENT);
	
	
	
//    /* ----------------------------------------------------------------------------
//    TIMER2 Configuration: 
//    TIMER2CLK = SystemCoreClock/18000 = 10KHz, the period is 1s(10/10000 = 1ms).
//    ---------------------------------------------------------------------------- */
//	
//		/* enable the peripherals clock */
//		rcu_periph_clock_enable(RCU_TIMER4);
//    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);//配置定时器时钟，就相当于是倍频4，50M*4=200M
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

//				/*配置定时器2作为定时器0的从模式，定时器0启动，定时器2也启动*/
//				 /* select the master slave mode */
//    timer_master_slave_mode_config(TIMER4, TIMER_MASTER_SLAVE_MODE_ENABLE);
//				//配置定时器的触发源来自定时器0
//		timer_input_trigger_source_select(TIMER4,TIMER_SMCFG_TRGSEL_ITI1);
//				 /* slave mode selection: TIMER0  事件模式*/
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


