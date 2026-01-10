/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_expand.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 14:41:34 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 17:36:27 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// static int	should_expand(char *s, char *end, t_token *prev, t_token_type flag);
// static void	append_char(char *c, char *end, t_token_type *flag, t_string *str);
// static int	init_expand_vars(t_token *token, t_string *str, t_expand *exp);
// static int	is_fully_quoted(const char *raw, size_t len);

char	*expand_remove_quote(char *src, t_shell *shell)
{
	t_expand		s;
	t_token_type	flag;

	s = init_expand(src, ft_strlen(src));
	if (!s.dst)
		return (perror(MINI), NULL);
	flag = WORD;
	while (s.src < s.src_end)
	{
		if ((*s.src == '\'' && (flag == WORD || flag == SQUOTE))
			|| (*s.src == '\"' && (flag == WORD || flag == DQUOTE)))
			flag = update_quote_flag(*s.src, flag);
		else if (dollar_expandable(s.src, s.src_end) && flag != SQUOTE)
		{
			if (append_var(&s, shell))
				return (free(s.dst), free(src), NULL);
			continue ;
		}
		else
			s.dst[s.len++] = *s.src;
		s.src++;
	}
	s.dst[s.len] = 0;
	free(src);
	return (s.dst);
}

/*
DESCRIPTION:
	Expands variables and tilde in a word token and allocates a new string
	for the expanded result. Returns 0 on success, 1 on error.
*/

// int	expand_word_token(t_token *token, t_token *prev, t_shell *shell)
// {
// 	t_string		str;
// 	t_expand		exp;

// 	if (init_expand_vars(token, &str, &exp))
// 		return (1);
// 	while (exp.p < exp.end)
// 	{
// 		if (!should_expand(exp.p, exp.end, prev, exp.quote_state))
// 			append_char(exp.p++, exp.end, &exp.quote_state, &str);
// 		else
// 		{
// 			if (exp.quote_state == WORD)
// 				exp.unquoted_var = true;
// 			if (append_var(&str, &exp.p, exp.end, shell))
// 				return (free(str.dst), 1);
// 		}
// 	}
// 	str.dst[str.len] = 0;
// 	token->word = str.dst;
// 	return (0);
// }

// static int	init_expand_vars(t_token *token, t_string *str, t_expand *exp)
// {
// 	*str = alloc_tstring(token->len);
// 	if (!str->dst)
// 		return (perror(MINI), 1);
// 	exp->p = token->raw;
// 	exp->end = exp->p + token->len;
// 	exp->quote_state = WORD;
// 	exp->unquoted_var = false;
// 	return (0);
// }

/*
DESCRIPTION:
	Determines if the character at position s should be expanded.
	Returns 1 if expansion should occur, 0 otherwise.
*/

// static int	should_expand(char *s, char *end, t_token *prev, t_token_type flag)
// {
// 	if (prev && prev->type == HEREDOC)
// 		return (0);
// 	if (*s == '~' && (s + 1 == end || s[1] == '/'))
// 		return (1);
// 	if (dollar_expandable(s, end) && flag != SQUOTE)
// 		return (1);
// 	return (0);
// }

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

// static void	append_char(char *s, char *end, t_token_type *quote_state,
// 		t_string *str)
// {
// 	t_token_type	old_flag;

// 	if (*s == '$' && *quote_state == WORD && s + 1 < end && (s[1] == '\''
// 			|| s[1] == '\"'))
// 		return ;
// 	old_flag = *quote_state;
// 	*quote_state = update_quote_flag(*s, *quote_state);
// 	if (*quote_state == old_flag)
// 		str->dst[(str->len)++] = *s;
// }
