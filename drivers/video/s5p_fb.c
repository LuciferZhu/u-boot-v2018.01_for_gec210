/*
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *
 * S5PC110 - LCD Driver for U-Boot
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/types.h>
#include <lcd.h>
#include <asm/arch/s5pc110.h>


#define LCD_BGCOLOR		0x0//0x1428A0

#if 0
static unsigned int gFgColor = 0xFF00;
static unsigned int gLeftColor = LCD_BGCOLOR;
#endif

#define Inp32(_addr)		readl(_addr)
#define Outp32(addr, data)	(*(volatile u32 *)(addr) = (data))
#define Delay(_a)		udelay(_a*1000)


#if defined(CFG_LCD_TL2796)
/*
 * TL2796
 */

#define LCD_WIDTH		480
#define LCD_HEIGHT		800

#define IIC_SDA_GPIO_ADDRESS		(0xE02000C0)	//GPB
#define IIC_SDA_GPIO_BIT			(0)		//GPB[7]
#define IIC_SCL_GPIO_ADDRESS		(0xE02000C0)	//GPB
#define IIC_SCL_GPIO_BIT			(1)		//GPB[4]
#define SDA_GPIO_ADDRESS			(0xE0200040)	//GPB
#define SDA_GPIO_BIT				(7)		//GPB[7]
#define SCL_GPIO_ADDRESS			(0xE0200040)	//GPB
#define SCL_GPIO_BIT				(4)		//GPB[4]
#define CS_GPIO_ADDRESS				(0xE0200040)	//GPB
#define CS_GPIO_BIT					(5)		//GPB[5]
#define BACKLIGHT_GPIO_ADDRESS		(0xE02000A0)	//GPD0
#define BACKLIGHT_GPIO_BIT			(3)		//GPD0[3]
#define RESET_GPIO_ADDRESS			(0xE0200C00)	//GPH0
#define RESET_GPIO_BIT				(6)		//GPH0.6
#define POWER_GPIO_ADDRESS			(0xE0200040)
#define POWER_GPIO_BIT				(6)
#define POWER_GPIO_ADDRESS2			(0xE0200040)
#define POWER_GPIO_BIT2				(7)
#define BACKLIGHT_ACTIVE 			HIGH_ACTIVE
#define RESET_ACTIVE 				LOW_ACTIVE
#define POWER_ACTIVE 				HIGH_ACTIVE

