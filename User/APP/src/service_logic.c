#include "service_logic.h"
#include "dci_config.h"
#include "usart_config.h"
#include "systick.h"
#include "string.h"
#include "fmc_flash.h"
#include "adc_config.h"
#include "dac_config.h"
#include "gpio_config.h"

Tx_ORIUTG_Data_t oriutg_data = {0};
#define MAX_AVG_TIME         (10)
static uint16_t sample_count = 0;
static bool sample_flag = false;
static uint32_t accumulator_data[SENSOR_DATA_LEN] = {0};


const uint8_t allChannelList[MAX_PROBE_NUM] = {1,2,3}; //注意初始化

SensorParameter_t mult_sensorParam[4];

Sensor_Parameter_t sensorParam[MAX_PROBE_NUM] = {
										{0x01,0x02,72,0x8,0x4,0x0,500},
									  };

static uint8_t losePkgNum[LOSE_PAKET_MAX] = {0};

uint16_t* get_sample_data()
{
	return oriutg_data.loraTxData.Data ;	
}

void sample_flag_set(){
	sample_flag = true;
}

void sample_flag_clear(){
	sample_flag = false;
}

bool get_sample_flag(){
	return sample_flag;
}

uint16_t get_sample_count(void){
	return sample_count;
}

void acumulator_sensor_data(uint16_t* ad_sample_data ){
	
	if(ad_sample_data == NULL){
		return;
	}
	for(uint16_t i = 0;i < SENSOR_DATA_LEN ; i++){
		accumulator_data[i] += (ad_sample_data[i]&0x3ff);
	}	
}

void acumulator_average(uint16_t* data){
		for(uint16_t i = 0;i < SENSOR_DATA_LEN;i++){
			accumulator_data[i] /= sample_count;
			data[i] = accumulator_data[i];
			debug_printf("%d,",data[i]);
		}
		debug_printf("\r\n");
}

void flush_acumulator(){
	memset((uint8_t*)accumulator_data,0,SENSOR_DATA_LEN*sizeof(uint32_t));
}

void sample_start(Tx_ORIUTG_Data_t * Poriutg_tx_data,uint8_t avg_count){
	uint32_t timeout = 1000*30;
	uint32_t startTick = 0;
	if(avg_count > MAX_AVG_TIME){
		return;
	}
	startTick = Get_Tick();
	sample_count = 1<<avg_count;
//	sample_count = 1;
		//开启定时器2，重复频率500HZ，开启采集
	debug_printf("sample_count = %d\r\n",sample_count);

}


