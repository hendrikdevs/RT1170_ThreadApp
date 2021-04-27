#ifndef THREADS_H
#define THREADS_H

#include <can/message.h>

#define USER_STACKSIZE 2048
#define COMMUNICATION_THREAD_PRIORITY 2
#define WORKER_THREAD_PRIORITY -2

#define COMMUNICATION_TO_WORKER_FIFO_NAME "communication_to_worker"
#define WORKER_TO_COMMUNICATION_FIFO_NAME "worker_to_communication"

/**
 * @brief Used to send incoming can messages from the 
 * communication thread to the worker thread.
 * 
 */
struct FifoCanMessageItem 
{
	void *fifo_reserved;  /* First word reserved for use by FIFO */
	struct Message message;
};

/**
 * @brief The main function for a worker thread.
 * 
 */
void worker_function(void* arg1, void* arg2, void* arg3);

/**
 * @brief The main function for a communication thread.
 * The communication thread is running in usermode.
 *
 */
void communication_function(void* arg1, void* arg2, void* arg3);

K_FIFO_DEFINE(communication_to_worker);
K_FIFO_DEFINE(worker_to_communication);

K_SEM_DEFINE(my_sem, 0, 1);

struct k_poll_event events[2] = {
    K_POLL_EVENT_STATIC_INITIALIZER(K_POLL_TYPE_SEM_AVAILABLE,
                                    K_POLL_MODE_NOTIFY_ONLY,
                                    &my_sem, 0),
    K_POLL_EVENT_STATIC_INITIALIZER(K_POLL_TYPE_FIFO_DATA_AVAILABLE,
                                    K_POLL_MODE_NOTIFY_ONLY,
                                    &worker_to_communication, 0),
};

K_THREAD_STACK_DEFINE(communication_thread_stack, USER_STACKSIZE);
// K_THREAD_STACK_DEFINE(worker_thread_stack, USER_STACKSIZE);
K_KERNEL_STACK_DEFINE(worker_thread_stack, USER_STACKSIZE);

K_THREAD_DEFINE(c1, USER_STACKSIZE, communication_function, NULL, NULL, NULL, COMMUNICATION_THREAD_PRIORITY, K_USER, 0);
K_THREAD_DEFINE(w1, USER_STACKSIZE, worker_function, NULL, NULL, NULL,  WORKER_THREAD_PRIORITY, 0, 0);


#endif /* THREADS_H */