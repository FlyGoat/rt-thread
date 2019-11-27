/*
 * File      : cpuport.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2011, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date                Author         Notes
 * 2010-07-09     Bernard        first version
 * 2010-09-11     Bernard        add CPU reset implementation
 * 2015-07-06     chinesebear  modified for loongson 1c
 */

#include <rtthread.h>
#include "ls1c.h"

register rt_uint32_t $GP __asm__ ("$28");

/**
 * @addtogroup Loongson LS1B
 */

/*@{*/

/**
 * this function will reset CPU
 *
 */
void rt_hw_cpu_reset(void)
{
	/* open the watch-dog */
	WDT_EN = 0x01; 		/* watch dog enable */
	WDT_TIMER = 0x01;	/* watch dog will be timeout after 1 tick */
	WDT_SET = 0x01;		/* watch dog start */

	rt_kprintf("reboot system...\n");
	while (1);
}

/**
 * this function will shutdown CPU
 *
 */
void rt_hw_cpu_shutdown(void)
{
	rt_kprintf("shutdown...\n");

	while (1);
}

#define cache_op(op,addr)                       \
	    __asm__ __volatile__(                       \
				    "   .set    push                    \n" \
				    "   .set    noreorder               \n" \
				    "   .set    mips3\n\t               \n" \
				    "   cache   %0, %1                  \n" \
				    "   .set    pop                 \n" \
				    :                               \
				    : "i" (op), "R" (*(unsigned char *)(addr)))

#if defined(CONFIG_CPU_LOONGSON2)
#define Hit_Invalidate_I    0x00
#else
#define Hit_Invalidate_I    0x10
#endif
#define Hit_Invalidate_D    0x11
#define CONFIG_SYS_CACHELINE_SIZE   32
#define Hit_Writeback_Inv_D 0x15


void flush_cache(unsigned long start_addr, unsigned long size)
{
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	unsigned long addr = start_addr & ~(lsize - 1); 
	unsigned long aend = (start_addr + size - 1) & ~(lsize - 1); 

	while (1) {
		cache_op(Hit_Writeback_Inv_D, addr);
		cache_op(Hit_Invalidate_I, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}   
}


/*@}*/

