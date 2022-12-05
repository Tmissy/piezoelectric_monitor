/********************************************************************************
* @file    		at_uart_hal.c
* @function     NB模块串口收发
* @attention		
* @version    	V1.0
* @Author       
* @date       	2021/07/21
********************************************************************************/


#include <stdbool.h>
#include "usart_config.h"
#include "at_uart_hal.h"
#include "string.h"
#include "systick.h"


#define AT_UART UART3
#define AT_UART_SEND_BUF_SIZE  300
#define AT_UART_RECV_BUF_SIZE  300 

extern bool ls_start_udp_tcp;

bool atReceiveFinish;


//4字节对齐 __attribute__ ((aligned (4))) 

static bool is_inited=false;
static char at_recv_buf[AT_UART_RECV_BUF_SIZE];
static uint32_t at_recv_len = 0;

/**
*@brief		AT串口清除初始化
*@param     无   
*@note      无
*@return	true: 清除初始化成功
*			false: 清除初始化失败
*/
	
bool at_uart_deinit(void)
{
    printf("at uart deinit");

    is_inited=false;
		usart_interrupt_disable(AT_UART, USART_INTEN_RBNEIE); // 使能“读数据缓冲区非空中断和过载错误中断”
//		usart_interrupt_disable(USART2, USART_INT_IDLE); // 使能“IDLE线检测中断”
		usart_interrupt_disable(AT_UART, USART_INTEN_TBEIE);  // 使能“发送缓冲区空中断”	
//		Uart_recv_buf_unregster();
     return true;
}

/**
*@brief		AT收发初始化
*@param     无   
*@note      无
*@return	true: 初始化成功
*			false: 初始化失败
*/
bool at_uart_init(void)
{
  if(is_inited)return true;
  printf("at uart init!\r\n");
	is_inited = Uart_recv_buf_regster((uint8_t*)at_recv_buf, &at_recv_len);
	if (is_inited )
		printf("at uart init ok!\r\n"); 
	else
		debug_printf("at uart init failed!\r\n");
	return is_inited;
   
}

/**
*@brief		AT串口发送
*@param     pdata：发送的源数据
*           size：发送的数据长度，单位字节
*           timeout：超时时间
*@note      无
*@return	发送的长度，单位字节
*			
*/
uint32_t at_uart_send(const void *pdata, uint32_t size, uint32_t timeout)
{
	at_recv_len = 0;
	usart_datas_transmit(AT_UART,(uint8_t *)pdata, size, 0xffff);
  return size;
}

/**
*@brief		AT串口接收
*@param     pdata：接收的缓存区，需要用户提前好开辟空间
*           expect_size：预计接收的数据长度，单位字节
*           timeout：超时时间
*@note      无
*@return	实际接收的长度，单位字节
*			
*/
uint32_t at_uart_receive(void *pdata, uint32_t expect_size, uint32_t timeout)
{
    uint32_t read_len = 0;
		uint16_t timeout_cnt = 0;
//	UNUSED(expect_size);
    if(is_inited==false)return 0;
	do
	{	
		if(atReceiveFinish)
		{
			Uart_recv_reset();
			debug_printf("at_recv_len = %d \r\n",at_recv_len);
			
			if (at_recv_len > 2 && 
				(!memcmp((void*)(at_recv_buf + at_recv_len - 2), (void*)("\r\n"), 2) || !memcmp(at_recv_buf+at_recv_len-2, "OK", 2)))
			{
				at_recv_buf[at_recv_len] = '\0';
				strcpy((char*)pdata, at_recv_buf);
				debug_printf("R(%d):%s",at_recv_len,at_recv_buf);
				at_recv_buf[at_recv_len - 2] = '\0';
			}
			else if(ls_start_udp_tcp)
			{
				strcpy(pdata, at_recv_buf);					
			}
			atReceiveFinish = 0;
			read_len = at_recv_len;
			break;
		}
		else
		{
			delay_1ms(5);
			timeout_cnt += 5;
		}
	}
	while(timeout_cnt < timeout);
		
	return read_len;
}

/**
*@brief		获取最近一次AT串口数据
*@param     pdata：接收的缓存区，需要用户提前好开辟空间
*@note      无
*@return	实际接收的长度，单位字节
*			
*/
uint16_t at_get_last_recv(uint8_t** pdata)
{
	strcpy((char*)*pdata, at_recv_buf);
	return at_recv_len;
}
