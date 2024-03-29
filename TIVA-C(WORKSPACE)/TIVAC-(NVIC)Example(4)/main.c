#include "tm4c123gh6pm_registers.h"

/*Requirements:
     a) MCU Frequency is 80MHZ (Use PLL)
     b) Use 3 RGB LEDS at PF1, PF2, and PF3
     c) A roll action is performed using the 3 LEDs each LED for 1 Second. This operation is done continuously
     d) Use SYSTICK timer to count the one second using interrupt technique*/

/*_______________________________________________________________________________________________________________*/

//Masking Values for PLL Registers
#define SYSCTL_RCC2_USERCC2_MASK        0x80000000  /* USERCC2 Bit MASK */
#define SYSCTL_RCC2_BYPASS2_MASK        0x00000800  /* PLL BYPASS2 Bit MASK */
#define SYSCTL_RCC_XTAL_MASK            0x000007C0  /* XTAL Bits MASK */
#define SYSCTL_RCC_XTAL_8MHZ            0x0E        /* 8 MHz Crystal Value */
#define SYSCTL_RCC_XTAL_16MHZ           0x15        /* 16 MHz Crystal Value */
#define SYSCTL_RCC_XTAL_BIT_POS         6           /* XTAL Bits Position start from bit number 6 */
#define SYSCTL_RCC2_OSCSRC2_MASK        0x00000070  /* OSCSRC2 Bits MASK */
#define SYSCTL_RCC2_OSCSRC2_MOSC        0x0         /* MOSC(Main Oscillator) value */
#define SYSCTL_RCC2_OSCSRC2_BIT_POS     4           /* OSCSRC2 Bits Position start from bit number 4 */
#define SYSCTL_RCC2_PWRDN2_MASK         0x00002000  /* PWRDN2 Bit MASK */
#define SYSCTL_RCC2_DIV400_MASK         0x40000000  /* DIV400 Bit MASK to Divide PLL as 400 MHz vs. 200 */
#define SYSCTL_RCC2_SYSDIV2_MASK        0x1FC00000  /* SYSDIV2 Bits MASK */
#define SYSCTL_RIS_PLLLRIS_MASK         0x00000040  /* PLLLRIS Bit MASK */
#define SYSCTL_RCC2_SYSDIV2_BIT_POS     22       /* SYSDIV2 Bits Position start from bit number 22 */
#define SYSDIV2_VALUE                   4



#define SYSTICK_PRIORITY_MASK        0x1FFFFFFF
#define SYSTICK_INTERRUPT_PRIORITY       3
#define SYSTICK_PRIORITY_BITS_POS        29

#define NUMBER_TICKS_PER_ONE_SECOND      5
#define NUMBER_TICKS_PER_TWO_SECOND      10
#define NUMBER_TICKS_PER_THREE_SECOND    15

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

volatile uint8 ticks_num = 0;

void PLL_Init(void)
{
    //Configure the system to use RCC2
    SYSCTL_RCC2_REG |= SYSCTL_RCC2_USERCC2_MASK;

    //Bypass PLL while initializing, do not use PLL while Initialization
    SYSCTL_RCC2_REG |= SYSCTL_RCC2_PWRDN2_MASK;

    //Select the crystal value and source 16MHZ from main oscillator
    SYSCTL_RCC_REG &= ~SYSCTL_RCC_XTAL_MASK; //Clear all the XTAL bits first
    SYSCTL_RCC_REG |= (SYSCTL_RCC2_OSCSRC2_MOSC << SYSCTL_RCC_XTAL_BIT_POS); //Choose 16bit Oscillator
    SYSCTL_RCC2_REG &= ~SYSCTL_RCC2_OSCSRC2_MOSC;
    SYSCTL_RCC2_REG |= (SYSCTL_RCC2_OSCSRC2_MOSC << SYSCTL_RCC2_OSCSRC2_BIT_POS); //Source Main Oscillator

    //Activate PLL
    SYSCTL_RCC2_REG &= ~SYSCTL_RCC2_PWRDN2_MASK;

    //Set the desired system divider
    SYSCTL_RCC2_REG |= SYSCTL_RCC2_DIV400_MASK;
    SYSCTL_RCC2_REG = (SYSCTL_RCC2_REG & ~SYSCTL_RCC2_SYSDIV2_MASK) | (SYSDIV2_VALUE << SYSCTL_RCC2_SYSDIV2_BIT_POS);

    //Wait for PLL to LOCK and start working on 80MHZ
    while(!(SYSCTL_RIS_REG & SYSCTL_RIS_PLLLRIS_MASK));

    //Enable PLL and connect it using bybass
    SYSCTL_RCC2_REG &= ~SYSCTL_RCC2_BYPASS2_MASK;
}

