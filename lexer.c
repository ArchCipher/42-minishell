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

static void		save_token(char *s, t_token *new);
static size_t	word_token(char *s);

t_token	*create_new_token(void *token)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (NULL);
	new->token = token;
	new->next = NULL;
	return (new);
}

void	free_list(t_token *tokens, bool free_content)
{
	t_token	*current;

	while (tokens)
	{
		current = tokens;
		tokens = tokens->next;
		if (free_content)
			free(current->token);
		free(current);
	}
}

void	lstadd_back(void **tokens, void *new, void *last, e_node_type type)
{
	if (!tokens || !new || !last)
		return ;
	if (!*tokens)
		*tokens = new;
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


t_token	*split_into_tokens(char *s)
{
	t_token	*tokens;
	t_token	*current;
	t_token	*new;

	if (!s)
		return (NULL);
	tokens = NULL;
	while (*s)
	{
		while (*s && ft_isspace(*s))
			s++;
		if (!*s)
			break ;
		new = create_new_token(s);
		if (!new)
			return (free_list(tokens, false), NULL);
		save_token(s, new);
		lstadd_back((void **)&tokens, (void *)new, (void *)current, TYPE_TOKEN);
		current = new;
		s += current->len;
	}
	return (tokens);
}

static void	save_token(char *s, t_token *new)
{
	if (!ft_strchr(OPERATORS, *s))
	{
		new->len = word_token(s);
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

static size_t	word_token(char *s)
{
	e_token_type	flag;
	char			*p;

	flag = word;
	p = s;
	while (*p)
	{
		if (flag == word && ft_isspace(*p))
			break ;
		if (*p == '\'' && flag == word && ft_strchr(p + 1, *p))
			flag = squote;
		else if (*p == '\"' && flag == word && ft_strchr(p + 1, *p))
			flag = dquote;
		else if ((*p == '\'' && flag == squote) || (*p == '\"' && flag == dquote))
			flag = word;
		p++;
	}
	return (p - s);
}