void LoRa_execute_instruction(Lora_Control_Def *plora,Tx_ORIUTG_Data_t* oriutg_data , uint8_t instruction){
	uint32_t * probeGainAvg = NULL;
	switch(instruction){
		case Start_Collection_Instruction:
			//获取通道号
			oriutg_data->loraTxData.channel_num = plora->LoRaRxData_t.LoRaCommand.channle_Id;
			//清空接收buffer
			memset(plora->LoRaRxData_t.lora_rxBuf,0,256);
			loraTxData(&lora_ctrl,oriutg_data,'R');
			break ;
		
		case Set_Channel_Parameter_Instruction:
			debug_printf("networkID  : %x\r\n",plora->LoRaRxData_t.arg.networkId);
			debug_printf("plora->LoRaRxData_t.arg.inst = %d\r\n",plora->LoRaRxData_t.arg.instruction);
			debug_printf("plora->LoRaRxData_t.arg.reserve = %d\r\n",plora->LoRaRxData_t.arg.reserve);
			debug_printf("plora->LoRaRxData_t.arg.probeId= %d\r\n",plora->LoRaRxData_t.arg.channle_Id);
			debug_printf("plora->LoRaRxData_t.arg.gain = %d\r\n",plora->LoRaRxData_t.arg.gain);
			debug_printf("plora->LoRaRxData_t.arg.avg= %d\r\n",plora->LoRaRxData_t.arg.avg);
			//将 flash 存储的参数取出
			probeGainAvg = readFlash_X_Word(CHANNALE_PRAREMETER,16);
			debug_printf("probeGainAvg addr : %p\r\n",probeGainAvg);
			//擦除页
			fmc_erase_pages(FMC_PRAREMETER_WRITE_START_ADDR);

			//更改参数
			if((plora->LoRaRxData_t.arg.channle_Id > 0) && (plora->LoRaRxData_t.arg.channle_Id <= 16)){
				sensorParam[plora->LoRaRxData_t.arg.channle_Id - 1].gain = plora->LoRaRxData_t.arg.gain;
				sensorParam[plora->LoRaRxData_t.arg.channle_Id - 1].avg = plora->LoRaRxData_t.arg.avg;
			}	
			if((sensorParam[plora->LoRaRxData_t.arg.channle_Id - 1].gain >= 0 )&& (sensorParam[plora->LoRaRxData_t.arg.channle_Id - 1].gain <= 48 )){
				*(probeGainAvg+2*(plora->LoRaRxData_t.arg.channle_Id - 1)) = sensorParam[plora->LoRaRxData_t.arg.channle_Id - 1].gain;
			}
//			*(probeGainAvg+2*(plora->LoRaRxData_t.arg.channle_Id - 1)+1) = sensorParam[plora->LoRaRxData_t.arg.channle_Id - 1].avg;			
			//将接收到的参数写回
			for(uint8_t i = 0;i < MAX_PROBE_NUM; i++ ){
				sensorParam[i].gain = *(probeGainAvg+2*i);
				sensorParam[i].avg = *(probeGainAvg+2*i+1);
				debug_printf("probenum: %d\r\n",i);
				debug_printf("GAIN: %d\r\n",sensorParam[i].gain );
				debug_printf("AVG: %d\r\n",sensorParam[i].avg);
			}
		if(writeFlash_X_Word(CHANNALE_PRAREMETER,MAX_PROBE_NUM*2,probeGainAvg)){
			debug_printf("\r\nset gain succeed\r\n");
			loraTxData(plora, oriutg_data,'O');
		}else{
			debug_printf("\r\nset gain failed\r\n");
			loraTxData(plora, oriutg_data,'F');
		}
		memset(plora->LoRaRxData_t.lora_rxBuf,0,256);
		break ;
		case Jion_Network_Instruction:
			debug_printf("\r\n recieve jion network instruction \r\n");
			debug_printf("sensorID: %llu\r\n",readFlashDoubleWord(SENSOR_ID_ADDRL));
			debug_printf("networkID  : %llu\r\n",plora->LoRaRxData_t.netIn.sensorID);
			if(plora->LoRaRxData_t.netIn.sensorID == readFlashDoubleWord(SENSOR_ID_ADDRL)){
				debug_printf("\r\nRX ID\r\n");
				fmc_erase_pages(FMC_NETWORK_WRITE_START_ADDR);
				debug_printf("\r\nRX ID\r\n");
				writeFlashWord(NETWORK_ID_BASE, plora->LoRaRxData_t.netIn.networkId);
				debug_printf("rxsensorID  : %0x\r\n",plora->LoRaRxData_t.netIn.networkId);
				debug_printf("rxsensorID  : %0x\r\n",readFlashWord(NETWORK_ID_BASE));
			}
				loraTxData(plora, oriutg_data,'O');
			break ;
		case Awake_Detect_Instruction:
				memset(plora->LoRaRxData_t.lora_rxBuf,0,256);
				delay_1ms(1800);
				loraTxData(plora, oriutg_data,'O');
				debug_printf("tx 0 wancheng \r\n");
				break ;
		default:
			break ;
	}

}