//do not modify
#define LCDM_SDA_HIGH			Outp32(SDA_GPIO_ADDRESS+4,(Inp32(SDA_GPIO_ADDRESS+4) |= (1<<SDA_GPIO_BIT)))
#define LCDM_SDA_LOW			Outp32(SDA_GPIO_ADDRESS+4,(Inp32(SDA_GPIO_ADDRESS+4) &= ~(1<<SDA_GPIO_BIT)))
#define LCDM_SCL_HIGH			Outp32(SCL_GPIO_ADDRESS+4,(Inp32(SCL_GPIO_ADDRESS+4) |= (1<<SCL_GPIO_BIT)))
#define LCDM_SCL_LOW			Outp32(SCL_GPIO_ADDRESS+4,(Inp32(SCL_GPIO_ADDRESS+4) &= ~(1<<SCL_GPIO_BIT)))
#define LCDM_CS_HIGH			Outp32(CS_GPIO_ADDRESS+4,(Inp32(CS_GPIO_ADDRESS+4) |=(1<<CS_GPIO_BIT)))
#define LCDM_CS_LOW				Outp32(CS_GPIO_ADDRESS+4,(Inp32(CS_GPIO_ADDRESS+4) &= ~(1<<CS_GPIO_BIT)))
#define LCDM_BACKLIGHT_HIGH 	Outp32(BACKLIGHT_GPIO_ADDRESS+4,(Inp32(BACKLIGHT_GPIO_ADDRESS+4) |= (1<<BACKLIGHT_GPIO_BIT)))
#define LCDM_BACKLIGHT_LOW	Outp32(BACKLIGHT_GPIO_ADDRESS+4,(Inp32(BACKLIGHT_GPIO_ADDRESS+4) &= ~(1<<BACKLIGHT_GPIO_BIT)))
#define LCDM_RESET_HIGH		 	Outp32(RESET_GPIO_ADDRESS+4,(Inp32(RESET_GPIO_ADDRESS+4) |= (1<<RESET_GPIO_BIT)))
#define LCDM_RESET_LOW			Outp32(RESET_GPIO_ADDRESS+4,(Inp32(RESET_GPIO_ADDRESS+4) &= ~(1<<RESET_GPIO_BIT)))
#define LCDM_POWER_HIGH		 	Outp32(POWER_GPIO_ADDRESS+4,(Inp32(POWER_GPIO_ADDRESS+4) |= (1<<POWER_GPIO_BIT)))
#define LCDM_POWER_LOW			Outp32(POWER_GPIO_ADDRESS+4,(Inp32(POWER_GPIO_ADDRESS+4) &= ~(1<<POWER_GPIO_BIT)))
#define LCDM_POWER2_HIGH	 	Outp32(POWER_GPIO_ADDRESS+4,(Inp32(POWER_GPIO_ADDRESS+4) |= (1<<POWER_GPIO_BIT)))
#define LCDM_POWER2_LOW	 		Outp32(POWER_GPIO_ADDRESS+4,(Inp32(POWER_GPIO_ADDRESS+4) &= ~(1<<POWER_GPIO_BIT)))
#define LCDM_IIC_SCL_HIGH		Outp32(SCL_GPIO_ADDRESS+4,(Inp32(SCL_GPIO_ADDRESS+4) |= (1<<SCL_GPIO_BIT)))
#define LCDM_IIC_SCL_LOW		Outp32(SCL_GPIO_ADDRESS+4,(Inp32(SCL_GPIO_ADDRESS+4) &= ~(1<<SCL_GPIO_BIT)))
#define LCDM_IIC_SDA_HIGH		Outp32(SDA_GPIO_ADDRESS+4,(Inp32(SDA_GPIO_ADDRESS+4) |= (1<<SDA_GPIO_BIT)))
#define LCDM_IIC_SDA_LOW		Outp32(SDA_GPIO_ADDRESS+4,(Inp32(SDA_GPIO_ADDRESS+4) &= ~(1<<SDA_GPIO_BIT)))


void Write_LDI_TL2796(u16 uAddr, u16 uData)
{
	s32 j;

	if(uAddr != 0x0000)
	{
		LCDM_CS_HIGH;
		LCDM_SCL_HIGH;
		LCDM_SDA_HIGH;

		LCDM_CS_LOW;
		LCDM_SCL_LOW;

	    // Command
	    for (j = 15 ; j >= 0; j--)
		{
			LCDM_SCL_LOW;

			if ((uAddr >> j) & 0x0001)
			{
				LCDM_SDA_HIGH;
			}
			else
			{
				LCDM_SDA_LOW;
			}
			LCDM_SCL_HIGH;
		}
	}

	if (uAddr == 0x0000)
	{
		LCDM_SDA_HIGH;
	}

	LCDM_CS_HIGH;
	LCDM_SCL_HIGH;
	LCDM_CS_LOW;
	LCDM_SCL_LOW;

	// parameter
	for (j = 15 ; j >= 0; j--)
	{
		LCDM_SCL_LOW;

		if ((uData >> j) & 0x0001)
		{
			LCDM_SDA_HIGH;
		}
		else
		{
			LCDM_SDA_LOW;
		}
		LCDM_SCL_HIGH;
	}
}

