/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_ops.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 06:11:55 by borabi            #+#    #+#             */
/*   Updated: 2026/09/23 06:25:27 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_dongles(t_engine *engine)
{
	int	i;

	i = 0;
	engine->dongles = malloc(sizeof(t_dongle)
			* engine->config.number_of_coders);
	if (!engine->dongles)
		return (1);
	while (i < engine->config.number_of_coders)
	{
		engine->dongles[i].id = i;
		pthread_mutex_init(&engine->dongles[i].mutex, NULL);
		engine->dongles[i].is_in_use = 0;
		engine->dongles[i].ready_at_ms = 0;
		i++;
	}
	return (0);
}

void	destroy_dongles(t_engine *engine)
{
	int	i;

	i = 0;
	while (i < engine->config.number_of_coders)
	{
		pthread_mutex_destroy(&engine->dongles[i].mutex);
		i++;
	}
	free(engine->dongles);
	engine->dongles = NULL;
}

static void	acquire_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	if (get_time_ms() < dongle->ready_at_ms)
		precise_sleep(dongle->ready_at_ms - get_time_ms());
	log_status(coder, "has taken a dongle");
}

void	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->engine->config.number_of_coders == 1)
	{
		pthread_mutex_lock(&coder->left_dongle->mutex);
		log_status(coder, "has taken a dongle");
		precise_sleep(coder->engine->config.time_to_burnout);
		pthread_mutex_unlock(&coder->left_dongle->mutex);
		return ;
	}
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
	}
	else
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	acquire_dongle(coder, first);
	acquire_dongle(coder, second);
}

void	drop_dongles(t_coder *coder)
{
	long long	cooldown_until;

	cooldown_until = get_time_ms() + coder->engine->config.dongle_cooldown;
	coder->left_dongle->ready_at_ms = cooldown_until;
	coder->right_dongle->ready_at_ms = cooldown_until;
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_unlock(&coder->right_dongle->mutex);
}

