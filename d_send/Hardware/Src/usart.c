#include "stm32f10x.h"
#include "usart.h"	  

#include "device_info.h" 
#include "stmflash.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
int _sys_exit(int x) 
{ 
	x = x; 
	return 0;
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0)
		;//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART_TX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	
u8 USART_RX_BUFF[20]={0};
  
void uart_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
		USART_DeInit(USART1);  //��λ����1
	
		//USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

		//Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
		//USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure);     //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
//		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�������ڿ����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}

void USART1_Send_Hex(unsigned char Hex)
{
	USART_SendData(USART1, Hex);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void USART1_Send_PartialCommand(unsigned char *String)
{
	for(;*String!='\0';String++)
	{
		USART1_Send_Hex(*String);
	}
}

//int length;
int receive_data;
int tx_en_counter=0;

void USART1_IRQHandler(void)                	//����1�жϷ������
{
//	USART_RX_STA = 0;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//		{  //�ַ������ж�(���յ������ݱ�����0x0d 0x0a��β)
//		USART_RX_BUFF[length++] = USART_ReceiveData(USART1);
//		}
//		else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
//		{//����֡�ж�
//			USART_RX_BUFF[length++] = USART_ReceiveData(USART1);
//			length = 0;
//			USART_RX_STA = 1;
//		}
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
		{  //�ַ������ж�(���յ������ݱ�����0x0d 0x0a��β)
		receive_data= USART_ReceiveData(USART1);
		}
		if(receive_data=='R')
			tx_en_counter=1000;
			//tx_en_counter=0;
} 

#endif	

