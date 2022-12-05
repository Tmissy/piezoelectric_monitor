#include "fmc_flash.h"
#include "usart_config.h"
#include "net_protocol.h"
#include "rtc_config.h"
#include "lora.h"

#define FMC_PAGE_SIZE           ((uint16_t)0x2000U)  //8k
#define FMC_WRITE_START_ADDR    ((uint32_t)0x0800A000U)
#define FMC_WRITE_END_ADDR      ((uint32_t)0x0800BFFFU)

#define SENSORID_USART   USART1

uint32_t *ptrd;
uint32_t address = 0x00;
uint32_t data0   = 0x01234567U;
uint32_t data1   = 0xd583179bU;

static uint32_t data[50] = {0};
/* calculate the number of page to be programmed/erased */
uint32_t PageNum = (FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) / FMC_PAGE_SIZE;
/* calculate the number of page to be programmed/erased */
uint32_t WordNum = ((FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) >> 2);

static uint8_t flashData[12] = {0};


/*!
    \brief      erase fmc pages from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_pages(uint32_t page_address)
{

    /* unlock the flash program/erase controller */
    fmc_unlock();
    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* erase the flash pages */
    fmc_sector_erase(page_address);
    /* lock the main FMC after the erase operation */
    fmc_lock();
}


/*!
    \brief      program fmc word by word from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program(void)
{
    /* unlock the flash program/erase controller */
    fmc_unlock();

    address = FMC_WRITE_START_ADDR;

    /* program flash */
    while(address < FMC_WRITE_END_ADDR){
        fmc_word_program(address, data0);
        address += 4;
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
    }

    /* lock the main FMC after the program operation */
    fmc_lock();
}



/*!
    \brief      check fmc erase result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_pages_check(void)
{
    uint32_t i;

    ptrd = (uint32_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been erased */
    for(i = 0; i < WordNum; i++){
        if(0xFFFFFFFF != (*ptrd)){

            break;
        }else{
            ptrd++;
        }
    }
}

/*!
    \brief      check fmc program result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program_check(void)
{
    uint32_t i;

    ptrd = (uint32_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been programmed */
    for(i = 0; i < WordNum; i++){
        if((*ptrd) != data0){

            break;
        }else{
            ptrd++;
        }
    }
}

void writeFlashSensorID() {
		uint32_t writeFlashData = 0;

		uint32_t sersoridH = 0;
		uint32_t sersoridL = 0;
	    /* unlock the flash program/erase controller */
    fmc_unlock();

    address = SENSOR_ID_ADDR;
	
		while(*(__IO uint32_t*)SENSOR_ID_ADDR != 0xaaaaaaaa){
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
		
			debug_printf("sensorID: %x\r\n",writeFlashData);
		fmc_erase_pages(SENSOR_ID_ADDR);	
			fmc_unlock();
		fmc_word_program(SENSOR_ID_FLAG, writeFlashData);
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
		fmc_word_program(SENSOR_ID_ADDRH, sersoridH);
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
		fmc_word_program(SENSOR_ID_ADDRL, sersoridL);
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
		fmc_lock();
		printf("flash write to successful");
		debug_printf("sensorID: %llu\r\n",readFlashDoubleWord(SENSOR_ID_ADDRL));
		debug_printf("sensorID: %x\r\n",*(__IO uint32_t*)SENSOR_ID_ADDR);
		ReLoadSersorId();
		fmc_lock();
	}
}

uint64_t readFlashDoubleWord(uint32_t flashAddr)
{

	uint64_t  ret = 0;
	ret =  *(__IO uint64_t*)flashAddr;
	return  ret;

}

uint32_t readFlashWord(uint32_t flashAddr)
{

	uint32_t  ret = 0;
	fmc_unlock();
	ret =  *(__IO uint32_t*)flashAddr;
	fmc_lock();
	return  ret;

}

