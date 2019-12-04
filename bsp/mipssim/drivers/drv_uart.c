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

#include <rthw.h>
#include <rtthread.h>

#include "mipssim.h"
#include "drv_uart.h"

#define RT_UART_RX_BUFFER_SIZE 64

/**
 * @addtogroup mipssim
 */

/*@{*/

struct rt_uart_mipssim
{
    struct rt_device parent;

    rt_uint32_t hw_base;
    rt_uint32_t irq;

    /* buffer for reception */
    rt_uint8_t read_index, save_index;
    rt_uint8_t rx_buffer[RT_UART_RX_BUFFER_SIZE];
} uart_device;

static void rt_uart_irqhandler(int irqno, void *param)
{
    rt_ubase_t level;
    rt_uint8_t isr;
    struct rt_uart_mipssim *uart = &uart_device;

    /* read interrupt status and clear it */
    isr = UART_IIR(uart->hw_base);
    isr = (isr >> 1) & 0x3;

    /* receive data available */
    if (isr & 0x02)
    {
        /* Receive Data Available */
        while (UART_LSR(uart->hw_base) & UARTLSR_DR)
        {
            uart->rx_buffer[uart->save_index] = UART_DAT(uart->hw_base);

            level = rt_hw_interrupt_disable();
            uart->save_index ++;
            if (uart->save_index >= RT_UART_RX_BUFFER_SIZE)
                uart->save_index = 0;
            rt_hw_interrupt_enable(level);
        }

        /* invoke callback */
        if (uart->parent.rx_indicate != RT_NULL)
        {
            rt_size_t length;
            if (uart->read_index > uart->save_index)
                length = RT_UART_RX_BUFFER_SIZE - uart->read_index + uart->save_index;
            else
                length = uart->save_index - uart->read_index;

            uart->parent.rx_indicate(&uart->parent, length);
        }
    }

    return;
}

static rt_err_t rt_uart_init(rt_device_t dev)
{
    rt_uint32_t baud_div;
    struct rt_uart_mipssim *uart = (struct rt_uart_mipssim *)dev;

    RT_ASSERT(uart != RT_NULL);

    /* init UART Hardware */
    UART_IER(uart->hw_base) = 0; /* clear interrupt */
    UART_FCR(uart->hw_base) = 0xc1; /* reset UART Rx/Tx */
    /* set databits, stopbits and parity. (8-bit data, 1 stopbit, no parity) */
    UART_LCR(uart->hw_base) = 0x3;
    UART_MCR(uart->hw_base) = 0x3;
    UART_LSR(uart->hw_base) = 0x60;
    UART_MSR(uart->hw_base) = 0xb0;

    return RT_EOK;
}

static rt_err_t rt_uart_open(rt_device_t dev, rt_uint16_t oflag)
{
    struct rt_uart_mipssim *uart = (struct rt_uart_mipssim *)dev;

    RT_ASSERT(uart != RT_NULL);
    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        /* Enable the UART Interrupt */
        UART_IER(uart->hw_base) |= UARTIER_IRXE;

        /* install interrupt */
        rt_hw_interrupt_install(uart->irq, rt_uart_irqhandler, RT_NULL, "UART");
        rt_hw_interrupt_umask(uart->irq);
    }
    return RT_EOK;
}

static rt_err_t rt_uart_close(rt_device_t dev)
{
    struct rt_uart_mipssim *uart = (struct rt_uart_mipssim *)dev;

    RT_ASSERT(uart != RT_NULL);
    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        /* Disable the UART Interrupt */
        UART_IER(uart->hw_base) &= ~(UARTIER_IRXE);
    }

    return RT_EOK;
}

static rt_size_t rt_uart_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_uint8_t *ptr;
    struct rt_uart_mipssim *uart = (struct rt_uart_mipssim *)dev;

    RT_ASSERT(uart != RT_NULL);

    /* point to buffer */
    ptr = (rt_uint8_t *)buffer;
    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        while (size)
        {
            /* interrupt receive */
            rt_base_t level;

            /* disable interrupt */
            level = rt_hw_interrupt_disable();
            if (uart->read_index != uart->save_index)
            {
                *ptr = uart->rx_buffer[uart->read_index];

                uart->read_index ++;
                if (uart->read_index >= RT_UART_RX_BUFFER_SIZE)
                    uart->read_index = 0;
            }
            else
            {
                /* no data in rx buffer */

                /* enable interrupt */
                rt_hw_interrupt_enable(level);
                break;
            }

            /* enable interrupt */
            rt_hw_interrupt_enable(level);

            ptr ++;
            size --;
        }

        return (rt_uint32_t)ptr - (rt_uint32_t)buffer;
    }

    return 0;
}

static rt_size_t rt_uart_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    char *ptr;
    struct rt_uart_mipssim *uart = (struct rt_uart_mipssim *)dev;

    RT_ASSERT(uart != RT_NULL);

    ptr = (char *)buffer;

    if (dev->flag & RT_DEVICE_FLAG_STREAM)
    {
        /* stream mode */
        while (size)
        {
            if (*ptr == '\n')
            {
                /* FIFO status, contain valid data */
                while (!(UART_LSR(uart->hw_base) & (UARTLSR_TE | UARTLSR_TFE)));
                /* write data */
                UART_DAT(uart->hw_base) = '\r';
            }

            /* FIFO status, contain valid data */
            while (!(UART_LSR(uart->hw_base) & (UARTLSR_TE | UARTLSR_TFE)));
            /* write data */
            UART_DAT(uart->hw_base) = *ptr;

            ptr ++;
            size --;
        }
    }
    else
    {
        while (size != 0)
        {
            /* FIFO status, contain valid data */
            while (!(UART_LSR(uart->hw_base) & (UARTLSR_TE | UARTLSR_TFE)));

            /* write data */
            UART_DAT(uart->hw_base) = *ptr;

            ptr++;
            size--;
        }
    }

    return (rt_size_t)ptr - (rt_size_t)buffer;
}

void rt_hw_uart_init(void)
{
    struct rt_uart_mipssim *uart;

    /* get uart device */
    uart = &uart_device;

    /* device initialization */
    uart->parent.type = RT_Device_Class_Char;
    rt_memset(uart->rx_buffer, 0, sizeof(uart->rx_buffer));
    uart->read_index = uart->save_index = 0;

    uart->hw_base = UART0_BASE;
    uart->irq = 4;

    /* device interface */
    uart->parent.init       = rt_uart_init;
    uart->parent.open       = rt_uart_open;
    uart->parent.close      = rt_uart_close;
    uart->parent.read       = rt_uart_read;
    uart->parent.write      = rt_uart_write;
    uart->parent.control    = RT_NULL;
    uart->parent.user_data  = RT_NULL;

    rt_device_register(&uart->parent, "uart",
                        RT_DEVICE_FLAG_RDWR |
                        RT_DEVICE_FLAG_STREAM |
                        RT_DEVICE_FLAG_INT_RX);
}

/*@}*/
