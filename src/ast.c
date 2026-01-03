/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:27 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 19:44:48 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int  	parse_cmd(t_token **tok, t_cmd *cmd, void **last_redir);
static t_cmd	*create_cmd(void);

t_cmd   *parse_cmd_list(t_token **tok)
{
    t_list  cmd;
    void    *last_redir;

    cmd.head = NULL;
    while(*tok && (*tok)->type != R_PAREN)
    {
        cmd.new = create_cmd();
        if (!cmd.new)
            return (free_cmds(cmd.head), NULL);
        lstadd_back((void **)&cmd.head, cmd.new, cmd.tail, TYPE_CMD);
		cmd.tail = cmd.new;
		if (*tok && is_type_con((*tok)->type))
        {
            ((t_cmd *)cmd.new)->con = (*tok)->type;
            *tok = (*tok)->next;
        }
        if (!parse_cmd(tok, cmd.new, &last_redir))
            return (free_cmds(cmd.head), NULL);
    }
    return (cmd.head);
}

static int  parse_cmd(t_token **tok, t_cmd *cmd, void **last_redir)
{
	if (!*tok)
		return (0);
    if ((*tok)->type != L_PAREN)
		return (build_cmd(tok, cmd, last_redir));
	*tok = (*tok)->next;
	cmd->sub = parse_cmd_list(tok);
	if (!cmd->sub || !*tok || (*tok)->type != R_PAREN)
		return (0);
	*tok = (*tok)->next;
    return (1);
}

/*
DESCRIPTION:
	Mallocates the cmd struct and char **args to fit given word_count words.
	Returns the newly malloced cmd struct.
*/

static t_cmd	*create_cmd(void)
{
	t_cmd	*new;

	new = malloc(sizeof(t_cmd));
	if (!new)
		return (perror(MINI), NULL);
	new->args = NULL;
	new->redirs = NULL;
	new->next = NULL;
	new->exec.builtin = -1;
	new->exec.pid = -1;
	new->exec.p_fd = -1;
	new->con = NONE;
	new->sub = NULL;
	return (new);
}
