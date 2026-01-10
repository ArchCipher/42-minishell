/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:58:58 by kmurugan          #+#    #+#             */
/*   Updated: 2025/10/16 18:49:51 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new, t_list **last)
{
	if (!lst || !new || (*lst && !last))
		return ;
	if (!*lst)
		*lst = new;
	else
		(*last)->next = new;
	*last = new;
}
