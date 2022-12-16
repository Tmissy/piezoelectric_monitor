#include "fmc_flash.h"
#include "usart_config.h"
#include "net_protocol.h"
#include "rtc_config.h"
#include "lora.h"
#include "systick.h"
#include "dac_config.h"

#define FMC_PAGE_SIZE           ((uint16_t)0x2000U)  //8k
#define FMC_WRITE_START_ADDR    ((uint32_t)0x0800A000U)
#define FMC_WRITE_END_ADDR      ((uint32_t)0x0800BFFFU)

#define SENSORID_USART   USART1

#define FLASH_ERASE_FAILD   (0xaa)
#define FLASH_ADDR_ERROR       (0xff)
#define FLASH_DATA_OVERFLOW  (0xfe)

//static uint32_t data_buffer[SENSOR_PRARAMETER_SIZEOF]={0};
static union  
{
	sensor_parameter_t sersor_parameter;
	uint32_t sensor_parameter_buffer[SENSOR_PRARAMETER_SIZEOF];
}sensor_parameter_n;


static union{
	vga_gain_t channle_gain[CHANNLE_NUM];
	uint32_t sensor_gain_buffer[CHANNLE_GAIN_BUFFER];
}sensor_gain_n;

static uint8_t flashData[12] = {0};



//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~11,即addr所在的扇区
static inline fmc_state_enum get_falsh_sector(unsigned int addr)
{
	debug_printf("addr = %0x\r\n",addr);
	if(addr<ADDR_FLASH_SECTOR_1)          return CTL_SECTOR_NUMBER_0;
	else if(addr<ADDR_FLASH_SECTOR_2)			return CTL_SECTOR_NUMBER_1;
	else if(addr<ADDR_FLASH_SECTOR_3)			return CTL_SECTOR_NUMBER_2;
	else if(addr<ADDR_FLASH_SECTOR_4)			return CTL_SECTOR_NUMBER_3;
	else if(addr<ADDR_FLASH_SECTOR_5)			return CTL_SECTOR_NUMBER_4;
	else if(addr<ADDR_FLASH_SECTOR_6)			return CTL_SECTOR_NUMBER_5;
	else if(addr<ADDR_FLASH_SECTOR_7)			return CTL_SECTOR_NUMBER_6;
	else if(addr<ADDR_FLASH_SECTOR_8)			return CTL_SECTOR_NUMBER_7;
	else if(addr<ADDR_FLASH_SECTOR_9)			return CTL_SECTOR_NUMBER_8;
	else if(addr<ADDR_FLASH_SECTOR_10)		return CTL_SECTOR_NUMBER_9;
	else if(addr<ADDR_FLASH_SECTOR_11)		return CTL_SECTOR_NUMBER_10; 
	return CTL_SECTOR_NUMBER_11;	
}  


/*!
    \brief      erase fmc pages from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
static inline fmc_state_enum fmc_erase_sector_by_address(uint32_t address)
{
		fmc_state_enum status = FMC_READY;
		uint32_t tick = 0;
    /* unlock the flash program/erase controller */
    fmc_unlock();
    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* erase the flash pages */
//   status =  fmc_sector_erase(get_falsh_sector(address));
	        /* wait the erase operation complete*/
		 tick = Get_Tick();
		 debug_printf("addr is %0x \r\n",get_falsh_sector(address));
		 if((address > ADDR_FLASH_SECTOR_8)||(address < ADDR_FLASH_SECTOR_0)){
				return FLASH_ADDR_ERROR;
		 }
		 while(FMC_READY != fmc_sector_erase(get_falsh_sector(address))){
			if((Get_Tick() - tick > 5000)){
				debug_printf("fmc sector erase failed \r\n");
				return fmc_sector_erase(get_falsh_sector(address));
				break;
			}
		 }
		debug_printf("addr is %0x \r\n",get_falsh_sector(address));
    /* lock the main FMC after the erase operation */
    fmc_lock();
		return 	FMC_READY;
}

void read_datas_to_flash(uint32_t flashAddr,uint32_t* data_buffer,uint8_t num){
	if(num > SENSOR_PRARAMETER_SIZEOF){
		debug_printf("SENSOR_PRARAMETER_SIZEOF is %d \r\n",SENSOR_PRARAMETER_SIZEOF);
		debug_printf("num is %d \r\n",num);
		return ;
	}
	for(uint8_t i = 0;i < num ;i++){
		data_buffer[i] = *(__IO uint32_t*)(flashAddr+i*WORD);
	}

}


