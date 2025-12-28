#include "libft.h"

void	*ft_realloc(void *ptr, size_t old_size, size_t size)
{
	char	*new;

	new = malloc(size);
	if (!new)
	{
		free (ptr);
		return (NULL);
	}
	if (ptr)
		ft_memcpy(new, ptr, old_size);
	free(ptr);
	return (new);
}