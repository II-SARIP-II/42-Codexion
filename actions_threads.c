/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_thread.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgougne <pgougne@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 11:38:13 by pgougne           #+#    #+#             */
/*   Updated: 2026/03/13 11:38:19 by pgougne          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "struct_elements.h"
#include <unistd.h>
#include <stdio.h>

long	get_delta_time(struct timeval *start)
{
	struct timeval	end;
	long			delta_ms;

	gettimeofday(&end, NULL);
	delta_ms = (end.tv_sec - start->tv_sec) * 1000;
	delta_ms += (end.tv_usec - start->tv_usec) / 1000;
	return (delta_ms);
}

void	precise_sleep(long sleep_time_ms, t_elements *elements)
{
	long	start;

	start = get_delta_time(&elements->start_time);
	while (get_delta_time(&elements->start_time) < start + sleep_time_ms)
		usleep(100);
}

void	log_action(int id, char *msg, t_elements *elements)
{
	int stop;

	pthread_mutex_lock(&elements->state_lock);
	stop = elements->stop_sim;
	pthread_mutex_unlock(&elements->state_lock);

	if (stop)
		return;

	pthread_mutex_lock(&elements->print_lock);
	printf("%ld %d %s\n", get_delta_time(&elements->start_time), id, msg);
	pthread_mutex_unlock(&elements->print_lock);
}

void	action(int id, int time, t_elements *elements, char *msg)
{
	precise_sleep(time, elements);
	log_action(id, msg, elements);
}
