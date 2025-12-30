/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:27:11 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:34:04 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_token	*get_token(char *s);
static t_token	*create_token(void *token, e_token_type type, size_t len);
static size_t	parse_word_token(char *s);

t_token	*tokenise_input(char *s)
{
	t_list	tokens;

	if (!s)
		return (NULL);
	tokens.head = NULL;
	while (*s)
	{
		while (*s && ft_isspace(*s))
			s++;
		if (!*s)
			break ;
		tokens.new = get_token(s);
		if (!tokens.new)
			return (free_tokens(tokens.head, false, NULL), NULL); 
		lstadd_back((void **)&tokens, (void *)tokens.new, (void *)tokens.tail,
			TYPE_TOKEN);
		tokens.tail = tokens.new;
		s += ((t_token *)tokens.tail)->len;
	}
	return (tokens.head);
}

static t_token	*get_token(char *s)
{
	e_token_type	type;
	size_t			len;

	if (!ft_strchr(OPERATORS, *s))
		return (create_token(s, word, parse_word_token(s)));
	len = 1;
	if (*s == '>' && s[1] == '>')
		type = append;
	else if (*s == '<' && s[1] == '<')
		type = heredoc;
	else if (*s == '<')
		type = redir_in;
	else if (*s == '>')
		type = redir_out;
	else if (*s == '|')
		type = pipe_char;
	else
		return (NULL);
	if (type == append || type == heredoc)
		len = 2;
	return (create_token(s, type, len));
}

static size_t	parse_word_token(char *s)
{
	e_token_type	flag;
	char			*p;
	char			*tmp;

	flag = word;
	p = s;
	while (*p && !(flag == word && (ft_strchr(WORD_DELIMITERS, *p))))
	{
		if (*p == '\'' && flag == word && ft_strchr(p + 1, *p))
			flag = squote;
		else if (*p == '\"' && flag == word && ft_strchr(p + 1, *p))
			flag = dquote;
		else if ((*p == '\'' && flag == squote) || (*p == '\"'
				&& flag == dquote))
			flag = word;
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

static t_token	*create_token(void *token, e_token_type type, size_t len)
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
