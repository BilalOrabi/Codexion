---
title: "Deep Dive: Pointers, Memory Architecture, and the &retval Trap in Pthreads"
tags:
  - concurrency
  - pthreads
  - memory-management
  - pointers
  - c-programming
  - deep-dive
  - 42school
created: 2026-09-15
status: active
module: "Phase 1: The Building Blocks Lab"
---

[[MOC|⬅️ Back to Map of Content]] | [[01_Concurrency_Primitives|⬅️ Back to Lesson 01]]

# 🔬 Deep Dive: Memory Architecture, Double Pointers (`void **`), and Dynamic Thread Returns

During the implementation of **Lab 01 (`lab01_threads.c`)**, several subtle, low-level systems programming concepts collided: pointer indirection, stack vs. heap lifetimes, and compiler safety warnings.

This document breaks down the internal mechanics of each lesson learned.

---

## 1. Stack vs. Heap: Where Thread Memory Lives

In multi-threaded C applications:
- **Every thread has its own private Stack** (call frames, local variables, return addresses).
- **All threads share one single Heap** (`malloc`, `free`, global/static data).

```
Virtual Address Space (Single Process)
+--------------------------------------------------------------------------+
| HEAP (Shared by all threads, survives until free())                      |
|  [ malloc block 1 ]  [ malloc block 2 ]  [ malloc block 3 ]              |
+--------------------------------------------------------------------------+
| Thread 1 (main) Stack        | Thread 2 (Worker) Stack                   |
|  - main()'s local variables  |  - worker()'s local variables             |
|    void *retval;             |    int result = 42; (DEAD AFTER RETURN!)  |
+--------------------------------------------------------------------------+
```

### The Return-From-Stack Trap:
```c
void *worker_func(void *arg)
{
    int result = 42;
    return (&result); /* ❌ FATAL BUG */
}
```
When `worker_func` hits `return`:
1. The thread terminates.
2. The operating system unmaps/invalidates that thread's stack.
3. Returning `&result` hands `main()` a **dangling pointer** pointing to dead memory. Dereferencing it yields garbage or segmentation faults.

### The Heap Solution:
```c
void *worker_func(void *arg)
{
    int *exit_code = malloc(sizeof(int));
    if (!exit_code)
        return (NULL);
    *exit_code = 42;
    return ((void *)exit_code); /* ✅ SAFE */
}
```
Because `exit_code` resides on the **heap**, it survives thread termination and remains valid until `free()` is explicitly called.

---

## 2. The Double Pointer Mystery: Why `pthread_join` Requires `void **`

```c
int pthread_join(pthread_t thread, void **retval);
```

Why does `pthread_join` accept a double pointer `void **` instead of a single pointer `void *`?

### The Principle: Pass-by-Reference in C
In C, all function arguments are passed **by value** (copied). If a function needs to modify a caller's variable, it must receive the **address** of that variable:

| Goal | Variable in `main` | Parameter Type | How to Call |
| :--- | :--- | :--- | :--- |
| Modify an `int` | `int x;` | `int *` | `func(&x);` |
| Modify a pointer `void *` | `void *retval;` | `void **` | `pthread_join(tid, &retval);` |

```mermaid
sequenceDiagram
    participant Main as main() Stack
    participant Join as pthread_join()
    participant Heap as Heap Memory

    Note over Main: void *retval = NULL;<br/>Address of retval = 0x7ffd00
    Main->>Join: pthread_join(tid, &retval) [passes 0x7ffd00]
    Note over Join: Copies worker's return address<br/>(e.g., 0x55aa10) into *retval
    Join-->>Main: *(&retval) = 0x55aa10
    Note over Main: retval now points directly to Heap block!
    Main->>Heap: Read *(int *)retval
    Main->>Heap: free(retval)
```

---

## 3. The Fatal Trap: `free(&retval)` vs. `free(retval)`

When finishing `pthread_join`, beginners frequently write:
```c
/* ❌ COMPILER ERROR: -Werror=free-nonheap-object */
free(&retval);
```

### Why this breaks:
1. `retval` is declared as a local variable inside `main()`:
   ```c
   void *retval; /* Resides on main's stack at 0x7fffffffd400 */
   ```
2. After `pthread_join`:
   - `retval` holds the **value** `0x0000000001052ab0` (the Heap address).
   - `&retval` is the **address of the variable on main's stack** (`0x7fffffffd400`).
3. Calling `free(&retval)` asks the memory allocator to free `main()`'s stack memory:
   ```text
   error: 'free' called on unallocated object 'retval' [-Werror=free-nonheap-object]
   ```
4. Calling `free(retval)` passes the heap address stored inside `retval`, freeing the actual allocated integer correctly.

---

## 4. The Loop Overwrite Memory Leak Trap

```c
/* ❌ MEMORY LEAK */
for (int i = 0; i < NUM_THREADS; i++)
{
    pthread_join(threads[i], &retval);
}
```

### Why this leaks memory:
- Each worker allocates an integer using `malloc`.
- Iteration $0$: `retval` receives pointer to Thread 0's memory.
- Iteration $1$: `retval` is **overwritten** with Thread 1's pointer without freeing Thread 0!
- The memory allocated by Threads 0 through $N-2$ is orphaned and leaked permanently.

### The Fix:
Immediately consume and free the result on every iteration:
```c
for (int i = 0; i < NUM_THREADS; i++)
{
    pthread_join(threads[i], &retval);
    printf("exit status: %d\n", *((int *)retval));
    free(retval); /* Frees Thread i's memory before next iteration */
}
```

---

## 5. The Array Pointer Trap: `&tdata` vs. `&tdata[i]`

```c
t_threads_data tdata[NUM_THREADS];

/* ❌ THE TRAP */
pthread_create(&threads[i], NULL, worker, (void *)&tdata);
```

- In C, array variables degrade to pointers to their first element.
- `&tdata` is the address of `tdata[0]`.
- Passing `(void *)&tdata` passes `tdata[0]` to **every single thread**.
- Thread 1, Thread 2, Thread 3, and Thread 4 all access the exact same memory cell!
- To give each thread its own independent workspace:
  ```c
  /* ✅ CORRECT */
  pthread_create(&threads[i], NULL, worker, (void *)&tdata[i]);
  ```

---

## 6. Output Interleaving & Scheduling Non-Determinism

During Lab 01, we observed this terminal output:
```text
[Thread 3] Finished.
[Thread 4] Finished.
work in ms : 13
work in ms : 12
```

### Why this happens:
1. **Thread-safety vs. Atomicity**: While `printf` is internally thread-safe (the buffer doesn't corrupt), **two consecutive `printf` calls are not atomic**.
2. The OS scheduler can preempt Thread 3 between its first and second `printf` to give CPU cycles to Thread 4.
3. Thread 4 prints its first line, and only then does Thread 3 resume to print its second line.
4. **The Lesson for Codexion**: In our final project, coder logs must be protected by a **Logging Mutex** to guarantee that timestamps and coder actions are never split across context switches.
