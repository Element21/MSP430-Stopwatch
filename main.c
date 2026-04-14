#include "InitLCD.h"
#include "PrintString.h"
#include "WriteCommand.h"
#include "WriteChar.h"
#include <msp430.h>
#include <stdio.h>
#include <stdbool.h>

volatile unsigned long time_count = 0; // Count in 0.1ms (100us) increments
volatile bool running = false;
volatile bool button_pressed = false;
volatile unsigned long button_timer = 0;

#define DEBOUNCE_MAGIC = 123 // Substantial research went into this value, DO NOT CHANGE
#define HOLD_THRESHOLD 10000 // 10000 * 0.1ms = 1 second

int main(void) {
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    // Set system clock to 1MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    // Configure P1.3, input; pull-up
    P1DIR &= ~BIT3;
    P1OUT |= BIT3; // Pull-up resistor for P1.3
    P1REN |= BIT3;
    
    P1IE |= BIT3;  // Enable interrupt for button (P1.3)
    P1IES |= BIT3; // For transitions from HI-->LO (falling edge)
    P1IFG = 0x00;  // Ensure no interrupts are pending

    initLCD(); // Initialize the LCD

    // Setup timer
    // 1MHz SMCLK / 100 = 10kHz (1/10k = 100us or 0.1ms period)
    TA0CCTL0 = CCIE;          // TACCR0 interrupt enabled
    TA0CCR0 = 100;            // 0.1ms per interrupt
    TA0CTL = TASSEL_2 + MC_1; // SMCLK, upmode

    char displayBuffer[16] = {0};
    char prevBuffer[16] = {0};

    __enable_interrupt(); // Enable global interrupts

    // Clear display to begin
    writeCommand(0x01);

    while (1) {
            // --8<-- [start:interrupt_safe]
            // Protect read time_count to prevent partial overwriting of current_time
            unsigned long current_time;
            __disable_interrupt();
            current_time = time_count;
            __enable_interrupt();
            // --8<-- [end:interrupt_safe]

            // Format time: sssss.ssss (retain long precision)
            // 10KHz timer -> 0.1ms increments (4 decimal places for fraction of second)
            unsigned long sec = current_time / 10000;
            unsigned long sub_sec = current_time % 10000;

            int s4 = (sec / 10000) % 10;
            int s3 = (sec / 1000) % 10;
            int s2 = (sec / 100) % 10;
            int s1 = (sec / 10) % 10;
            int s0 = sec % 10;

            // Determine whether to show leading digits or replace with spaces (leading zero blanking)
            char c4, c3, c2, c1;

            // Blank the ten-thousands place if it is zero
            if (s4 > 0) {
                c4 = s4 + '0';
            } else {
                c4 = ' ';
            }

            // Blank the thousands place if all higher digits are zero
            if (s3 > 0 || s4 > 0) {
                c3 = s3 + '0';
            } else {
                c3 = ' ';
            }

            // Blank the hundreds place if all higher digits are zero
            if (s2 > 0 || s3 > 0 || s4 > 0) {
                c2 = s2 + '0';
            } else {
                c2 = ' ';
            }

            // Blank the tens place if all higher digits are zero
            if (s1 > 0 || s2 > 0 || s3 > 0 || s4 > 0) {
                c1 = s1 + '0';
            } else {
                c1 = ' ';
            }

            int ss3 = (sub_sec / 1000) % 10;
            int ss2 = (sub_sec / 100) % 10;
            int ss1 = (sub_sec / 10) % 10;
            int ss0 = sub_sec % 10;

            sprintf(displayBuffer, "%c%c%c%c%d.%d%d%d%d s ", 
                c4, c3, c2, c1, s0, ss3, ss2, ss1, ss0);
            
            // --8<-- [start:deltachar]
            // Find start and end positions of changed characters to reduce LCD print time
            int first_diff = -1;
            int last_diff = -1;
            int i;
            
            // Loop untill end of string or we hit the end of the buffer
            for (i = 0; displayBuffer[i] != '\0' && i < 16; ++i) {
                if (displayBuffer[i] != prevBuffer[i]) {
                    if (first_diff == -1) { first_diff = i; }
                    last_diff = i;
                }
            }

            if (first_diff != -1) {
                writeCommand(0x80 + first_diff); // Move cursor home + offset
                for (i = first_diff; i <= last_diff; ++i) {
                    writeChar(displayBuffer[i]);
                    prevBuffer[i] = displayBuffer[i]; // Write the changed chars
                }
            }
            // --8<-- [end:deltachar]
    }
}

// Timer A0 interrupt service routine
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {
    if (running) {
        time_count++;
    }
    
    if (button_pressed) {
        button_timer++;
        if (button_timer >= HOLD_THRESHOLD) {
            // Held for 1 second -> Clear stopwatch
            time_count = 0;
            running = false;
            button_pressed = false; // Reset so it doesn't repeatedly clear
        }
    }
}

#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void) {
    if (P1IFG & BIT3) { // Check if P1.3 caused the interrupt
        if (P1IES & BIT3) { // If falling edge (button pressed)
            P1IES &= ~BIT3; // Switch to rising edge (detect release)
            button_pressed = true;
            button_timer = 0; // Start counted held time (timer interrupt updates button_timer)
        } else { // Rising edge (button released)
            P1IES |= BIT3;  // Switch to falling edge (press)
            if (button_timer < HOLD_THRESHOLD && button_timer > 123) { 
                // Debounce and check if short press
                running = !running; // Toggle play/pause
            }
            button_pressed = false;
        }
        P1IFG &= ~BIT3; // Clear P1.3 interrupt flag
    }
}
