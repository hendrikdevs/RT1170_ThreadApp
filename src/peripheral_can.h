#include <drivers/can.h>
#include "threads.h"

#define CAN_IDENTIFIER "CAN_1"
#define CAN_UNITIALIZED -1

/**
 * @brief Initializes the CAN Connection to be able to send and receive
 * messages.
 * 
 * @return int 0 on success, -1 on error
 */
int init_can(void);

/**
 * @brief This function gets called by the can isr when a new message
 * is received.
 * 
 * @param frame 
 * @param arg 
 */
void receive(struct zcan_frame *frame, void *arg);

/**
 * @brief This function gets called when you want to send a message
 * via CAN.
 * 
 * @param item The Message Item that has all the data needed to be send.
 */
void send(FifoMessageItem_t *item);
