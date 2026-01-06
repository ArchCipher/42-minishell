/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 14:41:34 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 17:36:27 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	should_expand(char *s, char *end, t_token *prev, t_token_type flag);
static void	append_char(char *c, char *end, t_token_type *flag, t_string *str);
static int	insert_token(t_token **head, t_token **prev, t_token *next,
				const char *tok);

/*
DESCRIPTION:
	Expands variables and tilde in a word token and allocates a new string
	for the expanded result. Returns 0 on success, 1 on error.
*/

int	expand_word_token(t_token *tok, t_token *prev, t_shell *shell)
{
	t_token_type	quote_state;
	t_string		str;
	char			*end;
	char			*p;

	str = alloc_tstring(tok->len);
	if (!str.s)
		return (perror(MINI), 1);
	p = tok->raw;
	end = p + tok->len;
	quote_state = WORD;
	while (p < end)
	{
		if (!should_expand(p, end, prev, quote_state))
			append_char(p++, end, &quote_state, &str);
		else if (append_var(&str, &p, end, shell))
			return (free(str.s), 1);
	}
	str.s[str.len] = 0;
	tok->word = str.s;
	return (0);
}

/*
DESCRIPTION:
	Determines if the character at position s should be expanded.
	Returns 1 if expansion should occur, 0 otherwise.
*/

static int	should_expand(char *s, char *end, t_token *prev, t_token_type flag)
{
	if (prev && prev->type == HEREDOC)
		return (0);
	if (*s == '~' && (s + 1 == end || s[1] == '/'))
		return (1);
	if (dollar_expandable(s, end) && flag != SQUOTE)
		return (1);
	return (0);
}

/*
DESCRIPTION:
	Appends a character to the string unless it is a quote or an expandable
	dollar sign. In case of quotes, it updates the flag to the appropriate type.

	Updates quote state. Does not append the quote character itself

	skips '$' for following cases,but does not support ANSCI-C quoting or locale
	translation
	$'string'	ANSI-C quoting
	$"string"	locale translation
*/

static void	append_char(char *s, char *end, t_token_type *quote_state,
		t_string *str)
{
	t_token_type	old_flag;

	if (*s == '$' && *quote_state == WORD && s + 1 < end && (s[1] == '\''
			|| s[1] == '\"'))
		return ;
	old_flag = *quote_state;
	*quote_state = update_quote_flag(*s, *quote_state);
	if (*quote_state == old_flag)
		str->s[(str->len)++] = *s;
}

/*
DESCRIPTION:
	Splits a word token containing spaces into multiple word tokens.
	The original token is deleted and replaced with new tokens created from
	the space-separated words. Returns 0 on success, 1 on failure.
*/

int	split_word_token(t_token **head, t_token **cur, t_token *prev)
{
	t_strtok	str;

	str.str = strdup((*cur)->word);
	if (!str.str)
		return (1);
	del_one_token(head, prev, cur);
	str.token = ft_strtok_r(str.str, IS_SPACE, &str.p);
	while (str.token)
	{
		if (insert_token(head, &prev, *cur, str.token))
			return (free(str.str), 1);
		str.token = ft_strtok_r(NULL, IS_SPACE, &str.p);
	}
	free(str.str);
	return (0);
}

/*
DESCRIPTION:
	Inserts a new word token into the token list at the specified position.
	Returns 0 on success, 1 on failure.
*/

static int	insert_token(t_token **head, t_token **prev, t_token *next,
		const char *tok)
{
	t_token	*new;
	char	*word;

	word = strdup(tok);
	if (!word)
		return (1);
	new = create_token(word, WORD, ft_strlen(word));
	if (!new)
		return (free(word), 1);
	new->word = word;
	new->next = next;
	if (!*prev)
		*head = new;
	else
		(*prev)->next = new;
	*prev = new;
	return (0);
}
