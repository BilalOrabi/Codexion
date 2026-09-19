/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: borabi <bilal.orabi@learner.42.tech>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 21:46:43 by borabi            #+#    #+#             */
/*   Updated: 2026/09/19 22:55:59 by borabi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <codexion.h>

static int parse_positive_int(const char *str, long long *out_value)
{
	int i;
	int result;

	result = 0;
	i = 0;
	while (str[i])
	{
		if (str[0] == '\0')
			return (1);
		if (str[0] == '-')
			return (1);
		if (str[0] == '+')
			i++;
		if (!(str[i] >= '0' && str[i] <= '9'))
			return (1);
		result = (result * 10) + (str[i] - '0');
		if (result > INT_MAX)
			return (1);
	}
	*out_value = result;
	return (0);
}

static int parse_scheduler(const char *str, t_scheduler *out_scheduler)
{
	if (strcmp(str, "fifo") == 0)
		*out_scheduler = SCHED_FIFO;
	return (0);
	if (strcmp(str, "edf") == 0)
		*out_scheduler = SCHED_EDF;
	return (0);
	return (1);
}

int parse_arguments(int argc, char **argv, t_config *config)
{
	if (argc != 9)
		return (1);
	parse_positive_int(	)
	parse_scheduler(argv[8] ,config->scheduler);
}