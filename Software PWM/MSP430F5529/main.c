#include <msp430.h> 

/**
 * Author: Nate Hoffman
 * Date: October 4, 2018
 * Software PWM
 * Description: A PWM signal with 256 different levels at
 * 1 kHz is generated using a timer and two CCRs. Every
 * time the button is pressed, the PWM pulse gets 10%
 * longer, wrapping around to 0% after 100% is reached.
 * The button is connected to P2.1, while the PWM output
 * is connected to an LED on P1.0. An LED on P4.7 shows
 * the state of the button.
 */

void configureLED() {
    // LED1
    P1DIR |= BIT0; // Set P1.0 to output direction
    P1SEL &= ~BIT0; // P1.0 as I/O
    P1OUT &= ~BIT0; // P1.0 turn off LED

    // LED2
    P4DIR |= BIT7; // Set P4.7 to output direction
    P4SEL &= ~BIT7; // P4.7 as I/O
    P4OUT &= ~BIT7; // P4.7 turn off LED
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
    TA0CTL = TASSEL_2 + ID_2 + MC_1 + TACLR + TAIE; // SMCLK, /4, up, clear, enable interrupt

    // Setup CCR0 for shutting down the LED
    TA0CCR0 = 255;

    // Start at about 50% brightness
    TA0CCR1 = 125;
    TA0CCTL1 = CCIE;
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
	
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
            TA0CCR1 = 0;
            P1OUT &= ~BIT0;
            TA0CTL = TACLR; // clear
        } else {
            TA0CTL = TASSEL_2 + ID_2 + MC_1 + TAIE; // SMCLK, /4, up, clear, enable interrupt
        }
    }

    P4OUT ^= BIT7; // P4.7 toggle state
    P2IES ^= BIT1; // P2.1 toggle transition
    P2IFG &= ~BIT1; // Clear the P2.1 interrupt
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1(void) {
    switch (TA0IV) {
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
