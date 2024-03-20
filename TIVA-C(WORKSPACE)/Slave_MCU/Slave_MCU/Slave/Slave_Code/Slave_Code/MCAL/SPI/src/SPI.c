/*
* SPI.c
*
* Created: 4/29/2023 3:21:04 PM
*  Author: user
*/
#include "SPI.h"


void SPI_Init(SPI_ConfigTypes * options){
	switch(options->stateMode){
		case Master:
		DIO_ConfigChannel(SPI_SSPin,Output);
		DIO_ConfigChannel(SPI_MOSIPin,Output);
		DIO_ConfigChannel(SPI_MISOPin,Input);
		DIO_ConfigChannel(SPI_SCKPin,Output);
		SPI->SPCR_Reg.Bits.MSTR_Bit = 1;
		SPI->SPCR_Reg.Bits.SPE_Bit = 1;
		break;
		case Slave:
		DIO_ConfigChannel(SPI_SSPin,Input);
		DIO_ConfigChannel(SPI_MOSIPin,Input);
		DIO_ConfigChannel(SPI_MISOPin,Output);
		DIO_ConfigChannel(SPI_SCKPin,Input);
		SPI->SPCR_Reg.Bits.MSTR_Bit = 0;
		SPI->SPCR_Reg.Bits.SPE_Bit = 1;
		break;
	}
	SPI->SPCR_Reg.AllRegister |= (options->dataOrderMode<<5);
	if (options->doubleSpeedMode == DoupleSpeedEnable)
	{
		SPI->SPSR_Reg.Bits.SPI2X_Bit = 1;
		SPI->SPCR_Reg.AllRegister |= (options->clockMode<<0);
	}
	else
	{
		SPI->SPSR_Reg.Bits.SPI2X_Bit = 0;
		SPI->SPCR_Reg.AllRegister |= (options->clockMode<<0);
	}
}
u8 SPI_TxRx(u8 data){
	SPI->SPDR_Reg = data;
	while(SPI->SPSR_Reg.Bits.SPIF_Bit == 0);
	return SPI->SPDR_Reg;
}
void SPI_SlaveEnable(DIO_ChannelTypes channelId){
	DIO_ConfigChannel(channelId,Output);
	DIO_WriteChannel(channelId,STD_Low);
}
void SPI_SlaveDisable(DIO_ChannelTypes channelId){
	DIO_ConfigChannel(channelId,Output);
	DIO_WriteChannel(channelId,STD_High);
}