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
		if (!ft_strchr(OPERATORS, *s))
			tokens.new = create_token(s, WORD, parse_word_token(s));
		else
			tokens.new = get_token(s);
		if (!tokens.new)
			return (free_tokens(tokens.head, false, NULL), NULL);
		lstadd_back((void **)&tokens, tokens.new, tokens.tail, TYPE_TOKEN);
		tokens.tail = tokens.new;
		tmp = (t_token *)tokens.new;
		if (tmp->type == WORD && (ft_memchr(tmp->token, '\'', tmp->len)
				|| ft_memchr(tmp->token, '\"', tmp->len)))
			tmp->quoted = true;
		s += ((t_token *)tokens.tail)->len;
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
	size_t			len;

	len = 1;
	if (*s == '>' && s[1] == '>')
		type = APPEND;
	else if (*s == '<' && s[1] == '<')
		type = HEREDOC;
	else if (*s == '|' && s[1] == '|')
		type = OR_OP;
	else if (*s == '&' && s[1] == '&')
		type = AND_OP;
	else if (*s == '<')
		type = REDIR_IN;
	else if (*s == '>')
		type = REDIR_OUT;
	else if (*s == '|')
		type = PIPE_CHAR;
	else
		return (print_type_error(s[1]), NULL);
	if (type == APPEND || type == HEREDOC || type == OR_OP || type == AND_OP)
		len = 2;
	return (create_token(s, type, len));
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
	while (*p && !(flag == WORD && (ft_strchr(WORD_DELIMITERS, *p))))
	{
		if (*p == '\'' && flag == WORD && ft_strchr(p + 1, *p))
			flag = SQUOTE;
		else if (*p == '\"' && flag == WORD && ft_strchr(p + 1, *p))
			flag = DQUOTE;
		else if ((*p == '\'' && flag == SQUOTE) || (*p == '\"'
				&& flag == DQUOTE))
			flag = WORD;
		else if (*p == '$' && p[1] != '\0' && p[1] == '{')
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

void	print_type_error(char c)
{
	if (!c)
		ft_dprintf(STDERR_FILENO, "%s: %s `newline'\n", MINI, E_PARSE);
	else
		ft_dprintf(STDERR_FILENO, "%s: %s `%c'\n", MINI, E_PARSE, c);
}
