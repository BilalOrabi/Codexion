---
title: "Deep Dive: CPU Cycles, Kernel Sleep, and Linux Futexes"
tags:
  - hardware
  - linux
  - kernel
  - futex
  - scheduler
  - cpu
created: 2026-09-19
status: completed
module: "Phase 1: The Building Blocks Lab"
---

[[MOC|⬅️ Back to Map of Content]]

# 🔍 Deep Dive: CPU Cycles, Kernel Sleep, and Linux Futexes

---

## 1. The Hardware Reality: How a CPU Core "Waits"

A CPU core running at **4.0 GHz** executes **4 billion clock cycles per second**. The silicon hardware itself cannot "pause" in user space; it is an instruction-consuming engine that continually:
1. **Fetches** instructions at the address pointed to by the Instruction Pointer (`RIP`/`EIP`).
2. **Decodes** opcodes.
3. **Executes** operations in arithmetic logic units (ALUs) and registers.

### The 100% CPU Spinlock (Busy-Waiting)
When a program polls a condition:
```c
while (pizza_slices == 0)
    ; // Busy wait
```
The compiler emits a tight loop:
```assembly
.L1:
    mov eax, [pizza_slices]   ; Load value from L1/L2 cache
    test eax, eax             ; Compare with 0
    jz .L1                    ; Jump back if zero
```
- The CPU executes these instructions billions of times per second.
- Transistors switch state continuously, pulling peak wattage (e.g. 30W–65W+ per core) and generating heat.
- The OS reports **100% CPU utilization** for that core because 100% of the core's clock cycles were occupied by non-idle thread instructions.

---

## 2. Kernel Sleep (0% CPU)

The Operating System Kernel manages hardware through its **Scheduler**.

### The Scheduler's Two Primary Queues
1. **The Runqueue**: A list of threads in `TASK_RUNNING` state that are currently executing on a core or waiting for their next CPU timeslice.
2. **The Waitqueue**: A list of threads in `TASK_INTERRUPTIBLE` or `TASK_UNINTERRUPTIBLE` (sleeping/blocked) waiting for an external hardware interrupt or synchronization event.

```text
┌──────────────────────────────────────┐     ┌──────────────────────────────────────┐
│         RUNQUEUE (Active)            │     │         WAITQUEUE (Blocked)          │
├──────────────────────────────────────┤     ├──────────────────────────────────────┤
│ • Thread #1042 (VS Code)             │     │ • Thread #1055 (lab04_worker) 💤     │
│ • Thread #1043 (Compiler cc)         │     │   (Wait on futex address 0x7fff...)  │
│ • Thread #1050 (Terminal bash)       │     │                                      │
└──────────────────────────────────────┘     └──────────────────────────────────────┘
```

### What Happens During `pthread_cond_wait()`
1. **`sys_futex(..., FUTEX_WAIT, ...)` System Call**:
   The thread voluntarily relinquishes the CPU and issues a trap to kernel mode.
2. **Saving the Register Context**:
   The kernel saves the calling thread's register state (`RIP`, `RSP`, general-purpose registers, floating-point state) into its Process Control Block / Thread Control Block (`struct task_struct` in Linux).
3. **Queue Migration**:
   The kernel scheduler removes the thread from the **Runqueue** and places it onto the condition variable's **Waitqueue**.
4. **Context Switch**:
   The scheduler selects another ready thread from the Runqueue and switches CPU registers to execute it.
5. **The `HLT` (Halt) Instruction**:
   If no other runnable threads exist on the entire system, the kernel executes the x86 `hlt` instruction.
   - `HLT` stops the CPU instruction clock pipeline at the silicon hardware level.
   - The core enters low-power processor states (C-states: C1, C3, C6).
   - Dynamic power consumption drops to near zero.
   - CPU utilization reports **0%**.

---

## 3. The Wake-Up Sequence (`pthread_cond_signal`)

When another thread executes:
```c
pthread_mutex_lock(&lock);
pizza_slices = 1;
pthread_cond_signal(&cond);
pthread_mutex_unlock(&lock);
```

1. **`sys_futex(..., FUTEX_WAKE, 1)`**:
   The signaling thread notifies the kernel that one sleeping waiter should be awakened.
2. **Kernel Queue Migration**:
   The kernel locates the sleeping thread in the Waitqueue and migrates it back to the **Runqueue** (`TASK_RUNNING`).
3. **Re-acquiring the Mutex**:
   The awakened thread cannot resume user code immediately. Before `pthread_cond_wait()` returns, it must compete for and acquire the associated `pthread_mutex_t`.
4. **Resuming Execution**:
   Once the lock is re-acquired, the kernel restores the thread's saved registers (`RIP`, `RSP`), and the thread resumes execution as if `pthread_cond_wait()` was a simple function call.

---

## 4. Architectural Summary

| Dimension | Busy-Waiting / Polling | Futex-Based Kernel Sleep (`cond_wait`) |
| :--- | :--- | :--- |
| **CPU Core Activity** | Redlining instruction pipeline (`cmp`, `jz`) | Core runs other processes or executes `hlt` |
| **OS State** | `TASK_RUNNING` (on Runqueue) | `TASK_UNINTERRUPTIBLE` (on Waitqueue) |
| **CPU Usage** | **100%** on active core | **0%** |
| **Thermal / Power** | High heat, high power draw | Cool, minimal power |
| **42 Evaluation** | Immediate failure (uncontrolled spinlock) | Required standard for event synchronization |
