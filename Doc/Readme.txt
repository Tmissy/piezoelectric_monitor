/***************************��Ʒ����
������ : ͨ���շ������������ȡ�������
оƬ���� : 
			GD32F450VET6 + AD9215-105
			
			����ʱ������
				Power_PWM1			PE2
				Power_PWM2			PE3
				PWM_EN				PE4
				T_PWDN				PE6
			
			DCI��������
				DCI_PIXCLK			PA6
				DCI_HSYNC			PA4
				DCI_VSYNC			PB7

				DCI_D9				PC(12)		//HSB--D9
				DCI_D8				PC(10)
				DCI_D7				PB(9)
				DCI_D6				PB(8)
				DCI_D5				PD(3)
				DCI_D4				PC(11)
				DCI_D3				PC(9)
				DCI_D2				PC(8)
				DCI_D1				PA(10)
				DCI_D0				PA(9)
			
			Usart����
				TXD					PA2
				RXD					PA3
			
ע������ �� ����DCI+ADC�Ĺ��ܣ���ͨ���źŷ����������0.75~1.95����5MHz���Ҳ���
			�õ�20���㣨100MHz��5MHz=20������һ�����ڼ���������
			
�������� : �����Ƽ� ��Ʒ��
************************************************************/




/***************************��������ϸ����
���ԭ��
			MCU���͡������ź�ʱ��ʹ������·����
			������·��ȡ��Ϣ���ξ������˷ŷŴ��AD9212-105��AD����оƬ��
			AD�����꽫��Ϣͨ��MCU��DCI��MCU

���ܱ�д�߼���
			1�����á�ʹ�ܡ�ʱ�ӡ�GPIO��DCI��DMA��USART��
			2��������������ʱ�򡢻�ȡDCI���ݡ�ͨ�����ڷ���
			3�������ظ�2

�����޸ĵ㣺
			���ַǸ�������			�ο�port.c��port.h�еĹܽŶ���
			
			DCI�Ļ�ȡ�ٶ�			dci_adc.c	��	rcu_ckout0_config(RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV2);
			��ȡ���ݸ���			dci_adc.h	��	#define Sampling_number	4096
			USART����������			my_usart.c	��	usart_baudrate_set(USART1,115200);
			ƽ������				main.c		��	dci_average_count
			DCI���ݲɼ�����ʱ		main.c		��	Sampling_time
*********************************************************************************************************/





u8 flag_uart_out = 0;			//���ڷ�����־λ��0--��������1--����

u16 dci_average_count = 1;		//���ö��ٴ�ƽ��

#define Sampling_time 100		//DCI���ݲɼ�����ʱʱ��

extern u16 battery_DCI_value_arry[Sampling_number_rel];

u16 dci_average_value[Sampling_number] = {0};	//DCI��ƽ��ֵ����ŵ�����







void my_driver_timeserie(void)
{
	u16 i,j;
	static u16 data_count = 0;					//��¼�ڼ������ݲɼ�
