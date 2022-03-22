/*! ----------------------------------------------------------------------------
 * @file	deca_spi.c
 * @brief	SPI access functions
 *
 * @attention
 *
 * Copyright 2013 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */
#include <string.h>

#include "deca_spi.h"
#include "deca_sleep.h"
#include "deca_device_api.h"
#include "port.h"
#include "dma.h"

volatile int read_dma_flag=0;
volatile int write_dma_flag=0;
volatile int read_error_flag=0;
volatile int write_error_flag=0;

static uint8 tx_null[1023] ={0} ;
static uint8 read_null[1023] ={0} ;

int writetospi_serial( uint16_t headerLength,
			   	    const uint8_t *headerBuffer,
					uint32_t bodylength,
					const uint8_t *bodyBuffer
				  );

int readfromspi_serial( uint16_t	headerLength,
			    	 const uint8_t *headerBuffer,
					 uint32_t readlength,
					 uint8_t *readBuffer );
int writetospi_dma( uint16_t headerLength,
			   	    const uint8_t *headerBuffer,
					uint32_t bodylength,
					const uint8_t *bodyBuffer
				  );

int readfromspi_dma( uint16_t	headerLength,
			    	 const uint8_t *headerBuffer,
					 uint32_t readlength,
					 uint8_t *readBuffer );
/*! ------------------------------------------------------------------------------------------------------------------
 * Function: openspi()
 *
 * Low level abstract function to open and initialise access to the SPI device.
 * returns 0 for success, or -1 for error
 */
int openspi(/*SPI_TypeDef* SPIx*/)
{
	// done by port.c, default SPI used is SPI1

	return 0;

} // end openspi()

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: closespi()
 *
 * Low level abstract function to close the the SPI device.
 * returns 0 for success, or -1 for error
 */
int closespi(void)
{
	while (port_SPIx_busy_sending()); //wait for tx buffer to empty

	port_SPIx_disable();

	return 0;

} // end closespi()

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: writetospi()
 *
 * Low level abstract function to write to the SPI
 * Takes two separate byte buffers for write header and write data
 * returns 0 for success, or -1 for error
 */
int writetospi_serial
(
    uint16_t       headerLength,
    const uint8_t *headerBuffer,
    uint32_t       bodylength,
    const uint8_t *bodyBuffer
)
{

	int i=0;

    decaIrqStatus_t  stat ;

    stat = decamutexon() ;

    SPIx_CS_GPIO->BRR = SPIx_CS;

    for(i=0; i<headerLength; i++)
    {
    	SPIx->DR = headerBuffer[i];

    	while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

    	SPIx->DR ;
    }

    for(i=0; i<bodylength; i++)
    {
     	SPIx->DR = bodyBuffer[i];

    	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

		SPIx->DR ;
	}

    SPIx_CS_GPIO->BSRR = SPIx_CS;

    decamutexoff(stat) ;

    return 0;
} // end writetospi()

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: writetospi()
 *
 * Low level abstract function to write to the SPI
 * Takes two separate byte buffers for write header and write data
 * returns 0 for success, or -1 for error
 */
int writetospi_dma
(
    uint16_t       headerLength,
    const uint8_t *headerBuffer,
    uint32_t       bodylength,
    const uint8_t *bodyBuffer
)
{
		int i=0;
    decaIrqStatus_t  stat ;

    stat = decamutexon() ;
		write_dma_flag=0;
		write_error_flag=0;
		SPIx_CS_GPIO->BRR = SPIx_CS;
	
    for(i=0; i<headerLength; i++)
    {
    	SPIx->DR = headerBuffer[i];

    	while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

    	SPIx->DR ;
    }
//		while((DMA_GetFlagStatus(DMA1_FLAG_TC3)||DMA_GetFlagStatus(DMA1_FLAG_TE3)) == (uint16_t)RESET)
//			;
//		if((DMA_GetFlagStatus(DMA1_FLAG_TE3)) != (uint16_t)RESET)
//		{
//			DMA_ClearFlag(DMA1_FLAG_TE3);
//			write_error_flag=1;
//			SPIx_CS_GPIO->BSRR = SPIx_CS;
//		}
//		else
//		{
//			DMA_ClearFlag(DMA1_FLAG_TC3);
			DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, ENABLE);
			DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);                                   //开启 DMA1_Channel3 传输完成中断
			write_dma_flag=1; //开始写数据
			SPI1_Send( bodyBuffer,bodylength);
			SPI1_Recive(read_null,bodylength);
