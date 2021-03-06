#include <msp430.h> 

/**
 * Author: Nate Hoffman
 * Date: October 5, 2018
 * Hardware PWM
 * Description: A PWM signal with 256 different levels at
 * 1 kHz is connected to a timer that uses two CCRs. This
 * does not use interrupts to change the output state of
 * the signal. Instead the output of CCR1 is connected to
 * P1.2 on the board. A button connected to P2.1 is used
 * to increase the duty cycle by 10% each time it is
 * pressed, rolling over from 100% to 0%. An LED that is
 * connected to P4.7 shows the state of the button.
 */

void configureLED() {
    // LED2
    P4DIR |= BIT7; // Set P4.7 to output direction
    P4SEL &= ~BIT7; // P4.7 as I/O
    P4OUT &= ~BIT7; // P4.7 turn off LED

    P1DIR |= BIT2; // Set P1.2 to output direction
    P1SEL |= BIT2; // P1.2 as TA
    P1OUT &= ~BIT2; // P1.2 turn off signal
}

void configureButton() {
    // Button
    P2DIR &= ~BIT1; // Set P2.1 to input
    P2SEL &= ~BIT1; // P2.1 as I/O
    P2REN |= BIT1; // P2.1 enable resistor
    P2OUT |= BIT1; // P2.1 pullup resistor

    // Configure button interrupts
    P2IES |= BIT1; // P2.1 high-to-low transition
    P2IE |= BIT1; // Enable P2.1 interrupt
}

void configurePWM() {
    // Setup timer to be 1 MHz
    TA0CTL = TASSEL_2 + ID_2 + MC_1 + TACLR; // SMCLK, /4, up, clear

    // Setup CCR0 for shutting down the LED
    TA0CCR0 = 255;

    // Start at about 50% brightness
    TA0CCR1 = 125;
    TA0CCTL1 = OUTMOD_7;
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    configureLED();
    configureButton();
    configurePWM();

    // Enter low power mode
    _BIS_SR(LPM0_bits + GIE);
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {
    if (P2IES & BIT1) { // The button is pressed
        TA0CCR1 += 25;
        if (TA0CCR1 > 255) {
            TA0CCR1 = 0; // Set to 0% duty cycle
        }
    }

    P4OUT ^= BIT7; // P4.7 toggle state
    P2IES ^= BIT1; // P2.1 toggle transition
    P2IFG &= ~BIT1; // Clear the P2.1 interrupt
}
