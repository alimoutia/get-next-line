#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 5
#endif

int main()
{
	ssize_t rread = 1;
	int counter = 1;
	int fd = open("text.md", O_RDONLY);
	if (fd == -1)
	{
		printf("%s", "OPEN failed and returned -1");
		return (0);
	}
	char *str;
	str = malloc(BUFFER_SIZE + 1);
	if (!str)
	{
		printf("%s", "malloc failed the allocation!");
		close(fd);
		return 0;
	}
	while (rread > 0)
	{
		rread = read(fd, str, BUFFER_SIZE);
		if (rread <= 0)
			break;
		str[rread] = '\0';
		// write(1, str, rread);
		printf("%s\nread counts : %d\n", str, counter++);
	}
	close(fd);
	free(str);
	if (rread == 0)
		printf("read finished reading and returned %zd", rread);
	if (rread == -1)
		printf("read failed and returned %zd", rread);
}
