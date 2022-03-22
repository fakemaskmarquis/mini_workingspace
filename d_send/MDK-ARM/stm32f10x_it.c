/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x.h"
#include "deca_spi.h"
#include "port.h"
#include "hw_config.h"

/* Tick timer count. */
__IO unsigned long time32_incr;

u32 Dw_conter_time=0;
void SysTick_Handler(void)
{
	time32_incr++;
}
void EXTI5_IRQHandler(void)
{
		
}

void DMA1_Channel2_IRQHandler(void)
{
			DMA_ITConfig(DMA1_Channel2, DMA_IT_TE, DISABLE);
			DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, DISABLE);  	//关闭 DMA1_Channel2 传输完成中断
			DMA_ClearFlag(DMA1_FLAG_TC3);
			DMA_ClearFlag(DMA1_FLAG_TE3);
    if(DMA_GetITStatus(DMA1_FLAG_TC2)==SET)
    {
				SPIx_CS_GPIO->BSRR = SPIx_CS;
        DMA_ClearITPendingBit(DMA1_IT_TC2);
				read_dma_flag=0;
    }
		else
		{
				SPIx_CS_GPIO->BSRR = SPIx_CS;
			  DMA_ClearITPendingBit(DMA1_IT_TE2);
				read_error_flag=1;
		}
}

void DMA1_Channel3_IRQHandler(void)
{
			DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, DISABLE);
			DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, DISABLE);
			DMA_ClearFlag(DMA1_FLAG_TC2);
			DMA_ClearFlag(DMA1_FLAG_TE2);//关闭 DMA1_Channel3 传输完成中断
    if(DMA_GetFlagStatus(DMA1_FLAG_TC3) == SET)
    {
					SPIx_CS_GPIO->BSRR = SPIx_CS;
					DMA_ClearITPendingBit(DMA1_IT_TC3);
					write_dma_flag=0; 					
   }
		else
		{
					SPIx_CS_GPIO->BSRR = SPIx_CS;
					DMA_ClearITPendingBit(DMA1_IT_TE3);
					write_error_flag=1;
		}
}
//定时器2中断服务程序
void TIM2_IRQHandler(void)   //TIM2中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );     //清除TIM2更新中断标志			
	}
	Dw_conter_time++;
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
