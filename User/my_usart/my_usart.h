#include "gd32f4xx.h"
#include "stdio.h"

/*
	����Usart1
*/
void USART1_Config(void);

/*
	��ʼ��Usart1
*/
void Usart1_Init(void);

/*
	����u8���ֽ�����	
*/
void usart_send_byte(uint8_t ch);


/**
  * @brief  �ض���c�⺯��printf��USART1
  * @param  None
  * @retval 
  */
int fputc(int ch, FILE *f);

/*
	����Ҫ��int fputc(int ch, FILE *f)������д
*/
void Usart_Send_String(unsigned char *string);

