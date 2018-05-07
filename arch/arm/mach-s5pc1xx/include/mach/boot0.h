/*
 * Copyright 2018 Lucifer Zhu, LuciferZhu@yeah.net.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/* BOOT0 header information */
	.word 0x2000		@ indicate BL1 size = 8KB
	.word 0x0
	.word 0x0
	.word 0x0
_start:
	ARM_VECTORS
