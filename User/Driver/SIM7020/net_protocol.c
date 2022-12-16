/********************************************************************************
* @file    		net_protocol.c
* @function     监测服务器通信协议
* @attention		
* @version    	V1.0
* @Author       WLQ
* @date       	2021/07/21
********************************************************************************/

#include "net_protocol.h"
#include "communicate.h"
#include "dci_config.h"
#include "fmc_flash.h"
#include "crc16.h"
#include "string.h"
#include "service_logic.h"
#include "adc_config.h"
#include "rtc_config.h"
#include "dac_config.h"
#include "gpio_config.h"

//uint8_t sensor_id[] = {"11111111111111111111;"};
uint8_t sensor_id[] = {"10010002202203060001;"};
uint8_t remote_ip1[] = {"121.36.220.210"};
uint8_t sleep_step = LONG_SLEEP_WAIT;

static uint8_t use_probe_id[] = { DEFAULT_PROBE_ID };
static uint8_t debug_mode = DEBUG_MODE_DONE;
static char net_send_buf[NET_BUF_SIZE];
static uint16_t net_send_size = 0;
static bool net_ls_close = true;
static bool net_ls_send_succ = false;
static bool ls_sample_resp = false;
static bool ls_need_wait = true;
static char net_send_buf_hex[NET_PACK_SIZE];
static uint16_t sample_len = SENSOR_DATA_LEN;
static upload_data_t upload_data_info;

static uint16_t send_fail_cnt = 0;
static uint32_t sample_times=0;

static void sample_full_crc(upload_data_t* upload_data);



/**
*@brief		字节转16进制字符串
*@param     src：源数据
*           dst：目标缓存区，需要用户提前好开辟空间
*           data_len：预计接收的数据长度，单位字节
*@note      无
*@return	无
*			
*/

static void byte_to_hexstr(const uint8_t* src, uint8_t* dst, uint16_t data_len)
{
	uint16_t i = 0;
	while(i < data_len)
	{
		sprintf((char*)dst,"%02X",src[i]);	
		i++;
		dst += 2;
	}
}

/**
*@brief		字节转16进制字符串
*@param     src：源数据
*           dst：目标缓存区，需要用户提前好开辟空间
*           data_len：预计接收的数据长度，单位字节
*@note      无
*@return	无
*			
*/
bool net_send_udp_tcp(const char* pdata, uint16_t data_len)
{
	//发送成功状态复位
	net_set_send_sta(false);
	//待发送数据转换为16进制字符串
	byte_to_hexstr((uint8_t*)pdata, (uint8_t*)net_send_buf_hex, data_len);
	//执行发送
	return sim7020_send_udp_tcp(net_send_buf_hex, data_len);
}

/**
*@brief		填充传感器ID
*@param     start_data：起始地址
*@note      无
*@return	无
*			
*/
static void full_sensor_id(uint8_t* start_data)
{
	memcpy(start_data, sensor_id, 21);	
}


/**
*@brief		填充上传波形包数据头信息
*@param     upload_data：信息头地址
*@note      无
*@return	无
*			
*/
static void full_updata_head_info(upload_data_t* upload_data,uint8_t gain, uint8_t ch)
{
	uint16_t temp_data = 0;
	
	//填充探头编号
	strcpy((char*)(upload_data->probe_num), (char*)use_probe_id);
	upload_data->probe_num[3] = ';';
	
	//获取增益
	upload_data->gain[0] = gain;
	upload_data->gain[1] = ';';
	
	//获取电池
	upload_data->battery[0] = BatGetPrecent();
	upload_data->battery[1] = ';';
	
	//获取平均等级		
	upload_data->average[0] = 10;
	upload_data->average[1] = ';';
	
	//获取温度
//	temp_data = ThermGetLastTemp();
//	temp_data = ThermGetLastTemp();
	debug_printf("temperature = %d\r\n",temp_data);
	upload_data->temperature[0] = temp_data >> 8;
	upload_data->temperature[1] = temp_data;
	upload_data->temperature[2] = ';';
	
	//填充长度
	upload_data->data_len[0] = sample_len >> 8;
	
	
	
	 
	upload_data->data_len[1] = sample_len;
	upload_data->data_len[2] = ';';
	
}

