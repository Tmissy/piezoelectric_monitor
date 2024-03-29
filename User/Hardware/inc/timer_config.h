#ifndef TIMER_CONFIG_H
#define TIMER_CONFIG_H

#include "gd32f4xx.h"
#include "stdbool.h"

void timer_plus_output_gpio_config(void);

void timer3_master_config(void);
void timer0_pwm1_output_config(void);
void timer1_master3_slave1_config(void);
void timer2_slave1_config(void);
void timer7_slave1_config(void);

void enable_tiemr3_50hz();

void timer_config(void);
void TIM_Reset_Timeout(void);
#endif 

