#include <string.h>
#include "at_parse.h"
#include "dev_sim7020.h"
#include "communicate.h"
#include "at_uart_hal.h"
#include "at_parse.h"
#include "gpio_config.h"
#include "systick.h"
#include "net_protocol.h"

#define AT_UART 						UART3
#define AT_CMD_AT           "AT"
#define AT_CMD_CPIN         "AT+CPIN?"
#define AT_CMD_VERSION         "AT+NV=GET,EXTVER"
#define AT_CMD_CLOSE_FLY_MODE  "AT+CFUN=1"

#define AT_CMD_POWER_CLASS_QUERY  "AT+NPOWERCLASS?"
#define AT_CMD_NPOWER_CLASS_B3_6    "AT+NPOWERCLASS=3,6"
#define AT_CMD_NPOWER_CLASS_B5_6    "AT+NPOWERCLASS=5,6"
#define AT_CMD_NPOWER_CLASS_B8_6    "AT+NPOWERCLASS=8,6"
#define AT_CMD_NPOWER_CLASS_B20_6    "AT+NPOWERCLASS=20,6"

#define AT_CMD_CSQ          "AT+CSQ"
#define AT_CMD_SET_APN      "AT+ECATTBEARER=3,1,\"cmnbiot1\""
#define AT_CMD_NET_PS     "AT+CEREG=1"
#define AT_CMD_CHECK_PS     "AT+CEREG?"
#define AT_CMD_NET_IP       "AT+CGCONTRDP"
#define AT_CMD_NSONMI      	"AT+NSONMI=3"
#define AT_CMD_CHECK_NSONMI      "AT+NSONMI?"

#define AT_CMD_GET_SOFTWARE_VERSION  "AT+NV=GET,EXTVER"
#define AT_CMD_CPSMS_QUERY      "AT+CPSMS?"
#define AT_RESETCTL_20MS        "AT+RESETCTL=0"
#define AT_RESETCTL_QUERY       "AT+RESETCTL?"


//#define AT_DNS_NET         "AT+ECDNS=www.baidu.com"
//#define AT_PING_NET        "AT+ECPING=\"139.9.174.115\",10,32,9000"
//#define AT_PING_NET        "AT+ECPING=\"180.97.33.107\",10,32,9000"
//#define AT_PING_NET        "AT+ECPING=\"221.229.214.202\",10,32,9000"

#define AT_DNS_NET         "AT+ECDNS=www.yymit.cn"
#define AT_PING_NET        "AT+ECPING=\"139.9.174.115\",10,32,9000"
#define AT_SET_VDD_2V8     "AT+VIOSET=2"
#define AT_SET_VDD_1V8     "AT+VIOSET=1"
#define AT_PING_STOP       "AT+ECPING=0"

#define AT_SET_SOCK_UDP     "AT+NSOCR=DGRAM,17,1234,1,AF_INET"								//����UDP socket ��Ӧ�����socket id
#define AT_SET_SOCK_TCP     "AT+NSOCR=STREAM,6,1234,1,AF_INET"
#define AT_CON_SOCK_TCP     "AT+NSOST=1,\""
#define AT_SEND_SOCK_UDP    "AT+NSOST=0,\""																	//����UDP����
#define AT_SEND_SOCK_TCP    "AT+NSOST=1,"
#define AT_CLOSE_SOCK       "AT+NSOCL=0"

#define AT_SET_115200_RATE  "AT+IPR=115200"

//#define AT_SLEEP_800nA      "AT+ECPMUCFG=1,4"
//#define AT_SLEEP_8uA        "AT+ECPMUCFG=1,3"
//#define AT_SLEEP_22uA       "AT+ECPMUCFG=1,2"
//#define AT_IDEL_2_8mA       "AT+ECPMUCFG=1,1"
//#define AT_POWER_MAX        "AT+ECPMUCFG=0"


#define AT_REPORT_CLOSE     "AT+ECURC=ALL,0"

//#define AT_SLEEP_NOW        "AT+WORKLOCK=0"
#define AT_SLEEP_NOW        "AT+FASTOFF=0"

