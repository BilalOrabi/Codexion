*This activity has been created as part of the 42 curriculum by borabi.*

# Codexion

<p align="center">
  <img src="codexion_image.png" alt="Codexion Banner" width="650">
</p>

A high-performance concurrent simulation engine written in **pure C**, modeling multiple coders competing for shared hardware USB dongles in a circular co-working space. The project solves the classic **Dining Philosophers Problem** while introducing real-time systems constraints: hardware cooldowns, deadlock elimination, priority scheduling, and sub-10ms burnout detection.

---

## 📌 Description

In modern distributed and concurrent systems, multiple processes compete for limited shared resources. In `codexion`:
- **$N$ Coders** sit in a circular ring, competing for **$N$ USB Dongles** placed between them.
- To compile quantum code, each coder requires **two dongles simultaneously** (left and right).
- Coders alternate through a continuous lifecycle:
  $$\text{Compile} \longrightarrow \text{Debug} \longrightarrow \text{Refactor} \longrightarrow \text{Compile}$$
- After releasing dongles, each dongle enters a mandatory **hardware cooldown** before anyone can grab it again.
- If a coder fails to start compiling within `time_to_burnout` milliseconds, they **burn out** and the simulation terminates immediately.
- A dedicated **Watchdog Monitor Thread** audits deadlines in real time to catch burnout within **< 10ms**.

**Key Constraints**: Zero global variables, zero data races (`-fsanitize=thread`), zero memory leaks (`valgrind`), and strict 42 Norm compliance.

---

## 🚀 Instructions (Build & Run)

### Compilation
Compile the project with strict 42 compilation flags:
```bash
make        # Compiles the codexion binary
make clean  # Removes object files
make fclean # Removes object files and binary
make re     # Recompiles from scratch
```

### Execution
The program accepts 8 mandatory arguments:
```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Description |
| :--- | :--- |
| `number_of_coders` | Total coders and USB dongles in the ring |
| `time_to_burnout` | Max time (ms) without compiling before a coder burns out |
| `time_to_compile` | Time (ms) spent compiling while holding 2 dongles |
| `time_to_debug` | Time (ms) spent debugging after releasing dongles |
| `time_to_refactor` | Time (ms) spent refactoring before requesting dongles again |
| `number_of_compiles_required` | Target compiles per coder to complete the simulation |
| `dongle_cooldown` | Cooldown period (ms) before a released dongle can be used |
| `scheduler` | Arbitration policy: `fifo` or `edf` (Earliest Deadline First) |

**Example Run**:
```bash
# 4 coders, 410ms to burnout, 200ms compile, 200ms debug, 0ms refactor, 5 rounds, 0ms cooldown, FIFO
./codexion 4 410 200 200 0 5 0 fifo
```

---

## 🛡️ Blocking Cases Handled

### 1. Deadlock Prevention (Breaking Circular Wait)
A classic deadlock occurs if every coder simultaneously grabs their left dongle and waits indefinitely for their right dongle.
- **Solution (Dijkstra's Resource Ordering)**: Coders must always acquire dongles in ascending numerical order (`min(id)` first, then `max(id)`).
- Because the last coder ($N - 1$) is forced to request Dongle 0 before Dongle $N - 1$, circular wait is mathematically broken. A deadlock is impossible.

### 2. Starvation & The "Thundering Herd"
When all threads spawn at $t = 0$, uncoordinated contention can starve specific coders.
- **Solution**: Even-numbered coders start with a microscopic staggered delay, allowing odd-numbered coders to acquire dongles first. This establishes an alternating, harmonious rhythm around the ring.

### 3. Dongle Cooldown Management
After compiling, dongles are stamped with `ready_at_ms = now + cooldown`. The next coder acquiring the lock checks the timestamp and pauses if the cooldown period has not yet passed.

### 4. Real-Time Burnout Detection (< 10ms)
Coders cannot monitor their own death while sleeping or compiling. An independent **Watchdog Monitor Thread** continuously audits all coders' compile timestamps at 1ms intervals, guaranteeing that any burnout is detected and reported within < 10ms.

### 5. Log Serialization
Console logs (`has taken a dongle`, `is compiling`, `burned out`) are protected by a dedicated `log_mutex`. This ensures lines are never interleaved or corrupted across threads, and halts all output immediately when the simulation ends.

---

## 🧵 Thread Synchronization Mechanisms

- **`pthread_mutex_t`**: Protects shared state per dongle and serializes terminal logging.
- **Hybrid Precise Sleep**: Combines `gettimeofday` with micro-sleeps (`usleep`) to avoid standard OS scheduling latency and drift.
- **Custom Min-Heap**: A priority queue built from scratch in pure C (`heap_ops.c`) to power both FIFO and EDF scheduling with deterministic tie-breaking.
- **Atomic Termination**: Thread-safe status query (`is_simulation_ended`) allowing clean, graceful thread joins without zombie processes.

---

## 📚 Resources & AI Usage

### References
- **POSIX Threads Programming** (IEEE Std 1003.1).
- **Operating Systems: Three Easy Pieces (OSTEP)** — Concurrency & Deadlocks.
- **Dijkstra, E. W.** — *Hierarchical Ordering of Sequential Processes*.

### AI Usage Disclosure
In accordance with 42 curriculum standards, AI was used strictly as a **conceptual engineering mentor**:
- **100% Student Code**: All architecture, logic, state machines, and data structures were designed and implemented by hand.
- **Mentor Role**: Theoretical explanations (futexes, Coffman conditions, assembly races), Socratic debugging guidance, and cosmetic 42 Norm formatting.
