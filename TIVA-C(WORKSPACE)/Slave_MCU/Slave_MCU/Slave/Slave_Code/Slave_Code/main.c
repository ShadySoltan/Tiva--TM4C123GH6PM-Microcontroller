/*
* Slave_Code.c
*
* Created: 5/25/2023 5:35:52 AM
* Author : Administrator
*/

#include "Clock.h"
#include "STD_Types.h"
#include "BitMath.h"
#include "SPI.h"
#include "LCD.h"
#include "DIO.h"

int main(void)
{
	DIO_Init(); //Initialize DIO Pins of Slave MCU
	LCD_Init();
	SPI_ConfigTypes SPI_options = {Slave,MSB,SPIClock_64,DoupleSpeedDisable}; //Slave SPI Config Options
	SPI_Init(&SPI_options); //Initialize Slave SPI
	
	LCD_String("All LEDs are OFF");
	
	while (1)
	{	
		u8 Received_From_Master = STD_Low;
	
		Received_From_Master = SPI_TxRx('0');
		
		switch(Received_From_Master)
		{
			case '1':
			if(DIO_ReadChannel(DIO_ChannelC2) == STD_Low)
			{
				DIO_WriteChannel(DIO_ChannelC2,STD_High);
				LCD_Cmd(0x01);
				LCD_String("LED0 ON");				
			}
			else
			{
				DIO_WriteChannel(DIO_ChannelC2,STD_Low);
				LCD_Cmd(0x01);
				LCD_String("LED0 OFF");				
			}

			break;
			
			case '2':
			if(DIO_ReadChannel(DIO_ChannelC7) == STD_Low)
			{
				DIO_WriteChannel(DIO_ChannelC7,STD_High);
				LCD_Cmd(0x01);
				LCD_String("LED1 ON");
			}
			else
			{
				DIO_WriteChannel(DIO_ChannelC7,STD_Low);
				LCD_Cmd(0x01);
				LCD_String("LED1 OFF");
			}
						
			break;
			
			case '3':
			if(DIO_ReadChannel(DIO_ChannelD3) == STD_Low)
			{
				DIO_WriteChannel(DIO_ChannelD3,STD_High);
				LCD_Cmd(0x01);
				LCD_String("LED2 ON");
			}
			else
			{
				DIO_WriteChannel(DIO_ChannelD3,STD_Low);
				LCD_Cmd(0x01);
				LCD_String("LED2 OFF");
			}
			break;
			
			case '0':
			DIO_WriteChannel(DIO_ChannelC2,STD_Low);
			DIO_WriteChannel(DIO_ChannelC7,STD_Low);
			DIO_WriteChannel(DIO_ChannelD3,STD_Low);
			
			LCD_Cmd(0x01);
			LCD_String("All LEDs are OFF");
			break;
		}
	}
}