static fmc_state_enum write_datas_to_flash(uint32_t flashAddr,uint16_t num,uint32_t* data){
	uint8_t i = 0;
	uint32_t *buffer_pointer =NULL;
	fmc_state_enum status = FMC_READY;
	status = fmc_erase_sector_by_address(flashAddr);
	fmc_unlock();
	
	if(status == FMC_READY){
		for(i = 0;i<num;i++){
			status = fmc_word_program(flashAddr+i*WORD,*(data+i));
//			debug_printf("write_datas_to_flash addr %0x \r\n",flashAddr+i*WORD);
//			debug_printf("write_datas_to_flash data %0x \r\n",*(__IO uint32_t*)(flashAddr+i*WORD));
			fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
			if(status != FMC_READY){
				 return status;
			}
		}
	}else{
		return FLASH_ERASE_FAILD;
		debug_printf("flash addr %0x erase failed\r\n",flashAddr);
	}
	fmc_lock();
	return status;
}
/*!
    \brief      写入sensorid 到flash
    \param[flashAddr]  flash 地址
    \param[num] 			 需要操作的数据长度
    \retval     			 none
*/
void write_sensorid_to_flash() {
		uint32_t writeFlashData = 0;

		uint32_t sersoridH = 0;
		uint32_t sersoridL = 0;
	    /* unlock the flash program/erase controller */
    fmc_unlock();

    read_datas_to_flash(SENSOR_PARAMETER_ADDR,sensor_parameter_n.sensor_parameter_buffer,SENSOR_PRARAMETER_SIZEOF);
			
	
		while(sensor_parameter_n.sersor_parameter.sensor_id.flag != 0xaaaaaaaa){
			printf("\r\nplease send flash sensorId\r\n");
			while(usart_datas_receive(SENSORID_USART,flashData,12,100));
			writeFlashData = 0;
			writeFlashData |= flashData[0]<<24; 
			writeFlashData |= flashData[1]<<16; 
			writeFlashData |= flashData[2]<<8; 
			writeFlashData |= flashData[3];
			sersoridH = 0;
			sersoridH |= flashData[4]<<24; 
			sersoridH |= flashData[5]<<16; 
			sersoridH |= flashData[6]<<8; 
			sersoridH |= flashData[7];
			sersoridL = 0;
			sersoridL |= flashData[8]<<24; 
			sersoridL |= flashData[9]<<16; 
			sersoridL |= flashData[10]<<8; 
			sersoridL |= flashData[11];
			sensor_parameter_n.sersor_parameter.sensor_id.flag = 0xaaaaaaaa;
			sensor_parameter_n.sersor_parameter.sensor_id.sensorid[0] = sersoridL;
			sensor_parameter_n.sersor_parameter.sensor_id.sensorid[1] = sersoridH;
			debug_printf("sensorID: %x\r\n",(uint64_t)sensor_parameter_n.sersor_parameter.sensor_id.sensorid[0]);
			fmc_erase_sector_by_address(SENSOR_PARAMETER_ADDR);	
			fmc_unlock();
			
			if(write_datas_to_flash(SENSOR_PARAMETER_ADDR,SENSOR_PRARAMETER_SIZEOF,sensor_parameter_n.sensor_parameter_buffer) == FMC_READY){
				debug_printf("flash write to successful");
			}else{
				debug_printf("flash wirte to failed");
			}
			fmc_lock();
			
			debug_printf("sensorID: %llu\r\n",read_double_words_form_flash(SENSOR_ID_ADDR));
			debug_printf("sensorID: %x\r\n",*(__IO uint32_t*)SENSOR_ID_ADDR);
			ReLoadSersorId();
			fmc_lock();
	}
}

/*!
    \brief      从flash读取sensorid；
    \param[flashAddr]  flash 地址
    \param[num] 			 需要操作的数据长度
    \retval     			 none
*/

uint64_t read_sensorid_form_flash(uint32_t flashAddr){
	uint64_t ret;
	ret = read_double_words_form_flash(flashAddr);
	return ret;
}

/*!
    \brief      						 写入network id；
    \param[flashAddr]  			 flash 地址
    \param[network_id] 			 待写入的网络号
    \retval     			 none
*/
fmc_state_enum write_network_id_to_flash(uint32_t flashAddr,uint32_t network_id){
	fmc_state_enum status = FMC_READY;
	//读取falsh数据；
	read_datas_to_flash(SENSOR_PARAMETER_ADDR,sensor_parameter_n.sensor_parameter_buffer,SENSOR_PRARAMETER_SIZEOF);
	//修改数据；
	sensor_parameter_n.sersor_parameter.network_id.flag = PARAMETER_WRITE_FLASH_FALG;
	sensor_parameter_n.sersor_parameter.network_id.network_id = network_id;
	//数据回写
	status = write_datas_to_flash(SENSOR_PARAMETER_ADDR,SENSOR_PRARAMETER_SIZEOF,sensor_parameter_n.sensor_parameter_buffer);

		return status;
}

