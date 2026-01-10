/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:27 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 14:35:24 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int		parse_cmd(t_list **tokens, t_cmd *cmd, t_list **last_redir);
static t_list	*create_cmd(void);

/*
DESCRIPTION:
	Parses tokens into a command list, handling connectors and parentheses.
	Returns the command list on success, NULL on error.
*/

t_list	*parse_cmd_list(t_list **tokens)
{
	t_list_info	cmds;
	t_token		*token;
	t_list		*last_redir;

	cmds.head = NULL;
	cmds.last = NULL;
	last_redir = NULL;
	token = get_tok(*tokens);
	while (*tokens && token && token->type != R_PAREN)
	{
		cmds.new = create_cmd();
		if (!cmds.new)
			return (ft_lstclear(&cmds.head, free_cmd), NULL);
		ft_lstadd_back(&cmds.head, cmds.new, &cmds.last);
		if (token && is_type_con(token->type))
		{
			get_cmd(cmds.new)->con = token->type;
			*tokens = (*tokens)->next;
		}
		if (!parse_cmd(tokens, get_cmd(cmds.new), &last_redir))
			return (ft_lstclear(&cmds.head, free_cmd), NULL);
		token = get_tok(*tokens);
	}
	return (cmds.head);
}

/*
DESCRIPTION:
	Parses a single command from tokens, handling regular commands and
	subshells (parentheses). Returns 1 on success, 0 on error.
*/

static int	parse_cmd(t_list **tokens, t_cmd *cmd, t_list **last_redir)
{
	if (!*tokens || !(*tokens)->content)
		return (0);
	if (get_tok(*tokens)->type != L_PAREN)
		return (build_cmd(tokens, cmd, last_redir));
	*tokens = (*tokens)->next;
	cmd->subshell = parse_cmd_list(tokens);
	if (!cmd->subshell || !*tokens || !(*tokens)->content
		|| get_tok(*tokens)->type != R_PAREN)
		return (0);
	*tokens = (*tokens)->next;
	return (1);
}

/*
DESCRIPTION:
	Allocates the cmd struct and returns the newly allocated cmd struct.
*/

static t_list	*create_cmd(void)
{
	t_list	*new;
	t_cmd	*cmd;

	new = ft_lstnew(sizeof(t_cmd));
	if (!new)
		return (perror(MINI), NULL);
	cmd = get_cmd(new);
	cmd->args = NULL;
	cmd->redirs = NULL;
	cmd->exec.builtin = -1;
	cmd->exec.pid = -1;
	cmd->exec.prev_fd = -1;
	cmd->con = NONE;
	cmd->subshell = NULL;
	return (new);
}
