
#include "threads.h"
#include "message/message.h"
#include "dummy.h"

void dummy_send_out(FifoMessageItem_t* fifoItem) {
    printk("DRVR: Sent out message: %s\n", fifoItem->message.text);
}

void dummy_recieve_to_fifo(void) {
    FifoMessageItem_t* fifoItem = createFifoMessageItem(
        
    )
    fifoItem->dev = NULL;
    fifoItem->send = dummy_send_out;
    struct Message msg = {0,1000,"hello"};
    fifoItem->message = msg;

    k_fifo_put(&extern_to_validation, fifoItem);
    printk("DRVR: \"Recieved\" message and sent into fifo\n");
}