/**
*@brief		回复服务器命令
*@param     无
*@note      无
*@return	回复成功或失败
*			
*/
static bool net_response_operation()
{
	net_set_close_sta(false);
	net_send_udp_tcp(net_send_buf, net_send_size);
	ls_sample_resp = false;
	if(debug_mode == DEBUG_MODE_DONE)
	{
		net_set_send_sta(true);
	}
	return true;
}

/**
*@brief		计算填充采样CRC
*@param     upload_data：上传数据结构体地址
*@note      无
*@return	无
*			
*/

static void sample_full_crc(upload_data_t* upload_data)
{
	uint16_t crc_temp = 0;	
	upload_data->data_crc[2] = ';';
	crc_temp = crc16_con((char*)upload_data, sizeof(upload_data_t)-3, 0);
	crc_temp = crc16_con((char*)";", 1, crc_temp);
	
	if (upload_data->data_len[0] || upload_data->data_len[1])
	{
		uint16_t* sample_data = get_sample_data();
		crc_temp = crc16_con((char*)(sample_data), sample_len * 2, crc_temp);		
	}
	else
	{		
		crc_temp = crc16_con((char*)0, 1, crc_temp);
	}
	upload_data->data_crc[0] = crc_temp >> 8;
	upload_data->data_crc[1] = crc_temp;

	//test print
	debug_printf("CRC is:%04X\r\n", crc_temp);
}



//注意data_len不是字节数，是 uint16_t 数据长度
void short_swap_byte(uint16_t* pdata, uint16_t data_len)
{
	uint16_t i=0;
	uint8_t temp_data = 0;
	
	for (i=0;i < data_len;i++)
	{
		temp_data = pdata[i] >> 8;
		pdata[i] <<= 8;
		pdata[i] |= temp_data;
	}
}
/**
*@brief		采样回复
*@param     ls_retra：是否重发，
*           ls_one_pack：是否单包重发
*           part：重发的单包序号数，大于0
*           ch 通道号，小于MAX_PROBE_NUM
*         ------------------------------------------------------
*           ls_retra   ls_one_pack   组合行为
*           true       true          重发第 part 包
*           true       false         重发所有包
*           false      任意          执行一次采样并完整发送一次
*         -------------------------------------------------------
*@note      无
*@return	是否采样回复成功
*			
*/

