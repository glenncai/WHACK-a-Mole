#include "stm32f10x.h"
#include "FONT.h"
#include "CFONT.h"
#include "IERG3810_Delay.h"

// put your procedure and code here
typedef struct
{
		u16 LCD_REG;
		u16 LCD_RAM;
	
} LCD_TypeDef;

#define LCD_BASE   ((u32)(0x6C000000 | 0x000007FE))
#define LCD        ((LCD_TypeDef *) LCD_BASE)

void IERG3810_TFTLCD_WrReg(u16 regval)
{
    LCD->LCD_REG = regval;
}

void IERG3810_TFTLCD_WrData(u16 data)
{
    LCD->LCD_RAM = data;
}

void IERG3810_TFTLCD_SetParameter(void){
	IERG3810_TFTLCD_WrReg(0x01);  // software reset
	IERG3810_TFTLCD_WrReg(0x11);  // exit_sleep_mode
	
	IERG3810_TFTLCD_WrReg(0x3A);  // set_pixel_format
	IERG3810_TFTLCD_WrData(0x55);  // 65536 colors
	
	IERG3810_TFTLCD_WrReg(0x29);  // display ON
	
	IERG3810_TFTLCD_WrReg(0x36);  // Memory Access Control (section 8.2.29 p.127)
	IERG3810_TFTLCD_WrData(0xCA);  
}

void IERG3810_TFTLCD_Init(void) // set FSMC
{
    RCC->AHBENR |= 1 << 8;    // FSMC
    RCC->APB2ENR |= 1 << 3;   // PORT B
    RCC->APB2ENR |= 1 << 5;   // PORT D
    RCC->APB2ENR |= 1 << 6;   // PORT E
    RCC->APB2ENR |= 1 << 8;   // PORT G
    GPIOB->CRL &= 0xFFFFFFF0; // PB0
    GPIOB->CRL |= 0x00000003;

    // PORT D
    GPIOD->CRH &= 0x00FFF000;
    GPIOD->CRH |= 0xBB000BBB;
    GPIOD->CRL &= 0xFF00FF00;
    GPIOD->CRL |= 0x00BB00BB;

    // PORT E
    GPIOE->CRH &= 0x00000000;
    GPIOE->CRH |= 0xBBBBBBBB;
    GPIOE->CRL &= 0x0FFFFFFF;
    GPIOE->CRL |= 0xB0000000;

    // PORTG 12
    GPIOG->CRH &= 0xFFF0FFFF;
    GPIOG->CRH |= 0x000B0000;
    GPIOG->CRL &= 0xFFFFFFF0; // PG0->RS
    GPIOG->CRL |= 0x0000000B;

    // LCD uses FSMC Bank 4 memory bank.
    // Use Mode A
    FSMC_Bank1->BTCR[6] = 0x00000000;  // FSMC_BCR4 (reset)
    FSMC_Bank1->BTCR[7] = 0x00000000;  // FSMC_BTR4 (reset)
    FSMC_Bank1E->BWTR[6] = 0x00000000; // FSMC_BWTR4 (reset)
    FSMC_Bank1->BTCR[6] |= 1 << 12;    // FSMC_BCR4 -> WREN
    FSMC_Bank1->BTCR[6] |= 1 << 14;    // FSMC_BCR4 -> EXTMOD
    FSMC_Bank1->BTCR[6] |= 1 << 4;     // FSMC_BCR4 -> MWID
    FSMC_Bank1->BTCR[7] |= 0 << 28;    // FSMC_BCR4 -> ACCMOD
    FSMC_Bank1->BTCR[7] |= 1 << 0;     // FSMC_BCR4 -> ADDSET
    FSMC_Bank1->BTCR[7] |= 0XF << 8;   // FSMC_BCR4 -> DATASET
    FSMC_Bank1E->BWTR[6] |= 0 << 28;   // FSMC_BWTR4 -> ACCMOD
    FSMC_Bank1E->BWTR[6] |= 0 << 0;    // FSMC_BWTR4 -> ADDSET
    FSMC_Bank1E->BWTR[6] |= 3 << 8;    // FSMC_BWTR4 -> DATAST
    FSMC_Bank1->BTCR[6] |= 1 << 0;     // FSMC_BCR4 -> FACCEN
    IERG3810_TFTLCD_SetParameter();    // special setting for LCD module
		// LCD_LIGHT_ON;
		GPIOB->BSRR |= 0x00000001; // light the LCD (at B0) on 
}


