#include <zephyr.h>
#include <kernel.h>
#include <drivers/uart.h>
#include <app_memory/mem_domain.h>
#include <logging/log.h>

#include "threads.h"

LOG_MODULE_REGISTER(threads);

/* Private headers */
void worker_thread_entry(void*, void*, void*);
void validation_thread_entry(void*, void*, void*);
void validation_thread_setup(void*, void*, void*);

/* Set up for memory management */
struct k_mem_domain validation_domain;
extern struct k_mem_partition v1_partition;

K_APPMEM_PARTITION_DEFINE(v1_partition);
struct k_mem_partition *validation_domain_parts[] = {
    &v1_partition
};

/* Fifo for communication between threads and ISR */
K_FIFO_DEFINE(validation_to_worker);
K_FIFO_DEFINE(extern_to_validation);

/* Validation thread poll events setup */
#define K_POLL_EVENT_AMOUNT 2
#define EXTERN_MESSAGE_INCOMING 0

K_APP_DMEM(c1_partition) struct k_poll_event events[K_POLL_EVENT_AMOUNT] = {
    K_POLL_EVENT_STATIC_INITIALIZER(K_POLL_TYPE_FIFO_DATA_AVAILABLE,
                                    K_POLL_MODE_NOTIFY_ONLY,
                                    &extern_to_validation, 0)
};

/* Setup Threads */
K_THREAD_DEFINE(v1, STACKSIZE, validation_thread_setup, NULL, NULL, NULL, COMMUNICATION_THREAD_PRIORITY, 0, 0);
K_THREAD_DEFINE(w1, STACKSIZE, worker_thread_entry, NULL, NULL, NULL,  WORKER_THREAD_PRIORITY, 0, 0);

K_HEAP_DEFINE(validation_heap, 512);
K_HEAP_DEFINE(worker_heap, 512);

/* Grant validation thread access to needed kernel objects */
K_APP_DMEM(v1_partition) K_HEAP_DEFINE(message_item_heap, sizeof(FifoMessageItem_t) * 20);
// K_THREAD_ACCESS_GRANT(c1, &communication_to_worker, &worker_to_communication, &message_item_heap, &events, &extern_to_communication);

/** 
 * @brief Setup for the validation thread before it enters usermode.
 */
void validation_thread_setup(void* p1, void* p2, void* p3)
{
    k_thread_heap_assign(v1, &validation_heap);
    k_thread_access_grant(v1, &extern_to_validation, &validation_to_worker, &message_item_heap, &events);
    
    k_mem_domain_init(&validation_domain, ARRAY_SIZE(validation_domain_parts), validation_domain_parts);
    k_mem_domain_add_thread(&validation_domain, v1);

    k_thread_user_mode_enter(validation_thread_entry, NULL, NULL, NULL);
}

/**
 * @brief The main function for a validation thread.
 * The validation thread is running in usermode.
 *
 */
void validation_thread_entry(void* p1, void* p2, void* p3) 
{
    LOG_INF("Validation thread started");

    /* Loop, der die FIFOS checkt ob neue "Ressourcen" (/Nachrichten)
       vorhanden sind, wenn ja diese abarbeiten. */
    FifoMessageItem_t *work_item;
    int poll = -1;

    while(true)
    {
        /* Reset Events for next loop .
         * By doing this here we can reduce the indentation level ins the IFs.
         */
        events[0].state = K_POLL_STATE_NOT_READY;

        /* "Poll" for new event */
        poll = k_poll(events, K_POLL_EVENT_AMOUNT, K_FOREVER);

        /* Poll again if an error happens */
        if(poll != 0)
            continue;

        /* ISR recieved a message and put it into it FIFO */
        if(events[EXTERN_MESSAGE_INCOMING].state == K_POLL_STATE_FIFO_DATA_AVAILABLE) 
        {
            LOG_INF("Received message from ISR FIFO");
            work_item = k_fifo_get(&extern_to_validation, K_MSEC(100));
            if(work_item == NULL) 
                continue;
            
            /* validate recieved message (TODO) */

            /* send to worker */
            k_fifo_alloc_put(&validation_to_worker, work_item);
            LOG_DBG("Put FifoMessageItem into validation_to_worker queue");
        }
    }
}

/**
 * @brief The main function for a worker thread.
 * 
 */
void worker_thread_entry(void* p1, void* p2, void* p3) 
{
    LOG_INF("Worker thread started");

    k_thread_heap_assign(w1, &worker_heap);
    struct FifoMessageItem *item;

    while(true) 
    {
        /* Wait for new can message from validation thread */
        item = k_fifo_get(&validation_to_worker, K_FOREVER);

        /* Process Message */
        k_msleep(item->message.sleep_in_ms);
        //reverse_in_place(item->message.text, sizeof(item->message.text));

        /* Send message and free it from heap */
        item->send(item);
        k_heap_free(&message_item_heap, item);
        LOG_INF("Send Message to extern and erased item from heap");
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

FifoMessageItem_t* createFifoMessageItem(message_t msg, void (*cc)(struct FifoMessageItem*), struct device* dev) {
    FifoMessageItem_t* fifoItem = k_heap_alloc(&message_item_heap, sizeof(FifoMessageItem_t), K_NO_WAIT);
    fifoItem->dev = dev;
    fifoItem->send = cc;
    fifoItem->message = msg;

    return fifoItem;
}