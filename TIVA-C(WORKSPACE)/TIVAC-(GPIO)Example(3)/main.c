#include "tm4c123gh6pm_registers.h"

//Example3 we will use 3 RGB leds at PF1,PF2, and PF3
//roll action performed using leds each led for 1 second, this operation is done continuously
//The delay which is used in this example is using a delay function

#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 364

void Delay_MS(unsigned long long n)
{
    volatile unsigned long long count = 0;
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND * n) );
}

void Leds_Init(void)
{
    //Disable Analog for the 3 leds
    GPIO_PORTF_AMSEL_REG &= 0xF1;
    //Choose GPIO mode
    GPIO_PORTF_PCTL_REG &= 0xFFFF000F;
    //Configure the direction for the three leds to be output
    GPIO_PORTF_DIR_REG |= 0x0E;
    //Disable Alternate function for the three PINS
    GPIO_PORTF_AFSEL_REG &= 0xF1;
    //Enable digital I/O for the three PINS
    GPIO_PORTF_DEN_REG |= 0x0E;
    //Disable All three Leds
    GPIO_PORTF_DATA_REG &= 0xF1;
}

int main(void)
{
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));
    Leds_Init();
    while(1)
    {
        GPIO_PORTF_DATA_REG = (GPIO_PORTF_DATA_REG & 0xF1) | 0x02; /* Turn on the Red and disable the others*/
        Delay_MS(1000);
        GPIO_PORTF_DATA_REG = (GPIO_PORTF_DATA_REG & 0xF1) | 0x04; /* Turn on the Blue LED and disable the others */
        Delay_MS(1000);
        GPIO_PORTF_DATA_REG = (GPIO_PORTF_DATA_REG & 0xF1) | 0x08; /* Turn on the Green LED and disable the others */
        Delay_MS(1000);
    }
	return 0;
}
