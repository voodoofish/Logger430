#ifndef SUPPORTUTILS_H_
#define SUPPORTUTILS_H_


void WD_intervalTimerInit(unsigned char interval, unsigned short delay);
void delay(unsigned int ms);
void setpins(unsigned char pins);
void blinkbit(unsigned char bits, unsigned char time);
void blinkfun();
void endBlink(int interval);
void adcConvert(unsigned short channels);
//unsigned int writeHeader(unsigned short bitMode, unsigned short channels, unsigned int maxBytes);
unsigned char HeaderWriter(unsigned short channels, unsigned short bitmode, unsigned char *ptr);
#endif /*SUPPORTUTILS_H_*/
