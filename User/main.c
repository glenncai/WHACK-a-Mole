#include "stm32f10x.h"  
#include "IERG3810_LED.h"
#include "IERG3810_KEY.h"
#include "IERG3810_Buzzer.h"
#include "IERG3810_Clock.h"
#include "IERG3810_USART.h"
#include "IERG3810_TFTLCD.h"
#include "IERG3810_Delay.h"
#include "IERG3810_Interrupt.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// --- Global variables
u32 sheep = 0;
u32 timeout = 20000; // for PS2 keyboard timeout
u32 ps2key = 0; // store PS2 Keyboard data
u32 ps2count = 0; // count PS2 keyboard received bit
u8 ps2dataReady = 0; // 1= PS2 keyboard received data correctly
u8 task1HeartBeat = 0; // DS0
u8 task2HeartBeat = 0; // DS1
u16 caveColor = 0xfff0;
u16 hitColor = 0xff00;
int box;	
int timeCount=0;

u16 i=0, j=0, k=0;
int p, q, r; // for randomization
u32 sec = 0;
u8 flag=0;
int moleflag = 0;  // show mouse position
int hitflag = 0;   // show hit position
int rnd;


		
u8 GAME[16] = {0x57, 0x48, 0x41, 0x43, 0x4B, 0x2D, 0x61, 0x2D, 0x4D, 0x6F, 0x6C, 0x65, 0x21, 0x21, 0x21}; // WHACK-a-Mole!!!
u8 BY[12] = {0x42, 0x59, 0x3A, 0x20, 0X47, 0X72, 0x6F, 0x75, 0x70, 0x42, 0x32}; // print "BY: GroupB2"
u8 sid1[12] = {0x31, 0x31, 0x35, 0x35, 0x31, 0x32, 0x35, 0x39, 0x32, 0x33, 0x20}; // sid of the lower line
u8 sid0[12] = {0x31, 0x31, 0x35, 0x35, 0x31, 0x32, 0x36, 0x38, 0x37, 0x35, 0x20}; // sid of the upper line
u8 PRESS0[12] ={0x20, 0X50, 0X52, 0X45, 0X53, 0X53, 0X20, 0x2B, 0x20, 0x74, 0X6F, 0X20}; // print "PRESS O to "
u8 START[12] = {0x20, 0x53, 0x54, 0x41, 0x52, 0x54, 0x20, 0x47, 0x41, 0x4D, 0x45, 0x20}; // print "START GAME"
u8 name[12] = {0x4E, 0x61, 0x6D, 0x65, 0x3A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}; // Name:
u8 TLW[12] = {0x43, 0x61, 0x69, 0x20, 0x4C, 0x6F, 0x6E, 0x67, 0x68, 0x75, 0x61}; // Cai Longhua
u8 NKM[12] = {0x4E, 0x67, 0x20, 0x4B, 0x61, 0x6D, 0x20,0x4D, 0x69, 0x6E, 0x67}; // Ng Kam Ming
u8 Instn[12] = {0x49, 0x4E, 0x53, 0x54, 0x52, 0x55, 0x43, 0x54, 0x49, 0x4F, 0x4E, 0x53}; // Instructioins
u8 score[10] = {0x53, 0x63, 0x6F, 0x72, 0x65, 0x3A}; // show the "Score" string
u8 mark[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39}; // show the player mark 
u8 timeshow[6] = {0x54, 0x69, 0x6D, 0x65, 0x3A, 0x20};
u8 GameOver[10] ={0x47, 0x41, 0x4D, 0x45, 0x20, 0x4F, 0x56, 0x45, 0x52, 0x21};
u8 marks[3] = {0x30, 0x30, 0x30};


