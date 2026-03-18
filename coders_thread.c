/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgougne <pgougne@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 14:45:44 by pgougne           #+#    #+#             */
/*   Updated: 2026/03/11 14:45:45 by pgougne          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include "unistd.h"

void	print_locked_dongle(int id, char *msg, t_elements *elements)
{
	pthread_mutex_lock(&elements->print_lock);
	if (!elements->stop_sim)
		printf("%d %s\n", id, msg);
	pthread_mutex_unlock(&elements->print_lock);
}

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

int	is_priority_edf(t_dongle *d, t_coder *coder, t_elements *el)
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

int	is_priority(t_dongle *d, t_coder *coder)
{
	if (d->queue[0] == NULL)
		return (1);
	if (d->queue[0] == coder)
		return (1);
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

int	take_dongle(t_coder *coder, t_elements *elements)
{
	coder->d_left->free = 0;
	coder->d_right->free = 0;
	remove_from_queue(coder->d_left, coder);
	remove_from_queue(coder->d_right, coder);
	log_action(coder->id, "has taken a dongle", elements);
	log_action(coder->id, "has taken a dongle", elements);
	return (1);
}

void	lock_dgl(t_coder *coder)
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
			if (is_priority(coder->d_left, coder)
				&& is_priority(coder->d_right, coder))
				return (take_dongle(coder, elements));
		}
		else if (elements->parsed_datas.scheduler == 1)
		{
			if (is_priority_edf(coder->d_left, coder, elements)
				&& is_priority_edf(coder->d_right, coder, elements))
				return (take_dongle(coder, elements));
		}
	}
	pthread_mutex_unlock(&coder->d_right->lock);
	pthread_mutex_unlock(&coder->d_left->lock);
	return (0);
}

void	get_timeout(struct timespec *ts, int ms_to_wait)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	ts->tv_sec = tv.tv_sec + (ms_to_wait / 1000);
	ts->tv_nsec = (tv.tv_usec * 1000) + ((ms_to_wait % 1000) * 1000000);
	if (ts->tv_nsec >= 1000000000)
	{
		ts->tv_sec++;
		ts->tv_nsec -= 1000000000;
	}
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

void	coder_actions(t_thread_param	*thread_param)
{
	t_coder		*coder;
	t_parsed	parsed_datas;

	coder = &thread_param->elements->coders[thread_param->idx];
	parsed_datas = thread_param->elements->parsed_datas;
	pthread_mutex_lock(&thread_param->elements->state_lock);
	gettimeofday(&coder->last_comp_start, NULL);
	pthread_mutex_unlock(&thread_param->elements->state_lock);
	action(coder->id, parsed_datas.time_to_debug,
		thread_param->elements, "is compiling");
	pthread_mutex_lock(&thread_param->elements->state_lock);
	coder->comp_count++;
	pthread_mutex_unlock(&thread_param->elements->state_lock);
	free_dongles(coder);
	action(coder->id, parsed_datas.time_to_debug,
		thread_param->elements, "is debugging");
	action(coder->id, parsed_datas.time_to_refactor,
		thread_param->elements, "is refactoring");
}

static int	wait_for_dongles(t_coder *coder, t_thread_param *thread_param, struct timespec *ts)
{
	while (1)
	{
		if (try_to_grab_dongles(coder, thread_param->elements) == 1)
			break ;
		get_timeout(&ts, 10);
		pthread_mutex_lock(&coder->d_left->lock);
		if (!thread_param->elements->stop_sim)
			pthread_cond_timedwait(&coder->d_left->cond,
				&coder->d_left->lock, &ts);
		pthread_mutex_unlock(&coder->d_left->lock);
		pthread_mutex_lock(&thread_param->elements->state_lock);
		if (thread_param->elements->stop_sim)
		{
			pthread_mutex_unlock(&thread_param->elements->state_lock);
			return (NULL);
		}
		pthread_mutex_unlock(&thread_param->elements->state_lock);
	}
}

void	*actions_loop(void *arg)
{
	t_thread_param	*thread_param;
	t_coder			*coder;
	t_parsed		parsed_datas;
	struct timespec	ts;

	thread_param = (t_thread_param *)arg;
	coder = &thread_param->elements->coders[thread_param->idx];
	parsed_datas = thread_param->elements->parsed_datas;
	if (coder->id % 2 != 0)
		usleep(1000);
	while (1)
	{
		pthread_mutex_lock(&thread_param->elements->state_lock);
		if (thread_param->elements->stop_sim == 1)
		{
			pthread_mutex_unlock(&thread_param->elements->state_lock);
			break ;
		}
		pthread_mutex_unlock(&thread_param->elements->state_lock);
		add_to_queue(coder->d_right, coder);
		add_to_queue(coder->d_left, coder);
		while (1)
		{
			if (try_to_grab_dongles(coder, thread_param->elements) == 1)
				break ;
			get_timeout(&ts, 10);
			pthread_mutex_lock(&coder->d_left->lock);
			if (!thread_param->elements->stop_sim)
				pthread_cond_timedwait(&coder->d_left->cond,
					&coder->d_left->lock, &ts);
			pthread_mutex_unlock(&coder->d_left->lock);
			pthread_mutex_lock(&thread_param->elements->state_lock);
			if (thread_param->elements->stop_sim)
			{
				pthread_mutex_unlock(&thread_param->elements->state_lock);
				return (NULL);
			}
			pthread_mutex_unlock(&thread_param->elements->state_lock);
		}
		coder_actions(thread_param);
	}
	return (NULL);
}
