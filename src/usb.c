#include "usb.h"
#include "threads.h"
#include "message/message.h"
#include <stdio.h>
#include <string.h>
#include <device.h>
#include <drivers/uart.h>
#include <zephyr.h>

#include <usb/usb_device.h>
#include <logging/log.h>
LOG_MODULE_REGISTER(cdc_acm_echo, LOG_LEVEL_INF);

void send_over_uart(FifoMessageItem_t* fifoItem) {
	union Serialized_Message tx_data;
	tx_data.message = fifoItem->message;
	uart_tx(fifoItem->dev, tx_data.buffer, sizeof(tx_data.buffer), 1000);
}

static void recieve_to_fifo(const struct device* dev, void* user_data) {
	ARG_UNUSED(user_data);

	/* recive data into buffer after interrupt */
	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		if(uart_irq_rx_ready(dev)) {
			int recv_len;
			union Serialized_Message rx_data;

			recv_len = uart_fifo_read(dev, rx_data.buffer, sizeof(rx_data.buffer));

			/* put into kernel fifo */
			struct FifoMessageItem fifoItem;
			fifoItem.dev = dev;
			fifoItem.message = rx_data.message;
			fifoItem.send = send_over_uart;
			k_fifo_put(&extern_to_validation, &fifoItem);

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

    uart_irq_callback_set(dev, recieve_to_fifo);

    /* enable rx interrupts */
    uart_irq_rx_enable(dev);

	return 0;
}
