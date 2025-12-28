#include "libft.h"

char	*ft_strjoin3(const char *s1, const char *s2, const char *s3)
{
	size_t	slen1;
	size_t	slen2;
	size_t	slen3;
	char	*dst;

	if (!s1 || !s2 || !s3)
		return (NULL);
	slen1 = ft_strlen(s1);
	slen2 = ft_strlen(s2);
	slen3 = ft_strlen(s3);
	dst = malloc(slen1 + slen2 + slen3 + 1);
	if (!dst)
		return (NULL);
	ft_memcpy(dst, s1, slen1);
	ft_memcpy(dst + slen1, s2, slen2);
	ft_memcpy(dst + slen1 + slen2, s3, slen3);
	dst[slen1 + slen2 + slen3] = '\0';
	return (dst);
}
