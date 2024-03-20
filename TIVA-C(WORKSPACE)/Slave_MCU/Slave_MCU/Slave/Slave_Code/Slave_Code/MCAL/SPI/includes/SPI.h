/*
 * SPI.h
 *
 * Created: 4/29/2023 3:01:15 PM
 *  Author: user
 */ 


#ifndef SPI_H_
#define SPI_H_

#include "SPI_Reg.h"
#include "SPI_Types.h"

void SPI_Init(SPI_ConfigTypes * options);
u8 SPI_TxRx(u8 data);
void SPI_SlaveEnable(DIO_ChannelTypes channelId);
void SPI_SlaveDisable(DIO_ChannelTypes channelId);

#endif /* SPI_H_ */