/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amoutia <amoutia@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 11:15:28 by amoutia           #+#    #+#             */
/*   Updated: 2026/11/18 16:22:25 by amoutia          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int ft_strlen(char *str)
{
	int i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
		i++;
	return (i);
}

char *ft_strchr(char *s, int c)
{
	int i;

	if (!s)
		return (NULL);
	i = 0;
	while (s[i])
	{
		if (s[i] == (char)c)
			return ((char *)&s[i]);
		i++;
	}
	if ((char)c == '\0')
		return ((char *)&s[i]);
	return (NULL);
}

char *ft_substr(char *s, int start, int len)
{
	int slen;
	char *subs;
	int i;

	if (!s)
		return (NULL);
	slen = ft_strlen(s);
	if (start >= slen || len == 0)
	{
		subs = malloc(1);
		if (!subs)
			return (NULL);
		subs[0] = '\0';
		return (subs);
	}
	i = 0;
	if (len > slen - start)
		len = slen - start;
	subs = malloc(len + 1);
	if (!subs)
		return (NULL);
	while (s[start] && i < len)
		subs[i++] = s[start++];
	subs[i] = '\0';
	return (subs);
}

char *ft_strjoin(char *s1, char *s2)
{
	int len;
	char *s;
	int i;
	int j;

	if (!s1)
		return (ft_substr(s2, 0, ft_strlen(s2)));
	else if (!s2)
		return (ft_substr(s1, 0, ft_strlen(s1)));
	len = ft_strlen(s1) + ft_strlen(s2);
	s = malloc(len + 1);
	if (!s)
		return (NULL);
	i = 0;
	while (s1[i])
	{
		s[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2[j])
		s[i++] = s2[j++];
	s[len] = '\0';
	return (s);
}