#define AT_LISTEN_KEY_CSQ           "+CSQ:"
#define AT_LISTEN_KEY_SERVER        "+NSONMI:"
#define AT_LISTEN_KEY_NETCON        "+CEREG:"

#define NB_RESET_START()   gpio_bit_set(NB_RST_GPIO_PROT,NB_RST_PIN) 
#define NB_RESET_STOP()    gpio_bit_reset(NB_RST_GPIO_PROT,NB_RST_PIN) 

#define NB_WEAKUP_START()    gpio_bit_reset(NB_WAKE_GPIO_PROT,NB_WAKE_PIN)
#define NB_WEAKUP_STOP()     gpio_bit_set(NB_WAKE_GPIO_PROT,NB_WAKE_PIN)

#define NUM_TO_CHAR(num, str)      sprintf(str, "%d", num)
#define NUM_TO_HEX_CHAR(num, str)  sprintf(str, "%02X", num)

#define SIM_RECV_BUF_SIZE 300


bool ls_start_udp_tcp = false; 
bool ls_con_net = false;
bool nb_reboot_done = false;

static bool ls_init = false;
static bool ls_use_udp = false;
static bool ls_register_cb = false;
static uint16_t net_watch_dog_cnt = 0;

char remote_ip[20] = {REMOTE_IP};
uint16_t remote_port = REMOTE_PORT;

struct sim7020_control_t
{
    char recv_buf[SIM_RECV_BUF_SIZE];
    uint16_t recv_len;
}sim_ctl;

/**
*@brief		��NB���Ͳ�ָ��ȴ��ظ�
*@param     cmd��ָ������
*           prefix���ȴ��Ĺؼ���
*           timeout����ʱʱ��
*@note      ��
*@return	true: ͨ�ųɹ�
*			true: ͨ��ʧ��
*/

static bool sim7020_send_atcmd(char *cmd,char *prefix,uint32_t timeout)
{
    memset(sim_ctl.recv_buf,0,SIM_RECV_BUF_SIZE);
    sim_ctl.recv_len=SIM_RECV_BUF_SIZE;
    return at_send_wait_reply  (cmd,true,
                        prefix,"ERROR",
                        sim_ctl.recv_buf,&sim_ctl.recv_len,
                        timeout);
}

/**
*@brief		����NB���ڵĲ�����
*@param     ��
*@note      ��
*@return	true: ���óɹ�
*			false: ����ʧ��
*/
static bool sim7020_set_band()
{
	uint8_t i = 0;
//	while(i++ < 3)at_send_no_reply(AT_IDEL_2_8mA,strlen(AT_IDEL_2_8mA), true, 0);	
	usart_baudrate_set(AT_UART, 9600U);

//	while(i++ < 3)at_send_no_reply(AT_IDEL_2_8mA,strlen(AT_IDEL_2_8mA), true, 0);

	while(i++ < 3)at_send_no_reply(AT_SET_115200_RATE,strlen(AT_SET_115200_RATE), true, 200);
		usart_baudrate_set(AT_UART, 115200U);
	return true;	
}

/**
*@brief		����NB��Դ
*@param     ��
*@note      ��
*@return	true: ���óɹ�
*			false: ����ʧ��
*/

bool sim7020_pwr_on()
{
//	at_send_no_reply(AT_IDEL_2_8mA,strlen(AT_IDEL_2_8mA), true, 0);	
    printf("E7025 power on!\r\n");
	NB_RESET_STOP();	
	NB_WEAKUP_STOP();	
	return true;
}

/**
*@brief		�ر�NB��Դ
*@param     ��
*@note      ��
*@return	true: ���óɹ�
*			false: ����ʧ��
*/

bool sim7020_pwr_off()
{
    uint8_t err=0;
    printf("E7025 power off!\r\n");
    
    while (err < 3)
    {
        err++;
				delay_1ms(200);
        if(!sim7020_sleep(0))continue;
        return true;
    }
    return false;
}

