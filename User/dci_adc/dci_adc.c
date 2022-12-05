#include "dci_adc.h"

/**************************
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

***************************/

//1、dci的dma是否传输完成标志位
bool flag_DMA_DCI_accomplish = FALSE;

//2、dma缓冲区
u16 battery_DCI_value_arry[Sampling_number] = {0};

/*
DCI初始化配置：
		连续捕获模式
		时钟上升沿触发
		DCI下降沿捕获
		水平消隐期间低电平
		垂直消隐期间低电平
		帧频率为每隔三帧捕获一次
		数码相机接口格式为10位并行数据
*/
void mcu_dci_config(void)
{
	//定义一个dci_parameter_struct类型的结构体指针
	dci_parameter_struct dci_struct;
	
	//1、GPIO口时钟使能
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
	
	//2、DCI时钟使能
	rcu_periph_clock_enable(RCU_DCI);
	
	
	/*	CKOUT0(PA8)输出时钟信号				*/
	//								 GPIO_PUPD_NONE
	//								 GPIO_PUPD_PULLUP
	//								 GPIO_PUPD_PULLDOWN
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_8);
	//RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV1————————8MHz
	//RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV2————————4MHz
	//RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV4————————2MHz
	
	//RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV4—————————50MHz
	//RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV5—————————40MHz	
	rcu_ckout0_config(RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV2);				//1————8、
	

	
	//2、DCI GPIO AF configuration
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_6);		//DCI_PIXCLK		PA6
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_4);		//DCI_HSYNC			PA4
    gpio_af_set(GPIOB, GPIO_AF_13, GPIO_PIN_7);		//DCI_VSYNC			PB7
	
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_12);	//DCI_D9			PC(12)		//HSB--D9
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_10);	//DCI_D8			PC(10)
    gpio_af_set(GPIOB, GPIO_AF_13, GPIO_PIN_9);		//DCI_D7			PB(9)
    gpio_af_set(GPIOB, GPIO_AF_13, GPIO_PIN_8);		//DCI_D6			PB(8)
    gpio_af_set(GPIOD, GPIO_AF_13, GPIO_PIN_3);		//DCI_D5			PD(3)
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_11);	//DCI_D4			PC(11)
    gpio_af_set(GPIOE, GPIO_AF_13, GPIO_PIN_1);		//DCI_D3			PE(1)
    gpio_af_set(GPIOC, GPIO_AF_13, GPIO_PIN_8);		//DCI_D2			PC(8)
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_10);	//DCI_D1			PA(10)
    gpio_af_set(GPIOA, GPIO_AF_13, GPIO_PIN_9);		//DCI_D0			PA(9)
	
	//3、config DCI GPI
	GPIO_Out_config(GPIOA,GPIO_PIN_6,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_PIXCLK		PA6
	GPIO_Out_config(GPIOA,GPIO_PIN_4,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_HSYNC			PA4
	GPIO_Out_config(GPIOB,GPIO_PIN_7,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_VSYNC			PB7

	GPIO_Out_config(GPIOC,GPIO_PIN_12,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	//DCI_D9			PC(12)		//HSB--D9
	GPIO_Out_config(GPIOC,GPIO_PIN_10,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	//DCI_D8			PC(10)
	GPIO_Out_config(GPIOB,GPIO_PIN_9,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D7			PB(9)
	GPIO_Out_config(GPIOB,GPIO_PIN_8,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D6			PB(8)
	GPIO_Out_config(GPIOD,GPIO_PIN_3,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D5			PD(3)
	GPIO_Out_config(GPIOC,GPIO_PIN_11,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	//DCI_D4			PC(11)
	GPIO_Out_config(GPIOE,GPIO_PIN_1,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D3			PC(9)
	GPIO_Out_config(GPIOC,GPIO_PIN_8,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D2			PC(8)
	GPIO_Out_config(GPIOA,GPIO_PIN_10,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	//DCI_D1			PA(10)
	GPIO_Out_config(GPIOA,GPIO_PIN_9,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D0			PA(9)
	
	//3、配置DCI接口
	/*
		dci_struct.capture_mode=DCI_CAPTURE_MODE_CONTINUOUS;		//连续拍照
		dci_struct.clock_polarity=  DCI_CK_POLARITY_RISING;			//时钟上升沿触发
		dci_struct.hsync_polarity= DCI_HSYNC_POLARITY_LOW;			//水平同步 - 消隐期间低电平
		dci_struct.vsync_polarity= DCI_VSYNC_POLARITY_LOW;			//垂直同步 - 消隐期间低电平
		dci_struct.frame_rate= DCI_FRAME_RATE_ALL;					//捕获所有帧
		dci_struct.interface_format= DCI_INTERFACE_FORMAT_8BITS;	//数据宽度为8位
		dci_init(&dci_struct);										//DCI初始化
	*/
	
	
	dci_sync_codes_disable();									//硬件同步模式
	dci_struct.clock_polarity = DCI_CK_POLARITY_FALLING;		//时钟极性选择CKS  - 下降沿捕获
	
	dci_struct.hsync_polarity = DCI_HSYNC_POLARITY_HIGH;		//水平同步极性选择HPS - 消隐期间高电平
	dci_struct.vsync_polarity = DCI_VSYNC_POLARITY_HIGH;		//垂直同步极性选择VPS - 消隐期间高电平	
	
	dci_struct.capture_mode = DCI_CAPTURE_MODE_CONTINUOUS;		//连续捕获模式SNAP = 0		
//	dci_struct.capture_mode = DCI_CAPTURE_MODE_SNAPSHOT;		//快照模式SNAP = 1	
	dci_struct.frame_rate = DCI_FRAME_RATE_ALL;					//帧频率FR = 00  	  - 捕获所有帧	
	dci_struct.interface_format = DCI_INTERFACE_FORMAT_10BITS;	//DCI接口数据宽度DCIF - 10位DCI数据位
	
	/*	初始化DCI寄存器	*/
	dci_init (&dci_struct);
	
	/*	【手册22.5.3、连续模式——————使能CAP帧捕获位，之后“帧捕获”】*/
//	dci_enable();							//使能DCI功能
	dci_capture_enable();		//DCI捕获使能
}




void mcu_dma_config(void)
{
    /* DCI DMA configuration */ 
    rcu_periph_clock_enable(RCU_DMA1);	
	
    dma_deinit(DMA1,DMA_CH1);	
	
/*!
	periph_addr			: peripheral base address
	periph_width		: DMA_PERIPH_WIDTH_8BIT,DMA_PERIPH_WIDTH_16BIT,DMA_PERIPH_WIDTH_32BIT
	periph_inc			: DMA_PERIPH_INCREASE_ENABLE,DMA_PERIPH_INCREASE_DISABLE,DMA_PERIPH_INCREASE_FIX 
	memory0_addr		: memory0 base address
	memory_width		: DMA_MEMORY_WIDTH_8BIT,DMA_MEMORY_WIDTH_16BIT,DMA_MEMORY_WIDTH_32BIT
	memory_inc			: DMA_MEMORY_INCREASE_ENABLE,DMA_MEMORY_INCREASE_DISABLE
	direction			: DMA_PERIPH_TO_MEMORY,DMA_MEMORY_TO_PERIPH,DMA_MEMORY_TO_MEMORY
	number				: the number of remaining data to be transferred by the DMA
	priority			: DMA_PRIORITY_LOW,DMA_PRIORITY_MEDIUM,DMA_PRIORITY_HIGH,DMA_PRIORITY_ULTRA_HIGH
	circular_mode		: DMA_CIRCULAR_MODE_ENABLE,DMA_CIRCULAR_MODE_DISABLE
	memory_burst_width	: DMA_MEMORY_BURST_SINGLE,DMA_MEMORY_BURST_4_BEAT,DMA_MEMORY_BURST_8_BEAT,DMA_MEMORY_BURST_16_BEAT
	periph_burst_width	: DMA_PERIPH_BURST_SINGLE,DMA_PERIPH_BURST_4_BEAT,DMA_PERIPH_BURST_8_BEAT,DMA_PERIPH_BURST_16_BEAT
	critical_value		: DMA_FIFO_1_WORD,DMA_FIFO_2_WORD,DMA_FIFO_3_WORD,DMA_FIFO_4_WORD
*/
//	dma_multi_data_parameter_struct dma_multi_data_parameter;

//    dma_multi_data_parameter.periph_addr		=	(uint32_t)DCI_DR_ADDRESS;				//配置外设DCI_DATA地址:0x50050028U
//	dma_multi_data_parameter.periph_width		=	DMA_PERIPH_WIDTH_32BIT;
//    dma_multi_data_parameter.memory0_addr		=	(uint32_t)(&battery_DCI_value_arry);	//配置Memory地址:&battery_DCI_value_arry
//	dma_multi_data_parameter.memory_width		=	DMA_MEMORY_WIDTH_32BIT;
//    dma_multi_data_parameter.circular_mode		=	DMA_CIRCULAR_MODE_ENABLE;				//DMA开启循环模式
//	dma_multi_data_parameter.direction			=	DMA_PERIPH_TO_MEMORY;					//传输方向(外设 → Memory)
//	dma_multi_data_parameter.number				=	Sampling_number_rel;						//DMA传输数据个数
//	dma_multi_data_parameter.priority			=	DMA_PRIORITY_ULTRA_HIGH;				//优先级为“超高”
//    
//	dma_multi_data_mode_init(DMA1,DMA_CH1,dma_multi_data_parameter);
	
	
    dma_single_data_parameter_struct dma_single_struct;    /* DCI configuration */ 	
	
    dma_single_struct.direction				=	DMA_PERIPH_TO_MEMORY;					//传输方向(外设 → Memory)
    dma_single_struct.periph_addr			=	(uint32_t)DCI_DR_ADDRESS;				//配置外设DCI_DATA地址:0x50050028U
    dma_single_struct.periph_memory_width	=	DMA_PERIPH_WIDTH_32BIT;					//配置外设数据传输宽度  
	dma_single_struct.periph_inc			=	DMA_MEMORY_INCREASE_DISABLE;			//外设的下一地址为固定地址模式
    dma_single_struct.memory0_addr			=	(uint32_t)(&battery_DCI_value_arry);	//配置Memory地址:&battery_DCI_value_arry
	dma_single_struct.memory_inc			=	DMA_MEMORY_INCREASE_ENABLE;				//内存的下一个地址是递增地址模式
	dma_single_struct.number				=	Sampling_number;						//DMA传输数据个数
	dma_single_struct.priority				=	DMA_PRIORITY_ULTRA_HIGH;				//优先级为“超高”
    dma_single_struct.circular_mode			=	DMA_CIRCULAR_MODE_ENABLE;				//DMA开启循环模式
	
    dma_single_data_mode_init(DMA1,DMA_CH1,dma_single_struct);			//DMA单数据传输模式初始化
	
	
    dma_channel_subperipheral_select(DMA1,DMA_CH1,DMA_SUBPERI1);		//DMA通道外设使能选择
	
	
    dma_channel_enable(DMA1,DMA_CH1);									//使能DMA1的通道1
}

