/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_manager.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgougne <pgougne@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 08:56:11 by pgougne           #+#    #+#             */
/*   Updated: 2026/03/16 08:56:15 by pgougne          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <unistd.h>

int	verif(int *count, t_elements *el)
{
	int			i;
	t_parsed	psd;

	psd = el->parsed_datas;
	i = 0;
	while (i < psd.number_of_coders)
	{
		pthread_mutex_lock(&el->state_lock);
		if (get_delta_time(&el->coders[i].last_comp_start)
			>= psd.time_to_burnout)
		{
			log_action(el->coders[i].id, "burned out", el);
			el->stop_sim = 1;
			pthread_mutex_unlock(&el->state_lock);
			return (-1);
		}
		if (el->coders[i].comp_count >= psd.number_of_compiles_required)
			(*count)++;
		pthread_mutex_unlock(&el->state_lock);
		i++;
	}
	return (0);
}

void	*manager(void *elements_param)
{
	t_elements	*elements;
	int			count;
	t_parsed	psd;
	t_coder		*coders;

	elements = elements_param;
	psd = elements->parsed_datas;
	coders = elements->coders;
	while (1)
	{
		count = 0;
		if (verif(&count, elements) == -1)
			return (NULL);
		if (count >= psd.number_of_coders)
		{
			pthread_mutex_lock(&elements->state_lock);
			elements->stop_sim = 1;
			pthread_mutex_unlock(&elements->state_lock);
			break ;
		}
		usleep(100);
	}
	return (NULL);
}
