/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-09-07     Urey         first version
 */

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include "../common/mips.h"

#define SYSTEM_STACK		0x80003fe8

;/*********************************************************************************************************
;  ���
;*********************************************************************************************************/
	.global	rtthread_startup
	.global 	mips_vfp32_init

	.global	_start
    .section ".start", "ax"
    .set noreorder
_start:
    .set    noreorder
    la      ra, _start

    li      t1, 0x00800000
    mtc0    t1, CP0_CAUSE

    /* init cp0 registers. */
    li      t0, 0x1000FC00 /* BEV = 0 and mask all interrupt */
    mtc0    t0, CP0_STATUS

#ifdef __mips_hard_float
	jal 	mips_vfp32_init
	nop
#endif

    /* setup stack pointer */
    li      sp, SYSTEM_STACK
    la      gp, _gp

_cache_init:
    /* init caches, assumes a 4way * 128set * 32byte I/D cache */
    mtc0    zero, CP0_TAGLO  /* TAGLO reg */
    mtc0    zero, CP0_TAGHI  /* TAGHI reg */
    li      t0, 3            /* enable cache for kseg0 accesses */
    mtc0    t0, CP0_CONFIG   /* CONFIG reg */
    la      t0, 0x80000000   /* an idx op should use an unmappable address */
    ori     t1, t0, 0x4000   /* 16kB cache */

_cache_loop:
    cache   0x8, 0(t0)       /* index store icache tag */
    cache   0x9, 0(t0)       /* index store dcache tag */
    bne     t0, t1, _cache_loop
    addiu   t0, t0, 0x20     /* 32 bytes per cache line */
    nop

    /* invalidate BTB */
    mfc0    t0, CP0_CONFIG
    nop
    ori     t0, 2
    mtc0    t0, CP0_CONFIG
    nop


    /* jump to RT-Thread RTOS */
    jal     rtthread_startup
    nop

    /* restart, never die */
    j       _start
    nop
    .set    reorder
