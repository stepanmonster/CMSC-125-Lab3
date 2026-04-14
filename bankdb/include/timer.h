#ifndef TIMER_H
#define TIMER_H

#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

// Timer configuration
#define DEFAULT_TICK_INTERVAL_MS 100

// Global simulation clock (shared by all threads)
extern volatile int global_tick;
extern pthread_mutex_t tick_lock;
extern pthread_cond_t tick_changed;
extern bool simulation_running;
extern int tick_interval_ms;

// Function prototypes
void* timer_thread(void* arg);
void wait_until_tick(int target_tick);
int get_current_tick(void);
void init_timer(int tick_interval);
void stop_timer(void);

#endif