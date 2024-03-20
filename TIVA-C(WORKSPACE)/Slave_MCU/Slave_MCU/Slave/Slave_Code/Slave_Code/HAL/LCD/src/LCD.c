/*
* LCD.c
*
* Created: 3/25/2023 2:50:13 PM
*  Author: user
*/
#include "LCD.h"

void LCD_Init(void){
	DIO_WriteChannel(LCD_RW,STD_Low);
	DIO_WriteChannel(LCD_E,STD_High);
	_delay_ms(20);
	LCD_Cmd(0x33);
	LCD_Cmd(0x32);
	LCD_Cmd(0x28);//Set Function 4Bit & 2Line & 5*8
	LCD_Cmd(0x06);//Entry Mode move to right
	LCD_Cmd(0x0C);//Display ON & Cursor ON & Blink
}
void LCD_Cmd(u8 cmd){
	u8 portValue = STD_Low;
	DIO_WriteChannel(LCD_RS,STD_Low);
	portValue = DIO_ReadPort(LCD_DataPort);
	portValue = (portValue & 0x0F) | (cmd & 0xF0);
	DIO_WritePort(LCD_DataPort,portValue);
	DIO_WriteChannel(LCD_E,STD_Low);
	_delay_us(100);
	DIO_WriteChannel(LCD_E,STD_High);
	
	_delay_ms(5);
	
	portValue = DIO_ReadPort(LCD_DataPort);
	portValue = (portValue & 0x0F) | (cmd << 4);
	DIO_WritePort(LCD_DataPort,portValue);
	DIO_WriteChannel(LCD_E,STD_Low);
	_delay_us(100);
	DIO_WriteChannel(LCD_E,STD_High);
	_delay_ms(2);
}
void LCD_Char(s8 data){
	u8 portValue = STD_Low;
	DIO_WriteChannel(LCD_RS,STD_High);
	portValue = DIO_ReadPort(LCD_DataPort);
	portValue = (portValue & 0x0F) | (data & 0xF0);
	DIO_WritePort(LCD_DataPort,portValue);
	DIO_WriteChannel(LCD_E,STD_Low);
	_delay_us(100);
	DIO_WriteChannel(LCD_E,STD_High);
	
	_delay_ms(5);
	
	portValue = DIO_ReadPort(LCD_DataPort);
	portValue = (portValue & 0x0F) | (data << 4);
	DIO_WritePort(LCD_DataPort,portValue);
	DIO_WriteChannel(LCD_E,STD_Low);
	_delay_us(100);
	DIO_WriteChannel(LCD_E,STD_High);
	_delay_ms(2);
}
void LCD_String(s8 * string){
	u8 i = 0;
	while (string[i] != '\0')
	{
		LCD_Char(string[i]);
		i++;
	}
}
void LCD_SetPos(u8 line,u8 pos){
	switch(line){
		case 1:
		LCD_Cmd((0x80 | (pos & 0x0F)));
		break;
		case 2:
		LCD_Cmd((0xC0 | (pos & 0x0F)));
		break;
	}
}
void LCD_Custome_Char(u8 loc,s8 * msg){
	if (loc<8)
	{
		LCD_Cmd((0x40+(loc*8)));
		for (u8 i=0;i<8;i++)
		{
			LCD_Char(msg[i]);
		}
	}
}