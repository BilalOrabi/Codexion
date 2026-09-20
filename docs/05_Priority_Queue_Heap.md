---
title: "Lesson 05: Min-Heap / Priority Queue from Scratch in C"
tags:
  - data-structures
  - algorithms
  - min-heap
  - priority-queue
  - 42school
  - codexion
created: 2026-09-19
status: completed
module: "Phase 1: The Building Blocks Lab"
---

[[MOC|⬅️ Back to Map of Content]]

# Lesson 05: Min-Heap / Priority Queue from Scratch in C

---

## 1. Why Does This Exist?

In `codexion`, the simulation must support two schedulers:
1. `fifo`: First-In, First-Out (simple linear queue).
2. `edf`: **Earliest Deadline First**.

### The Problem in `edf`:
In `edf`, whenever shared dongles become available, the coder whose deadline (`last_compile_start + time_to_burnout`) is closest to expiration **must be given the dongles first**. If a dying coder waits behind coders with plenty of time, they burn out, and the simulation fails!

### Why a Naive Array or Linked List Fails:
- If you store requests in an unsorted list:
  - Inserting a request: $O(1)$.
  - Finding who has the earliest deadline: **$O(N)$**. You have to scan through every coder every single time dongles are freed!
- With $N = 200$ coders and thousands of compile requests, $O(N)$ scans consume excessive CPU cycles and cause arbitration lag.
- A **Min-Heap (Priority Queue)** gives us:
  - **Find Minimum (Earliest Deadline)**: **$O(1)$** (Instant access at index 0).
  - **Extract Minimum**: **$O(\log N)$**.
  - **Insert New Request**: **$O(\log N)$**.

> [!IMPORTANT]
> **42 School Constraint**: No `std::priority_queue`, no external libraries, no `libft`. You must construct this data structure entirely from scratch in pure C.

---

## 2. Big Picture: Where It Fits

A Priority Queue is an Abstract Data Type (ADT). A **Binary Min-Heap** is the standard, high-performance engine used to implement it.

```text
       [ Schedulers in Codexion ]
                   │
      ┌────────────┴────────────┐
      ▼                         ▼
   "fifo"                    "edf"
(Simple Ring/Queue)    (Binary Min-Heap)
   - First Come           - Smallest Deadline First
   - O(1) Push/Pop        - O(log N) Push/Pop
```

---

## 3. Mental Model

### The Hospital ER Triage
In a regular bank, people are served First-Come, First-Served (**FIFO**).
In an **Emergency Room (Priority Queue / Min-Heap)**, triage nurses don't care who arrived first. The patient with the most urgent condition (smallest time left to survive) is seen immediately!

### The Corporate Pyramid
In a Min-Heap, think of a corporate pyramid:
- Every boss must have a smaller deadline than their direct subordinates.
- The CEO at the very top (the root) always has the **absolute smallest deadline** in the entire company.

---

## 4. Internal Mechanics: The Array Representation

You might think a binary tree requires complex structs with left and right pointers:
```c
/* ❌ NO POINTER TREE NEEDED! */
struct s_node {
    long long deadline;
    struct s_node *left;
    struct s_node *right;
};
```
Pointer-based trees require dynamic `malloc` per node, cause pointer overhead, and suffer from CPU cache misses.

### The Contiguous Array Formula:
A complete binary tree can be stored with **zero pointers** inside a flat 1D array!

```text
                Index 0 (Root / Smallest)
                      /           \
           Index 1                     Index 2
          /       \                   /       \
      Index 3   Index 4           Index 5   Index 6
```

For any node at index `i`:
- **Parent**: `(i - 1) / 2`
- **Left Child**: `2 * i + 1`
- **Right Child**: `2 * i + 2`

```text
Array indices: [ 0 | 1 | 2 | 3 | 4 | 5 | 6 ]
Contiguous in memory -> 100% CPU Cache-Friendly!
```

---

## 5. Core Algorithms

### Operation 1: Insert (`heap_push`) — "Heapify-Up" (Sift-Up)
1. Place the new element at the very end of the array (at index `size`).
2. Increment `size`.
3. Compare the element with its **parent** at `(i - 1) / 2`:
   - If the new element has an earlier deadline than its parent, **swap them**!
   - Update `i = parent`.
4. Repeat until the element reaches the root (`i == 0`) or its parent has an earlier deadline.
- **Time Complexity**: $O(\log N)$ (at most the height of the tree).

---

### Operation 2: Extract Min (`heap_pop`) — "Heapify-Down" (Sift-Down)
1. The smallest element is always at index `0` (the root). Save it to return.
2. Take the **last element** in the array (at index `size - 1`) and move it to index `0`.
3. Decrement `size`.
4. Now, the new root might violate the heap property. We must "sift it down":
   - Find its left child (`2*i + 1`) and right child (`2*i + 2`).
   - Find which child has the **smallest deadline**.
   - If the current node has a larger deadline than the smallest child, **swap them**!
   - Move `i` down to that child's index.
5. Repeat until the node is smaller than both children or becomes a leaf.
- **Time Complexity**: $O(\log N)$.

---

### Operation 3: The Deterministic Tie-Breaker
What if two coders have the exact same deadline?
```text
Coder A: deadline = 5000 ms, id = 3
Coder B: deadline = 5000 ms, id = 1
```
In `codexion`, scheduling must be **100% deterministic**:
- If `deadline_A != deadline_B`: Smallest deadline wins.
- If `deadline_A == deadline_B`: **Lowest coder `id` wins** (Coder 1 gets priority over Coder 3).

---

## 6. Data Structures

```c
typedef struct s_coder_req
{
    int         coder_id;
    long long   deadline_ms;
}   t_coder_req;

typedef struct s_min_heap
{
    t_coder_req *data;      /* Contiguous array of requests */
    int         size;       /* Current number of elements */
    int         capacity;   /* Maximum array capacity */
}   t_min_heap;
```

---

## 7. Common Systems Pitfalls

| Pitfall | Consequence | Prevention |
| :--- | :--- | :--- |
| **Swapping with the wrong child** | If node is larger than *both* children, you **must** swap with the *smaller* child! Swapping with the larger child destroys the min-heap invariant. | Always compare Left and Right children first to find the minimum child index. |
| **Out-of-bounds child access** | Checking `heap->data[right_child]` when `right_child >= heap->size`. | Always verify `left < size` and `right < size` before reading array elements! |
| **Ignoring tie-breakers** | Nondeterministic ordering fails 42 automated evaluation benchmarks. | Implement a dedicated comparator: `is_higher_priority(node_a, node_b)`. |
| **Integer division vs Parent index** | In C, `(0 - 1) / 2` evaluates to `0`. Loop condition for heapify-up must be `while (i > 0)`. | Stop sift-up when `i == 0`. |

---

## 8. Conceptual Check Questions

Before we open the lab file to implement the heap:

1. **Given a node at index `i = 4` in a zero-indexed array heap:**
   - What is the index of its **Parent**?
   - What are the indices of its **Left Child** and **Right Child**?
2. **When extracting the minimum element (`heap_pop`), why do we replace index `0` with the *last* element of the array instead of shifting every element over like `array[i] = array[i + 1]`?**
