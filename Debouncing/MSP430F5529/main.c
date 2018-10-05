#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
    UCSCTL4 = SELA_0; // Enable ACLK (32.768 kHz signal)
	WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

	// LED
	P1DIR |= BIT0; // Set P1.0 to output direction
	P1SEL &= ~BIT0; // P1.0 as I/O
	P1OUT &= ~BIT0; // P1.0 turn off LED

	// Button
	P2DIR &= ~BIT1; // Set P2.1 to input
	P2SEL &= ~BIT1; // P2.1 as I/O
	P2REN |= BIT1; // P2.1 enable resistor
	P2OUT |= BIT1; // P2.1 pullup resistor

	// Configure button interrupts
	P2IES |= BIT1; // P2.1 high-to-low transition
	P2IE |= BIT1; // Enable P2.1 interrupt
	
	// Setup timer to be 32.768 kHz
	TA0CTL = TASSEL_1 + MC_0 + TACLR; // ACLK, off, clear

	// Setup CCR0 for timing out after the button is done bouncing
	TA0CCR0 = 64;

	// Enter low power mode
	_BIS_SR(LPM3_bits + GIE);
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {
    TA0CTL = TASSEL_1 + MC_1 + TACLR + TAIE; // ACLK, up, clear, enable interrupt
    P2IES ^= BIT1; // P2.1 toggle transition
    P2IFG &= ~BIT1; // Clear the P2.1 interrupt
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1(void) {
    TA0CTL = TASSEL_1 + MC_0 + TACLR; // ACLK, off, clear
    if (P2IN & BIT1) { // Button is not pressed
        P1OUT ^= BIT0;
    }
}
