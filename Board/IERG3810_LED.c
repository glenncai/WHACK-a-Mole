#include "stm32f10x.h"
#include "IERG3810_LED.h"

// put your procedure and code here
//#define LED0_RESET() GPIOB->BRR = 1 << 5; // PB5
//#define LED0_SET() GPIOB->BSRR = 1 << 5;  // PB5
//#define LED1_RESET() GPIOE->BRR = 1 << 5; // PE5
//#define LED1_SET() GPIOE->BSRR = 1 << 5;  // PE5

void IERG3810_LED_Init()
{
/* For DS1 - PE5 */
GPIOE->CRL &= 0xFF0FFFFF;
GPIOE->CRL |= 0x00300000; // push-pull mode
GPIOE->BSRR = 1 << 5;;    // Set PE5 to high, which is not lit

/* For DSO(LED0) - PB5 , operation low -> use CRL */
GPIOB->CRL &= 0xFF0FFFFF;
GPIOB->CRL |= 0x00300000; // set max clock rate to 50 MHz, output push-pull mode
GPIOB->BSRR = 1 << 5;
}