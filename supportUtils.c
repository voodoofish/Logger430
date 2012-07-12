
#include "msp430.h"

void WD_intervalTimerInit(void)
{
  WDTCTL = WDT_ADLY_1000;                    // WDT 250ms, ACLK, interval timer
  IE1 |= WDTIE;                             // Enable WDT interrupt
}
void delay(unsigned int ms)
{
 while (ms--)
    {
        __delay_cycles(1000); // set for 16Mhz change it to 1000 for 1 Mhz
    }
}

void setpins(unsigned char pins){
//do something here
unsigned char counterLoop=0;
	while(counterLoop < 3){
	P2OUT |=pins;
	delay(200);
	P2OUT &= ~pins;
	delay(200);
	counterLoop++;
	}
	counterLoop = 0;
}
void blinkbit(unsigned char bits, unsigned char time){
    P2OUT |=bits;
	delay(time);
	P2OUT &= ~bits;
	delay(time);
}
void blinkfun(){
	blinkbit(BIT1, 100);
	blinkbit(BIT2, 100);
	blinkbit(BIT4, 100);
	blinkbit(BIT2, 100);
	blinkbit(BIT1, 100);
}
/*
void blinkfun(){
    P2OUT |=BIT1;
	delay(200);
	P2OUT &= ~BIT1;
	delay(200);
    P2OUT |=BIT2;
	delay(200);
	P2OUT &= ~BIT2;
	delay(200);
    P2OUT |=BIT4;
	delay(200);
	P2OUT &= ~BIT4;
	delay(200);
    P2OUT |=BIT2;
	delay(200);
	P2OUT &= ~BIT2;
	delay(200);
    P2OUT |=BIT1;
	delay(200);
	P2OUT &= ~BIT1;
	delay(200);
}
*/
