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
}

void	new_coder(t_coder *coders, t_dongle *d_left, t_dongle *d_right, int id)
{
	coders->id = id;
	coders->last_comp_start.tv_sec = 0;
	coders->comp_count = 0;
	coders->d_left = d_left;
	coders->d_right = d_right;
}

int	get_right_dongle(int i, int nb)
{
	if (i == 0)
		return (nb - 1);
	return (i - 1);
}

t_elements	*init_datas(int nb)
{
	t_elements	*datas;
	int		i;

	datas = malloc(sizeof(t_elements));
	if (!datas)
		return (NULL);
	datas->dongles = malloc(sizeof(t_dongle) * nb);
	datas->coders = malloc(sizeof(t_coder) * nb);
	if (!datas->dongles || !datas->coders)
	{
		if (datas->dongles)
			free(datas->dongles);
		if (datas->coders)
			free(datas->coders);
		free(datas);
		return (NULL);
	}
	i = -1;
	while (++i < nb)
		new_dongle(&datas->dongles[i]);
	i = -1;
	while (++i < nb)
		new_coder(&datas->coders[i], &datas->dongles[i],
			&datas->dongles[get_right_dongle(i, nb)], i + 1);
	return (datas);
}