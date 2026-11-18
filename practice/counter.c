#include <stdio.h>

int counter(void)
{
	static int n;

	n++;
	return (n);
}

int main(void)
{
	printf("%d\n", counter());
	printf("%d\n", counter());
	printf("%d\n", counter());
}