u8 rules[12][30]={
		{0x2a, 0x20, 0x59, 0x6f, 0x75, 0x20, 0x77, 0x69, 0x6c, 0x6c, 0x20, 0x73, 0x65, 0x65, 0x20, 0x61, 0x20, 0x33, 0x20, 0x62, 0x79, 0x20, 0x33, 0x20, 0x67, 0x72, 0x69, 0x64, 0x20, },
		{0x2a, 0x20, 0x48, 0x69, 0x74, 0x20, 0x61, 0x73, 0x20, 0x6d, 0x61, 0x6e, 0x79, 0x20, 0x6d, 0x6f, 0x6c, 0x65, 0x73, 0x20, 0x61, 0x73, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x63, 0x61, 0x6e},
		{0x2a, 0x20, 0x41, 0x64, 0x64, 0x20, 0x31, 0x20, 0x6d, 0x61, 0x72, 0x6b, 0x20, 0x69, 0x66, 0x20, 0x61, 0x20, 0x6d, 0x6f, 0x6c, 0x65, 0x20, 0x69, 0x73, 0x20, 0x68, 0x69, 0x74, },
		{0x2a, 0x20, 0x4e, 0x6f, 0x20, 0x6d, 0x61, 0x72, 0x6b, 0x20, 0x69, 0x66, 0x20, 0x61, 0x20, 0x6d, 0x6f, 0x6c, 0x65, 0x20, 0x69, 0x73, 0x20, 0x6d, 0x69, 0x73, 0x73, 0x65, 0x64, },
		{0x2a, 0x20, 0x4d, 0x69, 0x6e, 0x75, 0x73, 0x20, 0x31, 0x20, 0x6d, 0x61, 0x72, 0x6b, 0x20, 0x69, 0x66, 0x20, 0x6e, 0x6f, 0x74, 0x68, 0x69, 0x6e, 0x67, 0x20, 0x68, 0x69, 0x74},
		{0x2a, 0x20, 0x45, 0x61, 0x63, 0x68, 0x20, 0x72, 0x6f, 0x75, 0x6e, 0x64, 0x20, 0x6c, 0x61, 0x73, 0x74, 0x73, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x31, 0x20, 0x6d, 0x69, 0x6e, 0x2e, },
		{0x2a, 0x20, 0x45, 0x61, 0x63, 0x68, 0x20, 0x6d, 0x6f, 0x6c, 0x65, 0x20, 0x73, 0x68, 0x6f, 0x77, 0x73, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x31, 0x20, 0x73, 0x65, 0x63, 0x2e, 0x20, },
		{0x2a, 0x20, 0x4d, 0x6f, 0x6c, 0x65, 0x73, 0x20, 0x61, 0x72, 0x65, 0x20, 0x72, 0x61, 0x6e, 0x64, 0x6f, 0x6d, 0x6c, 0x79, 0x20, 0x61, 0x70, 0x70, 0x65, 0x61, 0x72, 0x65, 0x64, },
		{0x2a, 0x20, 0x4f, 0x6e, 0x6c, 0x79, 0x20, 0x31, 0x20, 0x6d, 0x6f, 0x6c, 0x65, 0x20, 0x70, 0x6f, 0x70, 0x73, 0x20, 0x75, 0x70, 0x20, 0x70, 0x65, 0x72, 0x20, 0x73, 0x65, 0x63, 0x2e, },
		{0x2a, 0x20, 0x4d, 0x61, 0x78, 0x2e, 0x73, 0x63, 0x6f, 0x72, 0x65, 0x3a, 0x20, 0x36, 0x30, 0x20, 0x20, 0x4d, 0x69, 0x6e, 0x2e, 0x73, 0x63, 0x6f, 0x72, 0x65, 0x3a, 0x20, 0x30, },
		{0x2d, 0x2d, 0x3e, 0x20, 0X50, 0X72, 0X65, 0X73, 0X73, 0X20, 0X31, 0X2D, 0X39, 0X20, 0X74, 0X6F, 0X20, 0X68, 0X69, 0X74, 0X20, 0X74, 0X68, 0X65, 0X20, 0X6D, 0X6F, 0X6C, 0X65, 0X73}
};

int timecounter = 0; // for counting time
u8 tensec[1] = {0x36}; // recording tens digit for each 10 second (default: 0x36)
int molecounter = 0; // for moles display
int scoreGet = 0; // to show total score

// --- end of Global variables



void StartPage();
void MainPage();


