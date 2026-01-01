/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:45:03 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 21:43:19 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
DESCRIPTION:
	Frees the tokens and their content.
*/

void	free_tokens(t_token *tokens, bool free_content, t_token *end)
{
	t_token	*current;

	while (tokens && tokens != end)
	{
		current = tokens;
		tokens = tokens->next;
		if (current->token && free_content && current->type == WORD)
			free(current->token);
		free(current);
	}
	while (tokens)
	{
		current = tokens;
		tokens = tokens->next;
		free(current);
	}
}

/*
DESCRIPTION:
	Frees the command list and their content. Closes any open fds.
*/
void	free_cmds(t_cmd *cmds)
{
	t_cmd	*tmp;
	t_redir	*cur_redir;
	size_t	i;
	t_cmd	*cmd;

	cmd = cmds;
	while (cmd)
	{
		i = 0;
		while (cmd->args[i])
			free(cmd->args[i++]);
		free(cmd->args);
		while (cmd->redirs)
		{
			cur_redir = cmd->redirs;
			cmd->redirs = cmd->redirs->next;
			free(cur_redir->file);
			if (cur_redir->fd != -1)
				close(cur_redir->fd);
			free(cur_redir);
		}
		tmp = cmd;
		cmd = cmd->next;
		free(tmp);
	}
}

/*
DESCRIPTION:
	Frees the command list and their content and tokens and returns NULL.
*/

t_cmd	*error_free(t_cmd *cmds, t_token *tokens)
{
	free_cmds(cmds);
	free_tokens(tokens, true, NULL);
	return (NULL);
}

/*
DESCRIPTION:
	Frees the environment list and their content.
*/

void	free_env(t_env *env)
{
	t_env	*tmp;

	tmp = env;
	while (env)
	{
		free(env->key);
		if (env->value)
			free(env->value);
		tmp = env;
		env = env->next;
		free(tmp);
	}
}

/*
DESCRIPTION:
	Frees the environment pointer array.
*/

void	free_envp(char **envp)
{
	size_t	i;

	if (!envp)
		return ;
	i = 0;
	while (envp[i])
		free(envp[i++]);
	free(envp);
}
