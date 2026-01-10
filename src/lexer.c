/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:27:11 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 17:11:57 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_list		*get_token(char *s);
static t_token_type	get_nonword_tok_type(char *s);
static size_t		get_word_tok_len(char *s);

/*
DESCRIPTION:
	Tokenises the input string and returns a list of tokens.
	Returns the list of tokens on success, NULL on failure.
*/

t_list	*tokenise_input(char *s)
{
	t_list_info	tokens;

	tokens.head = NULL;
	tokens.last = NULL;
	while (*s)
	{
		while (*s && ft_isspace(*s))
			s++;
		if (!*s)
			break ;
		tokens.new = get_token(s);
		if (!tokens.new)
			return (ft_lstclear(&tokens.head, free_token), NULL);
		ft_lstadd_back(&tokens.head, tokens.new, &tokens.last);
		s += get_tok(tokens.new)->len;
	}
	return (tokens.head);
}

static t_list	*get_token(char *s)
{
	t_token_type	type;
	size_t			len;

	type = WORD;
	if (is_redir(*s) || is_connector(s) || is_parenthesis(*s))
	{
		type = get_nonword_tok_type(s);
		if (type == NONE)
			return (NULL);
		len = 1;
		if (type == APPEND || type == HEREDOC || type == OR || type == AND)
			len = 2;
	}
	else
		len = get_word_tok_len(s);
	return (create_token(s, type, len));
}

/*
DESCRIPTION:
	Creates a token from the input string and returns it.
	Returns the token on success, NULL on failure.
*/

static t_token_type	get_nonword_tok_type(char *s)
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
		return (perr_token(s, 1), NONE);
	return (type);
}

/*
DESCRIPTION:
	Parses the word token and returns the length of the word.
*/

static size_t	get_word_tok_len(char *s)
{
	t_token_type	flag;
	char			*p;
	char			*tmp;

	flag = WORD;
	p = s;
	while (*p && !(flag == WORD && is_word_delimiter(p)))
	{
		flag = update_quote_flag(*p, flag);
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
