#include <REG51F380.H>


// Peripheral specific initialization functions,
// Called from the Init_Device() function
void Timer_Init()
{
	TCON      = 0x40;
  TMOD      = 0x22;
  CKCON     = 0x0A;
  TH1       = 0x30;
	TH0 			= -250;
	TL0 			= -250;	
}

void UART_Init()
{
    SCON0     = 0x10;
}

void Port_IO_Init()
{
		P0SKIP    = 0xCF;
    P1SKIP    = 0xFF;
    P2SKIP    = 0x03;
    XBR0      = 0x01;
    XBR1      = 0x41;
}

void Oscillator_Init()
{
    FLSCL     = 0x90;
    CLKSEL    = 0x03;
}

void Interrupts_Init()
{
     IE        = 0x92;
}

void PCA_Init()
{
	PCA0MD 		= 0x00;
	PCA0CN    = 0x40;
  PCA0CPM0  = 0x42;
	
	PCA0CPH0 	= 0;
}

// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
		PCA_Init();
    Timer_Init();
    UART_Init();
    Port_IO_Init();
    Oscillator_Init();
    Interrupts_Init();
}
