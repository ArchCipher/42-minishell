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

static t_cmd	*build_cmd(t_token **current);
static size_t	count_args(t_token *token);
static t_cmd	*create_cmd(size_t word_count);
static int		build_redir(t_token **current, void **head, void **last);

/*
DESCRIPTION:
	Builds the abstract syntax tree (AST) from a list of tokens and returns the
	head of the AST/ command list.
*/

t_cmd	*build_ast(t_token *tokens)
{
	t_list	cmd;
	t_token	*current;

	if (!tokens)
		return (NULL);
	current = tokens;
	cmd.head = NULL;
	while (current)
	{
		if (current->type == WORD || type_redir(current->type))
		{
			cmd.new = build_cmd(&current);
			if (!cmd.new)
				return (free_cmds_err(cmd.head, tokens));
			lstadd_back((void **)&cmd.head, cmd.new, cmd.tail, TYPE_CMD);
			cmd.tail = cmd.new;
		}
		if (current && type_con(current->type))
			current = current->next;
	}
	return (cmd.head);
}

/*
DESCRIPTION:
	Builds an entire command struct with arguments and redirections.
	Word tokens following redir are added as redir file or delimeter(heredoc).
	Other word tokens are added to args.
	Old word tokens are made null, to avoid double free on error.
	Returns the newly built command struct on success, NULL on error.
*/

static t_cmd	*build_cmd(t_token **cur)
{
	t_cmd	*new;
	void	*last;
	ssize_t	i;

	new = create_cmd(count_args(*cur));
	if (!new)
		return (NULL);
	i = 0;
	while (*cur && !type_con((*cur)->type))
	{
		if ((*cur)->type == WORD)
		{
			new->args[i++] = (*cur)->token;
			(*cur)->token = NULL;
			*cur = (*cur)->next;
		}
		else if (type_redir((*cur)->type) && !build_redir(cur, (void **)&new->redirs, &last))
			return (new->args[i] = NULL, free_cmds(new), NULL);
	}
	if (*cur && type_con((*cur)->type))
		new->con = (*cur)->type;
	new->args[i] = NULL;
	return (new);
}

/*
DESCRIPTION:
	Counts words until it reaches end of cmd / pipe character.
	Returns the number of words found.
	Although only defensive, returns -1 on leaks in the parser.
*/

static size_t	count_args(t_token *token)
{
	ssize_t	word_count;

	word_count = 0;
	while (token && !type_con(token->type))
	{
		if (token->type == WORD)
			word_count++;
		else if (type_redir(token->type))
			token = token->next;
		token = token->next;
	}
	return (word_count);
}

/*
DESCRIPTION:
	Mallocates the cmd struct and char **args to fit given word_count words.
	Returns the newly malloced cmd struct.
*/

static t_cmd	*create_cmd(size_t word_count)
{
	t_cmd	*new;

	new = malloc(sizeof(t_cmd));
	if (!new)
		return (perror(MINI), NULL);
		if (word_count)
	{
		new->args = (char **)malloc((word_count + 1) * sizeof(char *));
		if (!new->args)
			return (perror(MINI), free(new), NULL);
	}
	else
		new->args = NULL;
	new->redirs = NULL;
	new->next = NULL;
	new->exec.builtin = -1;
	new->exec.pid = -1;
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

static int	build_redir(t_token **current, void **head, void **last)
{
	t_redir	*new;

	if (*current && !(*current)->next)
		return (perr_msg("build_redir", "parsing error", NULL, false), 0);
	while (*current && (*current)->next && (*current)->type != PIPE_CHAR
		&& (*current)->type != WORD)
	{
		new = malloc(sizeof(t_redir));
		if (!new)
			return (perror(MINI), 0);
		new->file = (*current)->next->token;
		(*current)->next->token = NULL;
		new->flag = (*current)->type;
		new->fd = -1;
		new->quoted = (*current)->next->quoted;
		new->next = NULL;
		lstadd_back(head, (void *)new, *last, TYPE_REDIR);
		*last = new;
		*current = (*current)->next->next;
	}
	return (1);
}
