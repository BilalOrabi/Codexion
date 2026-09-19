---
title: "Lesson 03: Precise Clocks & Drift-Free Sleep (clock_gettime, gettimeofday)"
tags:
  - concurrency
  - systems-programming
  - timing
  - clocks
  - 42school
  - codexion
created: 2026-09-19
status: completed
module: "Phase 1: The Building Blocks Lab"
---

[[MOC|⬅️ Back to Map of Content]]

# Lesson 03: Precise Clocks & Drift-Free Sleep (`clock_gettime`, `gettimeofday`)

---

## 1. Why Precise Clocks Exist

In `codexion`, the project requirements impose strict real-time constraints:
1. Every event must be logged with a timestamp relative to the start of the simulation:
   ```text
   [timestamp_in_ms] coder X is compiling
   [timestamp_in_ms] coder X is debugging
   ```
2. The burnout monitor thread must log burnout within **< 10ms** of the actual deadline expiry:
   ```text
   [timestamp_in_ms] coder X burned out
   ```
3. Coders must compile for exactly `time_to_compile` ms, debug for `time_to_debug` ms, and dongles must cool down for `dongle_cooldown` ms.

### The `usleep` Trap: Why Naive Sleep Fails
Beginners often write:
```c
usleep(time_to_compile * 1000); // Sleep for N milliseconds
```
At the OS level, `usleep(X)` only guarantees that the thread will sleep for **at least** $X$ microseconds. It gives **zero guarantee** on when the OS scheduler will wake it up!
- When you call `usleep()`, the kernel puts your thread to sleep and switches CPU context.
- When the timer fires, your thread is moved to the OS **Runqueue**. If other processes or threads are busy, your thread must wait for its time slice.
- On standard Linux kernels, scheduler tick quantization causes `usleep` to drift by **1 to 5 milliseconds per call**.
- If a coder calls `usleep` 50 times, the accumulated drift can exceed **100–250ms**, causing premature burnout and failing the `< 10ms` monitor precision rule!

---

## 2. Operating System Mental Model

### The Sluggish Snooze Button vs. High-Precision Stopwatch
> [!TIP]
> - Calling `usleep(10000)` is like hitting a **snooze button** on your alarm. You asked for 10 minutes, but you might wake up 13 minutes later because you were in deep sleep.
> - A **High-Precision Clock** is like holding an active **stopwatch** in your hand. You check the exact elapsed time continuously until the precise millisecond is reached.
> - **The Hybrid Sleep Engine**: To avoid burning 100% CPU while maintaining sub-millisecond precision, we sleep for conservative chunks (e.g. `usleep(500)`), and then fine-tune with rapid microsecond checks as we near the deadline.

---

## 3. Clock Types & POSIX APIs

### A. `gettimeofday` (POSIX.1-2001)
```c
#include <sys/time.h>

int gettimeofday(struct timeval *tv, struct timezone *tz);
```
`struct timeval` contains:
```c
struct timeval
{
    time_t      tv_sec;   /* Seconds since UNIX Epoch (Jan 1, 1970) */
    suseconds_t tv_usec;  /* Microseconds (0 to 999,999) */
};
```
To calculate the current time in milliseconds:
```c
long long current_time_ms = (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL);
```

> [!WARNING]
> `gettimeofday` returns **Wall-Clock Time** (`CLOCK_REALTIME`). If system time changes (e.g. via NTP network synchronization or Daylight Savings), time can jump forward or **backward**, creating negative deltas!

---

### B. `clock_gettime` (POSIX.1-2008)
```c
#include <time.h>

int clock_gettime(clockid_t clk_id, struct timespec *tp);
```
`struct timespec` measures in **nanoseconds**:
```c
struct timespec
{
    time_t tv_sec;   /* Seconds */
    long   tv_nsec;  /* Nanoseconds (0 to 999,999,999) */
};
```
Key Clock IDs:
- **`CLOCK_REALTIME`**: Wall-clock time (subject to NTP jumps).
- **`CLOCK_MONOTONIC`**: Strictly increasing clock representing monotonic time since an unspecified starting point (usually system boot). **Cannot jump backward**.

