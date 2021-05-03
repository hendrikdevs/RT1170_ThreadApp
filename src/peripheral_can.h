#include <drivers/can.h>
#include "threads.h"

#define CAN_IDENTIFIER "CAN_2"
#define CAN_UNITIALIZED -1

/**
 * @brief Initializes the CAN Connection to be able to send and receive
 * messages.
 * 
 * @return int 0 on success, CAN_UNITIALIZED on error
 */
int init_can(void);