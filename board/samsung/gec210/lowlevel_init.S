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

	/* init system clock */
	bl system_clock_init

	/* Memory initialize */
	bl mem_ctrl_asm_init

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
1:
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

	/* set GPIO(GPA) to enable UART */
	@ GPIO setting for UART
	ldr	r0, =ELFIN_GPIO_BASE
	ldr	r1, =0x22222222
	str   	r1, [r0, #GPA0CON_OFFSET]

	ldr     r1, =0x2222
	str     r1, [r0, #GPA1CON_OFFSET]

	ldr	r0, =ELFIN_UART_CONSOLE_BASE		@0xEC000000
	mov	r1, #0x0
	str	r1, [r0, #UFCON_OFFSET]
	str	r1, [r0, #UMCON_OFFSET]

	mov	r1, #0x3
	str	r1, [r0, #ULCON_OFFSET]

	ldr	r1, =0x3c5
	str	r1, [r0, #UCON_OFFSET]

	ldr	r1, =UART_UBRDIV_VAL
	str	r1, [r0, #UBRDIV_OFFSET]

	ldr	r1, =UART_UDIVSLOT_VAL
	str	r1, [r0, #UDIVSLOT_OFFSET]

	ldr	r1, =0x4f4f4f4f
	str	r1, [r0, #UTXH_OFFSET]		@'O'

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


