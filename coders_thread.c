/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_thread.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgougne <pgougne@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 14:45:44 by pgougne           #+#    #+#             */
/*   Updated: 2026/03/11 14:45:45 by pgougne          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include "unistd.h"

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

static int	wait_for_dongles(t_coder *coder,
	t_thread_param *tp, struct timespec *ts)
{
	int	stop;

	while (1)
	{
		if (try_to_grab_dongles(coder, tp->elements) == 1)
			return (0);
		get_timeout(ts, 10);
		pthread_mutex_lock(&tp->elements->state_lock);
		stop = tp->elements->stop_sim;
		pthread_mutex_unlock(&tp->elements->state_lock);
		if (stop)
			return (1);
		pthread_mutex_lock(&coder->d_left->lock);
		pthread_cond_timedwait(&coder->d_left->cond, &coder->d_left->lock, ts);
		pthread_mutex_unlock(&coder->d_left->lock);
		pthread_mutex_lock(&tp->elements->state_lock);
		stop = tp->elements->stop_sim;
		pthread_mutex_unlock(&tp->elements->state_lock);
		if (stop)
			return (1);
	}
}

void	*actions_loop(void *arg)
{
	t_thread_param	*tp;
	t_coder			*coder;
	t_parsed		parsed_datas;
	struct timespec	ts;

	ts.tv_sec = 0;
	tp = (t_thread_param *)arg;
	coder = &tp->elements->coders[tp->idx];
	parsed_datas = tp->elements->parsed_datas;
	if (coder->id % 2 != 0)
		usleep(1000);
	while (1)
	{
		pthread_mutex_lock(&tp->elements->state_lock);
		if (tp->elements->stop_sim || parsed_datas.number_of_coders == 1)
			return (pthread_mutex_unlock(&tp->elements->state_lock), NULL);
		pthread_mutex_unlock(&tp->elements->state_lock);
		add_to_queue(coder->d_right, coder);
		add_to_queue(coder->d_left, coder);
		if (wait_for_dongles(coder, tp, &ts))
			return (NULL);
		coder_actions(tp);
	}
	return (NULL);
}
