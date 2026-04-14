#ifndef BANK_H
#define BANK_H

#include <pthread.h>
#include <unistd.h>

#define MAX_ACCOUNTS 100

typedef struct {
    int account_id;
    int balance_centavos;
    pthread_rwlock_t lock;   
} Account;

typedef struct {
    Account accounts[MAX_ACCOUNTS];
    int num_accounts;
    pthread_mutex_t bank_lock;
} Bank;

#endif