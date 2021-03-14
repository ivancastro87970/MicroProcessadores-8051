#include <REG51F380.H>
#include "Debounce.h"
#include <stdio.h>

#define PDISP P2
#define TI1 1
#define RI1 1

extern void Init_Device(void);

//----------------------------------------------------------------------------------------------------------------

data unsigned char c;
static char index =0;
//----------------------------------------------------------------------------------------------------------------

code char DIGIT_MASK = 16;
code char array_digit[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E};
// ----------------------------------------------------------------------------------------------------------------

volatile bit output_p0_6=1;
volatile bit output_p0_7=1;	
// ----------------------------------------------------------------------------------------------------------------

volatile bit timeout; // sinaliza o fim de 10 ms
volatile bit update =0; // var para dar update ao PDisp
// ----------------------------------------------------------------------------------------------------------------

int vtime; // contador virtual 
int carry; // funciona como o carry do psw -> usado para fazer verificações no contador virtual
// ----------------------------------------------------------------------------------------------------------------

void teste_ov(var)
{
	if (!var)
		carry=1;
	else carry=0;
}


void reload_timer()
{
	vtime = 0xffD8;
	  // 10 ms para verificar teclas   (0,01 / (250 * 1e-6))     
}


// ----------------------------------------------------------------------------------------------------------------

void update_buttons () interrupt 1    // interrupt do timer 0 !!!!! 
{

	vtime +=1;
	teste_ov(vtime);
	timeout = carry;
		
}

 void uart_interrupt() interrupt 16 
{
	bit test_uart = SCON1 & RI1;
	if (test_uart)											// quando recebido um byte via UART1
	{
		c = SBUF1;									// colocar em c o byte lido, armazenado em SBUF1
		SBUF1 = c;								  // devolver ao SBUF1 o caracter -> produz eco
		update = 1;
		SCON1 &= ~((char)test_uart);			// colocar a zero bit 0 de SCON1 -> RI1
	}
}


// ----------------------------------------------------------------------------------------------------------------
void VKeys()
{
	pSet->key_window.integer16 = 0x00ff;				// 00000000 11111111
	pSet->count_ones = 8;
	pLoad->key_window.integer16 = 0x00ff;			// 00000000 11111111
	pLoad->count_ones = 8;
}


void tu_buttons_index(char p_Set, char p_Load)
{
	char result;
	static bit old_P0_7 =1;  /// faz a verificação do botão p0_7 verificando se ele esta primido ou nao -> evita ciclos while
	static bit old_P0_6 =1;  ///  (ver em cima)
	
	//// ------ teste do P0_7 ---------------
	
	pSet->key_window.integer16 <<=1;
	pSet->key_window.b[1] |= p_Set;
	pSet->count_ones += p_Set;
	
	result = 4 - pSet->count_ones;  
 	
	pSet->count_ones -= pSet->key_window.b[0];
	
	pSet->key_window.b[0] =0;
	p_Set = result &(1<<7);
	
	
	output_p0_7 = ~((bit)p_Set);
	
	
	if (output_p0_7 && old_P0_7)
	{
		old_P0_7 = 0;
		update = 1;
		c = 'i';		
	}
	else if (!output_p0_7)
		old_P0_7 = 1;

	// ---------- teste P0_6 ---------------------
	
			
	pLoad->key_window.integer16 <<= 1;			
	pLoad->key_window.b[1] |= p_Load;				
	pLoad->count_ones += p_Load;								
	
	result = 4 - pLoad->count_ones;				
	
	pLoad->count_ones -= pLoad->key_window.b[0];		
	
	pLoad->key_window.b[0] = 0;				
	p_Load = result & (1<<7);									
		
	output_p0_6 = ~((bit)p_Load);		

	if (output_p0_6 && old_P0_6)
	{
		old_P0_6= 0;
		update = 1;
		c = 'd';		
	}
	else if (!output_p0_6)
		old_P0_6 = 1;
	
}


void update_bcd(void)
{
	PDISP = array_digit[index &(DIGIT_MASK -1)];
	
}


// -----------------------------------------------------------------------------------------------------------------

void main(void)
{
	Init_Device();
	
	VKeys();
	
	P0_6 = 1;
	P0_7 = 1;
	P0 = 0xff;
	TR0 =1;   		// timer 0 a contar -> usado nas teclas   
	EIE2 |= 0x02;	// ativa a interrupt da uart1 
	ET0 = 1;  		//ENABLE TIMER 0 INTERRUPT
	EA =1  ; 			//ALL INTERUPTS

	reload_timer();
	update_bcd();
	
	while(1)
	{
		if(timeout){
			EA =0;
			tu_buttons_index(P0_6,P0_7);
			timeout=0;
			reload_timer();
			EA =1;
		}
		if(update)
		{
			switch (c){
			
			case 'i':
			case 'I':
						index ++;
						update = 1;
						break;
			case 'd':
			case 'D':
					index --;
					update =1;
				break;
			case'r':
			case'R':
				index=0;
				update=1;
				break;
			default:
				break;
		}
			update_bcd();
			update=0;
		}	
	}

	
	
}



