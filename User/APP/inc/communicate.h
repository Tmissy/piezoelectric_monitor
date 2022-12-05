#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include "gd32f4xx.h"
#include  "stdbool.h"

bool communication_init(void);
bool communication_send(uint8_t* pdata, uint32_t dataSize);
bool communication_ls_con(void);
bool communication_sleep(void);
bool communication_reboot(void);
bool communication_wakeup(void);
void communication_test(void);
bool communication_ls_debug(void);
bool net_wait_con(uint16_t start_sec, uint16_t timeout);
bool communication_req(void);
bool communication_ls_lora(void);
bool communication_load_param(void);
void set_Communication_RTC_Time(void);
void wake_Up_Count(void);
//void dac_init();

#endif /* GD32E50X_EXMC_H */




