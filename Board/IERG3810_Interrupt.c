#include "stm32f10x.h"
#include "IERG3810_Interrupt.h"
#include "IERG3810_Delay.h"


void IERG3810_key2_ExtiInit(void)
{
		// KEY2 at PE2, EXTI-2, IRQ#8
    RCC->APB2ENR |= 1 << 6; //port E 2
    GPIOE->CRL &= 0xFFFFF0FF;
    GPIOE->CRL |= 0x00000800;
    GPIOE->ODR |= 1 << 2;
    RCC->APB2ENR |= 0x01;
    AFIO->EXTICR[0] &= 0xFFFFF0FF;
    AFIO->EXTICR[0] |= 0x00000400; //set pinE as input source
    EXTI->IMR |= 1 << 2;           //event masked on line x
    EXTI->FTSR |= 1 << 2;          //falling trigger configuration

    NVIC->IP[8] = 0x65; // set priority of this interrupt. (D10337E page 8-4, 8-16)
    NVIC->ISER[0] |= (1 << 8); // (D10337E page 8-3)
}

void IERG3810_keyUp_ExtiInit (void)
{
	RCC->APB2ENR |= 1 << 2; // enable portA KEYUP
	GPIOA->CRL&=0xFFFFFFF0; // PA0
  GPIOA->CRL|=0x00000008; // PA0
	GPIOA->ODR |= 0 << 0; // data register
	RCC->APB2ENR |= 0x01; // AFIO clock enable
	AFIO->EXTICR[0]&=0xFFFFFFF0;
  AFIO->EXTICR[0]|=0x00000000; // AFIO PA[X] Pin
	EXTI->IMR |= 1 << 0;          
  EXTI->FTSR |= 1 << 0;     
	
	NVIC->IP[6]=0x75; // IRQ6
  NVIC->ISER[0]|=(1 << 6);
}


void IERG3810_NVIC_SetPriorityGroup(u8 prigroup)
{
	// Set PRIGROUP AIRCR[10:8]
    u32 temp, temp1;
    temp1 = prigroup & 0x00000007; // only concern 3 bits
    temp1 <<= 8;
    temp = SCB->AIRCR;
    temp &= 0x0000F8FF;
    temp |= 0x05FA0000;
    temp |= temp1;
    SCB->AIRCR = temp;
}

// EXTI2 handler for KEY2
void EXTI2_IRQHandler(void)
{
    u8 i;
    for (i = 0; i < 10; i++)
    {
        GPIOB->BRR = 1 << 5; // DS0 ON
        Delay(1000000);
        GPIOB->BSRR = 1 << 5; // DS0 OFF
        Delay(1000000);
    }
    EXTI->PR = 1 << 2; //Clear this exception pending bit
}

// EXTI0 handler for KEY UP
void EXTI0_IRQHandler(void)
{
	u8 i;
  for (i = 0; i < 10; i++)
  {
       GPIOE->BRR = 1 << 5; // DS0 ON
       Delay(1000000);
       GPIOE->BSRR = 1 << 5; // DS0 OFF
       Delay(1000000);
  }
  EXTI->PR = 1 << 0; //Clear this exception pending bit
}


void IERG3810_PS2key_ExtiInit (void)
{
	// enable PC10,11
	RCC->APB2ENR|=1<<4;
	GPIOC->CRH &= 0xFFFF00FF;
	GPIOC->CRH |= 0x00008800;
	GPIOC->ODR |= 1<<11; // output data register (CLK is the interrupt)
	RCC->APB2ENR |= 0x01; // AFIO clock enable
	AFIO->EXTICR[2] &= 0xFFFF0FFF; // EXTI 11
	AFIO->EXTICR[2] |= 0x00002000; // AFIO PC[x] pin
	EXTI->IMR |= 1 << 11;
	EXTI->FTSR|= 1 << 11;
	
	NVIC->IP[40]=0x75; // IRQ40
  NVIC->ISER[1]|= (1 << 8); // 40 -> iser[1] since max is 31 -> iser[0]
}

void IERG3810_TIM3_Init(u16 arr, u16 psc){
	//TIM3, IRG#29
	RCC->APB1ENR |= 1<<1; //RM0008 P.111 enable TIM3
	TIM3->ARR=arr; //p.403 TIMx auto-reload register; ARR is the prescaler value to be loaded in the actual auto-reload register
	TIM3->PSC=psc; //p.402-403 TIMx prescaler (TIMx_PSC)
	TIM3->DIER |= 1<<0; //p.393 Bit 0 UIE: Update interrupt enable; 0//1: Update interrupt disabled//enabled
	TIM3->CR1 |= 0x01; //p.388 Buffered registers are then loaded with their preload values.
/* 1: UEV disabled. The Update event is not generated, shadow registers keep their value
(ARR, PSC, CCRx). However the counter and the prescaler are reinitialized if the UG bit is
set or if a hardware reset is received from the slave mode controller. */
	
	NVIC->IP[29] = 0x45; // lab4 IRQ29
	NVIC->ISER[0] |= (1<<29); // lab4 29
}

void IERG3810_TIM4_Init(u16 arr,u16 psc){
	RCC->APB1ENR |= 1<<2; //enable TIM4
	TIM4->ARR = arr;
	TIM4->PSC = psc;
	TIM4->DIER = 1<<0; // TIMx DMA/Interrupt enable register
	TIM4->CR1 |= 0x01; // TIMx control register 1
	
	NVIC->IP[30] = 0x45; //lab4 IRQ30
	NVIC->ISER[0] |= (1<<30); //lab4 30
	
}


void IERG3810_SYSTICK_Init10ms(void){
	//SYSTICK
	SysTick->CTRL = 0; //Clear Systick->CTRL setting
	SysTick->LOAD = 89999; //what should be filled? refer to 0337E page 8-10
	                       //72M / (8*100) - 1  = 72000000 / 800 -1 = 90000 - 1
	                       //10ms => 1/100 second per heartbeat => 100 Hz
	
	//CLKSOURCE=0 : STCLK (FCLK/8)
	//CLKSOURCE = 1: FCLK
	//CLKSOURCE=0 is synchronized and better than CLKSROUCE = 1
	//Refer to Clock tree on page-93 of RM0008
	SysTick->CTRL |= 0x03; //what should be filled?
	                      //refer to 033tE page 8-8
	// b00011 = 0x03 = 97 ... 128
}

void IERG3810_TIM3_PwmInit(u16 arr, u16 psc){
	RCC->APB2ENR |= 1<<3;  // Port D
	GPIOB->CRL &= 0xFF0FFFFF; // D5
	GPIOB->CRL |= 0x00B00000; // push-pull 50Mhz
	RCC->APB2ENR |= 1<<0 ; // Port A
	AFIO->MAPR &= 0xFFFFF3FF; // RM0008 page -179
	AFIO->MAPR |= 1<<11; //RM0008 page -179
	RCC->APB1ENR |= 1<<1; //RM0008 page 112
	TIM3->ARR = arr; //RM0008 page 403
	TIM3->PSC = psc; //RM0008 page 402
	TIM3->CCMR1 |= 7 << 12; // page 397
	TIM3->CCMR1 |= 1<<11; //page 397
	TIM3->CCER |= 1<<4; //page 401
	TIM3->CR1 = 0x0080; //RM0008 page 388
	TIM3->CR1 |= 0x01; //RM0008 page 388
}



