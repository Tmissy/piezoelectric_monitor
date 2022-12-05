/***************************产品介绍
程序功能 : 通过收发电磁脉冲来获取金属厚度
芯片引脚 : 
			GD32F450VET6 + AD9215-105
			
			驱动时序引脚
				Power_PWM1			PE2
				Power_PWM2			PE3
				PWM_EN				PE4
				T_PWDN				PE6
			
			DCI接收引脚
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
			
			Usart引脚
				TXD					PA2
				RXD					PA3
			
注意事项 ： 测试DCI+ADC的功能，可通过信号发生器输出（0.75~1.95）的5MHz正弦波，
			得到20个点（100MHz÷5MHz=20）描述一个周期即功能正常
			
程序作者 : 零声科技 李品智
************************************************************/




/***************************程序功能详细介绍
检测原理：
			MCU发送“驱动信号时序”使驱动电路工作
			驱动电路获取信息波形经过三运放放大给AD9212-105（AD采用芯片）
			AD采样完将信息通过MCU的DCI给MCU

功能编写逻辑：
			1、配置、使能“时钟、GPIO、DCI、DMA、USART”
			2、发送驱动脉冲时序、获取DCI数据、通过串口发送
			3、不断重复2

程序修改点：
			各种非复用引脚			参考port.c和port.h中的管脚定义
			
			DCI的获取速度			dci_adc.c	的	rcu_ckout0_config(RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV2);
			获取数据个数			dci_adc.h	的	#define Sampling_number	4096
			USART波特率设置			my_usart.c	的	usart_baudrate_set(USART1,115200);
			平均次数				main.c		的	dci_average_count
			DCI数据采集的延时		main.c		的	Sampling_time
*********************************************************************************************************/





u8 flag_uart_out = 0;			//串口发出标志位：0--不发出，1--发出

u16 dci_average_count = 1;		//设置多少次平均

#define Sampling_time 100		//DCI数据采集的延时时间

extern u16 battery_DCI_value_arry[Sampling_number_rel];

u16 dci_average_value[Sampling_number] = {0};	//DCI的平均值所存放的数组







void my_driver_timeserie(void)
{
	u16 i,j;
	static u16 data_count = 0;					//记录第几批数据采集
