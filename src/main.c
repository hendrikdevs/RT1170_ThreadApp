#include <zephyr.h>
#include <sys/printk.h>
#include "threads.h"
#include "usb.h"
#include "peripheral_can.h"
#include "dummy.h"

const k_timeout_t thread_timeout = K_FOREVER;

void main(void) 
{
    printk("Hello World from main! %s\n", CONFIG_BOARD);

    dummy_recieve_to_fifo();

    /* Intialize USB Peripheral */
    int ret_usb;

    //ret_usb = init_usb();
    if(ret_usb){
        printk("Error while setting up USB\n");
    }

    /* Initialize CAN Peripheral */
    int ret_can = -1;
    
    //ret_can = init_can();
    if(ret_can == -1) {
        // LOG_ERR();
    }
}
