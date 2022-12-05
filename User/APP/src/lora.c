#include "lora.h"
#include "gpio_config.h"
#include "systick.h"
#include "usart_config.h"
#include "crc16.h"
#include "fmc_flash.h"

LoRa_parament_t lora_parament={0,};

#define LORA_DIO0_GPIO_PROT 	LORA_DIO0_PORT
#define SPI_NSS_GPIO_PORT    	LORA_SPI_NSS_PORT
#define  SPI_NSS_PIN					LORA_SPI_NSS_PIN
#define  LORA_RST_GPIO_PROT   LORA_RST_PROT


/**
*@brief		LoRa模块初始化
*@param   psx1278hw ：	  LoRa引脚配置
*@param   psx1278 ：		  LoRa参数配置
*@param    plora  ：   		LoRa数据配置
*@param   spi_periph  ：  硬件SPI选择
*@return	无
*/

void lora_ctrl_init(SX1278_hw_t *psx1278hw, SX1278_t *psx1278, Lora_Control_Def *plora, uint32_t spi_periph)
{
	psx1278hw->dio0.port = LORA_DIO0_GPIO_PROT;
	psx1278hw->dio0.pin = LORA_DIO0_PIN;
	psx1278hw->nss.port = SPI_NSS_GPIO_PORT;
	psx1278hw->nss.pin = SPI_NSS_PIN;
	psx1278hw->reset.port = LORA_RST_GPIO_PROT;
	psx1278hw->reset.pin = LORA_RST_PIN;
	psx1278hw->spi = spi_periph;
	
	psx1278->hw = psx1278hw;
	
	plora->pSX1278 = psx1278;
	plora->data_len = LORA_DATA_LEN; 

	loraSetMode(plora, LORA_SLAVER);

}



/**
*@brief		LoRa工作模式模式配置
*@param   plora  ：   		LoRa数据配置
*@param   mode   ：  			LoRa工作模式选择
*@return	无
*/
void loraSetMode(Lora_Control_Def *plora, uint8_t mode)
{
	plora->master = mode;
	
	if (plora->master == LORA_MASTER) {
		SX1278_begin(&SX1278, SX1278_470_3MHZ, lora_parament.power_factor, SX1278_LORA_SF_7,SX1278_LORA_BW_125KHZ, 10);
		SX1278_LoRaEntryTx(plora->pSX1278, 16, 2000);
	}
	if (plora->master == LORA_SLAVER){
		SX1278_begin(&SX1278, SX1278_500_3MHZ, lora_parament.power_factor, SX1278_LORA_SF_7,SX1278_LORA_BW_125KHZ, 10);//下行信道，接收
		SX1278_LoRaEntryRx(plora->pSX1278, 16, 2000);
	}
}


/**
*@brief		设置数据包帧头
*@param   plora  ：   			LoRa数据配置
*@param   pkgNum   
					plora->lora_data.data_pkg.instruction = 'R'  pkgNum代表数据的总包数
					plora->lora_data.data_pkg.instruction = 'D'  pkgNum代表当前数据包为第几包
*@param   eachLen ：    	
					plora->lora_data.data_pkg.instruction = 'R'  eachLen发送数据包的数据长度
					plora->lora_data.data_pkg.instruction = 'D'  eachLen当前数据包的数据长度
*@return	无
*/
static inline  void loraAddExtDataPkg(Lora_Control_Def *plora, uint16_t pkgNum, uint8_t eachLen)
{
	switch(plora->lora_data.data_pkg.instruction){
	case 'R':
		plora->lora_data.LoRaAnserPkg.totNum = pkgNum;
		plora->lora_data.LoRaAnserPkg.effLen  = eachLen;
		debug_printf ("R totNum = %d\r\n",pkgNum);
		debug_printf ("R eachLen = %d\r\n",eachLen);
		break;
	case 'D':
		plora->num = pkgNum;
		plora->eff_len = eachLen;
		plora->lora_data.data_pkg.ext[0] = (uint8_t)(plora->num & 0x00ff);
		plora->lora_data.data_pkg.ext[1] = (uint8_t)((plora->num & 0xff00)>>8);
		plora->lora_data.data_pkg.ext[2] = eachLen;
		debug_printf ("num : %d\r\n",pkgNum);
		break;
	default :
		break ;
	}
}

/**
*@brief		添加波形数据
*@param   plora  ：   			LoRa数据配置
*@param   pkgNum   
					plora->lora_data.data_pkg.instruction = 'R'  pkgNum代表数据的总包数
					plora->lora_data.data_pkg.instruction = 'D'  pkgNum代表当前数据包为第几包
*@param   eachLen ：    	
					plora->lora_data.data_pkg.instruction = 'R'  eachLen发送数据包的数据长度
					plora->lora_data.data_pkg.instruction = 'D'  eachLen当前数据包的数据长度
*@return	无
*/

