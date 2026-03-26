/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_elements.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgougne <pgougne@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 09:03:34 by pgougne           #+#    #+#             */
/*   Updated: 2026/03/11 09:03:36 by pgougne          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	new_dongle(t_dongle *dongle, int dongle_cooldown)
{
	dongle->lr_time.tv_sec = 0;
	dongle->queue = malloc(sizeof(t_coder *) * 2);
	if (!dongle->queue)
		return (1);
	pthread_mutex_init(&dongle->lock, NULL);
	pthread_cond_init(&dongle->cond, NULL);
	dongle->queue[0] = NULL;
	dongle->queue[1] = NULL;
	dongle->free = 1;
	gettimeofday(&dongle->lr_time, NULL);
	dongle->lr_time.tv_sec -= (dongle_cooldown + 1);
	return (0);
}

void	new_coder(t_coder *coder, t_dongle *d_left, t_dongle *d_right, int id)
{
	coder->id = id;
	coder->last_comp_start.tv_sec = 0;
	coder->comp_count = 0;
	coder->d_left = d_left;
	coder->d_right = d_right;
	gettimeofday(&coder->last_comp_start, NULL);
}

int	create_dongles(int nb_coders, t_elements *datas, int dongle_cooldown)
{
	int	i;

	i = -1;
	while (++i < nb_coders)
	{
		if (new_dongle(&datas->dongles[i], dongle_cooldown))
		{
			errors(datas->dongles, datas->coders, datas, 1);
			return (1);
		}
	}
	return (0);
}

void	create_coders(int nb_coders, t_elements *datas)
{
	int	i;
	int	rd;

	i = -1;
	rd = 0;
	while (++i < nb_coders)
	{
		if (i == 0)
			rd = nb_coders - 1;
		else
			rd = i - 1;
		new_coder(&datas->coders[i], &datas->dongles[i],
			&datas->dongles[rd], i + 1);
	}
}

t_elements	*init_datas(t_parsed *parsed_datas)
{
	t_elements	*datas;

	datas = malloc(sizeof(t_elements));
	if (!datas)
		return (NULL);
	gettimeofday(&datas->start_time, NULL);
	datas->dongles = malloc(sizeof(t_dongle) * parsed_datas->number_of_coders);
	datas->coders = malloc(sizeof(t_coder) * parsed_datas->number_of_coders);
	if (!datas->dongles || !datas->coders)
	{
		errors(datas->dongles, datas->coders, datas, 1);
		return (NULL);
	}
	pthread_mutex_init(&datas->print_lock, NULL);
	pthread_mutex_init(&datas->state_lock, NULL);
	if (create_dongles(parsed_datas->number_of_coders, datas,
			parsed_datas->dongle_cooldown) == 1)
		return (NULL);
	create_coders(parsed_datas->number_of_coders, datas);
	datas->parsed_datas = *parsed_datas;
	datas->stop_sim = 0;
	return (datas);
}
