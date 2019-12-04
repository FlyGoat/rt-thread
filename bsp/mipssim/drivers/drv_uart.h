/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-12-04     Jiaxun Yang  Initial version
 */

#ifndef _MIPSSIM_DRV_UART_H__
#define _MIPSSIM_DRV_UART_H__

#include "mipssim.h"

/* UART registers */
#define UART_DAT(base)		__REG8(base + 0x00)
#define UART_IER(base)		__REG8(base + 0x01)
#define UART_IIR(base)		__REG8(base + 0x02)
#define UART_FCR(base)		__REG8(base + 0x02)
#define UART_LCR(base)		__REG8(base + 0x03)
#define UART_MCR(base)		__REG8(base + 0x04)
#define UART_LSR(base)		__REG8(base + 0x05)
#define UART_MSR(base)		__REG8(base + 0x06)

#define UART_LSB(base)		__REG8(base + 0x00)
#define UART_MSB(base)		__REG8(base + 0x01)


/* UART interrupt enable register value */
#define UARTIER_IME		(1 << 3)
#define UARTIER_ILE		(1 << 2) 
#define UARTIER_ITXE	(1 << 1)
#define UARTIER_IRXE	(1 << 0)

/* UART line control register value */
#define UARTLCR_DLAB	(1 << 7)
#define UARTLCR_BCB		(1 << 6)
#define UARTLCR_SPB		(1 << 5)
#define UARTLCR_EPS		(1 << 4)
#define UARTLCR_PE		(1 << 3)
#define UARTLCR_SB		(1 << 2)

/* UART line status register value */
#define UARTLSR_ERROR	(1 << 7)
#define UARTLSR_TE		(1 << 6)
#define UARTLSR_TFE		(1 << 5)
#define UARTLSR_BI		(1 << 4)
#define UARTLSR_FE		(1 << 3)
#define UARTLSR_PE		(1 << 2)
#define UARTLSR_OE		(1 << 1)
#define UARTLSR_DR		(1 << 0)

#endif