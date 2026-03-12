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
	int	i;

	write(1, "echo\n", 5);
	threads = malloc(sizeof(pthread_t) * elements->parsed_datas.number_of_coders);
	if (!threads)
		return (1);
	i = 0;
	params = malloc(sizeof(t_thread_param) * elements->parsed_datas.number_of_coders);
	if (!params)
	{
		free(threads);
		return (1);
	}
	params->elements = elements;
	while (i < elements->parsed_datas.number_of_coders)
	{
		params[i].elements = elements;
		params[i].idx = i;
		pthread_create(&threads[i], NULL, actions_loop, &params[i]);
		i++;
	}
	write(1, "echa\n", 5);
	i = 0;
	while (i < elements->parsed_datas.number_of_coders)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	write(1, "echu\n", 5);
	return (0);
}

int	main(int argc, char **argv)
{
	t_parsed	parsed_datas;
	t_elements	*elements;
	int			i;

	i = 0;
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
	printf("nb_code: %d\nt_burnout: %d\nt_compile: %d\nt_debug: %d\nt_refactor: %d\nnb_compile_required: %d\ndongle_cooldown: %d\nscheduler: %d", parsed_datas.number_of_coders, parsed_datas.time_to_burnout, parsed_datas.time_to_compile, parsed_datas.time_to_debug, parsed_datas.time_to_refactor, parsed_datas.number_of_compiles_required, parsed_datas.dongle_cooldown, parsed_datas.scheduler);
	printf("\n\nid  |   left     |    right\n");
	while (i < elements->parsed_datas.number_of_coders)
	{
		printf("%d   |", elements->coders[i].id);
		printf(" %p | ", elements->coders[i].d_left);
		printf("%p\n", elements->coders[i].d_right);
		i++;
	}
}
