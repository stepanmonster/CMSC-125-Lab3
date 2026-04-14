#include <stdio.h>
#include <pthread.h>

#include "../include/timer.h"

// Global simulation clock (shared by all threads)
volatile int global_tick = 0;
pthread_mutex_t tick_lock;
pthread_cond_t tick_changed;

// Timer thread increments clock every TICK_INTERVAL_MS
void* timer_thread(void* arg) {
    while (simulation_running) {
        pthread_mutex_lock(&tick_lock);
        usleep(TICK_INTERVAL_MS * 1000);  // Sleep to simulate a tick
        global_tick++;
        pthread_cond_broadcast(&tick_changed);  // Wake waiting
        pthread_mutex_unlock(&tick_lock);
    }
    return NULL;
}

// Transactions wait until their start_tick
void wait_until_tick(int target_tick) {
    pthread_mutex_lock(&tick_lock);
    while (global_tick < target_tick) {
        pthread_cond_wait(&tick_changed, &tick_lock);
    }
    pthread_mutex_unlock(&tick_lock);
}
