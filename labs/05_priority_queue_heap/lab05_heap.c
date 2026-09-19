/*
 * Lab 05: Binary Min-Heap / Priority Queue from Scratch in C
 * Specifications in docs/05_Priority_Queue_Heap.md
 *
 * Requirements:
 * 1. Implement a complete binary Min-Heap in a contiguous array.
 * 2. min_heap_insert (O(log N)) with Heapify-Up.
 * 3. min_heap_extract_minimum (O(log N)) with Heapify-Down.
 * 4. Deterministic tie-breaker: if deadlines match, lower coder_id wins.
 * 5. Strict 42 Norm, zero memory leaks.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct s_coder
{
	int coder_id;
	long long burnout_deadline_ms;
} t_coder;

typedef struct s_min_heap
{
	t_coder *request_array;
	int current_size;
	int maximum_capacity;
} t_min_heap;

/*
 * Priority Comparator:
 * Returns 1 if candidate has higher priority than reference (closer to root).
 * Returns 0 otherwise.
 *
 * Arbitration Rules:
 * 1. Smaller burnout_deadline_ms wins (earliest deadline first).
 * 2. If burnout_deadline_ms is identical, smaller coder_id wins.
 */
int has_higher_priority(t_coder candidate, t_coder reference)
{
	if (candidate.burnout_deadline_ms != reference.burnout_deadline_ms)
		return (candidate.burnout_deadline_ms < reference.burnout_deadline_ms);
	return (candidate.coder_id < reference.coder_id);
}

int min_heap_initialize(t_min_heap *heap, int maximum_capacity)
{
	heap->request_array = malloc(sizeof(t_coder) * maximum_capacity);
	if (!heap->request_array)
		return (1);
	heap->current_size = 0;
	heap->maximum_capacity = maximum_capacity;
	return (0);
}

void min_heap_destroy(t_min_heap *heap)
{
	free(heap->request_array);
	heap->maximum_capacity = 0;
	heap->current_size = 0;
	heap->request_array = NULL;
}

int min_heap_peek(const t_min_heap *heap, t_coder *result)
{
	if (heap->current_size == 0)
		return (1);
	*result = heap->request_array[0];
	return (0);
}

void swap(t_min_heap *heap, int index_a, int index_b)
{
	t_coder tmp;
	tmp = heap->request_array[index_a];
	heap->request_array[index_a] = heap->request_array[index_b];
	heap->request_array[index_b] = tmp;
}

int min_heap_push(t_min_heap *heap, t_coder new_coder)
{
	int current_index = 0;
	int parant_index = 0;
	if (heap->current_size >= heap->maximum_capacity)
		return (1);
	current_index = heap->current_size;
	heap->request_array[current_index] = new_coder;
	heap->current_size++;
	while (current_index > 0)
	{
		parant_index = (current_index - 1) / 2;
		if (has_higher_priority(heap->request_array[current_index],
								heap->request_array[parant_index]))
		{
			swap(heap, current_index, parant_index);
			current_index = parant_index;
		}
		else
			break;
	}

	return (0);
}

int min_heap_pop(t_min_heap *heap, t_coder *result)
{
	int current_index = 0;
	int left_child = 0;
	int right_child = 0;
	int best_child = 0;

	if (heap->current_size == 0)
		return (1);
	*result = heap->request_array[0];
	heap->request_array[0] = heap->request_array[heap->current_size - 1];
	heap->current_size--;
	if (heap->current_size == 0)
		return (0);
	while (2 * current_index + 1 < heap->current_size)
	{
		left_child = 2 * current_index + 1;
		right_child = 2 * current_index + 2;
		best_child = left_child;
		if (right_child < heap->current_size &&
			has_higher_priority(heap->request_array[right_child],
								heap->request_array[left_child]))
			best_child = right_child;
		if (has_higher_priority(heap->request_array[best_child],
								heap->request_array[current_index]))
		{
			swap(heap, best_child, current_index);
			current_index = best_child;
		}
		else
			break;
	}
	return (0);
}

int main(void)
{
	t_min_heap heap;
	t_coder test_requests[] = {
		{1, 5000}, {2, 2000}, {3, 8000},
		{4, 2000}, /* Tie with coder 2! Coder 2 should come out first */
		{5, 1000}, /* Lowest deadline: should come out 1st */
		{6, 3000}};
	int total_requests_count;
	int request_index;
	t_coder extracted_request;

	total_requests_count = sizeof(test_requests) / sizeof(test_requests[0]);
	if (min_heap_initialize(&heap, 10) != 0)
	{
		fprintf(stderr, "Error: Failed to initialize min-heap\n");
		return (1);
	}
	printf("--- Inserting %d Requests into Min-Heap (push) ---\n",
		   total_requests_count);
	request_index = 0;
	while (request_index < total_requests_count)
	{
		printf("Pushing Coder %d (deadline: %lld ms)\n",
			   test_requests[request_index].coder_id,
			   test_requests[request_index].burnout_deadline_ms);
		min_heap_push(&heap, test_requests[request_index]);
		request_index++;
	}
	printf("\n--- Extracting in Priority Order (pop) (Expected: 5, 2, 4, 6, 1, "
		   "3) ---\n");
	while (min_heap_pop(&heap, &extracted_request) == 0)
	{
		printf("Popped Coder %d (deadline: %lld ms)\n",
			   extracted_request.coder_id,
			   extracted_request.burnout_deadline_ms);
	}
	min_heap_destroy(&heap);
	return (0);
}