/**
*@brief		��λ����NB
*@param     ��
*@note      ��
*@return	true: ���óɹ�
*			false: ����ʧ��
*/
bool sim7020_reboot()
{
	//��λNBģ��
//	NB_WEAKUP_STOP();
	debug_printf("start nb reset !\r\n");
	NB_RESET_START();
	delay_1ms(100);
	NB_RESET_STOP();
	delay_1ms(500);
	ls_con_net = false;
	
	//����115200�����ʲ������ÿ���ģʽ����ֹ˯��		
	sim7020_set_band();
	
	//��������DMA����
//	Uart_start_recv(COM_UART4);
    return true;
}

/**
*@brief		���NB�����������
*@param     ��
*@note      ��
*@return	true: �������ɹ�
*			false: �������ʧ��
*/

bool sim7020_check_net()
{
    uint16_t err=0;
	
	if(ls_con_net)return true;
		
    while(err<5)	
    {
			err++; 
			delay_1ms(400);
			if(sim7020_send_atcmd(AT_CMD_CSQ,"+CSQ:",2000)!=true)continue;
			uint8_t* last_data = (uint8_t*)sim_ctl.recv_buf;
			uint16_t data_len = 0;
			data_len = at_get_last_recv(&last_data);
			at_signal_cb((char*)last_data, data_len, NULL);	
    }
		err = 0;
		while(err < 5){
			err++;
			if(sim7020_send_atcmd(AT_CMD_CHECK_PS,"OK",1000)!=true)continue;	
			uint8_t* last_data = (uint8_t*)sim_ctl.recv_buf;
			uint16_t data_len = 0;
			data_len = at_get_last_recv(&last_data);
			at_net_con_cb((char*)last_data, data_len, NULL);
			if(!ls_con_net)
			{
				delay_1ms(400);
				continue;
			}
			else
			{
				return sim7020_start_udp_tcp(true, remote_ip, remote_port);
			}
			
		}
    return false;

}
/**
*@brief		ע���������
*@param     ��
*@note      ��
*@return	true: ע��ȫ���ɹ�
*			false: ע�����ʧ��
*/

static bool sim7020_register_cb()
{
	bool suc = true;
	if(!at_register_callback(AT_LISTEN_KEY_CSQ, at_signal_cb, NULL))suc = false;
	if(!at_register_callback(AT_LISTEN_KEY_NETCON, at_net_con_cb, NULL))suc = false;
	if(!at_register_callback(AT_LISTEN_KEY_SERVER, at_net_recv_cb, NULL))suc = false;
	
	return suc;	
}

/**
*@brief		NB�����ʼ��
*@param     ��
*@note      ��
*@return	true: �����ʼ���ɹ�
*			false: �����ʼ��ʧ��
*/

bool sim7020_deinit()
{
    printf("sim7020 deinit!\r\n");
    sim7020_pwr_off();
    at_parse_deinit();
    memset(&sim_ctl,0,sizeof(sim_ctl));	
		ls_init = false;
		ls_con_net = false;
		ls_start_udp_tcp = false; 
		ls_use_udp = false;
		ls_register_cb = false;
		net_watch_dog_cnt = 0;
		nb_reboot_done = false;
    return true;
}


/**
*@brief		����NBģ������
*@param     apn_mode�������ѡ�񣬴˴�Ϊ�ƶ�����ͨ����������ͬԤ�����ݶ�0Ϊ�ƶ�
*@note      ��
*@return	true: ���óɹ�
*			false: ����ʧ��
*/

bool sim7020_set_apn(uint8_t apn_mode)
{
	delay_1ms(1000);
	uint16_t err=0;
	if(apn_mode == 0)
	{
		while(err < 3)
		{
			err++; 
			delay_1ms(100);
//			if(sim7020_send_atcmd(AT_CMD_SET_APN,"OK",4000)!=true)continue;			
			return true;
		}
	}
	return false;
}

/**
*@brief		����NBģ��˯��ģʽ
*@param     sleep_mode��0 HIB2, 1 HIB1, 2 SLEEP2, 3 SLEEP1
*@note      ��
*@return	true: ���óɹ�
*			false: ����ʧ��
*/

