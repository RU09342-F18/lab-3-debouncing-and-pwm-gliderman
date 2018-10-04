#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5; // Unlock GPIO ports
	CSCTL4 = SELA_1; // 32 kHz ACLK
	
	// LED
	P1DIR |= BIT0; // Set P1.0 to output direction
	P1SEL0 &= ~BIT0; // Set P1.0 as I/O
	P1SEL1 &= ~BIT0; // Set P1.0 as I/O
	P1OUT &= ~BIT0; // P1.0 turn off LED

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

	// Setup timer
    TB0CTL = TBSSEL_1 + MC_0 + TBCLR; // ACLK, off, clear

    // Setup CCR0 for timing out after the button is done bouncing
    TB0CCR0 = 128;

	_BIS_SR(LPM3_bits + GIE);
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    TB0CTL = TBSSEL_1 + MC_1 + TBCLR + TBIE; // ACLK, up, clear, enable interrupt
    P1IES ^= BIT1;
    P1IFG &= ~BIT1;
}

#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer0_B1(void) {
    TB0CTL = TBSSEL_1 + MC_0 + TBCLR; // ACLK, off, clear
    if (P1IN & BIT1) {
        P1OUT ^= BIT0;
    }
}
