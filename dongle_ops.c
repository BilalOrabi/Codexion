/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_ops.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 06:11:55 by borabi            #+#    #+#             */
/*   Updated: 2026/09/20 06:31:43 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int init_dongles(t_engine *engine)
{
	int i = 0;
	engine->dongles =
		malloc(sizeof(t_dongle) * engine->config.number_of_coders);
    if (!engine->dongles)
        return (1);
	while (i <= engine->config.number_of_coders)
	{
		engine->dongles[i].id = i;
		pthread_mutex_init(&engine->dongles[i].mutex, NULL);
		engine->dongles[i].is_in_use = 0;
        engine->dongles[i].ready_at_ms = 0;
		i++;
	}
    return (0);
}

void destroy_dongles(t_engine *engine)
{
    int i = 0;
    while (i <= engine->config.number_of_coders)
	{
		pthread_mutex_destroy(&engine->dongles[i].mutex);
        free(engine->dongles);
		i++;
	}
}

void take_dongles(t_coder *coder)
{
    if (coder->engine->config.number_of_coders == 1)
    {
        pthread_mutex_lock(&coder->left_dongle->mutex);
        log_status()
    }
}
