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

void	lock_dgl(int id, pthread_mutex_t *lock1,
	pthread_mutex_t *lock2, t_elements *elements)
{
	pthread_mutex_lock(lock1);
	log_action(id, "has taken a dongle", elements);
	pthread_mutex_lock(lock2);
	log_action(id, "has taken a dongle", elements);
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

int	try_to_grab_dongles(t_coder *coder, t_elements *elements)
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
	if (coder->d_left->free && coder->d_right->free
		&& get_delta_time(&coder->d_left->lr_time)
		>= elements->parsed_datas.dongle_cooldown
		&& get_delta_time(&coder->d_right->lr_time)
		>= elements->parsed_datas.dongle_cooldown)
	{
		coder->d_left->free = 0;
		coder->d_right->free = 0;
		log_action(coder->id, "has taken a dongle", elements);
		log_action(coder->id, "has taken a dongle", elements);
		return (1);
	}
	pthread_mutex_unlock(&coder->d_right->lock);
	pthread_mutex_unlock(&coder->d_left->lock);
	return (0);
}

void get_timeout(struct timespec *ts, int ms_to_wait)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	ts->tv_sec = tv.tv_sec + (ms_to_wait / 1000);
	ts->tv_nsec = (tv.tv_usec * 1000) + ((ms_to_wait % 1000) * 1000000);
	if (ts->tv_nsec >= 1000000000)
	{
		ts->tv_sec++;
		ts->tv_nsec -= 1000000000;
	}
}

void	*actions_loop(void *arg)
{
	t_thread_param	*thread_param;
	t_coder			*coder;
	t_parsed		parsed_datas;

	thread_param = (t_thread_param *)arg;
	coder = &thread_param->elements->coders[thread_param->idx];
	parsed_datas = thread_param->elements->parsed_datas;
	while (1)
	{
		pthread_mutex_lock(&thread_param->elements->state_lock);
		if (coder->comp_count >= parsed_datas.number_of_compiles_required
			|| thread_param->elements->stop_sim == 1)
		{
			pthread_mutex_unlock(&thread_param->elements->state_lock);
			break ;
		}
		pthread_mutex_unlock(&thread_param->elements->state_lock);
		if (try_to_grab_dongles(coder, thread_param->elements) == 1)
		{
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
		else
    	{
			struct timespec ts;
			get_timeout(&ts, 10);
			pthread_mutex_lock(&coder->d_left->lock);
			pthread_cond_timedwait(&coder->d_left->cond, &coder->d_left->lock, &ts);
			pthread_mutex_unlock(&coder->d_left->lock);
		}
	}
	return (NULL);
}

/*
format: time | coder_id | action

0) take left + right dongle
1) compiling
then
2) debugging
3) refactoring
*/