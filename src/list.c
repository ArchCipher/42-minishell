/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 14:09:03 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 21:07:55 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
DESCRIPTION:
	Adds a new node to the end of the list.

NOTE:
	Must update tail in the caller function to maintain the list.
*/

void	lstadd_back(void **head, void *new, void *last, t_node_type type)
{
	if (!head || !new)
		return ;
	if (*head && !last)
	{
		perr_msg("lstadd_back: last is NULL", NULL, NULL);
		return ;
	}
	if (!*head)
		*head = new;
	else
	{
		if (type == TYPE_TOKEN)
			((t_token *)last)->next = (t_token *)new;
		else if (type == TYPE_CMD)
			((t_cmd *)last)->next = (t_cmd *)new;
		else if (type == TYPE_REDIR)
			((t_redir *)last)->next = (t_redir *)new;
		else if (type == TYPE_ENV)
			((t_env *)last)->next = (t_env *)new;
	}
}

void	perr_msg(const char *s1, const char *s2, const char *s3)
{
	ft_dprintf(FD_ERR, "%s", MINI);
	ft_dprintf(FD_ERR, ": %s", s1);
	if (s2)
		ft_dprintf(FD_ERR, ": %s", s2);
	if (s3)
		ft_dprintf(FD_ERR, ": %s", s3);
	write(FD_ERR, "\n", 1);
}
