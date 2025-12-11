/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 13:47:52 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/10 21:18:52 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_var(char **token, char *end)
{
	char	*env_var;
	char	*start;
	char	*temp;

	start = *token;
	while (*token < end && (**token == '_' || ft_isalnum(**token)))
		(*token)++;
	if (**token == '{' &&((*token)[1] == '_' || ft_isalnum((*token)[1])) &&(temp = ft_strchr(*token, '}')))
	{
		start = *token + 1;
		*token = temp;
	}
	temp = ft_strndup(start, *token - start);
	env_var = getenv(temp);
	free(temp);
	if (**token == '}')
		(*token)++;
	return (env_var);
}

char	*handle_word(char *token, char *end)
{
	enum e_token_type flag;
	char	*new;
	char	*var;
	size_t	j;
	size_t	cap;
	size_t	len;

	new = malloc((end - token) + 1);
	if (!new)
		return (NULL);
	cap = end - token;
	j = 0;
	flag = word;
	while (token < end)
	{
		if (*token == '\'' && flag == word && ft_strchr(token + 1, *token))
			flag = squote;
		else if (*token == '\"' && flag == word && ft_strchr(token + 1, *token))
			flag = dquote;
		else if((*token == '\'' && flag == squote) || (*token == '\"' && flag == dquote))
			flag = word;
		else if (!(*token == '$' && flag != squote && (token[1] == '_' || token[1] == '{' || token[1] == '\'' || token[1] == '\"' || ft_isalnum(token[1]))))
			new[j++] = *token;
		if (*token == '$' && flag != squote && (token[1] == '_' || token[1] == '{' || ft_isalnum(token[1])))
		{
			token++;
			len = 0;
			var = expand_var(&token, end);
			if (var)
				len = ft_strlen(var);
			if (cap < j + len + (end - token))
			{
				cap = j + len + (end - token);
				new = realloc(new, cap + 1);
				if (!new)
					return (NULL);
			}
			ft_memcpy(new + j, var, len);
			j += len;
		}
		else
			token++;
	}
	new[j] = 0;
	printf("new: %s\n", new);
	return (new);
}

t_node	*parse_tokens(t_node *tokens)
{
	t_node	*current;

	current = tokens;
	while (current)
	{
		if (current->type == word)
			current->token = handle_word(current->token, current->token + current->len);
		else
			current->token = ft_strndup(current->token, current->len);
		if (!current->token)
			return (free_list(tokens, true), NULL);
		// printf("tok: %s\n", current->token);
		current = current->next;
	}
	return (tokens);
}
