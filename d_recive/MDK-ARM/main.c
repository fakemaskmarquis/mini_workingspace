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
#include <string.h>
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
static uint8 zeros[FRAME_LEN_MAX]={0};
static uint32 status_reg = 0;
static uint16 frame_len = 0;
/* Index to access to sequence number of the blink frame in the tx_msg array. */



/* Inter-frame delay period, in milliseconds. */
#define TX_DELAY_MS 1000

 u32 Dw_conter=0;
 u32 Dw_conter0=0;
extern u8 Dw_conter_n[8];
extern u32 Dw_conter_time;

uint8 Message_flag1[17]="请配置设备参数\r\n";
uint8 Message_newline[3]="\r\n";
static u8 USB_TxBuff[400]={0};
int USB_RxBuff_len=0;
int USB_TxBuff_len=0;
//int error_counter=0;
//int bit_error_counter=0;
//int msg_id =0;
uint8 hsrb = 0x01 ;

int main(void)
{
    /* Start with board specific hardware init. */
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
		//USB_SendData((u8 *)("START\r\n"),8);
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
	//USB_SendData((u8 *)("START\r\n"),8);
	
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
		}
		else
		{
			dwt_configure(&S_config);
		}
		
  dwt_setleds(1);
	dwt_setdblrxbuffmode(ENABLE);
	dwt_setautorxreenable(ENABLE);
	dwt_setrxtimeout(0);
	dwt_rxenable(0);
	TIM_Cmd(TIM2, ENABLE);  //使能TIM2
	//dwt_writetodevice(SYS_CTRL_ID, SYS_CTRL_HRBT_OFFSET, 1, &hsrb);
     while (1)
    {
        while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR | SYS_STATUS_RXOVRR)))
        { };
        if (status_reg & SYS_STATUS_RXFCG)
        {
            /* A frame has been received, copy it to our local buffer. */
					if ((status_reg & SYS_STATUS_RXOVRR)==0)
					{
											
											memcpy(rx_buffer,zeros,FRAME_LEN_MAX);
									    frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
											//dwt_syncrxbufptrs();
											if (frame_len <= FRAME_LEN_MAX)
											{
											dwt_readrxdata(rx_buffer, frame_len, 0);
												while(read_dma_flag==1)					
												{
												}
												if(rx_buffer[2]==0x02)
												{
													Dw_conter++;
													if(Dw_conter_time>=1)
												{
													USB_TxWrite((u8 *)(&Dw_conter),4);
													Dw_conter=0;
													Dw_conter_time=0;
												}
												}
											}
											pin1_toggle();
										if(dwt_checkoverrun() == 0)
                    {
                        dwt_writetodevice(SYS_CTRL_ID, SYS_CTRL_HRBT_OFFSET, 1, &hsrb); // We need to swap RX buffer status reg (write one to toggle internally)
                    }
                    else
                    {
                        // The call-back has completed, but the overrun has been set, before we toggled, this means two new frames have arrived (one in the other buffer) and the 2nd's PHR good set the overrun flag
                        // Due to a receiver bug, which cannot guarantee the last frame's data was not corrupted need to reset receiver and discard any new data
                        dwt_forcetrxoff();
                        dwt_rxreset();
												//dwt_rxenable(0);
												dwt_write16bitoffsetreg(SYS_CTRL_ID,0,(uint16)SYS_CTRL_RXENAB) ;
                    }
					}
            /* Clear good RX frame event in the DW1000 status register. */
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);						
        }
        else if (status_reg & SYS_STATUS_RXOVRR) // NOTE when overrun both HS and RS pointers point to the same buffer
				{
					dwt_forcetrxoff();
					dwt_rxreset();
					dwt_writetodevice(SYS_CTRL_ID, SYS_CTRL_HRBT_OFFSET, 1, &hsrb);					
					dwt_write16bitoffsetreg(SYS_CTRL_ID,0,(uint16)SYS_CTRL_RXENAB) ;
				}
				else
        {
            /* Clear RX error events in the DW1000 status register. */
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
						dwt_forcetrxoff(); // This will clear all events
						dwt_rxreset();	// Reset the RX
						dwt_writetodevice(SYS_CTRL_ID, SYS_CTRL_HRBT_OFFSET, 1, &hsrb);
						dwt_write16bitoffsetreg(SYS_CTRL_ID,0,(uint16)SYS_CTRL_RXENAB) ;
        }
    }

}
