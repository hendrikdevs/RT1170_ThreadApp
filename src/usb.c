#include "usb.h"
#include "threads.h"
#include <stdio.h>
#include <string.h>
#include <device.h>
#include <drivers/uart.h>
#include <zephyr.h>
#include <sys/ring_buffer.h>

#include <usb/usb_device.h>
#include <logging/log.h>
LOG_MODULE_REGISTER(cdc_acm_echo, LOG_LEVEL_INF);

#define RING_BUF_SIZE 1024
uint8_t ring_buffer[RING_BUF_SIZE];

struct ring_buf ringbuf;

union Serialize {
	struct Message msg;
	char buffer[sizeof(struct Message)];
};

static void interrupt_handler(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);

	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		if (uart_irq_rx_ready(dev)) {
			int recv_len, rb_len;
			uint8_t buffer[64];
			size_t len = MIN(ring_buf_space_get(&ringbuf),
					 sizeof(buffer));

			recv_len = uart_fifo_read(dev, buffer, len);

			rb_len = ring_buf_put(&ringbuf, buffer, recv_len);
			if (rb_len < recv_len) {
				LOG_ERR("Drop %u bytes\n", recv_len - rb_len);
			}
            LOG_DBG("tty fifo -> ringbuf %d bytes\n", rb_len);

			uart_irq_tx_enable(dev);
		}

		if (uart_irq_tx_ready(dev)) {
			uint8_t buffer[64];
			int rb_len, send_len;

			rb_len = ring_buf_get(&ringbuf, buffer, sizeof(buffer));
			if (!rb_len) {
				LOG_DBG("Ring buffer empty, disable TX IRQ\n");
				uart_irq_tx_disable(dev);
				continue;
			}

			send_len = uart_fifo_fill(dev, buffer, rb_len);
			if (send_len < rb_len) {
				LOG_ERR("Drop %d bytes\n", rb_len - send_len);
			}

			LOG_DBG("ringbuf -> tty fifo %d bytes\n", send_len);
		}
	}
}

static void write_to_fifo(const struct device* dev, void* user_data) {
	ARG_UNUSED(user_data);

	/* recive data into buffer after interrupt */
	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		if(uart_irq_rx_ready(dev)) {
			int recv_len;
			union Serialize tx_data;

			recv_len = uart_fifo_read(dev, tx_data.buffer, sizeof(tx_data.buffer));

			/* put into kernel fifo */
			struct FifoMessageItem fifoItem;
			fifoItem.dev = dev;
			fifoItem.message = tx_data.msg;
			k_fifo_put(&extern_to_communication, &tx_data);

		}
	}
}

int init_usb(void){
    const struct device* dev;
    uint32_t baudrate, dtr = 0u;
    int ret;

    dev = device_get_binding("CDC_ACM_0");
    if(!dev){
        LOG_ERR("CDC ACM device not found\n");
        return -1;
    }

    ret = usb_enable(NULL);
    if(ret != 0){
        LOG_ERR("Failed to enable USB\n");
        return -1;
    }

	ring_buf_init(&ringbuf, sizeof(ring_buffer), ring_buffer);

	LOG_INF("Wait for DTR");
    while(true){
        uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
        if(dtr) {
            break;
        } else {
            k_sleep(K_MSEC(100));
        }
    }
	LOG_INF("DTR set");

	ret = uart_line_ctrl_set(dev, UART_LINE_CTRL_DCD, 1);
	if(ret) {
		LOG_WRN("Failed to set DCD, ret code %d", ret);
	}

	ret = uart_line_ctrl_set(dev, UART_LINE_CTRL_DSR, 1);
	if(ret) {
		LOG_WRN("Failed to set DSR, ret code %d", ret);
	}

    /* wait for the host to do all settings */
    k_busy_wait(1000000);

	ret = uart_line_ctrl_get(dev, UART_LINE_CTRL_BAUD_RATE, &baudrate);
	if (ret) {
        LOG_WRN("Failed to get baudrate, ret code %d\n", ret);
	} else {
        LOG_INF("Baudrate detected: %d\n", baudrate);
	}

    uart_irq_callback_set(dev, interrupt_handler);

    /* enable rx interrupts */
    uart_irq_rx_enable(dev);

	return 0;
}
