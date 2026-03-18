/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgougne <pgougne@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 09:52:54 by pgougne           #+#    #+#             */
/*   Updated: 2026/03/10 09:52:58 by pgougne          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdlib.h>
# include <limits.h>
# include <stdio.h>
# include <string.h>
# include <pthread.h>
# include "struct_elements.h"

t_elements	*init_datas(t_parsed *parsed_datas);
int			fill_data(t_parsed *datas, char **argv);
void		*actions_loop(void *arg);
int			errors(void *elem1, void *elem2, void *elem3, int return_value);
void		precise_sleep(long sleep_time_ms, t_elements *elements);
long		get_delta_time(struct timeval *start);
void		log_action(int id, char *msg, t_elements *elements);
void		action(int id, int time, t_elements *elements, char *msg);
void		*manager(void *elements);
int			edf(t_dongle *d, t_coder *coder, t_elements *el);
int			fifo(t_dongle *d, t_coder *coder);
int			is_priority(t_dongle *d, t_coder *coder);
void		remove_from_queue(t_dongle *d, t_coder *coder);
void		free_dongles(t_coder *coder);
int			try_to_grab_dongles(t_coder *coder, t_elements *elements);
int			add_to_queue(t_dongle *d, t_coder *coder);

#endif
