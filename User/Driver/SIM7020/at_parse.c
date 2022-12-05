/********************************************************************************
* @file    		at_parse.c
* @function     NB模块的数据收发中间层
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
*@brief		解析来自NB的数据，调用相关已注册函数
*@param     pdata：解析的源数据 
*           datalen：数据长度，单位字节      
*@note      不要在此函数下调用sim7020_handle(),或者sim7020_loss_handle()否则可能引起无限回调
*@return	true: 匹配到了已注册函数
*			false: 未匹配到了已注册函数
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
*@brief		AT收发初始化
*@param     无   
*@note      无
*@return	true: 初始化成功
*			false: 初始化失败
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
*@brief		AT收发清除初始化
*@param     无   
*@note      无
*@return	true: 清除初始化成功
*			false: 清除初始化失败
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
*@brief		发送AT指令并等待回复
*@param     cmd：指令数据
*           delimiter：是否加回车换行，即是否结束本句指令
*           right_prefix：正确回复关键词
*           fail_prefix：错误回复关键词
*           reply_buf：回复数据缓存区地址，需要调用前自己开辟好新空间
*           reply_size：回复数据缓存区大小
*           timeout：等待超时时间，单位ms
*@note      无
*@return	true: 交互成功
*			false: 交互失败
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
*@brief		发送AT指令，不等待回复
*@param     data：指令数据
*           timeout：发送后延时时间，单位ms
*@note      无
*@return	true: 发送成功
*			false: 发送失败
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
*@brief		注册NB数据监听回调函数
*@param     prefix：监听关键词
*           at_recv_cb：回调函数
*           arg：回调函数传递参数
*@note      无
*@return	true: 添加成功
*			false: 添加失败
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
*@brief		注销NB数据监听回调函数
*@param     prefix：监听关键词
*@note      无
*@return	true: 注销成功
*			false: 注销失败
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