bool net_response_sample(bool ls_retra, bool ls_one_pack, uint8_t part, uint8_t ch)
{
	
	uint16_t temp_data = 0;
	uint16_t header_size = sizeof(upload_data_t);
	uint8_t pack_cnt = 1;
	static uint8_t pack_num = 1;  
	uint16_t pack_len = 420;
	uint16_t send_len_cnt = 0;
	static uint16_t* sample_data = NULL;
	bool sample_res = false;
	uint16_t* send_data = NULL;
	
		uint32_t * probeGainAvg = 0;
	uint8_t gain = 0;

	ls_sample_resp = true;
	upload_data_t* upload_data = (upload_data_t*)net_send_buf;
	net_set_close_sta(false);
	net_set_send_sta(false);

//	probeGainAvg = readFlash_X_Word(CHANNALE_PRAREMETER,16);
	gain = *(probeGainAvg+2*ch);
	set_gain(gain);
	if(!ls_retra)			
	{	
		//启动采样
		powerOn();
		sample_start(&oriutg_data,8);
		

		strcpy((char*)(upload_data->source),RESP_SOURCE);
		strcpy((char*)(upload_data->header), RESP_HEADRE_UPLOAD);
		
		//填充sensor_id
		full_sensor_id(upload_data->sensor_id);
			
		pack_num = sample_len / pack_len + (1&&(sample_len % pack_len));
		upload_data->part_number[0] = pack_num;
		upload_data->part_number[1] = ';';
		upload_data->part[1] = ';';

		sample_res = get_sample_flag();
		if (!sample_res)
		{						
			sample_data = get_sample_data();
			debug_printf("sample_data addr : %p\r\n",sample_data);
			short_swap_byte(sample_data,sample_len);
			upload_data->upload_ack[0] = (debug_mode == DEBUG_MODE_DONE) ? \
			RESP_OPERATION_AUTO_SAMPLE_NOW : RESP_OPERATION_DEBUG_SAMPLE_NOW;
			debug_printf("get sample data succ!\r\n");
		}
		
		//获取其他信息
		full_updata_head_info(upload_data,gain, ch);
		powerOff();
		upload_data->upload_ack[1] = ';';
		send_data = sample_data;
		
		//填充CRC
		sample_full_crc(upload_data);
		debug_printf("sample_full_crc!\r\n");
		//备份波形信息头，用于重发
		memcpy(&upload_data_info, upload_data, sizeof(upload_data_t));
		debug_printf("memcpy!\r\n");				
		if (sample_res)
		{
			upload_data->upload_ack[0] = (debug_mode == DEBUG_MODE_DONE) ? \
			RESP_OPERATION_AUTO_SAMPLE_NULL : RESP_OPERATION_DEBUG_SAMPLE_NULL;
			debug_printf("get sample data failed!\r\n");
			//
			upload_data->part_number[0] = 1;
			upload_data->part[0] = 1;
			upload_data->data_len[0] = 0;
			upload_data->data_len[1] = 0;
			
			//填充空数据
			net_send_size = header_size;
			((uint8_t*)upload_data->source)[net_send_size] = ';';
			((uint8_t*)upload_data->source)[net_send_size + 1] = 0;
			net_send_size += 2;
			
			//填充CRC
			sample_full_crc(upload_data);		
									
			//发送
			net_send_udp_tcp((char*)(upload_data->source), net_send_size);
			ls_sample_resp = false;
			memcpy(&upload_data_info, upload_data, sizeof(upload_data_t));
			return false;
		}
		
	}

	

	while(pack_cnt <= pack_num)
	{	
		//note
		if(net_get_ls_send_succ())return true;	
		
		if(sample_len - send_len_cnt > pack_len)
		{
			temp_data = pack_len;
		}
		else
		{
			temp_data = sample_len - send_len_cnt;
		}
		send_len_cnt += temp_data;
		
		if (1==pack_cnt)
		{
			send_data = sample_data;
			memcpy(upload_data, &upload_data_info, sizeof(upload_data_t));			
			memcpy(upload_data->wave_data, send_data, temp_data * 2);
			net_send_size = header_size + temp_data * 2;
		}
		else
		{
			memcpy(upload_data->gain, send_data, temp_data * 2);
			net_send_size = header_size + temp_data * 2 - 15;
		}
		
		send_data += temp_data;
		upload_data->part[0] = pack_cnt;
		
		pack_cnt++;
		//发送
		debug_printf("pack_cnt!\r\n");
		net_send_udp_tcp((char*)(upload_data->source), net_send_size);
		
	}
	return true;
}
/**
*@brief		解析网络接收的数据并更新传感器参数
*@param     pdata：数据，从探头编号开始          
*@note      无
*@return	是否处理成功
*			
*/

static bool net_set_sensor_param(uint8_t* pdata)
{
	uint16_t ch = 100 * (pdata[0]-'0') + 10* (pdata[1]-'0') + (pdata[2]-'0') - 1;
	if (ch >= MAX_PROBE_NUM)
		return false;
	pdata += 3;
	

	mult_sensorParam[ch].gain_db = pdata[0];
	mult_sensorParam[ch].avg_time = pdata[1];
	mult_sensorParam[ch].wave_num = pdata[2];
	mult_sensorParam[ch].tx_frq_MHz =pdata[3];
	mult_sensorParam[ch].sample_depth = pdata[4];
	mult_sensorParam[ch].rep_frq_Hz = pdata[5]<<8 | pdata[6];
	
	debug_printf("*************SET PARAM******************\r\n");	
	
	debug_printf("CH%d,gain:%d,avg:%d,wave_num:%d,frea:%d,depth:%d,freq_repeat:%d\r\n",
	ch, mult_sensorParam[ch].gain_db, mult_sensorParam[ch].avg_time,
	mult_sensorParam[ch].wave_num, mult_sensorParam[ch].tx_frq_MHz,
	mult_sensorParam[ch].sample_depth, mult_sensorParam[ch].rep_frq_Hz);

//	return WriteFlashSampleParam();
}

