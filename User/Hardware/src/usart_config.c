#include "usart_config.h"
#include "gpio_config.h"
#include "systick.h"
#include "timer_config.h"

#define USART_RECV_BUF_SIZE 300// 串口接收缓存长度

//static uint8_t USART_Rx_Buf[USART_RECV_BUF_SIZE];// 用于保存 RX 接收到的数据
//static uint16_t USART_Rx_Len = 0;                // 用于保存 RX 接收到的数据的个数
static uint16_t USART_Rx_Count = 0;              // 用于保存 RX 接收数据时已经接收到的数据个数
bool uartReceiveFlag = false;

typedef struct 
{	
	uint8_t* recv_buf;
	uint32_t* recv_size;
}recv_t;


static recv_t nb_sim7020_recv_out;


void gd_debug_com_init()
{
		rcu_periph_clock_enable(RCU_USART1);
		rcu_periph_clock_enable(RCU_GPIOA);
	    /* configure USART0 TX as alternate function push-pull */

		gpio_af_set(DEBUG_USART_TX_PROT,GPIO_AF_7,DEBUG_UASRT_TX_PIN);
		gpio_out_config(DEBUG_USART_TX_PROT,DEBUG_UASRT_TX_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
	
	    /* configure USART0 RX as alternate function push-pull */

		gpio_af_set(DEBUG_USART_RX_PORT,GPIO_AF_7,DEBUG_UASRT_RX_PIN);
		gpio_out_config(DEBUG_USART_RX_PORT,DEBUG_UASRT_RX_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
		
    /* USART configure */
		usart_deinit(DEBUG_USART);
    usart_baudrate_set(DEBUG_USART, 115200U);
    usart_word_length_set(DEBUG_USART, USART_WL_8BIT);
    usart_stop_bit_set(DEBUG_USART, USART_STB_1BIT);
    usart_parity_config(DEBUG_USART, USART_PM_NONE);
    usart_hardware_flow_rts_config(DEBUG_USART, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(DEBUG_USART, USART_CTS_DISABLE);
    usart_receive_config(DEBUG_USART, USART_RECEIVE_ENABLE);
    usart_transmit_config(DEBUG_USART, USART_TRANSMIT_ENABLE);
    usart_enable(DEBUG_USART);
}


void nb_usart_init(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOD);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART2);

	    /* configure USART0 TX as alternate function push-pull */
		gpio_af_set(NB_USART_TX_PROT,GPIO_AF_7,NB_UASRT_TX_PIN);
		gpio_out_config(NB_USART_TX_PROT,NB_UASRT_TX_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);
		
	    /* configure USART0 RX as alternate function push-pull */
		gpio_af_set(NB_USART_RX_PORT,GPIO_AF_7,NB_UASRT_RX_PIN);
		gpio_out_config(NB_USART_RX_PORT,NB_UASRT_RX_PIN,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ);

    /* USART configure */
    usart_baudrate_set(NB_USART, 9600U);
    usart_word_length_set(NB_USART, USART_WL_8BIT);
    usart_stop_bit_set(NB_USART, USART_STB_1BIT);
    usart_parity_config(NB_USART, USART_PM_NONE);
    usart_receive_config(NB_USART, USART_RECEIVE_ENABLE);
    usart_transmit_config(NB_USART, USART_TRANSMIT_ENABLE);
		
    usart_enable(NB_USART);
		nvic_irq_enable(USART2_IRQn, 0,0);                // 使能中断，配置中断的优先级
		usart_interrupt_enable(NB_USART, USART_INTEN_RBNEIE); // 使能“读数据缓冲区非空中断和过载错误中断”
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(DEBUG_USART, (uint8_t)ch);
    while(RESET == usart_flag_get(DEBUG_USART, USART_FLAG_TBE));
    return ch;
}

Status_TypeDef usart_datas_transmit(uint32_t usart_periph,uint8_t*data,uint16_t size,uint32_t timeout){
		uint32_t tickstart = Get_Tick() ;
		uint16_t count = 0;
		count = size;
		if((data == NULL) || (size == 0)){
			printf("AT cmd:data tx  STATUS_ERROR r\n");
			return STATUS_ERROR;
		}
		while(count > 0){
			usart_data_transmit(usart_periph, *data);
			while(RESET == usart_flag_get(usart_periph,USART_FLAG_TBE));
			count--;
			data++;
	}
		return STATUS_OK;
}



Status_TypeDef usart_datas_receive(uint32_t usart_periph,uint8_t*data,uint16_t size,uint32_t timeout){
		uint32_t tickstart = Get_Tick() ;
		uint16_t count = 0;
		count = size;
		if((data == NULL) || (size == 0)){
			return STATUS_ERROR;
		}
		debug_printf("stat rx \r\n");
		while(count > 0){
			if(usart_flag_get(usart_periph,USART_FLAG_RBNE) == SET){
				*data = usart_data_receive(usart_periph);
				count--;
				data++;
			}
		}
		debug_printf("rx compulete\r\n");
		return STATUS_OK;
}


bool Uart_recv_buf_regster(uint8_t* out_buf, uint32_t* line_size)
{
		if(out_buf == NULL && line_size == NULL)
		{
			return false;
		}
		nb_sim7020_recv_out.recv_buf = out_buf;
		nb_sim7020_recv_out.recv_size = line_size;		
		return true;
}

void Uart_recv_buf_unregster()
{
		nb_sim7020_recv_out.recv_buf = NULL;
		nb_sim7020_recv_out.recv_size = NULL;
}

void Uart_recv_reset()
{
		USART_Rx_Count = 0;		
}

void NB_UART_RxCpltCallback(){
		uint8_t ch = (uint8_t)usart_data_receive(UART3);// 从 RX 数据寄存器中读取数据
		if(USART_Rx_Count < USART_RECV_BUF_SIZE)
		{
			nb_sim7020_recv_out.recv_buf[USART_Rx_Count++] = ch;
			*(nb_sim7020_recv_out.recv_size) = USART_Rx_Count;
			uartReceiveFlag = true;
			TIM_Reset_Timeout();
		}else{
			Uart_recv_reset();
		}
}

