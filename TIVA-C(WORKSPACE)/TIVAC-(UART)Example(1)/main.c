#include "UART.h"

 /*Requirements:
         a) Use 16MHZ CPU frequency.

         b) Enable UART0.

         c)Receive data from PC and re send it back again.

           Receive string from PC till '#' and re send the string again.

           Receive byte from PC and re send it back again.
/*_______________________________________________________________________________________________________________*/

int main(void)
{
    uint8 data1[10];
    uint8 data2;
    uint8 str[20];
    UART0_Init();

    //Receive 5-bytes from PC terminal and re-send them again
    UART0_ReceiveData(data1, 10);
    UART0_SendData(data1, 10);

    //Receive String from PC terminal until the '#' character and re-send it again
    UART0_ReceiveString(str);
    UART0_SendString(str);

    while(1)
    {
        //Receive byte from PC and send it back again
        data2 = UART0_ReceiveByte();
        UART0_SendByte(data2);
    }
}