void LCDM_InitTL2796(void)
{
    Delay(50); // more than 20ms

	//[1] Panel Condition Set
	Write_LDI_TL2796(0x7031, 0x7208);
	Write_LDI_TL2796(0x7032, 0x7214);
	Write_LDI_TL2796(0x7030, 0x7202);

#if 1
	Write_LDI_TL2796(0x7027, 0x7203);
#else
	Write_LDI_TL2796(0x7027, 0x7201);
#endif

	//[2] Display Condition Set(RGB Data Interface)
	Write_LDI_TL2796(0x7012, 0x7208); //0x7208
	Write_LDI_TL2796(0x7013, 0x7208);
#if 1
	Write_LDI_TL2796(0x7015, 0x7210); //0x10
#else
	Write_LDI_TL2796(0x7015, 0x7200); //0x00
#endif
	Write_LDI_TL2796(0x7016, 0x7200);	//00 24bit 02 16bit RGB interface 
	Write_LDI_TL2796(0x70ef, 0x72d0);
	Write_LDI_TL2796(0x0000, 0x72e8);

	// set gamma control 230cd
	Write_LDI_TL2796(0x7039, 0x7244);
	Write_LDI_TL2796(0x7040, 0x7200);
	Write_LDI_TL2796(0x7041, 0x723F);
	Write_LDI_TL2796(0x7042, 0x722B);
	Write_LDI_TL2796(0x7043, 0x721f);
	Write_LDI_TL2796(0x7044, 0x7224);
	Write_LDI_TL2796(0x7045, 0x721b);
	Write_LDI_TL2796(0x7046, 0x7229);
	Write_LDI_TL2796(0x7050, 0x7200);
	Write_LDI_TL2796(0x7051, 0x7200);
	Write_LDI_TL2796(0x7052, 0x7200);
	Write_LDI_TL2796(0x7053, 0x721b);
	Write_LDI_TL2796(0x7054, 0x7222);
	Write_LDI_TL2796(0x7055, 0x721b);
	Write_LDI_TL2796(0x7056, 0x722a);
	Write_LDI_TL2796(0x7060, 0x7200);
	Write_LDI_TL2796(0x7061, 0x723F);
	Write_LDI_TL2796(0x7062, 0x7225);
	Write_LDI_TL2796(0x7063, 0x721c);
	Write_LDI_TL2796(0x7064, 0x7221);
	Write_LDI_TL2796(0x7065, 0x7218);
	Write_LDI_TL2796(0x7066, 0x723e);

	//Analog Power Condition Set
	Write_LDI_TL2796(0x7017, 0x7222);	//Boosting Freq
	Write_LDI_TL2796(0x7018, 0x7233);	//power AMP Medium
	Write_LDI_TL2796(0x7019, 0x7203);	//Gamma Amp Medium
	Write_LDI_TL2796(0x701a, 0x7201);	//Power Boosting
	Write_LDI_TL2796(0x7022, 0x72a4);	//Vinternal = 0.65*VCI
	Write_LDI_TL2796(0x7023, 0x7200);	//VLOUT1 Setting = 0.98*VCI
	Write_LDI_TL2796(0x7026, 0x72a0);	//Display Condition LTPS signal generation : Reference= DOTCLK

	//Stand-by Off Comman
	Write_LDI_TL2796(0x701d, 0x72a0);

	Delay(500); // more than 250ms

	//LCD ON
	Write_LDI_TL2796(0x7014, 0x7203);
}

