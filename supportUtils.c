
#include "msp430.h"

void WD_intervalTimerInit(void)
{
  WDTCTL = WDT_MDLY_32;                    // WDT 250ms, ACLK, interval timer
  IE1 |= WDTIE;                             // Enable WDT interrupt
}
void delay(unsigned int ms)
{
 while (ms--)
    {
        __delay_cycles(1000); // set for 16Mhz change it to 1000 for 1 Mhz
    }
}

void blinkbit(unsigned char bits, unsigned char time){
    P2OUT |=bits;
	delay(time);
	P2OUT &= ~bits;
	delay(time);
}

void setpins(unsigned char pins){
//This does a blink based on the button press
unsigned char counterLoop=0;
	while(counterLoop < 3){
	/*
	P2OUT |=pins;
	delay(200);
	P2OUT &= ~pins;
	delay(200);
	*/
	blinkbit(pins, 150);
	counterLoop++;
	}
	counterLoop = 0;
}

void blinkfun(){
	blinkbit(BIT1, 100);
	blinkbit(BIT2, 100);
	blinkbit(BIT4, 100);
	blinkbit(BIT2, 100);
	blinkbit(BIT1, 100);
}
void endBlink(int interval){
char i = 0;
	while (i < interval){
	blinkbit(BIT1, 150);
	blinkbit(BIT4, 150);
	i++;
	}
}


