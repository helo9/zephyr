#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/uart.h>

#define MY_STACK_SIZE 500
#define MY_PRIORITY 5

const uint8_t buf[25] = {
	0x0F, 0xE3, 0x83, 0xDE, 0x49, 0xB6,
	0xC7, 0x0A, 0xF0, 0x81, 0x0F, 0x7C,
	0xE0, 0x03, 0x1F, 0xF8, 0xC0, 0x07,
	0x3E, 0xF0, 0x81, 0x4F, 0xCB, 0x00,
	0x00
};

const struct device *send_uart = DEVICE_DT_GET(DT_NODELABEL(usart2));

void send_handler(struct k_timer *dummy)
{
	uart_tx(send_uart, buf, 25, SYS_FOREVER_MS);
}

K_TIMER_DEFINE(send_timer, send_handler, NULL);

void uart_callback(const struct device *dev, struct uart_event *evt, void *user_data) {}

void receive_task(void *a, void *b, void *c)
{

	const struct device *recv_uart = DEVICE_DT_GET(DT_NODELABEL(uart4));

	while (true) {
		char c;
		int ret = uart_poll_in(recv_uart, &c);

		if (ret == 0) {
			printk("%x", c);
		} else {
			k_sleep(K_SECONDS(1));
		}
	}
}

K_THREAD_DEFINE(receive_id, MY_STACK_SIZE,
				receive_task, NULL, NULL, NULL,
				MY_PRIORITY, 0, 0);

void main(void)
{

	if (!device_is_ready(send_uart)) {
		printk("send uart is not ready!\n");
		return;
	}
	int rc = uart_callback_set(send_uart, uart_callback, NULL);

	if (rc != 0) {
		printk("failed setting uart_callback %d\n", rc);
	}

	k_timer_start(&send_timer, K_SECONDS(1), K_SECONDS(1));
}
