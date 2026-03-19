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

static int	create_threads(t_elements *el, pthread_t *th, t_thread_param *p)
{
	int	i;

	i = 0;
	while (i < el->parsed_datas.number_of_coders)
	{
		p[i].elements = el;
		p[i].idx = i;
		if (pthread_create(&th[i], NULL, actions_loop, &p[i]) != 0)
		{
			pthread_mutex_lock(&el->state_lock);
			el->stop_sim = 1;
			pthread_mutex_unlock(&el->state_lock);
			break ;
		}
		i++;
	}
	return (i);
}

int	manage_threads(t_elements *elements)
{
	pthread_t		*threads;
	pthread_t		manager_thread;
	t_thread_param	*params;
	int				i;
	int				created_count;

	threads = malloc(sizeof(pthread_t)
			* elements->parsed_datas.number_of_coders);
	params = malloc(sizeof(t_thread_param)
			* elements->parsed_datas.number_of_coders);
	if (!params || !threads)
		return (errors(threads, params, NULL, 1));
	params->elements = elements;
	if (pthread_create(&manager_thread, NULL, manager, elements) != 0)
		return (errors(threads, params, NULL, 1));
	created_count = create_threads(elements, threads, params);
	i = -1;
	while (++i < created_count)
		pthread_join(threads[i], NULL);
	pthread_join(manager_thread, NULL);
	return (errors(threads, params, NULL, 0));
}

int	print_errors(char *msg, int return_value)
{
	fprintf(stderr, "%s\n", msg);
	return (return_value);
}

int	main(int argc, char **argv)
{
	t_parsed	parsed_datas;
	t_elements	*elements;

	if (argc != 9)
		return (print_errors("Error: wrong number of arguments\n", 1));
	if (fill_data(&parsed_datas, argv) == -1)
		return (print_errors("Error: invalid argument\n", 1));
	elements = init_datas(&parsed_datas);
	if (!elements)
		return (print_errors("Error: failed to allocate memory\n", 1));
	if (manage_threads(elements) == 1)
	{
		fprintf(stderr, "Error: failed to create threads\n");
		clear_mutex(elements);
		clear_queue(elements);
		return (errors(elements->coders, elements->dongles, elements, 1));
	}
	clear_mutex(elements);
	clear_queue(elements);
	return (errors(elements->coders, elements->dongles, elements, 0));
}
