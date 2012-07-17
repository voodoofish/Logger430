#include "25AA.h"
#include "spi.h"

unsigned char readStatusReg(unsigned char pin, unsigned char port, unsigned char command ){
unsigned char Reg = 0;
enablePin(pin,port);

spiTx(command);
Reg = spiTx(0xff);//fakebyte

disablePin(pin,port);

return Reg;		
}

void writeStatusReg(unsigned char pin, unsigned char port, unsigned char command, unsigned char bits ){
unsigned char Reg = 0;
enablePin(pin,port);
//__no_operation(); 
//__no_operation(); 
//__no_operation(); 
spiTx(command);
spiTx(bits);//fakebyte
//Reg = spiRx();
//__no_operation(); 
//__no_operation(); 
//__no_operation(); 

disablePin(pin,port);
}

unsigned char readPageMemLoc(int loc, unsigned char pin, unsigned char port){
unsigned char Reg = 0;
enablePin(pin, port);
//__no_operation(); 
//__no_operation(); 
//__no_operation(); 
spiTx(READ);
spiTxINT(loc);
Reg = spiRx();
//__no_operation(); 
//__no_operation(); 
//__no_operation(); 
disablePin(pin, port);
return Reg;
}

void wrtiePageLoc(int loc, unsigned char writebyte, unsigned char pin, unsigned char port) {
enablePin(pin,port);
spiTx(WREN); //init the write enable
disablePin(pin,port);
enablePin(pin,port);
spiTx(WRITE); //send write command.
spiTxINT(loc); //send location
spiTx(writebyte);//send databyte
disablePin(pin,port);
}

void chipErase(unsigned char pin, unsigned char port){
enablePin(pin,port);
spiTx(WREN); //init the write enable
disablePin(pin,port);
enablePin(pin,port);
spiTx(CE); //init the write enable
disablePin(pin,port);
}