// report remaining time here
void TIM3_IRQHandler(void){ 	

	if(TIM3->SR & 1<<0) //RM0008 P.395 When update interrupt pending occur
{ 
	timecounter ++; // timecounter +=1 for each 0.5 second
			if(timecounter==0)
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20,0x30, 0xffff, 0x0000); // one second
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20,0x36, 0xffff, 0x0000); // ten seconds
			

				// 10-second change
				if(timecounter%20==1){
					tensec[0] -=1;
				}
				// down-counting from 9 to 0
				if(timecounter%20==1||timecounter%20==2) {
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20,0x39, 0xffff, 0x0000);
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20, tensec[0], 0xffff, 0x0000);
				}
				if(timecounter%20==3||timecounter%20==4){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20,0x38, 0xffff, 0x0000);
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20, tensec[0], 0xffff, 0x0000);
				}
				if(timecounter%20==5||timecounter%20==6){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20,0x37, 0xffff, 0x0000); 
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20, tensec[0], 0xffff, 0x0000);
				}
				if(timecounter%20==7||timecounter%20==8){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20,0x36, 0xffff, 0x0000);
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20, tensec[0], 0xffff, 0x0000);	
				}
				if(timecounter%20==9||timecounter%20==10){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20,0x35, 0xffff, 0x0000); 
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20, tensec[0], 0xffff, 0x0000);
				}
				if(timecounter%20==11||timecounter%20==12){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20,0x34, 0xffff, 0x0000); 
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20, tensec[0], 0xffff, 0x0000);
				}
				if(timecounter%20==13||timecounter%20==14){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20,0x33, 0xffff, 0x0000); 
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20, tensec[0], 0xffff, 0x0000);
				}
				if(timecounter%20==15||timecounter%20==16){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20,0x32, 0xffff, 0x0000); 
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20, tensec[0], 0xffff, 0x0000);
				}
				if(timecounter%20==17||timecounter%20==18){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20,0x31, 0xffff, 0x0000);
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20, tensec[0], 0xffff, 0x0000);
				}
				if(timecounter%20==19||timecounter%20==0){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20,0x30, 0xffff, 0x0000);
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20, tensec[0], 0xffff, 0x0000);
				}
				
			if(timecounter>120){ // end of a game round
				
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20,0x30, 0xffff, 0x0000); // one second
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20,0x30, 0xffff, 0x0000); // ten seconds
				for (i=0; i<0x000A; i+=0x01) {
					IERG3810_TFTLCD_ShowChar(0x01A+i*0x0008,0x0F00,GameOver[i], 0xfff0, 0xf000); // show "Game Over" sign
					
						// Buzzer sound
						GPIOB->BSRR = 1 << 8;
					}
				tensec[0] = 0x36;
			}
} 
	TIM3->SR &= ~(1<<0); //RM0008 P.395  clear the interrupt pending bit
}

