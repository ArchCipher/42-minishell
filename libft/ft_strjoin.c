/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 18:27:26 by kmurugan          #+#    #+#             */
/*   Updated: 2025/10/16 18:38:21 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(const char *s1, const char *s2)
{
	size_t	slen1;
	size_t	slen2;
	char	*dst;

	if (!s1 || !s2)
		return (NULL);
	slen1 = ft_strlen(s1);
	slen2 = ft_strlen(s2);
	dst = malloc(slen1 + slen2 + 1);
	if (!dst)
		return (NULL);
	ft_memcpy(dst, s1, slen1);
	ft_memcpy(dst + slen1, s2, slen2);
	dst[slen1 + slen2] = '\0';
	return (dst);
}
