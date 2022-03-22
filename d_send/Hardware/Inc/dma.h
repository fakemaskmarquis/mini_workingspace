#ifndef __DMA_H
#define	__DMA_H	   
#include "stm32f10x.h"
							    					   
void SPI1_TX_DMA_Configuration( u32 cmar,u16 cndtr );

void SPI1_RX_DMA_Configuration( u32 cmar,u16 cndtr );

void SPI1_Send(const  u8 *buff,u32 len );

void SPI1_Recive( u8 *buff, u32 len );
		   
#endif

