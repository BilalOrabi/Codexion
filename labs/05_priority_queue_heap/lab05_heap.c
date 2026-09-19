/*
 * Lab 05: Min-Heap / Priority Queue from Scratch in C
 * Follow the specifications in docs/05_Priority_Queue_Heap.md
 *
 * Requirements:
 * 1. Implement a complete binary Min-Heap in a contiguous array.
 * 2. Support heap_push (O(log N)) with Heapify-Up.
 * 3. Support heap_pop (O(log N)) with Heapify-Down.
 * 4. Deterministic tie-breaker: if deadlines match, lower coder_id wins.
 * 5. Strict 42 Norm, zero memory leaks (valgrind / ASan).
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct s_coder_req
{
	int			coder_id;
	long long	deadline_ms;
}	t_coder_req;

typedef struct s_min_heap
{
	t_coder_req	*data;
	int			size;
	int			capacity;
}	t_min_heap;

/*
 * Comparator helper:
 * Returns 1 if 'a' has higher priority than 'b' (i.e. should be closer to root).
 * Returns 0 otherwise.
 * Priority rule:
 * 1. Smaller deadline_ms wins.
 * 2. If deadlines equal, smaller coder_id wins.
 */
int	is_higher_priority(t_coder_req a, t_coder_req b)
{
	/* TODO: Implement comparator */
	(void)a;
	(void)b;
	return (0);
}

int	heap_init(t_min_heap *heap, int capacity)
{
	/* TODO: Allocate heap->data array, initialize size and capacity */
	(void)heap;
	(void)capacity;
	return (0);
}

void	heap_free(t_min_heap *heap)
{
	/* TODO: Free data array */
	(void)heap;
}

int	heap_push(t_min_heap *heap, t_coder_req req)
{
	/* TODO: Insert req at end and sift-up (Heapify-Up) */
	(void)heap;
	(void)req;
	return (0);
}

int	heap_pop(t_min_heap *heap, t_coder_req *out_req)
{
	/* TODO: Extract min element at index 0 and sift-down (Heapify-Down) */
	(void)heap;
	(void)out_req;
	return (0);
}

int	main(void)
{
	t_min_heap	heap;
	t_coder_req	reqs[] = {
		{1, 5000},
		{2, 2000},
		{3, 8000},
		{4, 2000}, /* Tie with coder 2! Coder 2 should come out first */
		{5, 1000}, /* Lowest deadline: should come out 1st */
		{6, 3000}
	};
	int			n = sizeof(reqs) / sizeof(reqs[0]);
	int			i;
	t_coder_req	extracted;

	if (heap_init(&heap, 10) != 0)
		return (1);
	printf("--- Inserting %d Requests into Min-Heap ---\n", n);
	i = 0;
	while (i < n)
	{
		printf("Pushing Coder %d (deadline: %lld ms)\n", reqs[i].coder_id, reqs[i].deadline_ms);
		heap_push(&heap, reqs[i]);
		i++;
	}
	printf("\n--- Extracting in Priority Order (Expected: 5, 2, 4, 6, 1, 3) ---\n");
	while (heap_pop(&heap, &extracted) == 0)
	{
		printf("Popped Coder %d (deadline: %lld ms)\n", extracted.coder_id, extracted.deadline_ms);
	}
	heap_free(&heap);
	return (0);
}
