/*
* DIO_CFG.h
*
* Created: 3/24/2023 4:10:08 PM
*  Author: user
*/


#ifndef DIO_CFG_H_
#define DIO_CFG_H_

#include "BitMath.h"
#include "STD_Types.h"
#include "DIO_Reg.h"
#include "DIO_Types.h"

typedef struct{
	DIO_DataDir direction;
	STD_LevelTypes level;
}DIO_PinCfg;

#define PINCOUNT 32

void DIO_Init(void);

#endif /* DIO_CFG_H_ */