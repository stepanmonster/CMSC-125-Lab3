#include <stdio.h>
#include <stdbool.h>

#include "../include/transaction.h"
#include "../include/bank.h"
#include "../include/timer.h"

typedef struct {
    int tx_id;
    int waiting_for_tx;  // -1 if not waiting
    int waiting_for_account;
} WaitForEntry;

WaitForEntry wait_graph[MAX_TRANSACTIONS];
pthread_mutex_t graph_lock;



void* execute_transaction(void* arg) {
    Transaction* tx = (Transaction*)arg;
    
    // Wait until scheduled start time
    wait_until_tick(tx->start_tick);
    
    tx->actual_start = global_tick;
    
    for (int i = 0; i < tx->num_ops; i++) {
        Operation* op = &tx->ops[i];
        
        int tick_before = global_tick;
        
        switch (op->type) {
            case OP_DEPOSIT:
                deposit(op->account_id, op->amount_centavos);
                break;
                
            case OP_WITHDRAW:
                if (!withdraw(op->account_id, op->amount_centavos)) {
                    // Insufficient funds - abort transaction
                    tx->status = TX_ABORTED;
                    return NULL;
                }
                break;
                
            case OP_TRANSFER:
                if (!transfer(op->account_id, op->target_account,
                              op->amount_centavos)) {
                    tx->status = TX_ABORTED;
                    return NULL;
                }
                break;
                
            case OP_BALANCE:
                int balance = get_balance(op->account_id);
                printf("T%d: Account %d balance = PHP %d.%02d\n", 
                       tx->tx_id, op->account_id, 
                       balance / 100, balance % 100);
                break;
        }
        
        tx->wait_ticks += (global_tick - tick_before);
    }
    
    tx->actual_end = global_tick;
    tx->status = TX_COMMITTED;
    return NULL;
}

int get_balance(int account_id) {
    Account* acc = &bank.accounts[account_id];
    
    pthread_rwlock_rdlock(&acc->lock);
    int balance = acc->balance_centavos;
    pthread_rwlock_unlock(&acc->lock);
    
    return balance;
}

void deposit(int account_id, int amount_centavos) {
    Account* acc = &bank.accounts[account_id];
    
    pthread_rwlock_wrlock(&acc->lock);
    acc->balance_centavos += amount_centavos;
    pthread_rwlock_unlock(&acc->lock);
}

bool withdraw(int account_id, int amount_centavos) {
    Account* acc = &bank.accounts[account_id];
    
    pthread_rwlock_wrlock(&acc->lock);
    
    if (acc->balance_centavos < amount_centavos) {
        pthread_rwlock_unlock(&acc->lock);
        return false;  // Insufficient funds
    }
    
    acc->balance_centavos -= amount_centavos;
    pthread_rwlock_unlock(&acc->lock);
    return true;
}

// When a transaction blocks on a lock, record it
void record_wait(int tx_id, int account_id, int holder_tx) {
    pthread_mutex_lock(&graph_lock);
    wait_graph[tx_id].tx_id = tx_id;
    wait_graph[tx_id].waiting_for_tx = holder_tx;
    wait_graph[tx_id].waiting_for_account = account_id;
    pthread_mutex_unlock(&graph_lock);
}

// DFS-based cycle detection
bool has_cycle(int tx_id, bool* visited, bool* rec_stack) {
    visited[tx_id] = true;
    rec_stack[tx_id] = true;
    
    int next = wait_graph[tx_id].waiting_for_tx;
    if (next != -1) {
        if (!visited[next]) {
            if (has_cycle(next, visited, rec_stack)) {
                return true;
            }
        } else if (rec_stack[next]) {
            return true;  // Cycle detected!
        }
    }
    
    rec_stack[tx_id] = false;
    return false;
}

bool detect_deadlock() {
    pthread_mutex_lock(&graph_lock);
    
    bool visited[MAX_TRANSACTIONS] = {false};
    bool rec_stack[MAX_TRANSACTIONS] = {false};
    
    for (int i = 0; i < num_active_transactions; i++) {
        if (!visited[i]) {
            if (has_cycle(i, visited, rec_stack)) {
                pthread_mutex_unlock(&graph_lock);
                return true;
            }
        }
    }
    
    pthread_mutex_unlock(&graph_lock);
    return false;
}

bool transfer(int from_id, int to_id, int amount_centavos) {
    // This is where deadlock can occur!
    // See Part 3 for proper implementation
}
