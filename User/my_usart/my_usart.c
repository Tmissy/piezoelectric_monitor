#include "my_usart.h"

#include "port.h"

/*
	配置Usart1
*/
void USART1_Config(void)
{
//	//1、打开串口1所在的GPIO时钟
//	rcu_periph_clock_enable(GPIOA);	
	
	//2、打开串口1时钟
	rcu_periph_clock_enable(RCU_USART1);
	
	//3、Usart1配置（PA2———T，PA3———R）
	gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_2);
	GPIO_Out_config(GPIOA,GPIO_PIN_2,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	
	gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_3);
	GPIO_Out_config(GPIOA,GPIO_PIN_3,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	
	//3、串口复位设置
		usart_deinit(USART1);

	usart_baudrate_set(USART1,115200);						//设置波特率		115200
	usart_word_length_set(USART1,USART_WL_8BIT);			//设置传输字长		8
	usart_stop_bit_set(USART1,USART_STB_1BIT);				//设置停止位		1
	usart_parity_config(USART1,USART_PM_NONE);				//设置校验位		None
	usart_transmit_config(USART1,USART_TRANSMIT_ENABLE);	//设置发送使能
	
}


/*
	初始化Usart1
*/
void Usart1_Init(void)
{
	usart_enable(USART1);
}

/*
	发送u8单字节数据	
*/
void usart_send_byte(uint8_t ch)
{
  /* 发送一个字节数据到USART1 */
  usart_data_transmit(USART1,ch);

  /* 等待发送完毕 */
  while (usart_flag_get(USART1, USART_FLAG_TBE) == RESET);  
}


/**
  * @brief  重定向c库函数printf到USART1
  * @param  None
  * @retval 
  */
//int fputc(int ch, FILE *f)
//{
//  /*清除标志位*/
//  usart_flag_clear(USART1,USART_FLAG_TC);
//  
//  /* 发送一个字节数据到USART1 */
//  usart_data_transmit(USART1, (uint8_t) ch);
//  
//  /* 等待发送完毕 */
//  while (usart_flag_get(USART1, USART_FLAG_TC) == RESET);   
//  
//  return (ch);
//}

/*
	必须要有int fputc(int ch, FILE *f)函数复写
*/
void Usart_Send_String(unsigned char *string)
{
   while(*string)
   {
	     usart_send_byte(*string);//此函数表示串口发送单个字符的函数，需要自己结合实际去移植
	     string++;
   }
}

