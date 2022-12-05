#ifndef RTC_CONFIG_H
#define RTC_CONFIG_H

#include "gd32f4xx.h"
#include "stdio.h"
#include "stdbool.h"

typedef struct
{
	uint32_t flag;
	uint32_t wakeup_count;
	uint32_t hour;
	uint32_t min;
	uint32_t sec;
}sleep_time_t;

extern sleep_time_t sample_time;
extern sleep_time_t sleep_time;
extern bool  sample_wakeup;

bool RTC_Set_Sleep_Time(uint16_t sec);
void rtc_configuration(uint16_t sleep_Time_Uint_S);
void pre_standby();

#endif /* GD32E50X_EXMC_H */

