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

int	main(int argc, char **argv)
{
	t_parsed	parsed_datas;
	t_elements	*elements;
	int		i;

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
	elements = init_datas(parsed_datas.number_of_coders);
	if (!elements)
	{
		fprintf(stderr, "Error: failed to allocate memory\n");
		return (1);
	}
	printf("nb_code: %d\nt_burnout: %d\nt_compile: %d\nt_debug: %d\nt_refactor: %d\nnb_compile_required: %d\ndongle_cooldown: %d\nscheduler: %d", parsed_datas.number_of_coders, parsed_datas.time_to_burnout, parsed_datas.time_to_compile, parsed_datas.time_to_debug, parsed_datas.time_to_refactor, parsed_datas.number_of_compiles_required, parsed_datas.dongle_cooldown, parsed_datas.scheduler);
	printf("\n\nid  |   left     |    right\n");
	while (i < parsed_datas.number_of_coders)
	{
		printf("%d   |", elements->coders[i].id);
		printf(" %p | ", elements->coders[i].d_left);
		printf("%p\n", elements->coders[i].d_right);
		i++;
	}
}