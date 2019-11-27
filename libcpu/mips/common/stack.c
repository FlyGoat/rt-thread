/*
 * File      : stack.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2008 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016��9��8��     Urey         the first version
 */

#include <rtthread.h>

#include "mips.h"

register rt_uint32_t $GP __asm__ ("$28");

rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter, rt_uint8_t *stack_addr, void *texit)
{
	static rt_uint32_t wSR=0;
	static rt_uint32_t wGP;
	rt_uint8_t *stk;

	struct pt_regs *pt;

	rt_uint32_t i;

	/* Should we keep FPU enabled?  */
	/* Check if local varible have touched */
	if (wSR == 0)
	{
		wSR = read_c0_status();
		wSR &= ~(ST0_EXL | ST0_ERL);
		wSR |= (ST0_IE); /* Set IR EXL and IM3? */

		wGP = $GP;
	}

	/* Get stack aligned */
	stk = (rt_uint8_t *)RT_ALIGN_DOWN((rt_uint32_t)stack_addr, 8);
	stk -= sizeof(struct pt_regs);
	pt =  (struct pt_regs*)stk;

	for (i = 0; i < 8; ++i)
	{
		pt->pad0[i] = 0xdeadbeef;
	}

	/* Fill Stack register numbers */
	for (i = 0; i < 32; ++i)
	{
		pt->regs[i] = 0xdeadbeef;
	}


	pt->regs[REG_SP] = (rt_uint32_t)stk;
	pt->regs[REG_A0] = (rt_uint32_t)parameter;
	pt->regs[REG_GP] = (rt_uint32_t)wGP;
	pt->regs[REG_FP] = (rt_uint32_t)0x0;
	pt->regs[REG_RA] = (rt_uint32_t)texit;

	pt->hi	= 0x0;
	pt->lo	= 0x0;
	pt->cp0_cause	= read_c0_cause();
	pt->cp0_status	= wSR;
	pt->cp0_epc	= (rt_uint32_t)tentry;
	pt->cp0_badvaddr	= 0x00;

	return stk;
}
