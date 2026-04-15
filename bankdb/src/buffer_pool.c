#include <stdio.h>
#include <string.h>
#include "buffer_pool.h"

BufferPool buffer_pool;

void init_buffer_pool(BufferPool *pool)
{
    memset(pool, 0, sizeof(*pool));

    sem_init(&pool->empty_slots, 0, BUFFER_POOL_SIZE);
    sem_init(&pool->full_slots,  0, 0);
    pthread_mutex_init(&pool->pool_lock, NULL);

    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        pool->slots[i].account_id = -1;
        pool->slots[i].in_use     = false;
        pool->slots[i].data       = NULL;
    }
}

/*
 * Producer: load account data into a buffer slot.
 * Blocks if all slots are full (demonstrates bounded-buffer problem).
 */
void load_account(BufferPool *pool, int account_id)
{
    /* Check if already loaded — avoid double-loading same account */
    pthread_mutex_lock(&pool->pool_lock);
    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        if (pool->slots[i].in_use && pool->slots[i].account_id == account_id) {
            pthread_mutex_unlock(&pool->pool_lock);
            return; /* already in buffer */
        }
    }
    pthread_mutex_unlock(&pool->pool_lock);

    /* Try non-blocking first; if pool is full, record the block and wait */
    if (sem_trywait(&pool->empty_slots) != 0) {
        pool->blocked_ops++; /* had to wait — pool was full */
        sem_wait(&pool->empty_slots);
    }

    pthread_mutex_lock(&pool->pool_lock);

    /* Find an empty slot */
    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        if (!pool->slots[i].in_use) {
            pool->slots[i].account_id = account_id;
            pool->slots[i].data       = &bank.accounts[find_account_idx(account_id)];
            pool->slots[i].in_use     = true;
            pool->total_loads++;
            pool->current_usage++;
            if (pool->current_usage > pool->peak_usage)
                pool->peak_usage = pool->current_usage;
            break;
        }
    }

    pthread_mutex_unlock(&pool->pool_lock);
    sem_post(&pool->full_slots);
}

/*
 * Consumer: release the buffer slot for an account.
 */
void unload_account(BufferPool *pool, int account_id)
{
    sem_wait(&pool->full_slots);

    pthread_mutex_lock(&pool->pool_lock);

    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        if (pool->slots[i].in_use && pool->slots[i].account_id == account_id) {
            pool->slots[i].in_use     = false;
            pool->slots[i].account_id = -1;
            pool->slots[i].data       = NULL;
            pool->total_unloads++;
            pool->current_usage--;
            break;
        }
    }

    pthread_mutex_unlock(&pool->pool_lock);
    sem_post(&pool->empty_slots);
}

void print_buffer_pool_stats(BufferPool *pool)
{
    printf("\n=== Buffer Pool Report ===\n");
    printf("Pool size          : %d slots\n", BUFFER_POOL_SIZE);
    printf("Total loads        : %d\n", pool->total_loads);
    printf("Total unloads      : %d\n", pool->total_unloads);
    printf("Peak usage         : %d slots\n", pool->peak_usage);
    printf("Blocked operations : %d\n", pool->blocked_ops);
}