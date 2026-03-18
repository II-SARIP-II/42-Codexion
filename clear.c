/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgougne <pgougne@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 11:15:53 by pgougne           #+#    #+#             */
/*   Updated: 2026/03/13 11:15:59 by pgougne          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "codexion.h"

int	errors(void *elem1, void *elem2, void *elem3, int return_value)
{
	if (elem1)
		free(elem1);
	if (elem2)
		free(elem2);
	if (elem3)
		free(elem3);
	return (return_value);
}

int	clear_mutex(t_elements *elements)
{
	int	i;

	i = 0;
	while (i < elements->parsed_datas.number_of_coders)
	{
		pthread_mutex_destroy(&elements->dongles[i].lock);
		i++;
	}
	pthread_mutex_destroy(&elements->print_lock);
	pthread_mutex_destroy(&elements->state_lock);
	return (0);
}

void	clear_queue(t_elements *elements)
{
	int	i;

	i = -1;
	while (++i < elements->parsed_datas.number_of_coders)
		free(elements->dongles[i].queue);
}
