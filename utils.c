/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 17:28:00 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/04 17:52:55 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_isspace(int c)
{
	return (c == ' ' || (c >= '\t' && c <= '\r'));
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