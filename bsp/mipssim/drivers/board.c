/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-12-04     Jiaxun Yang  Initial version
 */

/**
 * @addtogroup mipssim
 */

/*@{*/

#include <rtthread.h>
#include <rthw.h>

#include "mips_regs.h"
#include "exception.h"
#include "drv_uart.h"

#define CPU_HZ	(100 * 1000 * 1000)
#define RT_HW_HEAP_END	(0x80000000 + 64 * 1024 * 1024)

extern unsigned char __bss_end;

/**
 * this function will reset CPU
 *
 */
void rt_hw_cpu_reset(void)
{
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


/**
 * This is the timer interrupt service routine.
 */
void rt_hw_timer_handler(void)
{
    unsigned int count;

    count = read_c0_compare();
    write_c0_compare(count);
    write_c0_count(0);
    /* increase a OS tick */
    rt_tick_increase();
}

/**
 * This function will initial OS timer
 */
void rt_hw_timer_init(void)
{
    write_c0_compare(CPU_HZ/2/RT_TICK_PER_SECOND);
    write_c0_count(0);
    mips_unmask_cpu_irq(7);
}


/**
 * init hardware FPU
 */
#ifdef RT_USING_FPU
void rt_hw_fpu_init(void)
{
    rt_uint32_t c0_status = 0;
    rt_uint32_t c1_status = 0;

    /* Enable CP1 */
    c0_status = read_c0_status();
    c0_status |= (ST0_CU1 | ST0_FR);
    write_c0_status(c0_status);

    /* Config FCSR */
    c1_status = read_32bit_cp1_register(CP1_STATUS);
    c1_status |= (FPU_CSR_FS | FPU_CSR_F0 | FPU_CSR_FN);    // set FS, FO, FN
    c1_status &= ~(FPU_CSR_ALL_E);                          // disable exception
    c1_status = (c1_status & (~FPU_CSR_RM)) | FPU_CSR_RN;   // set RN
    write_32bit_cp1_register(CP1_STATUS, c1_status);

    return;
}
#endif

/**
 * Board level initialization
 */
void rt_hw_board_init(void)
{
    rt_hw_exception_init();

    /* init hardware interrupt */
    rt_hw_interrupt_init();

    #ifdef RT_USING_FPU
    /* init hardware fpu */
    rt_hw_fpu_init();
    #endif

#ifdef RT_USING_SERIAL
    /* init hardware UART device */
    rt_hw_uart_init();
    /* set console device */
    rt_console_set_device("uart");
#endif

#ifdef RT_USING_HEAP
    rt_system_heap_init((void*)&__bss_end, (void*)RT_HW_HEAP_END);
#endif

    /* init operating system timer */
    rt_hw_timer_init();


#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

    rt_kprintf("Current SR: 0x%08x\n", read_c0_status());

}

/*@}*/