void IERG3810_TFTLCD_DrawDot(u16 x, u16 y, u16 color){
	
	//Hint: The size of the LCD is 320*240. Do not draw any dot out of the screen.
	IERG3810_TFTLCD_WrReg(0x2A);  //set x position
	IERG3810_TFTLCD_WrData(x>>8); 
	IERG3810_TFTLCD_WrData(x&0xFF);
	IERG3810_TFTLCD_WrData(0x01);
	IERG3810_TFTLCD_WrData(0x3F);
	
	IERG3810_TFTLCD_WrReg(0x2B);  //set y position
	IERG3810_TFTLCD_WrData(y>>8);
	IERG3810_TFTLCD_WrData(y & 0xFF);
	IERG3810_TFTLCD_WrData(0x01);
	IERG3810_TFTLCD_WrData(0xDF);
	
	IERG3810_TFTLCD_WrReg(0x2C);  //set point with color
	IERG3810_TFTLCD_WrData(color);
}

void IERG3810_TFTLCD_FillRectangle(u16 color,u16 start_x, u16 length_x, u16 start_y, u16 length_y){
	u32 index=0;
	IERG3810_TFTLCD_WrReg(0x2A);
	IERG3810_TFTLCD_WrData(start_x >> 8);
	IERG3810_TFTLCD_WrData(start_x & 0xFF);
	IERG3810_TFTLCD_WrData((length_x + start_x -1) >>8);
	IERG3810_TFTLCD_WrData((length_x + start_x -1) & 0xFF);
	
	IERG3810_TFTLCD_WrReg(0x2B);
	IERG3810_TFTLCD_WrData(start_y >> 8);
	IERG3810_TFTLCD_WrData(start_y&0xFF);
	IERG3810_TFTLCD_WrData((length_y + start_y -1) >>8);
	IERG3810_TFTLCD_WrData((length_y + start_y -1) & 0xFF);
	
	IERG3810_TFTLCD_WrReg(0x2C);  // LCD_WriteRAM_Prepare();
	for(index=0;index<length_x * length_y; index++)
  {
		IERG3810_TFTLCD_WrData(color);
	}
}

void IERG3810_TFTLCD_ShowChar(u16 x,u16 y,u8 ascii, u16 color, u16 bgcolor){
	u8 i,j;
	u8 index;
	u8 height = 16, length = 8;
	
	if(ascii < 32 || ascii > 127) return;
	ascii -= 32;
	IERG3810_TFTLCD_WrReg(0x2A);
	IERG3810_TFTLCD_WrData(x>>8);
	IERG3810_TFTLCD_WrData(x & 0xFF);
	IERG3810_TFTLCD_WrData((length + x - 1)>>8);
	IERG3810_TFTLCD_WrData((length + x - 1) & 0xFF);
	
	IERG3810_TFTLCD_WrReg(0x2B);
	IERG3810_TFTLCD_WrData(y>>8);
	IERG3810_TFTLCD_WrData(y & 0xFF);
	IERG3810_TFTLCD_WrData((height + y - 1)>>8);
	IERG3810_TFTLCD_WrData((height + y - 1) & 0xFF);
	
	IERG3810_TFTLCD_WrReg(0x2C); //LCD_WriteRAM_Prepare();
	
	for(j=0; j<height/8; j++){
		
		for(i=0; i<height/2; i++){
			
			for(index=0; index<length;index++){
				if((asc2_1608[ascii][index*2+1-j]>>i) & 0x01)
					IERG3810_TFTLCD_WrData(color);
				else
					IERG3810_TFTLCD_WrData(bgcolor);
			}
		}
	}
}

void IERG3810_TFTLCD_ShowChinChar(u16 x,u16 y,u8 code, u16 color, u16 bgcolor){
	u8 i,j;
	u8 index;
	u8 height = 16, length = 16;
	if(code >= 0x0A) return;
	
	IERG3810_TFTLCD_WrReg(0x2A);
	IERG3810_TFTLCD_WrData(x>>8);
	IERG3810_TFTLCD_WrData(x & 0xFF);
	IERG3810_TFTLCD_WrData((length + x - 1)>>8);
	IERG3810_TFTLCD_WrData((length + x - 1) & 0xFF);
	
	IERG3810_TFTLCD_WrReg(0x2B);
	IERG3810_TFTLCD_WrData(y>>8);
	IERG3810_TFTLCD_WrData(y & 0xFF);
	IERG3810_TFTLCD_WrData((height + y - 1)>>8);
	IERG3810_TFTLCD_WrData((height + y - 1) & 0xFF);
	
	IERG3810_TFTLCD_WrReg(0x2C);//LCD_WriteRAM_Prepare();
	
	for(j=0; j<height/8; j++){
		
		for(i=0; i<height/2; i++){
			
			for(index=0; index<length;index++){
				if((chi_1616[code][index*2+1-j]>>i) & 0x01)
					IERG3810_TFTLCD_WrData(color);
			  else
					IERG3810_TFTLCD_WrData(bgcolor);
				
			}
		}
	}
	
}

