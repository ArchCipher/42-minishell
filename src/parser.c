/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 13:47:52 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/07 16:47:01 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_token(t_list **tokens, t_list **cur, t_list *prev,
				t_shell *shell);
static int	is_target_fd(t_token *cur, t_token *next);
static int	update_paren_depth(t_token *cur, int *depth);

/*
DESCRIPTION:
	Parses the tokens and returns the parsed tokens.
	Allocates a new string for only word tokens, others are pointers to the
	input strings with just the token->type set.
	Returns the parsed tokens on success, NULL on error.
*/

t_list	*parse_tokens(t_list *tokens, t_shell *shell)
{
	t_list	*cur;
	t_list	*prev;
	int		depth;
	int		ret;

	if (!tokens)
		return (NULL);
	cur = tokens;
	prev = NULL;
	depth = 0;
	while (cur && cur->content)
	{
		ret = process_token(&tokens, &cur, prev, shell);
		if (!cur || ret == 2)
			continue ;
		if (ret == 1 || (cur && update_paren_depth(get_tok(cur), &depth)))
			return (ft_lstclear(&tokens, free_token), NULL);
		prev = cur;
		cur = cur->next;
	}
	if (depth || (prev && !is_valid_token(get_tok(prev), NULL)))
		return (perr_token(NULL, 0), ft_lstclear(&tokens, free_token), NULL);
	return (tokens);
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
	Processes a single token: validates it, expands word tokens, and splits
	unquoted word tokens containing spaces. Returns 0 on success, 1 on error.
*/

static int	process_token(t_list **tokens, t_list **cur, t_list *prev,
		t_shell *shell)
{
	t_token	*cur_tok;
	t_token	*prev_tok;

	cur_tok = get_tok(*cur);
	if (!cur_tok)
		return (1);
	prev_tok = get_tok(prev);
	if (!is_valid_token(prev_tok, cur_tok))
		return (perr_token(cur_tok->raw, cur_tok->len), 1);
	if (cur_tok->type != WORD)
		return (0);
	if (expand_word_token(cur_tok, prev_tok, shell))
		return (1);
	if (cur_tok->quoted || !cur_tok->word)
		return (0);
	if (!cur_tok->quoted && is_target_fd(cur_tok, get_tok((*cur)->next)))
		return (0);
	if (!*cur_tok->word)
		return (del_one_token(tokens, prev, cur), 2);
	if (ft_strlen(cur_tok->word) == ft_strcspn(cur_tok->word, IS_SPACE))
		return (0);
	if (prev_tok && is_type_redir(prev_tok->type))
		return (perr_tok_msg(NULL, cur_tok->raw, cur_tok->len, E_REDIR), 1);
	return (split_word_token(tokens, cur, prev));
}

static int	is_target_fd(t_token *cur, t_token *next)
{
	char	*p;
	size_t	i;

	p = cur->raw;
	if (!next || !is_type_redir(next->type))
		return (0);
	i = 0;
	while (i < cur->len && ft_isdigit(p[i]))
		i++;
	if (i != cur->len || !is_redir(p[i]))
		return (0);
	cur->type = TARGET_FD;
	return (1);
}

/*
DESCRIPTION:
	Returns 1 if t2 is a valid token after t1, 0 otherwise.
*/

int	is_valid_token(t_token *prev, t_token *cur)
{
	if (!prev && !cur)
		return (0);
	if (!prev)
		return (cur->type == WORD || cur->type == L_PAREN
			|| is_type_redir(cur->type));
	if (!cur)
		return (prev->type == WORD || prev->type == R_PAREN);
	if (prev->type == TARGET_FD)
		return (is_type_redir(cur->type));
	if (is_type_redir(prev->type))
		return (cur->type == WORD);
	if (prev->type == PIPE_CHAR || prev->type == AND || prev->type == OR)
		return (cur->type == WORD || is_type_redir(cur->type)
			|| cur->type == L_PAREN);
	if (prev->type == L_PAREN)
		return (cur->type == L_PAREN || cur->type == WORD
			|| is_type_redir(cur->type));
	if (prev->type == R_PAREN)
		return (cur->type == R_PAREN || is_type_con(cur->type)
			|| is_type_redir(cur->type));
	if (prev->type == WORD)
		return (cur->type == WORD || is_type_redir(cur->type)
			|| is_type_con(cur->type) || cur->type == R_PAREN);
	return (0);
}
