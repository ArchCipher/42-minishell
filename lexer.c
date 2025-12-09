/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:27:11 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/04 18:32:31 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_node	*new_node(void *token)
{
	t_node	*new;

	new = malloc(sizeof(t_node));
	if (!new)
		return (NULL);
	new->token = token;
	new->next = NULL;
	return (new);
}

void    free_list(t_node *tokens)
{
    t_node *current;

    while (tokens)
    {
        current = tokens;
        tokens = tokens->next;
        free(current);
    }
}

void    save_quote_token(char **s, t_node *new)
{
    if (**s != '\'' && **s != '\"')
        return ;
	new->type = squote;
    if (**s == '\"')
        new->type = dquote;
	(*s)++;
	new->token = *s;
    new->len = 0;
	while ((*s)[new->len] && ((new->type == squote && (*s)[new->len] != '\'' )|| (new->type == dquote && (*s)[new->len] != '\"')))
		new->len++;
	if ((*s)[new->len] == '\'' || (*s)[new->len] == '\"')
		(*s)++;
}

void    save_token(char **s, t_node *new)
{
    if (**s == '\'' || **s == '\"')
		save_quote_token(s, new);
    else if (ft_strchr(OPERATORS, **s))
    {
        new->len = ft_strspn(*s, OPERATORS);
        new->type = operator;
		(*s) += new->len;
    }
    else
    {
        new->len = ft_strcspn(*s, REJECT);
        new->type = word;
		(*s) += new->len;	
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
			return (free_list(tokens), NULL);
		save_token(&s, new);	
		if (!tokens)
			tokens = new;
		else
			current->next = new;
		current = new;
	}
	return (tokens);
}

/*
IN_WORD
IN_OPERATOR
IN_SQUOTE
IN_DQUOTE
*/