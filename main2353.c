//Project blinker button
//The idea of this project is to have led1 light on while the button is not pressed
//When pressed, led1 will go off and the LED2 will blink X times. LED1 will come
//on again.

/*LED blink status
 * LED's are on pins P2.0, P2.1 and P2.2
 * 001 -> 1 -> 0x1
 * 010 -> 2 -> 0x2
 * 011 -> 3 -> 0x3
 * 100 -> 4 -> 0x4
 * 101 -> 5 ->
 * 110 -> 6 ->
 * 111 -> 7 ->
 * 
 * Buttons are on P2.3 and P2.5
 */

#include "msp430g2353.h"
#include "supportUtils.h"

unsigned char S1 = 0; //button1/action  Also used as reset button when switch is enabled
unsigned char S2 = 0; //button2/selector
int toggler = 0x0;
int blinky = 0x0;
int j=0;
unsigned char counter = 0;
volatile unsigned int i; 
void delay(unsigned int ms);


void main(void)
{
WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
P2OUT &= ~0x07; //Set P2.0 - P2.2 output to low.
P2DIR |= 0x07; // Set P2.0 - P2.2 output direction

P2IE |= 0x28; // P2.3 and P2.5 interrupt enabled
P2IES |= 0x28; // P2.3 and P2.5  Hi/lo edge
P2IFG &= ~0x28; // P2.3 and P2.5  IFG cleared

while(1){
	if (blinky ==1){
	
	switch(S2)
	{
	case 1 :
		if (S1 == 0){//if S1 is set, skip the setpins blink and just go to the actual code we'll run.
			setpins(0x1);}
		else
			{blinkfun();
			S2 = 0;}
		break;
	case 2 :
		if (S1 == 0){	
			setpins(0x2);}	
		else
			{blinkfun();
			S2 = 0;}
		break;
	case 3 :
		if (S1 == 0)	
			setpins(0x3);
		else
			{blinkfun();
			S2 = 0;}
		break;
	case 4 :
		if (S1 == 0)	
			setpins(S2);
		else
			{blinkfun();
			S2 = 0;}
		break;
	case 5 :
		if (S1 == 0)	
			setpins(S2);
		else
			{blinkfun();
			S2 = 0;}
		break;
	case 6 :
		if (S1 == 0)	
			setpins(S2);
		else
			{blinkfun();
			S2 = 0;}
		break;
	case 7 :
		if (S1 == 0)	
			setpins(S2);
		else
			{blinkfun();
			S2 = 0;}
		break;
		
	default :
	counter = 0;
	}
	
		/*	
	    if (toggler == 0)
		{
			while((BIT3 & P2IN) == 0x0){
			P2OUT |=0x01;
			delay(200);
			P2OUT &= ~0x01;
			delay(200);
			}
		toggler = 1;
		}
		else
		{P2OUT |=0x03;
		delay(200);
		P2OUT &= ~0x03;
		toggler = 0;
		}
		*/
	}
	blinky = 0;
	//S2 = 0;
	S1 = 0;
_BIS_SR(LPM4_bits + GIE); // Enter LPM4 w/interrupt
}
}

// Port 1 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{

if((0x08 & P2IFG)==0x08) 
{ if(S2>=7)
	{S2=0;}
	else 
	{S2 +=1;}
}

if((P2IFG & 0x20)==0x20) 
{S1 = 1;}
blinky = 1;
P2IFG &= ~0x08; // P2.3 IFG cleared
P2IFG &= ~0x20; // P2.5 IFG cleared
_low_power_mode_off_on_exit();
}

