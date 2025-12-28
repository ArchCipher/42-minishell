/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 18:27:10 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 19:44:02 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

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

/*
// musl version simply uses ft_strlen and not
// strndup(as strndup uses strnlen to check again)
// but strnlen uses memchr() and not strchr()
char	*ft_strdup(const char *s1)
{
	return (ft_strndup(s1, ft_strlen(s1)));
}
*/