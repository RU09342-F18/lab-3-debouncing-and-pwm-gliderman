# Nate Hoffman's Hardware PWM

Using a timer to provide a pulse width modulation (PWM) signal to an LED to change it's brightness. This is the hardware implementation which can only be used on certain pins (whatever is connected to the output of the CCRs). If you were looking for a software implementation, see [here](https://github.com/RU09342-F18/lab-3-debouncing-and-pwm-gliderman/tree/master/Software%20PWM).

## Operation

A timer running at 1 MHz is stepped down to 250 kHz (roughly). This allows a counter running from 0-255 to loop 1000 times a second (running the PWM at 1 kHz). 8 bits provides a range of 0-255 which ties in with the timer to provide those different levels. Every time the button is pressed the duty cycle is increased by 10%. Once it passes 100% it wraps around to 0%.

## Devices

Code was written that runs on an MSP430F5529 and MSP430FR2311. The F5529 board uses two LEDs connected to P1.2 and P4.7 along with a button connected to P2.1. The FR2311 board uses two LEDs connected to P1.7 and P2.0 along with a button connected to P1.1.

### Note for MSP430FR2311

It should be noted that CCR1 is connected to P1.6, which was pulled high by the programming side of the board. Instead, CCR2 was used, which is connected to P1.7.
