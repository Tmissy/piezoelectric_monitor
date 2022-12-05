#ifndef _DCI_ADC_H_
#define _DCI_ADC_H_

#include "port.h"

#define DCI_DR_ADDRESS     (0x50050028U)		//DCI��4�ֽ����ݴ洢��

#define Sampling_number		4096				//DCI-DMA�ɼ�����������

extern u16 battery_DCI_value_arry[Sampling_number];

/*
DCI��ʼ�����ã�
		��������ģʽ
		ʱ�������ش���
		DCI�½��ز���
		ˮƽ�����ڼ�͵�ƽ
		��ֱ�����ڼ�͵�ƽ
		֡Ƶ��Ϊÿ����֡����һ��
		��������ӿڸ�ʽΪ10λ��������
*/
void mcu_dci_config(void);


void mcu_dma_config(void);


#endif



