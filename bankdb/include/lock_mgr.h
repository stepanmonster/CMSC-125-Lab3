#ifndef LOCK_MGR_H
#define LOCK_MGR_H

#include <stdbool.h>

typedef enum {
    DEADLOCK_PREVENTION,
    DEADLOCK_DETECTION
} DeadlockStrategy;

extern DeadlockStrategy deadlock_strategy;

void lock_mgr_init(DeadlockStrategy strategy);

/*
 * transfer() implementations are in bank.c and call these helpers
 * depending on the chosen strategy.
 */
bool transfer_prevention(int from_id, int to_id, int amount_centavos);
bool transfer_detection(int from_id, int to_id, int amount_centavos, int tx_id);

/* Wait-for graph (detection only) */
void record_wait(int tx_id, int account_id, int holder_tx);
void clear_wait(int tx_id);
bool detect_deadlock(void);
void resolve_deadlock(void);

#endif /* LOCK_MGR_H */