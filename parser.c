/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 13:47:52 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:53:15 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*handle_word(char *token, char *end, t_shell *shell);

/*
strndup only word tokens, others are pointers input with just the token->type.
*/
t_token	*parse_tokens(t_token *tokens, t_shell *shell)
{
	t_token	*current;
	t_token	*prev;
	t_token	*tmp;

	if (!tokens)
		return (NULL);
	current = tokens;
	prev = NULL;
	while (current)
	{
		if (current->type == word)
		{
			current->token = handle_word(current->token, current->token
					+ current->len, shell);
			if (!current->token)
				return (free_tokens(tokens, true, current), NULL);
			if (!*current->token && !prev)
			{
				free(current->token);
				tokens = current->next;
				tmp = current;
				current = current->next;
				free(tmp);
				continue ;
			}
		}
		else if (!current->next)
			return (ft_dprintf(STDERR_FILENO, "%s: %s`newline'\n", MINI,
					E_PARSE), free_tokens(tokens, true, current), NULL);
		else if (current->next && current->next->type != word)
			return (ft_dprintf(STDERR_FILENO, "%s: %s`%c'\n", MINI, E_PARSE,
					current->next->token[0]), free_tokens(tokens, true,
					current), NULL);
		prev = current;
		current = current->next;
	}
	return (tokens);
}

/*
when accessing token + 1 or token[1],
check if token + 1 < end and then check token[1] or use token + 1
would be nice to check end quote, but subject doesn't require this
	if (*token == '\'' && flag == word && ft_memchr(token + 1, '\'', end - token
		- 1))
*/
static char	*handle_word(char *token, char *end, t_shell *shell)
{
	e_token_type	flag;
	t_string		str;

	str.str = malloc((end - token) + 1);
	if (!str.str)
		return (perror(MINI), NULL);
	str.cap = end - token;
	str.i = 0;
	flag = word;
	while (token < end)
	{
		if (*token == '\'' && flag == word)
			flag = squote;
		else if (*token == '\"' && flag == word)
			flag = dquote;
		else if ((*token == '\'' && flag == squote) || (*token == '\"'
				&& flag == dquote))
			flag = word;
		else if (!(*token == '$' && flag != squote && token + 1 < end
				&& (ft_strchr(EXPANDABLE, token[1]) || ft_isalnum(token[1]))))
			str.str[(str.i)++] = *token;
		if (*token == '$' && flag != squote && token + 1 < end
			&& (ft_strchr(EXPANDABLE, token[1]) || ft_isalnum(token[1])))
		{
			if (!handle_dollar(&token, end, &str, shell))
				return (free(str.str), NULL);
		}
		else
			token++;
	}
	str.str[str.i] = 0;
	return (str.str);
}