bool sim7020_sleep(uint8_t sleep_mode)
{
    uint8_t err=0;

	ls_con_net = false;
	
    while (err < 3)
    {
        err++;
        delay_1ms(100);
        if(sim7020_send_atcmd(AT_SLEEP_NOW,"+POWERDOWN",1000)!=true)continue;
		return true;
	}
	return false;
}

/**
*@brief		����NBģ��
*@param     ��
*@note      ��
*@return	true: ���ѳɹ�
*			false: ����ʧ��
*/
bool sim7020_wakeup()
{
	bool lsWake;
	NB_WEAKUP_STOP();
	NB_WEAKUP_START();
	delay_1ms(3);
	lsWake = sim7020_pwr_on();
	NB_WEAKUP_STOP();
	
	return lsWake;
}

/**
*@brief		NB��ʼ��
*@param     ��
*@note      ��
*@return	true: ��ʼ���ɹ�
*			false: ��ʼ��ʧ��
*/
bool sim7020_init()
{
  uint8_t err=0;
  printf("E7025 init!\r\n");
	net_watch_dog_cnt = 0;
		
	memset(&sim_ctl,0,sizeof(sim_ctl));
	
	at_parse_init();
	
	if (!ls_register_cb)
	{
		ls_register_cb = sim7020_register_cb();		
	}
	

	while(!nb_reboot_done && err < 3)
	{	
		err++;
		delay_1ms(30);
 		if(sim7020_set_apn(0)!=true)continue;
		if(sim7020_send_atcmd(AT_CMD_GET_SOFTWARE_VERSION,"OK",1000)!=true)continue;
		delay_1ms(30);
		if(sim7020_send_atcmd(AT_CMD_NPOWER_CLASS_B3_6,"OK",1000)!=true)continue;
		delay_1ms(30);
		if(sim7020_send_atcmd(AT_CMD_NPOWER_CLASS_B5_6,"OK",1000)!=true)continue;
		delay_1ms(30);
		if(sim7020_send_atcmd(AT_CMD_NPOWER_CLASS_B8_6,"OK",1000)!=true)continue;
		delay_1ms(30);
		if(sim7020_send_atcmd(AT_CMD_NPOWER_CLASS_B20_6,"OK",1000)!=true)continue;
		delay_1ms(30);
		if(sim7020_send_atcmd(AT_CMD_POWER_CLASS_QUERY,"OK",1000)!=true)continue;
		delay_1ms(30);
		if(sim7020_send_atcmd(AT_CMD_CPSMS_QUERY,"OK",1000)!=true)continue;
		delay_1ms(30);
		if(sim7020_send_atcmd(AT_RESETCTL_20MS,"OK",1000)!=true)continue;
		delay_1ms(30);
		if(sim7020_send_atcmd(AT_RESETCTL_QUERY,"OK",1000)!=true)continue;
		delay_1ms(30);
//		if(sim7020_send_atcmd(AT_CLOSE_PSM,"OK",1000)!=true)continue;
//		delay_1ms(30);
		if(sim7020_send_atcmd(AT_CMD_NSONMI,"OK",1000)!=true)continue;
		delay_1ms(30);
		if(sim7020_send_atcmd(AT_CMD_CHECK_NSONMI,"OK",1000)!=true)continue;				
		nb_reboot_done = true;
		err = 0;		
	}
    while(err < 5)	
    {
        err++;
		delay_1ms(100);
 		if(sim7020_send_atcmd(AT_CMD_CLOSE_FLY_MODE,"OK",1000)!=true)continue;
		
		delay_1ms(100);	
 		if(sim7020_send_atcmd(AT_CMD_CPIN,"+CPIN:READY",1000)!=true)continue;
		  				
		ls_init = true;
        printf("E7025 init ok!\r\n");
        return true;
    }
		printf("E7025 init failed!\r\n");
    sim7020_deinit();
    return false;
}

