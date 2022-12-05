#include "spi_config.h"

/*!
    \brief      configure the SPI1(LoRa) peripheral 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lora_spi_config(void)
{
		rcu_periph_clock_enable(RCU_SPI1);
		
    spi_parameter_struct  spi_init_struct;

    /* configure SPI1 parameter */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_32;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;;
    spi_init(SPI1, &spi_init_struct);
		spi_enable(SPI1);

}

void thermocouple_spi_config(void){
	
	rcu_periph_clock_enable(RCU_SPI3);

    spi_parameter_struct  spi_init_struct;

    /* configure SPI1 parameter */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_32;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;;	
	
	spi_init(SPI3, &spi_init_struct);
	spi_enable(SPI3);
}
