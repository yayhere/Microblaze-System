#include "xparameters.h"
#include "xil_printf.h"
#include "xgpio.h"

int read_number(char terminator) {
    int num = 0;
    char c;

    while(1) {
        c = inbyte(); // wait for user to type a key

        // if user types a number, add it to our total
        if(c >= '0' && c <= '9') {
            outbyte(c); // echo the character back to the screen so you can see it
            num = num * 10 + (c - '0');
        }
        // if user types the separator/terminator, we are done
        else if (c == terminator || c == '\r' || c == '\n') {
            return num;
        }
    }
}

int main()
{
    XGpio gpio;
    XGpio_Initialize(&gpio, 0);
    XGpio_SetDataDirection(&gpio, 2, 0x00000000); // LEDs as Output

    int num1, num2;
    int product;

    xil_printf("\r\n--- Multiplication Calculator ---\r\n");

    while (1)
    {
        xil_printf("\r\nEnter input (e.g. 8,6): ");
        num1 = read_number(',');
        xil_printf(",");
        num2 = read_number('\r');
        product = num1 * num2;
        xil_printf("\r\nResult: %d * %d = %d \r\n", num1, num2, product);

        // LED logic
        if (product > 100) {
             xil_printf("Status: Limit Exceeded (LEDs ON)\r\n");
             XGpio_DiscreteWrite(&gpio, 2, 0xFFFFFFFF);
        }
        else {
             XGpio_DiscreteWrite(&gpio, 2, 0x00000000);
        }
    }
}