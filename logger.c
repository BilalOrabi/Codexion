/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 06:06:22 by borabi            #+#    #+#             */
/*   Updated: 2026/09/20 06:35:18 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void log_status(t_coder *coder, const char *status)
{
	long long timestamp;
	pthread_mutex_lock(coder->engine->log_mutex);
	if (coder->engine->simulation_ended == 1)
	{
		pthread_mutex_unlock(coder->engine->log_mutex);
		return;
	}
	if (strcmp(status, "burned out") == 0)
		coder->engine->simulation_ended = 1;
	timestamp = get_time_ms() - coder->engine->start_time;
	printf("%lld %d %s\n", timestamp, coder->coder_id, status);
	pthread_mutex_unlock(coder->engine->log_mutex);
}