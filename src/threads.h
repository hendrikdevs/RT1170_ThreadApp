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
void worker_thread_entry(void);

/**
 * @brief The main function for a communication thread.
 * The communication thread is running in usermode.
 *
 */
void communication_thread_entry(void);

extern const k_tid_t c1;
extern const k_tid_t w1;

#endif /* THREADS_H */