/**
*@brief		解析字符串得到要采样的通道
*@param     listStr：通道号字符串，3个字节一组为通道号
*           dstList：通道号解析结果，一个字节一个通道号
*           uint8_t：通道数
*           
*@note      无
*@return	是否解析成功
*			
*/

static bool GetChannelList(const uint8_t* listStr, uint8_t* dstList, uint8_t listLen)
{
	char* probeStr = (char*)listStr;
	if (strlen(probeStr) < listLen * 3)
		return false;
	for (uint8_t i=0; i < listLen; i++, probeStr += 3)
	{
		dstList[i] = (probeStr[0] - '0') * 100 + (probeStr[1] - '0') * 10 + (probeStr[2] - '0');
		debug_printf("Ready channel:%d\r\n",dstList[i]);
	}
	return true;
}

/**
*@brief		解析字符串得到要采样的通道，并执行采样
*@param     listCmdStr：通道号字符串，3个字节一组为通道号
*           listLen：通道号解析结果，一个字节一个通道号
*          
*           
*@note      无
*@return	是否采样成功
*			
*/

static bool StartSampleList(const uint8_t* listCmdStr, uint8_t listLen)
{
	if (listLen > MAX_PROBE_NUM) 
		return false;
	uint8_t channelList[MAX_PROBE_NUM];
	if (GetChannelList(listCmdStr, channelList, listLen))
		return StartSampleUpload(0, channelList, listLen);
	return false;
}

/**
*@brief		服务器命令处理
*@param     operation_type：动作类型，
*           pdata：数据
*@note      无
*@return	是否处理成功
*			
*/