void LCD_Initialize_TL2796(void)
{
	u32 uFbAddr = CFG_LCD_FBUFFER;

	u32 i,uTmpReg;
	u32* pBuffer = (u32*)uFbAddr;


	LCD_setprogress(0);


	uTmpReg = Inp32(0xE0107008);		// need to be changed later (09.01.23 WJ.Kim)	
	uTmpReg = (uTmpReg & ~(0x3)) | 0x2 ;
	Outp32(0xE0107008, uTmpReg);

	uTmpReg = Inp32(0xf800027c);
	uTmpReg = (uTmpReg & ~(0x3)) | 0 ;
	Outp32(0xf800027c,uTmpReg);


	Outp32(0xE0200120, 0x22222222);	//set GPF0 as LVD_HSYNC,VSYNC,VCLK,VDEN,VD[3:0]
	Outp32(0xE0200128,0x0);			//set pull-up,down disable
	Outp32(0xE0200140, 0x22222222);	//set GPF1 as VD[11:4]
	Outp32(0xE0200148,0x0);			//set pull-up,down disable
	Outp32(0xE0200160, 0x22222222);	//set GPF2 as VD[19:12]
	Outp32(0xE0200168,0x0);			//set pull-up,down disable
	Outp32(0xE0200180, 0x00002222);	//set GPF3 as VD[23:20]
	Outp32(0xE0200188,0x0);			//set pull-up,down disable

	Outp32(0xe02000A0, 0x11111111);	//set GPF0 as LVD_HSYNC,VSYNC,VCLK,VDEN,VD[3:0]
	Outp32(0xe02000A8,0x0);			//set pull-up,down disable
	Outp32(0xe02000A4,(1<<3));

	Outp32(0xE0200040, 0x11111111);	//set GPF0 as LVD_HSYNC,VSYNC,VCLK,VDEN,VD[3:0]
	Outp32(0xE0200048,0x0);			//set pull-up,down disable
	Outp32(0xE0200044,0);

	Outp32(0xE02000c0, 0x11111111);	//set GPF0 as LVD_HSYNC,VSYNC,VCLK,VDEN,VD[3:0]
	Outp32(0xE02000c8,0x0);			//set pull-up,down disable
	Outp32(0xE02000c4,0);

	Outp32(0xE0200C00, 0x11111111);	//set GPF0 as LVD_HSYNC,VSYNC,VCLK,VDEN,VD[3:0]
	Outp32(0xE0200C08,0x0);			//set pull-up,down disable
	Outp32(0xE0200C04,0);
	Delay(50);
	Outp32(0xE0200C04,0xffffffff);

	Outp32(SDA_GPIO_ADDRESS+4,(Inp32(SDA_GPIO_ADDRESS+4) |= (1<<SDA_GPIO_BIT)));
	Outp32(SCL_GPIO_ADDRESS+4,(Inp32(SCL_GPIO_ADDRESS+4) |= (1<<SCL_GPIO_BIT)));
	Outp32(CS_GPIO_ADDRESS+4,(Inp32(CS_GPIO_ADDRESS+4) |=(1<<CS_GPIO_BIT)));
	Outp32(SCL_GPIO_ADDRESS+4,(Inp32(SCL_GPIO_ADDRESS+4) |= (1<<SCL_GPIO_BIT)));
	Outp32(SDA_GPIO_ADDRESS+4,(Inp32(SDA_GPIO_ADDRESS+4) |= (1<<SDA_GPIO_BIT)));

	Outp32(0xf8000004, 0xf0);
	Outp32(0xf8000010, 0x50505);
	Outp32(0xf8000014, 0x50505);
	Outp32(0xf8000170, 0x0);
	Outp32(0xf8000018, 0x18f9df);
	Outp32(0xf8000000, 0x0);
	Outp32(0xf8000000, 0x190);
	Outp32(0xf8000130, 0x20);
	Outp32(0xf8000020, 0x0);
	Outp32(0xf8000024, 0x0);
	Outp32(0xf8000028, 0x0);
	Outp32(0xf800002c, 0x0);
	Outp32(0xf8000030, 0x0);
	Outp32(0xf8000034, 0x0);
	Outp32(0xf8000180, 0x0);
	Outp32(0xf8000184, 0x0);
	Outp32(0xf8000188, 0x0);
	Outp32(0xf800018c, 0x0);
	Outp32(0xf8000190, 0x0);
	Outp32(0xf8000140, 0x0);
	Outp32(0xf8000148, 0x0);
	Outp32(0xf8000150, 0x0);
	Outp32(0xf8000158, 0x0);
	Outp32(0xf8000058, 0x0);
	Outp32(0xf8000208, 0x0);
	Outp32(0xf800020c, 0x0);
	Outp32(0xf8000068, 0x0);
	Outp32(0xf8000210, 0x0);
	Outp32(0xf8000214, 0x0);
	Outp32(0xf8000078, 0x0);
	Outp32(0xf8000218, 0x0);
	Outp32(0xf800021c, 0x0);
	Outp32(0xf8000088, 0x0);
	Outp32(0xf8000220, 0x0);
	Outp32(0xf8000224, 0x0);
	Outp32(0xf8000260, 0x1);
	Outp32(0xf8000034, 0x0);
	Outp32(0xf80000a4, uFbAddr + 0x0);
	Outp32(0xf80000d4, uFbAddr + 0x177000);
	Outp32(0xf80000a0, uFbAddr + 0x0);
	Outp32(0xf80000d0, uFbAddr + 0x177000);
	Outp32(0xf80020a0, uFbAddr + 0x0);
	Outp32(0xf80020d0, uFbAddr + 0x177000);
	Outp32(0xf8000100, 0x780);
	Outp32(0xf8000020, 0x8000);
	Outp32(0xf8000020, 0x802c);
	Outp32(0xf8000040, 0x0);
	Outp32(0xf8000044, 0xefb1f);
	Outp32(0xf8000200, 0xffffff);
	Outp32(0xf8000204, 0xffffff);
	Outp32(0xf8000034, 0x400);
	Outp32(0xf8000020, 0x802d);
	Outp32(0xf8000034, 0x1);
	Outp32(0xf8000034, 0x1);
	Outp32(0xf8000034, 0x401);
	Outp32(0xf8000020, 0x802d);
	Outp32(0xf8000034, 0x1);
	Outp32(0xf8000034, 0x1);

	Outp32(0xf8000000, 0x193);

//	LCDM_InitLdi(LCD_TL2796, 0);
	LCDM_InitTL2796();
}
#elif defined(CFG_LCD_NONAME1)
/*
 *
 */

