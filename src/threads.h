#ifndef THREADS_H
#define THREADS_H

#include <message/message.h>
#include <zephyr.h>
#include <kernel/thread.h>
#include <app_memory/app_memdomain.h>

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

extern struct k_mem_partition c1_partition;

/**
 * @brief All incoming messages that gets created as
 * FifoMessageItem_t gets allocated on this heap.
 * 
 */
extern struct k_heap message_item_heap;

/**
 * @brief Heap space for the communication (usermode) thread.
 * 
 */
extern struct k_heap usermode_heap;

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
 * @brief Fifo Message Item
 * 
 * Used to send incoming can messages from the 
 * communication thread to the worker thread.
 */
struct FifoMessageItem 
{
	void *fifo_reserved;  /* First word reserved for use by FIFO */
	message_t message;
	void (*send)(struct FifoMessageItem *message_item);  /* Function Pointer to the specific send function (uart, can, etc.) */
	const struct device* dev;  /* Device which recieved the Message*/
} __packed __aligned(4);
typedef struct FifoMessageItem FifoMessageItem_t;

/**
 * @brief The main function for a worker thread.
 * 
 */
void worker_thread_entry(void*, void*, void*);

/**
 * @brief The main function for a communication thread.
 * The communication thread is running in usermode.
 *
 */
void communication_thread_entry(void*, void*, void*);

/** @brief Setup for the Thread before it enters Usermode */
void communication_thread_setup(void*, void*, void*);

/**
 * @brief Reverses a char array in place.
 * 
 * @param text The pointer to the char array.
 * @param length The length of the char array.
 */
void reverse_in_place(char* text, const size_t length);

#endif /* THREADS_H */