/**
*@brief		����NB��Դģʽ
*@param     ls_sleep���Ƿ���ʡ��ģʽ
*@note      �������ʡ��ģʽ��ϵͳ��ʱ��������������ߣ�֮���ָ������Ӧ��
*           Ҳ���ܹ����ѣ�ֻ��ͨ����λ���������Դ˴�ע�ͣ�����ʡ��ģʽ
*@return	true: ���óɹ�
*			false: ����ʧ��
*/

bool sim7020_set_power(bool ls_sleep)
{
    uint8_t err=0;
    			
    while (err < 2)	
    {
        err++;


//			if(sim7020_send_atcmd(AT_IDEL_2_8mA,"OK",1000)!=true)continue;


				
        return true;
    }
    return false;
}

/**
*@brief		ΪNB����UDP��TCP�׽��֣�Ϊ��������������׼��
*@param     ls_udp���Ƿ�ʹ��UDP
*           ip��Զ�̵�ַ
*           port��Զ�̶˿�
*@note      ����һ�κ�δ�رգ��ڶ��ε��ûᴴ���µ��׽���
*@return	true: �����ɹ�
*			false: ����ʧ��
*/

bool sim7020_start_udp_tcp(bool ls_udp, char* ip, uint16_t port)
{
	if(false == ls_init)return false;
	uint8_t err=0;
	bool cmdSta;
	char* setSockCmd = (ls_udp) ? AT_SET_SOCK_UDP : AT_SET_SOCK_TCP;
	char conSockCmd[30] = {AT_CON_SOCK_TCP};

	strcpy(remote_ip, ip);
	remote_port = port;
	ls_use_udp = ls_udp;
		
	while(err < 5)
    {
			err++;
			delay_1ms(100);
			if(sim7020_send_atcmd(setSockCmd, "OK", 1000)!=true)continue;
			cmdSta = true;
			printf("E7025 set udp mode successful!\r\n");
			err = 0;
			break;
    }
	
	if(!ls_udp)
	{
		char conParamStr[8];
		strcat(conSockCmd, ip);
		strcat(conSockCmd, "\",");
		NUM_TO_CHAR(port, conParamStr);
		strcat(conSockCmd, conParamStr);
		
	}
	else
	{
		printf("E7025 set udp mode successful!..........................\r\n");
		ls_start_udp_tcp = true;
		return cmdSta;
	}
	
    while (err < 5)
    {
        err++;
        delay_1ms(100);
			if(sim7020_send_atcmd(conSockCmd, "OK", 1000)!=true)continue;
			ls_start_udp_tcp = true;
        return true;
    }
	printf("E7025 set udp mode failed!\r\n");
	ls_start_udp_tcp = false;
	return false;
}

/**
*@brief		����UDP��TCP���ݣ������粻�ã����Զ��ط�3��
*@param     pdata��Դ���ݣ�Ϊ16�����ַ�����ʽ
*           data_size�����ݴ�С����λ�ֽ�          
*@note      ��СС��1000�ֽ�
*@return	true: ���ͳɹ�
*			false: ����ʧ��
*/
bool sim7020_send_udp_tcp(char* pdata, uint32_t data_size)
{
	if(!ls_start_udp_tcp)return false;
//	if(!ls_con_net)return false;
	if(data_size > 1000)return false;
	
	uint8_t err=0;
	bool sendSta;
	uint32_t send_done_size = 0;
	uint32_t send_size = 0;
	char sendParamStr[10];
	static char send_buf[2700];
	
	uint16_t uart_one_size = 1300; //���Ϊż��
	char* send_data_pos = pdata;
	
	if (ls_use_udp)
	{
		NUM_TO_CHAR(remote_port, sendParamStr);
		strcpy(send_buf, AT_SEND_SOCK_UDP);
		strcat(send_buf, remote_ip);
		strcat(send_buf, "\",");
		strcat(send_buf, sendParamStr);
		strcat(send_buf, ",");
		NUM_TO_CHAR(data_size, sendParamStr);
		strcat(send_buf, sendParamStr);
		strcat(send_buf, ",\"");
		memset(send_buf + strlen(send_buf)+data_size*2,0,4);
		memcpy(send_buf + strlen(send_buf), pdata, data_size*2);
		send_buf[strlen(send_buf)] = '\"';
	}
	else
	{
		strcpy(send_buf, AT_SEND_SOCK_TCP);
	}
		
    do
    {
		if(data_size - send_done_size > uart_one_size)
		{
			send_size = uart_one_size;			
		}
		else
		{
			send_size = data_size - send_done_size;	
		}
		
		sendSta = at_send_wait_reply(send_buf, true,
							  "OK","ERROR", 
							  sim_ctl.recv_buf,
							  &sim_ctl.recv_len, 1500);
		
		if(sim7020_loss_handle() && net_get_ls_send_succ())return true;
	
		if(sendSta != true)
		{
			err++;
			if (err > 3)
			{
				printf("send failed!\r\n");
				return false;			
			}
			else
			{
				printf("send no respond %d \r\n", err);
				continue;				
			}
		}		
		err = 0;
		send_done_size += send_size;
		send_data_pos += (send_size*2);      
    }
	while(send_done_size < data_size);
	
	return true;
	
}

