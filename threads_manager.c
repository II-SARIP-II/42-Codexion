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
			if (coders[i].burnout == 0)
			{
				log_action(i, "burned out", elements);
				elements->stop_sim = 1;
				return (NULL);
			}
			if (coders[i].comp_count >= psd.number_of_compiles_required)
				count++;
			i++;
		}
		if (count >= psd.number_of_coders)
			break ;
	}
	return (NULL);
}
