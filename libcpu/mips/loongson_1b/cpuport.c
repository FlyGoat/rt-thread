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
 * Date           Author       Notes
 * 2010-07-09     Bernard      first version
 * 2010-09-11     Bernard      add CPU reset implementation
 */

#include <rtthread.h>
#include "ls1b.h"

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