uint32_t* readFlash_X_Word(uint32_t flashAddr,uint8_t num){
	uint8_t i = 0;
	if(!((num > 0)&&(num <= 32))){
		return 0 ;
	}
	fmc_unlock();
	for(i = 0;i<num;i++){
		data[i] = *(__IO uint32_t*)(flashAddr+i*WORD);
	}
	fmc_lock();
	return data;
}
bool writeFlash_X_Word(uint32_t flashAddr,uint8_t num,uint32_t* data){
	uint8_t i = 0;
	
	if(!((num > 0)&&(num <= 32))){
		return 0;
	}
	fmc_erase_pages(flashAddr);	
	fmc_unlock();

	for(i = 0;i<num;i++){
		fmc_word_program(flashAddr+i*WORD,*(data+i));
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
	}
	fmc_lock();
	return 1;
}

bool WriteFlashBatteryCapacity(uint32_t batteryCapacity)
{
	fmc_erase_pages(BATTERY_CAPACITY_ADDR);	
	fmc_unlock();
	
	fmc_word_program(BATTERY_CAPACITY_FLAG,0xaaaaaaaa);
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
	fmc_word_program(BATTERY_CAPACITY_BASE,batteryCapacity);
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
	fmc_lock();

	return true;
}

void writeFlashWord(uint32_t Address, uint64_t Data){
	uint32_t sensorIdL = 0,sensorIdH = 0;
	sensorIdL = Data & 0xffffffff;
	sensorIdH = (Data>>32)&0xffffffff;
	fmc_unlock();
	fmc_word_program(Address,sensorIdL);

	fmc_word_program(Address+WORD,sensorIdH);
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR );
	fmc_lock();
}

bool ReLoadSersorId()
{	
	fmc_unlock();
	//获取sensor_id
	sprintf((char*)sensor_id, "%llu", readFlashDoubleWord(SENSOR_ID_ADDRL));
	sensor_id[20] = ';';
	fmc_lock();
	return true;
}






//ip_port[0]:flag 0xaaaa;
//ip_port[1-4]:ipaddr ;
//ip_port[5]:ip port;
bool ReLoad_Ip_Port()
{	
	uint32_t * ip_port = NULL;	
	ip_port = readFlash_X_Word(NB_IP_PROT_ADDR,6);
		debug_printf("flag: %x\r\n",*ip_port);
		debug_printf("ip1: %d\r\n",*(ip_port+1));
		debug_printf("ip1: %d\r\n",*(ip_port+2));
		debug_printf("ip1: %d\r\n",*(ip_port+3));
		debug_printf("ip1: %d\r\n",*(ip_port+4));
		debug_printf("port: %d\r\n",*(ip_port+5));
	//获取sensor_id
	if(*ip_port == 0xaaaa){
		debug_printf("remote ip = %s\r\n",remote_ip);
		sprintf((char*)remote_ip, "%d%s%d%s%d%s%d", (uint32_t)*(ip_port+1),".",(uint32_t)*(ip_port+2),".",(uint32_t)*(ip_port+3),".",(uint32_t)*(ip_port+4));
		remote_port = *(ip_port+5);
		debug_printf("remote ip = %s\r\n",remote_ip);
	}
	return true;
}



void write_LoRa_parament(uint32_t power_factor){
	lora_parament.flag = 0xaaaaaaaa;
	lora_parament.power_factor = power_factor;
	writeFlash_X_Word(LORA_PARAMENTER,sizeof(LoRa_parament_t)/sizeof(uint32_t),(uint32_t*) &lora_parament);
}
void reload_lora_parameter(){
	if(*(__IO uint32_t*)LORA_PARAMENTER == 0xaaaaaaaa){
		lora_parament.power_factor = *(__IO uint32_t*)LORA_POWER_FACTOR;
	}
}

bool WriteFlashSleepParam()
{
	uint32_t * temp_sample_time = NULL;	
	uint8_t state = 0;
	temp_sample_time = readFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t));
	if(*temp_sample_time == 0xaaaa){
		*(temp_sample_time+1) += 1;
		fmc_erase_pages(RTC_STANDBY_TIME_ADDR);	
		writeFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t),temp_sample_time);
		temp_sample_time = readFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t));
		return true;
	}else{
		fmc_erase_pages(RTC_STANDBY_TIME_ADDR);	
		writeFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t),(uint32_t*) &sample_time);
		temp_sample_time = readFlash_X_Word(RTC_STANDBY_TIME_ADDR,sizeof(sleep_time_t)/sizeof(uint32_t));
		return false;
	}
}







