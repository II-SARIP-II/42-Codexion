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

int	manage_threads(t_elements *elements)
{
	pthread_t		*threads;
	pthread_t		manager_thread;
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
	pthread_create(&manager_thread, NULL, manager, elements);
	while (++i < elements->parsed_datas.number_of_coders)
	{
		params[i].elements = elements;
		params[i].idx = i;
		pthread_create(&threads[i], NULL, actions_loop, &params[i]);
	}
	i = -1;
	while (++i < elements->parsed_datas.number_of_coders)
		pthread_join(threads[i], NULL);
	pthread_join(manager_thread, NULL);
	free(threads);
	free(params);
	return (0);
}

int	clear_threads(t_elements *elements)
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

int	main(int argc, char **argv)
{
	t_parsed	parsed_datas;
	t_elements	*elements;
	int			i;

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
	elements = init_datas(&parsed_datas);
	if (!elements)
	{
		fprintf(stderr, "Error: failed to allocate memory\n");
		return (1);
	}
	manage_threads(elements);
	clear_threads(elements);
	i = -1;
	while (++i < parsed_datas.number_of_coders)
		free(elements->dongles[i].queue);
	return (errors(elements->coders, elements->dongles, elements, 0));
}
