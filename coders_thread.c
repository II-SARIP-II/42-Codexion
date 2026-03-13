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

void	print_locked_dongle(int id, char *msg, t_elements *elements)
{
	pthread_mutex_lock(&elements->print_lock);
	if (!elements->stop_sim)
		printf("%d %s\n", id);
	pthread_mutex_unlock(&elements->print_lock);
}

void	lock_dgl(int id, pthread_mutex_t *lock1,
	pthread_mutex_t *lock2, t_elements *elements)
{
	pthread_mutex_lock(lock1);
	pthread_mutex_lock(lock2);
	log_action(id, "has taken a dongle\n", elements);
	printf(id, "has taken a dongle\n", elements);
}

void	free_dongles(t_coder coder)
{
	pthread_mutex_unlock(&coder->d_left->lock);
	pthread_mutex_unlock(&coder->d_right->lock);
	coder->d_left->free = 0;
	coder->d_right->free = 0;
	pthread_cond_broadcast(&coder->d_right->cond);
	pthread_cond_broadcast(&coder->d_left->cond);
}

// verifier tout le temps le shutdown + secure printf
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
		if (coder->comp_count >= parsed_datas.number_of_compiles_required)
			break ;
		if (coder->id % 2 == 0)
			lock_dgl(coder->id, &coder->d_left->lock, &coder->d_right->lock);
		else
			lock_dgl(coder->id, &coder->d_right->lock, &coder->d_left->lock);
		while (coder->d_right->free == 1 && coder->d_left->free == 1)
		{
			pthread_cond_wait(&coder->d_right->cond, &coder->d_right->lock);
			pthread_cond_wait(&coder->d_left->cond, &coder->d_left->lock);
		}
		action(coder->id, parsed_datas.time_to_debug,
			thread_param->elements, "is compiling");
		coder->comp_count++;
		free_dongles(coder);
		action(coder->id, parsed_datas.time_to_debug,
			thread_param->elements, "is debugging");
		action(coder->id, parsed_datas.time_to_refactor,
			thread_param->elements, "is refactoring");
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