---
title: "Lesson 02: Race Conditions & Critical Sections (pthread_mutex_*)"
tags:
  - concurrency
  - systems-programming
  - pthreads
  - mutex
  - race-conditions
  - 42school
  - codexion
created: 2026-09-15
status: completed
module: "Phase 1: The Building Blocks Lab"
---

[[MOC|⬅️ Back to Map of Content]]

# Lesson 02: Race Conditions & Critical Sections (`pthread_mutex_*`)

---

## 1. Why Mutexes Exist

In Lesson 01, we established that threads share the same virtual address space (Heap and Data/BSS segments). 

While sharing memory enables microsecond communication without IPC overhead, it introduces the most notorious bug class in systems engineering: **The Data Race**.

When multiple threads read and write to the same memory location simultaneously without synchronization, execution order is non-deterministic. A simple operation like:
```c
counter++;
```
looks like a single step in C, but at the CPU assembly level, it is broken down into **three distinct instructions**:
1. **Load (Read)**: `mov eax, [counter]` (copy value from RAM into CPU register)
2. **Modify**: `add eax, 1` (increment register)
3. **Store (Write)**: `mov [counter], eax` (write register back to RAM)

### The Assembly Interleaving Disaster:
If Thread A and Thread B both attempt `counter++` when `counter = 0`:

```
Time   Thread A (CPU Core 0)          Thread B (CPU Core 1)         RAM Value (counter)
  |    LOAD counter (eax = 0)                                            0
  v                                  LOAD counter (ebx = 0)              0
       ADD eax, 1    (eax = 1)                                           0
                                     ADD ebx, 1    (ebx = 1)             0
       STORE eax     (writes 1)                                          1
                                     STORE ebx     (writes 1)            1
```
Two increments occurred, but `counter` ended up as **1**, not **2**! One entire write was obliterated.

A **Mutex** (*Mutual Exclusion*) exists to enforce atomicity across critical regions of code.

---

## 2. Operating System Mental Model

### The Single-Occupancy Restroom & Key
> [!TIP]
> - Imagine a shared room with a lockable door (the **Critical Section**).
> - There is **exactly one key** hanging on the wall (the **`pthread_mutex_t`**).
> - When Worker A wants to enter, they grab the key and lock the door from the inside (`pthread_mutex_lock`).
> - While Worker A is inside, Worker B arrives, sees the key missing, and goes to sleep in the waiting line (the OS puts Worker B to sleep off the CPU).
> - When Worker A finishes, they unlock the door and put the key back (`pthread_mutex_unlock`).
> - The OS wakes Worker B up, Worker B grabs the key, and enters.

---

## 3. POSIX Mutex API Mechanics

### 1. Initialization
```c
#include <pthread.h>

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
// Or statically:
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
```
- Allocates OS-level synchronization primitives and futex metadata.
- Attributes (`attr`): Passing `NULL` configures standard default mutex behavior.

### 2. Acquiring the Lock (Entering the Critical Section)
```c
int pthread_mutex_lock(pthread_mutex_t *mutex);
```
- If the mutex is unlocked: Locks it immediately and returns `0`. The calling thread becomes the owner.
- If the mutex is already locked: The kernel suspends the calling thread and places it on a wait queue until the mutex is released.

### 3. Releasing the Lock (Exiting the Critical Section)
```c
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```
- Releases ownership of the lock.
- If other threads are sleeping in the wait queue, the OS scheduler wakes up one of the waiting threads to acquire the lock.

### 4. Destruction
```c
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```
- Frees internal kernel/OS resources associated with the mutex.
- Calling `destroy` on a locked mutex or a mutex that other threads are waiting on produces undefined behavior.

---

## 4. Deep Systems Pitfalls

### Pitfall A: The Unlocked-Lock / Double-Lock Deadlock
```c
/* FATAL: Deadlock */
pthread_mutex_lock(&mutex);
pthread_mutex_lock(&mutex); // Calling thread deadlocks itself waiting for itself!
```
A standard POSIX mutex is non-recursive. If the same thread attempts to lock it twice, it blocks permanently waiting for itself to unlock it.

### Pitfall B: Asymmetric Lock/Unlock in Error Branches
```c
pthread_mutex_lock(&mutex);
if (error_condition)
    return (ERROR); /* LEAKED LOCK: Permanently deadlocks the system! */
pthread_mutex_unlock(&mutex);
```
Every exit path from a function must unlock any held mutexes before returning.

### Pitfall C: Priority Inversion & Lock Contention
Holding a mutex while performing slow operations (like disk I/O, heavy computation, or `sleep()`) starves all other threads and destroys system parallelism. Keep critical sections as small as possible!

---

## 5. Connection to Codexion

In `codexion`:
1. **Shared Dongles**: Coders compete for left and right dongles. Every dongle requires synchronization so two coders never hold the same dongle simultaneously.
2. **Synchronized Output (Logging Mutex)**: In Lesson 01, we saw `printf` lines get interleaved! In `codexion`, a dedicated `log_mutex` ensures that log messages (`X ms coder Y is compiling`, `X ms coder Y is thinking`) are printed atomically with drift-free timestamps.
3. **Shared State & Burnout Flag**: When the monitor detects a coder has burned out, it updates a shared simulation-stopping flag protected by a mutex.

---

## 6. Hands-on Lab Exercise: `lab02_mutexes`

### Objective
Expose an unshielded data race using ThreadSanitizer, observe corrupted state, and then eliminate the race condition completely using a `pthread_mutex_t`.

### Specifications:
1. Program name: `lab02_mutexes`
2. Directory: `labs/02_mutexes_and_critical_sections/`
3. Global or shared heap counter initialized to `0`.
4. Spawn $N = 4$ worker threads.
5. Each thread performs a loop incrementing the shared counter $100,000$ times.
6. **Part 1 (The Broken Version)**: Run without mutex. Observe that the final count is far below the expected $400,000$ and `-fsanitize=thread` detects a data race.
7. **Part 2 (The Thread-Safe Version)**: Protect the increment with `pthread_mutex_lock` and `pthread_mutex_unlock`. Observe exact $400,000$ count and 0 sanitizer warnings.

---

## 7. Deep Dives & Lessons Learned

During the execution of this lab, a critical systems deep-dive was documented:
- 📖 [[Deep_Dive_Mutex_Internals|Deep Dive: Mutex Internals, CPU Assembly, and Linux Futexes]] (Where mutexes live, user-space fast path `LOCK CMPXCHG`, kernel slow path `sys_futex`, cache line bouncing, and comparison matrix)
