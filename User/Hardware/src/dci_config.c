#include "dci_config.h"
#include "gpio_config.h"


//2��dma������
uint32_t  dci_sampling_value_buffer[SAMPLING_DEPTH] = {0};

void dci1_config(void)
{
	//����һ��dci_parameter_struct���͵Ľṹ��ָ��
	dci_parameter_struct dci_struct;
	
	//1��GPIO��ʱ��ʹ��
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
	
	//2��DCIʱ��ʹ��
	rcu_periph_clock_enable(RCU_DCI);
	rcu_periph_clock_enable(RCU_SYSCFG);
	
	/*	CKOUT0(PA8)���ʱ���ź�				*/
	//								 GPIO_PUPD_NONE
	//								 GPIO_PUPD_PULLUP
	//								 GPIO_PUPD_PULLDOWN
//	gpio_mode_set(AD_CLK_PROT,GPIO_MODE_AF,GPIO_PUPD_NONE,AD_CLK_PIN);
	gpio_out_config(AD_CLK_PROT,AD_CLK_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_200MHZ);	
	//RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV1����������������8MHz
	//RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV2����������������4MHz
	//RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV4����������������2MHz

	//RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV2������������������100MHz
	//RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV4������������������50MHz
	//RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV5������������������40MHz
	
	rcu_ckout0_config(RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV5);				//1��������8��
	
	//2��DCI GPIO AF configuration
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
	
	//3��config DCI GPI
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
	
	//3������DCI�ӿ�
	/*
		dci_struct.capture_mode=DCI_CAPTURE_MODE_CONTINUOUS;		//��������
		dci_struct.clock_polarity=  DCI_CK_POLARITY_RISING;			//ʱ�������ش���
		dci_struct.hsync_polarity= DCI_HSYNC_POLARITY_LOW;			//ˮƽͬ�� - �����ڼ�͵�ƽ
		dci_struct.vsync_polarity= DCI_VSYNC_POLARITY_LOW;			//��ֱͬ�� - �����ڼ�͵�ƽ
		dci_struct.frame_rate= DCI_FRAME_RATE_ALL;					//��������֡
		dci_struct.interface_format= DCI_INTERFACE_FORMAT_8BITS;	//���ݿ��Ϊ8λ
		dci_init(&dci_struct);										//DCI��ʼ��
	*/
	
	
	dci_sync_codes_disable();									//Ӳ��ͬ��ģʽ
	dci_struct.clock_polarity = DCI_CK_POLARITY_FALLING;		//ʱ�Ӽ���ѡ��CKS  - �½��ز���
	
	dci_struct.hsync_polarity = DCI_HSYNC_POLARITY_HIGH;		//ˮƽͬ������ѡ��HPS - �����ڼ�ߵ�ƽ
	dci_struct.vsync_polarity = DCI_VSYNC_POLARITY_HIGH;		//��ֱͬ������ѡ��VPS - �����ڼ�ߵ�ƽ	
	
	dci_struct.capture_mode = DCI_CAPTURE_MODE_CONTINUOUS;		//��������ģʽSNAP = 0		
//	dci_struct.capture_mode = DCI_CAPTURE_MODE_SNAPSHOT;		//����ģʽSNAP = 1	
	dci_struct.frame_rate = DCI_FRAME_RATE_ALL;					//֡Ƶ��FR = 00  	  - ��������֡	
	dci_struct.interface_format = DCI_INTERFACE_FORMAT_10BITS;	//DCI�ӿ����ݿ��DCIF - 10λDCI����λ
	
	/*	��ʼ��DCI�Ĵ���	*/
	dci_init (&dci_struct);
	
	/*	���ֲ�22.5.3������ģʽ������������ʹ��CAP֡����λ��֮��֡���񡱡�*/
//	dci_enable();							//ʹ��DCI����
	dci_capture_enable();		//DCI����ʹ��
}


