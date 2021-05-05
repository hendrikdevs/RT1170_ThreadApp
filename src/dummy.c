
#include "threads.h"
#include "message/message.h"
#include "dummy.h"

void dummy_send_out(FifoMessageItem_t* fifoItem) {
    printk("DRVR: Sent out message: %s\n", fifoItem->message.text);
}

void dummy_recieve_to_fifo(void) {
    FifoMessageItem_t* fifoItem = k_heap_alloc(&message_item_heap, sizeof(FifoMessageItem_t), K_NO_WAIT);
    fifoItem->dev = NULL;
    fifoItem->send = dummy_send_out;
    struct Message msg = {0,1000,"hello"};
    fifoItem->message = msg;

    // printk("original: {%d, %d, %s}\n", 
    //     fifoItem->message.priority, 
    //     fifoItem->message.sleep_in_ms, 
    //     fifoItem->message.text
    // );
    // printk("fifoItem: {%d, %d, %s}\n", 
    //     fifoItem->message.priority, 
    //     fifoItem->message.sleep_in_ms, 
    //     fifoItem->message.text
    // );

    k_fifo_put(&extern_to_communication, fifoItem);
    printk("DRVR: \"Recieved\" message and sent into fifo\n");
}