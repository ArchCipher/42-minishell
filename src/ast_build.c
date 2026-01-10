/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_build.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 14:35:12 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/07 16:58:20 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int		alloc_cmd_args(t_list *tokens, t_cmd *cmd);
static int		build_redir(t_list **tokens, t_list **redirs,
					t_list **last_redir);
static t_list	*create_redir(t_list *tokens, int target_fd);
static int		validate_redir(t_list *tokens);

/*
DESCRIPTION:
	Builds an entire command struct with arguments and redirections.
	Word tokens following redir are added as redir file or delimeter(heredoc).
	Other word tokens are added to args.
	Old word tokens are made null, to avoid double free on error.
	Returns the newly built command struct on success, NULL on error.
*/

int	build_cmd(t_list **tokens, t_cmd *cmd, t_list **last_redir)
{
	t_token	*token;
	ssize_t	i;

	i = 0;
	if (!alloc_cmd_args(*tokens, cmd))
		return (0);
	while (*tokens && (*tokens)->content)
	{
		token = get_tok(*tokens);
		if (!token || is_type_con(token->type) || token->type == R_PAREN)
			break ;
		if (token->type == WORD)
		{
			cmd->args[i++] = token->word;
			token->word = NULL;
			*tokens = (*tokens)->next;
		}
		else if (!build_redir(tokens, &cmd->redirs, last_redir))
		{
			if (cmd->args)
				cmd->args[i] = NULL;
			return (0);
		}
	}
	return (1);
}

/*
DESCRIPTION:
	Counts words until it reaches end of cmd / pipe character.
	Returns the number of words found.
	Although only defensive, returns -1 on leaks in the parser.
*/

static int	alloc_cmd_args(t_list *tokens, t_cmd *cmd)
{
	t_token	*token;
	ssize_t	word_count;

	word_count = 0;
	while (tokens && tokens->content)
	{
		token = get_tok(tokens);
		if (!token || is_type_con(token->type))
			break ;
		if (token->type == WORD)
			word_count++;
		else if (is_type_redir(token->type))
			tokens = tokens->next;
		else
			break ;
		tokens = tokens->next;
	}
	if (!word_count)
		return (1);
	cmd->args = malloc(sizeof(char *) * (word_count + 1));
	if (!cmd->args)
		return (perror(MINI), 0);
	cmd->args[word_count] = NULL;
	return (1);
}

/*
DESCRIPTION:
	Returns 1 if valid redir tokens, 0 otherwise.
*/

/*
DESCRIPTION:
	Builds redirect struct redirection token and following word token.
	Word token following redirection token is treated as delimeter for heredoc,
	and file for others.
	Old word tokens are made null, to avoid double free on error.
*/

static int	build_redir(t_list **tokens, t_list **redirs, t_list **last_redir)
{
	t_token	*token;
	t_list	*new;
	int		target_fd;

	while (*tokens && (*tokens)->content)
	{
		token = get_tok(*tokens);
		target_fd = -1;
		if (token->type == TARGET_FD)
		{
			target_fd = ft_atoi(token->word);
			*tokens = (*tokens)->next;
		}
		if (!is_type_redir(token->type)
			|| get_tok_type((*tokens)->next) != WORD)
			break ;
		new = create_redir(*tokens, target_fd);
		if (!new)
			return (perror(MINI), 0);
		ft_lstadd_back(redirs, new, last_redir);
		*tokens = (*tokens)->next->next;
	}
	if (*tokens && !validate_redir(*tokens))
		return (0);
	return (1);
}

static t_list	*create_redir(t_list *tokens, int target_fd)
{
	t_list	*new;
	t_redir	*redir;
	t_token	*next_token;

	if (!tokens || !tokens->next)
		return (NULL);
	new = ft_lstnew(sizeof(t_redir));
	if (!new)
		return (perror(MINI), NULL);
	redir = get_redir(new);
	next_token = get_tok(tokens->next);
	redir->file = next_token->word;
	next_token->word = NULL;
	redir->flag = get_tok_type(tokens);
	redir->fd = -1;
	redir->target_fd = target_fd;
	redir->quoted = next_token->quoted;
	return (new);
}

static int	validate_redir(t_list *tokens)
{
	t_token	*token;
	t_token	*next;

	if (!tokens || !tokens->content)
		return (0);
	token = get_tok(tokens);
	if (!token || (token->type != TARGET_FD && !is_type_redir(token->type)))
		return (1);
	if (token->type == TARGET_FD)
	{
		if (!tokens->next)
			return (perr_token(token->raw, token->len), 0);
		tokens = tokens->next;
		token = get_tok(tokens);
		if (!is_type_redir(token->type))
			return (perr_token(token->raw, token->len), 0);
	}
	next = get_tok(tokens->next);
	if (is_valid_token(token, next))
		return (1);
	if (!next)
		return (perr_token(token->raw, token->len), 0);
	perr_token(next->raw, next->len);
	return (0);
}
