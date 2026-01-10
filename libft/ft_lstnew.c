/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 18:22:41 by kmurugan          #+#    #+#             */
/*   Updated: 2025/10/16 18:23:01 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew(size_t size)
{
	t_list	*new;
	void	*content;

	content = malloc(size);
	if (!content)
		return (NULL);
	new = malloc(sizeof(t_list));
	if (!new)
		return (free(content), NULL);
	new->content = content;
	new->next = NULL;
	return (new);
}