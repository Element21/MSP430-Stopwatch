# Examples

## Stopwatch
### Architecture

The Stopwatch relies on interrupts to separate updating the internal time from rendering it. This allows the timer to function without being blocked by slow LCD operations.

### Interupt Safety
Because the main loop can be interrupted by the Timer while reading `time_count`, we lock the interrupts during read to prevent data tearing:

```c
--8<-- "main.c:interrupt_safe"
```

### DeltaChar(tm) Technology! 
Instead of clearing the display (`0x01`) and writing 16 characters sequentially, the algorithm determines `first_diff` and `last_diff`.

```c
--8<-- "main.c:deltachar"
```

## Hello World

```c
--8<-- "examples/hello_lcd.c"
```
