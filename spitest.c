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
int toggler = 0x0;
int blinky = 0x0;
int j=0;
unsigned char myData=0;
unsigned char counter = 0;
unsigned char MYPORT = 2;
unsigned char statusRg = 0xff;
unsigned char rxbuff;
unsigned char Reg;
volatile unsigned int i; 
void delay(unsigned int ms);


void main(void)
{
//Configure buttons and CS	
WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
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
enablePin(CS,MYPORT);
spiTx(WREN); //init the write enable
disablePin(CS,MYPORT);

statusRg = readStatusReg(CS, MYPORT, RDSR);//This function just does a read so should work with both status RDSR and RDID 
/*
enablePin(CS,MYPORT);
spiTx(WREN); //init the write enable
disablePin(CS,MYPORT);
delay(100);
enablePin(CS,MYPORT);
spiTx(WRITE); //send write command.
spiTxINT(0x0001);
//Reg = spiTx(0x00);
//Reg = spiTx(0x01);
spiTx(98);//send databyte
disablePin(CS,MYPORT);
delay(100);
enablePin(CS,MYPORT);
Reg = spiTx(READ);
//Reg = spiTx(0x00);
//Reg = spiTx(0x01);
spiTxINT(0x0001);
Reg = spiTx(0xff);//fake byte
//Reg = spiRx();
disablePin(CS,MYPORT);
*/

_BIS_SR(LPM0_bits + GIE); // Enter LPM4 w/interrupt

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
_low_power_mode_off_on_exit();
blinkfun();
}
