#ifndef _FIFO_H_
#define _FIFO_H_


#define    OLEN  16                             // size of serial transmission buffer
#define    ILEN  16                              // size of serial receiving buffer

extern void com_initialize();

int inbuff_push (char);
int inbuff_pop (void);
int outbuff_push (char);
int outbuff_pop(void);


#endif //_FIFO_H_