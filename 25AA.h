#ifndef mem25AA_INC
#define mem25AA_INC
/*
 *Instruction Name Instruction Format Description
 *READ 0000 0011 Read data from memory array beginning at selected address
 *WRITE 0000 0010 Write data to memory array beginning at selected address
 *WRDI 0000 0100 Reset the write enable latch (disable write operations)
 *WREN 0000 0110 Set the write enable latch (enable write operations)
 *RDSR 0000 0101 Read STATUS register
 *WRSR 0000 0001 Write STATUS register 
 * 
 */

#define READ 	0x03
#define WRITE	0x02
#define WRDI	0x04
#define WREN	0x06
#define RDSR	0x05
#define WRSR	0x01
//Additional instructs for 25A512
#define PE		0x42 //Page Erase
#define SE		0xD8 //Sector Erase
#define CE		0xC7 //chip Erase
#define RDID	0xAB //release from deep powerdown and read electronic signiture
#define DPD		0xB9 // deep powerdown mode


//other deinfes for the chip
#define MAXPAGE 511 //512 pages 0-511
#define MAXPAGESIZE 127 //128 bytes 0-127
#define MAXMEM 65535 //65536 bytes total

//these should go into their own header files.
int findPageStart(int page);
int findPageMemLoc(int page, int loc);
void writeEn(void);
void writePageMem(int page);
void wrtiePageLoc(int loc, unsigned char writebyte, unsigned char pin, unsigned char port); //get this from findPageMemLoc() 
int readPageMem(int page);
unsigned char readPageMemLoc(int loc, unsigned char pin, unsigned char port);
unsigned char readStatusReg(unsigned char pin, unsigned char port, unsigned char command);
void writeStatusReg(unsigned char pin, unsigned char port, unsigned char command, unsigned char bits);
void chipErase(unsigned char pin, unsigned char port);
#endif /*mem25AA*/
