#include "dev_sim7020.h"
#include "stdbool.h"

#ifndef    NET_PROTOCOL_H
#define    NET_PROTOCOL_H

#define    NET_BUF_SIZE          2048
#define    NET_PACK_SIZE         2048

#define    DEBUG_MODE_CHECK       0
#define    DEBUG_MODE_RUN         1
#define    DEBUG_MODE_DONE        2

#define    LONG_SLEEP_WAIT       0
#define    LONG_SLEEP_READY      1
#define    LONG_SLEEP_OPEN       2

#define    DEFAULT_PROBE_ID         "001"


//#define    RESP_SOURCE               ";"
#define    RESP_SOURCE               "ETM100Sensor;"
#define    HEADRE_BEAT               "heartbeat;"
#define    RESP_HEADRE_BEAT_ACK      "heartbeat_ack;"
#define    RESP_HEADRE_UPLOAD        "upload;"
#define    RESP_HEADRE_SET_ACK       "set_params_ack;"
#define    RESP_HEADRE_GET_ACK       "get_params_ack;"
#define    RESP_HEADRE_DEBUG_ACK     "debug_ack;"
#define    RESP_HEADRE_GET_DATA_ACK  "get_data_ack;"
#define    REQU_HEADRE_DEBUG         "check_debug;"
#define    RESP_HEADRE_SET_TIME_ACK  "set_time_ack;"
#define    RESP_HEADRE_PRODUCT_ACK   "production_ack;"
#define    RESP_EEADRE_UPDATE_IP_ACK "update_ip_ack;"

#define    RECV_OPERATION_CLOSE       0
#define    RECV_OPERATION_SET         1
#define    RECV_OPERATION_GET         2
#define    RECV_OPERATION_GET_DATA    3
#define    RECV_OPERATION_DEBUG       4
#define    RECV_OPERATION_SET_TIME    5
#define    RECV_OPERATION_LONG_SLEEP  6
#define    RECV_OPERATION_UPDATE_FIREWARE  7
#define    RECV_OPERATION_UPDATE_IP_PORT  8


#define    RESP_OPERATION_INVALID           0
#define    RESP_OPERATION_SET_SUCC          11
#define    RESP_OPERATION_SET_FAIL          10
#define    RESP_OPERATION_SET_TIME_SUCC     13
#define    RESP_OPERATION_SET_TIME_FAIL     12

#define    RESP_OPERATION_POLL_NOW          21
#define    RESP_OPERATION_POLL_NULL         20
#define    RESP_OPERATION_DEBUG_SAMPLE_NOW  31
#define    RESP_OPERATION_DEBUG_SAMPLE_NULL 30
#define    RESP_OPERATION_DEBUG_OPEN_SUCC   41
#define    RESP_OPERATION_REQ_DEBUG         42
#define    REQ_OPERATION_HEART_DEBUG        43

#define    RESP_OPERATION_DEBUG_OPEN_FAIL   40
#define    RESP_OPERATION_DEBUG_CLOSE_SUCC  51
#define    RESP_OPERATION_DEBUG_CLOSE_FAIL  50
#define    RESP_OPERATION_AUTO_SAMPLE_NOW   61
#define    RESP_OPERATION_AUTO_SAMPLE_NULL  60
#define    RESP_OPERATION_PRODUCTION_SUCC   71
#define    RESP_OPERATION_PRODUCTION_FAIL   70
#define    RESP_OPERATION_UPDATE_IP_FAILED  80
#define    RESP_OPERATION_UPDATE_IP_SUCCEED 81

//注意此处2字节未对齐，所以在头添加保留一字节，第一字节不发送
typedef struct
{
	//uint8_t reserve;
	//1字节
	uint8_t source[13];
	uint8_t header[7];
	uint8_t sensor_id[21];
	uint8_t part_number[2];
	uint8_t part[2];
	uint8_t upload_ack[2];
	//47字节
	uint8_t probe_num[4];	
	uint8_t gain[2];
	uint8_t battery[2];
	uint8_t average[2];
	uint8_t temperature[3];
	uint8_t data_len[3];
	uint8_t data_crc[3];
	//19字节
	uint16_t wave_data[];
	
}upload_data_t;

bool StartSampleUpload(uint8_t reTryTimes, const uint8_t channelList[], uint8_t channelNum);
bool net_recv_handle(char* pdata, uint16_t data_len);
void net_get_debug_data(char* pdata, uint16_t* data_len);
bool net_cmd_handle(char* pdata, uint16_t data_len);
uint8_t net_get_debug_status(void);
void net_reset_debug_status(void);
bool net_get_ls_close(void);
void net_set_close_sta(bool ls_close);
bool net_get_ls_send_succ(void);
void net_set_send_sta(bool ls_succ);
//bool net_response_sample(bool ls_retra, bool ls_one_pack, uint8_t part);
bool net_retra_handle(void);
bool net_send_udp_tcp(const char* pdata, uint16_t data_len);
void net_get_heart_data(char* pdata, uint16_t* data_len);

extern uint8_t sensor_id[21];
extern uint8_t remote_ip1[21];
extern uint8_t sleep_step;

#endif
