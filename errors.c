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
