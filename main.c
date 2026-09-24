/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 06:44:52 by borabi            #+#    #+#             */
/*   Updated: 2026/09/24 07:36:50 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_coders(t_engine *engine)
{
	int	i;

	i = 0;
	engine->coders = malloc(sizeof(t_coder) * engine->config.number_of_coders);
	if (!engine->coders)
		return (1);
	while (i < engine->config.number_of_coders)
	{
		engine->coders[i].coder_id = i + 1;
		engine->coders[i].compile_count = 0;
		engine->coders[i].engine = engine;
		engine->coders[i].left_dongle = &engine->dongles[i];
		engine->coders[i].right_dongle
			= &engine->dongles[(i + 1) % engine->config.number_of_coders];
		i++;
	}
	return (0);
}

static int	init_engine(t_engine *engine)
{
	engine->simulation_ended = 0;
	pthread_mutex_init(&engine->log_mutex, NULL);
	pthread_mutex_init(&engine->state_mutex, NULL);
	if (init_dongles(engine) != 0)
		return (1);
	if (init_coders(engine) != 0)
		return (1);
	return (0);
}

static int	start_simulation(t_engine *engine)
{
	int	i;

	engine->start_time = get_time_ms();
	i = 0;
	while (i < engine->config.number_of_coders)
	{
		engine->coders[i].last_compile_start = engine->start_time;
		i++;
	}
	i = 0;
	while (i < engine->config.number_of_coders)
	{
		pthread_create(&engine->coders[i].thread, NULL, coder_routine,
			&engine->coders[i]);
		i++;
	}
	pthread_create(&engine->monitor, NULL, monitor_routine, engine);
	return (0);
}

static void	cleanup_simulation(t_engine *engine)
{
	int	i;

	i = 0;
	pthread_join(engine->monitor, NULL);
	while (i < engine->config.number_of_coders)
	{
		pthread_join(engine->coders[i].thread, NULL);
		i++;
	}
	destroy_dongles(engine);
	pthread_mutex_destroy(&engine->log_mutex);
	pthread_mutex_destroy(&engine->state_mutex);
	free(engine->coders);
	engine->coders = NULL;
}

int	main(int argc, char **argv)
{
	t_engine	engine;

	if (parse_arguments(argc, argv, &engine.config) != 0)
		return (1);
	if (init_engine(&engine) != 0)
		return (1);
	if (start_simulation(&engine) != 0)
		return (1);
	cleanup_simulation(&engine);
	return (0);
}
