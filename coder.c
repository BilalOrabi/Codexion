/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 07:11:10 by borabi            #+#    #+#             */
/*   Updated: 2026/09/24 12:48:26 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_simulation_ended(t_engine *engine)
{
	int	ended;

	ended = 0;
	pthread_mutex_lock(&engine->log_mutex);
	ended = engine->simulation_ended;
	pthread_mutex_unlock(&engine->log_mutex);
	return (ended);
}

static void	coder_cycle(t_coder *coder)
{
	take_dongles(coder);
	if (is_simulation_ended(coder->engine))
	{
		if (coder->engine->config.number_of_coders > 1)
			drop_dongles(coder);
		return ;
	}
	pthread_mutex_lock(&coder->engine->state_mutex);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->engine->state_mutex);
	log_status(coder, "is compiling");
	precise_sleep(coder->engine->config.time_to_compile);
	pthread_mutex_lock(&coder->engine->state_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->engine->state_mutex);
	drop_dongles(coder);
	if (is_simulation_ended(coder->engine))
		return ;
	log_status(coder, "is debugging");
	precise_sleep(coder->engine->config.time_to_debug);
	if (is_simulation_ended(coder->engine))
		return ;
	log_status(coder, "is refactoring");
	precise_sleep(coder->engine->config.time_to_refactor);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->coder_id % 2 == 0)
		precise_sleep(1);
	while (!is_simulation_ended(coder->engine))
		coder_cycle(coder);
	return (NULL);
}
