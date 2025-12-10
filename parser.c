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

int	end_quote(char *s, char c)
{
	while (*s)
	{
		if (*s == c)
			return (1);
		s++;
	}
	return (0);
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
	env_var = strndup(var, i);
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

	s = token;
	new = NULL;
	if ((*s == '\'' || *s == '\"') && end_quote(s + 1, *s))
		new = ft_strndup(token + 1, len -= 2);
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
	t_node	*p_tokens;
	t_node	*current;
	t_node	*p_current;
	t_node	*new;
	char	*token;

	current = tokens;
	p_tokens = NULL;
	while (current)
	{
		if (current->type == word)
			token = handle_word(current->token, current->len);
		else
			token = ft_strndup(current->token, current->len);
		if (!token)
			return (free_list(tokens, false), free_list(p_tokens, true), NULL);
		new = new_node(token);
		if (!new)
			return (free_list(tokens, false), free_list(p_tokens, true), NULL);
		if (!p_tokens)
			tokens = new;
		else
			p_current->next = new;
		p_current = new;
		current = current->next;
	}
	free_list(tokens, false);
	return (p_tokens);
}
