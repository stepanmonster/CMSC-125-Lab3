#ifndef METRICS_H
#define METRICS_H

#include <pthread.h>
#include "transaction.h"

typedef struct {
    int total_transactions;
    int committed;
    int aborted;
    int total_ticks;
    long long total_wait_ticks;
    double avg_wait_time;
    double throughput;
    
    // Lock metrics
    int rwlock_read_acquisitions;
    int rwlock_write_acquisitions;
    int mutex_acquisitions;
    long long total_lock_wait_ticks;
    
    // Buffer pool metrics
    int buffer_pool_loads;
    int buffer_pool_unloads;
    int buffer_pool_peak_usage;
    int buffer_pool_full_waits;
} Metrics;

// Function prototypes
void init_metrics(void);
void record_transaction_completion(Transaction* tx);
void record_lock_wait(int wait_ticks, bool is_rwlock);
void calculate_metrics(void);
void print_metrics(void);
void print_transaction_performance(Transaction* transactions, int num);

#endif