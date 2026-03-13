/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_elements.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgougne <pgougne@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 12:38:32 by pgougne           #+#    #+#             */
/*   Updated: 2026/03/11 12:38:36 by pgougne          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCT_ELEMENTS_H
# define STRUCT_ELEMENTS_H
# include <sys/time.h>
# include <pthread.h>

typedef struct s_parsed
{
	int	number_of_coders;
	int	time_to_burnout;
	int	time_to_compile;
	int	time_to_debug;
	int	time_to_refactor;
	int	number_of_compiles_required;
	int	dongle_cooldown;
	int	scheduler;
}	t_parsed;

struct		s_coder;

typedef struct s_dongle
{
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	struct timeval	lr_time;	// last released time
	struct s_coder	*queue;		// Priority Queue
	int				scheduler;
	int				free; 		// 0 = true 1 = false
}	t_dongle;

typedef struct s_coder
{
	int				id;
	struct timeval	last_comp_start;
	int				comp_count;
	t_dongle		*d_left;
	t_dongle		*d_right;
}	t_coder;

// maybe add time parameter to manage the general time like the output
typedef struct s_elements
{
	t_coder		*coders;
	t_dongle	*dongles;
	t_parsed	parsed_datas;
	struct timeval	start_time;
	pthread_mutex_t	print_lock;
	int				stop_sim;
}	t_elements;

typedef struct s_thread_param
{
	t_elements	*elements;
	int			idx;
}	t_thread_param;

#endif