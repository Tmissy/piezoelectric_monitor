#ifndef FMC_FLASH_H
#define FMC_FLASH_H

#include "gd32f4xx.h"
#include "stdbool.h"
#define WORD              (4)

#define FMC_PRAREMETER_WRITE_START_ADDR    ((uint32_t)0x0803C000U)
#define FMC_PRAREMETER_WRITE_END_ADDR      ((uint32_t)0x0803DFFFU)
#define CHANNALE_PRAREMETER    	  					(0x0803C000U)


#define BATTERY_CAPACITY_ADDR   (0x08030000)
#define BATTERY_CAPACITY_FLAG  (BATTERY_CAPACITY_ADDR)
#define BATTERY_CAPACITY_BASE  (BATTERY_CAPACITY_ADDR +WORD)

#define LORA_PARAMENTER        (0x08032000U)
#define LORA_POWER_FACTOR			 (LORA_PARAMENTER+WORD)

#define FWDGT_RESET_FLAG  				(0x08034000U)
#define RTC_STANDBY_TIME_ADDR     (0x08036000)
#define NB_IP_PROT_ADDR    				(0x08038000)

#define SENSOR_ID_ADDR    				(0x0803A000)
#define SENSOR_ID_FLAG    (SENSOR_ID_ADDR)
#define LORA_ID_ADDR   	  (SENSOR_ID_ADDR+WORD)
#define SENSOR_ID_ADDRL   (SENSOR_ID_ADDR+WORD*2)
#define SENSOR_ID_ADDRH   (SENSOR_ID_ADDR+WORD*3)

#define FMC_PRAREMETER_WRITE_START_ADDR    ((uint32_t)0x0803C000U)
#define FMC_PRAREMETER_WRITE_END_ADDR      ((uint32_t)0x0803DFFFU)
#define CHANNALE_PRAREMETER    	  					(0x0803C000U)

#define LAST_PAGE_BASE    (0x0803E000U)
#define FMC_NETWORK_WRITE_START_ADDR    ((uint32_t)0x0803E000U)
#define NETWORK_ID_BASE  (LAST_PAGE_BASE)



void writeFlashSensorID(void);
void fmc_erase_pages(uint32_t page_address);
uint64_t readFlashDoubleWord(uint32_t flashAddr);
uint32_t readFlashWord(uint32_t flashAddr);
uint32_t* readFlash_X_Word(uint32_t flashAddr,uint8_t max_channale_num);
bool writeFlash_X_Word(uint32_t flashAddr,uint8_t num,uint32_t* data);
void writeFlashWord(uint32_t Address, uint64_t Data);
bool WriteFlashBatteryCapacity(uint32_t batteryCapacity);
void reload_lora_parameter(void);
bool ReLoadSersorId(void);
bool ReLoad_Ip_Port(void);

bool WriteFlashSleepParam(void);

#endif /* GD32E50X_EXMC_H */

