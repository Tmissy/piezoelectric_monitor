#ifndef __AT_UART_HAL_H
#define __AT_UART_HAL_H

#include <stdbool.h>
#include "gd32f4xx.h"

bool at_uart_init(void);
bool at_uart_deinit(void);
uint32_t at_uart_send(const void *pdata, uint32_t size, uint32_t timeout);
uint32_t at_uart_receive(void *pdata, uint32_t expect_size,uint32_t timeout);
void at_uart_irq_handler(void);
uint16_t at_get_last_recv(uint8_t** pdata);



#endif

