#include "device_info.h"  
#include "stm32f10x.h"	
#include "stdio.h"
#include "stmflash.h"	

#define PAGE61_ADDR  (0x08000000 + 200 * STM_SECTOR_SIZE) 	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
#define PAGE62_ADDR  (0x08000000 + 201* STM_SECTOR_SIZE) 	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
#define PAGE63_ADDR  (0x08000000 + 202* STM_SECTOR_SIZE) 	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)

uint8_t device_status=0;
uint8_t device_id=0;
uint8_t decice_grade=0;
uint8_t device_name[4]={0};


//====================================================================//
// 语法格式：void Flash_Configuration(void)
// 实现功能：Flash记忆检测
// 参    数：无
// 返 回 值：无
// 备    注：无
//====================================================================//

u16 FLAH_BUFF0[10]={0};
void Read_Flash_Config(void)
{
	STMFLASH_Read(PAGE61_ADDR, FLAH_BUFF0, 10);
}

void Flash_Configuration(void)
{
	int count_id=0;
	STMFLASH_Read(PAGE61_ADDR, FLAH_BUFF0, 10);
	if((FLAH_BUFF0[0]=='H')&&(FLAH_BUFF0[1]=='I')&&(FLAH_BUFF0[2]=='T'))
	{
		device_status=1;
		switch(FLAH_BUFF0[4])
		{
			case 'M' :
				device_id=MASTER;
			break;
			case 'S' :
				device_id=SLAVERY;
			break;
			default:
				device_id=MASTER;			
		}
		switch(FLAH_BUFF0[5])
		{
			case '0':
				decice_grade=ZERO;
			break;
			case '1':
				decice_grade=ONE;
			break;
			case '2':
				decice_grade=TWO;
			break;
			case '3':
				decice_grade=THREE;
			break;
			case '4':
				decice_grade=FOUR;
			break;
			default:
				decice_grade=ZERO;
		}
		for(count_id = 6;count_id<10;count_id++)
		{
			if((FLAH_BUFF0[count_id]<'0')||(FLAH_BUFF0[count_id]>'9'))
			{
				FLAH_BUFF0[count_id]='0';
			}
			device_name[count_id-6]=	FLAH_BUFF0[count_id];
		}
	}
		else
	{
		device_status=0;
	}
}

void My_STMFLASH_Write(void)
{
	STMFLASH_Write(PAGE61_ADDR, FLAH_BUFF0, 10);//用作平时存储记录
}


