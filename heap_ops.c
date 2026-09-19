/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 20:03:00 by borabi            #+#    #+#             */
/*   Updated: 2026/09/19 20:10:24 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	has_higher_priority(t_coder candidate, t_coder reference)
{
	if (candidate.burnout_deadline_ms != reference.burnout_deadline_ms)
		return (candidate.burnout_deadline_ms < reference.burnout_deadline_ms);
	return (candidate.coder_id < reference.coder_id);
}

int	min_heap_peek(const t_min_heap *heap, t_coder *result)
{
	if (heap->current_size == 0)
		return (1);
	*result = heap->request_array[0];
	return (0);
}

int	min_heap_push(t_min_heap *heap, t_coder new_coder)
{
	int	current_index;
	int	parent_index;

	if (heap->current_size >= heap->maximum_capacity)
		return (1);
	current_index = heap->current_size;
	heap->request_array[current_index] = new_coder;
	heap->current_size++;
	while (current_index > 0)
	{
		parent_index = (current_index - 1) / 2;
		if (has_higher_priority(heap->request_array[current_index],
				heap->request_array[parent_index]))
		{
			swap(heap, current_index, parent_index);
			current_index = parent_index;
		}
		else
			break ;
	}
	return (0);
}

static void	heapfiy_down(t_min_heap *heap, int current_index)
{
	int	left_child;
	int	right_child;
	int	best_child;

	while (2 * current_index + 1 < heap->current_size)
	{
		left_child = 2 * current_index + 1;
		right_child = 2 * current_index + 2;
		best_child = left_child;
		if (right_child < heap->current_size
			&& has_higher_priority(heap->request_array[right_child],
				heap->request_array[left_child]))
			best_child = right_child;
		if (!has_higher_priority(heap->request_array[best_child],
				heap->request_array[current_index]))
			break ;
		swap(heap, best_child, current_index);
		current_index = best_child;
	}
}

int	min_heap_pop(t_min_heap *heap, t_coder *result)
{
	if (heap->current_size == 0)
		return (1);
	*result = heap->request_array[0];
	heap->request_array[0] = heap->request_array[heap->current_size - 1];
	heap->current_size--;
	if (heap->current_size > 0)
		heapfiy_down(heap, 0);
	return (0);
}