static bool net_operation_handle(uint8_t operation_type, uint8_t* pdata)
{
	uint16_t temp_data = 0;
		uint32_t * probeGainAvg = NULL;
	
	static ip_prot_t* temp_ip ={0}; //temp_ip[5]存放ip_port;
	
	switch (operation_type)
	{
		case RECV_OPERATION_CLOSE:
			debug_printf("send to server OK and no cmd!\r\n");
			if (debug_mode == DEBUG_MODE_DONE)
			{
				net_set_close_sta(true);
			}
//			net_set_wait_sta(false);
			//执行命令
			
		break;
		case RECV_OPERATION_SET:
			
			//执行命令
			//回复SET命令
			strcpy((char*)net_send_buf,RESP_SOURCE);
			strcat((char*)net_send_buf, RESP_HEADRE_SET_ACK);
			net_send_size = strlen((char*)net_send_buf);
							
			//填充sensor_id
			full_sensor_id((uint8_t*)(net_send_buf + net_send_size));		
			net_send_size += 21;
		
			//填充探头编号
			net_send_buf[net_send_size] = pdata[2];
			net_send_buf[net_send_size+1] = pdata[3];
			net_send_buf[net_send_size+2] = pdata[4];
			net_send_buf[net_send_size+3] = ';';		
			net_send_size += 4;

			net_send_buf[net_send_size] = net_set_sensor_param(&pdata[2]) ? RESP_OPERATION_SET_SUCC : RESP_OPERATION_SET_FAIL;
			net_send_buf[net_send_size+1] = ';';
			net_send_size += 2;

		//解析通道号
			temp_data = 100 * (pdata[2]-'0') + 10* (pdata[3]-'0') + (pdata[4]-'0');
			
//			probeGainAvg = readFlash_X_Word(CHANNALE_PRAREMETER,16);
			debug_printf("probeGainAvg addr : %p\r\n",probeGainAvg);
			//擦除页

			//更改参数
		
				sensorParam[temp_data- 1].gain = pdata[5];
				sensorParam[temp_data- 1].avg = pdata[6];
				debug_printf("probenum: %d\r\n",temp_data);
				debug_printf("GAIN: %d\r\n",pdata[5] );
				debug_printf("AVG: %d\r\n",pdata[6]);
			*(probeGainAvg+2*(temp_data - 1)) = sensorParam[temp_data - 1].gain;
			*(probeGainAvg+2*(temp_data - 1)+1) = sensorParam[temp_data - 1].avg;		
				debug_printf("GAIN1: %d\r\n",*(probeGainAvg+2*(temp_data - 1)) );
				debug_printf("AVG1: %d\r\n",*(probeGainAvg+2*(temp_data - 1)+1));			
			//将接收到的参数写回
			for(uint8_t i = 0;i < MAX_PROBE_NUM; i++ ){
				sensorParam[i].gain = *(probeGainAvg+2*i);
				sensorParam[i].avg = *(probeGainAvg+2*i+1);
				debug_printf("GAIN: %d\r\n",sensorParam[i].gain );
				debug_printf("AVG: %d\r\n",sensorParam[i].avg);
			}
//		if(writeFlash_X_Word(CHANNALE_PRAREMETER,MAX_PROBE_NUM*2,probeGainAvg)){
//			debug_printf("\r\nset gain succeed\r\n");
//		}else{
//			debug_printf("\r\nset gain failed\r\n");
//		}
			net_response_operation();
		break;
		case RECV_OPERATION_SET_TIME:
			pdata+=2;
			sample_time.hour = 24 * pdata[0] + pdata[1];//天*24+小时
			sample_time.min = pdata[2];//分
			sample_time.sec = 0;//秒
			sample_time.flag = 0xaaaa;
			sample_time.wakeup_count = 0;
//			writeFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sample_time)/sizeof(uint32_t),(uint32_t*) &sample_time);
			debug_printf("sample time : %02d:%02d:%02d\r\n",sample_time.hour, sample_time.min, sample_time.sec);
			debug_printf(" sample_time.flag %x\r\n",sample_time.flag);
			debug_printf(" sample_time.hour %d\r\n",sample_time.hour);
			debug_printf(" sample_time.min  %d\r\n",sample_time.min);
			debug_printf(" sample_time.sec  %d\r\n",sample_time.sec);
			
//			//清除采样计时
//		sample_set_signal(false);
			//回复GET_TIME命令
			strcpy((char*)net_send_buf,RESP_SOURCE);
			strcat((char*)net_send_buf, RESP_HEADRE_SET_TIME_ACK);
			net_send_size = strlen((char*)net_send_buf);
			//填充sensor_id
			full_sensor_id((uint8_t*)(net_send_buf + net_send_size));		
			net_send_size += 21;
		
			//填充探头编号		
		
			net_send_buf[net_send_size] = '0';
			net_send_buf[net_send_size+1] = '0';
			net_send_buf[net_send_size+2] = '1';
			net_send_buf[net_send_size+3] = ';';		
			net_send_size += 4;

			net_send_buf[net_send_size] = RESP_OPERATION_SET_TIME_SUCC;
			net_send_buf[net_send_size+1] = ';';
			net_send_size += 2;
			//发送
			net_response_operation();
		
		break;
		case RECV_OPERATION_GET:
			
			//解析通道号
			temp_data = 100 * (pdata[2]-'0') + 10* (pdata[3]-'0') + (pdata[4]-'0') - 1;

			//回复GET命令
			strcpy((char*)net_send_buf,RESP_SOURCE);
			strcat((char*)net_send_buf, RESP_HEADRE_GET_ACK);
			net_send_size = strlen(net_send_buf);
			//填充sensor_id
			full_sensor_id((uint8_t*)(net_send_buf + net_send_size));		
			net_send_size += 21;
		
			//填充探头编号
			net_send_buf[net_send_size] = pdata[2];
			net_send_buf[net_send_size+1] = pdata[3];
			net_send_buf[net_send_size+2] = pdata[4];
			net_send_buf[net_send_size+3] = ';';		
			net_send_size += 4;
		
			if (temp_data > MAX_PROBE_NUM)
			{
				net_send_buf[net_send_size] = RESP_OPERATION_POLL_NULL;
				temp_data = 0;
			}
			else
			{
				net_send_buf[net_send_size] = RESP_OPERATION_POLL_NOW;
			}
			
			net_send_buf[net_send_size+1] = ';';
		
			//模拟获取增益
			net_send_size += 2;
			uint32_t * probeGainAvg = 0;
			uint8_t gain = 0;
//			probeGainAvg = readFlash_X_Word(CHANNALE_PRAREMETER,16);
			gain = *(probeGainAvg+2*temp_data);
			net_send_buf[net_send_size] = gain;
			net_send_buf[net_send_size+1] = ';';
			net_send_buf[net_send_size+2] = mult_sensorParam[temp_data].avg_time;
			net_send_buf[net_send_size+3] = ';';	
			net_send_buf[net_send_size+4] = mult_sensorParam[temp_data].wave_num;
			net_send_buf[net_send_size+5] = ';';	
			net_send_buf[net_send_size+6] = mult_sensorParam[temp_data].tx_frq_MHz;
			net_send_buf[net_send_size+7] = ';';	
			net_send_buf[net_send_size+8] = mult_sensorParam[temp_data].sample_depth;
			net_send_buf[net_send_size+9] = ';';	
			net_send_buf[net_send_size+10] = mult_sensorParam[temp_data].rep_frq_Hz >> 8;
			net_send_buf[net_send_size+11] = mult_sensorParam[temp_data].rep_frq_Hz;
			net_send_buf[net_send_size+12] = ';';	
		
			net_send_size += 13;
//			net_send_buf[net_send_size] = sample_time.hour/24;
//			net_send_buf[net_send_size+1] = sample_time.hour%24;	
//			net_send_buf[net_send_size+2] = sample_time.min;
			net_send_buf[net_send_size+3] = ';';	
			net_send_size += 4;
				
			//发送
			net_response_operation();
			//执行命令

		break;
		case RECV_OPERATION_GET_DATA:
			//回复GET_DATA命令，开启采样获取数据
		    StartSampleList(&pdata[3],pdata[2]);
		break;
		case RECV_OPERATION_DEBUG:
			//回复DEBUG命令，开启长连接调试
			strcpy(net_send_buf,RESP_SOURCE);
			strcat(net_send_buf, RESP_HEADRE_DEBUG_ACK);
			net_send_size = strlen(net_send_buf);
			//填充sensor_id
			full_sensor_id((uint8_t*)(net_send_buf + net_send_size));	
			net_send_size += 21;
		
			if(pdata[2])
			{
				net_send_buf[net_send_size] = RESP_OPERATION_DEBUG_OPEN_SUCC;	
				debug_mode = DEBUG_MODE_RUN;
				net_set_close_sta(false);
				sim7020_set_power(false);
//				WriteFlashDebugParam(true);
				sleep_step = LONG_SLEEP_WAIT;
				debug_printf("network start debug!\r\n");
			}
			else
			{
				net_send_buf[net_send_size] = RESP_OPERATION_DEBUG_CLOSE_SUCC;
				debug_mode = DEBUG_MODE_DONE;				
				debug_printf("network stop debug!\r\n");
			}
			net_send_buf[net_send_size+1] = ';';
			net_send_size += 2;			
			//发送		
			net_response_operation();
			//执行命令	
			if(!pdata[2]) net_set_close_sta(true);
		break;
		case RECV_OPERATION_LONG_SLEEP:	

		
		break;
		case RECV_OPERATION_UPDATE_IP_PORT:

//			if(pdata[2] == 1){
//				
//			}else{
				temp_ip->flag = 0xaaaaaaaa;
				temp_ip->ip[0] = pdata[3];
				temp_ip->ip[1] = pdata[4];
				temp_ip->ip[2] = pdata[5];
				temp_ip->ip[3] = pdata[6];
				temp_ip->prot = pdata[7]<<8;
				temp_ip->prot |= pdata[8];
				debug_printf("ip:%d\r\n",pdata[3]);
				debug_printf("ip:%d\r\n",pdata[4]);
				debug_printf("ip:%d\r\n",pdata[5]);
				debug_printf("ip:%d\r\n",pdata[6]);
				debug_printf("temp_Port:%d\r\n",pdata[7]);
				debug_printf("temp_Port:%d\r\n",pdata[8]);
				strcpy((char*)net_send_buf,RESP_SOURCE);
				strcat((char*)net_send_buf, RESP_EEADRE_UPDATE_IP_ACK);
				net_send_size = strlen((char*)net_send_buf);
								
				//填充sensor_id
				full_sensor_id((uint8_t*)(net_send_buf + net_send_size));		
				net_send_size += 21;
				net_send_buf[net_send_size] = RESP_OPERATION_UPDATE_IP_SUCCEED;	
				net_set_close_sta(false);
				sim7020_set_power(false);
				debug_printf("change ip and prot!\r\n");
				net_send_buf[net_send_size+1] = ';';
				net_send_size += 2;			
				//发送		
				write_ip_prot_to_flash(SENSOR_PARAMETER_ADDR,temp_ip);

				net_response_operation();
//			}
		break ;
		default:
			break;
			
	}
	return true;
}

