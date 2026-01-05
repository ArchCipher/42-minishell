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
	Adds a new node to the end of the list.

NOTE:
	Must update tail in the caller function to maintain the list.
*/

void	lstadd_back(void **head, void *new, void *last, t_node_type type)
{
	if (!head || !new || (*head && !last))
	{
		perr_msg("lstadd_back", "internal invariant violated", NULL, false);
		return ;
	}
	if (!*head)
		*head = new;
	else
	{
		if (type == TYPE_TOKEN)
			((t_token *)last)->next = new;
		else if (type == TYPE_CMD)
			((t_cmd *)last)->next = new;
		else if (type == TYPE_REDIR)
			((t_redir *)last)->next = new;
		else if (type == TYPE_ENV)
			((t_env *)last)->next = new;
	}
}

/*
DESCRIPTION:
	Frees the tokens and their content.
	 && free_content && current->type == WORD
*/

void	free_tokens(t_token *tok)
{
	t_token	*current;

	while (tok)
	{
		current = tok;
		tok = tok->next;
		if (current->word)
			free(current->word);
		free(current);
	}
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
	Frees the command list and their content. Closes any open fds.
	
	// if subshell is added add check for cmd->args existence
	// or move to separate fucntion
*/

void	free_cmds(t_cmd *cmds)
{
	t_cmd	*tmp;
	t_redir	*cur_redir;
	t_cmd	*cmd;

	cmd = cmds;
	while (cmd)
	{
		if (cmd->args)
			free_arr(cmd->args);
		while (cmd->redirs)
		{
			cur_redir = cmd->redirs;
			cmd->redirs = cmd->redirs->next;
			free(cur_redir->file);
			if (cur_redir->fd != -1)
				close(cur_redir->fd);
			free(cur_redir);
		}
		if (cmd->sub)
			free_cmds(cmd->sub);
		tmp = cmd;
		cmd = cmd->next;
		free(tmp);
	}
}
