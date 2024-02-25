#include "tm4c123gh6pm_registers.h"

/*Requirements:
         a) This is example4 in NVIC examples but with clock frequency 16MHZ.

         b) We will enable memory management fault exception.

         c) Enable MPU at the beginning of the program for only two regions:
             -Region 1: FLASH EEPROM memory to be able to access code area

             -Region 2: SRAM memory to be able to access code area

         d) In this example a memory management fault will be triggered as a result of accessing the register REGCGC2, since
             it is part from the system control space region.*/
/*_______________________________________________________________________________________________________________*/
#define SYSTICK_PRIORITY_MASK        0x1FFFFFFF
#define SYSTICK_INTERRUPT_PRIORITY       3
#define SYSTICK_PRIORITY_BITS_POS        29

#define NUMBER_TICKS_PER_ONE_SECOND      1
#define NUMBER_TICKS_PER_TWO_SECOND      2
#define NUMBER_TICKS_PER_THREE_SECOND    3

#define MEM_MANAGE_FAULT_PRIORITY_MASK        0xFFFFFF1F
#define MEM_MANAGE_FAULT_PRIORITY                 1
#define MEM_MANAGE_FAULT_PRIORITY_BITS_POS        5
#define MEM_MANAGE_FAULT_ENABLE_BIT_POS           16

#define MPU_ATTR_SIZE_BITS_POS                    1
#define MPU_ATTR_AP_BITS_POS                      24
#define MPU_ATTR_REGION_ENABLE_MASK           0x00000001
#define MPU_ATTR_B_MASK                       0x00010000
#define MPU_ATTR_C_MASK                       0x00020000
#define MPU_ATTR_S_MASK                       0x00040000
#define MPU_BASE_VALID_MASK                   0x00000010
#define MPU_CTRL_ENABLE_MASK                  0x00000001
#define MPU_CTRL_PRIVDEFEN_MASK               0x00000004

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

/* Go to low power mode while waiting for the next interrupt */
#define Wait_For_Interrupt()   __asm(" WFI ")

/* Data memory barrier instruction */
#define DMB()                  __asm(" DMB ")

/* Data synchronization barrier instruction */
#define DSB()                  __asm(" DSB ")

/* Instruction synchronization barrier instruction */
#define ISB()                  __asm(" ISB ")

volatile uint8 ticks_num = 0;

//Initialize Systick timer
void SysTick_Init(void)
{
    SYSTICK_CTRL_REG    = 0;               /* Disable the SysTick Timer by Clear the ENABLE Bit */
    SYSTICK_RELOAD_REG  = 15999999;        /* Set the Reload value with 15999999 to count 1 Second */
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

void MemoryManageFault_Handler(void)
{
    while(1);
}

void MemManage_Fault_Init(void)
{
    /* Assign priority level 1 to the MemManage Fault Exception */
    NVIC_SYSTEM_PRI1_REG = (NVIC_SYSTEM_PRI1_REG & MEM_MANAGE_FAULT_PRIORITY_MASK) | (MEM_MANAGE_FAULT_PRIORITY << MEM_MANAGE_FAULT_PRIORITY_BITS_POS);

    /* Enable the MemManage Fault Exception */
    NVIC_SYSTEM_SYSHNDCTRL |= (1<<MEM_MANAGE_FAULT_ENABLE_BIT_POS);
}

void MPU_Init(void)
{
    //Enable Memory fault exception
    MemManage_Fault_Init();

    //Check if we have MPU or not
    if(MPU_TYPE_REG == 0)
    {
        return;
    }

    DMB();

    //Disable MPU while initializing
    MPU_CTRL_REG = 0;

    /********************** Programming Region 0 - Flash memory (256KB) **********************
     * Normal Memory, Cacheable, Not Shareable and Write Through
     * Base Address 0x00000000
     * Region Size 256KB --> SIZE field in the attribute register should be 0x11
     * Privilege has FULL ACCESS (RW) --> AP field in the attribute register should be 0x03
     * Non-Privilege has FULL ACCESS (RW) --> AP field in the attribute register should be 0x03
     ******************************************************************************************/

    MPU_NUMBER_REG = 0; //Region number

    MPU_BASE_REG = 0x00000000; //Base address

    MPU_ATTR_REG = (0x11<<MPU_ATTR_SIZE_BITS_POS) | (MPU_ATTR_REGION_ENABLE_MASK) | (MPU_ATTR_C_MASK) | (0x03 << MPU_ATTR_AP_BITS_POS);

    /*************************** Programming Region 1 - SRAM (32KB) *************************
     * Normal Memory, Cacheable, Shareable and Write Through
     * Base Address 0x20000000
     * Region Size 32KB --> SIZE field in the attribute register should be 0x0E
     * Privilege has FULL ACCESS (RW) --> AP field in the attribute register should be 0x03
     * Non-Privilege has FULL ACCESS (RW) --> AP field in the attribute register should be 0x03
     *****************************************************************************************/

    MPU_BASE_REG = 0x20000000 | MPU_BASE_VALID_MASK | 1;

    MPU_ATTR_REG = (0x0E << MPU_ATTR_SIZE_BITS_POS) | (MPU_ATTR_C_MASK) | (MPU_ATTR_S_MASK) | (MPU_ATTR_REGION_ENABLE_MASK) | (0x03 << MPU_ATTR_AP_BITS_POS);

    /******************** Programming region 2 - PORTF Memory Map Area **********************
     * Device Memory Sharable and Bufferable
     * Base Address 0x40025000
     * region Size 4k(0x1000) --> SIZE field in the attribute register should be 0x0B
     * Privilege has FULL ACCESS (RW) --> AP field in the attribute register should be 0x03
     * Non-Privilege has FULL ACCESS (RW) --> AP field in the attribute register should be 0x03
     *****************************************************************************************/

    MPU_BASE_REG = 0x40025000 | MPU_BASE_VALID_MASK | 2;


    MPU_ATTR_REG = (0x0B << MPU_ATTR_SIZE_BITS_POS) | MPU_ATTR_REGION_ENABLE_MASK |
            MPU_ATTR_B_MASK | MPU_ATTR_S_MASK | (0x03 << MPU_ATTR_AP_BITS_POS);


    MPU_CTRL_REG |= MPU_CTRL_ENABLE_MASK;   //Enable the MPU


    DSB();
    ISB();

}

int main(void)
{
    /* Enable Interrupts, Exceptions and Faults */
    Enable_Exceptions();
    Enable_Faults();

    /*MPU UNIT*/
    MPU_Init();

    /* Enable clock for PORTF and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    /* Initialize the LEDs as GPIO Pins */
    Leds_Init();

    /* Initialize the SysTick Timer to generate an interrupt every 1 second */
    SysTick_Init();

    while(1)
    {

    }
}

