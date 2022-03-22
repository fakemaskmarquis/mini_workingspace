#include "dma.h"


#define SPI1_DR_Addr ( (u32)0x4001300C )
 DMA_InitTypeDef DMA_InitStructure_T;
  DMA_InitTypeDef DMA_InitStructure_R;
	NVIC_InitTypeDef NVIC_InitStructure_T;
	NVIC_InitTypeDef NVIC_InitStructure_R;

/*******************************************************************************
* Function Name  : SPI1_DMA_Configuration
* Description    : 配置SPI1_RX的DMA通道2，SPI1_TX的DMA通道3
* Input          : None
* Output         : None
* Return         : None
* Attention             :
*******************************************************************************/
void SPI1_RX_DMA_Configuration( u32 cmar,u16 cndtr )
{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输时钟

    /* DMA1 Channel2 (triggered by SPI1 Rx event) Config */
  DMA_DeInit(DMA1_Channel2);
  DMA_InitStructure_R.DMA_PeripheralBaseAddr =SPI1_DR_Addr;                          //设置 SPI1 发送外设(0x4001300C) 地址(目的地址)
  DMA_InitStructure_R.DMA_MemoryBaseAddr = cmar;                    //设置 SRAM 存储地址(目的地址)
  DMA_InitStructure_R.DMA_DIR = DMA_DIR_PeripheralSRC;                                //传输方向 外设-内存
  DMA_InitStructure_R.DMA_BufferSize = cndtr;                         //设置 SPI1 发送长度
  DMA_InitStructure_R.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure_R.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure_R.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure_R.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure_R.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure_R.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure_R.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel2, &DMA_InitStructure_R);

  DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, DISABLE);
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TE, DISABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	NVIC_InitStructure_R.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_InitStructure_R.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure_R.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure_R.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_R);
  /* Enable SPI1 DMA RX request */
//  SPI1->CR2 |= 1<<0;                                                                 //接收缓冲区DMA使能
// DMA_Cmd(DMA1_Channel2, ENABLE);
}
void SPI1_TX_DMA_Configuration( u32 cmar,u16 cndtr )
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输时钟
    /* DMA1 Channel3 (triggered by SPI1 Tx event) Config */
  DMA_DeInit(DMA1_Channel3);
  DMA_InitStructure_T.DMA_PeripheralBaseAddr = SPI1_DR_Addr;                          //设置  接收外设(0x4001300C) 地址(源地址)
  DMA_InitStructure_T.DMA_MemoryBaseAddr = cmar;                    //设置 SRAM 存储地址(源地址)
  DMA_InitStructure_T.DMA_DIR = DMA_DIR_PeripheralDST;                                //传输方向 内存-外设
  DMA_InitStructure_T.DMA_BufferSize = cndtr;                           //设置 SPI1 接收长度
  DMA_InitStructure_T.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                  //外设地址增量(不变)
  DMA_InitStructure_T.DMA_MemoryInc = DMA_MemoryInc_Enable;                           //内存地址增量(变化)
  DMA_InitStructure_T.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;           //外设传输宽度(字节)
  DMA_InitStructure_T.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                   //内存传输宽度(字节)
  DMA_InitStructure_T.DMA_Mode = DMA_Mode_Normal;                                     //传输方式,一次传输完停止,不重新加载
  DMA_InitStructure_T.DMA_Priority = DMA_Priority_VeryHigh;                           //中断方式-高(三级)
  DMA_InitStructure_T.DMA_M2M = DMA_M2M_Disable;                                      //内存到内存方式禁止
  DMA_Init(DMA1_Channel3, &DMA_InitStructure_T);

  DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, DISABLE);                                   //开启 DMA1_Channel3 传输完成中断
  DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, DISABLE);                                   //开启 DMA1_Channel3 传输错误中断
  /* Enable SPI1 DMA TX request */
//  SPI1->CR2 |= 1<<1;                                                                //发送缓冲区DMA使能
//  DMA_Cmd(DMA1_Channel3, ENABLE);                                                  //开启 DMA 通道 DMA1_Channel3
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	NVIC_InitStructure_T.NVIC_IRQChannel = DMA1_Channel3_IRQn;
	NVIC_InitStructure_T.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure_T.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure_T.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_T);
}


/*******************************************************************************
* Function Name  : SPI1_Send
* Description    : SPI1的DMA方式发送
* Input          : SPI1_TX_Buff[SPI1_SendBufferSize]
* Output         : None
* Return         : None
* Attention             : 关闭DMA通道3之前必须等待TXE为1，等待忙标志为0
*******************************************************************************/
void SPI1_Send(const  u8 *buff,u32 len )
{
   DMA_Cmd(DMA1_Channel3, DISABLE );  //关闭USART1 TX DMA1 所指示的通道 
	 DMA1_Channel3->CMAR = (u32) buff; //mem地址
 	 DMA_SetCurrDataCounter(DMA1_Channel3,len);//DMA通道的DMA缓存的大小
 	 DMA_Cmd(DMA1_Channel3, ENABLE);  //使能USART1 TX DMA1 所指示的通道 
}

/*******************************************************************************
* Function Name  : SPI1_Recive
* Description    : SPI1的DMA方式接收
* Input          : None
* Output         : SPI1_RX_Buff[SPI1_ReciveBufferSize]
* Return         : None
* Attention             : 必须要先关闭通道2,然后再配置通道2的参数
*******************************************************************************/
void SPI1_Recive( u8 *buff, u32 len )
{
    DMA_Cmd(DMA1_Channel2, DISABLE );  //关闭USART1 TX DMA1 所指示的通道 
	DMA1_Channel2->CMAR = (uint32_t)buff; //mem地址
 	 DMA_SetCurrDataCounter(DMA1_Channel2,len);//DMA通道的DMA缓存的大小
 	 DMA_Cmd(DMA1_Channel2, ENABLE);  //使能USART1 TX DMA1 所指示的通道 
	
}
/*******************************************************************************
* Function Name  : SPI1_Recive
* Description    : SPI1的DMA方式接收
* Input          : None
* Output         : SPI1_RX_Buff[SPI1_ReciveBufferSize]
* Return         : None
* Attention             : 必须要先关闭通道2,然后再配置通道2的参数
*******************************************************************************/
//void SPI1_Recive( u8 *buff, u32 len )
//{
//    DMA1_Channel2->CCR &= ~( 1 << 0 );          //关闭DMA通道2

//    DMA1_Channel2->CPAR = SPI1_DR_Addr; //外设地址
//    DMA1_Channel2->CMAR = (uint32_t)buff; //mem地址
//    DMA1_Channel2->CNDTR = len ; //传输长度
//    DMA1_Channel2->CCR = (0 << 14) | // 非存储器到存储器模式
//            (2 << 12) | // 通道优先级高
//            (0 << 11) | // 存储器数据宽度8bit
//            (0 << 10) | // 存储器数据宽度8bit
//            (0 <<  9) | // 外设数据宽度8bit
//            (0 <<  8) | // 外设数据宽度8bit
//            (1 <<  7) | // 存储器地址增量模式
//            (0 <<  6) | // 外设地址增量模式(不增)
//            (0 <<  5) | // 非循环模式
//            (0 <<  4) | // 传输方向 外设-内存
//            (0 <<  3) | // 允许传输错误中断
//            (0 <<  2) | // 允许半传输中断
//            (1 <<  1) | // 允许传输完成中断
//            (1);        // 通道开启
//}
