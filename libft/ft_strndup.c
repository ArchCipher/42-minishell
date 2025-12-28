#include "libft.h"

// musl version uses strnlen to search for null within len ans use that len.
char	*ft_strndup(const char *s1, size_t len)
{
	char	*dst;

	dst = malloc(len + 1);
	if (!dst)
		return (NULL);
	ft_memcpy(dst, s1, len);
	dst[len] = '\0';
	return (dst);
}