#include "tm4c123gh6pm_registers.h"

//Use Blue Led at PF2.
//Generate Edge triggered interrupt on PF4 pin using SW1. Interrupt should be configured with the falling edge.
//Turn on the LED with the falling edge.
/*_______________________________________________________________________________________________________________*/

#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 364

void Delay_MS(unsigned long long n)
{
    volatile unsigned long long count = 0;
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND * n) );
}


#define GPIO_PORTF_PRIORITY_MASK 0xFF1FFFFF
#define GPIO_PORTF_PRIORITY_BITS 21
#define GPIO_PORTF_INTERRUPT_PRIORITY 2

//Enable exceptions
#define Enable_Exceptions()     __asm(" CPSIE I ")
//Disable exceptions
#define Disable_Exceptions()    __asm(" CPSID I ")
//Enable faults
#define Enable_Faults()         __asm(" CPSIE F ")
//Disable faults
#define Disable_Faults()        __asm(" CPSID F ")
//Go to low power mode while waiting for the next interrupt
#define Wait_For_Interrupt()    __asm(" WFI ")

//GPIO_PORTF External interrupt - ISR Handler
void GPIOPortF_Handler(void) //This handler should be added to the vector table in startup code
{
    Delay_MS(10);
    //Enable_Blue led
    ToggleBit(GPIO_PORTF_DATA_REG,2);
    //Clear trigger flag for PF4 (Interrupt flag)
    SetBit(GPIO_PORTF_ICR_REG,4);
}

//Enable PF4 and activate external interrupt with falling edge
void SW1_Init(void)
{
    //Disable Analog on PF4
    ClearBit(GPIO_PORTF_AMSEL_REG,4);
    //Use GPIO mode on PF4
    GPIO_PORTF_PCTL_REG &= 0xFFF0FFFF;
    //Configure the direction to be input
    ClearBit(GPIO_PORTF_DIR_REG,4);
    //Disable Alternate function on PF4
    ClearBit(GPIO_PORTF_AFSEL_REG,4);
    //Use internal pull up resistor
    SetBit(GPIO_PORTF_PUR_REG,4);
    //Enable pin
    SetBit(GPIO_PORTF_DEN_REG,4);
    //configure the pin to detect edges
    ClearBit(GPIO_PORTF_IS_REG,4);
    //Disable BothEdges, because we will use falling edge only
    ClearBit(GPIO_PORTF_IBE_REG,4);
    //Configure the PIN to detect a falling edge by writing a zero on GPIOIEV
    ClearBit(GPIO_PORTF_IEV_REG,4);
    //Clear the flag
    SetBit(GPIO_PORTF_ICR_REG,4);
    //Enable Interrupt
    SetBit(GPIO_PORTF_IM_REG,4);
    //Set priority as 2
    NVIC_PRI7_REG = (NVIC_PRI7_REG & GPIO_PORTF_PRIORITY_MASK) | (GPIO_PORTF_INTERRUPT_PRIORITY << GPIO_PORTF_PRIORITY_BITS);
    //Enable NVIC for GPIO_PORTF by enable bit number 31 in Interrupt set enable
    SetBit(NVIC_EN0_REG,30);

}

//Enable PF2 blue led
void Led_Blue_Init(void)
{
    //Disable Analog on PF2
    ClearBit(GPIO_PORTF_AMSEL_REG,2);
    //Choose GPIO MODE on PF2
    GPIO_PORTF_PCTL_REG &= 0xFFFFF0FF;
    //Configure the direction to be output
    SetBit(GPIO_PORTF_DIR_REG,2);
    //Disable Alternate Function
    ClearBit(GPIO_PORTF_AFSEL_REG,2);
    //Enable Digital I/O
    SetBit(GPIO_PORTF_DEN_REG,2);
    //Disable the led
    ClearBit(GPIO_PORTF_DATA_REG,2);
}

int main(void)
{
    /* Enable clock for PORTF and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    SW1_Init();
    Led_Blue_Init();

    //Enable Interrupts
    Enable_Exceptions();

    //Enable Faults
    Enable_Faults();
    while(1)
    {
        //Go to low power mode and wait for interrupt
        Wait_For_Interrupt();
    }
}
