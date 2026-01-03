/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:27:11 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 21:04:56 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_token	*get_token(char *s);
static size_t	get_tok_len(t_token_type type);
static size_t	parse_word_token(char *s);
static t_token	*create_token(void *token, t_token_type type, size_t len);

/*
DESCRIPTION:
	Tokenises the input string and returns a list of tokens.
	Returns the list of tokens on success, NULL on failure.
*/

t_token	*tokenise_input(char *s)
{
	t_list	tokens;
	t_token	*tmp;

	tokens.head = NULL;
	while (*s)
	{
		while (*s && ft_isspace(*s))
			s++;
		if (!*s)
			break ;
		if (is_redir(*s) || is_connector(s) || is_parenthesis(*s))
			tokens.new = get_token(s);
		else
			tokens.new = create_token(s, WORD, parse_word_token(s));
		if (!tokens.new)
			return (free_tokens(tokens.head, false, NULL), NULL);
		lstadd_back((void **)&tokens, tokens.new, tokens.tail, TYPE_TOKEN);
		tokens.tail = tokens.new;
		tmp = (t_token *)tokens.new;
		if (tmp->type == WORD && (ft_memchr(tmp->token, '\'', tmp->len)
				|| ft_memchr(tmp->token, '\"', tmp->len)))
			tmp->quoted = true;
		s += tmp->len;
	}
	return (tokens.head);
}

/*
DESCRIPTION:
	Creates a token from the input string and returns it.
	Returns the token on success, NULL on failure.
*/

static t_token	*get_token(char *s)
{
	t_token_type	type;

	if (*s == '>' && s[1] == '>')
		type = APPEND;
	else if (*s == '<' && s[1] == '<')
		type = HEREDOC;
	else if (*s == '|' && s[1] == '|')
		type = OR;
	else if (*s == '&' && s[1] == '&')
		type = AND;
	else if (*s == '(')
		type = L_PAREN;
	else if (*s == ')')
		type = R_PAREN;
	else if (*s == '<')
		type = REDIR_IN;
	else if (*s == '>')
		type = REDIR_OUT;
	else if (*s == '|')
		type = PIPE_CHAR;
	else
		return (perr_token(s, 1), NULL);
	return (create_token(s, type, get_tok_len(type)));
}

static size_t	get_tok_len(t_token_type type)
{
	if (type == APPEND || type == HEREDOC || type == OR || type == AND)
		return (2);
	if (type == REDIR_IN || type == REDIR_OUT || type == PIPE_CHAR || type == L_PAREN || type == R_PAREN)
		return (1);
	return (0);
}

/*
DESCRIPTION:
	Parses the word token and returns the length of the word.
*/

static size_t	parse_word_token(char *s)
{
	t_token_type	flag;
	char			*p;
	char			*tmp;

	flag = WORD;
	p = s;
	while (*p && !(flag == WORD && is_word_delimiter(p)))
	{
		flag = update_quote_flag(p, flag);
		if (*p == '$' && p[1] == '{')
		{
			tmp = ft_strchr(p + 1, '}');
			if (tmp)
				p = tmp;
		}
		p++;
	}
	return (p - s);
}

/*
DESCRIPTION:
	Mallocates a token and returns it.
	Returns the token on success, NULL on failure.
*/

static t_token	*create_token(void *token, t_token_type type, size_t len)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (perror(MINI), NULL);
	new->token = token;
	new->type = type;
	new->len = len;
	new->quoted = false;
	new->next = NULL;
	return (new);
}