static inline void loraDataPkgAddData(Lora_Control_Def *plora, uint8_t *pdata, uint8_t len)
{
	uint8_t i = 0;
	if( (plora->lora_data.data_pkg.instruction == 'D')&&(len==plora->eff_len) ){
		for(i = 0;i < len;i++){
			plora->lora_data.data_pkg.data[i] = pdata[i];
		}
	}
}


/**
*@brief		发送数据
*@param   plora  ：   			LoRa数据配置
					plora->lora_data.data_pkg.instruction = 'R'  发送数据起始包
					plora->lora_data.data_pkg.instruction = 'D'  发送波形数据包
					plora->lora_data.data_pkg.instruction = default  其它指令
*@return	无
*/
static inline void loraSendPkg(Lora_Control_Def *plora)
{
	uint16_t len;
	uint8_t instruct = 0;
	instruct = plora->lora_data.data_pkg.instruction;
  switch(instruct){
	case'R':
		len = LORA_ANSWER_PKG_LEN;
		plora->lora_data.LoRaAnserPkg.crc16 =  crc16((const char*) &plora->lora_data.LoRaAnserPkg, LORA_ANSWER_PKG_LEN-sizeof(plora->lora_data.LoRaAnserPkg.crc16));
		if (plora->master == LORA_MASTER) {
			SX1278_LoRaEntryTx(plora->pSX1278, len, 1000);
			if(SX1278_LoRaTxPacket(plora->pSX1278, (uint8_t *)&plora->lora_data.LoRaAnserPkg, len,1000)){
				debug_printf ("inst R \r\n");
			}
		}
		break ;
	case 'D':
		len = LORA_DATAPKG_LEN;
		plora->lora_data.data_pkg.crc16=  crc16((const char*) &plora->lora_data.data_pkg, LORA_DATAPKG_LEN-sizeof(plora->lora_data.data_pkg.crc16));
		if (plora->master == LORA_MASTER) {
			SX1278_LoRaEntryTx(plora->pSX1278, len, 1000);
			if(SX1278_LoRaTxPacket(plora->pSX1278, (uint8_t *)&plora->lora_data.LoRaAnserPkg, len,1000)){
				debug_printf ("inst D\r\n");
				debug_printf ("tx data len %d\r\n",len);
			}
		}
		break;
	default :
		len = LORA_DATAPKG_INST_LEN;
		plora->lora_data.LoRaCommand.crc16 =  crc16((const char*) &plora->lora_data.LoRaCommand, LORA_DATAPKG_INST_LEN-sizeof(plora->lora_data.LoRaCommand.crc16));
		if (plora->master == LORA_MASTER) {
		SX1278_LoRaEntryTx(plora->pSX1278, len, 1000);
		if(SX1278_LoRaTxPacket(plora->pSX1278, (uint8_t *) (plora->lora_data.tx_data), len,1000)){
			debug_printf ("inst %d\r\n",plora->lora_data.data_pkg.instruction);
			debug_printf ("tx data len %d\r\n",len);
		}
	}
		break;
	}
}
/**
*@brief		发送数据帧
*@param   plora  ：   						LoRa数据配置
*@param   tx_data_buf  ：   			LoRa数据buffer
*@return	无
*/

