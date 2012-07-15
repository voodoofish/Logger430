//#include <io.h>
#include "spi.h"
#include "msp430.h"
#include <stdint.h>
/** \file spi.c
SPI functions are either on here or in spi.h.
*/
/** Initialise spi interface on uart0.
Set for 8 bits data, master, 3 pin mode.
Uses SMCLK with divide set to 2 (ie maximum speed). Interrupts disabled.
*/
/*I'm considering giving the spiInit function an option list so that I can turn on each one of the features.
 * I might need to just bite the bullet and keep this as a project specific function as it controls the setup.
 */
void spiInit(void) {
	UCB0CTL0 = UCMODE_0 + UCMST + UCMSB + UCCKPH;
	UCB0CTL1 = UCSSEL_2 + UCSWRST;			  //smclk
	UCB0BR0 =2; //60 does not work
    UCB0BR1 = 0;
    }

void spiStart(void){
	UCB0CTL1 &= ~UCSWRST; 
//	IE2 |= (UCB0RXIE);                          // Enable USCI0 RX interrupt  
}

void spiStop(void){
UCB0CTL1 |= UCSWRST; 
}

unsigned char spiTx(unsigned char c) {
P2OUT ^=0x1;
while (!(IFG2 & UCB0TXIFG)); // wait for previous tx to complete
UCB0TXBUF = c; // setting TXBUF clears the TXIFG flag
while (!(IFG2 & UCB0TXIFG)); // wait for previous tx to complete
P2OUT ^=0x2;
return UCB0RXBUF; // reading clears RXIFG flag
//__bis_SR_register(LPM0_bits + GIE);
 
}

unsigned char spiTxINT(uint16_t i) {
unsigned char rxTrash;
uint8_t lsb=0;
uint8_t msb=0;
__no_operation();
lsb=i&0xFF;
msb=(i>>8)&0xFF;
while (!(UC0IFG & UCB0TXIFG));
UCB0TXBUF = msb;
while (!(UC0IFG & UCB0RXIFG));
rxTrash = UCB0RXBUF;
UCB0TXBUF = lsb;
while (!(UC0IFG & UCB0RXIFG));
rxTrash = UCB0RXBUF;
return rxTrash;
} 

unsigned char spiRx()
{
unsigned char blah;
UCB0TXBUF =0xff;
while (!(UC0IFG & UCB0RXIFG));
return UCB0RXBUF;
}
//Enter the port pin and port...ex pin 4 on port 2
void enablePin(unsigned char bits,unsigned char ports)
{
	if (ports ==1){
	P1OUT &= ~bits;}
	else if(ports ==2){
	P2OUT &= ~bits;}
	else {P1OUT &= ~bits;}

}
 
void disablePin(unsigned char bits,unsigned char ports)
{

  if (ports ==1){
  P1OUT |= bits;}
  else if(ports ==2){
  P2OUT |= bits;}
  else{P1OUT |= bits;}

}

