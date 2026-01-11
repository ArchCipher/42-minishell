/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cast.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 18:59:45 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/11 18:59:46 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*get_tok(t_list *lst)
{
	if (!lst)
		return (NULL);
	return ((t_token *)lst->content);
}

t_token_type	get_tok_type(t_list *lst)
{
	t_token	*token;

	token = get_tok(lst);
	if (token)
		return (token->type);
	return (NONE);
}

t_cmd	*get_cmd(t_list *lst)
{
	if (!lst)
		return (NULL);
	return ((t_cmd *)lst->content);
}

t_redir	*get_redir(t_list *lst)
{
	if (!lst)
		return (NULL);
	return ((t_redir *)lst->content);
}

t_env	*get_env(t_list *lst)
{
	if (!lst)
		return (NULL);
	return ((t_env *)lst->content);
}
