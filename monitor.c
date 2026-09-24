/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 08:19:18 by borabi            #+#    #+#             */
/*   Updated: 2026/09/24 06:31:45 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_coders_burnout(t_engine *engine)
{
	int			i;
	long long	elapsed;

	elapsed = 0;
	i = 0;
	while (i < engine->config.number_of_coders)
	{
		elapsed = get_time_ms() - engine->coders[i].last_compile_start;
		if (elapsed >= engine->config.time_to_burnout)
		{
			log_status(&engine->coders[i], "burned out");
			return (1);
		}
		i++;
	}
	return (0);
}

static int	check_all_compiled(t_engine *engine)
{
	int	i;

	if (engine->config.number_of_compiles_required == -1)
		return (0);
	i = 0;
	while (i < engine->config.number_of_coders)
	{
		if (engine->coders[i].compile_count
			< engine->config.number_of_compiles_required)
			return (0);
		i++;
	}
	pthread_mutex_lock(&engine->log_mutex);
	engine->simulation_ended = 1;
	pthread_mutex_unlock(&engine->log_mutex);
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_engine	*engine;

	engine = (t_engine *)arg;
	while (!is_simulation_ended(engine))
	{
		if (check_coders_burnout(engine))
			break ;
		if (check_all_compiled(engine))
			break ;
		usleep(1000);
	}
	return (NULL);
}