#ifndef LOCK_MGR_H
#define LOCK_MGR_H

#include <pthread.h>
#include <stdbool.h>
#include "bank.h"
#include "transaction.h"

// Deadlock strategy
typedef enum {
    DEADLOCK_PREVENTION,
    DEADLOCK_DETECTION
} DeadlockStrategy;

// For deadlock detection (Strategy B)
typedef struct {
    int tx_id;
    int waiting_for_tx;     // -1 if not waiting
    int waiting_for_account;
} WaitForEntry;

void init_lock_manager(DeadlockStrategy strategy);
bool transfer_with_deadlock_handling(Bank* bank, int from_id, int to_id, int amount_centavos, int tx_id);

// Deadlock prevention (Strategy A)
bool transfer_lock_ordering(Bank* bank, int from_id, int to_id, int amount_centavos);

#endif