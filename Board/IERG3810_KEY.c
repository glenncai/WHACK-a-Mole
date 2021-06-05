#include "stm32f10x.h"
#include "IERG3810_KEY.h"

// put your procedure and code here
//#define KEY2_SET() GPIOE->BSRR = 1 << 2;   // PE2
//#define KEY1_SET() GPIOE->BSRR = 1 << 3;   //PE3
//#define KEYUP_RESET() GPIOA->BRR = 1 << 0; // PA0


void IERG3810_KEY_Init()
{
/* For KEY_UP - PA0 */
GPIOA->CRL &= 0xFFFFFFF0;
GPIOA->CRL |= 0x00000008; // set max clock rate to 50 MHz, input push-up and push-down mode
GPIOA->BRR = 1 << 0;      // Set to low, which is not be pressed

/* KEY2-PE2(push-up and push-down mode), KEY1-PE3(push-up and push-down mode), 
AND set them max clock rate  to 50 MHz */
GPIOE->CRL &= 0xFFFF00FF;
GPIOE->CRL |= 0x00008800;
GPIOE->BSRR = 1 << 2; // Set PE2 to high, which is not pressed
GPIOE->BSRR = 1 << 3; // Set PE3 to high, which is not pressed
}
