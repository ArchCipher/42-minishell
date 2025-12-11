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

char *end_quote(char *s, char c)
{
	while (*s)
	{
		if (*s == c)
			return (s);
		s++;
	}
	return (NULL);
}

char	*expand_var(char *token, char *var, size_t len)
{
	char	*env_var;
	size_t	i;
	size_t	prefix_len;
	size_t	suffix_len;
	size_t	env_len;
	char	*new;

	i = 0;
	while (var[i] && (var[i] != '\'' && var[i] != '\"' && var[i] != '$'))
		i++;
	env_var = ft_strndup(var, i);
	if (!env_var)
		return (free(token), NULL);
	env_var = getenv(env_var);
	if (!env_var)
		return (free(token), NULL);
	prefix_len = var - token - 1;
	suffix_len = (token + len) - (var + i);
	env_len = ft_strlen(env_var);
	new = malloc(prefix_len + env_len + suffix_len + 1);
	if (!new)
		return (free(token), NULL);
	new[prefix_len + env_len + suffix_len] = 0;
	ft_memcpy(new, token, prefix_len);
	ft_memcpy(new + prefix_len, env_var, env_len);
	ft_memcpy(new + prefix_len + env_len, var + i, suffix_len);
	free(token);
	return (new);
}

char	*handle_word(char *token, size_t len)
{
	char	*new;
	char	*s;
	char	*d;
	char	*equote;

	s = token;
	new = NULL;
	if (*s == '\'' || *s == '\"')
	{
		equote = end_quote(s + 1, *s);
		if (!equote)
			new = ft_strndup(token, len);
		else
			new = ft_strndup(token + 1, (equote - token) - 1);
	}
	else
		new = ft_strndup(token, len);
	if (!new)
		return (NULL);
	if (*s == '\'')
		return (new);
	d = new;
	while (d && *d)
	{
		d = ft_strchr(d, '$');
		if (!d)
			break ;
		new = expand_var(new, ++d, len);
		if (!new)
			return (NULL);
	}
	return (new);
}

t_node	*parse_tokens(t_node *tokens)
{
	t_node	*current;

	current = tokens;
	while (current)
	{
		if (current->type == word)
			current->token = handle_word(current->token, current->len);
		else
			current->token = ft_strndup(current->token, current->len);
		if (!current->token)
			return (free_list(tokens, true), NULL);
		printf("tok: %s\n", current->token);
		current = current->next;
	}
	return (tokens);
}

// echo $name'hello'"hello" should be kiruhellohello - need to remove all quotes.