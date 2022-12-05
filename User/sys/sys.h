#ifndef _SYS_H_
#define _SYS_H_

#include "gd32f4xx.h"
//#include <stdint.h>

//#define SysTick_Freq  (1000U)  // Hz
//#define SysTick_Tick  (1000U / SysTick_Freq)

//定义一些常用的数据类型短关键字 
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

	 
/*===========================位带操作,实现类似51的GPIO控制功能==========================*/
//具体实现思想，参考《GD32F30x 用户手册》P38、《Cortex M3与M4权威指南》P89、P195和《Cortex M3权威指南(中文)》P87~92

//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIO_GD32F30x_ODR		0x14U
#define GPIOA_ODR_Addr    (GPIOA + GPIO_GD32F30x_ODR)
#define GPIOB_ODR_Addr    (GPIOB + GPIO_GD32F30x_ODR)
#define GPIOC_ODR_Addr    (GPIOC + GPIO_GD32F30x_ODR)
#define GPIOD_ODR_Addr    (GPIOD + GPIO_GD32F30x_ODR)
#define GPIOE_ODR_Addr    (GPIOE + GPIO_GD32F30x_ODR)
#define GPIOF_ODR_Addr    (GPIOF + GPIO_GD32F30x_ODR)   
#define GPIOG_ODR_Addr    (GPIOG + GPIO_GD32F30x_ODR)  
#define GPIOH_ODR_Addr    (GPIOH + GPIO_GD32F30x_ODR)   
#define GPIOI_ODR_Addr    (GPIOI + GPIO_GD32F30x_ODR)
#define GPIOJ_ODR_ADDr    (GPIOJ + GPIO_GD32F30x_ODR)
#define GPIOK_ODR_ADDr    (GPIOK + GPIO_GD32F30x_ODR)

#define GPIO_GD32F30x_IDR		0x10U
#define GPIOA_IDR_Addr    (GPIOA + GPIO_GD32F30x_IDR)
#define GPIOB_IDR_Addr    (GPIOB + GPIO_GD32F30x_IDR)
#define GPIOC_IDR_Addr    (GPIOC + GPIO_GD32F30x_IDR)
#define GPIOD_IDR_Addr    (GPIOD + GPIO_GD32F30x_IDR)
#define GPIOE_IDR_Addr    (GPIOE + GPIO_GD32F30x_IDR) 
#define GPIOF_IDR_Addr    (GPIOF + GPIO_GD32F30x_IDR)
#define GPIOG_IDR_Addr    (GPIOG + GPIO_GD32F30x_IDR) 
#define GPIOH_IDR_Addr    (GPIOH + GPIO_GD32F30x_IDR)
#define GPIOI_IDR_Addr    (GPIOI + GPIO_GD32F30x_IDR)
#define GPIOJ_IDR_Addr    (GPIOJ + GPIO_GD32F30x_IDR)
#define GPIOK_IDR_Addr    (GPIOK + GPIO_GD32F30x_IDR) 

//IO口操作,只对单一的IO口!
//确保n的值小于GPIO_GD32F30x_IDR!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入

#define PJout(n)   BIT_ADDR(GPIOJ_ODR_Addr,n)  //输出 
#define PJin(n)    BIT_ADDR(GPIOJ_IDR_Addr,n)  //输入

#define PKout(n)   BIT_ADDR(GPIOK_ODR_Addr,n)  //输出 
#define PKin(n)    BIT_ADDR(GPIOK_IDR_Addr,n)  //输入






#endif
