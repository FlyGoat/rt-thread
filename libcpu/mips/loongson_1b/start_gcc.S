/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-05-17     swkyer       first version
 * 2010-09-04     bernard      porting to Jz47xx
 * 2019-07-19     Zhou Yanjie  clean up code
 */

#ifndef __ASSEMBLY__
#define __ASSEMBLY__
#endif

#include "../common/mips_def.h"
#include "../common/stackframe.h"

	.section ".start", "ax"
	.set noreorder

	/* the program entry */
	.globl  _start
_start:
	.set	noreorder
	la	ra, _start

	/* disable interrupt */
	mfc0	t0, CP0_STATUS
	and 	t0, 0xfffffffe	# By default it will be disabled.
	mtc0	t0, CP0_STATUS	# Set CPU to disable interrupt.
	nop

	/* disable cache */
	mfc0	t0, CP0_CONFIG
	and	t0, 0xfffffff8
	or	t0, 0x2		# disable,!default value is not it!
	mtc0	t0, CP0_CONFIG	# Set CPU to disable cache.
	nop

	/* setup stack pointer */
	li	sp, SYSTEM_STACK
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
	.set	reorder
	