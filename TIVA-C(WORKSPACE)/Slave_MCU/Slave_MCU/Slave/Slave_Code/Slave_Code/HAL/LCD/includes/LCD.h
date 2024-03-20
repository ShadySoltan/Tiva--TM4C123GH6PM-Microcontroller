/*
 * LCD.h
 *
 * Created: 3/25/2023 2:50:03 PM
 *  Author: user
 */ 


#ifndef LCD_H_
#define LCD_H_

#include "DIO.h"
#include "Clock.h"

#define LCD_RS DIO_ChannelB1
#define LCD_RW DIO_ChannelB2
#define LCD_E DIO_ChannelB3

#define LCD_DataPort DIO_PortA

void LCD_Init(void);
void LCD_Cmd(u8 cmd);
void LCD_Char(s8 data);
void LCD_String(s8 * string);
void LCD_SetPos(u8 line,u8 pos);
void LCD_Custome_Char(u8 loc,s8 * msg);

#endif /* LCD_H_ */