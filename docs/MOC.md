---
title: "Codexion: Master Map of Content (MOC)"
tags:
  - index
  - moc
  - codexion
  - concurrency
  - systems-programming
created: 2026-09-12
status: active
---

# 🧠 Codexion: Concurrency & Systems Engineering MOC

Welcome to your Obsidian knowledge base for **Codexion** (42 School Concurrency Project). This vault stores deep-dive theoretical foundations, internal OS mechanics, pitfalls, and architectural guides as you progress through each lesson.

> [!IMPORTANT]
> 📖 [[Concepts_To_Revisit|🧠 Master Concepts to Revisit (Phase-by-Phase Matrix)]]  
> *A persistent index tracking every concept, OS trap, and pointer rule mapped directly to where they appear in future phases.*

---

## 🗺️ Master Curriculum Roadmap

### Phase 1: The Building Blocks Lab
Foundational micro-experiments to master POSIX concurrency primitives in isolation before touching the main project.

- [[01_Concurrency_Primitives|01. Concurrency Primitives (`pthread_create`, `pthread_join`)]] ✅
  - [[Deep_Dive_joining|🔍 Deep Dive: What Happens When You Don't Call pthread_join()?]]
  - [[Deep_Dive_Pointers_Stack_Heap_Retval|🔍 Deep Dive: Pointers, Memory Architecture, and the &retval Trap]]
- [[02_Mutexes_and_Critical_Sections|02. Race Conditions & Critical Sections (`pthread_mutex_*`)]] 📍 *(CURRENT)*
- [[03_Clocks_and_Precise_Timers|03. Precise Clocks & Drift-Free Sleep (`clock_gettime`, `gettimeofday`)]]
- [[04_Condition_Variables|04. Condition Variables (`pthread_cond_*`, spurious wakeups)]]
- [[05_Priority_Queue_Heap|05. Min-Heap / Priority Queue from Scratch in C]]

---

### Phase 2: The Prototype Simulator
Small-scale integration sandbox (2–3 coders, dongle cooldowns, thread sanitizers).

- [[Phase2_Prototype_Simulator|Phase 2: Prototype Architecture & Stress Testing]]

---

### Phase 3: The Codexion Main Engine
The complete 42 compliant project implementation.

- [[Project_Architecture|Step 1 & 2: CLI Parsing & Global-Free Engine Struct Architecture]]
- [[Coffman_Conditions_and_Deadlocks|Step 3: Dongle Arbitration & Deadlock Elimination (Coffman Conditions)]]
- [[Scheduler_Algorithms|Step 4: Custom Schedulers (FIFO vs EDF Priority Queue)]]
- [[Coder_Lifecycle_State_Machine|Step 5: Coder State Machine (Compiling, Debugging, Refactoring)]]
- [[Burnout_Monitor_Architecture|Step 6: Real-Time Burnout Monitor (< 10ms Precision)]]
- [[Submission_Audit|Step 7: Final Submission Audit (Norminette, Leaks, Sanitizers)]]
