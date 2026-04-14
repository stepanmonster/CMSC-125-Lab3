#define BUFFER_POOL_SIZE 5
#include <pthread.h>
#include <semaphore.h>
#include "../include/bank.h"

typedef struct {
    int account_id;
    Account* data;
    bool in_use;
} BufferSlot;

typedef struct {
    BufferSlot slots[BUFFER_POOL_SIZE];
    sem_t empty_slots;
    sem_t full_slots;
    pthread_mutex_t pool_lock;
} BufferPool;