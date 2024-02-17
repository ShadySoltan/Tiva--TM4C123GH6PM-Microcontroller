#include "tm4c123gh6pm_registers.h"

/*Example3 requirements:
 * 1)   MCU Frequency 16MHZ
 * 2)   Use red led PF1
 * 3)   Flash the led every half second
 * 4)   Use systick timer to count to half second using interrupt technique*/

/*______________________________________________________________________________________________*/

#define SYSTICK_PRIORITY_MASK  0x1FFFFFFF
#define SYSTICK_INTERRUPT_PRIORITY  3
#define SYSTICK_PRIORITY_BITS_POS   29

/* Enable Exceptions ... This Macro enable IRQ interrupts, Programmable Systems Exceptions and Faults by clearing the I-bit in the PRIMASK. */
#define Enable_Exceptions()    __asm(" CPSIE I ")

/* Disable Exceptions ... This Macro disable IRQ interrupts, Programmable Systems Exceptions and Faults by setting the I-bit in the PRIMASK. */
#define Disable_Exceptions()   __asm(" CPSID I ")

/* Enable Faults ... This Macro enable Faults by clearing the F-bit in the FAULTMASK */
#define Enable_Faults()        __asm(" CPSIE F ")

/* Disable Faults ... This Macro disable Faults by setting the F-bit in the FAULTMASK */
#define Disable_Faults()       __asm(" CPSID F ")

/* Go to low power mode while waiting for the next interrupt */
#define Wait_For_Interrupt()   __asm(" WFI ")

/* SysTick Timer ISR ... No need to clear the trigger flag (COUNT) bit ... it cleared automatically by the HW */
void SysTick_Handler(void)
{
    GPIO_PORTF_DATA_REG ^= (1<<1); /* Toggle the Red LED at PF1 */
}

//Init function for Systick Timer
void SysTick_Init(void)
{
    //Disable systick while initializing
    SYSTICK_CTRL_REG = 0;
    //Clear current register
    SYSTICK_CURRENT_REG = 0;
    //Set the reload value to count half second
    SYSTICK_RELOAD_REG = 7999999;
    //Choose the clock source to be system clock by writing 1
    //Enable Systick and enable the interrupt
    SYSTICK_CTRL_REG = 0x07;
    //Set priority for the systick timer
    NVIC_SYSTEM_PRI3_REG = (NVIC_SYSTEM_PRI3_REG & SYSTICK_PRIORITY_MASK)|(SYSTICK_INTERRUPT_PRIORITY << SYSTICK_PRIORITY_BITS_POS);
}

//Init Function for Leds
void Led_Red_Init(void)
{
    /* Disable Analog on PF1 */
    ClearBit(GPIO_PORTF_AMSEL_REG,1);
    /* Clear PMCx bits for PF1 to use it as GPIO pin */
    GPIO_PORTF_PCTL_REG &= 0xFFFFFF0F;
    /* Configure PF1 as output pin */
    SetBit(GPIO_PORTF_DIR_REG,1);
    /* Disable alternative function on PF1 */
    ClearBit(GPIO_PORTF_AFSEL_REG,1);
    /* Enable Digital I/O on PF1 */
    SetBit(GPIO_PORTF_DEN_REG,1);
    /* Clear bit 1 in Data register to turn off the led */
    ClearBit(GPIO_PORTF_DATA_REG,1);
}

int main(void)
{
    /* Enable clock for PORTF and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    /* Initialize the Red LED as GPIO Pin */
    Led_Red_Init();

    /* Initialize the SysTick Timer to count half second */
    SysTick_Init();

    /* Enable Interrupts, Exceptions and Faults */
    Enable_Exceptions();
    Enable_Faults();

	while(1)
	{
        /* Go to low power mode while waiting for the next interrupt */
        Wait_For_Interrupt();
	}
}
