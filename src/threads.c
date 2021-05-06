#include <zephyr.h>
#include <kernel.h>
#include <sys/printk.h>
#include <drivers/uart.h>
#include <app_memory/mem_domain.h>
#include <logging/log.h>

#include "threads.h"

LOG_MODULE_REGISTER(threads);

struct k_mem_domain test_domain;
extern struct k_mem_partition c1_partition;

/* Set up for memory management */
K_APPMEM_PARTITION_DEFINE(c1_partition);
struct k_mem_partition *test_domain_parts[] = {
    &c1_partition
};

/* Fifo for communication between threads and ISR */
K_FIFO_DEFINE(communication_to_worker);
K_FIFO_DEFINE(worker_to_communication);
K_FIFO_DEFINE(extern_to_communication);

/* Communication thread poll events setup */
#define K_POLL_EVENT_AMOUNT 2
#define EXTERN_MESSAGE_INCOMING 0
#define WORKER_MESSAGE_INCOMING 1

K_APP_DMEM(c1_partition) struct k_poll_event events[K_POLL_EVENT_AMOUNT] = {
    K_POLL_EVENT_STATIC_INITIALIZER(K_POLL_TYPE_FIFO_DATA_AVAILABLE,
                                    K_POLL_MODE_NOTIFY_ONLY,
                                    &extern_to_communication, 0),
    K_POLL_EVENT_STATIC_INITIALIZER(K_POLL_TYPE_FIFO_DATA_AVAILABLE,
                                    K_POLL_MODE_NOTIFY_ONLY,
                                    &worker_to_communication, 0),
};

/* Setup Threads */
K_THREAD_DEFINE(c1, STACKSIZE, communication_thread_setup, NULL, NULL, NULL, COMMUNICATION_THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(w1, STACKSIZE, worker_thread_entry, NULL, NULL, NULL,  WORKER_THREAD_PRIORITY, 0, 0);

K_HEAP_DEFINE(usermode_heap, 512);
K_HEAP_DEFINE(message_item_heap, sizeof(FifoMessageItem_t) * 20);


void communication_thread_setup(void)
{
    k_thread_heap_assign(c1, &usermode_heap);
    k_thread_access_grant(c1, &communication_to_worker, &worker_to_communication, &message_item_heap, &events);

    k_thread_user_mode_enter(communication_thread_entry, NULL, NULL, NULL);
}


void communication_thread_entry(void) 
{
    LOG_INF("Communication thread started");

    /* Loop, der die FIFOS checkt ob neue "Ressourcen" (/Nachrichten)
       vorhanden sind, wenn ja diese abarbeiten. */
    FifoMessageItem_t *work_item;
    int poll = -1;

    while(true)
    {
        poll = k_poll(events, K_POLL_EVENT_AMOUNT, K_FOREVER);

        /* Poll again if an error happens */
        if(poll != 0)
            continue;

        /* We got atleast one event */
        if (events[WORKER_MESSAGE_INCOMING].state == K_POLL_STATE_FIFO_DATA_AVAILABLE) {
            LOG_INF("Received message from the worker thread");
            work_item = k_fifo_get(&worker_to_communication, K_FOREVER);

            /* Send message and free it from heap */
            work_item->send(work_item);
            k_heap_free(&message_item_heap, work_item);
            LOG_INF("Send Message to extern and erased item from heap");
        }

        /* ISR recieved a message and put it into it FIFO */
        if(events[EXTERN_MESSAGE_INCOMING].state == K_POLL_STATE_FIFO_DATA_AVAILABLE) {
            LOG_INF("Received message from ISR FIFO");
            work_item = k_fifo_get(&extern_to_communication, K_FOREVER);

            /* validate recieved message (TODO) */

            /* send to worker */
            k_fifo_put(&communication_to_worker, work_item);
        }

        /* Reset Events for next loop */
        events[0].state = K_POLL_STATE_NOT_READY;
        events[1].state = K_POLL_STATE_NOT_READY;
    }
}


void worker_thread_entry(void) 
{
    printk("Hello World from Worker Thread! %s\n", CONFIG_BOARD);
    struct FifoMessageItem *item;

    //k_thread_access_grant(c1, &communication_to_worker, &worker_to_communication);

    while(true) 
    {
        /* Wait for new can message from communication thread */
        item = k_fifo_get(&communication_to_worker, K_FOREVER);

        /* Process Message */
        k_msleep(item->message.sleep_in_ms);
        //reverse_in_place(item->message.text, sizeof(item->message.text));

        /* Send message backwards to the communication thread */
        k_fifo_put(&worker_to_communication, item);
    }
}


void reverse_in_place(char* text, const size_t length) 
{
    /* Counters */
    int i;
    int len = length - 1;

    char tmp;

    for (i = 0; i < length / 2; i++) 
    {
        tmp = text[i];
        text[i] = text[len];
        text[len--] = tmp;
    }

    return;
}
