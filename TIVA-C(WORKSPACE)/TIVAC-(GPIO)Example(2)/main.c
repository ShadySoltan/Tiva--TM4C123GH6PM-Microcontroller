/*
 * GPIO_EXAMPLE2.c
 *
 *  Created on: Feb 10, 2024
 *      Author: shady
 */
#include "tm4c123gh6pm_registers.h"

//Example 2 using switch 1 PF4 toggle Blue led PF2 every new switch press

#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 364

void Delay_MS(unsigned long long n)
{
    volatile unsigned long long count = 0;
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND * n) );
}

/* Enable PF4 (SW1) */
void SW1_Init(void)
{
    GPIO_PORTF_AMSEL_REG &= ~(1<<4);      /* Disable Analog on PF4 */
    GPIO_PORTF_PCTL_REG  &= 0xFFF0FFFF;   /* Clear PMCx bits for PF4 to use it as GPIO pin */
    GPIO_PORTF_DIR_REG   &= ~(1<<4);      /* Configure PF4 as input pin */
    GPIO_PORTF_AFSEL_REG &= ~(1<<4);      /* Disable alternative function on PF4 */
    GPIO_PORTF_PUR_REG   |= (1<<4);       /* Enable pull-up on PF4 */
    GPIO_PORTF_DEN_REG   |= (1<<4);       /* Enable Digital I/O on PF4 */
}

/* Enable PF2 (Blue LED) */
void Led_Blue_Init(void)
{
    GPIO_PORTF_AMSEL_REG &= ~(1<<2);      /* Disable Analog on PF2 */
    GPIO_PORTF_PCTL_REG  &= 0xFFFFF0FF;   /* Clear PMCx bits for PF2 to use it as GPIO pin */
    GPIO_PORTF_DIR_REG   |= (1<<2);       /* Configure PF2 as output pin */
    GPIO_PORTF_AFSEL_REG &= ~(1<<2);      /* Disable alternative function on PF2 */
    GPIO_PORTF_DEN_REG   |= (1<<2);       /* Enable Digital I/O on PF2 */
    GPIO_PORTF_DATA_REG  &= ~(1<<2);      /* Clear bit 2 in Data register to turn off the led */
}

int main(void)
{
    uint8 sw_flag = 0;

    /* Enable clock for PORTF and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    /* Initialize the SW1 and Blue LED as GPIO Pins */
    SW1_Init();
    Led_Blue_Init();

    while(1)
    {
        /* Check the switch state */
        if(!(GPIO_PORTF_DATA_REG & (1<<4)))
        {
            Delay_MS(10); /* Delay for the debounce */
            if(!(GPIO_PORTF_DATA_REG & (1<<4)))
            {
                if(sw_flag == 0)
                {
                    GPIO_PORTF_DATA_REG ^= (1<<2);
                    sw_flag = 1;
                }
            }
        }
        else
        {
            sw_flag = 0;
        }
    }
}
