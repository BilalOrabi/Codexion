/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 21:42:17 by borabi            #+#    #+#             */
/*   Updated: 2026/09/19 21:42:20 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	min_heap_initialize(t_min_heap *heap, int maximum_capacity)
{
	heap->request_array = malloc(sizeof(t_coder) * maximum_capacity);
	if (!heap->request_array)
		return (1);
	heap->current_size = 0;
	heap->maximum_capacity = maximum_capacity;
	return (0);
}

void	min_heap_destroy(t_min_heap *heap)
{
	free(heap->request_array);
	heap->maximum_capacity = 0;
	heap->current_size = 0;
	heap->request_array = NULL;
}

void	swap(t_min_heap *heap, int index_a, int index_b)
{
	t_coder	tmp;

	tmp = heap->request_array[index_a];
	heap->request_array[index_a] = heap->request_array[index_b];
	heap->request_array[index_b] = tmp;
}