#define LCD_WIDTH		1366
#define LCD_HEIGHT		500	//766

void LCD_Initialize_NONAME1(void)
{
	u32 uFbAddr = CFG_LCD_FBUFFER;

	u32 i;
	u32* pBuffer = (u32*)uFbAddr;

	Outp32(0xe0200040, 0x10000000);
	Outp32(0xe0200048, 0x1555);
	Outp32(0xe020004c, 0xc000);
	Outp32(0xe0200040, 0x10010000);
	Outp32(0xe0200048, 0x1455);
	Outp32(0xe020004c, 0xc300);
	Outp32(0xe0200040, 0x10110000);
	Outp32(0xe0200048, 0x1055);
	Outp32(0xe020004c, 0xcf00);
	Outp32(0xe02000c0, 0x1);
	Outp32(0xe02000c8, 0x54);
	Outp32(0xe02000cc, 0x3);
	Outp32(0xe02000c0, 0x11);
	Outp32(0xe02000c8, 0x50);
	Outp32(0xe02000cc, 0xf);
	Outp32(0xe02000a0, 0x1001);
	Outp32(0xe02000a8, 0x15);
	Outp32(0xe02000ac, 0xc0);
	Outp32(0xe0200c00, 0x1000010);
	Outp32(0xe0200c08, 0x4455);
	Outp32(0xe0200c0c, 0x3000);
	Outp32(0xe0200040, 0x11110000);
	Outp32(0xe0200048, 0x55);
	Outp32(0xe020004c, 0xff00);
	Outp32(0xe0200040, 0x11110100);
	Outp32(0xe0200048, 0x55);
	Outp32(0xe020004c, 0xff00);
	Outp32(0xe0200044, 0x80);
	Outp32(0xe0200044, 0x98);
	Outp32(0xe0200044, 0xb9);
	Outp32(0xe0200044, 0xbb);
	Outp32(0xe0200044, 0xbb);
	Outp32(0xe02000a4, 0xd);
	Outp32(0xe0200c04, 0xd1);
	Outp32(0xe0200044, 0xfb);
	Outp32(0xe0200044, 0xff);
	Outp32(0xe0200c04, 0x91);
	Outp32(0xe0200c04, 0xd1);
	Outp32(0xe0200c04, 0xd3);


	Outp32(0xe0200120, 0x22222222);	//set GPF0 as LVD_HSYNC,VSYNC,VCLK,VDEN,VD[3:0]
	Outp32(0xe0200128,0x0);			//set pull-up,down disable
	Outp32(0xe0200140, 0x22222222);	//set GPF1 as VD[11:4]
	Outp32(0xe0200148,0x0);			//set pull-up,down disable
	Outp32(0xe0200160, 0x22222222);	//set GPF2 as VD[19:12]
	Outp32(0xe0200168,0x0);			//set pull-up,down disable
	Outp32(0xe0200180, 0x00002222);	//set GPF3 as VD[23:20]
	Outp32(0xe0200188,0x0);			//set pull-up,down disable

	//--------- S5PC110 EVT0 needs MAX drive strength	---------//
	Outp32(0xe020012c,0xffffffff);			//set GPF0 drive strength max by WJ.KIM(09.07.17)
	Outp32(0xe020014c,0xffffffff);			//set GPF1 drive strength max by WJ.KIM(09.07.17)
	Outp32(0xe020016c,0xffffffff);			//set GPF2 drive strength max by WJ.KIM(09.07.17)
	Outp32(0xe020018c,0x3ff);					//set GPF3 drive strength max by WJ.KIM(09.07.17)

	Outp32(0xf8000004, 0x60);
	Outp32(0xf8000010, 0xe0e0305);
	Outp32(0xf8000014, 0x3103020);
	Outp32(0xf8000170, 0x0);
	Outp32(0xf8000018, 0x17fd55);
	Outp32(0xf8000000, 0x0);
	Outp32(0xf8000000, 0x254);
	Outp32(0xf8000130, 0x20);
	Outp32(0xf8000020, 0x0);
	Outp32(0xf8000024, 0x0);
	Outp32(0xf8000028, 0x0);
	Outp32(0xf800002c, 0x0);
	Outp32(0xf8000030, 0x0);
	Outp32(0xf8000034, 0x0);
	Outp32(0xf8000180, 0x0);
	Outp32(0xf8000184, 0x0);
	Outp32(0xf8000188, 0x0);
	Outp32(0xf800018c, 0x0);
	Outp32(0xf8000190, 0x0);
	Outp32(0xf8000140, 0x0);
	Outp32(0xf8000148, 0x0);
	Outp32(0xf8000150, 0x0);
	Outp32(0xf8000158, 0x0);
	Outp32(0xf8000058, 0x0);
	Outp32(0xf8000208, 0x0);
	Outp32(0xf800020c, 0x0);
	Outp32(0xf8000068, 0x0);
	Outp32(0xf8000210, 0x0);
	Outp32(0xf8000214, 0x0);
	Outp32(0xf8000078, 0x0);
	Outp32(0xf8000218, 0x0);
	Outp32(0xf800021c, 0x0);
	Outp32(0xf8000088, 0x0);
	Outp32(0xf8000220, 0x0);
	Outp32(0xf8000224, 0x0);
	Outp32(0xf8000260, 0x1);
	Outp32(0xf8000048, 0x100200);
	Outp32(0xf8000200, 0xffffff);
	Outp32(0xf8000204, 0xffffff);
	Outp32(0xf8000034, 0x0);
	Outp32(0xf8000020, 0x802c);
	Outp32(0xf80000a0, uFbAddr + 0x00000000);
	Outp32(0xf80000d0, uFbAddr + 0x00400800);
	Outp32(0xf80000a4, uFbAddr + 0x00000000);
	Outp32(0xf80000d4, uFbAddr + 0x00400800);
	Outp32(0xf80020a0, uFbAddr + 0x00000000);
	Outp32(0xf80020d0, uFbAddr + 0x00400800);
	Outp32(0xf8000100, 0x1558);
	Outp32(0xf8000040, 0x0);
	Outp32(0xf8000044, 0x2aaaff);
	Outp32(0xf8000020, 0x802d);
	Outp32(0xf8000034, 0x1);
	Outp32(0xf8000020, 0x802d);
	Outp32(0xf8000034, 0x1);
//	Outp32(0xf8000000, 0x257);
//	Outp32(0xf8000000, 0x57); //===> MPLL should be 667 !!!!
	Outp32(0xf8000000, 0x53);
	Outp32(0xf8000010, 0x60400);
	Outp32(0xf80001a4, 0x3);

	Outp32(0xe0107008,0x2); //syscon output path
	Outp32(0xe0100204,0x700000); //syscon fimdclk = mpll

	LCD_setprogress(0);
}
#elif defined(CONFIG_LCD_AT070TN90)

