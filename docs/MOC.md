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

---

## 🗺️ Master Curriculum Roadmap

### Phase 1: The Building Blocks Lab
Foundational micro-experiments to master POSIX concurrency primitives in isolation before touching the main project.

- [[01_Concurrency_Primitives|01. Concurrency Primitives (`pthread_create`, `pthread_join`)]] 📍 *(CURRENT)*
  - [[Deep_Dive_Without_Pthread_Join|🔍 Deep Dive: What Happens When You Don't Call pthread_join()?]]
- [[02_Mutexes_and_Critical_Sections|02. Race Conditions & Critical Sections (`pthread_mutex_*`)]]
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

- [[Project_Architecture|Codexion System Architecture & State Machine]]
- [[Coffman_Conditions_and_Deadlocks|Deadlock Elimination & Coffman Conditions]]
- [[Scheduler_Algorithms|Schedulers: FIFO vs Earliest Deadline First (EDF)]]
- [[Burnout_Monitor_Architecture|Real-Time Burnout Monitor (< 10ms Precision)]]
