#include "tm4c123gh6pm_registers.h"

/*****************************************************************************
UART5 Registers
*****************************************************************************/
#define UART5_DR_REG              (*((volatile uint32 *)0x40011000))
#define UART5_RSR_REG             (*((volatile uint32 *)0x40011004))
#define UART5_ECR_REG             (*((volatile uint32 *)0x40011004))
#define UART5_FR_REG              (*((volatile uint32 *)0x40011018))
#define UART5_ILPR_REG            (*((volatile uint32 *)0x40011020))
#define UART5_IBRD_REG            (*((volatile uint32 *)0x40011024))
#define UART5_FBRD_REG            (*((volatile uint32 *)0x40011028))
#define UART5_LCRH_REG            (*((volatile uint32 *)0x4001102C))
#define UART5_CTL_REG             (*((volatile uint32 *)0x40011030))
#define UART5_IFLS_REG            (*((volatile uint32 *)0x40011034))
#define UART5_IM_REG              (*((volatile uint32 *)0x40011038))
#define UART5_RIS_REG             (*((volatile uint32 *)0x4001103C))
#define UART5_MIS_REG             (*((volatile uint32 *)0x40011040))
#define UART5_ICR_REG             (*((volatile uint32 *)0x40011044))
#define UART5_DMACTL_REG          (*((volatile uint32 *)0x40011048))
#define UART5_9BITADDR_REG        (*((volatile uint32 *)0x400110A4))
#define UART5_9BITAMASK_REG       (*((volatile uint32 *)0x400110A8))
#define UART5_PP_REG              (*((volatile uint32 *)0x40011FC0))
#define UART5_CC_REG              (*((volatile uint32 *)0x40011FC8))


#define UART5_DATA_5BITS          0x0
#define UART5_DATA_6BITS          0x1
#define UART5_DATA_7BITS          0x2
#define UART5_DATA_8BITS          0x3
#define UART5_LCRH_WLEN_BITS_POS  5
#define UART5_CTL_UARTEN_MASK     0x00000001
#define UART5_CTL_TXE_MASK        0x00000100
#define UART5_CTL_RXE_MASK        0x00000200
#define UART5_FR_TXFE_MASK        0x00000080
#define UART5_FR_RXFE_MASK        0x00000010


/*****************************************************************************
SPI1 Registers
*****************************************************************************/
#define SYSCTL_RCGCSSI_REG          (*((volatile uint32 *)0x400FE61C))
#define SPI1_SSICR0_REG             (*((volatile uint32 *)0x40009000))
#define SPI1_SSICR1_REG             (*((volatile uint32 *)0x40009004))
#define SPI1_SSIDR_REG              (*((volatile uint32 *)0x40009008))
#define SPI1_SSISR_REG              (*((volatile uint32 *)0x4000900C))
#define SPI1_SSICPSR_REG            (*((volatile uint32 *)0x40009010))
#define SPI1_SSIIM_REG              (*((volatile uint32 *)0x40009014))
#define SPI1_SSIRIS_REG             (*((volatile uint32 *)0x40009018))
#define SPI1_SSIMIS_REG             (*((volatile uint32 *)0x4000901C))
#define SPI1_SSIICR_REG             (*((volatile uint32 *)0x40009020))
#define SPI1_SSIDMACTL_REG          (*((volatile uint32 *)0x40009024))
#define SPI1_SSICC_REG              (*((volatile uint32 *)0x40009FC8))

#define SS_PIN                                      1
#define SCK_PIN                                     0
#define MOSI_PIN                                    3
#define MISO_PIN                                    2

void SPI1_Init(void)
{
    SYSCTL_RCGCSSI_REG |= 0x02;
    SYSCTL_RCGCGPIO_REG |= 0x08;

    GPIO_PORTD_AFSEL_REG |= (1 << SCK_PIN) | (1 << SS_PIN) | (1 << MISO_PIN) | (1 << MOSI_PIN);
    GPIO_PORTD_PCTL_REG  |= (2 << (SCK_PIN * 4)) | (2 << (SS_PIN * 4)) | (2 << (MISO_PIN * 4)) | (2 << (MOSI_PIN * 4));
    GPIO_PORTD_DIR_REG   |= (1 << SS_PIN);
    GPIO_PORTD_DEN_REG   |= (1 << SCK_PIN) | (1 << SS_PIN) | (1 << MISO_PIN) | (1 << MOSI_PIN);

    //Configure SPI1 module
    SPI1_SSICR1_REG  = 0;                                               //Disable SPI1 Module before configuration
    SPI1_SSICR1_REG |= 0x00000004;                                      //Set SPI module as master
    SPI1_SSICR0_REG  = 0x07;                                            //Set Data Size to be 8 bits
    SPI1_SSICPSR_REG = 64;                                              //Set Clock prescale div to 64
    SPI1_SSICR1_REG  = 0x02;                                            //Enable SPI1
}

