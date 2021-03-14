#ifndef _DEBOUNCE_H
#define _DEBOUNCE_H

typedef union break_integer{
	int integer16;
	char b[2];
} b_int_t;

typedef struct virt_key{
	b_int_t key_window;  // cria uma "cena" para termos uma lista com 16 bits de historial
	char count_ones;	/// conta o nr de "1"
	char key_now;     /// estado atual da key
} virt_key_t;


virt_key_t data *pSet  = 0x30;
virt_key_t data *pLoad = 0x40;


#endif