//Initialize Systick timer
void SysTick_Init(void)
{
    SYSTICK_CTRL_REG    = 0;               /* Disable the SysTick Timer by Clear the ENABLE Bit */
    SYSTICK_RELOAD_REG  = 15999999;        /* Set the Reload value with 15999999 to count 0.2 Second */
    SYSTICK_CURRENT_REG = 0;               /* Clear the Current Register value */
    /* Configure the SysTick Control Register
     * Enable the SysTick Timer (ENABLE = 1)
     * Enable SysTick Interrupt (INTEN = 0)
     * Choose the clock source to be System Clock (CLK_SRC = 1) */
    SYSTICK_CTRL_REG   |= 0x07;
}

void SysTick_Handler(void)
{
    ticks_num++;

    if(ticks_num == NUMBER_TICKS_PER_ONE_SECOND)
    {
        GPIO_PORTF_DATA_REG = (GPIO_PORTF_DATA_REG & 0xF1) | 0x02; /* Turn on the Red LED and disable the others */
    }
    else if(ticks_num == NUMBER_TICKS_PER_TWO_SECOND)
    {
        GPIO_PORTF_DATA_REG = (GPIO_PORTF_DATA_REG & 0xF1) | 0x04; /* Turn on the Blue LED and disable the others */
    }
    else if(ticks_num == NUMBER_TICKS_PER_THREE_SECOND)
    {
        GPIO_PORTF_DATA_REG = (GPIO_PORTF_DATA_REG & 0xF1) | 0x08; /* Turn on the Green LED and disable the others */
        ticks_num = 0; /* reset the variable value */
    }
}

/* Enable PF1, PF2 and PF3 (RED, Blue and Green LEDs) */
void Leds_Init(void)
{
    GPIO_PORTF_AMSEL_REG &= 0xF1;         /* Disable Analog on PF1, PF2 and PF3 */
    GPIO_PORTF_PCTL_REG  &= 0xFFFF000F;   /* Clear PMCx bits for PF1, PF2 and PF3 to use it as GPIO pin */
    GPIO_PORTF_DIR_REG   |= 0x0E;         /* Configure PF1, PF2 and PF3 as output pin */
    GPIO_PORTF_AFSEL_REG &= 0xF1;         /* Disable alternative function on PF1, PF2 and PF3 */
    GPIO_PORTF_DEN_REG   |= 0x0E;         /* Enable Digital I/O on PF1, PF2 and PF3 */
    GPIO_PORTF_DATA_REG  &= 0xF1;         /* Clear bit 0, 1 and 2 in Data register to turn off the leds */
}

int main(void)
{
    /* Enable clock for PORTF and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    /* Initialize the LEDs as GPIO Pins */
    Leds_Init();

    /* Initialize the PLL to operate using 10Mhz frequency */
    PLL_Init();

    /* Initialize the SysTick Timer to generate an interrupt every 0.2 second */
    SysTick_Init();

    /* Enable Interrupts, Exceptions and Faults */
    Enable_Exceptions();
    Enable_Faults();

    while(1)
    {

    }
}
