#ifndef _DEVICE_INFO_H_
#define _DEVICE_INFO_H_
#include "stm32f10x.h"	

enum DW100_Device_ID
{ MASTER, 
SLAVERY};

enum DW100_Device_G
{ ZERO, 
ONE,
TWO,
THREE,
FOUR};

extern u16 FLAH_BUFF0[10];

extern uint8_t device_status;
extern uint8_t device_id;
extern uint8_t decice_grade;
extern uint8_t device_name[4];

void Flash_Configuration(void);
void Flash_Configuration(void);
void Read_Flash_Config(void);
void My_STMFLASH_Write(void);
#endif