// pop out rats randomly here
void TIM4_IRQHandler(void){  
	
	if(TIM4->SR & 1<<0)//RM0008 P.395 When update interrupt pending occur
	{
				molecounter++;
		
				
				if(rnd == 0 && molecounter <= 60) {
					IERG3810_TFTLCD_ShowChinChar(0x20,0x20,0x06,0x00ff, 0xfff0);
					moleflag=1;
				} else {
					IERG3810_TFTLCD_FillRectangle(caveColor, 0xA, 0x37, 0xA, 0x37);
				}

				if (rnd == 1 && molecounter <= 60) {
					IERG3810_TFTLCD_ShowChinChar(0x76,0x20,0x06,0x00ff, 0xfff0);
					moleflag=2;
				} else {
					IERG3810_TFTLCD_FillRectangle(caveColor, 0x60, 0x37, 0xA, 0x37);
				}
				
				if (rnd == 2 && molecounter <= 60) {
					IERG3810_TFTLCD_ShowChinChar(0xCA,0x20,0x06,0x00ff, 0xfff0);
					moleflag=3;
				} else {
					IERG3810_TFTLCD_FillRectangle(caveColor, 0xB4, 0x37, 0xA, 0x37);
				}
				
				if (rnd == 3 && molecounter <= 60) {
					IERG3810_TFTLCD_ShowChinChar(0x20,0x76,0x06,0x00ff, 0xfff0);
					moleflag=4;
				} else {
					IERG3810_TFTLCD_FillRectangle(caveColor, 0xA, 0x37, 0x60, 0x37);
				}
				
				if (rnd == 4 && molecounter <= 60) {
					IERG3810_TFTLCD_ShowChinChar(0x76,0x76,0x06,0x00ff, 0xfff0);
					moleflag=5;
				} else {
					IERG3810_TFTLCD_FillRectangle(caveColor, 0x60, 0x37, 0x60, 0x37);
				}
				
				if (rnd == 5 && molecounter <= 60) {
					IERG3810_TFTLCD_ShowChinChar(0xCA,0x76,0x06,0x00ff, 0xfff0);
					moleflag=6;
				} else {
					IERG3810_TFTLCD_FillRectangle(caveColor, 0xB4, 0x37, 0x60, 0x37);
				}
				
				if (rnd == 6 && molecounter <= 60) {
					IERG3810_TFTLCD_ShowChinChar(0x20,0xCC,0x06,0x00ff, 0xfff0);
					moleflag=7;
				} else {
					IERG3810_TFTLCD_FillRectangle(caveColor, 0xA, 0x37, 0xB6, 0x37);
				}
				
				if (rnd == 7 && molecounter <= 60) {
					IERG3810_TFTLCD_ShowChinChar(0x76,0xCC,0x06,0x00ff, 0xfff0);
					moleflag=8;
				} else {
					IERG3810_TFTLCD_FillRectangle(caveColor, 0x60, 0x37, 0xB6, 0x37);
				}
				
				if (rnd == 8 && molecounter <= 60) {
						IERG3810_TFTLCD_ShowChinChar(0xCA,0xCC,0x06,0x00ff, 0xfff0);
						moleflag=9;
				} else {
					IERG3810_TFTLCD_FillRectangle(caveColor, 0xB4, 0x37, 0xB6, 0x37);
				}
				
	}
	TIM4->SR &= ~(1<<0);//RM0008 P.395 clear the interrupt pending bit
}
	

// EXIT-11 handler for PS2keyboard CLK
void EXTI15_10_IRQHandler(void) {
	 //Delay(10); // We found that the processor is too fast and get error
	// A short delay can eliminate the error
	if (ps2count >= 1 && ps2count <= 8)
	{
		u32 flag = GPIOC->IDR &= 0x00000400;//pc10
		if (flag == 0)
		{
			box |= 0<<(ps2count - 1) ; //00000000 (box = 0)
		}
		else
		{
			box |= 1<<(ps2count - 1);//000000001 if 1<<1 = 00000011
		}
		Delay(10);
	}
	
	ps2count++;
	
	if (ps2count >= 11)
	{
		ps2count = 0;
		ps2dataReady = 1;
	}
	
	EXTI->PR = 1 << 11; // Clear his exception pending bit
}

void Background(){
	Delay(1000000);
	IERG3810_TFTLCD_FillRectangle(0x0000,0,240,0,320); // Rectangle cover all LCD: black background
}

void PrintScore(int Getmark){
			if(Getmark<10){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x30, 0xffff, 0x0000);
			}
			else if(Getmark<0){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x30, 0xffff, 0x0000);
			}
			else if(Getmark>=10 && Getmark<20){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x31, 0xffff, 0x0000);
			}
			else if(Getmark>=20 && Getmark<30){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x32, 0xffff, 0x0000);
			}
			else if(Getmark>=30 && Getmark<40){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x33, 0xffff, 0x0000);
			}
			else if(Getmark>=40 && Getmark<50){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x34, 0xffff, 0x0000);
			}
			else if(Getmark>=50 && Getmark<60){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x35, 0xffff, 0x0000);
			}
			else if(Getmark>=60 && Getmark<70){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x36, 0xffff, 0x0000);
			}
			else if(Getmark>=70 && Getmark<80){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x37, 0xffff, 0x0000);
			}
			else if(Getmark>=80 && Getmark<90){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x38, 0xffff, 0x0000);
			}
			else if(Getmark>=90 && Getmark<100){
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x39, 0xffff, 0x0000);
			}
			else if(Getmark>=100 && Getmark<110){
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F00, 0x31, 0xffff, 0x0000);
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x30, 0xffff, 0x0000);
			}
			else if(Getmark>=110 && Getmark<120){
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F00, 0x31, 0xffff, 0x0000);
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x31, 0xffff, 0x0000);
			}
			else if(Getmark>=120){
				IERG3810_TFTLCD_ShowChar(0x0D0,0x0F00, 0x31, 0xffff, 0x0000);
				IERG3810_TFTLCD_ShowChar(0x0D8,0x0F00, 0x32, 0xffff, 0x0000);
			}
			
			for(i=0; i<10; i++){
				if(Getmark%10==i){
					IERG3810_TFTLCD_ShowChar(0x0E0,0x0F00, 0x30+i*0x01, 0xffff, 0x0000);
				}
			}
}

