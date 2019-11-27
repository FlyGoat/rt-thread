/*
 * File      : cpu.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2010, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-05-17     swkyer       first version
 */
#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "ptrace.h"

#ifndef __ASSEMBLY__
extern rt_uint32_t __ebase_start;

typedef void (* exception_func_t)(struct pt_regs *regs);

extern int rt_hw_exception_init(void);
extern exception_func_t sys_exception_handlers[];
extern void rt_do_mips_cpu_irq(rt_uint32_t ip);
exception_func_t rt_set_except_vector(int n, exception_func_t func);
#endif

#endif /* end of __EXCEPTION_H__ */