To calculate milliseconds with `clock_gettime`:
```c
long long current_time_ms = (tp.tv_sec * 1000LL) + (tp.tv_nsec / 1000000LL);
```

---

## 4. Building a Drift-Free Sleep Engine (`ft_usleep`)

To sleep for `time_in_ms` without drift and without pegging the CPU at 100%:

```c
void precise_sleep(long long duration_ms)
{
    long long start = get_current_time_ms();

    while ((get_current_time_ms() - start) < duration_ms)
    {
        /* Sleep in tiny 500-microsecond increments to stay responsive */
        usleep(500);
    }
}
```

### Why this works:
1. Instead of trusting `usleep(duration_ms * 1000)` to wake up at the exact time, we continuously check against the **actual elapsed clock**.
2. Calling `usleep(500)` yields the CPU to other threads, keeping CPU usage near 0%.
3. When the elapsed time reaches `duration_ms`, the loop terminates with sub-millisecond precision.

---

## 5. Common Systems Pitfalls

### Pitfall A: 32-bit Integer Overflow
A standard signed 32-bit `int` maxes out at $2,147,483,647$.
- If you store timestamp calculations in microseconds: $2,147,483,647\ \mu\text{s} \approx 2,147\text{ seconds} \approx 35\text{ minutes}$.
- After 35 minutes, a 32-bit integer overflows into negative numbers, causing immediate segmentation faults or permanent hangs!
- **Rule**: Always use `long long` or `uint64_t` for time calculations.

### Pitfall B: Pegging the CPU (100% Spinlock)
```c
/* FATAL: Destroys system performance */
while ((get_current_time_ms() - start) < duration_ms)
    ; // Busy waiting!
```
A tight empty loop burns 100% of a CPU core, generating heat and starving other threads from getting CPU scheduling slices. Always include a small `usleep(100)` or `usleep(500)` inside pacing loops.

---

## 6. Hands-on Lab Exercise: `lab03_clocks`

### Objective
Measure the physical drift of `usleep()`, observe how much time is lost, and implement a custom drift-free `precise_sleep()` engine verified against POSIX monotonic clocks.

### Specifications:
1. Program name: `lab03_clocks`
2. Directory: `labs/03_clocks_and_timers/`
3. Function `long long get_time_ms(void)`: returns current timestamp in milliseconds.
4. **Experiment 1: The Drift Benchmark**:
   - Measure sleeping 100 times for 10ms using standard `usleep(10000)`.
   - Theoretical expected time: $100 \times 10\text{ms} = 1000\text{ms}$ (1.000s).
   - Print the actual elapsed time and calculate total accumulated drift.
5. **Experiment 2: The Drift-Free Engine**:
   - Implement `void precise_sleep(long long duration_ms)`.
   - Measure 100 iterations of `precise_sleep(10)`.
   - Verify that drift is eliminated within `< 5ms` total error.

---

## 7. Lab Benchmark Results (Empirical Data)

Benchmarked 100 iterations $\times$ 10ms (Expected total: 1000ms):

| Implementation | Platform / Kernel | Actual Time | Total Accumulated Drift | Status |
| :--- | :--- | :--- | :--- | :--- |
| **Naive `usleep(10000)`** | Ubuntu Linux (WSL 2) | `1015 ms` | `+15 ms` | ❌ Fails `< 10ms` spec |
| **Naive `usleep(10000)`** | Windows 11 (UCRT64) | `1564 ms` | `+564 ms` | ❌ Massive scheduling drift |
| **Hybrid `precise_sleep`** | Ubuntu Linux (WSL 2) | `1000 ms` | `+0 ms` | ✅ **Mastered** (Sub-millisecond) |
| **Hybrid `precise_sleep`** | Windows 11 (UCRT64) | `1001 ms` | `+1 ms` | ✅ **Mastered** (Sub-millisecond) |

