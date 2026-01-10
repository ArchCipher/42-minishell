/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lst_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:45:03 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/07 17:29:29 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
DESCRIPTION:
	Frees the token and its contents.
*/

void	free_token(void *content)
{
	t_token	*token;

	token = (t_token *)content;
	if (!token)
		return ;
	free(token->word);
	free(token);
}

/*
DESCRIPTION:
	Frees the command lis content. Closes any open fds.
*/

void	free_cmd(void *content)
{
	t_cmd	*cmd;

	cmd = (t_cmd *)content;
	free_arr(cmd->args);
	ft_lstclear(&cmd->redirs, free_redir);
	if (cmd->subshell)
		ft_lstclear(&cmd->subshell, free_cmd);
	free(cmd);
}

/*
DESCRIPTION:
	Frees the command list's content. Closes any open fds.
*/

void	free_redir(void *content)
{
	t_redir	*redir;

	redir = (t_redir *)content;
	free(redir->file);
	if (redir->fd != -1)
		close(redir->fd);
	free(redir);
}

/*
DESCRIPTION:
	Frees the string array.
*/

void	free_arr(char **arr)
{
	size_t	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

/*
DESCRIPTION:
	Frees the environment list's content.
*/

void	free_env(void *content)
{
	t_env	*env;

	env = (t_env *)content;
	free(env->key);
	if (env->value)
		free(env->value);
	free(env);
}
