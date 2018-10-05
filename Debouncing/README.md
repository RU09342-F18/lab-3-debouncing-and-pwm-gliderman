# Nate Hoffman's Software Debouncing

Using timers to provide a clean button press/unpress signal to the microcontroller. Eliminates the bouncing signal from the button as it is pressed or unpressed.

## Operation

A timer running at 32.768 kHz is used to time 4 ms on the FR2311 (2 ms on the F5529) after the last button state change. Once this timer has elapsed, it reads the button state to toggle the LED. If before the timer lapsed the button state changes, the timer is reset to start the counter again. This allows the button to bounce repeatedly, but only perform an action after it settles.

## Devices

Code was written that runs on an MSP430F5529 and MSP430FR2311. The F5529 uses an LED connected to P1.0 and a button connected to P2.1. The FR2311 uses an LED connected to P1.0 and a button connected to P1.1.
