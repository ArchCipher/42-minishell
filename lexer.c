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

static void		save_token(char *s, t_node *new);
static size_t	word_token(char *s);

t_node	*new_node(void *token)
{
	t_node	*new;

	new = malloc(sizeof(t_node));
	if (!new)
		return (NULL);
	new->token = token;
	new->next = NULL;
	return (new);
}

void	free_list(t_node *tokens, bool free_content)
{
	t_node	*current;

	while (tokens)
	{
		current = tokens;
		tokens = tokens->next;
		if (free_content)
			free(current->token);
		free(current);
	}
}

t_node	*split_into_tokens(char *s)
{
	t_node	*tokens;
	t_node	*current;
	t_node	*new;

	if (!s)
		return (NULL);
	tokens = NULL;
	while (*s)
	{
		while (*s && ft_isspace(*s))
			s++;
		if (!*s)
			break ;
		new = new_node(s);
		if (!new)
			return (free_list(tokens, false), NULL);
		save_token(s, new);
		if (!tokens)
			tokens = new;
		else
			current->next = new;
		current = new;
		s += current->len;
	}
	return (tokens);
}

static void	save_token(char *s, t_node *new)
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
	enum e_token_type	flag;
	char				*p;

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
