#include "25AA.h"
#include "msp430.h"

extern unsigned int memCounter; 
extern const unsigned int CS;
extern const unsigned short MYPORT;
extern unsigned short doConversion;

void WD_intervalTimerInit(unsigned char interval, unsigned short delay){
unsigned int currentDelay =0;
  	switch(delay)
	{
	case 1 :
	currentDelay = WDT_ADLY_1_9;
	break;
	case 2 :
	currentDelay = WDT_ADLY_16;
	break;
	case 3 :
	currentDelay = WDT_ADLY_250;
	break;
	case 4 :
	currentDelay = WDT_ADLY_1000;
	break;
	default :
	__no_operation();
	}
	
	unsigned char i = 0;
	WDTCTL = currentDelay;
	for(i=0;i<=interval;i++){
  	//WDTCTL = WDT_ADLY_250;                    // WDT 250ms, ACLK, interval timer
  	//WDTCTL = currentDelay;
  	//IE1 |= WDTIE;
	_low_power_mode_0();
	}
	WDTCTL = WDTPW + WDTHOLD;
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

void adcConvert(unsigned short channels){
	//BIT0, BIT3, BIT4, BIT10//primary channels
	//BIT1, BIT2 //secondary channels shared with HW/SW uart pins
	//TODO add logic for channels var.
	//TODO add logic for conseq and mode to use. Mode will be a selectable list of options in
	//a select case fashion.
	
	switch(channels){
	case 1 :
		ADC10CTL1 = INCH_0 + CONSEQ_0;
		ADC10DTC1 = 1;
		ADC10AE0 |= BIT0;
	break;
	case 2 :
		ADC10CTL1 = INCH_3 + CONSEQ_1;
		ADC10DTC1 = 2;
		ADC10AE0 |= BIT0 + BIT3;
	break;
	case 3 :
		ADC10CTL1 = INCH_4 + CONSEQ_1;
		ADC10DTC1 = 3;
		ADC10AE0 |= BIT0 + BIT3 + BIT4;
	break;
	
	default :
	__no_operation();	
	}

	ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE + REFON + SREF_1;//+ MSC 	// ADC10ON, interrupt enabled
	//ADC10CTL1 = INCH_0 + CONSEQ_0;//INCH_4;                  	// input A4
	//ADC10AE0 |= BIT0;//BIT4+BIT5;//0x10;				// PA.4 ADC option select
	//ADC10DTC1 = 1;//was 6 //this is a reminder boys and girls, don't just copy paste, know that
	//if you are using conseq_0 it's only going to do a single conversion not n>1
	//you will be waiting a long time for it to do nothing.
}
