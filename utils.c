/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 17:28:00 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/10 21:03:13 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_isspace(int c)
{
	return (c == ' ' || (c >= '\t' && c <= '\r'));
}

int	ft_isalnum(int c)
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'));
}

void	*ft_memset(void *b, int c, size_t len)
{
	size_t	i;

	i = 0;
	while (i < len)
	{
		((unsigned char *)b)[i] = (unsigned char)c;
		i++;
	}
	return (b);
}

char	*ft_strchr(const char *s, int c)
{
	while (*s)
	{
		if (*s == (unsigned char)c)
			return ((char *)s);
		s++;
	}
	if ((unsigned char)c == '\0')
		return ((char *)s);
	return (NULL);
}
/*
size_t	ft_strspn(const char *s, const char *accept)
{
	size_t	i;

	i = 0;
	while (s[i] && ft_strchr(accept, s[i]))
		i++;
	return (i);
}

size_t	ft_strcspn(const char *s, const char *reject)
{
	size_t	i;

	i = 0;
	while (s[i] && !ft_strchr(reject, s[i]))
		i++;
	return (i);
}
*/

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


int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
		return (0);
	while (n-- > 1 && *s1 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (((int)(unsigned char)*s1) - ((int)(unsigned char)*s2));
}

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char			*d;
	const unsigned char		*s;

	d = (unsigned char *)dst;
	s = (unsigned char *)src;
	while (n-- > 0)
		*d++ = *s++;
	return (dst);
}

char	*ft_strdup(const char *s1)
{
	char	*dst;
	size_t	len;

	len = ft_strlen(s1);
	dst = malloc(len + 1);
	if (!dst)
		return (NULL);
	ft_memcpy(dst, s1, len);
	dst[len] = '\0';
	return (dst);
}

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

size_t	ft_strlen(const char *s)
{
	size_t	n;

	n = 0;
	while (s[n])
		n++;
	return (n);
}

/*
libc realloc doesn't free ptr when malloc fails,
but this version frees ptr when malloc fails.
*/

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