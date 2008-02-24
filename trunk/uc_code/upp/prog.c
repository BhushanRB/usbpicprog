/*********************************************************************
 *
 *                UsbPicProg v0.1
 *                Frans Schreuder 23-02-2008
 ********************************************************************/
#ifdef SDCC
#include <pic18f2550.h>
#else
#include <p18cxxx.h>
#endif
#include "system\typedefs.h"
#include "io_cfg.h"             // I/O pin mapping
#include "prog.h"
#include "system\interrupt\interrupt.h"

ERASESTATE erasestate=ERASEIDLE;
PROGSTATE progstate=PROGIDLE;
long lasttick=0;
extern long tick;

/**
This function has to be called as many times until erasestate==ERASESUCCESS
*/

void bulk_erase(PICTYPE pictype)
{
	char i;
	switch(erasestate)
	{
		case ERASESTART:
			VDD=0; //high, (inverted)
			for(i=0;i<10;i++)continue; //wait at least 100 ns;
			VPP=0; //high, (inverted)
			for(i=0;i<10;i++)continue; //wait at least 2 us;

			erasestate=ERASE1;
			break;
		case ERASE1:
			switch(pictype)
			{
				case PIC18:
					pic18_send(0x00,0x0E3C); //MOVLW 3Ch
					pic18_send(0x00,0x6eF8); //TBLPTRU
					pic18_send(0x00,0x0E00); //MOVLW 00h
					pic18_send(0x00,0x6eF7); //TBLPTRH
					pic18_send(0x00,0x0E05); //MOVLW 05h
					pic18_send(0x00,0x6eF6); //TBLPTRL
					pic18_send(0x0C,0x3F3F); //Write 3F3Fh to 3C0005h
					pic18_send(0x00,0x0E3C); //MOVLW 3Ch
					pic18_send(0x00,0x6eF8); //TBLPTRU
					pic18_send(0x00,0x0E00); //MOVLW 00h
					pic18_send(0x00,0x6eF7); //TBLPTRH
					pic18_send(0x00,0x0E04); //MOVLW 04h
					pic18_send(0x00,0x6EF6); //TBLPTRL
					pic18_send(0x0C,0x8FF8); //TBLPTRU
					pic18_send(0x00,0x0000); //NOP
					lasttick=tick;
					pic18_send(0x00,0x0000); //hold PGD low until erase completes
					
					break;
				default:
					break;
			}
			
			erasestate=ERASE2;
			break;
		case ERASE2:
			if((tick-lasttick)>P11)
			{
				erasestate=ERASESTOP;
				lasttick=tick;
			}
			break;
		case ERASESTOP:
			VPP=1; //low, (inverted)
			Nop();
			VDD=1; //low, (inverted)
			if((tick-lasttick)>P10)
				erasestate=ERASESUCCESS;
			break;
		case ERASEIDLE:
			break;
		case ERASESUCCESS:
			break;
		default:
			erasestate=ERASEIDLE;
			break;
			
			
	}
}



/**
This function has to be called as many times until progstate==PROGNEXTBLOCK
or when lastblock=1:
call as many times until progstate==PROGSUCCESS
*/
void program_memory(PICTYPE pictype,unsigned long address, char* data,char blocksize,char lastblock)
{
	char i;
	char blockcounter;
	switch(progstate)
	{
		case PROGSTART:
			VDD=0; //high, (inverted)
			for(i=0;i<10;i++)continue; //wait at least 100 ns;
			VPP=0; //high, (inverted)
			for(i=0;i<10;i++)continue; //wait at least 2 us;

			progstate=PROG1;
			break;
		case PROG1:
			switch(pictype)
			{
				case PIC18:
					pic18_send(0x00,0x8EA6); //BSF EECON1, EEPGD
					pic18_send(0x00,0x9CA6); //BCF EECON1, CFGS
					break;
				default:
					break;
			}
			progstate=PROG2;
		case PROG2:
			switch(pictype)
			{
				case PIC18:
					pic18_send(0x00,(unsigned int)(0x0E|((address>>16)&0x3F))); //MOVLW Addr [21:16]
					pic18_send(0x00,0x6EF8); //MOVWF TBLPTRU
					pic18_send(0x00,(unsigned int)(0x0E|((address>>8)&0xFF))); //MOVLW Addr [15:8]
					pic18_send(0x00,0x6EF7); //MOVWF TBLPTRU
					pic18_send(0x00,(unsigned int)(0x0E|((address)&0xFF))); //MOVLW Addr [7:0]
					pic18_send(0x00,0x6EF6); //MOVWF TBLPTRU
					for(blockcounter=0;blockcounter<(blocksize-2);blockcounter+=2)
					{

						//write 2 bytes and post increment by 2
						//				MSB				LSB
						pic18_send(0x0D,((unsigned int)*(data+blockcounter))<<8|((unsigned int)*(data+1+blockcounter))); 
					}
					//write last 2 bytes of the block and start programming
					pic18_send(0x0F,((unsigned int)*(data+blockcounter))<<8|((unsigned int)*(data+1+blockcounter))); 
					pic18_send(0x00, 0x0000); //nop, hold PGC high for time P9 and low for P10
					break;
				default:
					break;
			}
			lasttick=tick;
			PGC=1;	//hold PGC high for P9
			progstate=PROG2;
			break;
		case PROG3:
			if((tick-lasttick)>P9)
			{
				progstate=PROG3;
				PGC=0;	//hold PGC low for time P10
				lasttick=tick;
			}
			break;
		case PROG4:
			if((tick-lasttick)>P10)
			{
				if(lastblock==0)progstate=PROGNEXTBLOCK;
				else progstate=PROGSTOP;
			}
		case PROGNEXTBLOCK:		
			/**
			the higher level program has to check for this state and load the next block
			then make progstate PROG2 to continue the next block
			**/
			break;
		case PROGSTOP:
			VPP=1; //low, (inverted)
			Nop();
			VDD=1; //low, (inverted)
			if((tick-lasttick)>P10)
				progstate=PROGSUCCESS;
			break;
		case PROGIDLE:
			break;
		case PROGSUCCESS:
			break;
		default:
			progstate=PROGIDLE;
			break;
	}
}

void program_ids(PICTYPE pictype,char address, char* data, char blocksize)
{
}
void program_data_ee(PICTYPE pictype,char address, char* data, char blocksize)
{
}
char verify_program(PICTYPE pictype,char address, char* data, char blocksize)
{
}
char verify_ids(PICTYPE pictype,char address, char* data, char blocksize)
{
}
char verify_data(PICTYPE pictype,char address, char* data, char blocksize)
{
}
void program_config_bits(PICTYPE pictype,char address, char* data, char blocksize)
{
}

void pic18_send(char command, unsigned int payload)
{
	char i;
	TRISPGD=0;
	TRISPGC=0;
	PGC=0;
	PGD=0;
	for(i=0;i<3;i++)
	{
		
		PGC=1;
		Nop();
		if(command&(1<<i))PGD=1;
		else PGD=0;
		Nop();
		PGC=0;
		Nop();
		
	}
	Nop();	//wait at least 40ns
	for(i=0;i<15;i++)
	{
		
		PGC=1;
		Nop();
		if(payload&(1<<i))PGD=1;
		else PGD=0;
		Nop();
		PGC=0;
		Nop();
		
	}
	Nop();
	
}