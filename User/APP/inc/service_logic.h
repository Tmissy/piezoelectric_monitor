#ifndef SERVICE_LOGIC_H
#define SERVICE_LOGIC_H

#include "gd32f4xx.h"
#include "stdio.h"
#include "stdbool.h"
#include "dci_config.h"
#include "timer_config.h"
#include "lora.h"
#define    MAX_PROBE_NUM         3

typedef enum {
	SampleCount_1 = 0, SampleCount_2, SampleCount_4, SampleCount_8, SampleCount_16,
	SampleCount_32, SampleCount_64, SampleCount_128, SampleCount_256, SampleCount_512, SampleCount_1024
} SampleCount_t;

typedef enum {
	Wait_Receive_Command_Status = 0, Power_On_Get_ORIGTG_Data_Status, LoRa_Tx_ORIGTG_Data_Status, LoRa_Rx_Gateway_Check_Data_Status, LoRa_Retry_Tx_ORIGTG_Data_Status,
} LoRa_Working_Status_t;

typedef enum {
	Start_Collection_Instruction = 'S',Set_Channel_Parameter_Instruction = 'G', Jion_Network_Instruction = 'J',Awake_Detect_Instruction = 'A',
} LoRa_Instruction_t;



//ע��4�ֽڶ���
typedef struct
{
//	uint8_t ch;	              //ͨ����
	uint8_t ls_enable;        //�Ƿ�ʹ�ܴ�ͨ��
	uint8_t wave_num;		  //�ܲ���
	uint8_t gain_db;		  //����
	uint8_t avg_time;         //ƽ���ȼ�����ʵ��Ϊ2�� avg_time �η�
	uint8_t tx_frq_MHz;		  //����Ƶ�ʣ���λMHZ
	uint8_t sample_depth;	  //������ȣ�ʵ��Ϊ sample_depth * 1000
	uint16_t rep_frq_Hz;	  //�ظ�Ƶ��
}SensorParameter_t;
extern SensorParameter_t mult_sensorParam[4];
extern const uint8_t allChannelList[MAX_PROBE_NUM];




//ע��4�ֽڶ���
typedef struct
{
//	uint8_t ch;	              //ͨ����
	uint8_t ls_enable;        //�Ƿ�ʹ�ܴ�ͨ��
	uint8_t wave_num;		  //�ܲ���
	uint8_t gain_db;		  //����
	uint8_t avg_time;         //ƽ���ȼ�����ʵ��Ϊ2�� avg_time �η�
	uint8_t tx_frq_MHz;		  //����Ƶ�ʣ���λMHZ
	uint8_t sample_depth;	  //������ȣ�ʵ��Ϊ sample_depth * 1000
	uint16_t rep_frq_Hz;	  //�ظ�Ƶ��
	
} ensorParameter_t;

typedef struct {
	uint8_t probeId;
	uint8_t cycle ;
	uint8_t gain ;
	uint8_t avg ;
	uint8_t freq;
	uint8_t depth;
	uint16_t repfreq;
}Sensor_Parameter_t;


extern Sensor_Parameter_t sensorParam[MAX_PROBE_NUM];
extern Tx_ORIUTG_Data_t  oriutg_data ;

//void sample_start(uint8_t avg_count);
uint16_t* get_sample_data();
void sample_start(Tx_ORIUTG_Data_t * Poriutg_tx_data,uint8_t avg_count);
void sample_flag_clear(void);
bool get_sample_flag(void);
void acumulator_sensor_data(uint16_t* ad_sample_data );
void lora_passivity_communication(Lora_Control_Def *plora,Tx_ORIUTG_Data_t* oriutg_data);
#endif /* GD32E50X_EXMC_H */