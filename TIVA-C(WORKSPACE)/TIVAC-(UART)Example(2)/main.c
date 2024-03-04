#include "UART.h"
#include "tm4c123gh6pm_registers.h"
 /*Requirements:
         a) Use 16MHZ CPU frequency.

         b) Enable UART0.

         c)Receive a number from PC from 0-7 then turn on the corresponding RGB LED.*/
/*____________________________________________________________________________________________________________*/

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

int main(void)
{
	UART0_Init();
	Leds_Init();

	uint8 Number;

	while(1)
	{
	    Number = UART0_ReceiveByte();
	    UART0_SendByte(Number);
	    if(Number >= '0' && Number <= '7')
	    {
            GPIO_PORTF_DATA_REG = (GPIO_PORTF_DATA_REG & 0xF1) | ((Number - '0')<<1);
	    }
	    Number = 0;
	}

}
