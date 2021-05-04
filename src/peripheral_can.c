#include <logging/log.h>
#include "peripheral_can.h"
#include "threads.h"
#include <device.h>
#include <drivers/can.h>

//LOG_MODULE_REGISTER(peripheral_can, CONFIG_PERIPHERAL_CAN_LOG_LEVEL);

const struct zcan_filter can_standard_filter = {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = 0x1,
        .rtr_mask = 1,
        .id_mask = CAN_STD_ID_MASK
};

const struct device* can_dev;
struct zcan_frame send_frame;

void send(FifoMessageItem_t *item);
void receive(struct zcan_frame *frame, void *arg);

int init_can(void) 
{
    int filter_id;

    can_dev = device_get_binding(CAN_IDENTIFIER);
    if(can_dev == NULL)
        return CAN_UNITIALIZED;

    filter_id = can_attach_isr(can_dev, receive, NULL, &can_standard_filter);
    if (filter_id < 0) {
        //LOG_ERR("Unable to attach can isr [%d]", filter_id);
    }

    return filter_id;
}

/**
 * @brief This function gets called by the can isr when a new message
 * is received.
 * 
 * @param frame 
 * @param arg 
 */
void receive(struct zcan_frame *frame, void *arg)
{
    FifoMessageItem_t* work_item = k_heap_alloc(&message_item_heap, sizeof(FifoMessageItem_t), K_NO_WAIT);
    work_item->dev = can_dev;
    work_item->send = send;
    work_item->message.priority = frame->data[0];
    work_item->message.sleep_in_ms = frame->data[1];

    strncpy(work_item->message.text, frame->data, sizeof(work_item->message.text));

    k_fifo_put(&extern_to_communication, work_item);
}

/**
 * @brief This function gets called when you want to send a message
 * via CAN.
 * 
 * @param item The Message Item that has all the data needed to be send.
 */
void send(FifoMessageItem_t *item)
{
    /* Construct CAN send frame */
    send_frame.dlc = 8;
    send_frame.data[0] = item->message.priority;
    send_frame.data[1] = item->message.sleep_in_ms;
    for(int i = 2; i < 8; i++) 
    {
        send_frame.data[i] = item->message.text[i-2];
    }

    /* Send CAN Message */
    int ret = -1;
    ret = can_send(item->dev, &send_frame, K_MSEC(100), NULL, NULL);

    if (ret != CAN_TX_OK) {
        //LOG_ERR("Sending failed [%d]", ret);
    }
}