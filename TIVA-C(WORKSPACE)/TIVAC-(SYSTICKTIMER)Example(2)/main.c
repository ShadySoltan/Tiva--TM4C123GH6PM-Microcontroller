#include "tm4c123gh6pm_registers.h"

//Init function for Systick Timer
void SysTick_Init(void)
{
    //Disable systick while initializing
    ClearBit(SYSTICK_CTRL_REG,0);
    //Set the reload value to count 1 second
    SYSTICK_RELOAD_REG = 15999999;
    //Choose the clock source to be system clock by writing 1
    //Enable Systick and disable the interrupt
    SYSTICK_CTRL_REG = 0x05;
}

void Leds_Init(void)
{
    /* Disable Analog on PF1, PF2 and PF3 */
    GPIO_PORTF_AMSEL_REG &= 0xF1;
    /* Clear PMCx bits for PF1, PF2 and PF3 to use it as GPIO pin */
    GPIO_PORTF_PCTL_REG  &= 0xFFFF000F;
    /* Configure PF1, PF2 and PF3 as output pin */
    GPIO_PORTF_DIR_REG   |= 0x0E;
    /* Disable alternative function on PF1, PF2 and PF3 */
    GPIO_PORTF_AFSEL_REG &= 0xF1;
    /* Enable Digital I/O on PF1, PF2 and PF3 */
    GPIO_PORTF_DEN_REG   |= 0x0E;
    /* Clear bit 0, 1 and 2 in Data register to turn off the leds */
    GPIO_PORTF_DATA_REG  &= 0xF1;
}

/*In example 2 we will use the same polling technique to wait for
 * the timer to finish counting and we will flash each led for 1 second*/

int main(void)
{
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    Leds_Init();
    SysTick_Init();

    while(1)
    {
        SetBit(GPIO_PORTF_DATA_REG,1);
        ClearBit(GPIO_PORTF_DATA_REG,2);
        ClearBit(GPIO_PORTF_DATA_REG,3);

        while(GetBit(SYSTICK_CTRL_REG,16) == 0);

        SetBit(GPIO_PORTF_DATA_REG,2);
        ClearBit(GPIO_PORTF_DATA_REG,1);
        ClearBit(GPIO_PORTF_DATA_REG,3);

        while(GetBit(SYSTICK_CTRL_REG,16) == 0);

        SetBit(GPIO_PORTF_DATA_REG,3);
        ClearBit(GPIO_PORTF_DATA_REG,1);
        ClearBit(GPIO_PORTF_DATA_REG,2);

        while(GetBit(SYSTICK_CTRL_REG,16) == 0);
    }
}