/**
*@brief		获取调试状态
*@param     无
*@note      无
*@return	调试状态：调试中，等待调试，或者调试完成
*			
*/

uint8_t net_get_debug_status()
{
	return debug_mode;
}

/**
*@brief		重置调试状态为等待调试
*@param     无
*@note      无
*@return	调试状态：
*           调试中     DEBUG_MODE_RUN，
*           等待调试   DEBUG_MODE_CHECK
*           调试完成   DEBUG_MODE_DONE			
*/

void net_reset_debug_status()
{
	debug_mode = DEBUG_MODE_CHECK;
}

/**
*@brief		获取是否需要等待
*@param     无
*@note      无
*@return	是否发送成功
*			
*/

inline bool net_get_ls_need_wait()
{
	return ls_need_wait;
}

/**
*@brief		设置是否需要等待
*@param     ls_wait：是否需要等待
*@note      无
*@return	无
*			
*/

void net_set_wait_sta(bool ls_wait)
{
	ls_need_wait = ls_wait;
}

/**
*@brief		获取是否发送成功
*@param     无
*@note      无
*@return	是否发送成功
*			
*/

bool net_get_ls_send_succ()
{
	return net_ls_send_succ;
}

/**
*@brief		设置发送状态
*@param     ls_succ：是否发送成功
*@note      无
*@return	无
*			
*/

