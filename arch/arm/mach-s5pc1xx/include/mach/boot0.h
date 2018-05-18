/*
 * Copyright 2018 Lucifer Zhu, LuciferZhu@yeah.net.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/* BOOT0 header information */
	.word COPY_BL1_SIZE		@ indicate BL1 size for irom
	.word 0x0
	.word 0x0
	.word 0x0
_start:
	ARM_VECTORS
