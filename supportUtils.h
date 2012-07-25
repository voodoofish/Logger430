#ifndef SUPPORTUTILS_H_
#define SUPPORTUTILS_H_


void WD_intervalTimerInit(void);
void delay(unsigned int ms);
void setpins(unsigned char pins);
void blinkbit(unsigned char bits, unsigned char time);
void blinkfun();
void endBlink(int interval);
void adcConvert(unsigned short channels);


#endif /*SUPPORTUTILS_H_*/
