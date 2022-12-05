#include "my_usart.h"

#include "port.h"

/*
	����Usart1
*/
void USART1_Config(void)
{
//	//1���򿪴���1���ڵ�GPIOʱ��
//	rcu_periph_clock_enable(GPIOA);	
	
	//2���򿪴���1ʱ��
	rcu_periph_clock_enable(RCU_USART1);
	
	//3��Usart1���ã�PA2������T��PA3������R��
	gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_2);
	GPIO_Out_config(GPIOA,GPIO_PIN_2,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	
	gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_3);
	GPIO_Out_config(GPIOA,GPIO_PIN_3,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	
	//3�����ڸ�λ����
		usart_deinit(USART1);

	usart_baudrate_set(USART1,115200);						//���ò�����		115200
	usart_word_length_set(USART1,USART_WL_8BIT);			//���ô����ֳ�		8
	usart_stop_bit_set(USART1,USART_STB_1BIT);				//����ֹͣλ		1
	usart_parity_config(USART1,USART_PM_NONE);				//����У��λ		None
	usart_transmit_config(USART1,USART_TRANSMIT_ENABLE);	//���÷���ʹ��
	
}


/*
	��ʼ��Usart1
*/
void Usart1_Init(void)
{
	usart_enable(USART1);
}

/*
	����u8���ֽ�����	
*/
void usart_send_byte(uint8_t ch)
{
  /* ����һ���ֽ����ݵ�USART1 */
  usart_data_transmit(USART1,ch);

  /* �ȴ�������� */
  while (usart_flag_get(USART1, USART_FLAG_TBE) == RESET);  
}


/**
  * @brief  �ض���c�⺯��printf��USART1
  * @param  None
  * @retval 
  */
//int fputc(int ch, FILE *f)
//{
//  /*�����־λ*/
//  usart_flag_clear(USART1,USART_FLAG_TC);
//  
//  /* ����һ���ֽ����ݵ�USART1 */
//  usart_data_transmit(USART1, (uint8_t) ch);
//  
//  /* �ȴ�������� */
//  while (usart_flag_get(USART1, USART_FLAG_TC) == RESET);   
//  
//  return (ch);
//}

/*
	����Ҫ��int fputc(int ch, FILE *f)������д
*/
void Usart_Send_String(unsigned char *string)
{
   while(*string)
   {
	     usart_send_byte(*string);//�˺�����ʾ���ڷ��͵����ַ��ĺ�������Ҫ�Լ����ʵ��ȥ��ֲ
	     string++;
   }
}

