---
title: "Phase 3: Core Data Architecture & Engine Hierarchy"
tags:
  - architecture
  - systems-programming
  - data-structures
  - 42school
  - codexion
created: 2026-09-20
status: in-progress
module: "Phase 3: The Codexion Production Engine"
---

[[MOC|⬅️ Back to Map of Content]]

# Phase 3, Step 2: Core Data Architecture (`t_engine`)

---

## 1. Why Does a Master Context Struct Exist?

The 42 School Codexion subject contains one non-negotiable rule:
> **"Global variables are strictly forbidden!"**

In a multi-threaded system with:
- Up to 200 concurrent coder threads
- 200 shared USB dongles with cooldown timers
- A background real-time burnout monitor thread
- A shared console that must serialize outputs
- A simulation shutdown flag

Threads cannot use global variables (`extern`, static globals at file scope). 
Every single piece of shared state must be allocated cleanly and passed down through **pointer indirection**.

The standard systems architecture pattern for this is the **Master Context / Engine Pattern** (`t_engine`).

---

## 2. Big Picture: The Object Hierarchy

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                                  t_engine                                   │
│  (Master singleton allocated in main stack or heap, zero global variables)  │
├─────────────────────────────────────────────────────────────────────────────┤
│ • t_config           config;                 (Parsed CLI arguments)         │
│ • long long          start_time_ms;          (Simulation baseline clock)    │
│ • int                simulation_stopped;     (1 = someone died or all done) │
│ • pthread_mutex_t    state_mutex;            (Guards simulation_stopped)    │
│ • pthread_mutex_t    log_mutex;              (Serializes printf lines)      │
│ • pthread_t          monitor_thread;         (Burnout watchdog)             │
│ • t_dongle           *dongles;               (Contiguous array of N dongles)│
│ • t_coder            *coders;                (Contiguous array of N coders) │
└─────────────────────────────────────────────────────────────────────────────┘
          │                                              │
          ▼                                              ▼
┌──────────────────────────────┐               ┌──────────────────────────────┐
│           t_dongle           │               │           t_coder            │
├──────────────────────────────┤               ├──────────────────────────────┤
│ • int             id;        │ ◄──────────── │ • int             coder_id;  │
│ • pthread_mutex_t mutex;     │  left_dongle  │ • pthread_t       thread;    │
│ • int             is_taken;  │               │ • long long       last_meal; │
│ • long long       cooldown_at│ ◄──────────── │ • int             compiles;  │
└──────────────────────────────┘  right_dongle │ • t_dongle        *left;     │
                                               │ • t_dongle        *right;    │
                                               │ • t_engine        *engine;   │
                                               └──────────────────────────────┘
```

---

## 3. Mental Model: The Circular Co-Working Table

Imagine a round table with $N$ coders and $N$ dongles:
- Dongle $0$ sits between Coder $0$ and Coder $1$.
- Dongle $1$ sits between Coder $1$ and Coder $2$.
- Dongle $N-1$ sits between Coder $N-1$ and Coder $0$.

```text
                     [Coder 0]
                    /         \
          (Dongle 0)           (Dongle N-1)
                  /             \
            [Coder 1] --- ... --- [Coder N-1]
```

### The Circular Indexing Formula:
For Coder $i$ (from $0$ to $N - 1$):
- **Left Dongle**: `&engine->dongles[i]`
- **Right Dongle**: `&engine->dongles[(i + 1) % config->number_of_coders]`

> [!NOTE]
> The modulo operator `(i + 1) % N` cleanly wraps the last coder's right hand back to Dongle `0`!

---

## 4. Component Deep Dive

### A. The Dongle (`t_dongle`)
Every dongle is a shared resource that requires mutual exclusion and tracking:
```c
typedef struct s_dongle
{
    int             id;
    pthread_mutex_t mutex;
    int             is_in_use;
    long long       ready_at_ms; /* Timestamp when cooldown expires */
}   t_dongle;
```
- When released at time $T$, `ready_at_ms = T + config->dongle_cooldown`.
- No other coder can take it while `current_time < ready_at_ms`.

---

### B. The Coder (`t_coder`)
Each coder thread needs its own private workspace plus pointers to shared resources:
```c
typedef struct s_coder
{
    int             coder_id;
    pthread_t       thread;
    long long       last_compile_start;
    int             compile_count;
    t_dongle        *left_dongle;
    t_dongle        *right_dongle;
    struct s_engine *engine;
}   t_coder;
```
- `engine`: Backlink to the master engine so the coder can access `engine->log_mutex`, `engine->config`, and `engine->state_mutex`.

---

### C. The Master Engine (`t_engine`)
```c
typedef struct s_engine
{
    t_config        config;
    long long       start_time;
    int             simulation_ended;
    pthread_mutex_t state_mutex;
    pthread_mutex_t log_mutex;
    pthread_t       monitor;
    t_dongle        *dongles;
    t_coder         *coders;
}   t_engine;
```

---

## 5. Common Systems Pitfalls in Struct Layouts

### Pitfall 1: Dangling Struct Pointer (`struct s_engine *engine`)
In C, circular references between structs (`t_coder` holding `t_engine *`, and `t_engine` holding `t_coder *`) require a **forward declaration**:
```c
/* Forward declaration */
struct s_engine;

typedef struct s_coder
{
    ...
    struct s_engine *engine;
}   t_coder;
```
Without the forward declaration, the compiler throws an `unknown type name` error.

### Pitfall 2: Output Splicing (The Log Mutex)
In `codexion`, state transitions look like:
```text
[timestamp_in_ms] X is compiling
[timestamp_in_ms] X is debugging
```
If two threads print simultaneously without a dedicated `log_mutex`, the terminal output gets spliced:
`100 1 is c101 2 is compiling` 💥
The automated 42 evaluation tester will immediately fail the run. All prints must be guarded by `engine->log_mutex`.

---

## 6. Conceptual Check Questions

Before you update `codexion.h` with the production structs:

1. **Why does each `t_coder` struct need a backlink pointer (`struct s_engine *engine`)? When a coder wants to print `[time] X is compiling`, which mutex does it need to lock, and where does that mutex live?**
2. **For Coder $i = 4$ in a simulation of $N = 5$ coders (indices 0, 1, 2, 3, 4):**
   - What is the index of their `left_dongle`?
   - Using the formula `(i + 1) % N`, what is the index of their `right_dongle`?
