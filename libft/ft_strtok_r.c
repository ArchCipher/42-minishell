/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtok_r.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:21 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:39:09 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
