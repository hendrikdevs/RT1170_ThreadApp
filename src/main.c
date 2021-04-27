#include <zephyr.h>
#include <sys/printk.h>
#include "threads.h"

const k_timeout_t thread_timeout = K_FOREVER;

void main(void) 
{
    printk("Hello World from main! %s\n", CONFIG_BOARD);
	k_thread_join(w1, thread_timeout);
    k_thread_join(c1, thread_timeout);
	return;
}
