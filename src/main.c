#include <zephyr.h>
#include <sys/printk.h>
#include "threads.h"
#include "usb.h"
#include "peripheral_can.h"
#include "dummy.h"
#include "main.h"

const k_timeout_t thread_timeout = K_FOREVER;

void main(void) 
{
    printk("Hello World from main! %s\n", CONFIG_BOARD);

    #ifdef USE_DUMMY
    dummy_recieve_to_fifo();
    #endif

    #ifdef USE_USB
    /* Intialize USB Peripheral */
    int ret_usb;

    ret_usb = init_usb();
    if(ret_usb){
        printk("Error while setting up USB\n");
    }
    #endif

    #ifdef USE_CAN
    /* Initialize CAN Peripheral */
    int ret_can = -1;
    
    ret_can = init_can();
    if(ret_can == -1) {
        LOG_ERR();
    }
    #endif
}