/**
*@brief		�ر�UDP��TCP�׽���
*@param     ��
*@note      ��
*@return	true: �رճɹ�
*			false: �ر�ʧ��
*/

bool sim7020_close_udp_tcp()
{
	uint8_t err=0;
	
    while(err < 3)
    {
        err++;       
        if(sim7020_send_atcmd(AT_CLOSE_SOCK,"OK",1000)!=true)continue;
				ls_start_udp_tcp = false;
        return true;
    }
	
	return false;
}

/**
*@brief		���մ�������NB�����ݣ����մ���Լ8��
*@param     ��
*@note      ��
*@return	��
*			
*/

void sim7020_handle()
{
	sim_ctl.recv_len = at_uart_receive(sim_ctl.recv_buf, 0, 8000);
	if(!sim_ctl.recv_len)
	{		
		net_watch_dog_cnt++;
		printf("network idle cnt is %d sec\r\n", net_watch_dog_cnt*10);

		if(net_watch_dog_cnt % 1 == 0 && communication_ls_debug())
		{
			char heart_buf[50];
			uint16_t data_len = 0;
			net_get_heart_data(heart_buf,&data_len);
			net_send_udp_tcp(heart_buf, data_len);
			sim7020_loss_handle();
		}
	}
	else
	{
		printf("start process server data\r\n");		
		if (at_parse_match_obj(sim_ctl.recv_buf, sim_ctl.recv_len))
		{
			net_watch_dog_cnt = 0;
		}	
	}
}

/**
*@brief		���մ�����һ��NB�����ݣ�Ӧ���ڷ��������ݺ�ģ�����ûظ�����ճ��һ������
*@param     ��
*@note      ��
*@return	��
*			
*/
bool sim7020_loss_handle()
{
	uint8_t* last_data = (uint8_t*)sim_ctl.recv_buf;
	uint16_t data_len = 0;
	data_len = at_get_last_recv(&last_data);
	if(data_len > 20)
	{
		printf("start process server loss data\r\n");
		at_parse_match_obj((char*)last_data, data_len);
		return true;
	}
	else
	{
		return false;
	}

}

/**
*@brief		ģ����Ժ���
*@param     ��
*@note      ��
*@return	��
*			
*/
void sim7020_test()
{
	//sim7020_send_atcmd(AT_CMD_CSQ,"+CSQ:",1000);
}

/**
*@brief		ģ������жϣ���Լ20s����δ���յ�����������ʱ���ж�ģ�����
*@param     ��
*@note      ��
*@return	true:ģ�����
*           false:ģ��ǿ���
*			
*/

bool sim7020_ls_idel()
{	
	return (net_watch_dog_cnt > 2) ? true : false;
}

/**
*@brief		ģ�����ʱ���ȡ
*@param     ��
*@note      ��
*@return	ģ�����ʱ������λ��s
*           
*			
*/

uint8_t sim7020_get_idel()
{
	return net_watch_dog_cnt;
}



