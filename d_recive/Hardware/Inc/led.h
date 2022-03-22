#ifndef __LED_H
#define __LED_H	 

#include "stm32f10x.h"

void GPIO_Configuration(void);
void led_off (void);
void led_on  (void);
void led_toggle(void);

		 				    
#endif
