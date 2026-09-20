/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 21:42:39 by borabi            #+#    #+#             */
/*   Updated: 2026/09/20 04:05:34 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
#define CODEXION_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

typedef enum e_scheduler
{
	SCHEDULER_FIFO,
	SCHEDULER_EDF
} t_scheduler;

typedef struct s_config
{
	int number_of_coders;
	long long time_to_burnout;
	long long time_to_compile;
	long long time_to_debug;
	long long time_to_refactor;
	int number_of_compiles_required;
	long long dongle_cooldown;
	t_scheduler scheduler;
} t_config;

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

/* --- Time & Sleep Functions (time.c) --- */
long long get_time_ms(void);
void precise_sleep(long long duration_ms);

/* --- Min-Heap Lifecycle & Helpers (heap_utils.c) --- */
int min_heap_initialize(t_min_heap *heap, int maximum_capacity);
void min_heap_destroy(t_min_heap *heap);
void swap(t_min_heap *heap, int index_a, int index_b);

/* --- Min-Heap Operations (heap_ops.c) --- */
int has_higher_priority(t_coder candidate, t_coder reference);
int min_heap_peek(const t_min_heap *heap, t_coder *result);
int min_heap_push(t_min_heap *heap, t_coder new_coder);
int min_heap_pop(t_min_heap *heap, t_coder *result);

/* --- Parsing--- */
int parse_arguments(int argc, char **argv, t_config *config);

#endif
