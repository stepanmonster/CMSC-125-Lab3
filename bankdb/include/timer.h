#ifndef TIMER_H
#define TIMER_H

#include <pthread.h>

extern volatile int global_tick;
extern pthread_mutex_t tick_lock;
extern pthread_cond_t tick_changed;

// Function prototypes for the timer thread
void* timer_thread_func(void* arg);

#endif