static inline void loraSendDataFrame(Lora_Control_Def *plora, Tx_ORIUTG_Data_t *tx_data_buf)
{
	uint16_t tot_len;
	uint16_t pkgLable,base;
	uint8_t numPkg,residualDataVolume;
	uint8_t num = 0;
	uint8_t i = 0;
	tot_len = LORA_TX_DATA_LEN;
//	each_len = plora->data_len;
	numPkg = tot_len/plora->data_len;
	residualDataVolume = tot_len%plora->data_len;
	//剩余数据量不等于0，数据包加1
	if(residualDataVolume ){
		numPkg = numPkg +1;
	}
	switch(plora->lora_data.data_pkg.instruction){
	case 'D':
//		tx_data_buf->loraTxData.sensorId = *(__IO uint64_t*)SENSOR_ID_ADDRL;
//		tx_data_buf->loraTxData.networkId= readFlashWord(NETWORK_ID_BASE);
//		tx_data_buf->loraTxData.Battery = BatGetPrecent();
//		tx_data_buf->loraTxData.temperate = ThermGetLastTemp();
//		tx_data_buf->loraTxData.temperate = ThermGetLastTemp();
//		debug_printf("tx_data_buf->loraTxData.temperate = %d\r\n",tx_data_buf->loraTxData.temperate);
//		tx_data_buf->loraTxData.average  = 10;

		for(base=0,pkgLable=1;pkgLable<numPkg;pkgLable++,base+=plora->data_len){
			loraAddExtDataPkg(plora,pkgLable,plora->data_len);
			loraDataPkgAddData(plora,&(tx_data_buf->tx_buf[base]),plora->data_len);
			loraSendPkg(plora);
			delay_1ms(2);
		}
		if(residualDataVolume!=0){
			debug_printf("tot_len %d \r\n",tot_len);
			debug_printf("residualDataVolume %d \r\n",residualDataVolume);
			loraAddExtDataPkg(plora,numPkg,residualDataVolume);
			for(i = 0;i<residualDataVolume;i++,base++){
				plora->lora_data.data_pkg.data[i] = tx_data_buf->tx_buf[base];
			}
			loraSendPkg(plora);
		}
		break;
	case 'R':
		for(num = 0;num < 3;num++) {
			loraAddExtDataPkg(plora,numPkg,plora->data_len);
			loraSendPkg(plora);
			delay_1ms(800);
		}
		break;
	case 'C':
	case 'E':
		for(num = 0;num < 3;num++) {
			loraSendPkg(plora);
			delay_1ms(10);
		}
	case 'O':
		for(num = 0;num < 3;num++) {
			loraSendPkg(plora);
			delay_1ms(800);
		}
		break;
	default:
		break;
	}
}

/**
*@brief		设置lora数据帧指令
*@param   plora  ：   		LoRa数据配置
*@param   inst  ：   			指令
					inst = 'R'  :    准备发送据指令；
					inst = 'D'  :    发送实际数据指令
					inst = 'C'	：   数据包发送完成指令
*@return	无
*/

static inline void loraAddDataPkginst(Lora_Control_Def *plora, uint8_t inst)
{
	switch(inst){
	case 'R':
		plora->lora_data.LoRaAnserPkg.networkId = readFlashWord(NETWORK_ID_BASE);
		plora->lora_data.LoRaAnserPkg.instruction = inst;
		break ;
	case 'D':
	case 'C':
	case 'E':
	case 'O':
		plora->lora_data.data_pkg.networkId = readFlashWord(NETWORK_ID_BASE);
		plora->lora_data.data_pkg.instruction = inst;
		break ;
	case 'F':
		
		break;
	default :
		break ;
	}
	
}


/**
*@brief		设置lora数据帧指令
*@param   plora  ：   		LoRa数据配置
*@param   tx_data_buf     数据buffer
*@param   inst  ：   			指令
					inst = 'R'  :    准备发送据指令；
					inst = 'D'  :    发送实际数据指令
					inst = 'C'	：   数据包发送完成指令
					inst = 'O'	：   'OK'简写，表示设置成功
*@return	无
*/
void loraTxData(Lora_Control_Def *plora,Tx_ORIUTG_Data_t *tx_data_buf,uint8_t inst){
	loraSetMode(plora, LORA_MASTER);
	loraAddDataPkginst(plora,inst);
	loraSendDataFrame(plora, tx_data_buf);
	loraSetMode(plora, LORA_SLAVER);
}

