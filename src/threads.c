#include <zephyr.h>
#include <sys/printk.h>
#include "threads.h"

/* Thread communication FIFOS */
K_FIFO_DEFINE(communication_to_worker);
K_FIFO_DEFINE(worker_to_communication);

/* CAN Communication */
K_SEM_DEFINE(my_sem, 0, 1);

/* Communication thread poll events setup */
struct k_poll_event events[K_POLL_EVENT_AMOUNT] = {
    K_POLL_EVENT_STATIC_INITIALIZER(K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
                                    K_POLL_MODE_NOTIFY_ONLY,
                                    &can_msgq, 0),
    K_POLL_EVENT_STATIC_INITIALIZER(K_POLL_TYPE_FIFO_DATA_AVAILABLE,
                                    K_POLL_MODE_NOTIFY_ONLY,
                                    &worker_to_communication, 0),
};

/* Thread setup */
K_THREAD_STACK_DEFINE(communication_thread_stack, STACKSIZE);
K_KERNEL_STACK_DEFINE(worker_thread_stack, STACKSIZE);

K_THREAD_DEFINE(c1, STACKSIZE, communication_thread_entry, NULL, NULL, NULL, COMMUNICATION_THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(w1, STACKSIZE, worker_thread_entry, NULL, NULL, NULL,  WORKER_THREAD_PRIORITY, 0, 0);

/* Grant communication thread access to needed kernel objects */
//k_thread_access_grant(c1, &communication_to_worker, &worker_to_communication);
K_HEAP_DEFINE(c1_heap, 2048);
K_THREAD_ACCESS_GRANT(c1, &communication_to_worker, &worker_to_communication);

void worker_thread_entry(void) 
{
    printk("Hello World from Worker Thread! %s\n", CONFIG_BOARD);
    struct FifoCanMessageItem *item;

    while(true) 
    {
        // Wait for new can message from communication thread
        item = k_fifo_get(&communication_to_worker, K_FOREVER);

        // Nachricht verabeiten
        k_msleep(item->message.sleep_in_ms);

        // Nachricht an rückwärts communication thread senden
        reverse_in_place(item->message.text, 10);
        k_fifo_put(&worker_to_communication, &item);
    }
}

void communication_thread_entry(void) 
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
    FifoCanMessageItem_t *work_item;
    int poll = -1;

    while(true)
    {
        poll = k_poll(events, K_POLL_EVENT_AMOUNT, K_FOREVER);

        /* Poll again if an error happened */
        if(poll != 0)
            continue;

        /* We got atleast one event */
        if (events[WORKER_MESSAGE_INCOMING].state == K_POLL_STATE_FIFO_DATA_AVAILABLE) 
            {
                /* Send CAN Message to extern */
            printk("Got message from the worker thread!\n");
            work_item = k_fifo_get(&worker_to_communication, K_FOREVER);
        
        }
        if (events[CAN_MESSAGE_INCOMING].state == K_POLL_STATE_SEM_AVAILABLE) 
        {
            /* Read CAN Message */
            k_msgq_get(&can_msgq, &rx_frame, K_FOREVER);

            /* Construct FIFO Queue Item */

            
                /* Send FIFO Message to worker */

            }

        /* Reset Events for next loop */
        events[0].state = K_POLL_STATE_NOT_READY;
        events[1].state = K_POLL_STATE_NOT_READY;
    }
}

void reverse_in_place(char* text, const size_t length) 
{
    char tmp;

    for (int i = 0; i < length / 2; i++) 
    {
        tmp = text[i];
        text[i] = text[length - (i + 1)];
        text[length - (i + 1)] = tmp;
    }

    return;
}