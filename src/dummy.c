
#include "threads.h"
#include "message/message.h"
#include "dummy.h"
#include <logging/log.h>

LOG_MODULE_REGISTER(dummy);

void dummy_send_out(FifoMessageItem_t* fifoItem) {
    LOG_INF("DRVR: Sent out message: %s", log_strdup(fifoItem->message.text));
}

void dummy_recieve_to_fifo(void) {

    char text[10] = "helloworld";
    FifoMessageItem_t* fifoItem = createFifoMessageItem(
        0, 1000, text, dummy_send_out, NULL
    );

    k_fifo_put(&extern_to_validation, fifoItem);
    LOG_INF("DRVR: \"Recieved\" message and sent into fifo");
}