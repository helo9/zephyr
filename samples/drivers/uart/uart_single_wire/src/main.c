/*
 * Copyright (c) 2021 Jonathan Hahn
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include "kernel.h"
#include <device.h>
#include <devicetree.h>
#include <drivers/uart.h>

#define USART2_NODE DT_NODELABEL(usart2)
#define UART4_NODE DT_NODELABEL(uart4)

const struct device *usart2 = DEVICE_DT_GET(USART2_NODE);
const struct device *uart4 = DEVICE_DT_GET(UART4_NODE);

void test_main(void)
{
	unsigned char recv;

	if (!device_is_ready(usart2) || !device_is_ready(uart4)) {
		printk("uart devices not ready");
		return;
	}

	while (true) {
		printk("Loop\n");

		uart_poll_out(usart2, 'c');

		k_busy_wait(1000);

		int ret = uart_poll_in(uart4, &recv);

		if (ret < 0) {
			printk("Recv on uart4 failed with %d\n", ret);
		} else {
			printk("Received %c\n", recv);
		}

		k_sleep(K_MSEC(2000));
	}
}
