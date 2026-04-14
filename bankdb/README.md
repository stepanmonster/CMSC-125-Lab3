# CMSC-125 Lab 3 — Concurrent Banking System

## Members

| Name |
|------|
| _Almonia, D. L._ |
| _Niedes, S._ |

---

## Project Overview

A simulated concurrent banking system written in C. It models multiple transactions executing
concurrently against shared bank accounts, using POSIX threads and synchronization primitives
to guarantee correctness under contention.

Core concepts demonstrated:

- Multithreaded transaction execution with a logical simulation clock
- Reader-writer locks for per-account concurrency
- Deadlock detection via a wait-for graph (DFS cycle detection)
- A bounded buffer pool using semaphores (producer-consumer pattern)
- Metrics collection for throughput, lock contention, and wait times

---

## Repository Structure

```
.
├── include/
│   ├── bank.h           # Bank and Account structs
│   ├── buffer_pool.h    # BufferPool struct and API
│   ├── lock_mgr.h       # Deadlock strategy enum and lock manager API
│   ├── metrics.h        # Metrics struct and collection API
│   ├── timer.h          # Global tick and timer thread declarations
│   └── transaction.h    # Transaction, Operation, TxStatus types
│
├── src/
│   ├── bank.c           # Account init and balance operations (stub)
│   ├── buffer_pool.c    # Semaphore-based pool: load/unload account
│   ├── lock_mgr.c       # Lock ordering / deadlock detection (stub)
│   ├── metrics.c        # Metrics recording and printing (stub)
│   ├── timer.c          # Timer thread: increments global_tick each tick
│   ├── transaction.c    # execute_transaction, wait-for graph, DFS cycle detect
│   ├── utils.c          # Trace file parser: parseTransaction, loadInput
│   └── main.c           # Entry point: load trace, spawn threads, print results
│
├── design.md            # Full design rationale and expected results
└── README.md            # This file
```

---

## Compilation

> **Note:** Build system is not yet finalized. Update this section once a Makefile is added.

**Manual (temporary):**
```bash
gcc -Wall -pthread \
    src/main.c src/utils.c src/timer.c src/transaction.c \
    src/bank.c src/buffer_pool.c src/lock_mgr.c src/metrics.c \
    -o bank_sim
```

**Expected (once Makefile is added):**
```bash
make
```

**Run:**
```bash
./bankdb <trace_file>
```

---

## Trace File Format

Each line describes one transaction:

```
T<id> <start_tick> <OPERATION> [args...]
```

Supported operations:

| Operation | Format | Description |
|-----------|--------|-------------|
| `DEPOSIT` | `T1 0 DEPOSIT <acc> <amount>` | Add centavos to account |
| `WITHDRAW` | `T2 1 WITHDRAW <acc> <amount>` | Remove centavos (aborts if insufficient) |
| `TRANSFER` | `T3 2 TRANSFER <from> <to> <amount>` | Move centavos between accounts |
| `BALANCE` | `T4 3 BALANCE <acc>` | Print current balance |

Amounts are in **centavos** (100 centavos = PHP 1.00).

---

## Implemented Features

### Week 1 (Current)

- Trace file parser (`utils.c`) — reads and tokenizes transaction lines into `Transaction` structs
- Transaction data types (`transaction.h`) — `Transaction`, `Operation`, `TxStatus` enums
- Timer thread skeleton (`timer.c`) — increments `global_tick`, broadcasts on `tick_changed`
-  `execute_transaction` skeleton (`transaction.c`) — waits on start tick, dispatches per-operation
- Wait-for graph structure and DFS cycle detection (`transaction.c`)
- uffer pool semaphore skeleton (`buffer_pool.c`) — `load_account` / `unload_account`
- Debug print in `main.c` — validates parsed transactions before thread execution

### Pending

- [ ] `bank.c` — account initialization from file, actual balance mutations
- [ ] `lock_mgr.c` — `transfer_with_deadlock_handling`, `transfer_lock_ordering`
- [ ] `metrics.c` — `record_transaction_completion`, `record_lock_wait`, `print_metrics`
- [ ] Thread spawning in `main.c` — `pthread_create` per transaction, `pthread_join` barrier
- [ ] Timer thread integration in `main.c` — start timer, pass `simulation_running` flag
- [ ] Buffer pool integration into transaction execution
- [ ] Accounts file loading and `bank_lock` initialization
- [ ] Makefile

---

## Design Decisions

See [`design.md`](design.md) for full rationale. Key choices:

**Deadlock handling:** Detection via wait-for graph rather than prevention via lock ordering.
Allows greater concurrency; only intervenes when a deadlock actually occurs. Resolution policy:
abort the youngest transaction in the cycle to minimize wasted work.

**Buffer pool:** Bounded producer-consumer pool using `sem_t` (empty/full semaphores) and a
`pthread_mutex_t` for the critical section. Accounts are loaded on first access and unloaded
after the transaction commits or aborts.

**Locking:** Per-account `pthread_rwlock_t` allows concurrent reads (BALANCE operations) while
serializing writes (DEPOSIT, WITHDRAW, TRANSFER). Expected to significantly improve throughput
on read-heavy workloads.

**Simulation clock:** A dedicated timer thread increments `global_tick` at fixed intervals and
broadcasts on `tick_changed`. Transactions call `wait_until_tick()` before starting, enabling
reproducible concurrency scenarios.

---

## Known Limitations

- `bank.c` is a stub — no account operations are functional yet.
- `lock_mgr.c` and `metrics.c` are empty — deadlock handling and reporting are not wired in.
- `transaction.c` calls `bank`, `get_balance`, `deposit`, etc. as bare functions without a
  `Bank*` pointer — these signatures need to be reconciled with the `bank.h` API before
  threads can execute operations correctly.
- `buffer_pool.h` reuses the `#ifndef BANK_H` guard (copy-paste error) — needs its own guard
  (`#ifndef BUFFER_POOL_H`).
- `TICK_INTERVAL_MS`, `simulation_running`, and `MAX_TRANSACTIONS` are referenced but not yet
  `#define`d or declared in any visible header.
- The `transfer()` function body in `transaction.c` is empty — deadlock-safe transfer is not
  yet implemented.
- No Makefile exists yet.

---

## Progress Log

| Week | Status |
|------|--------|
| Week 1 | Parser, data types, skeletons for timer/transaction/buffer pool, debug main |
| Week 2 | _(fill in)_ |
| Week 3 | _(fill in)_ |