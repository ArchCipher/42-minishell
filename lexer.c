/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:27:11 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/10 18:49:41 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_token	*create_token(void *token);
static void		set_token_type(char *s, t_token *new);
static size_t	parse_word_token(char *s);

t_token	*tokenise_input(char *s)
{
	t_list tokens;

	if (!s)
		return (NULL);
	tokens.head = NULL;
	while (*s)
	{
		while (*s && ft_isspace(*s))
			s++;	
		if (!*s)
			break ;
		tokens.new = create_token(s);
		if (!tokens.new)
			return (free_tokens(tokens.head, false, NULL), NULL);
		set_token_type(s, tokens.new);
		lstadd_back((void **)&tokens, (void *)tokens.new, (void *)tokens.tail, TYPE_TOKEN);
		tokens.tail = tokens.new;
		s += ((t_token *)tokens.tail)->len;
	}
	return (tokens.head);
}

static t_token	*create_token(void *token)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	new->token = token;
	new->next = NULL;
	return (new);
}

/*
must update tail in the caller function
*/

void	lstadd_back(void **head, void *new, void *last, e_node_type type)
{
	if (!head || !new || (*head && !last))
		return ;
	if (!*head)
		*head = new;
	else
	{
		if (type == TYPE_TOKEN)
			((t_token *)last)->next = (t_token *)new;
		else if (type == TYPE_CMD)
			((t_cmd *)last)->next = (t_cmd *)new;
		else if (type == TYPE_REDIR)
			((t_redir *)last)->next = (t_redir *)new;
	}
}

static void	set_token_type(char *s, t_token *new)
{
	if (!ft_strchr(OPERATORS, *s))
	{
		new->len = parse_word_token(s);
		new->type = word;
		return ;
	}
	new->len = 1;
	if (*s == '|')
		new->type = pipe_char;
	else if (*s == '>' && s[1] == '>')
		new->type = append;
	else if (*s == '<' && s[1] == '<')
		new->type = heredoc;
	else if (*s == '<')
		new->type = redir_in;
	else if (*s == '>')
		new->type = redir_out;
	if (new->type == append || new->type == heredoc)
		new->len = 2;
}

static size_t	parse_word_token(char *s)
{
	e_token_type	flag;
	char			*p;
	char			*tmp;

	flag = word;
	p = s;
	while (*p)
	{
		if (flag == word && (ft_strchr(WORD_DELIMITERS, *p)))
			break ;
		if (*p == '\'' && flag == word && ft_strchr(p + 1, *p))
			flag = squote;
		else if (*p == '\"' && flag == word && ft_strchr(p + 1, *p))
			flag = dquote;
		else if ((*p == '\'' && flag == squote) || (*p == '\"' && flag == dquote))
			flag = word;
		else if (*p == '$' && p[1] != '\0' && p[1] == '{' && (tmp = ft_strchr(p + 1, '}')))
			p = tmp;
		p++;
	}
	return (p - s);
}
