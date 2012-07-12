//Project DataRecorder
//The idea of this project is to have a small, simple but configurable, and easy to use remote data recorder.
//This makes use of ADC, USCIA(UART and possible extra ADC channels) and USCIB(spi link to onboard memory), 
//Watchdog timer as an interval timer for short and log time periods. This leaves TimerA and TimerB open for use.
//As well as the CompA which has the potential to be a battery monitor....though not sure on that part just yet.
//We also use 3 LED's as status indicators of how the device is currently hooked up.

/*LED blink status
 * LED's are on pins P2.0, P2.1 and P2.2 
 * Blink patterms are as follows.
 * 001 -> 1 -> 0x1 Single channel read using channel 0 
 * 010 -> 2 -> 0x2 double channel read using channel 0 and 3
 * 011 -> 3 -> 0x3 triple channel read using channel 0, 3 and 4
 * 100 -> 4 -> 0x4 quad channel 0, 3,4 and temp sensor
 * 101 -> 5 -> 0x5 5 channels 0,1,2,3,and 4 using 1 and 2 which alts as uart com/ OPT1: erase chip memory
 * 110 -> 6 -> 0x6 6 channels 0,1,2,3,4 and temp sensor/ OPT2:???
 * 111 -> 7 -> Command mode, turns on UART and allows user to set some variables
 * (saved to flash something like 16 bytes maybe?)
 * 
 * Buttons are on P2.3 and P2.5
 * Button Mode on start 
 * button 2 iterates through possible selections
 * button 1 selects option. for 1-6 this selects the different ADC channel options.
 * \_Branch 1 on select, timer interval is set next
 * 	|_WDT timer 1, 2 and 3,
 *  |_options 4,5 and 6 might be set to 1 minute,5 minutes and 10 minutes though this is something 
 *  that I'm thinking can be configurable via the command mode and saved to flash memory. 
 *  Once adc channels and time are set, the device is considered armed and ready. The LEDs will alternate with a 
 *  (101-010)pattern. To start and stop logging, press button 2.When stopped it will do an alternating pattern again.
 *  on active write, the center led will blink once. This may be a configurable option as well, well see. 
 *  Not sure of the best way to handle a reset. STart over from the beggining, or look for the last written byte. 
 * 
 * UART command mode options
 * 1. Pull data from mem chip.
 * 2. Erase chip.
 * 3. Configure options. which options? Where to save?
 * 4. 
 */

#include "msp430g2353.h"
#include "supportUtils.h"
#include "spi.h"
#include "25AA.h"


#define CS BIT4//p2.4
//SI	P1.7
//SO	P1.6
//CLK 	P1.5
 
unsigned char S1 = 0; //button1/action  Also used as reset button when switch is enabled
unsigned char S2 = 0; //button2/iterator
unsigned char toggler = 0x0;
unsigned char blinky = 0x0;
unsigned char j=0;
unsigned char myData=0;
unsigned char counter = 0;
unsigned char MYPORT = 2;
unsigned char statusRg = 0xff;
unsigned char rxbuff;
//volatile unsigned int i; 
void delay(unsigned int ms);


void main(void)
{
//Configure buttons and CS	
WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

/* SEE NOTES AT BOTTOM of this blurb
 * I've run into a probem which has been solved by setting the two port pins P2.6 and P2.7 to IO pins at the start of the main function. 
 * Normally they are * set to be used as Xin and Xout and for all intents and purposes 
 * I had planned to make them port pins anyway, but in 
 * the current version of this code, and with the current version of this board, leaving these two pins in the default state
 * appears to cause the UCB0STAT-->USBUSY flag to stay active. This caused issues after doing the first tx of data as the
 * txifg flag in in IFG2 would never appear to empty and would lock the while loop that checked this flag to get stuck in a 
 * permanent loop. Initial thoughts and discussions with Rickta brought up a possible issue with the tx/rx and lines and needing
 * to pull the pin up(or down I no longer have the chat log...). One thing that was also brought up and in just handling the device
 * was that touching 2 of the pins would cause the busy flag to reset(odd as that may be). This lead me to thinking that setting
 * the two mentioned pins could be set as IO pins. inital attempts failed to solve the issue, though it did bring up something
 * else....touching those pins no longer had any effect on the busy flag, it would just stay in the permanent busy state. 
 * I had also tried changing the pins to outputs and keeping them low but to no affect as well. Finally I tried to place the
 * port select setting near the top of the main function so it would be set asap. This appears to have fixed the issue...but 
 * also leaves a couple of questions. Why did I have to place it at the start of the code? What would the impact be if I had 
 * an actual crystal in place? Would I run into the same issue? What is actually causing the busy lockup? 
 * NOTE 1 I initiall left out the fact that I had no txIFG handler...this may have been part of the issue.
 * NOTE 2 I haven't been able to reproduce the exact issue today...I've done a lot of work late last night and may have forgotten
 * something due to the late hour...
 */
  
//Added P2SEL to the start as it seems to cause in issue if issued later. Need to do more tests with it.
P2SEL &= ~(BIT6 + BIT7);
//P2OUT |= BIT6 + BIT7;
//P2DIR |= BIT6 + BIT7;

P2OUT &= ~(0x07 + CS); //Set P2.0 - P2.2 output to low.
P2DIR |= (0x07 + CS); // Set P2.0 - P2.2 output direction
P2OUT |= CS;

//Configure button ints
P2IE |= 0x28; // P2.3 and P2.5 interrupt enabled
P2IES |= 0x28; // P2.3 and P2.5  Hi/lo edge
P2IFG &= ~0x28; // P2.3 and P2.5  IFG cleared

//Configure SPI IO ports by setting the clock, MOSI and MISO to UCSI mode
P1SEL |=BIT5 + BIT6 + BIT7; 
P1SEL2 |=BIT5 + BIT6 + BIT7;
//get spi working
spiInit();
spiStop();
spiStart();

//see if this helps as this should initialize the chip
__no_operation(); 
__no_operation(); 
enablePin(CS,MYPORT);
__no_operation(); 
__no_operation(); 
disablePin(CS,MYPORT);
__no_operation(); 
__no_operation(); 
//statusRg = readStatusReg(CS, MYPORT, RDSR);//This function just does a read so should work with both status RDSR and RDID 

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
			enablePin(CS,MYPORT);
			__no_operation(); 
			__no_operation(); 
			spiTx(WREN); //init the write enable
			//writeStatusReg(CS, MYPORT, WRSR,0xC);
			P2OUT ^=0x4;
			__no_operation(); 
			__no_operation(); 
			disablePin(CS,MYPORT);
			delay(50);
			//while(readStatusReg(CS, MYPORT, RDSR)&0x01==0x01){};
			statusRg = readStatusReg(CS, MYPORT, RDSR);//This function just does a read so should work with both status RDSR and RDID
			P2OUT ^=0x4;
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
_BIS_SR(LPM0_bits + GIE); // Enter LPM4 w/interrupt
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

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
rxbuff = UCB0RXBUF;
//_low_power_mode_off_on_exit();
//blinkfun();
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
//rxbuff = UCB0RXBUF;
_low_power_mode_off_on_exit();
//blinkfun();
}