void StartPage(void){
	
	timecounter = 0; // reset remaining time here
	
		for(j=0; j<0x0010; j+=0x01){
			IERG3810_TFTLCD_ShowChar(0x0010+j*0x0008,0x128,GAME[j], 0xffff, 0x00ff); // GAME TITLE
		}
		
		for(i=0; i<0x000C; i+=0x01){
		IERG3810_TFTLCD_ShowChar(0x0090+i*0x0008,0x0100,sid1[i], 0xfff0, 0x0000); // sid1
		IERG3810_TFTLCD_ShowChar(0x0090+i*0x0008,0x0110,sid0[i], 0xfff0, 0x0000); // sid0
		IERG3810_TFTLCD_ShowChar(0x0090+i*0x0008,0x0120,BY[i], 0xffff, 0x0000); // By:
		IERG3810_TFTLCD_ShowChar(0x0010+i*0x0008,0x0110,PRESS0[i], 0xffff, 0xf000); // press 0 to
		IERG3810_TFTLCD_ShowChar(0x0010+i*0x0008,0x0100,START[i], 0xffff, 0xf000); // start
		IERG3810_TFTLCD_ShowChar(0x0010+i*0x0008,0x0F0,name[i], 0xffff, 0x0000); // Name:
		IERG3810_TFTLCD_ShowChar(0x0010+i*0x0008,0x0E0,TLW[i], 0xfff0, 0x0000); // Cai 
		IERG3810_TFTLCD_ShowChar(0x0010+i*0x0008,0x0D0,NKM[i], 0xfff0, 0x0000); // Ng
		IERG3810_TFTLCD_ShowChar(0x0008+i*0x0008,0x0B0,Instn[i], 0xffff, 0x0f0f); // Instructions
		Delay(15000);  // keep the current state stable by delaying
		}
		
	  // Instructions Part
		for(j=0; j<0xE0; j++){
		IERG3810_TFTLCD_DrawDot(0x08+j, 0xC8, 0xff00); // horizontal line
		}
		
		for(j=0; j<12; j++){
			for(k=0; k<30; k++){
				IERG3810_TFTLCD_ShowChar(0x0+k*0x8,0xA0-j*0x10,rules[j][k], 0x0fff, 0x0000); // show game rules
			}
		}
	
		// Show the chinese name characters
		IERG3810_TFTLCD_ShowChinChar(0x90,0x00E0,0x00,0xf0f0, 0x0000); //upper
		IERG3810_TFTLCD_ShowChinChar(0xA0,0x00E0,0x01,0xf0f0, 0x0000);
		IERG3810_TFTLCD_ShowChinChar(0xB0,0x00E0,0x02,0xf0f0, 0x0000);
		IERG3810_TFTLCD_ShowChinChar(0x90,0x00D0,0x03,0xf0f0, 0x0000); //lower
		IERG3810_TFTLCD_ShowChinChar(0xA0,0x00D0,0x04,0xf0f0, 0x0000);
		IERG3810_TFTLCD_ShowChinChar(0xB0,0x00D0,0x05,0xf0f0, 0x0000);
		Delay(100000);
	
}


