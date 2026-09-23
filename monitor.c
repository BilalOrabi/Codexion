/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 08:19:18 by borabi            #+#    #+#             */
/*   Updated: 2026/09/23 08:29:24 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int check_coders_burnout(t_engine *engine)
{
	int i;
	int elapsed;

	elapsed = 0;
	i = 0;
	while (i < engine->config.number_of_coders)
	{
		elapsed = get_time_ms() - engine->coders[i]->last_compile_start;
		if (elapsed < engine->config.time_to_burnout)
		{
			log_status(&engine->coders[i], "burned out");
			return 1;
		}
	}
	return 0;
}

check_all_compiled(t_engine *engine)
{
    if(engine->config.number_of_compiles_required != -1)
    {
        engine->coders->compile_count >= engine->config.number_of_compiles_required
    }
}