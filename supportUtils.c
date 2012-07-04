
#include "msp430.h"

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
void blinkfun()
{   P2OUT |=0x1;
	delay(200);
	P2OUT &= ~0x1;
	delay(200);
    P2OUT |=0x2;
	delay(200);
	P2OUT &= ~0x2;
	delay(200);
    P2OUT |=0x4;
	delay(200);
	P2OUT &= ~0x4;
	delay(200);
    P2OUT |=0x2;
	delay(200);
	P2OUT &= ~0x2;
	delay(200);
    P2OUT |=0x1;
	delay(200);
	P2OUT &= ~0x1;
	delay(200);
	
	
	}
