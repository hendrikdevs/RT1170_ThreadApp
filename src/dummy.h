#ifndef DUMMY_H
#define DUMMY_H

#include "threads.h"

void dummy_send_out(FifoMessageItem_t*);

void dummy_recieve_to_fifo(void);

#endif /* DUMMY_H */