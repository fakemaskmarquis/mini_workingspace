/*! ----------------------------------------------------------------------------
 *  @file    main.c
 *  @brief   Simple TX example code
 *
 * @attention
 *
 * Copyright 2015 (c) Decawave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author Decawave
 */
#include "deca_device_api.h"
#include "deca_regs.h"
#include "deca_sleep.h"
#include "device_info.h"  
#include "deca_spi.h"
#include "lcd.h"
#include "port.h"
#include "led.h"
#include "usart.h"
#include "hw_config.h"
#include "dma.h"
#include "timer.h"
#include "stmflash.h"
#include "PIN_OUT.h"

/* Example application name and version to display on LCD screen. */
#define APP_NAME "SIMPLE TX v1.2"
//#define BLINK_FRAME_SN_IDX 1
#define FRAME_LEN_MAX 1023

/* Default communication configuration. We use here EVK1000's default mode (mode 3). */
static dwt_config_t M_config = {
    2,               /* Channel number. */
    DWT_PRF_16M,     /* Pulse repetition frequency. */
    DWT_PLEN_64,   /* Preamble length. Used in TX only. */
    DWT_PAC8,       /* Preamble acquisition chunk size. Used in RX only. */
    3,               /* TX preamble code. Used in TX only. */
    3,               /* RX preamble code. Used in RX only. */
    1,               /* 0 to use standard SFD, 1 to use non-standard SFD. */
    DWT_BR_6M8,     /* Data rate. */
    DWT_PHRMODE_EXT, /* PHY header mode. */
    (64 + 1 + 8 - 8) /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};

static dwt_config_t S_config = {
    5,               /* Channel number. */
    DWT_PRF_16M,     /* Pulse repetition frequency. */
    DWT_PLEN_64,   /* Preamble length. Used in TX only. */
    DWT_PAC8,       /* Preamble acquisition chunk size. Used in RX only. */
    4,               /* TX preamble code. Used in TX only. */
    4,               /* RX preamble code. Used in RX only. */
    1,               /* 0 to use standard SFD, 1 to use non-standard SFD. */
    DWT_BR_6M8,     /* Data rate. */
    DWT_PHRMODE_EXT, /* PHY header mode. */
    (64 + 1 + 8 - 8) /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};

/* The frame sent in this example is an 802.15.4e standard blink. It is a 12-byte frame composed of the following fields:
 *     - byte 0: frame type (0xC5 for a blink).
 *     - byte 1: sequence number, incremented for each new frame.
 *     - byte 2 -> 9: device ID, see NOTE 1 below.
 *     - byte 10/11: frame check-sum, automatically set by DW1000.  */
static uint8 tx_msg[FRAME_LEN_MAX] ={0} ;
static uint8 rx_buffer[FRAME_LEN_MAX]={0};

/* Inter-frame delay period, in milliseconds. */
#define TX_DELAY_MS 1000

extern u32 Dw_conter_time;
u32 Dw_conter;

uint8 Message_flag1[17]="请配置设备参数\r\n";
uint8 Message_newline[3]="\r\n";
u8 USB_RxBuff[400]={0};
u8 USB_TxBuff[400]={0};
int USB_RxBuff_len=0;
int USB_TxBuff_len=0;
int frame_count=0;
int tx_en_flag;
extern int tx_en_counter;
	
int main(void)
{
    /* Start with board specific hardware init. */
    peripherals_init();
		TIM2_Int_Init(9999,7199);//1s
		uart_init(115200);	 //串口初始化为115200 	
		GPIO_Configuration();//初始化与LED连接的硬件接口
		SPI_Configuration();
		deca_sleep(100);
		Flash_Configuration();
		deca_sleep(100);
		USB_Config();
		deca_sleep(100);
		SPI1_TX_DMA_Configuration( (u32)tx_msg,FRAME_LEN_MAX);
		SPI1_RX_DMA_Configuration( (u32)rx_buffer,FRAME_LEN_MAX );
		SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx,ENABLE );
		SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx,ENABLE );
    /* Display application name on LCD. */
	  USB_TxBuff_len = sprintf((char*)&USB_TxBuff[0], 						"|*******************哈尔滨工业大学导航仪器研究所******************\r\n");
		USB_TxBuff_len+=	sprintf((char*)&USB_TxBuff[USB_TxBuff_len], "|                                                                 \r\n");
		USB_TxBuff_len+=	sprintf((char*)&USB_TxBuff[USB_TxBuff_len], "|                    UWB设备     -   舱内舱间通信                 \r\n");
		USB_TxBuff_len+=	sprintf((char*)&USB_TxBuff[USB_TxBuff_len], "|                                                                 \r\n");
		USB_TxBuff_len+=	sprintf((char*)&USB_TxBuff[USB_TxBuff_len], "|*****************************UWB模块测试*************************\r\n");	
		USB_TxWrite(USB_TxBuff, USB_TxBuff_len);
	while( device_status == 0 )
	{				
		USB_TxWrite(Message_flag1, 17);
		led_on(); 
		deca_sleep(100);		
		led_off();
		deca_sleep(100);				
	}
	USB_TxWrite((u8 *)FLAH_BUFF0, 20);
	USB_TxWrite(Message_newline, 3);
	USB_TxWrite((u8 *)("START\r\n"),8);
	
	//开始初始化dw1000
    lcd_display_str(APP_NAME);
    reset_DW1000(); /* Target specific drive of RSTn line into DW1000 low for a period. */
    spi_set_rate_low();
    if (dwt_initialise(DWT_LOADNONE) == DWT_ERROR)
    {
        lcd_display_str("INIT FAILED");
        while (1)
        { };
    }
    spi_set_rate_high();
		if(device_id==MASTER)
		{
			dwt_configure(&M_config);
			tx_en_flag=ENABLE;
		}
		else
		{
			dwt_configure(&S_config);
			tx_en_flag=DISABLE;
		}
		
    dwt_setleds(1);

	for(frame_count=0;frame_count<1022;frame_count++)
	{
		tx_msg[frame_count]=(uint8)frame_count;
		//tx_msg[frame_count]=0xff;
	}

		TIM_Cmd(TIM2, ENABLE);  //使能TIM2
		Dw_conter_time=0;
	while (1)
    {
				
				while((tx_en_flag!=ENABLE)&&(tx_en_counter==0))
				{
//					printf("ERROR\n");
//					deca_sleep(1000);
				}
				dwt_write16bitoffsetreg(SYS_CTRL_ID,0,1);//SFCST
        dwt_writetxfctrl(sizeof(tx_msg), 0);
				dwt_starttx(DWT_START_TX_IMMEDIATE);
				dwt_writetxdata(sizeof(tx_msg), tx_msg, 0); /* Zero offset in TX buffer. */
				while(write_dma_flag==1)					
				{
				}
				dwt_write16bitoffsetreg(SYS_CTRL_ID,0,8);//CANSFCS
        while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
        { };
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
        tx_msg[1000]++;
				tx_msg[1022]--;
				Dw_conter++;
				tx_en_counter--;
				if(Dw_conter_time>=1)
				{
					USB_TxWrite((u8 *)(&Dw_conter),4);
					Dw_conter=0;
					Dw_conter_time=0;
					if(tx_en_flag==ENABLE)
					printf("R");
				}
		}
		
}

