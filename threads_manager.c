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

void	*manager(void *elements_param)
{
	t_elements	*elements;
	int			count;
	int			i;
	t_parsed	psd;
	t_coder		*coders;

	elements = elements_param;
	psd = elements->parsed_datas;
	coders = elements->coders;
	while (1)
	{
		count = 0;
		i = 0;
		while (i < psd.number_of_coders)
		{
			pthread_mutex_lock(&elements->state_lock);
			if (get_delta_time(&coders[i].last_comp_start) >= psd.time_to_burnout)
			{
				log_action(coders[i].id, "burned out", elements);
				elements->stop_sim = 1;
				pthread_mutex_unlock(&elements->state_lock);
				return (NULL);
			}
			if (coders[i].comp_count >= psd.number_of_compiles_required)
				count++;
			pthread_mutex_unlock(&elements->state_lock);
			i++;
		}
		if (count >= psd.number_of_coders)
			break ;
		usleep(200);
	}
	return (NULL);
}
