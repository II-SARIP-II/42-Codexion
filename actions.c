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

// time in milliseconde
void debugging(int id, int time, int time_stamp)
{
	usleep(time);
	printf("%d %d is debugging\n", time_stamp, id);
}

// time in milliseconde
void refactoring(int id, int time, int time_stamp)
{
	usleep(time);
	printf("%d %d is refactoring\n", time_stamp, id);
}

void compiling(int id, int time, int time_stamp)
{
	usleep(time);
	printf("%d %d is compiling\n", time_stamp, id);
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
			printf("%d has taken a dongle\n", coder->id);
			pthread_mutex_lock(&coder->d_right->lock);
			printf("%d has taken a dongle\n", coder->id);
		}
		else
		{
			pthread_mutex_lock(&coder->d_right->lock);
			printf("%d has taken a dongle\n", coder->id);
			pthread_mutex_lock(&coder->d_left->lock);
			printf("%d has taken a dongle\n", coder->id);
		}
	    while (coder->d_right->free == 1 && coder->d_left->free == 1)
		{
			pthread_cond_wait(&coder->d_right->cond, &coder->d_right->lock);
			pthread_cond_wait(&coder->d_left->cond, &coder->d_left->lock);
		}

		compiling(coder->id, parsed_datas.time_to_debug, 999);
		coder->comp_count++;
		pthread_mutex_unlock(&coder->d_right->lock);
		pthread_mutex_unlock(&coder->d_left->lock);
		coder->d_left->free = 0;
		coder->d_right->free = 0;
		pthread_cond_broadcast(&coder->d_right->cond);
		pthread_cond_broadcast(&coder->d_left->cond);

		debugging(coder->id, parsed_datas.time_to_debug, 999);
	    refactoring(coder->id, parsed_datas.time_to_refactor, 999);
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