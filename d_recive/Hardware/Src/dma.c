#include "dma.h"


#define SPI1_DR_Addr ( (u32)0x4001300C )
 DMA_InitTypeDef DMA_InitStructure_T;
  DMA_InitTypeDef DMA_InitStructure_R;
	NVIC_InitTypeDef NVIC_InitStructure_T;
	NVIC_InitTypeDef NVIC_InitStructure_R;

/*******************************************************************************
* Function Name  : SPI1_DMA_Configuration
* Description    : ����SPI1_RX��DMAͨ��2��SPI1_TX��DMAͨ��3
* Input          : None
* Output         : None
* Return         : None
* Attention             :
*******************************************************************************/
void SPI1_RX_DMA_Configuration( u32 cmar,u16 cndtr )
{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����ʱ��

    /* DMA1 Channel2 (triggered by SPI1 Rx event) Config */
  DMA_DeInit(DMA1_Channel2);
  DMA_InitStructure_R.DMA_PeripheralBaseAddr =SPI1_DR_Addr;                          //���� SPI1 ��������(0x4001300C) ��ַ(Ŀ�ĵ�ַ)
  DMA_InitStructure_R.DMA_MemoryBaseAddr = cmar;                    //���� SRAM �洢��ַ(Ŀ�ĵ�ַ)
  DMA_InitStructure_R.DMA_DIR = DMA_DIR_PeripheralSRC;                                //���䷽�� ����-�ڴ�
  DMA_InitStructure_R.DMA_BufferSize = cndtr;                         //���� SPI1 ���ͳ���
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
//  SPI1->CR2 |= 1<<0;                                                                 //���ջ�����DMAʹ��
// DMA_Cmd(DMA1_Channel2, ENABLE);
}
void SPI1_TX_DMA_Configuration( u32 cmar,u16 cndtr )
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����ʱ��
    /* DMA1 Channel3 (triggered by SPI1 Tx event) Config */
  DMA_DeInit(DMA1_Channel3);
  DMA_InitStructure_T.DMA_PeripheralBaseAddr = SPI1_DR_Addr;                          //����  ��������(0x4001300C) ��ַ(Դ��ַ)
  DMA_InitStructure_T.DMA_MemoryBaseAddr = cmar;                    //���� SRAM �洢��ַ(Դ��ַ)
  DMA_InitStructure_T.DMA_DIR = DMA_DIR_PeripheralDST;                                //���䷽�� �ڴ�-����
  DMA_InitStructure_T.DMA_BufferSize = cndtr;                           //���� SPI1 ���ճ���
  DMA_InitStructure_T.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                  //�����ַ����(����)
  DMA_InitStructure_T.DMA_MemoryInc = DMA_MemoryInc_Enable;                           //�ڴ��ַ����(�仯)
  DMA_InitStructure_T.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;           //���贫����(�ֽ�)
  DMA_InitStructure_T.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                   //�ڴ洫����(�ֽ�)
  DMA_InitStructure_T.DMA_Mode = DMA_Mode_Normal;                                     //���䷽ʽ,һ�δ�����ֹͣ,�����¼���
  DMA_InitStructure_T.DMA_Priority = DMA_Priority_VeryHigh;                           //�жϷ�ʽ-��(����)
  DMA_InitStructure_T.DMA_M2M = DMA_M2M_Disable;                                      //�ڴ浽�ڴ淽ʽ��ֹ
  DMA_Init(DMA1_Channel3, &DMA_InitStructure_T);

  DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, DISABLE);                                   //���� DMA1_Channel3 ��������ж�
  DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, DISABLE);                                   //���� DMA1_Channel3 ��������ж�
  /* Enable SPI1 DMA TX request */
//  SPI1->CR2 |= 1<<1;                                                                //���ͻ�����DMAʹ��
//  DMA_Cmd(DMA1_Channel3, ENABLE);                                                  //���� DMA ͨ�� DMA1_Channel3
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	NVIC_InitStructure_T.NVIC_IRQChannel = DMA1_Channel3_IRQn;
	NVIC_InitStructure_T.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure_T.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure_T.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_T);
}


/*******************************************************************************
* Function Name  : SPI1_Send
* Description    : SPI1��DMA��ʽ����
* Input          : SPI1_TX_Buff[SPI1_SendBufferSize]
* Output         : None
* Return         : None
* Attention             : �ر�DMAͨ��3֮ǰ����ȴ�TXEΪ1���ȴ�æ��־Ϊ0
*******************************************************************************/
void SPI1_Send(const  u8 *buff,u32 len )
{
   DMA_Cmd(DMA1_Channel3, DISABLE );  //�ر�USART1 TX DMA1 ��ָʾ��ͨ�� 
	 DMA1_Channel3->CMAR = (u32) buff; //mem��ַ
 	 DMA_SetCurrDataCounter(DMA1_Channel3,len);//DMAͨ����DMA����Ĵ�С
 	 DMA_Cmd(DMA1_Channel3, ENABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}

/*******************************************************************************
* Function Name  : SPI1_Recive
* Description    : SPI1��DMA��ʽ����
* Input          : None
* Output         : SPI1_RX_Buff[SPI1_ReciveBufferSize]
* Return         : None
* Attention             : ����Ҫ�ȹر�ͨ��2,Ȼ��������ͨ��2�Ĳ���
*******************************************************************************/
void SPI1_Recive( u8 *buff, u32 len )
{
    DMA_Cmd(DMA1_Channel2, DISABLE );  //�ر�USART1 TX DMA1 ��ָʾ��ͨ�� 
	DMA1_Channel2->CMAR = (uint32_t)buff; //mem��ַ
 	 DMA_SetCurrDataCounter(DMA1_Channel2,len);//DMAͨ����DMA����Ĵ�С
 	 DMA_Cmd(DMA1_Channel2, ENABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
	
}
/*******************************************************************************
* Function Name  : SPI1_Recive
* Description    : SPI1��DMA��ʽ����
* Input          : None
* Output         : SPI1_RX_Buff[SPI1_ReciveBufferSize]
* Return         : None
* Attention             : ����Ҫ�ȹر�ͨ��2,Ȼ��������ͨ��2�Ĳ���
*******************************************************************************/
//void SPI1_Recive( u8 *buff, u32 len )
//{
//    DMA1_Channel2->CCR &= ~( 1 << 0 );          //�ر�DMAͨ��2

//    DMA1_Channel2->CPAR = SPI1_DR_Addr; //�����ַ
//    DMA1_Channel2->CMAR = (uint32_t)buff; //mem��ַ
//    DMA1_Channel2->CNDTR = len ; //���䳤��
//    DMA1_Channel2->CCR = (0 << 14) | // �Ǵ洢�����洢��ģʽ
//            (2 << 12) | // ͨ�����ȼ���
//            (0 << 11) | // �洢�����ݿ��8bit
//            (0 << 10) | // �洢�����ݿ��8bit
//            (0 <<  9) | // �������ݿ��8bit
//            (0 <<  8) | // �������ݿ��8bit
//            (1 <<  7) | // �洢����ַ����ģʽ
//            (0 <<  6) | // �����ַ����ģʽ(����)
//            (0 <<  5) | // ��ѭ��ģʽ
//            (0 <<  4) | // ���䷽�� ����-�ڴ�
//            (0 <<  3) | // ����������ж�
//            (0 <<  2) | // ����봫���ж�
//            (1 <<  1) | // ����������ж�
//            (1);        // ͨ������
//}