/**
*@brief		设置lora接收模式
*@param   plora  ：   		LoRa数据配置
*@return	len    接收数据的长度
*/
uint8_t lora_receive_data(Lora_Control_Def *plora)
{	
	uint8_t len=0;
	if(plora->master == LORA_SLAVER){
		len = SX1278_LoRaRxPacket(plora->pSX1278);
		if (len > 0){
			SX1278_read(plora->pSX1278, plora->LoRaRxData_t.lora_rxBuf, len);
		}
	}
	return len;
}
/**
*@brief		处理接收到的数据
*@param   plora  ：   		LoRa数据配置
*@param		len    					接收数据的长度
*@return	返回接收到的指令
*/
uint8_t loraDealData(Lora_Control_Def *plora, uint8_t len)
{
	uint8_t inst;
	static uint16_t crc;
	switch(len){
	case LORA_DATAPKG_INST_LEN:
		if (plora->LoRaRxData_t.LoRaCommand.crc16 ==  crc16((const char*) &plora->LoRaRxData_t.LoRaCommand, sizeof (LoRaCommand_t)-sizeof(plora->LoRaRxData_t.LoRaCommand.crc16))){
			debug_printf("sensorID: %llu\r\n",readFlashDoubleWord(SENSOR_ID_ADDRL));
			debug_printf("rxsensorID  : %d\r\n",plora->LoRaRxData_t.LoRaCommand.networkId);
			debug_printf("rxsensorID  : %d\r\n",readFlashWord(NETWORK_ID_BASE));
			if(readFlashWord(NETWORK_ID_BASE) == plora->LoRaRxData_t.LoRaCommand.networkId ){
				inst = plora->LoRaRxData_t.LoRaCommand.command;
				return inst;
			}
			else{
				return LORA_CRTL_FAILD;
			}
		}
		break;
	case LORA_NETIN_LEN:
		if(plora->LoRaRxData_t.netIn.crc16 == crc16((const char*) &plora->LoRaRxData_t.netIn, sizeof (netIn_t)-sizeof(plora->LoRaRxData_t.netIn.crc16))){
			debug_printf("sensorID: %llu\r\n",readFlashDoubleWord(SENSOR_ID_ADDRL));
			debug_printf("networkID  : %llu\r\n",plora->LoRaRxData_t.netIn.sensorID);
			if(readFlashDoubleWord(SENSOR_ID_ADDRL) == plora->LoRaRxData_t.netIn.sensorID){
				inst = plora->LoRaRxData_t.netIn.instruction;
				debug_printf("LOSS_PKG_LEN  inst %d\r\n",inst);
				return inst;
			}
			else{
				return LORA_CRTL_FAILD;
			}
		}
		break;
	case LOSS_PKG_LEN:
		if(plora->LoRaRxData_t.lossPkg.crc16 == crc16((const char*) &plora->LoRaRxData_t.lossPkg, sizeof (plora->LoRaRxData_t.lossPkg)-sizeof(plora->LoRaRxData_t.lossPkg.crc16))){
			if(readFlashWord(NETWORK_ID_BASE) == plora->LoRaRxData_t.lossPkg.networkId){
				inst = plora->LoRaRxData_t.lossPkg.instruction;
				return inst;
			}
			else{
				return LORA_CRTL_FAILD;
			}

		}
	default:
		return LORA_CRTL_FAILD;
	}
	return LORA_CRTL_FAILD;
}

/**
*@brief		发送指定的数据包数
*@param   plora  ：   					LoRa数据配置
*@param   tx_data_buf  ：   		LoRa数据buffer
*@param   pkgCount  ：   				LoRa丢失数据包总数
*@param   pkgBuff   ：  				丢失的第几个数据包
*@return	无
*/
void loraSendSpecifiedPkg(Lora_Control_Def *plora, Tx_ORIUTG_Data_t *tx_data_buf,uint8_t pkgCount,uint8_t * pkgBuff, uint8_t inst)
{
	uint16_t tot_len,each_len;
	uint8_t i = 0;
	uint16_t pkgLable ,base ,j;
	uint8_t numPkg,residualDataVolume;
	uint8_t num = 0;
	tot_len = LORA_TX_DATA_LEN;
	each_len = plora->data_len;
	numPkg = tot_len/each_len;
	residualDataVolume = tot_len%each_len;
	
	loraSetMode(plora, LORA_MASTER);
	loraAddDataPkginst(plora,inst);
	loraAddDataPkginst(plora, inst);
	//剩余数据量不等于0，数据包加1
	if(residualDataVolume ){	
		numPkg = numPkg +1;
	}
	if(plora->lora_data.data_pkg.instruction == 'D') {
		for(base=0,pkgLable=1;pkgLable<numPkg;pkgLable++,base+=each_len){
			for(j = 0;j < pkgCount;j++){
				if(pkgLable == pkgBuff[j]) {
					debug_printf ("pkgBuff[j] = %d\r\n",pkgBuff[j]);
					loraAddExtDataPkg(plora,pkgLable,each_len);
					loraDataPkgAddData(plora,&(tx_data_buf->tx_buf[base]),each_len);
					loraSendPkg(plora);
					delay_1ms(2);
				}
			}
	}
	for(j = 0;j < pkgCount;j++){
		if(pkgBuff[j] == 0x13){
			debug_printf("tot_len %d \r\n",tot_len);
			debug_printf("residualDataVolume %d \r\n",residualDataVolume);
			loraAddExtDataPkg(plora,numPkg,residualDataVolume);
			for(i = 0;i<residualDataVolume;i++,base++){
				plora->lora_data.data_pkg.data[i] = tx_data_buf->tx_buf[base];
			}
			loraSendPkg(plora);		
		}
	}
	  
	}
		loraSetMode(plora, LORA_SLAVER);
}








