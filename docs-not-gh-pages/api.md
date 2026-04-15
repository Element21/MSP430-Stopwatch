# LCD + Stopwatch API Reference

## LCD Functions

### `initLCD`

```c
void initLCD(void);
```

Sets the correct MSP430 pins (P1.4 - P1.7, P2.0, P2.1) as outputs and sends the required initialization sequence to the LCD. Sets the display into 4-bit mode, configures for 2 lines, 5x8 font, increments the cursor, and turns on the cursor blinking.

### `printString`

```c
void printString(char* text, unsigned int delay_ms);
```

Iterates through a null-terminated string and prints each character sequentially. Includes a built-in busy delay function `delay_ms` wait per character.

**Parameters:**
- `text`: Pointer to the null-terminated string/character array.
- `delay_ms`: Software delay in milliseconds between each character.

### `writeChar`

```c
void writeChar(char data);
```

Sets the RS pin (`P2.0`) HIGH and places the bits corresponding to the character onto the data bus (`P1.4 - P1.7`). Uses a falling edge read via `pulseEnable()`.

**Parameters:**
- `data`: ASCII encoded data to send to the LCD data register.

### `writeCommand`

```c
void writeCommand(unsigned char command);
```

Clears the RS pin (`P2.0`) to LOW and passes command bit instructions to the LCD. Uses `pulseEnable()` to clock in the corresponding upper and lower nibbles.

**Parameters:**
- `command`: 8-bit instruction representing commands (e.g. `0x01` for Clear Display).

### `pulseEnable`

```c
void pulseEnable(void);
```

Primes the `E` (Enable) line HIGH, waits for `450_ns`, and pulls it LOW to clock in data on the falling edge. This is an internal function utilized by `writeChar()` and `writeCommand()`.


## Stopwatch Main Variables

- **`time_count`**: Counter tracking elapsed time in 0.1ms (100µs) increments.
- **`running`**: Boolean flag toggled when the stopwatch starts or pauses.
- **`button_pressed`**: Boolean reflecting whether the button is currently held down.
- **`button_timer`**: Tracks how long the button has been held. Reaching `HOLD_THRESHOLD` (10000 ticks = 1s default) triggers a stopwatch reset.

## Interrupt Service Routines

### `Timer_A (TIMER0_A0_VECTOR)`
Triggers repeatedly every 0.1ms. If `running` is `true`, `time_count` is incremented. If `button_pressed` is `true`, `button_timer` increments to track hold duration.

### `P1_ISR (PORT1_VECTOR)`
Edge-triggered hardware interrupt for the P1.3 push button.
- On a falling edge (button push), registers the press, resets `button_timer` and flips the edge selection to rising.

- On a rising edge (button release), applies software debouncing. If the button was depressed longer than `123 * 0.1ms` but less than `HOLD_THRESHOLD`, the `running` flag is toggled. Finally, it flips the edge selection back to falling.