uint32_t read_network_id_from_flash(uint32_t flashAddr){
	uint32_t ret;
	ret = read_word_form_flash(flashAddr);
	return ret;
}



/*!
    \brief      						 写入ip addr；
    \param[flashAddr]  			 flash 地址
    \param[network_id] 			 待写入的ip地址和端口
    \retval     			 none
*/
fmc_state_enum write_ip_prot_to_flash(uint32_t flashAddr,ip_prot_t* ip_prot){
	
	//读取falsh数据；
	read_datas_to_flash(SENSOR_PARAMETER_ADDR,sensor_parameter_n.sensor_parameter_buffer,SENSOR_PRARAMETER_SIZEOF);
	//修改数据；
	sensor_parameter_n.sersor_parameter.ip_prot.flag = PARAMETER_WRITE_FLASH_FALG;
	sensor_parameter_n.sersor_parameter.ip_prot.ip[0] = ip_prot->ip[0];
	sensor_parameter_n.sersor_parameter.ip_prot.ip[1] = ip_prot->ip[1];
	sensor_parameter_n.sersor_parameter.ip_prot.ip[2] = ip_prot->ip[2];
	sensor_parameter_n.sersor_parameter.ip_prot.ip[3] = ip_prot->ip[3];
	sensor_parameter_n.sersor_parameter.ip_prot.prot = ip_prot->prot;
	//数据回写
	write_datas_to_flash(flashAddr,SENSOR_PRARAMETER_SIZEOF,sensor_parameter_n.sensor_parameter_buffer);
}


/*!
    \brief      						 读取 ip addr；
    \param[flashAddr]  			 flash 地址
    \param[network_id] 			 待写入的ip地址和端口
    \retval     			 none
*/
void reload_ip_port(uint32_t flashAddr)
{	
		read_datas_to_flash(SENSOR_PARAMETER_ADDR,sensor_parameter_n.sensor_parameter_buffer,SENSOR_PRARAMETER_SIZEOF);
		debug_printf("flag: %x\r\n",sensor_parameter_n.sersor_parameter.ip_prot.flag);
		debug_printf("ip1: %d\r\n",sensor_parameter_n.sersor_parameter.ip_prot.ip[0]);
		debug_printf("ip1: %d\r\n",sensor_parameter_n.sersor_parameter.ip_prot.ip[1]);
		debug_printf("ip1: %d\r\n",sensor_parameter_n.sersor_parameter.ip_prot.ip[2]);
		debug_printf("ip1: %d\r\n",sensor_parameter_n.sersor_parameter.ip_prot.ip[3]);
		debug_printf("port: %d\r\n",sensor_parameter_n.sersor_parameter.ip_prot.prot);
	//获取sensor_id
	if(sensor_parameter_n.sersor_parameter.ip_prot.flag == PARAMETER_WRITE_FLASH_FALG){
		debug_printf("remote ip = %s\r\n",remote_ip);
		sprintf((char*)remote_ip, "%d%s%d%s%d%s%d", sensor_parameter_n.sersor_parameter.ip_prot.ip[0],".",sensor_parameter_n.sersor_parameter.ip_prot.ip[1],".",sensor_parameter_n.sersor_parameter.ip_prot.ip[2],".",sensor_parameter_n.sersor_parameter.ip_prot.ip[3]);
		remote_port = sensor_parameter_n.sersor_parameter.ip_prot.prot;
		debug_printf("remote ip = %s\r\n",remote_ip);
	}
}



/*!
    \brief      						 写入ip addr；
    \param[flashAddr]  			 flash 地址
    \param[network_id] 			 待写入的ip地址和端口
    \retval     			 none
*/
fmc_state_enum write_lora_power_factor_to_flash(uint32_t flashAddr,lora_power_factor_t* lora_power_factor){
	
	//读取falsh数据；
	read_datas_to_flash(SENSOR_PARAMETER_ADDR,sensor_parameter_n.sensor_parameter_buffer,SENSOR_PRARAMETER_SIZEOF);
	//修改数据；
	sensor_parameter_n.sersor_parameter.power_factor.flag = PARAMETER_WRITE_FLASH_FALG;
	sensor_parameter_n.sersor_parameter.power_factor.power_factor = lora_power_factor->power_factor;
	//数据回写
	write_datas_to_flash(flashAddr,SENSOR_PRARAMETER_SIZEOF,sensor_parameter_n.sensor_parameter_buffer);
}

