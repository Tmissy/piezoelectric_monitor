#include "ds18b20.h"
#include "gpio.h"
#include "usart.h"
#include "systick.h"


#define  DS_H   gpio_bit_set(DS18B20_GPIO_PROT,DS18B20_PIN) 
#define  DS_L  	gpio_bit_reset(DS18B20_GPIO_PROT,DS18B20_PIN) 

#define READ_ROM   					0x33
#define MATCH_ROM  					0x55
#define SKIP_ROM   					0xcc
#define SEARCH_ROM 					0xf0
#define ALARM_SEARCH 				0xec

#define WRITE_SCRATCHPAD 		0x4e
#define READ_SCRATCHPAD  		0xbe
#define COPY_SCRATCHPAD  		0x48
#define CONVERT_TEMPERATURE 0x44
#define RECALL_EPROM  			0xb8
#define READ_POWER_SUPPLY   0xb4

/**
    @Brief ：  void delay_us(uint8_t Time)
    @Param：   无
    @Function：us延时
    @Return：  无
    @Author：  自由eπ
  @Note：    无 
*/

void delay_us(uint16_t time){

//	uint16_t diff = TIMER_CNT(TIMER5);
	
	TIMER_CNT(TIMER5) = 0;
	while(TIMER_CNT(TIMER5)<  time){
	}
}

/**
    @Brief ：  static void Set_DS_Pin_In(void)
    @Param：   无
    @Function：对温度传感器的数据引脚配置为输入模式
    @Return：  无
    @Author：  自由eπ
  @Note：    无 
*/
static void Set_DS_Pin_In(void){
	
	rcu_periph_clock_enable(RCU_GPIOE);
	
	gpio_init(GPIOE, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_MAX, GPIO_PIN_2);

}

/**
    @Brief ：  static void Set_DS_Pin_Out(void)
    @Param：   无
    @Function：对温度传感器的数据引脚配置为开漏输出模式
    @Return：  无
    @Author：  自由eπ
  @Note：    无 
*/
static void Set_DS_Pin_Out(void){
	rcu_periph_clock_enable(RCU_GPIOE);
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_2);
}

/**
    @Brief ：  void DS_Reset(void)
    @Param：   无
    @Function：对温度传感器进行复位
    @Return：  无
    @Author：  自由eπ
  @Note：    无 
*/
void DS_Reset(void){
    Set_DS_Pin_Out();
    DS_L;//将DS引脚拉低
//		delay_1ms(1);
	 delay_us (750);
//	 delay_us (150);
//	 delay_us (150);
//	 delay_us (150);
//	 delay_us (150);
//    delay_us (750);
    DS_H;//将DS引脚拉高
    delay_us (15);
}


/**
    @Brief ：  uint8_t Check_DS(void)
    @Param：   无
    @Function：检查温度传感器DS18B20是否存在
    @Return：  0->存在；1->不存在
    @Author：  自由eπ
  @Note：    无 
*/
uint8_t Check_DS(void){
    uint8_t Pulse_Time=0;
    Set_DS_Pin_In();//Set DSPin Into Push Up Input
    //Waitting for the low pulse from DS18B20 and can keep 60-240us 
    while(gpio_input_bit_get(DS18B20_GPIO_PROT, DS18B20_PIN)&&Pulse_Time<100){
        Pulse_Time++;
        delay_us (1);
    }
    if( Pulse_Time>=100)
        return 1;
    else{
        Pulse_Time= 0;
		}
    //The processing of out time
        while(!gpio_input_bit_get(DS18B20_GPIO_PROT, DS18B20_PIN)&& Pulse_Time<240){
        Pulse_Time++;
        delay_us (1);
    }
    if( Pulse_Time>=240)
        return 1;
    else
        return 0;
}



/**
    @Brief ：  static uint8_t DS_Read_Bit(void)
    @Param：   无
    @Function：从温度传感器中获取一个比特位的数据
    @Return：  0->   ；1->    
    @Author：  自由eπ
  @Note：    无 
*/
static  uint8_t DS_Read_Bit(void){
    uint8_t data;
    Set_DS_Pin_Out();
		gpio_bit_write(DS18B20_GPIO_PROT,DS18B20_PIN,RESET);//Set DS pin into reset
    delay_us (10);
    
    Set_DS_Pin_In();
    if(gpio_input_bit_get(DS18B20_GPIO_PROT, DS18B20_PIN) == SET)
        data=1;
    else 
        data=0;
    delay_us(45);
    return data ;
}

/**
    @Brief ：  static uint8_t DS_Read_Byte(void)
    @Param：   无
    @Function：从温度传感器中获取一个字节的数据，低位先行
    @Return：  0->   ；1->    
    @Author：  自由eπ
  @Note：    无 
*/
static  uint8_t DS_Read_Byte(void){
    
    uint8_t i,j,data=0;
    for(i=0;i<8;i++){
        j=DS_Read_Bit();
        data=(data)|(j<<i);
    }
    
    return data ;
}


/**
    @Brief ：  static  void  Write_DS_Byte(uint8_t data)
    @Param：   uint8_t data
    @Function：向温度传感器中写入一个字节的数据，低位先行
    @Return：  无 
    @Author：  自由eπ
  @Note：    无 
*/
static  void  DS_Write_Byte(uint8_t data){
    uint8_t Test_Bit;
    
    Set_DS_Pin_Out();
    for(Test_Bit=0x01; Test_Bit !=0; Test_Bit<<=1){
        //Write 0
        if((data&Test_Bit)==RESET){
            DS_L;//Set DS Pin into reset
            delay_us(70);
            DS_H;
            delay_us(2);
        }
        //Write 1
        else {
            DS_L;//Set DS Pin into reset
            delay_us(9);
            DS_H;
            delay_us (55);
        }
    }
}		



/**
    @Brief ：  uint8_t DS_Init(void)
    @Param：   无
    @Function：对温度传感器进行初始化
    @Return：  无
    @Author：  自由eπ
  @Note：    无
*/
uint8_t DS_Init(void){
    DS_Reset();
    return Check_DS() ;
}

/**
    @Brief ：  float Get_DS_Tempture(void)
    @Param：   无
    @Function：获取温度
    @Return：  从DS18B20上读取到的温度值
    @Author：  自由eπ
  @Note：    此获取方法为跳过 ROM 读取,适合于总线上只有一个设备
*/
float Get_DS_Temperature(void){
    
    uint8_t tpmsb,tplsb;
    short s_tem;
    float f_tem;
    
    DS_Reset ();
    Check_DS ();
    DS_Write_Byte (SKIP_ROM );       //Skip ROM
    DS_Write_Byte (CONVERT_TEMPERATURE );   //Let transition temperature
		delay_us (350);
    DS_Reset ();
    Check_DS ();
    DS_Write_Byte (SKIP_ROM );       //Skip ROM
    DS_Write_Byte (READ_SCRATCHPAD); //Get temperature from DS18B20
    
    tplsb=DS_Read_Byte();
    tpmsb=DS_Read_Byte();
    
    s_tem = tpmsb<<8;
    s_tem = s_tem | tplsb;
    debug_printf("s_tem %x\r\n",s_tem);
    if( s_tem < 0 )        /* 负温度 */
        f_tem = (~s_tem+1) * 0.0625;    
    else
        f_tem = s_tem * 0.0625;
    
    return f_tem; 

}