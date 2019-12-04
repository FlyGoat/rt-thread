/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-12-04     Jiaxun Yang  Initial version
 */

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include "../common/mips.h"

    .section ".start", "ax"
    .set noreorder

    /* the program entry */
    .globl  _start
_start:
    .set	noreorder
    la	ra, _start

    /* disable interrupt */
    mtc0	zero, CP0_CAUSE	
    mtc0	zero, CP0_STATUS	# Set CPU to disable interrupt.
    ehb
    /* setup stack pointer */
    la	sp, _system_stack
    la	gp, _gp

    /* clear bss */
    la	t0, __bss_start
    la	t1, __bss_end
_clr_bss_loop:
    sw	zero, 0(t0)
    bne	t0, t1, _clr_bss_loop
    addiu	t0, t0, 4

    /* jump to RT-Thread RTOS */
    jal	rtthread_startup
    nop

    /* restart, never die */
    j	_start
    nop