uint32_t read_lora_power_factor_from_flash(){
	uint32_t ret;
	read_datas_to_flash(SENSOR_PARAMETER_ADDR,sensor_parameter_n.sensor_parameter_buffer,SENSOR_PRARAMETER_SIZEOF);
	if(sensor_parameter_n.sersor_parameter.power_factor.flag == PARAMETER_WRITE_FLASH_FALG){
		ret = sensor_parameter_n.sersor_parameter.power_factor.power_factor;
		return ret;
	}else{
		return 0;
	}
}



/*!
    \brief      						 写入channel gain
    \param[flashAddr]  			 flash 地址
    \param[network_id] 			 待写入的ip地址和端口
    \retval     			 none
*/
fmc_state_enum write_channel_gain_to_flash(uint32_t flashAddr,uint8_t channle,uint8_t gain){
	
	fmc_state_enum status = FMC_READY;
	if(channle > CHANNLE_NUM){
		return 0xff;
	}
	//读取falsh数据；
	read_datas_to_flash(CHANNALE_GAIN_ADDR,sensor_gain_n.sensor_gain_buffer,CHANNLE_GAIN_BUFFER);
	//修改数据；
	sensor_gain_n.channle_gain[channle-1].flag = PARAMETER_WRITE_FLASH_FALG;
	sensor_gain_n.channle_gain[channle-1].gain = gain;
	//数据回写
	status = write_datas_to_flash(flashAddr,CHANNLE_GAIN_BUFFER,sensor_gain_n.sensor_gain_buffer);
	
	return status;
}



uint8_t read_channel_gain_from_flash(uint32_t flashAddr,uint8_t channle){
	
	uint32_t  ret = 0;
	if(*(__IO uint32_t*)(flashAddr+((channle-1)*CHANNEL_GAIN_SIZIOF)) == PARAMETER_WRITE_FLASH_FALG){
		ret =  *(__IO uint32_t*)(flashAddr+((channle-1)*CHANNEL_GAIN_SIZIOF)+WORD);
	}else{
		ret = 0;
	}
	return  (uint8_t)ret;

}




uint64_t read_double_words_form_flash(uint32_t flashAddr)
{

	uint64_t  ret = 0;
	ret =  *(__IO uint64_t*)flashAddr;
	return  ret;

}

uint32_t read_word_form_flash(uint32_t flashAddr)
{
	uint32_t  ret = 0;
	ret =  *(__IO uint32_t*)flashAddr;
	return  ret;
}



fmc_state_enum write_Flash_double_Word(uint32_t address, uint64_t Data){
	fmc_state_enum status = 0;
	
	uint32_t data_L = 0,data_H = 0;
	data_L = Data & 0xffffffff;
	data_H = (Data>>32)&0xffffffff;
	
	status = fmc_erase_sector_by_address(address);
	
	if(status == FMC_READY){
		fmc_unlock();
		status = fmc_word_program(address,data_L);
		status = fmc_word_program(address+WORD,data_H);
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
		fmc_lock();
	}else{
		debug_printf("flash addr %0x erase failed\r\n",address);
		return FLASH_ERASE_FAILD;
		
	}
	debug_printf("writefalshword status %d\r\n",status);
	debug_printf("flash addr %0x program succse\r\n",address);
	return status;
}


bool WriteFlashBatteryCapacity(uint32_t batteryCapacity)
{
	fmc_erase_sector_by_address(BATTERY_CAPACITY_ADDR);	
	fmc_unlock();
	
	fmc_word_program(BATTERY_CAPACITY_FLAG,0xaaaaaaaa);
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
	fmc_word_program(BATTERY_CAPACITY_BASE,batteryCapacity);
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
	fmc_lock();

	return true;
}



bool ReLoadSersorId()
{	
	fmc_unlock();
	//获取sensor_id
	sprintf((char*)sensor_id, "%llu", read_double_words_form_flash(SENSOR_ID_ADDR));
	sensor_id[20] = ';';
	fmc_lock();
	return true;
}











bool WriteFlashSleepParam()
{
	uint32_t * temp_sample_time = NULL;	
	uint8_t state = 0;
//	temp_sample_time = readFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t));
	if(*temp_sample_time == 0xaaaa){
		*(temp_sample_time+1) += 1;
		fmc_erase_sector_by_address(RTC_STANDBY_TIME_ADDR);	
//		writeFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t),temp_sample_time);
//		temp_sample_time = readFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t));
		return true;
	}else{
		fmc_erase_sector_by_address(RTC_STANDBY_TIME_ADDR);	
//		writeFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t),(uint32_t*) &sample_time);
//		temp_sample_time = readFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t));
		return false;
	}
}







