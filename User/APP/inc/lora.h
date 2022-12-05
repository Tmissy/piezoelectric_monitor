#ifndef __LORA_H__
#define __LORA_H__


#include "sx1278.h"
#include "gd32f4xx.h"
#include "dci_config.h"

#define LORA_CRTL_OK          (0x01)
#define LORA_CRTL_FAILD       (0x00)

#define LOSE_PAKET_MAX  (10)
#define LOSE_START_INDEX (6)


#define LORA_MAX_PKG_LEN     (230) 


#define LORA_DATAPKG_LEN                 (sizeof(DataPackage_def))
#define LORA_DATAPKG_INST_LEN            (sizeof(LoRaCommand_t))
#define LORA_NETIN_LEN                   (sizeof(netIn_t))
#define LOSS_PKG_LEN                     (sizeof(lossPkg_t))
#define SET_GAIN_LEN                     (sizeof(arg_t))

#define LORA_DATAPKG_COM_LEN             (sizeof(uint16_t)+sizeof(uint16_t)+sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint8_t))

#define LORA_ANSWER_PKG_LEN               (sizeof(LoRaAnswerPkg_t))


#define LORA_DATA_LEN                    (LORA_MAX_PKG_LEN-LORA_DATAPKG_COM_LEN)

#define LORA_RXBUF_LEN       (256)
#define LORA_GATEWAY_ADDR    (0x00)

typedef struct
{
	uint32_t flag;
	uint32_t power_factor;
}LoRa_parament_t;

typedef enum {
	LORA_SLAVER = 0,
	LORA_MASTER ,
}Lora_Mode_t;


typedef struct{
	uint32_t networkId;
	uint8_t  instruction;
	uint8_t  reserve;
	uint8_t  channle_Id;
	uint8_t  cycle;
	uint8_t  gain;
	uint8_t  avg;
	uint8_t  freq;
	uint8_t  depth;
	uint16_t repfreq;
	uint16_t crc16;
}arg_t;

typedef struct {
	uint32_t networkId;
	uint8_t instruction;
	uint8_t  len;
	uint8_t  lossNum[8];
	uint16_t crc16;
}lossPkg_t;

typedef struct{
	uint32_t networkId;
	uint8_t instruction;
	uint8_t ext[3];
	
	uint8_t data[LORA_DATA_LEN];
	uint16_t crc16;
}DataPackage_def;

typedef struct{
	uint32_t networkId;
	uint8_t command;
	uint8_t channle_Id;
	uint16_t crc16;
}LoRaCommand_t;

typedef struct{
	uint64_t sensorID;
	uint64_t key;
	uint32_t networkId;
	uint8_t  instruction;
	uint8_t  reserve;
	uint16_t crc16;
}netIn_t;

typedef struct {
	uint32_t networkId;
	uint8_t instruction;
	uint16_t  totNum;
	uint8_t  effLen;
	uint16_t crc16;
}LoRaAnswerPkg_t;


typedef struct{
	union pkg_t{
		LoRaAnswerPkg_t LoRaAnserPkg;
		LoRaCommand_t LoRaCommand;
		DataPackage_def data_pkg;
		uint8_t tx_data[LORA_DATAPKG_LEN];
	}lora_data;
	
	uint8_t master;
	uint16_t pkg_num;
	uint8_t  data_len;
	uint16_t num;
	uint8_t eff_len;
	union {
		uint8_t lora_rxBuf[LORA_RXBUF_LEN];
		netIn_t netIn;
		lossPkg_t lossPkg;
		LoRaCommand_t LoRaCommand;
		arg_t arg;
	}LoRaRxData_t;
	
	SX1278_t *pSX1278;
}Lora_Control_Def;
extern LoRa_parament_t lora_parament;
extern Lora_Control_Def lora_ctrl;
extern SX1278_hw_t SX1278_hw;
extern SX1278_t SX1278;

void lora_ctrl_init(SX1278_hw_t *psx1278hw, SX1278_t *psx1278, Lora_Control_Def *plora, uint32_t spi);
 void loraSetMode(Lora_Control_Def *plora, uint8_t mode);

void loraTxData(Lora_Control_Def *plora,Tx_ORIUTG_Data_t *tx_data_buf,uint8_t inst);

uint8_t lora_receive_data(Lora_Control_Def *plora);
uint8_t loraDealData(Lora_Control_Def *plora, uint8_t len);
void loraSendSpecifiedPkg(Lora_Control_Def *plora, Tx_ORIUTG_Data_t *tx_data_buf,uint8_t pkgCount,uint8_t * pkgBuff, uint8_t inst);

#endif /*__CONTROL_H__*/
