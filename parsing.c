/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 21:46:43 by borabi            #+#    #+#             */
/*   Updated: 2026/09/24 11:49:52 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	parse_positive_int(const char *str, long long *out_value)
{
	int			i;
	long long	result;

	result = 0;
	i = 0;
	if (str[0] == '\0')
		return (1);
	if (str[0] == '-')
		return (1);
	if (str[0] == '+' && str[1] != '\0')
		i++;
	while (str[i])
	{
		if (!(str[i] >= '0' && str[i] <= '9'))
			return (1);
		result = (result * 10) + (str[i] - '0');
		if (result > INT_MAX)
			return (1);
		i++;
	}
	*out_value = result;
	return (0);
}

static int	parse_scheduler(const char *str, t_scheduler *out_scheduler)
{
	if (strcmp(str, "fifo") == 0)
	{
		*out_scheduler = SCHEDULER_FIFO;
		return (0);
	}
	if (strcmp(str, "edf") == 0)
	{
		*out_scheduler = SCHEDULER_EDF;
		return (0);
	}
	return (1);
}

int	parse_arguments(int argc, char **argv, t_config *config)
{
	long long	temp;

	if (argc != 9)
		return (1);
	if (parse_positive_int(argv[1], &temp) != 0 || temp < 1)
		return (1);
	config->number_of_coders = (int)temp;
	if (parse_positive_int(argv[2], &config->time_to_burnout) != 0)
		return (1);
	if (parse_positive_int(argv[3], &config->time_to_compile) != 0)
		return (1);
	if (parse_positive_int(argv[4], &config->time_to_debug) != 0)
		return (1);
	if (parse_positive_int(argv[5], &config->time_to_refactor) != 0)
		return (1);
	if (parse_positive_int(argv[6], &temp) != 0)
		return (1);
	config->number_of_compiles_required = (int)temp;
	if (parse_positive_int(argv[7], &config->dongle_cooldown) != 0)
		return (1);
	if (parse_scheduler(argv[8], &config->scheduler) != 0)
		return (1);
	return (0);
}
