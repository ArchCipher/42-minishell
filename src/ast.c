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

static int  parse_cmd(t_token **tok, t_cmd *cmd, void **last);
static int  build_cmd(t_token **cur, t_cmd *cmd, void **last);
static size_t   count_args(t_token *tok);
static t_cmd	*create_cmd(void);
static int	build_redir(t_token **current, void **head, void **last);

// not needed?
size_t  count_existing_args(char **args);

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
		if (*tok && type_con((*tok)->type))
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
    if ((*tok)->type == L_PAREN)
    {
        *tok = (*tok)->next;
        cmd->sub = parse_cmd_list(tok);
        if (!cmd->sub || !*tok || (*tok)->type != R_PAREN)
            return (0);
        *tok = (*tok)->next;
    }
    else if (!build_cmd(tok, cmd, last_redir))
            return (0);
    return (1);
}

/*
DESCRIPTION:
	Builds an entire command struct with arguments and redirections.
	Word tokens following redir are added as redir file or delimeter(heredoc).
	Other word tokens are added to args.
	Old word tokens are made null, to avoid double free on error.
	Returns the newly built command struct on success, NULL on error.
*/

char    **realloc_arr(char **src, size_t old, size_t new)
{
    char    **dst;

    dst = malloc(new * sizeof(char *));
    if (!dst)
        return (free(src), NULL);
    if (src)
        ft_memcpy(dst, src, old * sizeof(char *));
    free(src);
    return (dst);
}

static int  build_cmd(t_token **tok, t_cmd *cmd, void **last_redir)
{
	ssize_t	i;

    i = 0;
    if (cmd->args)
        i = count_existing_args(cmd->args);
    cmd->args = realloc_arr(cmd->args, i, i + count_args(*tok) + 1);
    if (!cmd->args)
        return (perror(MINI), 0);
	while (*tok && !type_con((*tok)->type) && (*tok)->type != R_PAREN)
	{
		if ((*tok)->type == WORD)
		{
			cmd->args[i++] = (*tok)->token;
            (*tok)->token = NULL;
			*tok = (*tok)->next;
		}
		else if (type_redir((*tok)->type) && !build_redir(tok, (void **)&cmd->redirs, last_redir))
			return (cmd->args[i] = NULL, 0);
	}
	cmd->args[i] = NULL;
	return (1);
}

/*
DESCRIPTION:
	Counts words until it reaches end of cmd / pipe character.
	Returns the number of words found.
	Although only defensive, returns -1 on leaks in the parser.
*/

static size_t   count_args(t_token *tok)
{
	ssize_t	word_count;

	word_count = 0;
	while (tok && !type_con(tok->type) && tok->type != R_PAREN)
	{
		if (tok->type == WORD)
			word_count++;
		else if (type_redir(tok->type))
			tok = tok->next;
		tok = tok->next;
	}
	return (word_count);
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
/*
DESCRIPTION:
	Builds redirect struct redirection token and following word token.
	Word token following redirection token is treated as delimeter for heredoc,
	and file for others.
	Old word tokens are made null, to avoid double free on error.
*/

static int	build_redir(t_token **tok, void **head, void **last)
{
	t_redir	*new;

	while (*tok && type_redir((*tok)->type) && (*tok)->next && (*tok)->next->type == WORD)
	{
		new = malloc(sizeof(t_redir));
		if (!new)
			return (perror(MINI), 0);
		new->file = (*tok)->next->token;
        (*tok)->next->token = NULL; 
		new->flag = (*tok)->type;
		new->fd = -1;
		new->quoted = (*tok)->next->quoted;
		new->next = NULL;
		lstadd_back(head, (void *)new, *last, TYPE_REDIR);
		*last = new;
		*tok = (*tok)->next->next;
	}
    if (*tok && type_redir((*tok)->type) && (!(*tok)->next || (*tok)->next->type != WORD))
		return (perr_msg("build_redir", "parsing error", NULL, false), 0);
	return (1);
}
