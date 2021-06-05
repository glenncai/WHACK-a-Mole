#include "stm32f10x.h"
#include "IERG3810_Clock.h"

// put your procedure and code here
// RCC = Reset Clock Controller

void IERG3810_clock_tree_init(void)
{
	// PLL is a feedback control system that automatically adjust the phase of a locally generated signal to match the phase of input signal
	u8 PLL = 7; // Phase Locked Loop
	unsigned char temp = 0;
	RCC->CFGR &= 0xF8FF0000; // CFGR = clock configuration register
	RCC->CR &= 0xFEF6FFFF;
	//refer to RM0008 page-119 for more details
	//for the following instructions
	RCC->CR |= 0x00010000; //HSEON=1
	while (!(RCC->CR >> 17))
		;					//Check HSERDY; if the HSC oscillator is stable or not.
	RCC->CFGR = 0x00000400; //PPRE1=100
	RCC->CFGR |= PLL << 18; //PLLMUL=111
	RCC->CFGR |= 1 << 16;	//PLLSRC=1
	FLASH->ACR |= 0x32;		//set FLASH with 2 wait states
	RCC->CR |= 0x01000000;	//PLLON=1
	while (!(RCC->CR >> 25))
		;					 //check PLLRDY
	RCC->CFGR |= 0x00000002; //SW=10
	while (temp != 0x02)
	{ //check SWS = system clock switch --> for counting time
		temp = RCC->CFGR >> 2;
		temp &= 0x03;
	}
}
