/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 13:47:52 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 20:55:43 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_token(t_token **head, t_token **cur, t_token *prev,
				t_shell *shell);
static int	update_paren_depth(t_token *cur, int *depth);
static int	is_valid_next_token(t_token_type t1, t_token_type t2);
static int	del_empty_token(t_token **tokens, t_token **cur, t_token *prev);

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
	int		ret;
	int		depth;

	if (!head)
		return (NULL);
	cur = head;
	prev = NULL;
	depth = 0;
	while (cur)
	{
		ret = process_token(&head, &cur, prev, shell);
		if (ret == 1)
			return (NULL);
		if (ret == 2)
			continue ;
		if (cur && update_paren_depth(cur, &depth))
			return (free_tokens(head, true, cur), NULL);
		prev = cur;
		cur = cur->next;
	}
	if (depth || (prev && !is_valid_next_token(prev->type, NONE)))
		return (perr_token(NULL, 0), free_tokens(head, true, NULL), NULL);
	return (head);
}

static int	process_token(t_token **head, t_token **cur, t_token *prev,
		t_shell *shell)
{
	if (!prev && !is_valid_next_token(NONE, (*cur)->type))
		return (perr_token((*cur)->token, (*cur)->len), free_tokens(*head,
				false, NULL), 1);
	if (prev && !is_valid_next_token(prev->type, (*cur)->type))
		return (perr_token((*cur)->token, (*cur)->len), free_tokens(*head, true,
				*cur), 1);
	if ((*cur)->type == WORD)
	{
		if (expand_word_token(*cur, (*cur)->token + (*cur)->len, shell))
			return (free_tokens(*head, true, *cur), 1);
		if (!prev && !*(*cur)->token)
			return (del_empty_token(head, cur, prev), 2);
	}
	return (0);
}

static int	update_paren_depth(t_token *cur, int *depth)
{
	if (cur->type == L_PAREN)
		(*depth)++;
	else if (cur->type == R_PAREN && *depth)
		(*depth)--;
	else if (cur->type == R_PAREN && !*depth)
		return (perr_token(cur->token, cur->len), 1);
	return (0);
}

/*
DESCRIPTION:
	Returns 1 if t2 is a valid token after t1, 0 otherwise.
*/

static int	is_valid_next_token(t_token_type t1, t_token_type t2)
{
	if (t1 == NONE)
		return (t2 == WORD || t2 == L_PAREN || is_type_redir(t2));
	if (t2 == NONE)
		return (t1 == WORD || t1 == R_PAREN);
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
/*
DESCRIPTION:
	Deletes the current token and returns 1.
*/

static int	del_empty_token(t_token **tokens, t_token **cur, t_token *prev)
{
	t_token	*tmp;

	tmp = *cur;
	*cur = tmp->next;
	free(tmp->token);
	if (!prev)
		*tokens = tmp->next;
	else
		prev->next = tmp->next;
	free(tmp);
	return (1);
}
