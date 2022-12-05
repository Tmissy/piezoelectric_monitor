#ifndef GPIO_CONFIG_H
#define GPIO_CONFIG_H

#include "gd32f4xx.h"

/* POWER CONTORL*/
#define  POWER_SYSTEM_EN_PROT      GPIOE

#define  POWER_SYSTEM_EN_PIN       GPIO_PIN_6

/* DEBUG USART*/
#define  DEBUG_USART_TX_PROT      GPIOA
#define  DEBUG_USART_RX_PORT      GPIOA

#define  DEBUG_UASRT_TX_PIN       GPIO_PIN_2
#define  DEBUG_UASRT_RX_PIN       GPIO_PIN_3

/* NB  USART*/
#define  NB_USART_TX_PROT      GPIOD
#define  NB_USART_RX_PORT      GPIOD

#define  NB_UASRT_TX_PIN       GPIO_PIN_8
#define  NB_UASRT_RX_PIN       GPIO_PIN_9

#define  NB_RST_GPIO_PROT      				GPIOB
#define  NB_RST_PIN            				GPIO_PIN_14
#define  NB_WAKE_GPIO_PROT            GPIOB
#define  NB_WAKE_PIN                  GPIO_PIN_15


/*LoRa   SPI   PORT */
#define  LORA_SPI_MOSI_PORT				GPIOB
#define  LORA_SPI_MISO_PORT				GPIOB
#define  LORA_SPI_SCK_PORT				GPIOB
#define  LORA_SPI_NSS_PORT				GPIOB
#define  LORA_RST_PROT						GPIOB
#define  LORA_DIO0_PORT						GPIOB

#define  LORA_SPI_MOSI_PIN				GPIO_PIN_15
#define  LORA_SPI_MISO_PIN				GPIO_PIN_14
#define  LORA_SPI_SCK_PIN					GPIO_PIN_13
#define  LORA_SPI_NSS_PIN					GPIO_PIN_12
#define  LORA_RST_PIN							GPIO_PIN_11
#define  LORA_DIO0_PIN						GPIO_PIN_10

/*TEMPERATE  SPI PORT*/
#define  MAX31865_SPI_MOSI_SDI_PORT							GPIOB
#define  MAX31865_SPI_MISO_SDO_PORT							GPIOB
#define  MAX31865_SPI_SCK__PORT									GPIOB
#define  MAX31865_SPI_CSN_PORT									GPIOB
#define  MAX31865_DRDY_TG_PROT									GPIOB


#define  MAX31865_SPI_MOSI_SDI_PIN							GPIO_PIN_2
#define  MAX31865_SPI_MISO_SDO_PIN							GPIO_PIN_4
#define  MAX31865_SPI_SCK__PIN									GPIO_PIN_3
#define  MAX31865_SPI_CSN_PIN										GPIO_PIN_5
#define  MAX31865_DRDY_TG_PIN										GPIO_PIN_1


/*BATEERY    VOLT   DETECT */
#define  BATTERY_VOLT_DETECT_PROT      GPIOC
#define  BATTERY_VOLT_DETECT_PIN       GPIO_PIN_0

/* DRIVER  SIGNAL */
#define POWER_PWM1_PORT         		GPIOA
#define POWER_PWM2_PORT 						GPIOB

#define POWER_PWM1_PIN         		GPIO_PIN_7
#define POWER_PWM2_PIN 						GPIO_PIN_0

/* AD8330 CONTORL */
#define  VGA_EN_PROT     				 	GPIOC
#define  GAIN_CONTROL_PROT      	GPIOA

#define  VGA_EN_PIN       				GPIO_PIN_7
#define  GAIN_CONTROL_PIN      		GPIO_PIN_5


/* DCI AD acquisiton  */
#define AD_DCI_DATA0_PORT         GPIOA
#define AD_DCI_DATA1_PORT 				GPIOA
#define AD_DCI_DATA2_PORT 				GPIOC
#define AD_DCI_DATA3_PORT 				GPIOE
#define AD_DCI_DATA4_PORT 				GPIOC
#define AD_DCI_DATA5_PORT 				GPIOD
#define AD_DCI_DATA6_PORT 				GPIOB
#define AD_DCI_DATA7_PORT 				GPIOB
#define AD_DCI_DATA8_PORT 				GPIOC
#define AD_DCI_DATA9_PORT 				GPIOC
#define AD_DCI_DATA10_OR_PORT 		GPIOD
#define AD_DCI_CLK_PORT 					GPIOA
#define AD_DCI_VSYNC_PORT 				GPIOB
#define AD_DCI_HSYNC_PORT 				GPIOA
#define AD_DCI_CLK_PORT 					GPIOA
#define AD_POWER_DOWN_PORT 				GPIOB


#define AD_DCI_DATA0_PIN       		GPIO_PIN_9
#define AD_DCI_DATA1_PIN					GPIO_PIN_10
#define AD_DCI_DATA2_PIN 					GPIO_PIN_8
#define AD_DCI_DATA3_PIN 					GPIO_PIN_1
#define AD_DCI_DATA4_PIN 					GPIO_PIN_11
#define AD_DCI_DATA5_PIN 					GPIO_PIN_3
#define AD_DCI_DATA6_PIN 					GPIO_PIN_8
#define AD_DCI_DATA7_PIN 					GPIO_PIN_9
#define AD_DCI_DATA8_PIN 					GPIO_PIN_10
#define AD_DCI_DATA9_PIN 					GPIO_PIN_12
#define AD_DCI_DATA10_OR_PIN 			GPIO_PIN_6
#define AD_DCI_CLK_PIN						GPIO_PIN_6
#define AD_DCI_VSYNC_PIN 					GPIO_PIN_7
#define AD_DCI_HSYNC_PIN 					GPIO_PIN_4
#define AD_POWER_DOWN_PIN					GPIO_PIN_6


//NB_LORA SEL PROT
#define  NB_LORA_SEL_GPIO_PROT           GPIOE
#define  NB_LORA_SEL_PIN                 GPIO_PIN_15

void gpio_config(void);
void gpio_out_config(uint32_t gpio_periph,uint32_t pin,uint32_t mode,uint32_t pull_up_down,uint8_t otype,uint32_t speed);
void gpio_in_config(uint32_t gpio_periph,uint32_t pin,uint32_t mode,uint32_t pull_up_down);

void powerOff(void);
void powerOn(void);

#endif