void SPI1_Send(uint8 data)
{
    //wait until transmit FIFO is not full
    while((SPI1_SSISR_REG & 0x02) == 0);

    //Write data to data register
    SPI1_SSIDR_REG = data;

    //wait until transmit is complete
    while((SPI1_SSISR_REG & 0x10) == 0);
}

void HC05_Init(void)
{
    SYSCTL_RCGCUART_REG |= 0x20;
    while(!(SYSCTL_PRUART_REG & 0x20));

    SYSCTL_RCGCGPIO_REG |= 0x10;
    while(!(SYSCTL_RCGCGPIO_REG&0x10));

    UART5_CTL_REG = 0;

    UART5_IBRD_REG = 104;
    UART5_FBRD_REG = 11;
    UART5_CC_REG = 0;
    UART5_LCRH_REG = 0x60;

    UART5_CTL_REG = 0x301;

    GPIO_PORTE_AFSEL_REG = 0x30;
    GPIO_PORTE_AMSEL_REG = 0;
    GPIO_PORTE_PCTL_REG = 0x00110000;
    GPIO_PORTE_DEN_REG = 0x30;
}

void Leds_Init(void)
{
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));
    GPIO_PORTF_AMSEL_REG &= 0xF1;         /* Disable Analog on PF1, PF2 and PF3 */
    GPIO_PORTF_PCTL_REG  &= 0xFFFF000F;   /* Clear PMCx bits for PF1, PF2 and PF3 to use it as GPIO pin */
    GPIO_PORTF_DIR_REG   |= 0x0E;         /* Configure PF1, PF2 and PF3 as output pin */
    GPIO_PORTF_AFSEL_REG &= 0xF1;         /* Disable alternative function on PF1, PF2 and PF3 */
    GPIO_PORTF_DEN_REG   |= 0x0E;         /* Enable Digital I/O on PF1, PF2 and PF3 */
    GPIO_PORTF_DATA_REG  &= 0xF1;         /* Clear bit 0, 1 and 2 in Data register to turn off the leds */
}

void UART5_SendByte(uint8 data)
{
    while(!(UART5_FR_REG & UART5_FR_TXFE_MASK));
    UART5_DR_REG = data;
}

uint8 UART5_ReceiveByte(void)
{
    while(UART5_FR_REG & UART5_FR_RXFE_MASK);
    return UART5_DR_REG;
}

void UART5_ReceiveString(uint8 *pData)
{
    uint32 uCounter = 0;

    pData[uCounter] = UART5_ReceiveByte();

    while(pData[uCounter] != '#')
    {
        uCounter++;
        pData[uCounter] = UART5_ReceiveByte();
    }
    pData[uCounter] = '\0';
}

void UART5_SendString(const uint8 *pData)
{
    uint32 uCounter = 0;

    while(pData[uCounter] != '\0')
    {
        UART5_SendByte(pData[uCounter]);
        uCounter++;
    }
}



int main(void)
{
    HC05_Init();
    SPI1_Init();
    Leds_Init();

    uint8 Number;

	while(1)
	{
	    Number = UART5_ReceiveByte();

        switch(Number)
        {
        case '1':
            if(GetBit(GPIO_PORTF_DATA_REG,1) == 0)
            {
                UART5_SendString("LED0-SLAVE-ON");
                SetBit(GPIO_PORTF_DATA_REG,1);

                SPI1_Send('1');
            }
            else
            {
                UART5_SendString("LED0-SLAVE-OFF");
                ClearBit(GPIO_PORTF_DATA_REG,1);

                SPI1_Send('1');
            }

            break;

        case '2':
            if(GetBit(GPIO_PORTF_DATA_REG,2) == 0)
            {
                UART5_SendString("LED1-SLAVE-ON");
                SetBit(GPIO_PORTF_DATA_REG,2);

                SPI1_Send('2');
            }
            else
            {
                UART5_SendString("LED1-SLAVE-OFF");
                ClearBit(GPIO_PORTF_DATA_REG,2);

                SPI1_Send('2');
            }
            break;

        case '3':
            if(GetBit(GPIO_PORTF_DATA_REG,3) == 0)
            {
                UART5_SendString("LED2-SLAVE-ON");
                SetBit(GPIO_PORTF_DATA_REG,3);

                SPI1_Send('3');
            }
            else
            {
                UART5_SendString("LED2-SLAVE-OFF");
                ClearBit(GPIO_PORTF_DATA_REG,3);

                SPI1_Send('3');
            }
            break;

        case '0':
            UART5_SendString("All LEDs OFF!");
            GPIO_PORTF_DATA_REG = (GPIO_PORTF_DATA_REG & 0xF1) | ((Number - '0')<<1);

            SPI1_Send('0');
            break;
        }

	}
}
