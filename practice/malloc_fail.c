#include <stdlib.h>

void	*__real_malloc(size_t size);

static int	g_count;

void	*__wrap_malloc(size_t size)
{
	char	*at;

	at = getenv("FAIL_AT");
	if (at && ++g_count == atoi(at))
		return (NULL);
	return (__real_malloc(size));
}
