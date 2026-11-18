#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 5
#endif

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

char *make_line(char **left)
{
	int i;
	char *rest;
	char *temp;

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

char *read_helper(char *left, char *buf)
{
	char *line;
	char *temp;

	temp = left;
	line = ft_strjoin(temp, buf);
	free(temp);
	if (!line)
		return (NULL);
	return (line);
}

char *read_file(int fd, char *left)
{
	char *buf;
	int r;

	r = 1;
	while (r > 0 && !ft_strchr(left, '\n'))
	{
		buf = malloc(BUFFER_SIZE + 1);
		if (!buf)
			return (free(left), NULL);
		r = read(fd, buf, BUFFER_SIZE);
		if (r == -1)
			return (free(buf),free(left),NULL);
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

char *gnl(int fd)
{
	static char *left;
	char *line;

	if (fd < 0 || BUFFER_SIZE < 1)
		return (NULL);
	left = read_file(fd, left);
	if (!left)
		return (NULL);
	if (ft_strlen(left) == 0)
	{
		free(left);
		left = NULL;
		return (NULL);
	}
	line = make_line(&left);
	if (!line)
		return (NULL);
	return (line);
}

// char *gnl_bonus(int fd)
// {
// 	static char *left[1024];
// 	char *line;

// 	if (fd < 0 || BUFFER_SIZE < 1)
// 		return (NULL);
// 	left[fd] = read_file(fd, left[fd]);
// 	if (!left[fd])
// 		return (NULL);
// 	if (ft_strlen(left[fd]) == 0)
// 	{
// 		free(left[fd]);
// 		left[fd] = NULL;
// 		return (NULL);
// 	}
// 	line = make_line(&left[fd]);
// 	if (!line)
// 		return (NULL);
// 	return (line);
// }

int	main()
{
	int		fd;
	int		counter;
	char	*line;

	counter = 0;
	fd = open("text.md", O_RDONLY);
	line = gnl(fd);
	while (line)
	{
		counter++;
		printf("Line N%d : %s", counter, line);
		free(line);
		line = gnl(fd);
	}
	close(fd);
}