//Project blinker button
//The idea of this project is to have led1 light on while the button is not pressed
//When pressed, led1 will go off and the LED2 will blink X times. LED1 will come
//on again.

#include "msp430g2353.h"
//#include "msp430x20x2.h"
int toggler = 0x0;
int blinky = 0x0;
int j;
volatile unsigned int i; 
void delay(unsigned int ms);


void main(void)
{
WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
P2OUT &= ~0x07; //Set P2.0 and P1.6 output to low.
P2DIR |= 0x07; // Set P2.0 and P1.6 to output direction
//P1OUT |=0x40; //sets P1.6 high
P2IE |= 0x08; // P2.3 interrupt enabled
P2IES |= 0x08; // P2.3 Hi/lo edge
P2IFG &= ~0x08; // P2.3 IFG cleared

while(1){
	if (blinky ==1){
		if (toggler == 0)
		{
		while((BIT3 & P2IN) == 0x0){
		P2OUT |=0x01;
		delay(500);
		P2OUT &= ~0x01;
		delay(500);
		}
		toggler = 1;
		}
		else
		{P2OUT |=0x03;
		delay(500);
		P2OUT &= ~0x03;
		toggler = 0;
		}
	}
	blinky = 0;
_BIS_SR(LPM4_bits + GIE); // Enter LPM4 w/interrupt
}
}

// Port 1 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
/*
P1OUT &= ~0x40; //Turn off P1.6

for(j=0;j<5;j++){ //on->off-->on->off-->on   with 5 and <= you get 3 blinks
    P1OUT ^=0x01; //sets P1.0 high
    i = 10000; // SW Delay
    do i--;
    while (i != 0);
	}
	
P1OUT &= ~0x01; //turn off the led after it lights for the last blink
P1OUT |=0x40; //sets P1.6 high
*/
blinky = 1;
P2IFG &= ~0x08; // P1.3 IFG cleared
_low_power_mode_off_on_exit();
}
void delay(unsigned int ms)
{
 while (ms--)
    {
        __delay_cycles(1000); // set for 16Mhz change it to 1000 for 1 Mhz
    }
}
