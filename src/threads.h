#ifndef THREADS_H
#define THREADS_H

#include <can/message.h>
#include <zephyr.h>
#include <kernel/thread.h>

#define STACKSIZE 512
#define COMMUNICATION_THREAD_PRIORITY 2
#define WORKER_THREAD_PRIORITY -2


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
 * @brief The identifier for the FIFO from worker to communication thread
 * 
 */
extern struct k_fifo extern_to_communication;

/**
 * @brief Used to send incoming can messages from the 
 * communication thread to the worker thread.
 * 
 */
struct FifoMessageItem 
{
	void *fifo_reserved;  /* First word reserved for use by FIFO */
	struct Message message;
	const int peripheral_type; /* Tag for the incoming Bus */
	const struct device* dev; /* Device which recieved the Message*/
};
typedef struct FifoMessageItem FifoMessageItem_t;

/** @brief The peripheral_types for the FifoMessageItem struct */
enum peripheral_types {
	can, uart
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

/**
 * @brief Reverses a char array in place.
 * 
 * @param text The pointer to the char array.
 * @param length The length of the char array.
 */
void reverse_in_place(char* text, const size_t length);

/**
 * @brief Send out the Message.
 * All needed Information is contained in the FifoMessageItem.
 * 
 */
void send_via_peripheral(FifoMessageItem_t*);

#endif /* THREADS_H */