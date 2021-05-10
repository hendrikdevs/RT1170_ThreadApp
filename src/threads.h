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
 * @brief Identifier of the validation user mode thread.
 * Gets set by threads.c zephyr macro. Used for receiving
 * messages, and forwarding messages to the worker
 * thread.
 */
extern const k_tid_t v1;

extern struct k_mem_partition v1_partition;

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
extern struct k_heap validation_heap;

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
extern struct k_fifo validation_to_worker;

/**
 * @brief The identifier for the FIFO from worker to communication thread
 * 
 */
extern struct k_fifo extern_to_validation;

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
 * @brief Create a Fifo Message Item object
 * 
 * @param msg The message item
 * @param cc The callback function which sends out the response
 * @param dev The device which recieved the message item
 */
FifoMessageItem_t* createFifoMessageItem(message_t msg, void (*cc)(struct FifoMessageItem*), struct device* dev);

/**
 * @brief 
 * 
 * @param text The pointer to the char array.
 * @param length The length of the char array.
 */
void reverse_in_place(char* text, const size_t length);

#endif /* THREADS_H */