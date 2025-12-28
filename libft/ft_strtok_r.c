#include "libft.h"

char	*ft_strtok_r(char *s, const char *sep, char **p)
{
	if (!s && !*p)
		return (NULL);
	else if (!s)
		s = *p;
	while (*s && ft_strchr(sep, *s))
		s++;
	if (!*s)
		return (*p = NULL);
	*p = s;
	while (**p && !ft_strchr(sep, **p))
		(*p)++;
	if (**p)
		*(*p)++ = 0;
	else
		*p = NULL;
	return (s);
}