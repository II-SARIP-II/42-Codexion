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

void	*manager(void *elements_param)
{
	t_elements	*elements;
	int	count;	// count how many people finished
	int	i;

	elements = elements_param;
	while (1)
	{
		count = 0;
		if (count >= elements->parsed_datas.number_of_coders)
			break ;
		i = 0;
		while (i < elements->parsed_datas.number_of_coders)
		{
			if (elements->coders[i].burnout == 0)
			{
				printf("ECHOOOOO");
				elements->stop_sim = 1;
				return (NULL) ;
			}
			if (elements->coders[i].comp_count >= elements->parsed_datas.number_of_compiles_required)
				count++;
			i++;
		}
	}
	return (NULL);
}