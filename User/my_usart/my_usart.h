#include "gd32f4xx.h"
#include "stdio.h"

/*
	配置Usart1
*/
void USART1_Config(void);

/*
	初始化Usart1
*/
void Usart1_Init(void);

/*
	发送u8单字节数据	
*/
void usart_send_byte(uint8_t ch);


/**
  * @brief  重定向c库函数printf到USART1
  * @param  None
  * @retval 
  */
int fputc(int ch, FILE *f);

/*
	必须要有int fputc(int ch, FILE *f)函数复写
*/
void Usart_Send_String(unsigned char *string);

