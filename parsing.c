/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgougne <pgougne@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 09:05:12 by pgougne           #+#    #+#             */
/*   Updated: 2026/03/10 09:05:18 by pgougne          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	ft_atoi(const char *nptr)
{
	int			i;
	int			sign;
	long long	nb;

	i = 0;
	sign = 1;
	nb = 0;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == ' ')
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i++] == '-')
			sign = -1;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
		nb = nb * 10 + (nptr[i++] - '0');
	return (nb * sign);
}

int	int_verif(char *str)
{
	int		i;
	long	res;

	i = 0;
	res = 0;
	if (strlen(str) > 10)
		return (-1);
	while (str[i] != '\0')
	{
		if (str[i] < '0' || str[i] > '9')
			return (-1);
		i++;
	}
	res = ft_atoi(str);
	if (res > INT_MAX || res < 0)
		return (-1);
	return (res);
}

int	fill_data(t_datas *datas, char **argv)
{
	int	i;
	int	*fields[8];

	fields[0] = &datas->number_of_coders;
	fields[1] = &datas->time_to_burnout;
	fields[2] = &datas->time_to_compile;
	fields[3] = &datas->time_to_debug;
	fields[4] = &datas->time_to_refactor;
	fields[5] = &datas->number_of_compiles_required;
	fields[6] = &datas->dongle_cooldown;
	fields[7] = &datas->scheduler;
	i = 0;
	while (i < 8)
	{
		*fields[i] = int_verif(argv[i + 1]);
		if (*fields[i] == -1)
			return (-1);
		i++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_datas	datas;

	datas.number_of_coders = 0;
	if (argc != 9)
	{
		fprintf(stderr, "Error: there are too many/few arguments\n");
		return (1);
	}
	if (fill_data(&datas, argv) == -1)
	{
		fprintf(stderr, "Error: invalid argument\n");
		return (1);
	}
	printf("nb_code: %d\nt_burnout: %d\nt_compile: %d\nt_debug: %d\nt_refactor: %d\nnb_compile_required: %d\ndongle_cooldown: %d\nscheduler: %d", datas.number_of_coders, datas.time_to_burnout, datas.time_to_compile, datas.time_to_debug, datas.time_to_refactor, datas.number_of_compiles_required, datas.dongle_cooldown, datas.scheduler);
}

/*
number_of_coders
time_to_burnout
time_to_compile
time_to_debug
time_to_refactor
number_of_compiles_required
dongle_cooldown
scheduler
*/