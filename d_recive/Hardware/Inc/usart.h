#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f10x.h"


#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART_TX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);

void USART1_Send_PartialCommand(unsigned char *String);
void USART1_Send_Hex(unsigned char Hex);
void USART1_Send_HexToChar2(unsigned int Data);
void USART1_Send_HexToChar(unsigned int Data);
void USART1_Send_HexToChar4(uint8_t data);
void USART1_Send_PartialCommand(unsigned char *String);
#endif


