#include <REG51F380.H>
#include "Debounce.h"
#include "fifo.h"
#include <stdio.h>
     
extern void Init_Device(void);
extern void com_initialize (void);
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
data unsigned char c;
static int Cicle = 0;
//----------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------
volatile bit output_p0_6=1;
volatile bit output_p0_7=1;	
// ----------------------------------------------------------------------------------------------------------------
volatile bit timeout; // sinaliza o fim de 10 ms
volatile bit update =0; // var para dar update ao PDisp
bit enable =0;
// ----------------------------------------------------------------------------------------------------------------
int vtime; // contador virtual 
bit carry; // funciona como o carry do psw -> usado para fazer verificações no contador virtual
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


 void uart_isr() interrupt 4
{
	char c;
	
	if(RI0){	
		inbuff_push(SBUF0);
		RI0 = 0;
	}else
	
	if((c = outbuff_pop()) !=  1){
		TI0 = 0;
		SBUF0 = c;
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
	
	result = 8 - pSet->count_ones;  
 	
	pSet->count_ones -= pSet->key_window.b[0];
	
	pSet->key_window.b[0] =0;
	p_Set = result &(1<<7);
	
	
	output_p0_7 = ~((bit)p_Set);
	
	
	if (output_p0_7 && old_P0_7)
	{
		if(enable){
			c = 'd';
			update = 1;
		}
		old_P0_7 = 0;
		update = 1;
//		index++;	
	}
	else if (!output_p0_7)
		old_P0_7 = 1;

	// ---------- teste P0_6 ---------------------
	
			
	pLoad->key_window.integer16 <<= 1;			
	pLoad->key_window.b[1] |= p_Load;				
	pLoad->count_ones += p_Load;								
	
	result = 8 - pLoad->count_ones;				
	
	pLoad->count_ones -= pLoad->key_window.b[0];		
	
	pLoad->key_window.b[0] = 0;				
	p_Load = result & (1<<7);									
		
	output_p0_6 = ~((bit)p_Load);		

	if (output_p0_6 && old_P0_6)
	{
		if(enable){
			c = 'i';
			update = 1;
		}
	
		old_P0_6= 0;
		update = 1;
//		index--;
	}
	else if (!output_p0_6)
		old_P0_6 = 1;
	
}

// -----------------------------------------------------------------------------------------------------------------
void rec_char()
{
	if((c = inbuff_pop()) != 1){
		switch(c){
			case '1':
				enable =1;
				break;
			case '0':
				enable =0;
				break; 
			case 'i':
			case 'I':
				if(enable)
					Cicle += 5;

				update = 1;
				break;
			case 'd':
			case 'D':
				if(enable)
					Cicle -= 5;
				
				update= 1;
				break;
			case 'r':
				if(enable)
					Cicle = 0;
				
				update= 1;
				break;
			default:
				break;
		}
	}
}
	
// -----------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------

void main(void)
{
	
	Init_Device();
	VKeys();
	com_initialize();	
	reload_timer(); 
	P0_6 = 1;
	P0_7 = 1;
	TR0 =1;   		// timer 0 a contar -> usado nas teclas   
	ET0 = 1;  		//ENABLE TIMER 0 INTERRUPT
	TI0 = 1;
	EA =1  ; 			//ALL INTERUPTS

	while(1)
	{
		if(timeout)
		{
			tu_buttons_index(P0_6, P0_7);
			timeout =0;
			update=1;
		}
		
		rec_char();
		
		if(update)
		{
			//update_bcd();
			if(Cicle > 100){
				Cicle = 100;}
			
			if(Cicle < 0){
				Cicle = 0;}
			
			PCA0CPH0 = Cicle * 2.55;		
			EA = 0;
			printf("%d%%\r",Cicle);
			EA = 1;
			TI0 = 1;
				
			update=0;
		}	
		
		
		
	}
		
}
	

