#include "rtc_config.h"
#include "fmc_flash.h"
#include "usart_config.h"

sleep_time_t sample_time = {0x0,0,12,0,0};
sleep_time_t sleep_time = {0xaaaa,1,0,30,0};
bool  sample_wakeup = true ;
static uint32_t standy = 0;

/*!
    \brief      configure the RTC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_configuration(uint16_t sleep_Time_Uint_S)
{
//  		uint32_t param = readFlashWord(SLEEP_PARAM_BASE);
//			memcpy((uint8_t*)&sample_time, (uint8_t* )(&param), 3);  
	
		/* enable BKPI clock */
//    rcu_periph_clock_enable(RCU_BKPI);
	
    /* enable access to RTC registers in Backup domain */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();
	
	
    /* allow access to backup domain */
    pmu_backup_write_enable();
    /* reset backup domain */
		rcu_bkp_reset_disable();

		/* enable LXTAL */
		rcu_osci_on(RCU_LXTAL);
		/* wait till LXTAL is ready */
    rcu_osci_stab_wait(RCU_LXTAL);
		/* select LXTAL as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
	
    /* enable RTC clock */
    rcu_periph_clock_enable(RCU_RTC);
		
		/* wait for RTC registers synchronization */
		rtc_register_sync_wait();
		
    /* wait until last write operation on RTC registers has finished */

    /* enable the RTC alarm interrupt*/
    rtc_interrupt_enable(RTC_INT_ALARM0);
    /* wait until last write operation on RTC registers has finished */

    /* set RTC prescaler: set RTC period to 1s */
//		rtc_init(&rtc_initpara)
//    rtc_prescaler_set(40000);
    /* wait until last write operation on RTC registers has finished */
//    rtc_lwoff_wait();
//    rtc_counter_set(0U);
		rtc_wakeup_timer_set(0x05);
    /* wait until last write operation on RTC registers has finished */

//    rtc_alarm_config(sleep_Time_Uint_S);
    /* wait until last write operation on RTC registers has finished */

    /* clear the RTC alarm flag */
    rtc_flag_clear(RTC_STAT_ALRM0F);
		
		
		    /* clear wakeup timer occurs flag */
//    rtc_flag_clear(RTC_FLAG_WT);
    /* RTC wakeup configuration */
    rtc_interrupt_enable(RTC_INT_WAKEUP);
    /* set wakeup clock as ck_spre(default 1Hz) */
    rtc_wakeup_clock_set(WAKEUP_CKSPRE_2EXP16);
    /* every 5+1 ck_spre period set wakeup flag */
    rtc_wakeup_timer_set(0x05);
    rtc_wakeup_enable();
}
	
	/**
*@brief		设置采样信号标志为有效或者无效并清除采样定时
*@param     ls_done：true 有效，false 无效
*@note      无
*@return	无
*/
void pre_standby(){
	standy = 1;

	writeFlash_X_Word(FWDGT_RESET_FLAG,sizeof(sleep_time),&standy);
	debug_printf("standbyFlag = %x\r\n",*(__IO uint32_t*)FWDGT_RESET_FLAG);
//	flashAddrreadFlashWord(FWDGT_RESET_FLAG)
//	nvic_system_reset();
}



