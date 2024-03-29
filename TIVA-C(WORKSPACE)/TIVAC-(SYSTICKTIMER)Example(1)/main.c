#include "tm4c123gh6pm_registers.h"

//Init function for Systick Timer
void SysTick_Init(void)
{
    //Disable systick while initializing
    ClearBit(SYSTICK_CTRL_REG,0);
    //Set the reload value to count half second
    SYSTICK_RELOAD_REG = 7999999;
    //Choose the clock source to be system clock by writing 1
    //Enable Systick and disable the interrupt
    SYSTICK_CTRL_REG = 0x05;
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

void SW2_Init(void)
{
    //This PIN require unlocking
    GPIO_PORTF_LOCK_REG = 0x4C4F434B; //unlock commit register for portf
    //Allow configurations on PIN PF0
    SetBit(GPIO_PORTF_CR_REG,0);
    //Disable Analog on PF0
    ClearBit(GPIO_PORTF_AMSEL_REG,0);
    //Choose GPIO mode for the PF0 PIN
    GPIO_PORTF_PCTL_REG &= 0xFFFFFFF0;
    //Configure the direction of PF0 to be input
    ClearBit(GPIO_PORTF_DIR_REG,0);
    //Disable Alternate function
    ClearBit(GPIO_PORTF_AFSEL_REG,0);
    //Enable pull up resistor
    SetBit(GPIO_PORTF_PUR_REG,0);
    //Enable digital I/O
    SetBit(GPIO_PORTF_DEN_REG,0);
}

//For this simple example we need to flash the led every half a second
//while the switch is pressed and turn off if the switch is released
//we will wait for the timer to finish counting in this example since
//the interrupt is not used in this example
//we will use the interrupt in later examples (in NVIC examples)

#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 364

void Delay_MS(unsigned long long n)
{
    volatile unsigned long long count = 0;
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND * n) );
}

int main(void)
{
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    Led_Red_Init();
    SW2_Init();
    SysTick_Init();
    while(1)
    {
        if(GetBit(GPIO_PORTF_DATA_REG,0) == 0)
        {
            Delay_MS(10);
            SetBit(GPIO_PORTF_DATA_REG,1); //led on
            while(GetBit(SYSTICK_CTRL_REG,16) == 0);
            ClearBit(GPIO_PORTF_DATA_REG,1); //Led off
            while(GetBit(SYSTICK_CTRL_REG,16) == 0);
        }
        else
        {
            ClearBit(GPIO_PORTF_DATA_REG,1);
        }
    }
}
