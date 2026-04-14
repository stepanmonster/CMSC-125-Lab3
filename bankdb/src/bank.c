#include <stdio.h>
#include <pthread.h>
#include "../include/bank.h"

Bank bank;

int initialize_bank() {
    // Initialize the global bank mutex
    pthread_mutex_init(&bank.bank_lock, NULL);
    bank.num_accounts = 0;

    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        // Initialize basic data
        bank.accounts[i].account_id = i;
        bank.accounts[i].balance_centavos = 0;

        // Initialize the read-write lock for each account to avoid transaction threads locking
        if (pthread_rwlock_init(&bank.accounts[i].lock, NULL) != 0) {
            fprintf(stderr, "Failed to initialize rwlock for account %d\n", i);
            return -1; // Indicate failure
        }
    }
    
    return 0; // Success
}