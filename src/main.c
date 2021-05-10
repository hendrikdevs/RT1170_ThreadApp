#include <zephyr.h>
#include <sys/printk.h>
#include <logging/log.h>

#include "threads.h"
#include "usb.h"
#include "peripheral_can.h"
#include "dummy.h"

LOG_MODULE_REGISTER(main);

const k_timeout_t thread_timeout = K_FOREVER;

void main(void) 
{
    LOG_INF("Main thread started");

    #ifdef CONFIG_USE_DUMMY
    dummy_recieve_to_fifo();
    #endif

    #ifdef CONFIG_USE_USB
    /* Intialize USB Peripheral */
    int ret_usb = -1;

    ret_usb = init_usb();
    if(ret_usb == -1){
        LOG_ERR("Error while setting up USB");
    }
    #endif

    #ifdef CONFIG_USE_CAN
    /* Initialize CAN Peripheral */
    int ret_can = CAN_UNITIALIZED;
    
    ret_can = init_can();
    if(ret_can == CAN_UNITIALIZED) {
        LOG_ERR("Error while setting up CAN");
    }
    #endif
}
