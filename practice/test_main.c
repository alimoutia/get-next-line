#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#ifndef GNL
# define GNL get_next_line
#endif

char	*GNL(int fd);

int	main(int ac, char **av)
{
	int		fd;
	char	*line;

	if (ac < 2)
		return (1);
	fd = open(av[1], O_RDONLY);
	line = GNL(fd);
	while (line)
	{
		fwrite(line, 1, strlen(line), stdout);
		free(line);
		line = GNL(fd);
	}
	close(fd);
	return (0);
}
