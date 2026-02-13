#include "xgpio.h"
#include "xil_printf.h"
#include "xparameters.h"

char key_map[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'0', 'F', 'E', 'D'} 
};

char keypad_scan(XGpio *gpio_instance) {
    u32 row_val;
    u32 col_mask;
    int col, row;

    for (col = 0; col < 4; col++) {
        col_mask = ~(1 << col) & 0xF; 
        XGpio_DiscreteWrite(gpio_instance, 2, col_mask);
        row_val = XGpio_DiscreteRead(gpio_instance, 1);

        for (row = 0; row < 4; row++) {
            if ( (row_val & (1 << row)) == 0 ) {
                while((XGpio_DiscreteRead(gpio_instance, 1) & (1 << row)) == 0);
                char key = key_map[row][col];
                if (key == '1' || key == '2' || key == '3' ) {
                    return key;
                }
            }
        }
    }
    return 0; // if no valid key pressed
}

int read_number() {
    int num = 0;
    char c;

    while(1) {
        c = inbyte(); 
        // validate input
        if(c >= '1' && c <= '3') {
            num = c - '0';
            return num;
        }
    }
}

int main()
{
    XGpio keypad_gpio;

    XGpio_Initialize(&keypad_gpio, XPAR_AXI_GPIO_0_DEVICE_ID);
    XGpio_SetDataDirection(&keypad_gpio, 1, 0xF); // inputs (rows)
    XGpio_SetDataDirection(&keypad_gpio, 2, 0x0); // outputs (cols)

    int serial;
    int keypad_int;
    char keypad_char;

    xil_printf("\r\n--- Rock-Paper-Scissors Serial-Keypad Game ---\r\n");
    xil_printf("Rules: 1 = Rock, 2 = Paper, 3 = Scissors\r\n");

    const char *dic[] = {"Invalid", "Rock", "Paper", "Scissors"};

    while (1)
    {
        // Player 1 (PC)
        xil_printf("\r\nMake the first move, Player 1 (PC): ");
        serial = read_number();

        // Player 2 (FPGA)
        xil_printf("Your turn, Player 2 (FPGA): ");
        
        // loop forever until keypad_scan returns something other than 0
        while (1) {
            keypad_char = keypad_scan(&keypad_gpio);
            if (keypad_char != 0) {
                keypad_int = keypad_char - '0'; 
                break;
            }
        }

        // game logic
        if (serial == keypad_int) {
             xil_printf("Result: Draw! %s == %s\r\n", dic[serial], dic[keypad_int]);
        }
        else if ((serial == 1 && keypad_int == 3) || (serial == 3 && keypad_int == 1)) {
            // special rock/scissors cases
            if (serial == 1) 
                xil_printf("Result: PC won! %s > %s\r\n", dic[serial], dic[keypad_int]);
            else 
                xil_printf("Result: FPGA won! %s < %s\r\n", dic[serial], dic[keypad_int]);
        }
        else if (serial > keypad_int) {
            xil_printf("Result: PC won! %s > %s\r\n", dic[serial], dic[keypad_int]);
        } 
        else {
            xil_printf("Result: FPGA won! %s < %s\r\n", dic[serial], dic[keypad_int]);
        }

        // delay before next round
        for(int i=0; i<1000000; i++); 
    }
}