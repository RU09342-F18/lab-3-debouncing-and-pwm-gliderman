#include <msp430.h> 

/**
 * Author: Nate Hoffman
 * Date: October 4, 2018
 * Software PWM
 * Description: A PWM signal with 256 different levels at
 * 1 kHz is generated using a timer and two CCRs. Every
 * time the button is pressed, the PWM pulse gets 10%
 * longer, wrapping around to 0% after 100% is reached.
 * The button is connected to P1.1, while the PWM output
 * is connected to an LED on P1.0. An LED on P2.0 shows
 * the state of the button.
 */

void configureLED() {
    // LED1
    P1DIR |= BIT0; // Set P1.0 to output direction
    P1SEL0 &= ~BIT0; // Set P1.0 as I/O
    P1SEL1 &= ~BIT0; // Set P1.0 as I/O
    P1OUT &= ~BIT0; // P1.0 turn off LED

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
    TB0CTL = TBSSEL_2 + ID_2 + MC_1 + TBCLR + TBIE; // SMCLK, /4, up, clear, enable interrupt

    // Setup CCR0 for shutting down the LED
    TB0CCR0 = 255;

    // Start at about 50% brightness
    TB0CCR1 = 125;
    TB0CCTL1 = CCIE;
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Unlock GPIO ports

    configureLED();
    configureButton();
    configurePWM();

    _BIS_SR(LPM0_bits + GIE);
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    if (P1IES & BIT1) { // The button is pressed
        TB0CCR1 += 25;
        if (TB0CCR1 > 255) {
            TB0CCR1 = 0;
            P1OUT &= ~BIT0;
            TB0CTL = TBCLR; // clear
        } else {
            TB0CTL = TBSSEL_2 + ID_2 + MC_1 + TBIE; // SMCLK, /4, up, enable interrupt
        }
    }

    P2OUT ^= BIT0; // P2.0 toggle state
    P1IES ^= BIT1; // P1.1 toggle transition
    P1IFG &= ~BIT1; // Clear the P1.1 interrupt
}

#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer0_B1(void) {
    switch (TB0IV) {
    case 2:
        // CCR1 interrupt
        P1OUT &= ~BIT0;
        break;
    case 14:
        // Timer overflow
        P1OUT |= BIT0;
        break;
    }
}
