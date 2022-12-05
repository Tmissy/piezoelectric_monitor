/********************************************************************************
* @file    		at_parse.c
* @function     NBģ��������շ��м��
* @attention		
* @version    	V1.0
* @Author       
* @date       	2021/07/21
********************************************************************************/


#include <stdbool.h>
#include <string.h>
#include "at_parse.h"
#include "usart_config.h"
#include "systick.h"

#define MAX_OBJ_COUNT 5
#define MAX_RECV_BUF_LEN    200


typedef struct 
{
    char *prefix;
    at_recv_cb callback;
    void *arg;
}at_obj_t;

typedef struct 
{
    at_obj_t obj[MAX_OBJ_COUNT];
    uint16_t recv_counter;
}at_parse_t;



static bool is_inited=false;
static at_parse_t at;

/**
*@brief		��������NB�����ݣ����������ע�ắ��
*@param     pdata��������Դ���� 
*           datalen�����ݳ��ȣ���λ�ֽ�      
*@note      ��Ҫ�ڴ˺����µ���sim7020_handle(),����sim7020_loss_handle()��������������޻ص�
*@return	true: ƥ�䵽����ע�ắ��
*			false: δƥ�䵽����ע�ắ��
*/
bool at_parse_match_obj(char *pdata,uint16_t datalen)
{
	debug_printf("datalen:%d, str:(%s)\r\n",datalen, pdata);
    for(uint8_t i=0;i<MAX_OBJ_COUNT;i++)
    {
        if(strstr(pdata,at.obj[i].prefix)!=NULL)
        {
            at.obj[i].callback(pdata,datalen,at.obj[i].arg);
            return true;
        }
		else if(at.obj[i].prefix)
		{
			printf("Not find key %s\r\n", at.obj[i].prefix);
		}
		else
		{
			printf("listen key[%d] is NULL\r\n",i);	
		}
    }
	printf("match failed\r\n");	
    return false;
}

/**
*@brief		AT�շ���ʼ��
*@param     ��   
*@note      ��
*@return	true: ��ʼ���ɹ�
*			false: ��ʼ��ʧ��
*/

bool at_parse_init(void)
{
    if(is_inited)return true;
    printf("at parse init");
		
    memset(&at,0,sizeof(at));
    do
    {
        if(at_uart_init()==false)break;
        is_inited=true;
        return true;
    } while (0);
    
    at_parse_deinit();
    return false;
}

/**
*@brief		AT�շ������ʼ��
*@param     ��   
*@note      ��
*@return	true: �����ʼ���ɹ�
*			false: �����ʼ��ʧ��
*/
bool at_parse_deinit(void)
{
    is_inited=false;
    printf("at parse deinit");
    at_uart_deinit();
    memset(&at,0,sizeof(at));
    return true;
}

/**
*@brief		����ATָ��ȴ��ظ�
*@param     cmd��ָ������
*           delimiter���Ƿ�ӻس����У����Ƿ��������ָ��
*           right_prefix����ȷ�ظ��ؼ���
*           fail_prefix������ظ��ؼ���
*           reply_buf���ظ����ݻ�������ַ����Ҫ����ǰ�Լ����ٺ��¿ռ�
*           reply_size���ظ����ݻ�������С
*           timeout���ȴ���ʱʱ�䣬��λms
*@note      ��
*@return	true: �����ɹ�
*			false: ����ʧ��
*/
bool at_send_wait_reply(const char *cmd,bool delimiter,
                        char *right_prefix,char *fail_prefix,
                        char *reply_buf, uint16_t *reply_size,
                        uint32_t timeout)
{
    bool result=false;
		uint32_t timeout_cnt = 0;
    if(is_inited==false)return false;
    if(cmd==NULL)return false;
    if(right_prefix==NULL)return false;

	//send data
	printf("AT cmd:%s\r\n", (char*)cmd);
	if(at_uart_send(cmd,strlen(cmd),timeout)!=strlen(cmd))return false;
	if(delimiter)
	{
		if(at_uart_send(AT_DELIMITER,strlen(AT_DELIMITER),timeout)!=strlen(AT_DELIMITER))return false;    
	}
    do
    {
    //wait recv
		timeout_cnt += 200;
		*reply_size = at_uart_receive(reply_buf, 0, 200);
		if(*reply_size)
		{
			if(strstr(reply_buf,right_prefix) != NULL)
			{
				result = true;
				break;
			}
		}
		else
		{
			result = false;
		}
        
    } while (timeout_cnt < timeout);
    
    return result;
}

/**
*@brief		����ATָ����ȴ��ظ�
*@param     data��ָ������
*           timeout�����ͺ���ʱʱ�䣬��λms
*@note      ��
*@return	true: ���ͳɹ�
*			false: ����ʧ��
*/
bool at_send_no_reply(const char *data, uint16_t datalen, bool delimiter, uint32_t timeout)
{
    bool result=false;
    if(data==NULL)return false;
    if(datalen==0)return false;
	printf("AT cmd:%s\r\n", (char*)data);
    do
    {
       if(at_uart_send(data,datalen,timeout)!=datalen)break;
        if(delimiter)
        {
            if(at_uart_send(AT_DELIMITER,strlen(AT_DELIMITER),timeout)!=strlen(AT_DELIMITER))break;    
        }
        result=true;
    } 
	while (0);
		delay_1ms(timeout);
    return result;
}

/**
*@brief		ע��NB���ݼ����ص�����
*@param     prefix�������ؼ���
*           at_recv_cb���ص�����
*           arg���ص��������ݲ���
*@note      ��
*@return	true: ��ӳɹ�
*			false: ���ʧ��
*/
bool at_register_callback(const char *prefix, at_recv_cb cb, void *arg)
{
    for(uint8_t i=0; i<MAX_OBJ_COUNT; i++)
    {
        if(strcmp(prefix,at.obj[i].prefix)==0)
        {
            printf("This prefix has been registered");
            return false;
        }
    }

    for(uint8_t i=0;i<MAX_OBJ_COUNT;i++)
    {
        if(at.obj[i].prefix==NULL)
        {
            memset(&at.obj[i],0,sizeof(at_obj_t));
            at.obj[i].prefix=(char *)prefix;
            at.obj[i].callback=cb;
            at.obj[i].arg=arg;
            return true;
        }
    }
	return false;
}

/**
*@brief		ע��NB���ݼ����ص�����
*@param     prefix�������ؼ���
*@note      ��
*@return	true: ע���ɹ�
*			false: ע��ʧ��
*/

bool at_unregister_callback(const char *prefix)
{
    for(uint8_t i=0;i<MAX_OBJ_COUNT;i++)
    {
        if(strcmp(prefix,at.obj[i].prefix)==0)
        {
			at.obj[i].prefix=NULL;
			at.obj[i].callback = NULL;
			at.obj[i].arg = NULL;
			return true;
        }
    }
	printf("This prefix has not registered");
	return false;

}
