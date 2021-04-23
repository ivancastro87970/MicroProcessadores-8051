#ifndef _DEBOUNCE_H
#define _DEBOUNCE_H

typedef union break_int{
	int integer16;
	char b[2];
} b_int;

typedef struct virtual_key{
	b_int key_window;  // cria uma "cena" para termos uma lista com 16 bits de historial
	char count_ones;	/// conta o nr de "1"
	char key_now;     /// estado atual da key
} virt_key;

virt_key data a;
virt_key data b;

virt_key data *pSet  =&a;
virt_key data *pLoad =&b;


#endif