static void *fb_base;				/* Start of framebuffer memory	*/

vidinfo_t panel_info = {
	.vl_col = 800,		/* Number of columns (i.e. 160) */
	.vl_row = 480,		/* Number of rows (i.e. 100) */
	.vl_bpix = LCD_BPP,			/* Bits per pixel, 0 = 1, 1 = 2, 2 = 4, 3 = 8 */
} ;


static void LCD_Initialize_AT070TN90(void *lcdbase)
{
	/* 1.配置引脚
	 *   LCD_HSYNC LCD_VSYNC LCD_VDEN LCD_VCLK
	 *  LCD_VD[0] LCD_VD[1] LCD_VD[2] LCD_VD[3]
	 */
	Outp32(GPF0CON, 0x22222222);

	/* 2.配置引脚
	 *   LCD_VD[4] LCD_VD[5] LCD_VD[6] LCD_VD[7]
	 *    LCD_VD[8] LCD_VD[9] LCD_VD[10] LCD_VD[11]
	 */
	Outp32(GPF1CON, 0x22222222);

	/* 3.配置引脚
	   LCD_VD[12] LCD_VD[13] LCD_VD[14] LCD_VD[15]
	   LCD_VD[16] LCD_VD[17] LCD_VD[18] LCD_VD[19]
	*/
	Outp32(GPF2CON, 0x22222222);

	/* 4.配置引脚
	    LCD_VD[20] LCD_VD[21] LCD_VD[22] LCD_VD[23]
	*/
	Outp32(GPF3CON, Inp32(GPF3CON) & (~((0xF<<12)|(0xF<<8)|(0xF<<4)|(0xF<<0))));
	Outp32(GPF3CON, Inp32(GPF3CON) | ((0x2<<12)|(0x2<<8)|(0x2<<4)|(0x2<<0)));

	/* 5.VIDCON0
	   Note:ENVID and ENVID_F are set to “1”
	   [0]:1 Display On必须置1
	   [1]:1 Display On必须置1
	   [4]:1 因为显示的时钟需要用到33.3MHz的频率，那么要进行分频
	   [6]:4 显示主区域的HCLK时钟为166MHz，为了得到33.3MHz左右的频率，当前的值要设置为4
	   [18]:0 因为我们当前是RGB并行接口，意思就是说一次发送3个字节，什么是串行接口，3个字节分3次来发送
	   [26]:0 配置为RGB接口
	*/
	//rVIDCON0=(0<<26)|(0<<18)|(4<<6)|(1<<4)|(1<<1)|(1<<0);
	Outp32(ELFIN_LCD_BASE, (0<<26)|(0<<18)|(4<<6)|(1<<4)|(1<<1)|(1<<0));

	/* 6.VIDCON1，通过观看S70-AT070TN92(GEC210).pdf中的13页发现VSYNC和HSYNC信号发现与210手册的1207页极性相反，需要进行配置
	   [5]:1 VSYNC极性取反
	   [6]:1 HSYNC极性取反
	*/
	//rVIDCON1=(1<<6)|(1<<5);
	Outp32( ELFIN_LCD_BASE+0x4, (1<<6)|(1<<5));

	/* 7.配置rVIDTCON0，这部分要查看S70-AT070TN92(GEC210).pdf中的14页中3.3.3Timing
	   [7:0]:9+1	VSPW垂直同步信号宽度，这是VSYNC同步信号的高电平持续时间
	   [8:15]:21+1	VFPD
	   [16:23]:12+1 VBPD
	*/
	//rVIDTCON0=(12<<16)|(21<<8)|(9<<0);
	Outp32(ELFIN_LCD_BASE+0x10, (12<<16)|(21<<8)|(9<<0));

	/* 8.配置rVIDTCON1，这部分要查看S70-AT070TN92(GEC210).pdf中的14页中3.3.3 Timing
	   [7:0]:19+1	 HSPW水平同步信号宽度，这是VSYNC同步信号的高电平持续时间
	   [8:15]:209+1  HFPD
	   [16:23]:25+1  HBPD
	*/
	//rVIDTCON1=(25<<16)|(209<<8)|(19<<0);
	Outp32(ELFIN_LCD_BASE+0x14, (25<<16)|(209<<8)|(19<<0));

	/* 9.配置rVIDTCON2，这个要看LCD的分辨率，800x480
	   [10:0]:799+1   一行有多少个像素点
	   [11:21]:479+1  有多少行
	*/
	//rVIDTCON2 = (479<<11)|(799<<0);
	Outp32(ELFIN_LCD_BASE+0x18, (479<<11)|(799<<0));

	/* 10.配置rWINCON0
	    [0]:1	     使能视频输出
	    [5:2]:1011	 显示格式是24位色，R:8 G:8 B:8
	    [15]:1	 发现EN_LOCAL=0，因为我们待会使用显存即DMA,使能字交换
			 即每次DMA传输数据最小单元为字传输
	    [22]:0	使用DMA（直接内存访问:直接向某一地址写数据的时候会自动操作硬件，不需要CPU进行干预）
	*/
	//rWINCON0=(1<<15)|(0xB<<2)|(1<<0);
	Outp32(ELFIN_LCD_BASE+0x20, (1<<15)|(0xB<<2)|(1<<0));

	/* 11.配置rVIDW00ADD0B0，用于设置显示缓存的起始地址
	*/
	//rVIDW00ADD0B0=0x48000000;
	Outp32(ELFIN_LCD_BASE+0xA0, (u32)lcdbase);

	/* 12.rVIDW00ADD1B0，用于配置显示缓存的结束地址，告诉CPU我要占用多大的空间
	    公式如下:VBASEL = VBASEU +(PAGEWIDTH+OFFSIZE) x (LINEVAL+1)
			    = 0x48000000+(800+0)x(479+1)x4
		800x480*4[A:R:G:B]
	*/
	//rVIDW00ADD1B0=0x48000000+800*4*480;
	Outp32(ELFIN_LCD_BASE+0xD0, ((u32)lcdbase)+800*4*480);

	/* 13.rVIDW00ADD2:用于配置窗口的偏移值还有一行占用多少字节
	   [12:0]  : 800*4 因为Linux显示是32位色，为了兼容以后的Linux驱动部分，增加它的空间
					   字对齐
	   [13:25] : 因为当前显示不需要偏移，设置为0就可以了。
	 */
	//rVIDW00ADD2  = (0<<13)|((800*4)<<0);
	Outp32(ELFIN_LCD_BASE+0x100, (0<<13)|((800*4)<<0));

	/* 14.rSHADOWCON 使能Channel 0
	   [0]:1
	 */
	//rSHADOWCON|=(1<<0);
	Outp32(ELFIN_LCD_BASE+0x34, Inp32(ELFIN_LCD_BASE+0x34) | (1<<0));

	/* 15.rVIDOSD0A 配置显示区域的左上角x/y坐标值
	    [10:0] :0  左上角的Y坐标值
	    [21:11]:0  左上角的X坐标值
	*/
	//rVIDOSD0A = (0<<11)|(0<<0);
	Outp32(ELFIN_LCD_BASE+0x40, (0<<11)|(0<<0));

	/* 16.rVIDOSD0B 配置显示区域的右下角x/y坐标值
	    [10:0] :479  右下角的Y坐标值
	    [21:11]:799  右下角的X坐标值
	*/
	//rVIDOSD0B = (799<<11)|(479<<0);
	Outp32(ELFIN_LCD_BASE+0x44, (799<<11)|(479<<0));

	/* 17.rVIDOSD0C 配置窗口的大小也就是窗口的分辨率
	    [23:0] :800*480  For example, Height * Width
	*/
	//rVIDOSD0C = 800*480;
	Outp32(ELFIN_LCD_BASE+0x48, 800*480);

	/* 18.配置FIMD一定要为RGB接口
	 */
	//rDISPLAY_CONTROL=2<<0;
	Outp32(0xe0107008, 2<<0);

}

