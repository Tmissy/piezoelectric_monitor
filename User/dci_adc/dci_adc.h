#ifndef _DCI_ADC_H_
#define _DCI_ADC_H_

#include "port.h"

#define DCI_DR_ADDRESS     (0x50050028U)		//DCI的4字节数据存储器

#define Sampling_number		4096				//DCI-DMA采集传输数据量

extern u16 battery_DCI_value_arry[Sampling_number];

/*
DCI初始化配置：
		连续捕获模式
		时钟上升沿触发
		DCI下降沿捕获
		水平消隐期间低电平
		垂直消隐期间低电平
		帧频率为每隔三帧捕获一次
		数码相机接口格式为10位并行数据
*/
void mcu_dci_config(void);


void mcu_dma_config(void);


#endif