//		}
//		while((DMA_GetFlagStatus(DMA1_FLAG_TC3)||DMA_GetFlagStatus(DMA1_FLAG_TE3)) == (uint16_t)RESET)
//			;
//		DMA_ClearFlag(DMA1_FLAG_TC3);
//		DMA_ClearFlag(DMA1_FLAG_TE3);
//    SPIx_CS_GPIO->BSRR = SPIx_CS;
    decamutexoff(stat) ;

    return 0;
} // end writetospi()

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: writetospi()
 *
 * Low level abstract function to write to the SPI
 * Takes two separate byte buffers for write header and write data
 * returns 0 for success, or -1 for error
 */


/*! ------------------------------------------------------------------------------------------------------------------
 * Function: readfromspi()
 *
 * Low level abstract function to read from the SPI
 * Takes two separate byte buffers for write header and read data
 * returns the offset into read buffer where first byte of read data may be found,
 * or returns -1 if there was an error
 */
int readfromspi_serial
(
    uint16_t       headerLength,
    const uint8_t *headerBuffer,
    uint32_t       readlength,
    uint8_t       *readBuffer
)
{

	int i=0;

    decaIrqStatus_t  stat ;

    stat = decamutexon() ;

    /* Wait for SPIx Tx buffer empty */
    //while (port_SPIx_busy_sending());

    SPIx_CS_GPIO->BRR = SPIx_CS;

    for(i=0; i<headerLength; i++)
    {
    	SPIx->DR = headerBuffer[i];

     	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

     	readBuffer[0] = SPIx->DR ; // Dummy read as we write the header
    }

    for(i=0; i<readlength; i++)
    {
    	SPIx->DR = 0;  // Dummy write as we read the message body

    	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);
 
	   	readBuffer[i] = SPIx->DR ;//port_SPIx_receive_data(); //this clears RXNE bit
    }

    SPIx_CS_GPIO->BSRR = SPIx_CS;

    decamutexoff(stat) ;

    return 0;
} // end readfromspi()

int readfromspi_dma
	
(
    uint16_t       headerLength,
    const uint8_t *headerBuffer,
    uint32_t       readlength,
    uint8_t       *readBuffer
)
{
		int i=0;
    decaIrqStatus_t  stat ;

    stat = decamutexon() ;
		read_error_flag=0;
		read_dma_flag=0;

    /* Wait for SPIx Tx buffer empty */
    //while (port_SPIx_busy_sending());
    SPIx_CS_GPIO->BRR = SPIx_CS;

    for(i=0; i<headerLength; i++)
    {
    	SPIx->DR = headerBuffer[i];

     	while((SPIx->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);

     	readBuffer[0] = SPIx->DR ; // Dummy read as we write the header
    }
//		while((DMA_GetFlagStatus(DMA1_FLAG_TC3)||DMA_GetFlagStatus(DMA1_FLAG_TE3)) == (uint16_t)RESET)
//			;
//			DMA_ITConfig(DMA1_Channel2, DMA_IT_TE, ENABLE);
//			DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);                                   //开启 DMA1_Channel3 传输完成中断
//			read_dma_flag=1; //开始读数据
//		if((DMA_GetFlagStatus(DMA1_FLAG_TE3)) != (uint16_t)RESET)
//		{
//			DMA_ClearFlag(DMA1_FLAG_TE3);
//			read_error_flag=1;
//			SPIx_CS_GPIO->BSRR = SPIx_CS;
//		}
//		else
//		{
//			DMA_ClearFlag(DMA1_FLAG_TC3);
			DMA_ITConfig(DMA1_Channel2, DMA_IT_TE, ENABLE);
			DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);                                   //开启 DMA1_Channel3 传输完成中断
			read_dma_flag=1; //开始读数据
			SPI1_Recive( readBuffer,readlength) ;
			SPI1_Send( tx_null,readlength) ;			
//		}
    decamutexoff(stat) ;

    return 0;
} // end readfromspi()
