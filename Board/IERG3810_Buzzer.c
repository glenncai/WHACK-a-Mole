#include "stm32f10x.h"
#include "IERG3810_Buzzer.h"

// put your procedure and code here
//#define Buzzer_off() GPIOB->BRR = 1 << 8; // PB8
//#define Buzzer_On() GPIOB->BSRR = 1 << 8; // PB8

void IERG3810_Buzzer_Init()
{
/* For BUZZER - PB8 , operation high -> use CRH */
GPIOB->CRH &= 0xFFFFFFF0;
GPIOB->CRH |= 0x00000003; // set max clock rate to 50 MHz, output push-pull mode
GPIOB->BRR = 1 << 8;      // Set to low, which is no sound
}