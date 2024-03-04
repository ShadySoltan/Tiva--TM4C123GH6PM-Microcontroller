/*
 * UART.c
 *
 *  Created on: Mar 4, 2024
 *      Author: shady
 */
#include "UART.h"
#include "tm4c123gh6pm_registers.h"

//Private Function Definitions
static void GPIO_SetupUART0Pins(void)
{
    //Enable Clock for PORTA
    SetBit(SYSCTL_RCGCGPIO_REG,0);
    while(!(SYSCTL_RCGCGPIO_REG&0x01));

    ClearBit(GPIO_PORTA_AMSEL_REG,0);
    ClearBit(GPIO_PORTA_AMSEL_REG,1);

    ClearBit(GPIO_PORTA_DIR_REG,0);
    SetBit(GPIO_PORTA_DIR_REG,1);

    SetBit(GPIO_PORTA_AFSEL_REG,0);
    SetBit(GPIO_PORTA_AFSEL_REG,1);

    GPIO_PORTA_PCTL_REG  = (GPIO_PORTA_PCTL_REG & 0xFFFFFF00) | 0x00000011;

    SetBit(GPIO_PORTA_DEN_REG,0);
    SetBit(GPIO_PORTA_DEN_REG,1);
}


//Public Functions Definitions
extern void UART0_Init(void)
{
    GPIO_SetupUART0Pins();

    // Enable clock for UART0
    SYSCTL_RCGCUART_REG |= 0x01;
    while(!(SYSCTL_PRUART_REG & 0x01));

    //Disable UART0 while initializing
    UART0_CTL_REG = 0;

    //Use system clock
    UART0_CC_REG = 0;

    //Configure the baud rate of UART0 to be 9600
    UART0_IBRD_REG = 104;
    UART0_FBRD_REG = 11;

    /* UART Line Control Register Settings
     * BRK = 0 Normal Use
     * PEN = 0 Disable Parity
     * EPS = 0 No affect as the parity is disabled
     * STP2 = 0 1-stop bit at end of the frame
     * FEN = 0 FIFOs are disabled
     * WLEN = 0x3 8-bits data frame
     * SPS = 0 no stick parity
     */
    UART0_LCRH_REG = (UART_DATA_8BITS << UART_LCRH_WLEN_BITS_POS);

    /* UART Control Register Settings
     * RXE = 1 Enable UART Receive
     * TXE = 1 Enable UART Transmit
     * HSE = 0 The UART is clocked using the system clock divided by 16
     * UARTEN = 1 Enable UART
     */
    UART0_CTL_REG = UART_CTL_UARTEN_MASK | UART_CTL_TXE_MASK | UART_CTL_RXE_MASK;
}

extern void UART0_SendByte(uint8 data)
{
    while(!(UART0_FR_REG & UART_FR_TXFE_MASK));
    UART0_DR_REG = data;
}

extern uint8 UART0_ReceiveByte(void)
{
    while(UART0_FR_REG & UART_FR_RXFE_MASK);
    return UART0_DR_REG;
}

extern void UART0_SendData(const uint8 *pData, uint32 uSize)
{
    while(uSize--)
    {
        UART0_SendByte(*pData);
        pData++;
    }
}

extern void UART0_ReceiveData(uint8 *pData, uint32 uSize)
{
    while(uSize--)
    {
        *pData = UART0_ReceiveByte();
        pData++;
    }
}

extern void UART0_SendString(const uint8 *pData)
{
    uint32 uCounter = 0;

    while(pData[uCounter] != '\0')
    {
        UART0_SendByte(pData[uCounter]);
        uCounter++;
    }
}

extern void UART0_ReceiveString(uint8 *pData)
{
    uint32 uCounter = 0;

    pData[uCounter] = UART0_ReceiveByte();

    while(pData[uCounter] != '#')
    {
        uCounter++;
        pData[uCounter] = UART0_ReceiveByte();
    }
    pData[uCounter] = '\0';
}
