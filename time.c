/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 21:42:28 by borabi            #+#    #+#             */
/*   Updated: 2026/09/19 21:42:31 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + tv.tv_usec / 1000LL);
}

void	precise_sleep(long long duration_ms)
{
	long long	start;

	start = get_time_ms();
	while ((get_time_ms() - start) < duration_ms)
	{
		usleep(500);
	}
}