void MainPage(void){
	/* Show the string "Score" and "Time" */

	
		for (i=0; i<0x0006; i+=0x01) {
			IERG3810_TFTLCD_ShowChar(0x00A0+i*0x0008,0x0F00,score[i], 0xff00, 0x0000);
			IERG3810_TFTLCD_ShowChar(0x00A0+i*0x0008,0x0F20,timeshow[i], 0xff00, 0x0000);
		}
		Delay(10000);

		/* Show the cave */
		
		IERG3810_TFTLCD_FillRectangle(caveColor, 0xA, 0x37, 0xA, 0x37); // bottom left 1
		IERG3810_TFTLCD_FillRectangle(caveColor, 0x60, 0x37, 0xA, 0x37); // bottom center 2
		IERG3810_TFTLCD_FillRectangle(caveColor, 0xB4, 0x37, 0xA, 0x37); // bottom right 3
		IERG3810_TFTLCD_FillRectangle(caveColor, 0xA, 0x37, 0x60, 0x37); // center left 4
		IERG3810_TFTLCD_FillRectangle(caveColor, 0x60, 0x37, 0x60, 0x37); // center center 5
		IERG3810_TFTLCD_FillRectangle(caveColor, 0xB4, 0x37, 0x60, 0x37); // center right 6
		IERG3810_TFTLCD_FillRectangle(caveColor, 0xA, 0x37, 0xB6, 0x37); // top left 7
		IERG3810_TFTLCD_FillRectangle(caveColor, 0x60, 0x37, 0xB6, 0x37); // top center 8
		IERG3810_TFTLCD_FillRectangle(caveColor, 0xB4, 0x37, 0xB6, 0x37); // top center 9
	
		/* Show mouse */
		Delay(10000);
		
		
		IERG3810_TFTLCD_ShowChinChar(0xA,0x118,0x06,0x00ff, 0x0000);

			
			/* Show score here */
			for(i=0;i<0x03; i++){
				IERG3810_TFTLCD_ShowChar(0x0D0+i*8,0x0F00, 0x30, 0xffff, 0x0000); // 00 mark
			}
			
			// delay the first second at the beginning here
			IERG3810_TFTLCD_ShowChar(0x0D8,0x0F20, 0x30, 0xffff, 0x0000); // 0 second
			IERG3810_TFTLCD_ShowChar(0x0D0,0x0F20, 0x36, 0xffff, 0x0000); // 60 seconds
			Delay(10000000);
			
			
			// IERG3810_SYSTICK_Init10ms();
			IERG3810_TIM3_Init(4999, 7199); // set timer-3 (1 Hz) [for time-counting]
			IERG3810_TIM4_Init(9999, 7199); // set timer-4 (0.5Hz) [for mole-display]
			
}



