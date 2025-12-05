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
/*
char	*create_set(char *set, char *accept)
{
	ft_memset(set, 0, BUFLEN);
	while (*accept)
		set[(unsigned char)*accept++] = 1;
	return (set);
}
*/

void    save_quote_token(char *s, t_node *new)
{
    if (*s != '\'' && *s != '\"')
        return ;
	new->type = squote;
    if (*s == '\"')
        new->type = dquote;
	new->token = s;
    new->len = 1;
	while (s[new->len] && s[new->len] != '\'' && s[new->len] != '\"')
		new->len++;
	if (s[new->len] == '\'' || s[new->len] == '\"')
		new->len++;
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
		// if (!new)
		// 	return (free_list(tokens));
		if (!tokens)
		{
			tokens = new;
			current = tokens;
		}
		else
			current = new;
        if (*s == '\'' || *s == '\"')
			save_quote_token(s, new);
        else if (ft_strchr(OPERATORS, *s))
        {
            current->len = ft_strspn(s, OPERATORS);
			current->type = operator;
        }
        else
        {
            current->len = ft_strcspn(s, REJECT);
            current->type = word;
		}
        s += current->len;
		printf("len: %zu type: %d\n", current->len, current->type);
		current = current->next;
	}
	return (tokens);
}

/*
IN_WORD
IN_OPERATOR
IN_SQUOTE
IN_DQUOTE
*/