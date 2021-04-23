#include <REG51F380.H>
#include "fifo.h"

static unsigned   char  ostart;                        // transmission buffer start index
static unsigned   char  oend;                          // transmission buffer end index
static char idata outbuf[OLEN];                        // storage for transmission buffer

static unsigned   char  istart;                        // receiving buffer start index
static unsigned   char  iend;                          // receiving buffer end index
static char idata inbuf[ILEN];                         // storage for receiving buffer

void com_initialize (void)
{
  	istart = 0;                                  // empty transmit buffers
  	iend = 0;
  	ostart = 0;                                  // empty transmit buffers
  	oend = 0;
                
}

#pragma disable
int inbuff_push (char c) 
{
	if((iend - istart) >= ILEN)
		return 1;
	
	inbuf[iend++ & (ILEN - 1)] = c;	
	RI0 = 0;

return 0;	
}
#pragma disable
int inbuff_pop (void)
{	
 	if(istart == iend)
		return 1;
	
return inbuf[istart++ & (ILEN - 1)];
 }

 #pragma disable
int outbuff_push (char c) 
{
 		if((oend - ostart) >= OLEN)
		return 1;
	
	outbuf[oend++ & (OLEN - 1)] = c;
			
	return 0;	
}		

int outbuff_pop(void)
{
		if(ostart == oend){
		TI0 = 0;
		return 1;
	}
	return outbuf[ostart++ & (OLEN - 1)];
}


char putchar(char c){
	outbuff_push(c);
	return c;
}


