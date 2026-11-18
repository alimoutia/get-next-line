/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amoutia <amoutia@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 11:15:16 by amoutia           #+#    #+#             */
/*   Updated: 2026/11/18 16:25:05 by amoutia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

char *gnl_bonus(int fd)
{
	static char *left[1024];
	char *line;

	if (fd < 0 || BUFFER_SIZE < 1)
		return (NULL);
	left[fd] = read_file(fd, left[fd]);
	if (!left[fd])
		return (NULL);
	if (ft_strlen(left[fd]) == 0)
	{
		free(left[fd]);
		left[fd] = NULL;
		return (NULL);
	}
	line = make_line(&left[fd]);
	if (!line)
		return (NULL);
	return (line);
}
