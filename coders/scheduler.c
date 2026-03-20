/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgougne <pgougne@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 13:36:39 by pgougne           #+#    #+#             */
/*   Updated: 2026/03/18 13:36:42 by pgougne          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	edf(t_dongle *d, t_coder *coder, t_elements *el)
{
	t_coder	*competitor;
	long	my_life;
	long	their_life;

	if (d->queue[0] == NULL)
		return (1);
	competitor = d->queue[0];
	if (competitor == coder)
		return (1);
	pthread_mutex_lock(&el->state_lock);
	my_life = get_delta_time(&coder->last_comp_start);
	their_life = get_delta_time(&competitor->last_comp_start);
	pthread_mutex_unlock(&el->state_lock);
	if (my_life > their_life)
		return (1);
	return (0);
}

int	fifo(t_dongle *d, t_coder *coder)
{
	if (d->queue[0] == NULL)
		return (1);
	if (d->queue[0] == coder)
		return (1);
	return (0);
}

int	add_to_queue(t_dongle *d, t_coder *coder)
{
	pthread_mutex_lock(&d->lock);
	if (d->queue[0] == coder || d->queue[1] == coder)
	{
		pthread_mutex_unlock(&d->lock);
		return (0);
	}
	if (d->queue[0] == NULL)
		d->queue[0] = coder;
	else if (d->queue[1] == NULL)
		d->queue[1] = coder;
	pthread_mutex_unlock(&d->lock);
	return (0);
}

void	remove_from_queue(t_dongle *d, t_coder *coder)
{
	if (d->queue[0] == coder)
	{
		d->queue[0] = d->queue[1];
		d->queue[1] = NULL;
	}
	else if (d->queue[1] == coder)
	{
		d->queue[1] = NULL;
	}
}
