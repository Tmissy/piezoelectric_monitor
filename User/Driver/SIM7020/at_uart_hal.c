/********************************************************************************
* @file    		at_uart_hal.c
* @function     NBģ�鴮���շ�
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


//4�ֽڶ��� __attribute__ ((aligned (4))) 

static bool is_inited=false;
static char at_recv_buf[AT_UART_RECV_BUF_SIZE];
static uint32_t at_recv_len = 0;

/**
*@brief		AT���������ʼ��
*@param     ��   
*@note      ��
*@return	true: �����ʼ���ɹ�
*			false: �����ʼ��ʧ��
*/
	
bool at_uart_deinit(void)
{
    printf("at uart deinit");

    is_inited=false;
		usart_interrupt_disable(AT_UART, USART_INTEN_RBNEIE); // ʹ�ܡ������ݻ������ǿ��жϺ͹��ش����жϡ�
//		usart_interrupt_disable(USART2, USART_INT_IDLE); // ʹ�ܡ�IDLE�߼���жϡ�
		usart_interrupt_disable(AT_UART, USART_INTEN_TBEIE);  // ʹ�ܡ����ͻ��������жϡ�	
//		Uart_recv_buf_unregster();
     return true;
}

/**
*@brief		AT�շ���ʼ��
*@param     ��   
*@note      ��
*@return	true: ��ʼ���ɹ�
*			false: ��ʼ��ʧ��
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
*@brief		AT���ڷ���
*@param     pdata�����͵�Դ����
*           size�����͵����ݳ��ȣ���λ�ֽ�
*           timeout����ʱʱ��
*@note      ��
*@return	���͵ĳ��ȣ���λ�ֽ�
*			
*/
uint32_t at_uart_send(const void *pdata, uint32_t size, uint32_t timeout)
{
	at_recv_len = 0;
	usart_datas_transmit(AT_UART,(uint8_t *)pdata, size, 0xffff);
  return size;
}

/**
*@brief		AT���ڽ���
*@param     pdata�����յĻ���������Ҫ�û���ǰ�ÿ��ٿռ�
*           expect_size��Ԥ�ƽ��յ����ݳ��ȣ���λ�ֽ�
*           timeout����ʱʱ��
*@note      ��
*@return	ʵ�ʽ��յĳ��ȣ���λ�ֽ�
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
*@brief		��ȡ���һ��AT��������
*@param     pdata�����յĻ���������Ҫ�û���ǰ�ÿ��ٿռ�
*@note      ��
*@return	ʵ�ʽ��յĳ��ȣ���λ�ֽ�
*			
*/
uint16_t at_get_last_recv(uint8_t** pdata)
{
	strcpy((char*)*pdata, at_recv_buf);
	return at_recv_len;
}
