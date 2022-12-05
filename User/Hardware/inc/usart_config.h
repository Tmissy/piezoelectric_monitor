#ifndef USART_CONFIG_H
#define USART_CONFIG_H

#include "gd32f4xx.h"
#include <stdio.h>
#include "stdbool.h"

typedef enum
{
  STATUS_OK       = 0x00U,
  STATUS_ERROR    = 0x01U,
	STATUS_TIMEOUT  = 0x02U
} Status_TypeDef;

	


#define DEBUG_USART   USART1
#define NB_USART  	  USART2



#define	_DEBUG_PRINT        1   // 0: disable      1:debug

#if(_DEBUG_PRINT == 1)
	#define debug_printf(fmt,args ...) do {printf(fmt, ##args);} while(0)
#else
	#define debug_printf(fmt,args ...)
#endif

	

void gd_debug_com_init(void);
void	nb_usart_init(void);
	
Status_TypeDef usart_datas_transmit(uint32_t usart_periph,uint8_t*data,uint16_t size,uint32_t timeout);
Status_TypeDef usart_datas_receive(uint32_t usart_periph,uint8_t*data,uint16_t size,uint32_t timeout);

void Uart_recv_reset(void);
bool Uart_recv_buf_regster(uint8_t* out_buf, uint32_t* line_size);
void Uart_recv_buf_unregster(void);
void NB_UART_RxCpltCallback(void);	
	

#endif 