void lcd_ctrl_init(void *lcdbase)
{
	fb_base = lcdbase;
	LCD_Initialize_AT070TN90(lcdbase);

	/* Enable flushing if we enabled dcache for staying dma coherent */
	lcd_set_flush_dcache(1);
}

void lcd_enable (void)
{
	return;
}

void lcd_setcolreg(ushort regno, ushort red, ushort green, ushort blue)
{
	return;
}


#else
//# error "No LCD Type is defined!"

#endif


#if 0
void LCD_turnon(void)
{
#if defined(CFG_LCD_TL2796)
	LCD_Initialize_TL2796();
#elif defined(CFG_LCD_NONAME1)
	LCD_Initialize_NONAME1();
#elif defined(CONFIG_LCD_AT070TN90)
	LCD_Initialize_AT070TN90((void *)CFG_LCD_FBUFFER);
#endif
}

void LCD_setfgcolor(unsigned int color)
{
	gFgColor = color;
}

void LCD_setleftcolor(unsigned int color)
{
	gLeftColor = color;
}

void LCD_setprogress(int percentage)
{
#if defined(CFG_LCD_TL2796) || defined(CFG_LCD_NONAME1) || defined(CONFIG_LCD_AT070TN90)
	u32 i, j;
	u32* pBuffer = (u32*)fb_base;

	for (i=0; i < panel_info.vl_row*percentage/100; i++)
	{
		for (j=0; j < panel_info.vl_col; j++)
		{
			*pBuffer++ = gFgColor;
		}
	}

	for (; i < panel_info.vl_row; i++)
	{
		for (j=0; j < (panel_info.vl_col >> 5); j++)
		{
			*pBuffer++ = gLeftColor;
		}
		for (; j < panel_info.vl_col; j++)
		{
			*pBuffer++ = LCD_BGCOLOR;
		}
	}
#endif
}
#endif

