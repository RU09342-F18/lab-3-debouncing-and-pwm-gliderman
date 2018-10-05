# Nate Hoffman's Software PWM

Using a timer to provide a pulse width modulation (PWM) signal to an LED to change it's brightness. This is the software implementation that can be used on any pin. If you were looking for a hardware implementation, see [here](https://github.com/RU09342-F18/lab-3-debouncing-and-pwm-gliderman/tree/master/Hardware%20PWM).

## Operation

A timer running at 1 MHz is stepped down to 250 kHz (roughly). This allows a counter running from 0-255 to loop 1000 times a second (running the PWM at 1 kHz). 8 bits provides a range of 0-255 which ties in with the timer to provide those different levels. Every time the button is pressed the duty cycle is increased by 10%. Once it passes 100% it wraps around to 0%.

## Devices

Code was written that runs on an MSP430F5529 and MSP430FR2311. The F5529 board uses two LEDs connect to P1.0 and P4.7 along with a button connected to P2.1. The FR2311 board uses two LEDs connected to P1.0 and P2.0 along with a button connected to P1.1.
