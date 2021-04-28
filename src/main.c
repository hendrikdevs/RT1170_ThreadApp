#include <zephyr.h>
#include <sys/printk.h>
#include "threads.h"
#include "usb.h"

const k_timeout_t thread_timeout = K_FOREVER;


void main(void) 
{
    int ret;
    printk("Hello World from main! %s\n", CONFIG_BOARD);
    ret = init_usb();
    if(ret){
        printk("Error while setting up USB\n");
    }

	k_thread_join(w1, thread_timeout);
    k_thread_join(c1, thread_timeout);
	return;
}
