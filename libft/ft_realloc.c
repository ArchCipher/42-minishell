/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:43:52 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:38:58 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"


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