int main(void)
{
	u16 led0pwmval=0;
	u16 led1pwmval=0;
	u8 dir=1;
	int restartFlag = 0;
	int n = 0;
	
	IERG3810_TFTLCD_Init();
	IERG3810_clock_tree_init ();
	IERG3810_LED_Init ();
	IERG3810_Buzzer_Init();
	IERG3810_NVIC_SetPriorityGroup (5); // set PRIGROUP
	IERG3810_key2_ExtiInit (); // Init KEY2 as an interrupt
	IERG3810_keyUp_ExtiInit (); // Init KEYUP as an interrupt input
	IERG3810_PS2key_ExtiInit (); // Init PS2 keyboard as an interrupt input
	EXTI15_10_IRQHandler ();

	Background();
	StartPage();

	GPIOB->BSRR |= 1<<5;
	GPIOE->BSRR |= 1<<5;


	while(1)
	{		
		// sheep++; // count sheep, same as do nothing
		// Start Game Main Page
		rnd = rand() % 9;
	
	if(box==0x79&&ps2dataReady==1){ // Key value + plus sign to start game
		Background();
		CountDown();
		Background();
		MainPage();
		ps2dataReady = 0;
	}

		
	if(box==0x69&&ps2dataReady==1){ // Key value 1
		ps2dataReady = 0;
		hitflag=1;
		if(hitflag==moleflag){
			moleflag=-1;
			scoreGet++;
			IERG3810_TFTLCD_ShowChinChar(0x20,0x20,0x06,0xf000, 0xfff0);
			PrintScore(scoreGet);	
		}
		else if(hitflag!=moleflag && hitflag>0 && moleflag!=-1 && moleflag>0){
			moleflag=-2;
			if(moleflag==-2){
			scoreGet--;
			PrintScore(scoreGet);
			}
		}
	}
	else if(box==0xF0&&ps2dataReady==0) {
		IERG3810_TFTLCD_FillRectangle(caveColor, 0xA, 0x37, 0xA, 0x37);
		ps2dataReady = 0;
	}

	if(box==0x72&&ps2dataReady==1){ // Key value 2
		ps2dataReady = 0;
		hitflag=2;
		if(hitflag==moleflag){
			moleflag=-1;
			scoreGet++;
			IERG3810_TFTLCD_ShowChinChar(0x76,0x20,0x06,0xf000, 0xfff0);
			PrintScore(scoreGet);
		}
		else if(hitflag!=moleflag && hitflag>0 && moleflag!=-1 && moleflag>0){
			moleflag=-2;
			if(moleflag==-2){
			scoreGet--;
			PrintScore(scoreGet);
			}
		}
	}
	else if(box==0xF0&&ps2dataReady==0) {
		IERG3810_TFTLCD_FillRectangle(caveColor, 0x60, 0x37, 0xA, 0x37);
		ps2dataReady = 0;
		hitflag=0;
	}
	
	if(box==0x7A&&ps2dataReady==1){ // Key value 3
		ps2dataReady = 0;
		hitflag=3;
		if(hitflag==moleflag){
			moleflag=-1;
			scoreGet++;
			IERG3810_TFTLCD_ShowChinChar(0xCA,0x20,0x06,0xf000, 0xfff0);
			PrintScore(scoreGet);
		}
		else if(hitflag!=moleflag && hitflag>0 && moleflag!=-1 && moleflag>0){
			moleflag=-2;
			if(moleflag==-2){
			scoreGet--;
			PrintScore(scoreGet);
			}
		}
	}
	else if(box==0xF0&&ps2dataReady==0) {
		IERG3810_TFTLCD_FillRectangle(caveColor, 0xB4, 0x37, 0xA, 0x37);
		ps2dataReady = 0;
		hitflag=0;
		
	}
	if(box==0x6B&&ps2dataReady==1){ // Key value 4
		ps2dataReady = 0;
		hitflag=4;
		if(hitflag==moleflag){
			moleflag=-1;
			scoreGet++;
			IERG3810_TFTLCD_ShowChinChar(0x20,0x76,0x06,0xf000, 0xfff0);
			PrintScore(scoreGet);
		}
		else if(hitflag!=moleflag && hitflag>0 && moleflag!=-1 && moleflag>0){
			moleflag=-2;
			scoreGet--;
			PrintScore(scoreGet);
		}
	}
	else if(box==0xF0&&ps2dataReady==0) {
		IERG3810_TFTLCD_FillRectangle(caveColor, 0xA, 0x37, 0x60, 0x37);
		ps2dataReady = 0;
		hitflag=0;
	}
	if(box==0x73&&ps2dataReady==1){ // Key value 5
		ps2dataReady = 0;
		hitflag=5;
		if(hitflag==moleflag){
			moleflag=-1;
			scoreGet++;
			IERG3810_TFTLCD_ShowChinChar(0x76,0x76,0x06,0xf000, 0xfff0);
			PrintScore(scoreGet);
		}
		else if(hitflag!=moleflag && hitflag>0 && moleflag!=-1 && moleflag>0){
			moleflag=-2;
			scoreGet--;
			PrintScore(scoreGet);
		}
	}
	else if(box==0xF0&&ps2dataReady==0) {
		IERG3810_TFTLCD_FillRectangle(caveColor, 0x60, 0x37, 0x60, 0x37); 
		ps2dataReady = 0;
		hitflag=0;
	}
	if(box==0x74&&ps2dataReady==1){ // Key value 6
		ps2dataReady = 0;
		hitflag=6;
		if(hitflag==moleflag){
			moleflag=-1;
			scoreGet++;
			IERG3810_TFTLCD_ShowChinChar(0xCA,0x76,0x06,0xf000, 0xfff0);
			PrintScore(scoreGet);
		}
		else if(hitflag!=moleflag && hitflag>0 && moleflag!=-1 && moleflag>0){
			moleflag=-2;
			scoreGet--;
			PrintScore(scoreGet);
		}
	}
	else if(box==0xF0&&ps2dataReady==0) {
		IERG3810_TFTLCD_FillRectangle(caveColor, 0xB4, 0x37, 0x60, 0x37);	
		ps2dataReady = 0;
		hitflag=0;
	}
	if(box==0x6C&&ps2dataReady==1){ // Key value 7
		ps2dataReady = 0;
		hitflag=7;
		if(hitflag==moleflag){
			moleflag=-1;
			scoreGet++;
			IERG3810_TFTLCD_ShowChinChar(0x20,0xCC,0x06,0xf000, 0xfff0);
			PrintScore(scoreGet);
		}
		else if(hitflag!=moleflag && hitflag>0 && moleflag!=-1 && moleflag>0){
			moleflag=-2;
			scoreGet--;
			PrintScore(scoreGet);
		}
	}
	else if(box==0xF0&&ps2dataReady==0) {
		IERG3810_TFTLCD_FillRectangle(caveColor, 0xA, 0x37, 0xB6, 0x37);
		ps2dataReady = 0;
		hitflag=0;
	}
	if(box==0x75&&ps2dataReady==1){ // Key value 8
		ps2dataReady = 0;
		hitflag=8;
		if(hitflag==moleflag){
			moleflag=-1;
			scoreGet++;
			IERG3810_TFTLCD_ShowChinChar(0x76,0xCC,0x06,0xf000, 0xfff0);
			PrintScore(scoreGet);
		}
		else if(hitflag!=moleflag && hitflag>0 && moleflag!=-1 && moleflag>0){
			moleflag=-2;
			scoreGet--;
			PrintScore(scoreGet);
		}
	}
	else if(box==0xF0&&ps2dataReady==0) {
		IERG3810_TFTLCD_FillRectangle(caveColor, 0x60, 0x37, 0xB6, 0x37);
		ps2dataReady = 0;
		hitflag=0;
	}
	if(box==0x7D&&ps2dataReady==1){ // Key value 9
		ps2dataReady = 0;
		hitflag=9;
		if(hitflag==moleflag){
			moleflag=-1;
			scoreGet++;
			IERG3810_TFTLCD_ShowChinChar(0xCA,0xCC,0x06,0xf000, 0xfff0);
			PrintScore(scoreGet);
		}
		else if(hitflag!=moleflag && hitflag>0 && moleflag!=-1 && moleflag>0){
			moleflag=-2;
			scoreGet--;
			PrintScore(scoreGet);
		}
	}
	else if(box==0xF0&&ps2dataReady==0) {
		IERG3810_TFTLCD_FillRectangle(caveColor, 0xB4, 0x37, 0xB6, 0x37);
		ps2dataReady = 0;
		hitflag=0;
	}
	
	if(box==0x70&&ps2dataReady==1) {
		tensec[0] = 0x36;
	}
	
	if(moleflag==-1){ // hit the mole
		GPIOE->BRR |= 1<<5;
		GPIOB->BSRR |= 1<<5; 
	}
	else if(hitflag!=moleflag && hitflag>0 && moleflag!=-1){  // not hit the mole
		GPIOB->BRR |= 1<<5; // red led on
	}
	else if(hitflag==0 && moleflag!=-1){   // do nothing
		GPIOE->BSRR |= 1<<5; // green lid off
		GPIOB->BSRR |= 1<<5; // red led off
	}
	
	
	if(scoreGet<0){
		scoreGet=0;
	}
		
		timeout--;
	
		if (timeout == 0)
		{
			timeout = 20000;
			ps2key=0;
			ps2count=0;
			box = 0;
		} // end of "clear PS2 keyboard data when timeout"
		
	}
	
}

