#include "dci_config.h"
#include "gpio_config.h"


//2、dma缓冲区
uint32_t  dci_sampling_value_buffer[SAMPLING_DEPTH] = {0};

void dci1_config(void)
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
	rcu_periph_clock_enable(RCU_SYSCFG);
	
	/*	CKOUT0(PA8)输出时钟信号				*/
	//								 GPIO_PUPD_NONE
	//								 GPIO_PUPD_PULLUP
	//								 GPIO_PUPD_PULLDOWN
//	gpio_mode_set(AD_CLK_PROT,GPIO_MODE_AF,GPIO_PUPD_NONE,AD_CLK_PIN);
	gpio_out_config(AD_CLK_PROT,AD_CLK_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	
	//RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV1————————8MHz
	//RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV2————————4MHz
	//RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV4————————2MHz

	//RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV2—————————100MHz
	//RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV4—————————50MHz
	//RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV5—————————40MHz
	
	rcu_ckout0_config(RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV5);				//1————8、
	
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
	gpio_out_config(AD_DCI_CLK_PORT,AD_DCI_CLK_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_PIXCLK		PA6
	gpio_out_config(AD_DCI_HSYNC_PORT,AD_DCI_HSYNC_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_HSYNC			PA4
	gpio_out_config(AD_DCI_VSYNC_PORT,AD_DCI_VSYNC_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_VSYNC			PB7

	gpio_out_config(AD_DCI_DATA9_PORT,AD_DCI_DATA9_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	//DCI_D9			PC(12)		//HSB--D9
	gpio_out_config(AD_DCI_DATA8_PORT,AD_DCI_DATA8_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	//DCI_D8			PC(10)
	gpio_out_config(AD_DCI_DATA7_PORT,AD_DCI_DATA7_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D7			PB(9)
	gpio_out_config(AD_DCI_DATA6_PORT,AD_DCI_DATA6_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D6			PB(8)
	gpio_out_config(AD_DCI_DATA5_PORT,AD_DCI_DATA5_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D5			PD(3)
	gpio_out_config(AD_DCI_DATA4_PORT,AD_DCI_DATA4_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	//DCI_D4			PC(11)
	gpio_out_config(AD_DCI_DATA3_PORT,AD_DCI_DATA3_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D3			PC(9)
	gpio_out_config(AD_DCI_DATA2_PORT,AD_DCI_DATA2_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D2			PC(8)
	gpio_out_config(AD_DCI_DATA1_PORT,AD_DCI_DATA1_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	//DCI_D1			PA(10)
	gpio_out_config(AD_DCI_DATA0_PORT,AD_DCI_DATA0_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D0			PA(9)
	
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


void dci_config(void)
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
	rcu_ckout0_config(RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV5);				//1————8、
	

	
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
	gpio_out_config(GPIOA,GPIO_PIN_6,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_PIXCLK		PA6
	gpio_out_config(GPIOA,GPIO_PIN_4,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_HSYNC			PA4
	gpio_out_config(GPIOB,GPIO_PIN_7,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_VSYNC			PB7

	gpio_out_config(GPIOC,GPIO_PIN_12,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	//DCI_D9			PC(12)		//HSB--D9
	gpio_out_config(GPIOC,GPIO_PIN_10,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	//DCI_D8			PC(10)
	gpio_out_config(GPIOB,GPIO_PIN_9,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D7			PB(9)
	gpio_out_config(GPIOB,GPIO_PIN_8,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D6			PB(8)
	gpio_out_config(GPIOD,GPIO_PIN_3,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D5			PD(3)
	gpio_out_config(GPIOC,GPIO_PIN_11,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	//DCI_D4			PC(11)
	gpio_out_config(GPIOE,GPIO_PIN_1,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D3			PC(9)
	gpio_out_config(GPIOC,GPIO_PIN_8,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D2			PC(8)
	gpio_out_config(GPIOA,GPIO_PIN_10,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	//DCI_D1			PA(10)
	gpio_out_config(GPIOA,GPIO_PIN_9,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);		//DCI_D0			PA(9)
	
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


void dci_dma_config(void)
{
//    /* DCI DMA configuration */ 
//    rcu_periph_clock_enable(RCU_DMA0);	
//	
//    dma_deinit(DMA0,DMA_CH1);	
	
    /* DCI DMA configuration */ 
    rcu_periph_clock_enable(RCU_DMA1);	
	
    dma_deinit(DMA1,DMA_CH1);	
	
    dma_single_data_parameter_struct dma_single_struct;    /* DCI configuration */ 	
	
    dma_single_struct.direction				=	DMA_PERIPH_TO_MEMORY;					//传输方向(外设 → Memory)
    dma_single_struct.periph_addr			=	(volatile uint32_t)DCI_DR_ADDRESS;				//配置外设DCI_DATA地址:0x50050028U
    dma_single_struct.periph_memory_width	=	DMA_PERIPH_WIDTH_32BIT;					//配置外设数据传输宽度  
		dma_single_struct.periph_inc			=	DMA_MEMORY_INCREASE_DISABLE;			//外设的下一地址为固定地址模式
    dma_single_struct.memory0_addr			=	(uint32_t)(dci_sampling_value_buffer);	//配置Memory地址:&battery_DCI_value_arry
		dma_single_struct.memory_inc			=	DMA_MEMORY_INCREASE_ENABLE;				//内存的下一个地址是递增地址模式
		dma_single_struct.number				=	SAMPLING_DEPTH;						//DMA传输数据个数
		dma_single_struct.priority				=	DMA_PRIORITY_ULTRA_HIGH;				//优先级为“超高”
    dma_single_struct.circular_mode			=	DMA_CIRCULAR_MODE_DISABLE;				//DMA开启循环模式
		
		dma_single_data_mode_init(DMA1,DMA_CH1,dma_single_struct);			//DMA单数据传输模式初始化

	
    dma_channel_subperipheral_select(DMA1,DMA_CH1,DMA_SUBPERI1);		//DMA通道外设使能选择
	
//	
//    dma_channel_enable(DMA1,DMA_CH1);									//使能DMA1的通道1
	
	
}















