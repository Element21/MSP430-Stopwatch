# MSP430 LCD Library

Pathways: Technical + Communication + Documentation

This library provides a simple interface to control a 1602 LCD in 4-bit mode using an MSP430. 

## Included Functions

- **`initLCD`**: Sends initial setup commands correctly sequenced to set 4 bit mode, clear display, cursor on.
- **`printString`**: Writes strings onto the display with programmable per char delays.
- **`writeChar` / `writeCommand`**: Writes commands directly to the display.

## Getting Started

```md
# 4 Bit Mode Wiring

VCC -> + Rail (Female->Male)
GND -> - Rail (Male->Male)

\+ Rail -> VDD, LED+ (BLA) (Male->Male)
\- Rail -> VSS, LED- (BLK) (Male->Male)

Potentiometer (25k) -> + Rail (Male->Male)
|
-> V0 (Male->Male)
|
-> - Rail (Male->Male)

RW -> GND (Write mode) (Male->Male)
P1.4 -> D4 (Female->Male)
P1.5 -> D5
P1.6 -> D6
P1.7 -> D7
P2.0 -> RS
P2.1 -> E
```

Start here:

* [API Reference](api.md) - Functions and usage
* [Example Usage](examples.md) - Examples (stopwatch)
* [Codebase](https://github.com/Element21/MSP430-Stopwatch)