void net_set_send_sta(bool ls_succ)
{
	net_ls_send_succ = ls_succ;
}

/**
*@brief		读取网络状态
*@param     无
*@note      无
*@return	是否可以关闭连接
*			
*/
bool net_get_ls_close()
{
	return net_ls_close;
}

/**
*@brief		设置网络状态
*@param     ls_close：是否关闭网络连接
*@note      无
*@return	无
*			
*/

void net_set_close_sta(bool ls_close)
{
	if(!ls_close)
	{
		debug_printf("network connect keep!\r\n");
	}
	else
	{
		debug_printf("network connect close!\r\n");
		debug_mode = DEBUG_MODE_DONE;
		net_send_size = 0;
		net_ls_send_succ = true;
		ls_sample_resp = false;		
	}
	net_ls_close = ls_close;
}

/**
*@brief		获取 debug请求包的数据
*@param     pdata：输出数据，需要提前开辟好空间，需约50字节
*           data_len：输出数据长度，单位字节
*@note      无
*@return	无
*			
*/

void net_get_debug_data(char* pdata, uint16_t* data_len)
{
	strcpy(pdata,RESP_SOURCE);
	strcat(pdata, REQU_HEADRE_DEBUG);
	net_send_size = strlen(pdata);
	
//	//填充sensor_id
	full_sensor_id((uint8_t*)(pdata + net_send_size));	
	net_send_size += 21;
	pdata[net_send_size] = RESP_OPERATION_REQ_DEBUG;
	pdata[net_send_size+1] = ';';
	net_send_size+=2;
	
	*data_len = net_send_size;
}

