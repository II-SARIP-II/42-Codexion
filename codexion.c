/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgougne <pgougne@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 08:52:47 by pgougne           #+#    #+#             */
/*   Updated: 2026/03/12 08:52:52 by pgougne          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include "struct_elements.h"
//debug
#include "unistd.h"

int	manage_threads(t_elements *elements)
{
	pthread_t		*threads;
	t_thread_param	*params;
	int				i;

	threads = malloc(sizeof(pthread_t)
			* elements->parsed_datas.number_of_coders);
	if (!threads)
		return (errors(NULL, NULL, NULL, 1));
	i = -1;
	params = malloc(sizeof(t_thread_param)
			* elements->parsed_datas.number_of_coders);
	if (!params)
		return (errors(threads, NULL, NULL, 1));
	params->elements = elements;
	while (++i < elements->parsed_datas.number_of_coders)
	{
		params[i].elements = elements;
		params[i].idx = i;
		pthread_create(&threads[i], NULL, actions_loop, &params[i]);
	}
	i = -1;
	while (++i < elements->parsed_datas.number_of_coders)
		pthread_join(threads[i], NULL);
	return (errors(threads, params, NULL, 0));
}

int	main(int argc, char **argv)
{
	t_parsed	parsed_datas;
	t_elements	*elements;

	parsed_datas.number_of_coders = 0;
	if (argc != 9)
	{
		fprintf(stderr, "Error: there are too many/few arguments\n");
		return (1);
	}
	if (fill_data(&parsed_datas, argv) == -1)
	{
		fprintf(stderr, "Error: invalid argument\n");
		return (1);
	}
	elements = init_datas(parsed_datas);
	if (!elements)
	{
		fprintf(stderr, "Error: failed to allocate memory\n");
		return (1);
	}
	manage_threads(elements);
	return (errors(elements->coders, elements->dongles, elements, 0));
}
