#ifndef _MIPSSIM_H__
#define _MIPSSIM_H__

#include <mips.h>

#define ISA_MMIO_BASE       0x1fd00000

#define UART0_ISA_OFF   0x3f8
#define UART0_BASE KSEG1ADDR(ISA_MMIO_BASE + UART0_ISA_OFF)

#define __REG8(addr)		*((volatile unsigned char *)(addr))
#define __REG16(addr)		*((volatile unsigned short *)(addr))
#define __REG32(addr)		*((volatile unsigned int *)(addr))

void rt_hw_timer_handler(void);
void rt_hw_uart_init(void);

#endif