/********************************************************************************
* @file    		at_recv_cb.c
* @function     ��������NBģ�������
* @attention		
* @version    	V1.0
* @Author       WLQ
* @date       	2021/07/21
********************************************************************************/

#include "at_parse.h"
#include "net_protocol.h"
#include "string.h"
#include <stdio.h>

extern bool ls_con_net;

static char net_recv_buf[150];

/**
*@brief		16�����ַ���תԭ�ֽ�
*@param     src��Ҫת����Դ�����ַ���
*           dst��Ŀ�껺��������Ҫ�Լ���ǰ���ٿռ�
*           datalen�����ݳ��ȣ���λ�ֽ�      
*@note      ��
*@return	true: ת���ɹ�
*			
*/
static bool hexstr_to_byte(uint8_t* src, uint8_t* dst, uint16_t data_len)
{
	uint16_t byte_cnt = 0;
	uint8_t hex_h = 0;
	uint8_t hex_l = 0;
	while(*src) 
	{
        //sscanf(src, "%2X", dst);�˿⺯���������쳣
		if (src[0] > '9')hex_h = src[0] - 'A' + 10;
		else hex_h = src[0] - '0';
		if (src[1] > '9')hex_l = src[1] - 'A' + 10;
		else hex_l = src[1] - '0';
			
		dst[0] = (hex_h << 4) | hex_l;
        src += 2;
        dst++;
		byte_cnt++;
		if(byte_cnt >= data_len)break;
    }
	dst[0] = 0;
    return true;
}

/**
*@brief		�ź�ǿ�ȼ��ص�����
*@param     pdata��Դ�����ַ���
*           datalen�����ݳ��ȣ���λ�ֽ�
*           arg���û��ص�����      
*@note      ��
*@return	��
*			
*/
void at_signal_cb(char *pdata, uint16_t data_len, void *arg)
{
//	UNUSED(arg);
	uint16_t data_pos = 0;
	uint8_t net_csq = 0;
	if (data_len < 9) return;
	while(data_pos < data_len)
	{
		if(*pdata != ':')
		{
			data_pos++;
			pdata++;
		}
		else
		{
			pdata += 1;
			data_pos += 1;
			break;
		}
	}
	if (data_pos < data_len)
	{
		if(pdata[0] >= '0' && pdata[0] <= '9')
		{
			if (pdata[1] >= '0'&& pdata[1] <= '9')
			{
				net_csq = 10*(pdata[0]-'0') + (pdata[1]-'0');			
//				ls_con_net = (net_csq > 0 && net_csq <= 32);
				printf("RSSI is %d\r\n", net_csq);				
			}
			else if(pdata[1] == ',')
			{
				net_csq = (pdata[0]-'0');		
//				ls_con_net = (net_csq > 0 && net_csq <= 9);
				printf("RSSI is %d\r\n", net_csq);				
				
			}
			else
			{
				printf("CSQ format error!\r\n");
			}
		}
		else
		{
			printf("CSQ format error!\r\n");
		}
	}
}

/**
*@brief		������ص�����
*@param     pdata��Դ�����ַ���
*           datalen�����ݳ��ȣ���λ�ֽ�
*           arg���û��ص�����      
*@note      ��
*@return	��
*			
*/

void at_net_con_cb(char *pdata, uint16_t data_len, void *arg)
{
//	UNUSED(pdata);
//	UNUSED(arg);
	
	uint16_t data_pos = 0;
	uint8_t net_csq = 0;
	if (data_len < 9) return;
	while(data_pos < data_len)
	{
		if(*pdata != ',')
		{
			data_pos++;
			pdata++;
		}
		else
		{
			pdata += 1;
			data_pos += 1;
			break;
		}
	}
	debug_printf("data_pos = %d\r\n",data_pos);
	debug_printf("data_len = %d\r\n",data_len);
	debug_printf("pdata[0] = %d\r\n",pdata[0]);
	if (data_pos < data_len)
	{
		if(pdata[0] == '1' )
		{
			ls_con_net = true;
			printf("network ps succ!\r\n");
		}
		else
		{
			ls_con_net = false;
			printf("network ps failed!\r\n");
		}
	}
	
	
}

/**
*@brief		�������ݽ��ջص�����
*@param     pdata��Դ�����ַ���
*           datalen�����ݳ��ȣ���λ�ֽ�
*           arg���û��ص�����      
*@note      ��
*@return	��
*			
*/

void at_net_recv_cb(char *pdata, uint16_t data_len, void *arg)
{
	uint8_t data_pos = 0;
	uint16_t net_data_len = 0;
//	UNUSED(arg);
	if (data_len == 0)return;
	while(data_pos < data_len)
	{		
		if(memcmp(pdata,"+NSONMI",7) != 0){pdata++; data_pos++; continue;}
		else break;		
	}
	debug_printf("data_len = %d\r\n",data_len);
	debug_printf("data_pos = %d\r\n",data_pos);
	if(data_pos == data_len)
	{
		printf("analysis failed!\r\n");
		return;
	}
	data_pos = 0;
	while(data_pos < data_len)
	{
		if(pdata[data_pos++] != ',')continue;
		else break;		
	}
	sscanf(pdata + data_pos, "%d", (uint32_t*)&net_data_len);
	debug_printf("net_data_len = %d\r\n",net_data_len);
	while(data_pos < data_len)
	{
		if(pdata[data_pos++] != ',')continue;
		else break;
	}
	debug_printf("data_len = %d\r\n",data_len);
	debug_printf("data_pos = %d\r\n",data_pos);
	if(data_pos < data_len)
	{
		hexstr_to_byte((uint8_t*)pdata + data_pos, (uint8_t*)net_recv_buf, net_data_len);
		net_recv_handle(net_recv_buf, net_data_len);	
		printf("recv form server : %s", net_recv_buf);
	}
}


/**
*@brief		�������ݽ��ջص�����
*@param     pdata��Դ�����ַ���
*           datalen�����ݳ��ȣ���λ�ֽ�
*           arg���û��ص�����      
*@note      ��
*@return	��
*			
*/

void at_set_dataBuff_mode_cb(char *pdata, uint16_t data_len, void *arg)
{
	uint8_t data_pos = 0;
	uint16_t net_data_len = 0;
//	UNUSED(arg);
	if (data_len == 0)return;
	while(data_pos < data_len)
	{		
		if(memcmp(pdata,"+ECSONMI",8) != 0){pdata++; data_pos++; continue;}
		else break;		
	}
	
	if(data_pos == data_len)	
	{
		printf("analysis failed!\r\n");
		return;
	}
	data_pos = 0;
	while(data_pos < data_len)
	{
		if(pdata[data_pos++] != ',')continue;
		else break;		
	}
	sscanf(pdata + data_pos, "%d", (uint32_t*)&net_data_len);
	
	while(data_pos < data_len)
	{
		if(pdata[data_pos++] != '\"')continue;
		else break;
	}
	
	if(data_pos < data_len)
	{
		hexstr_to_byte((uint8_t*)pdata + data_pos, (uint8_t*)net_recv_buf, net_data_len);
		net_recv_handle(net_recv_buf, net_data_len);	
		printf("recv form server : %d", net_recv_buf);
	}
}

