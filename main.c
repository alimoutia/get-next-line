#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"
// #include "get_next_line_bonus.h"

void	ft_toupper(char *line)
{
	int i = 0;

	while (line[i])
	{
		if (line[i] >= 'a' && line[i] <= 'z')
			line[i] = line[i] - 32;
		i++;
	}
	printf("%s", line);
}

int main()
{
	int fd = open("text.txt", O_RDONLY);
	char *str;
	int counter = 0;
	
	while (counter <= 2)
	{
		counter++;
		str = get_next_line(fd);
		// printf("Line N %d : %s", counter, str);
		if (counter <= 2)
			free(str);
	}
	ft_toupper(str);
	free(str);
	close(fd);
	return 0;
}

// int main()
// {
// 	int fd1 = open("text.md", O_RDONLY);
// 	int fd2 = open("text.txt", O_RDONLY);
// 	char *str;
// 	char *line2;

// 	str = get_next_line(fd1);
// 	printf("%s", str);
// 	free(str);
// 	line2 = get_next_line(fd2);
// 	printf("%s", line2);
// 	free(line2);
// 	line2 = get_next_line(fd2);
// 	printf("%s", line2);
// 	free(line2);
// 	line2 = get_next_line(fd2);
// 	printf("%s", line2);
// 	free(line2);
// 	str = get_next_line(fd1);
// 	printf("%s", str);
// 	free(str);
// 	return 0;
// }