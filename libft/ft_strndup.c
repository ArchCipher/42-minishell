/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strndup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:13 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:39:34 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
