#include <msp430.h>
#include "InitLCD.h"
#include "PrintString.h"
#include "WriteCommand.h"
#include "WriteChar.h"

int example_main(void) {
    // Stop the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // Set DCO to 1MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    // Initialize the LCD in 4-bit mode
    initLCD();

    // Print a hello world message
    // Parameters: ("String", delay_ms per character)
    printString("Hello World!", 200);

    // Clear the display for another message
    // writeCommand(0x01); // 0x01 = Clear Display + delay included

    // writeChar('A');

    // Go to low power mode
    _BIS_SR(CPUOFF | GIE);

    return 0;
}
