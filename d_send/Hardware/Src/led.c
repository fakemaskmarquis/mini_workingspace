#include "led.h"



void GPIO_Configuration(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;					 //定义GPIO结构体
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);	//使能PC端口时钟
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  
	
	// Enable GPIO used for LEDs
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//	// Power Pin
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOB, GPIO_Pin_4);
	
	//EXTON
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}
void led_off (void)
{
		GPIO_ResetBits(GPIOB, GPIO_Pin_6 );
		// do nothing for undefined led number
}

void led_on (void)
{
		GPIO_SetBits(GPIOB, GPIO_Pin_6);

}

void led_toggle(void)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)!GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_6));
}
 


