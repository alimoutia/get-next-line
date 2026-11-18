/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amoutia <amoutia@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 11:15:16 by amoutia           #+#    #+#             */
/*   Updated: 2026/11/18 18:40:46 by amoutia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static char	*make_line(char **left)
{
	int		i;
	char	*rest;
	char	*temp;

	i = 0;
	while ((*left)[i] && (*left)[i] != '\n')
		i++;
	if ((*left)[i] == '\n')
		i++;
	temp = ft_substr(*left, 0, i);
	rest = ft_substr(*left, i, ft_strlen(*left));
	free(*left);
	*left = rest;
	if (!rest || !temp)
	{
		free(rest);
		free(temp);
		*left = NULL;
		return (NULL);
	}
	return (temp);
}

static char	*read_helper(char *left, char *buf)
{
	char	*line;

	line = ft_strjoin(left, buf);
	free(left);
	return (line);
}

static char	*read_file(int fd, char *left)
{
	char	*buf;
	int		r;

	r = 1;
	while (r > 0 && !ft_strchr(left, '\n'))
	{
		buf = malloc((size_t)BUFFER_SIZE + 1);
		if (!buf)
			return (free(left), NULL);
		r = read(fd, buf, BUFFER_SIZE);
		if (r == -1)
			return (free(buf), free(left), NULL);
		buf[r] = '\0';
		if (r == 0)
		{
			free(buf);
			break ;
		}
		left = read_helper(left, buf);
		free(buf);
		if (!left)
			return (NULL);
	}
	return (left);
}

char	*get_next_line(int fd)
{
	static char	*left[ARR_SIZE];
	char		*line;

	if (fd < 0 || fd >= ARR_SIZE || BUFFER_SIZE < 1)
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
