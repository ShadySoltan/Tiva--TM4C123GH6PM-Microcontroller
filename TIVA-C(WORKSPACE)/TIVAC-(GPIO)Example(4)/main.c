#include "tm4c123gh6pm_registers.h"
//Example4 we will use two switches SW1(PF4) and SW2(PF0) with pull up configuration
//3 RGB leds att PF1,Pf2, and PF3
//Flash Red(PF1) and Blue(PF2) each for half a second in order in case SW1(PF4) is pressed
//Flash Blue(Pf2) and Green(PF3) each for half a second in order in case SW2(PF0) is pressed

//Init Function for SW2

#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 364

void Delay_MS(unsigned long long n)
{
    volatile unsigned long long count = 0;
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND * n) );
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
//Init Function for SW1
void SW1_Init(void)
{
    //Disable Analog on PF4
    ClearBit(GPIO_PORTF_AMSEL_REG,4);
    //Clear PMCx bits to choose GPIO
    GPIO_PORTF_PCTL_REG &= 0xFFF0FFFF;
    //Configure PF4 as INPUT
    ClearBit(GPIO_PORTF_DIR_REG,4);
    //Disable Alternate Function
    ClearBit(GPIO_PORTF_AFSEL_REG,4);
    //Enable Pull-Up resistor
    SetBit(GPIO_PORTF_PUR_REG,4);
    //Enable digital I/O on PF4
    SetBit(GPIO_PORTF_DEN_REG,4);
}

//Init Function for Leds
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
    GPIO_PORTF_DATA_REG  &= 0xFFFFFFF1;
}

int main(void)
{
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));
    SW2_Init();
    SW1_Init();
    Leds_Init();
    uint8 SW1_Flag = 0, SW2_Flag = 0;
    while(1)
    {
        if(GetBit(GPIO_PORTF_DATA_REG,4) == 0) //If switch 1 is pressed
        {
            if(SW1_Flag == 0)
            {
            SW1_Flag = 1;
            Delay_MS(10);
            //Flash Red led for half a second
            SetBit(GPIO_PORTF_DATA_REG,1);
            Delay_MS(500);
            ClearBit(GPIO_PORTF_DATA_REG,1);
            //Flash Blue led for half a second
            SetBit(GPIO_PORTF_DATA_REG,2);
            Delay_MS(500);
            ClearBit(GPIO_PORTF_DATA_REG,2);
            }
        }
        else
        {
            SW1_Flag = 0;
        }

        if(GetBit(GPIO_PORTF_DATA_REG,0) == 0) //SW2 is pressed
        {
            if(SW2_Flag == 0)
            {
            SW2_Flag = 1;
            Delay_MS(10);
            //Flash Red led for half a second
            SetBit(GPIO_PORTF_DATA_REG,2);
            Delay_MS(500);
            ClearBit(GPIO_PORTF_DATA_REG,2);
            //Flash Blue led for half a second
            SetBit(GPIO_PORTF_DATA_REG,3);
            Delay_MS(500);
            ClearBit(GPIO_PORTF_DATA_REG,3);
            }
        }
        else
        {
            SW2_Flag = 0;
        }
    }
}
