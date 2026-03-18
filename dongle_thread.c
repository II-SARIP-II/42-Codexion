/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_thread.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgougne <pgougne@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 13:40:21 by pgougne           #+#    #+#             */
/*   Updated: 2026/03/18 13:40:34 by pgougne          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	free_dongles(t_coder *coder)
{
	coder->d_left->free = 1;
	coder->d_right->free = 1;
	gettimeofday(&coder->d_right->lr_time, NULL);
	gettimeofday(&coder->d_left->lr_time, NULL);
	pthread_cond_broadcast(&coder->d_left->cond);
	pthread_cond_broadcast(&coder->d_right->cond);
	if (coder->d_left < coder->d_right)
	{
		pthread_mutex_unlock(&coder->d_right->lock);
		pthread_mutex_unlock(&coder->d_left->lock);
	}
	else
	{
		pthread_mutex_unlock(&coder->d_left->lock);
		pthread_mutex_unlock(&coder->d_right->lock);
	}
}

static int	take_dongle(t_coder *coder, t_elements *elements)
{
	coder->d_left->free = 0;
	coder->d_right->free = 0;
	remove_from_queue(coder->d_left, coder);
	remove_from_queue(coder->d_right, coder);
	log_action(coder->id, "has taken a dongle", elements);
	log_action(coder->id, "has taken a dongle", elements);
	return (1);
}

static void	lock_dgl(t_coder *coder)
{
	if (coder->d_left < coder->d_right)
	{
		pthread_mutex_lock(&coder->d_left->lock);
		pthread_mutex_lock(&coder->d_right->lock);
	}
	else
	{
		pthread_mutex_lock(&coder->d_right->lock);
		pthread_mutex_lock(&coder->d_left->lock);
	}
}

int	try_to_grab_dongles(t_coder *coder, t_elements *elements)
{
	lock_dgl(coder);
	if (coder->d_left->free && coder->d_right->free
		&& get_delta_time(&coder->d_left->lr_time)
		>= elements->parsed_datas.dongle_cooldown
		&& get_delta_time(&coder->d_right->lr_time)
		>= elements->parsed_datas.dongle_cooldown)
	{
		if (elements->parsed_datas.scheduler == 0)
		{
			if (fifo(coder->d_left, coder)
				&& fifo(coder->d_right, coder))
				return (take_dongle(coder, elements));
		}
		else if (elements->parsed_datas.scheduler == 1)
		{
			if (edf(coder->d_left, coder, elements)
				&& edf(coder->d_right, coder, elements))
				return (take_dongle(coder, elements));
		}
	}
	pthread_mutex_unlock(&coder->d_right->lock);
	pthread_mutex_unlock(&coder->d_left->lock);
	return (0);
}