/**
*@brief		获取调试模式中心跳的发送包数据
*@param     pdata：输出数据，需要提前开辟好空间，需约50字节
*           data_len：输出数据长度，单位字节
*@note      无
*@return	无
*			
*/
void net_get_heart_data(char* pdata, uint16_t* data_len)
{
	strcpy((char*)pdata,RESP_SOURCE);
	strcat((char*)pdata, HEADRE_BEAT);
	*data_len = strlen((char*)pdata);
	
	//填充sensor_id
	full_sensor_id((uint8_t*)(pdata + (*data_len)));	
	(*data_len) += 21;
	pdata[(*data_len)] = REQ_OPERATION_HEART_DEBUG;
	pdata[(*data_len)+1] = ';';		
	(*data_len)+=2;
	
}
/**
*@brief		重发处理
*@param     无
*@note      此函数不做接收回复检查，需要自己另外加接收处理函数 sim7020_handle() 检查，否则一直调用会一直重发
*@return	重发成功或者失败
*			
*/

bool net_retra_handle()
{
	if(net_get_ls_send_succ())return true;
	
	if (!ls_sample_resp)
	{
		return net_send_udp_tcp(net_send_buf, net_send_size);	
	}
	else
	{
		return net_response_sample(true,false,1,0);
	}
	
}

/**
*@brief		网络数据接收处理
*@param     pdata：接收的数据
*           data_len：数据长度
*@note      无
*@return	是否处理成功
*			
*/

bool net_recv_handle(char* pdata, uint16_t data_len)
{
	net_set_close_sta(false);
	net_set_wait_sta(true);

	if (pdata[0])
	{
		//上一条指令发送成功并得到了正确回复，即发送成功
		net_set_send_sta(true);
		//处理新指令
		return net_operation_handle(pdata[1], (uint8_t*)pdata);	
	}
	else
	{
		//上一条指令发送成功但是得到了错误回复，需要重新发送
		net_set_send_sta(false);
		//重发一次
		if(!ls_sample_resp)
		{
			net_retra_handle();				
		}
		return true;
	}

}

/**
*@brief		修改探头编号
*@param     probe_dex：探头序号，范围 1 到 MAX_PROBE_NUM
*@note      无
*@return	是否处理成功
*			
*/

bool net_set_probeID(uint8_t probe_dex)
{
	sprintf((char*)use_probe_id, "%03d", probe_dex);
	return true;
}



/*
channelList[] 采样通道号列表，有效值大于等于1，小于等于MAX_PROBE_NUM
*/
bool StartSampleUpload(uint8_t reTryTimes, const uint8_t channelList[], uint8_t channelNum)
{
	uint8_t curProbe = 0;
	uint8_t send_retry_cnt = 0;
	uint8_t failedCnt = 0;
		
	while (curProbe < MAX_PROBE_NUM && curProbe < channelNum)
	{
		debug_printf("Probe channel start %d \r\n", channelList[curProbe]);
		net_set_probeID(channelList[curProbe]);
		net_response_sample(false, false, 1, channelList[curProbe] - 1);
		curProbe++;
		sim7020_loss_handle();
		
		//执行发送不成功重发5次
		do
		{
			sim7020_handle();
			if (send_retry_cnt >= reTryTimes)
				break;
			net_retra_handle();									
			send_retry_cnt++;
		}
		while(!net_get_ls_close());
		
		//重试大于等于5次未收到服务器回复，则可能发送失败
		
		sample_times++;		
		if(send_retry_cnt >= reTryTimes)
		{
			send_fail_cnt++;
			failedCnt++;			
		}
	}
	return (bool)failedCnt;
}
