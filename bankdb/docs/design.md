# Design Documentation (design.md)

## 1. Deadlock Strategy Choice

### Chosen Strategy: Deadlock Detection via Wait-For Graph

We will implement **deadlock detection using a wait-for graph with DFS-based cycle detection**.

### Why This Strategy?

We chose detection over prevention for the following reasons:

- It will allow **greater concurrency**, since transactions will not be restricted by a global lock ordering
- It reflects how **real-world database systems** (e.g., DBMS schedulers) handle deadlocks
- It will demonstrate deeper understanding of **dynamic deadlock conditions**
- It will enable the system to **recover from deadlocks rather than avoiding all potential cases**

While prevention simplifies correctness, it can unnecessarily serialize operations. Detection will allow more flexible execution and will only intervene when a deadlock actually occurs.

---

### Wait-For Graph Design

Each transaction will be represented as a node in the wait-for graph.

An edge:
`T_i` → `T_j` means transaction `T_i` is waiting for a resource (account lock) currently held by `T_j`.

The graph will be maintained using a shared structure:
- Each transaction will record:
  - Which transaction it is waiting for
  - Which account it is blocked on

All updates to the graph will be protected by a mutex to ensure consistency.

---

### Cycle Detection Algorithm

We will use **Depth-First Search (DFS)** to detect cycles in the wait-for graph.

Algorithm:
1. Maintain two arrays:
   - `visited[]` → tracks visited nodes
   - `rec_stack[]` → tracks recursion stack
2. For each active transaction:
   - Perform DFS traversal
3. If a node is revisited while still in the recursion stack:
   - A **cycle exists → deadlock detected**

This corresponds directly to the classical cycle detection in directed graphs.

---

### Deadlock Resolution Policy

When a deadlock is detected:
- We will **abort the youngest transaction** in the cycle

#### Rationale:
- Younger transactions will have performed less work → minimizes wasted computation
- Reduces rollback cost
- Common strategy in real systems (e.g., wound-wait / wait-die variations)

After abort:
- Locks held by the aborted transaction will be released
- Other transactions will proceed

---

### Correctness Argument

Deadlock detection will ensure:
- The system may temporarily enter a deadlock state
- However, **all deadlocks will eventually be detected and resolved**

Thus:
- The system will guarantee **progress (no permanent blocking)**
- It will maintain **correctness and consistency of balances**

---

## 2. Buffer Pool Integration

### Strategy Overview

We will implement a **bounded buffer pool using semaphores**, following the producer-consumer model.

### When will accounts be loaded?

Accounts will be **loaded into the buffer pool upon first access within a transaction**.

- Before performing an operation, the transaction will ensure the account is present in the buffer pool
- This corresponds to the "load on demand" strategy

### When will accounts be unloaded?

Accounts will be **unloaded after the transaction completes (commit or abort)**.

- This will ensure the account remains available throughout the transaction's execution
- It will prevent repeated load/unload overhead within a transaction

---

### Behavior When Pool is Full

If the buffer pool is full:
- The transaction will block on: `sem_wait(empty_slots)`

- It will resume only when: `sem_post(empty_slots)`

is triggered by another transaction unloading an account.

This will guarantee:
- No overflow of the fixed buffer size
- Proper synchronization between producers and consumers

---

### Design Justification

#### Correctness
- Will ensure exclusive ownership of buffer slots
- Will prevent race conditions using:
  - Semaphores (capacity control)
  - Mutex (critical section protection)
- Will guarantee that required data is available throughout transaction execution

#### Performance
- Will reduce redundant loads compared to per-operation strategies
- Will avoid overhead of complex eviction policies (e.g., LRU)
- Will provide predictable blocking behavior under contention

#### Trade-offs
- May temporarily hold unused slots until transaction completion
- Slightly lower utilization compared to fine-grained eviction strategies

However, the design will prioritize **simplicity, safety, and determinism**, which are critical in concurrent systems.

---

## 3. Reader-Writer Lock Performance

### Experimental Setup

We will compare:
- `pthread_mutex_t` (exclusive lock)
- `pthread_rwlock_t` (shared read, exclusive write)

Workload to be used:
- `trace_readers.txt` (multiple concurrent BALANCE operations)

---

### Expected Results

| Lock Type | Total Ticks | Throughput (tx/tick) |
|----------|------------|----------------------|
| Mutex    | 8 ticks    | 0.50                 |
| RWLock   | 3 ticks    | 1.33                 |

*(Representative results based on expected behavior of concurrent reads)*

---

### Workload with Greatest Expected Improvement

The largest improvement will occur in:
- **Read-heavy workloads** (e.g., multiple BALANCE operations)

---

### Explanation

With `pthread_mutex_t`:
- All operations (including reads) will be serialized
- Only one thread will access an account at a time

With `pthread_rwlock_t`:
- Multiple threads will be able to hold **read locks simultaneously**
- Only write operations will require exclusive access

Since BALANCE operations do not modify data:
- They will be able to safely execute concurrently
- This will significantly reduce waiting time and improve throughput

---

### Conclusion

Reader-writer locks will:
- Improve scalability under read-heavy workloads
- Reduce contention compared to mutexes
- Be essential for realistic concurrent database behavior

---

## 4. Timer Thread Design

### Purpose of the Timer Thread

The timer thread will maintain a **global logical clock (`global_tick`)** that:
- Controls when transactions begin execution
- Synchronizes all threads using condition variables

---

### Why is it Necessary?

Without a timer thread:
- Transactions would execute immediately upon creation
- There would be **no notion of simulated time**
- Start times (`start_tick`) would be meaningless

The timer thread will enable:
- Controlled scheduling
- Reproducible concurrency scenarios

---

### What Breaks Without It?

If transactions are executed sequentially:
- No overlapping execution will occur
- No lock contention will arise
- Deadlocks will not be able to occur
- Buffer pool blocking will not be observable

This would invalidate:
- Concurrency testing
- Synchronization correctness
- Performance measurements

---

### How It Will Enable True Concurrency

Each transaction will:
- Wait until its scheduled start time using: `pthread_cond_wait()`

The timer thread will:
- Increment `global_tick`
- Wake all waiting threads via: `pthread_cond_broadcast()`

This will guarantee:
- Transactions start at their designated ticks
- True concurrent execution is observable and testable
- Deadlock scenarios and buffer pool contention can be reproduced reliably