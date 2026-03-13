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

#include "struct_elements.h"
#include "codexion.h"

void	new_dongle(t_dongle *dongle)
{
	pthread_mutex_init(&dongle->lock, NULL);
	pthread_cond_init(&dongle->cond, NULL);
	dongle->lr_time.tv_sec = 0;
	dongle->queue = NULL;
	dongle->scheduler = -1;
	dongle->free = 0;
}

void	new_coder(t_coder *coders, t_dongle *d_left, t_dongle *d_right, int id)
{
	coders->id = id;
	coders->last_comp_start.tv_sec = 0;
	coders->comp_count = 0;
	coders->d_left = d_left;
	coders->d_right = d_right;
}

int	get_rd(int i, int nb)
{
	if (i == 0)
		return (nb - 1);
	return (i - 1);
}

t_elements	*init_datas(t_parsed parsed_datas)
{
	t_elements	*datas;
	int			i;

	datas = malloc(sizeof(t_elements));
	if (!datas)
		return (NULL);
	gettimeofday(&datas->start_time, NULL);
	datas->dongles = malloc(sizeof(t_dongle) * parsed_datas.number_of_coders);
	datas->coders = malloc(sizeof(t_coder) * parsed_datas.number_of_coders);
	if (!datas->dongles || !datas->coders)
	{
		errors(datas->dongles, datas->coders, datas, 1);
		return (NULL);
	}
	i = -1;
	while (++i < parsed_datas.number_of_coders)
		new_dongle(&datas->dongles[i]);
	i = -1;
	while (++i < parsed_datas.number_of_coders)
		new_coder(&datas->coders[i], &datas->dongles[i],
			&datas->dongles[get_rd(i, parsed_datas.number_of_coders)], i + 1);
	datas->parsed_datas = parsed_datas;
	datas->stop_sim = 0;
	return (datas);
}
