#include <zephyr.h>
#include <sys/printk.h>
#include "threads.h"

void worker_function(void* arg1, void* arg2, void* arg3) 
{
    printk("Hello World from Worker Thread! %s\n", CONFIG_BOARD);

    while(true) 
    {
        // Wait for new can message from communication thread
        struct FifoCanMessageItem *item;
        item = k_fifo_get(COMMUNICATION_TO_WORKER_FIFO_NAME, K_FOREVER);

        // Nachricht verabeiten


        // Nachricht an communication Thread senden

    }


}

void communication_function(void* arg1, void* arg2, void* arg3) 
{
    printk("Hello World from Communication Thread! %s\n", CONFIG_BOARD);

    /* Semaphor die Anzeigt wie CAN Nachrichten vom externen Sender
     * vorhanden sind
    */

    /* Loop, der die Semaphoren checkt ob neue "Ressourcen" (/Nachrichten)
     * vorhanden sind, wenn ja diese abarbeiten.
     * Erst Nachrichten raussenden, bevor neue angenommen werden, 
     * um Memory usage gering zu halten.
    */

    int poll = -1;
    while(true)
    {
        poll = k_poll(events, 2, K_FOREVER);
        if (events[1].state == K_POLL_STATE_FIFO_DATA_AVAILABLE) {
            // Send CAN Message to extern
            
        } else if (events[0].state == K_POLL_STATE_SEM_AVAILABLE) {
            // Send FIFO Message to worker

        }

        events[0].state = K_POLL_STATE_NOT_READY;
        events[1].state = K_POLL_STATE_NOT_READY;
    }


}