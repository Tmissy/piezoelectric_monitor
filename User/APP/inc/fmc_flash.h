#ifndef FMC_FLASH_H
#define FMC_FLASH_H

#include "gd32f4xx.h"
#include "stdbool.h"
#define WORD              (4)

#define PARAMETER_WRITE_FLASH_FALG  (0xaaaaaaaa)

typedef struct
{
	uint32_t flag;
	uint32_t ip[4];
	uint32_t prot;
}ip_prot_t;

typedef struct
{
	uint32_t flag;
	uint32_t power_factor;
}lora_power_factor_t;

typedef struct
{
	uint32_t flag;
	uint32_t sensorid[2];
}sensor_id_t;

typedef struct{
	uint32_t flag;
	uint32_t network_id;
}network_id_t;

typedef struct{
	//sensorID
	sensor_id_t sensor_id;
	//networkid
	network_id_t network_id;
	//IP 地址
	ip_prot_t ip_prot;
	//lora power
	lora_power_factor_t power_factor;

}sensor_parameter_t;



#define SENSOR_PRARAMETER_SIZEOF   (sizeof(sensor_parameter_t))

#define BATTERY_CAPACITY_ADDR   (0x0800c000)
#define BATTERY_CAPACITY_FLAG  (BATTERY_CAPACITY_ADDR)
#define BATTERY_CAPACITY_BASE  (BATTERY_CAPACITY_ADDR +WORD)
#define CHANNALE_PRAREMETER    	 (0x08010000)
#define RTC_STANDBY_TIME_ADDR    (0x08020000)
#define SENSOR_PARAMETER_ADDR    (0x08040000)
#define SENSOR_ID_ADDR           (SENSOR_PARAMETER_ADDR+WORD)
#define NETWORK_ID_FLAG_ADDR          (SENSOR_PARAMETER_ADDR+sizeof(sensor_id_t))
#define NETWORK_ID_ADDR          (SENSOR_PARAMETER_ADDR+sizeof(sensor_id_t)+WORD)


//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((unsigned int)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((unsigned int)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((unsigned int)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((unsigned int)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((unsigned int)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((unsigned int)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((unsigned int)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((unsigned int)0x08060000) 	//扇区7起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((unsigned int)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((unsigned int)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((unsigned int)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((unsigned int)0x080E0000) 	//扇区11起始地址,128 Kbytes  


void writeFlashSensorID(void);
uint64_t read_sensorid_form_flash(uint32_t flashAddr);

fmc_state_enum write_network_id_to_flash(uint32_t flashAddr,uint32_t network_id);
uint32_t read_network_id_from_flash(uint32_t flashAddr);

fmc_state_enum write_ip_prot_to_flash(uint32_t flashAddr,ip_prot_t* ip_prot);
void reload_ip_port(uint32_t flashAddr);

fmc_state_enum write_lora_power_factor_to_flash(uint32_t flashAddr,lora_power_factor_t* lora_power_factor);
uint32_t read_lora_power_factor_from_flash();

fmc_state_enum write_Flash_double_Word(uint32_t address, uint64_t Data);

fmc_state_enum fmc_erase_sector_by_address(uint32_t page_address);
uint64_t read_double_words_form_flash(uint32_t flashAddr);
uint32_t read_word_form_flash(uint32_t flashAddr);
//uint32_t* readFlash_X_Word(uint32_t flashAddr,uint8_t max_channale_num);

//bool writeFlash_X_Word(uint32_t flashAddr,uint8_t num,uint32_t* data);
fmc_state_enum write_Flash_double_Word(uint32_t Address, uint64_t Data);
bool WriteFlashBatteryCapacity(uint32_t batteryCapacity);
void reload_lora_parameter(void);
bool ReLoadSersorId(void);


bool WriteFlashSleepParam(void);

#endif /* GD32E50X_EXMC_H */