void dci_config(void)
{
	//����һ��dci_parameter_struct���͵Ľṹ��ָ��
	dci_parameter_struct dci_struct;
	
	//1��GPIO��ʱ��ʹ��
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
	
	//2��DCIʱ��ʹ��
	rcu_periph_clock_enable(RCU_DCI);
	
	
	/*	CKOUT0(PA8)���ʱ���ź�				*/
	//								 GPIO_PUPD_NONE
	//								 GPIO_PUPD_PULLUP
	//								 GPIO_PUPD_PULLDOWN
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_8);
	//RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV1����������������8MHz
	//RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV2����������������4MHz
	//RCU_CKOUT0SRC_HXTAL,RCU_CKOUT0_DIV4����������������2MHz
	
	//RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV4������������������50MHz
	//RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV5������������������40MHz	
	rcu_ckout0_config(RCU_CKOUT0SRC_PLLP,RCU_CKOUT0_DIV5);				//1��������8��
	

	
	//2��DCI GPIO AF configuration
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
	
	//3��config DCI GPI
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
	
	//3������DCI�ӿ�
	/*
		dci_struct.capture_mode=DCI_CAPTURE_MODE_CONTINUOUS;		//��������
		dci_struct.clock_polarity=  DCI_CK_POLARITY_RISING;			//ʱ�������ش���
		dci_struct.hsync_polarity= DCI_HSYNC_POLARITY_LOW;			//ˮƽͬ�� - �����ڼ�͵�ƽ
		dci_struct.vsync_polarity= DCI_VSYNC_POLARITY_LOW;			//��ֱͬ�� - �����ڼ�͵�ƽ
		dci_struct.frame_rate= DCI_FRAME_RATE_ALL;					//��������֡
		dci_struct.interface_format= DCI_INTERFACE_FORMAT_8BITS;	//���ݿ��Ϊ8λ
		dci_init(&dci_struct);										//DCI��ʼ��
	*/
	
	
	dci_sync_codes_disable();									//Ӳ��ͬ��ģʽ
	dci_struct.clock_polarity = DCI_CK_POLARITY_FALLING;		//ʱ�Ӽ���ѡ��CKS  - �½��ز���
	
	dci_struct.hsync_polarity = DCI_HSYNC_POLARITY_HIGH;		//ˮƽͬ������ѡ��HPS - �����ڼ�ߵ�ƽ
	dci_struct.vsync_polarity = DCI_VSYNC_POLARITY_HIGH;		//��ֱͬ������ѡ��VPS - �����ڼ�ߵ�ƽ	
	
	dci_struct.capture_mode = DCI_CAPTURE_MODE_CONTINUOUS;		//��������ģʽSNAP = 0		
//	dci_struct.capture_mode = DCI_CAPTURE_MODE_SNAPSHOT;		//����ģʽSNAP = 1	
	dci_struct.frame_rate = DCI_FRAME_RATE_ALL;					//֡Ƶ��FR = 00  	  - ��������֡	
	dci_struct.interface_format = DCI_INTERFACE_FORMAT_10BITS;	//DCI�ӿ����ݿ��DCIF - 10λDCI����λ
	
	/*	��ʼ��DCI�Ĵ���	*/
	dci_init (&dci_struct);
	
	/*	���ֲ�22.5.3������ģʽ������������ʹ��CAP֡����λ��֮��֡���񡱡�*/
//	dci_enable();							//ʹ��DCI����
	dci_capture_enable();		//DCI����ʹ��
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
	
    dma_single_struct.direction				=	DMA_PERIPH_TO_MEMORY;					//���䷽��(���� �� Memory)
    dma_single_struct.periph_addr			=	(volatile uint32_t)DCI_DR_ADDRESS;				//��������DCI_DATA��ַ:0x50050028U
    dma_single_struct.periph_memory_width	=	DMA_PERIPH_WIDTH_32BIT;					//�����������ݴ�����  
		dma_single_struct.periph_inc			=	DMA_MEMORY_INCREASE_DISABLE;			//�������һ��ַΪ�̶���ַģʽ
    dma_single_struct.memory0_addr			=	(uint32_t)(dci_sampling_value_buffer);	//����Memory��ַ:&battery_DCI_value_arry
		dma_single_struct.memory_inc			=	DMA_MEMORY_INCREASE_ENABLE;				//�ڴ����һ����ַ�ǵ�����ַģʽ
		dma_single_struct.number				=	SAMPLING_DEPTH;						//DMA�������ݸ���
		dma_single_struct.priority				=	DMA_PRIORITY_ULTRA_HIGH;				//���ȼ�Ϊ�����ߡ�
    dma_single_struct.circular_mode			=	DMA_CIRCULAR_MODE_DISABLE;				//DMA����ѭ��ģʽ
		
		dma_single_data_mode_init(DMA1,DMA_CH1,dma_single_struct);			//DMA�����ݴ���ģʽ��ʼ��

	
    dma_channel_subperipheral_select(DMA1,DMA_CH1,DMA_SUBPERI1);		//DMAͨ������ʹ��ѡ��
	
//	
//    dma_channel_enable(DMA1,DMA_CH1);									//ʹ��DMA1��ͨ��1
	
	
}















