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
#include <unistd.h>

long get_delta_time(struct timeval *start)
{
    struct timeval end;
    long delta_ms;

    gettimeofday(&end, NULL);

    delta_ms = (end.tv_sec - start->tv_sec) * 1000;
    delta_ms += (end.tv_usec - start->tv_usec) / 1000;
    return (delta_ms);
}

void log_action(int id, char *msg, t_elements *elements)
{
	pthread_mutex_lock(&elements->print_lock);

	if (!elements->stop_sim)
	{
		printf("%ld %d %s\n", get_delta_time(&elements->start_time), id, msg);
	}
	pthread_mutex_unlock(&elements->print_lock);
}

void precise_sleep(long sleep_time_ms, t_elements *elements)
{
	long start = get_delta_time(&elements->start_time);

	while (get_delta_time(&elements->start_time) < start + sleep_time_ms)
	{
		usleep(500);
	}
}

void debugging(int id, int time, t_elements *elements)
{
	precise_sleep(time, elements);
	log_action(id, "is debugging", elements);
}

void refactoring(int id, int time, t_elements *elements)
{
	precise_sleep(time, elements);
	log_action(id, "is refactoring", elements);
}

void compiling(int id, int time, t_elements *elements)
{
	precise_sleep(time, elements);
	log_action(id, "is compiling", elements);
}

// arg : 	 (void *)&thread_param);
void	*actions_loop(void *arg)
{
	t_thread_param	*thread_param = (t_thread_param *)arg;
	t_coder	*coder = &thread_param->elements->coders[thread_param->idx];   //CRASHING HERE
	t_parsed	parsed_datas = thread_param->elements->parsed_datas;
	while (1)
	{
		if (coder->comp_count >= parsed_datas.number_of_compiles_required)
			break;
		if (coder->id % 2 == 0)
		{
			pthread_mutex_lock(&coder->d_left->lock);
			pthread_mutex_lock(&coder->d_right->lock);
			printf("%d has taken a dongle\n", coder->id);
			printf("%d has taken a dongle\n", coder->id);
		}
		else
		{
			pthread_mutex_lock(&coder->d_right->lock);
			pthread_mutex_lock(&coder->d_left->lock);
			printf("%d has taken a dongle\n", coder->id);
			printf("%d has taken a dongle\n", coder->id);
		}
	    while (coder->d_right->free == 1 && coder->d_left->free == 1)
		{
			pthread_cond_wait(&coder->d_right->cond, &coder->d_right->lock);
			pthread_cond_wait(&coder->d_left->cond, &coder->d_left->lock);
		}
		// verifier tout le temps le shutdown

		compiling(coder->id, parsed_datas.time_to_debug, thread_param->elements);
		coder->comp_count++;
		pthread_mutex_unlock(&coder->d_right->lock);
		printf("%d has released a dongle\n", coder->id);

		pthread_mutex_unlock(&coder->d_left->lock);
		printf("%d has released a dongle\n", coder->id);
		coder->d_left->free = 0;
		coder->d_right->free = 0;
		pthread_cond_broadcast(&coder->d_right->cond);
		pthread_cond_broadcast(&coder->d_left->cond);

		debugging(coder->id, parsed_datas.time_to_debug, thread_param->elements);
	    refactoring(coder->id, parsed_datas.time_to_refactor, thread_param->elements);
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