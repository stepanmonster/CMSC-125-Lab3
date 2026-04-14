#ifndef BANK_H
#define BANK_H

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_ACCOUNTS 100

typedef struct {
    int account_id;          // Account number
    int balance_centavos;    // Balance in centavos
    pthread_rwlock_t lock;   // Per-account lock
} Account;

typedef struct {
    Account accounts[MAX_ACCOUNTS];
    int num_accounts;
    pthread_mutex_t bank_lock;  // Protects bank metadata
} Bank;

// Function prototypes
void init_bank(Bank* bank, const char* accounts_file);
int get_balance(Bank* bank, int account_id);
bool deposit(Bank* bank, int account_id, int amount_centavos);
bool withdraw(Bank* bank, int account_id, int amount_centavos);
bool transfer(Bank* bank, int from_id, int to_id, int amount_centavos);
long long get_total_balance(Bank* bank);
void print_bank_summary(Bank* bank);

#endif