/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 13:47:52 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 21:14:20 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_token(t_token **head, t_token **cur, t_token *prev,
				t_shell *shell);
static int	update_paren_depth(t_token *cur, int *depth);
static int	is_valid_token(t_token_type t1, t_token_type t2);

/*
DESCRIPTION:
	Parses the tokens and returns the parsed tokens.
	Allocates a new string for only word tokens, others are pointers to the
	input strings with just the token->type set.
	Returns the parsed tokens on success, NULL on error.
*/

t_token	*parse_tokens(t_token *head, t_shell *shell)
{
	t_token	*cur;
	t_token	*prev;
	int		depth;
	int		ret;

	if (!head)
		return (NULL);
	cur = head;
	prev = NULL;
	depth = 0;
	while (cur)
	{
		ret = process_token(&head, &cur, prev, shell);
		if (!cur || ret == 2)
			continue ;
		if (ret == 1 || (cur && update_paren_depth(cur, &depth)))
			return (free_tokens(head), NULL);
		prev = cur;
		cur = cur->next;
	}
	if (depth || (prev && !is_valid_token(prev->type, NONE)))
		return (perr_token(NULL, 0), free_tokens(head), NULL);
	return (head);
}

int	is_target_fd(t_token *cur)
{
	char	*p;
	size_t	i;

	p = cur->raw;
	if (!cur->next || !is_type_redir(cur->next->type))
		return (0);
	i = 0;
	while (i < cur->len && isdigit(p[i]))
		i++;
	if (i != cur->len || !is_redir(p[i]))
		return (0);
	cur->type = TARGET_FD;
	return (1);
}

/*
DESCRIPTION:
	Processes a single token: validates it, expands word tokens, and splits
	unquoted word tokens containing spaces. Returns 0 on success, 1 on error.
*/

static int	process_token(t_token **head, t_token **cur, t_token *prev,
		t_shell *shell)
{
	if ((!prev && !is_valid_token(NONE, (*cur)->type)) || (prev
			&& !is_valid_token(prev->type, (*cur)->type)))
		return (perr_token((*cur)->raw, (*cur)->len), 1);
	if ((*cur)->type != WORD)
		return (0);
	if (expand_word_token(*cur, prev, shell))
		return (1);
	if (!(*cur)->quoted && is_target_fd(*cur))
		return (0);
	if ((*cur)->quoted || !(*cur)->word)
		return (0);
	if (!*(*cur)->word)
		return (del_one_token(head, prev, cur), 2);
	if (ft_strlen((*cur)->word) == ft_strcspn((*cur)->word, IS_SPACE))
		return (0);
	if (prev && is_type_redir(prev->type))
		return (perr_tok_msg(NULL, (*cur)->raw, (*cur)->len,
				E_REDIR), 1);
	if (split_word_token(head, cur, prev))
		return (1);
	return (0);
}

/*
DESCRIPTION:
	Updates the parenthesis depth counter based on the current token type.
	Returns 0 on success, 1 if an unmatched closing parenthesis is found.
*/

static int	update_paren_depth(t_token *cur, int *depth)
{
	if (cur->type == L_PAREN)
		(*depth)++;
	else if (cur->type == R_PAREN && *depth)
		(*depth)--;
	else if (cur->type == R_PAREN && !*depth)
		return (perr_token(cur->raw, cur->len), 1);
	return (0);
}

/*
DESCRIPTION:
	Returns 1 if t2 is a valid token after t1, 0 otherwise.
*/

static int	is_valid_token(t_token_type t1, t_token_type t2)
{
	if (t1 == NONE)
		return (t2 == WORD || t2 == L_PAREN || is_type_redir(t2));
	if (t2 == NONE)
		return (t1 == WORD || t1 == R_PAREN);
	if (t1 == TARGET_FD)
		return (is_type_redir(t2));
	if (is_type_redir(t1))
		return (t2 == WORD);
	if (t1 == PIPE_CHAR || t1 == AND || t1 == OR)
		return (t2 == WORD || is_type_redir(t2) || t2 == L_PAREN);
	if (t1 == L_PAREN)
		return (t2 == L_PAREN || t2 == WORD || is_type_redir(t2));
	if (t1 == R_PAREN)
		return (t2 == R_PAREN || is_type_con(t2) || is_type_redir(t2));
	if (t1 == WORD)
		return (t2 == WORD || is_type_redir(t2) || is_type_con(t2)
			|| t2 == R_PAREN);
	return (0);
}
