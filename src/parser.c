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

static int	process_token(t_token **head, t_token **cur, t_token *prev, t_shell *shell);
static int	process_depth(t_token *cur, int *depth);
static int	handle_word(t_token *token, char *end, t_shell *shell);
static void	handle_char(char *s, t_token_type *flag, char *end, t_string *str);
static int	del_one_token(t_token **tokens, t_token **cur, t_token *prev);
static int	valid_next_token(t_token_type t1, t_token_type t2);

/*
DESCRIPTION:
	Parses the tokens and returns the parsed tokens.
	Mallocates a new string for only word tokens, others are pointers to the
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
		if (process_depth(cur, &depth))
			return (free_tokens(head, true, cur), NULL);
		prev = cur;
		cur = cur->next;
	}
	if (depth || (prev && !valid_next_token(prev->type, NONE)))
		return (perr_token(NULL, 0), free_tokens(head, true, NULL), NULL);
	return (head);
}

static int	process_token(t_token **head, t_token **cur, t_token *prev, t_shell *shell)
{
	if (!prev && !valid_next_token(NONE, (*cur)->type))
		return (perr_token((*cur)->token, (*cur)->len), free_tokens(*head, false, NULL), 1);
	if (prev && !valid_next_token(prev->type, (*cur)->type))
		return (perr_token((*cur)->token, (*cur)->len), free_tokens(*head, true, *cur), 1);
	if ((*cur)->type == WORD)
	{
		if (handle_word(*cur, (*cur)->token + (*cur)->len, shell))
			return (free_tokens(*head, true, *cur), 1);
		if (!prev && !*(*cur)->token)
			return (del_one_token(head, cur, prev), 2);
	}	
	return (0);
}

static int	process_depth(t_token *cur, int *depth)
{
	if (cur->type == PAREN_O)
		(*depth)++;
	else if (cur->type == PAREN_C && *depth)
		(*depth)--;
	else if (cur->type == PAREN_C && !*depth)
		return (perr_token(cur->token, cur->len), 1);
	return (0);
}

/*
DESCRIPTION:
	Handles the word token and returns 1 if successful, 0 on error.
*/

static int	handle_word(t_token *tok, char *end, t_shell *shell)
{
	t_token_type	flag;
	t_string		str;

	str.s = malloc(tok->len + 1);
	if (!str.s)
		return (perror(MINI), 1);
	str.cap = tok->len;
	str.len = 0;
	flag = WORD;
	while (tok->token < end)
	{
		handle_char(tok->token, &flag, end, &str);
		if (dollar_expandable(tok->token, end) && flag != SQUOTE)
		{
			tok->token++;
			if (append_var(&str, get_var(&tok->token, end, shell), end
					- tok->token))
				return (free(str.s), 1);
		}
		else
			tok->token++;
	}
	str.s[str.len] = 0;
	tok->token = str.s;
	return (0);
}

/*
DESCRIPTION:
	Appends a character to the string unless it is a quote or an expandable
	dollar sign. In case of quotes, it updates the flag to the appropriate type.
*/

static void	handle_char(char *s, t_token_type *flag, char *end, t_string *str)
{
	t_token_type	old_flag;

	old_flag = *flag;
	*flag = get_flag(s, *flag);
	if (*flag == old_flag && !(*flag != SQUOTE && dollar_expandable(s, end)))
		str->s[(str->len)++] = *s;
}

/*
DESCRIPTION:
	Deletes the current token and returns 1.
*/

static int	del_one_token(t_token **tokens, t_token **cur, t_token *prev)
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

/*
DESCRIPTION:
	Returns 0 if t2 is an invalid token after t1.

NOTE:
	The caller must ensure t2 exists (current->next != NULL).
*/

static int	valid_next_token(t_token_type t1, t_token_type t2)
{
	if (t1 == NONE)
		return (t2 == WORD || t2 == PAREN_O || type_redir(t2));
	if (t2 == NONE)
		return (t1 == WORD || t1 == PAREN_C);
	if (type_redir(t1))
		return (t2 == WORD);
	if (t1 == PIPE_CHAR || t1 == AND || t1 == OR)
		return (t2 == WORD || type_redir(t2) || t2 == PAREN_O);
	if (t1 == PAREN_O)
		return (t2 == PAREN_O || t2 == WORD || type_redir(t2));
	if (t1 == PAREN_C)
		return (t2 == PAREN_C || t2 == PIPE_CHAR || t2 == AND || t2 == OR);
	if (t1 == WORD)
		return (t2 == WORD || type_redir(t2) || t2 == PIPE_CHAR || t2 == AND
			|| t2 == OR || t2 == PAREN_C);
	return (0);
}
