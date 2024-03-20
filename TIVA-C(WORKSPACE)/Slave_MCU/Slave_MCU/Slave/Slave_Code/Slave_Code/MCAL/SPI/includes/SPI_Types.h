/*
* SPI_Types.h
*
* Created: 4/29/2023 3:01:55 PM
*  Author: user
*/


#ifndef SPI_TYPES_H_
#define SPI_TYPES_H_

#include "STD_Types.h"

typedef struct{
	u8 SPR0_Bit:1;
	u8 SPR1_Bit:1;
	u8 CPHA_Bit:1;
	u8 CPOL_Bit:1;
	u8 MSTR_Bit:1;
	u8 DORD_Bit:1;
	u8 SPE_Bit:1;
	u8 SPIE_Bit:1;
}SPI_SPCRBits;

typedef struct{
	u8 SPI2X_Bit:1;
	u8 RESERVED_Bit:5;
	u8 WCOL_Bit:1;
	u8 SPIF_Bit:1;
}SPI_SPSRBits;

typedef union{
	u8 AllRegister;
	SPI_SPCRBits Bits;
}SPI_SPCRTypes;

typedef union{
	u8 AllRegister;
	SPI_SPSRBits Bits;
}SPI_SPSRTypes;

typedef struct{
	SPI_SPCRTypes SPCR_Reg;
	SPI_SPSRTypes SPSR_Reg;
	u8 SPDR_Reg;
}SPI_Registers;


typedef enum{
	Slave=0,
	Master
}SPI_StateTypes;

typedef enum{
	MSB=0,
	LSB
}SPI_DataOrderTypes;

typedef enum{
	SPIClock_4=0,
	SPIClock_16,
	SPIClock_64,
	SPIClock_128,
}SPI_ClockTypes;

typedef enum{
	DoupleSpeedDisable=0,
	DoupleSpeedEnable
}SPI_DoupleSpeedTypes;

typedef struct{
	SPI_StateTypes stateMode;
	SPI_DataOrderTypes dataOrderMode;
	SPI_ClockTypes clockMode;
	SPI_DoupleSpeedTypes doubleSpeedMode;
}SPI_ConfigTypes;


#endif /* SPI_TYPES_H_ */