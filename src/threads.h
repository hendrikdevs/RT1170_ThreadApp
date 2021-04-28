#ifndef THREADS_H
#define THREADS_H

#include <can/message.h>

/* Settings for thread creation */
#define STACKSIZE 2048
#define COMMUNICATION_THREAD_PRIORITY 2
#define WORKER_THREAD_PRIORITY -2

/* Defines for the E_POLL Events for the comminication thread */
#define K_POLL_EVENT_AMOUNT 2
#define CAN_MESSAGE_INCOMING 0
#define WORKER_MESSAGE_INCOMING 1


/**
 * @brief Identifier of the communication user mode thread.
 * Gets set by threads.c zephyr macro. Used for receiving and
 * sending can messages, and forwarding messages to the worker
 * thread.
 */
extern const k_tid_t c1;

/**
 * @brief 
 * 
 */
extern struct k_heap c1_heap;

/**
 * @brief 
 * 
 */
extern struct k_msgq can_msgq;

/**
 * @brief Identifier of the worker kernel mode thread.
 * Gets set by threads.c zephyr macro. Used to process
 * the CAN Messages and send back result to the communication
 * thread.
 */
extern const k_tid_t w1;

/**
 * @brief The identifier for the FIFO from communication to worker thread
 * 
 */
extern struct k_fifo communication_to_worker;

/**
 * @brief The identifier for the FIFO from worker to communication thread
 * 
 */
extern struct k_fifo worker_to_communication;

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
typedef struct FifoCanMessageItem FifoCanMessageItem_t;

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

/**
 * @brief Reverses a char array in place.
 * 
 * @param text The pointer to the char array.
 * @param length The length of the char array.
 */
void reverse_in_place(char* text, const size_t length);

#endif /* THREADS_H */