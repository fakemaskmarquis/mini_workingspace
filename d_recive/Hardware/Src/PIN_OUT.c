#include "PIN_OUT.h"



void PIN_OUT_Configuration(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;					 //定义GPIO结构体
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能PC端口时钟
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9|GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
 
void pin1_off (void)
{
		GPIO_ResetBits(GPIOA, GPIO_Pin_9 );
		// do nothing for undefined led number
}
void pin2_off (void)
{
		GPIO_ResetBits(GPIOA, GPIO_Pin_10 );
		// do nothing for undefined led number
}


void pin1_on (void)
{
		GPIO_SetBits(GPIOA, GPIO_Pin_9);

}
void pin2_on (void)
{
		GPIO_SetBits(GPIOA, GPIO_Pin_10);

}


void pin1_toggle(void)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_9, (BitAction)!GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_9));
}

void pin2_toggle(void)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_10, (BitAction)!GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_10));
}