void IERG3810_TFTLCD_SevenSegment (u16 color, u16 start_x, u16 start_y, u8 digit)
{
	/* We use MSB, start at "a" */
	u8 patterns[10]={0XFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6};
	u8 pattern = patterns[digit];
	
	/* a 10000000 */
	if(pattern&(1<<7))
	{
		IERG3810_TFTLCD_FillRectangle(color, start_x + 10, 55, start_y + 130, 10); 
	}
	/* b 01000000 */
	if(pattern&(1<<6))
	{
		IERG3810_TFTLCD_FillRectangle(color, start_x + 65, 10, start_y + 75, 55); 
	}
	/* c 00100000 */
	if(pattern&(1<<5))
	{
		IERG3810_TFTLCD_FillRectangle(color, start_x + 65, 10, start_y + 10, 55); 
	}
	/* d 0001000 */
	if(pattern&(1<<4))
	{
		IERG3810_TFTLCD_FillRectangle(color, start_x + 10, 55, start_y + 0, 10); 
	}
	/* e 00001000 */
	if(pattern&(1<<3))
	{
		IERG3810_TFTLCD_FillRectangle(color, start_x + 0, 10, start_y + 10, 55); 
	}
	/* f 00000100 */
	if(pattern&(1<<2))
	{
		IERG3810_TFTLCD_FillRectangle(color, start_x + 0, 10, start_y + 75, 55); 
	}
	/* g 00000010 */
	if(pattern&(1<<1))
	{
		IERG3810_TFTLCD_FillRectangle(color, start_x + 10, 55, start_y + 65, 10); 
	}
}


void DrawLine_3_1 ()
{
	  u16 x;
		u16 y;
		/* black, white, green, red, blue */
		u16 colorBox[5] = {0x0000, 0xFFFF, 0x0F00, 0xF000, 0x00FF};

		/* y=10 to y=50, x=10 */
		for(y = 0xA; y <= 0x32; y += 0xA)
		{
			/* Ensure each line has 20 dots */
			for(x = 0xA; x < 0x1E; x+=0x01)
			{
				int index = (int)y / 0xA - 1;
				IERG3810_TFTLCD_DrawDot(x,y,colorBox[index]);
				Delay(10000); // Prevent from mix color
			}
		}
}

void DrawRectangle_3_2 ()
{
	u16 lineColor = 0xFFF0;
	u16 start_x = 0x64;
	u16 start_y = 0x64;
	u16 length_x = 0x64;
	u16 length_y = 0x64;
	
	IERG3810_TFTLCD_FillRectangle(lineColor, start_x, length_x, start_y, length_y);
	Delay(10000);
}

void CountDigits_3_3 ()
{
	u8 index;
	for(index = 0x0A; index >= 0x00; index-=0x01)
	{
			/* 80 and 80 is position, and 0 stand for the number */
			IERG3810_TFTLCD_SevenSegment (0xFF00, 80, 80, index);
			Delay(10000000);
		  IERG3810_TFTLCD_FillRectangle(0x0000,0,240,0,320); /* refresh the screen */
	
		if(index == 0x00)
		{
			index = 0x0A;
		}
	}
}

void CountDown()
{
	u8 index;
	for(index = 0x03; index >= 0x01; index-=0x01)
	{
			/* 80 and 80 is position, and 0 stand for the number */
			IERG3810_TFTLCD_SevenSegment (0xFF00, 80, 80, index);
			Delay(10000000);
		  IERG3810_TFTLCD_FillRectangle(0x0000,0,240,0,320); /* refresh the screen */
		  if (index == 0x01) {
				break;
			}
	}
}

void ShowCUSID_3_4 ()
{
		u8 sid1[10] = {0x31, 0x31, 0x35, 0x35, 0x31, 0x32, 0x35, 0x39, 0x32, 0x33}; // sid of the lower line
		u8 sid0[10] = {0x31, 0x31, 0x35, 0x35, 0x31, 0x32, 0x36, 0x38, 0x37, 0x35}; // sid of the upper line
		u16 i;
		for(i=0; i<0x000A; i+=0x01){
		IERG3810_TFTLCD_ShowChar(0x0040+i*0x0008,0x0050,sid1[i], 0x0000, 0x07E0); // lower line
		IERG3810_TFTLCD_ShowChar(0x0040+i*0x0008,0x0060,sid0[i], 0x0000, 0x07E0); // upper line
		Delay(150000);  // keep the current state stable by delaying
		}
}

void ShowChineseName_3_5 ()
{
		// Show the chinese name characters
		IERG3810_TFTLCD_ShowChinChar(0x40,0x90,0x00,0x0000, 0xF800);
		IERG3810_TFTLCD_ShowChinChar(0x50,0x90,0x01,0x0000, 0xF800);
		IERG3810_TFTLCD_ShowChinChar(0x60,0x90,0x02,0x0000, 0xF800);
		IERG3810_TFTLCD_ShowChinChar(0x40,0x80,0x03,0x0000, 0xF800);
		IERG3810_TFTLCD_ShowChinChar(0x50,0x80,0x04,0x0000, 0xF800);
		IERG3810_TFTLCD_ShowChinChar(0x60,0x80,0x05,0x0000, 0xF800);
		
		Delay(100000);
}