void lora_passivity_communication(Lora_Control_Def *plora,Tx_ORIUTG_Data_t* oriutg_data)
{
	static uint8_t main_state = 0x00;
	static uint8_t receiveDatalen=0;
	static uint8_t instruction =0;
	static uint16_t tick=0;
	static uint8_t communicate_flag = 1;
	uint32_t * probeGainAvg = 0;
	uint8_t totLosePkgNum = 0;
	uint8_t loseStartIndex = LOSE_START_INDEX;
	uint8_t i = 0;
	communicate_flag = 1;
	main_state = Wait_Receive_Command_Status;
	uint32_t tickstart = Get_Tick();
	uint32_t printfTick = Get_Tick();
	
	while (communicate_flag){
		switch(main_state){
			case Wait_Receive_Command_Status:
				receiveDatalen = lora_receive_data(plora);
				if(receiveDatalen > 0){
					instruction =loraDealData(plora,receiveDatalen);
					debug_printf("instruction = %d\r\n",instruction);
					LoRa_execute_instruction(plora,oriutg_data , instruction);
					receiveDatalen = 0;
					if(instruction == Start_Collection_Instruction){		
						main_state = Power_On_Get_ORIGTG_Data_Status;
					}else{
						memset(plora->LoRaRxData_t.lora_rxBuf,0,256);
						communicate_flag = 0;
					}
				}
			if ((Get_Tick() - tickstart) > 10000)
				{		
					communicate_flag = 0;
					debug_printf("wait lora tomeout!\r\n");
				}
				break;
	case Power_On_Get_ORIGTG_Data_Status:
				powerOn();
				debug_printf("poweron\r\n");
				/*更新参数*/
				//SensorID
				oriutg_data->loraTxData.sensorId = readFlashDoubleWord(SENSOR_ID_ADDRL);
				//数据长度
				oriutg_data->loraTxData.length = sizeof(Tx_ORIUTG_Data_t);
				//网络号
				oriutg_data->loraTxData.networkId = readFlashWord(NETWORK_ID_BASE) ;
				//增益
				probeGainAvg = readFlash_X_Word(CHANNALE_PRAREMETER,16);
				oriutg_data->loraTxData.gain = *(probeGainAvg+2*(oriutg_data->loraTxData.channel_num-1));
				set_gain(oriutg_data->loraTxData.gain);
				debug_printf("gain = %d \r\n",oriutg_data->loraTxData.gain);
				//平均
//				oriutg_data->loraTxData.average = *(probeGainAvg+2*(oriutg_data->loraTxData.channel_num-1)+1);
				oriutg_data->loraTxData.average = 10;
				debug_printf("avrage = %d \r\n",oriutg_data->loraTxData.average);
				//电量采集
				oriutg_data->loraTxData.Battery = BatGetPrecent();
				debug_printf("battery  = \r\n");
				sample_start(oriutg_data,10);
				//温度采集
				delay_1ms(1);
//				oriutg_data->loraTxData.temperate = ThermGetLastTemp() ;
//				oriutg_data->loraTxData.temperate = ThermGetLastTemp() ;
				debug_printf("temperature = \r\n");
				//关闭发射接收电压
//				powerOff();
				main_state = LoRa_Tx_ORIGTG_Data_Status;
				tickstart = Get_Tick();
				powerOff();
				debug_printf("poweroff\r\n");
			case LoRa_Tx_ORIGTG_Data_Status:
				loraTxData(plora,oriutg_data,'D');
				loraTxData(plora,oriutg_data,'C');
				tickstart = Get_Tick();
				main_state =LoRa_Rx_Gateway_Check_Data_Status;
			
			case LoRa_Rx_Gateway_Check_Data_Status:
				receiveDatalen = lora_receive_data(plora);
				if ( receiveDatalen >0 ){
					debug_printf("receiveDatalen = %d\r\n",receiveDatalen);
					instruction=loraDealData(plora,receiveDatalen);
					if(instruction=='T' ){
						debug_printf("inst == T\n");
						communicate_flag = 0;
						break;
					}
					if(instruction=='F'){
						totLosePkgNum = plora->LoRaRxData_t.lora_rxBuf[5];
						debug_printf("inst == F\n");
						debug_printf("totLosePkgNum %d\r\n",totLosePkgNum);
						if(totLosePkgNum < LOSE_PAKET_MAX){
							for(i = 0;i < totLosePkgNum;i++) {
								losePkgNum[i] = plora->LoRaRxData_t.lora_rxBuf[loseStartIndex++];
							}
						}
						printf("inst == F\n");
						main_state =LoRa_Retry_Tx_ORIGTG_Data_Status;//通信不成功，重发数据 
						break;
					}
				}
				if((Get_Tick() - tickstart) > 5000)
				{		
					debug_printf("lora enter main_state = 2\n");
					communicate_flag = 0;	
				}
				break;
			case LoRa_Retry_Tx_ORIGTG_Data_Status:
					loraSendSpecifiedPkg(plora, oriutg_data,totLosePkgNum,losePkgNum, 'D');
					loraTxData(plora,oriutg_data,'C');
					main_state = LoRa_Rx_Gateway_Check_Data_Status;
				break ;
			default:
				communicate_flag = 0;
				break ;

			}
		}
}
