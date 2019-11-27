/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-05-17     swkyer       first version
 * 2010-09-04     bernard      porting to JZ47xx
 * 2019-07-19     Zhou Yanjie  clean up code
 */

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include "../common/mips_def.h"
#include "../common/stackframe.h"
#include "stack.h"

    .section ".start", "ax"
    .set noreorder

    /* the program entry */
    .globl  _start
_start:
    .set    noreorder
    la      ra, _start

    li      t1, 0x00800000
    mtc0    t1, CP0_CAUSE

    /* init cp0 registers. */
    li      t0, 0x0000FC00 /* BEV = 0 and mask all interrupt */
    mtc0    t0, CP0_STATUS

    /* setup stack pointer */
    li      sp, SYSTEM_STACK
    la      gp, _gp

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

    /* copy IRAM section */
    la     t0, _iramcopy
    la     t1, _iramstart
    la     t2, _iramend
_iram_loop:
    lw     t3, 0(t0)
    sw     t3, 0(t1)
    addiu  t1, 4
    bne    t1, t2, _iram_loop
    addiu  t0, 4
    /* clear bss */
    la      t0, __bss_start
    la      t1, __bss_end
_clr_bss_loop:
    sw      zero, 0(t0)
    bne     t0, t1, _clr_bss_loop
    addiu   t0, t0, 4

    /* jump to RT-Thread RTOS */
    jal     rtthread_startup
    nop

    /* restart, never die */
    j       _start
    nop
    .set    reorder

