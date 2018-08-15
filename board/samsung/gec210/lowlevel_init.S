/*
 * Copyright (C) 2009 Samsung Electronics
 * Kyungmin Park <kyungmin.park@samsung.com>
 * Minkyu Kang <mk7.kang@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <asm/arch/cpu.h>
#include <asm/arch/power.h>
#include <asm/arch/s5pc110.h>

/*
 * Register usages:
 *
 * r5 has zero always
 */

	.globl lowlevel_init
lowlevel_init:
	mov	r9, lr

	/* r5 has always zero */
	mov	r5, #0

	ldr	r8, =S5PC110_GPIO_BASE

	/* Disable Watchdog */
	ldr	r0, =S5PC110_WATCHDOG_BASE
	orr	r0, r0, #0x0
	str	r5, [r0]

	/* when we already run in ram, we do not need to relocate U-Boot.
	 * and actually, memory controller must be configured before U-Boot
	 * is running in ram.
	 */
	ldr	r0, =0xff000fff
	bic	r1, pc, r0		/* r0 <- current base addr of code */
	ldr	r2, =CONFIG_SYS_TEXT_BASE /* r1 <- original base addr in ram */
	bic	r2, r2, r0		/* r0 <- current base addr of code */
	cmp     r1, r2                  /* compare r0, r1                  */
	beq     1f			/* r0 == r1 then skip sdram init   */

	/* init system clock */
	bl system_clock_init

	/* Memory initialize */
	bl mem_ctrl_asm_init

