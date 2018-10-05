#include <msp430.h> 

/**
 * Author: Nate Hoffman
 * Date: October 5, 2018
 * Hardware PWM
 * Description: A PWM signal with 256 different levels at
 * 1 kHz is connected to a timer that uses two CCRs. This
 * does not use interrupts to change the output state of
 * the signal. Instead the output of CCR1 is connected to
 * P1.7 on the board. A button connected to P1.1 is used
 * to increase the duty cycle by 10% each time it is
 * pressed, rolling over from 100% to 0%. An LED that is
 * connected to P2.0 shows the state of the button.
 *
 * It should be noted that CCR1 connected to P1.6, which
 * was pulled high by the programming side of the board.
 * CCR2 was used instead, which is connected to P1.7.
 */

void configureLED() {
    // LED1
    P1DIR |= BIT7; // Set P1.7 to output direction
    P1SEL0 &= ~BIT7; // Set P1.7 as TB0.1
    P1SEL1 |= BIT7; // Set P1.7 as TB0.1
    P1OUT &= ~BIT7; // P1.7 turn off LED

    // LED2
    P2DIR |= BIT0; // Set P2.0 to output direction
    P2SEL0 &= ~BIT0; // Set P2.0 as I/O
    P2SEL1 &= ~BIT0; // Set P2.0 as I/O
    P2OUT &= ~BIT0; // P2.0 turn off LED
}

void configureButton() {
    // Button
    P1DIR &= ~BIT1; // Set P1.1 to input
    P1SEL0 &= ~BIT1; // P1.1 as I/O
    P1SEL1 &= ~BIT1; // P1.1 as I/O
    P1REN |= BIT1; // P1.1 enable resistor
    P1OUT |= BIT1; // P1.1 pullup resistor

    // Configure button interrupts
    P1IES |= BIT1; // P1.1 high-to-low transition
    P1IE |= BIT1; // Enable P1.1 interrupt
    P1IFG &= ~BIT1; // P1.1 clear interrupt flag
}

void configurePWM() {
    // Setup timer to be 1 MHz
    TB0CTL = TBSSEL_2 + ID_2 + MC_1 + TBCLR; // SMCLK, /4, up, clear

    // Setup CCR0 for shutting down the LED
    TB0CCR0 = 255;

    // Start at about 50% brightness
    TB0CCR2 = 125;
    TB0CCTL2 = OUTMOD_7;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Unlock GPIO ports

    configureLED();
    configureButton();
    configurePWM();

    _BIS_SR(LPM0_bits + GIE);
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    if (P1IES & BIT1) { // The button is pressed
        TB0CCR2 += 25;
        if (TB0CCR2 > 255) {
            TB0CCR2 = 0; // Set to 0% duty cycle
        }
    }

    P2OUT ^= BIT0; // P2.0 toggle state
    P1IES ^= BIT1; // P1.1 toggle transition
    P1IFG &= ~BIT1; // Clear the P1.1 interrupt
}