1:
	/* for UART */
	bl uart_asm_init

	/* for TZPC */
	bl tzpc_asm_init

	/* simple init for NAND */
	bl nand_asm_init

	/* Print 'K' */
	ldr	r0, =ELFIN_UART_CONSOLE_BASE
	ldr	r1, =0x4b4b4b4b
	str	r1, [r0, #UTXH_OFFSET]

	mov	lr, r9
	mov	pc, lr


/*
 * system_clock_init: Initialize core clock and bus clock.
 * void system_clock_init(void)
 */
system_clock_init:

	ldr	r0, =ELFIN_CLOCK_POWER_BASE	@0xe0100000

	/* Set Mux to FIN */
	ldr	r1, =0x0
	str	r1, [r0, #CLK_SRC0_OFFSET]

	ldr	r1,	=APLL_LOCKTIME_VAL
	str	r1,	[r0, #APLL_LOCK_OFFSET]

	/* Disable PLL */
retryloop:
	ldr	r1, =0x0
	str	r1, [r0, #APLL_CON0_OFFSET]
	ldr	r1, =0x0
	str	r1, [r0, #MPLL_CON_OFFSET]

	ldr	r1, =0x0
	str	r1, [r0, #MPLL_CON_OFFSET]

	ldr   	r1, [r0, #CLK_DIV0_OFFSET]
	ldr	r2, =CLK_DIV0_MASK
	bic	r1, r1, r2

	ldr	r2, =CLK_DIV0_VAL
	orr	r1, r1, r2
	str	r1, [r0, #CLK_DIV0_OFFSET]

	ldr	r1, =APLL_VAL
	str	r1, [r0, #APLL_CON0_OFFSET]

	ldr	r1, =MPLL_VAL
	str	r1, [r0, #MPLL_CON_OFFSET]

	ldr	r1, =VPLL_VAL
	str	r1, [r0, #VPLL_CON_OFFSET]
	
	ldr	r1, =AFC_ON
	str	r1, [r0, #APLL_CON1_OFFSET]
	
	mov	r1, #0x10000
1:	subs	r1, r1, #1
	bne	1b

	/* MPLL software workaround */
	ldr	r1, [r0, #MPLL_CON_OFFSET]
	orr     r1, r1, #(1<<28)
	str	r1, [r0, #MPLL_CON_OFFSET]

	mov	r1, #0x100
1:	subs	r1, r1, #1
	bne	1b

	ldr	r1, [r0, #MPLL_CON_OFFSET]
	and	r1, r1, #(1<<29)
	cmp	r1, #(1<<29)
	bne 	retryloop

	/* H/W lock detect disable */
	ldr	r1, [r0, #MPLL_CON_OFFSET]
	bic     r1, r1, #(1<<28)
	str	r1, [r0, #MPLL_CON_OFFSET]

	ldr	r1, [r0, #CLK_SRC0_OFFSET]
	ldr	r2, =0x10001111
	orr	r1, r1, r2
	str	r1, [r0, #CLK_SRC0_OFFSET]

	/* CLK_DIV6 */
	ldr	r1, [r0, #CLK_DIV6_OFFSET]
	bic	r1, r1, #(0x7<<12)	@; ONENAND_RATIO: 0
	str	r1, [r0, #CLK_DIV6_OFFSET]

	mov	pc, lr


/*
 * uart_asm_init: Initialize UART in asm mode, 115200bps fixed.
 * void uart_asm_init(void)
 */
uart_asm_init:
	/* uart0 gpio setup */
	ldr 	r0, =GPA0CON
	ldr 	r1, [r0]
	bic 	r1, #0xff
	orr 	r1, #0x22
	str 	r1, [r0]

	ldr 	r0, =ELFIN_UART_CONSOLE_BASE

	ldr 	r1, =0x00000003		@ No parityNo parity, No parity, 8-bit
	str 	r1, [r0, #ULCON_OFFSET]

	ldr 	r1, =0x00000305		@  PCLK, Level, Interrupt request or polling mode
	str 	r1, [r0, #UCON_OFFSET]

	ldr 	r1, =0x00000001		@ [trigger level] Tx FIFO-0 byte, Rx FIFO-32 byte,fifo en
	str 	r1, [r0, #UFCON_OFFSET]

	ldr 	r1, =0			@ disable Auto Flow Control
	str 	r1, [r0, #UMCON_OFFSET]

	/* 
	 * baudrate=115200
	 * DIV_VAL = (660000000 / (115200 x 16)) -1 = 34.8
	 * UBRDIVn = 34 ( integer part of DIV_VAL )
	 * (num of 1's in UDIVSLOTn)/16 = 0.8, (num of 1's in UDIVSLOTn) = 13, UDIVSLOT=0xDFDD
	 */
	ldr 	r1, =34
	str 	r1, [r0, #UBRDIV_OFFSET]
	ldr 	r1, =0xDFDD
	str 	r1, [r0, #UDIVSLOT_OFFSET]

	ldr 	r1, =0x0f
	str 	r1, [r0, #UINTM_OFFSET]	@ disable all uart interrupt

	/* print "O" */
	ldr 	r1, =0x4f
	str 	r1, [r0, #UTXH_OFFSET]

	mov	pc, lr


/*
 * tzpc_asm_init: Initialize TZPC
 */
tzpc_asm_init:

	ldr	r0, =ELFIN_TZPC0_BASE
 	mov	r1, #0x0
 	str	r1, [r0]
 	mov	r1, #0xff
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET] 	

 	ldr 	r0, =ELFIN_TZPC1_BASE
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET] 	

 	ldr	r0, =ELFIN_TZPC2_BASE
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET] 

 	ldr	r0, =ELFIN_TZPC3_BASE
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET] 	

 	mov	pc, lr


/*
 * Nand Interface Init for GEC210
 */
nand_asm_init:
#if 0	/* own nand initial driver */
	ldr	r0, =ELFIN_GPIO_BASE
	/* pin CSn2 */
	ldr	r1, [r0,#MP01CON_OFFSET]
	bic	r1, r1, #(0xf<<8)
	orr	r1, r1, #(0x3<<8)
	str	r1, [r0,#MP01CON_OFFSET]
	
	/* pin CLE, ALE, WE, RE, RnB */
	ldr	r1, [r0,#MP03CON_OFFSET]
	ldr	r2, =0xFFFFF
	bic	r1, r1, r2
	ldr	r2, =0x22222
	orr	r1, r1, r2
	str	r1, [r0,#MP03CON_OFFSET]
	
	/* pin DATA[15:0] */
	ldr	r1, =0x22222222
	str	r1, [r0,#MP06CON_OFFSET]
	
	/*
	 *  [1]: When page size is 2K ,5 address cycle
	 */
	ldr	r0, =ELFIN_NAND_BASE
	ldr	r1, =(1<<23) | (0<<12) | (2<<8) | (0<<4) | (1<<1)
	str	r1, [r0,#NFCONF_OFFSET]
	
	ldr	r1, =(0<<8) | (1<<1) | (1<<0)
	str	r1, [r0,#NFCONT_OFFSET]
	 
	mov	pc, lr
#else
	/* Setting GPIO for NAND */
	/* This setting is NAND initialze code at booting time in iROM. */

	ldr	r0, =ELFIN_GPIO_BASE

	ldr	r1, [r0, #MP01CON_OFFSET]
	bic	r1, r1, #(0xf<<8)
	orr	r1, r1, #(0x3<<8)
	str	r1, [r0, #MP01CON_OFFSET]

	ldr	r1, [r0, #MP01PUD_OFFSET]
	bic	r1, r1, #(0x3<<4)
	str	r1, [r0, #MP01PUD_OFFSET]

	ldr	r1, [r0, #MP03CON_OFFSET]
	bic	r1, r1, #0xFFFFFF
	ldr	r2, =0x22222222
	orr	r1, r1, r2
	str	r1, [r0, #MP03CON_OFFSET]

	ldr	r1, [r0, #MP03PUD_OFFSET]
	ldr	r2, =0x3fff
	bic	r1, r1, r2
	str	r1, [r0, #MP03PUD_OFFSET]

	ldr	r0, =ELFIN_NAND_BASE

	ldr	r1, [r0, #NFCONF_OFFSET]
	ldr	r2, =0x777F
	bic	r1, r1, r2
	ldr	r2, =NFCONF_VAL
	orr	r1, r1, r2
	str	r1, [r0, #NFCONF_OFFSET]

	ldr	r1, [r0, #NFCONT_OFFSET]
	ldr	r2, =0x707C7
	bic	r1, r1, r2
	ldr	r2, =NFCONT_VAL
	orr	r1, r1, r2
	str	r1, [r0, #NFCONT_OFFSET]

	ldr	r1, [r0, #NFCONF_OFFSET]
	orr	r1, r1, #0x70
	orr	r1, r1, #0x7700
	str     r1, [r0, #NFCONF_OFFSET]

	ldr	r1, [r0, #NFCONT_OFFSET]
	orr	r1, r1, #0x03
	str     r1, [r0, #NFCONT_OFFSET]

	mov	pc, lr

#endif

