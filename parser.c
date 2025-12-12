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

static char	*handle_word(char *token, char *end);
static void	handle_dollar(char **token, char *end, t_string *str);
static char	*expand_var(char **token, char *end);

t_token	*parse_tokens(t_token *tokens)
{
	t_token	*current;

	if (!tokens)
		return (NULL);
	current = tokens;
	while (current)
	{
		if (current->type == word)
			current->token = handle_word(current->token, current->token + current->len);
		else
			current->token = ft_strndup(current->token, current->len);
		if (!current->token)
			return (free_list(tokens, true), NULL);
		current = current->next;
	}
	return (tokens);
}

static char	*handle_word(char *token, char *end)
{
	enum e_token_type	flag;
	t_string			str;
	
	str.str = malloc((end - token) + 1);
	if (!str.str)
		return (NULL);
	str.cap = end - token;
	str.i = 0;
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
			str.str[(str.i)++] = *token;
		if (*token == '$' && flag != squote && (token[1] == '_' || token[1] == '{' || ft_isalnum(token[1])))
		{
			handle_dollar(&token, end, &str);
			if (!str.str)
				return (NULL);
		}
		else
			token++;
	}
	str.str[str.i] = 0;
	// printf("token: %s\n", str.str);
	return (str.str);
}

static void	handle_dollar(char **token, char *end, t_string *str)
{
	char	*var;
	size_t	var_len;

	(*token)++;
	var_len = 0;
	var = expand_var(token, end);
	if (var)
		var_len = ft_strlen(var);
	if (str->cap < str->i + var_len + (end - *token))
	{
		str->cap = str->i + var_len + (end - *token);
		str->str = ft_realloc(str->str, str->i, (str->cap) + 1);
		if (!str->str)
			return ;
	}
	ft_memcpy(str->str + (str->i), var, var_len);
	str->i += var_len;
}

static char	*expand_var(char **token, char *end)
{
	char	*env_var;
	char	*start;
	char	*temp;

	start = *token;
	while (*token < end && (**token == '_' || ft_isalnum(**token)))
		(*token)++;
	if (*token == start && **token == '{' && ((*token)[1] == '_' || ft_isalnum((*token)[1])) &&(temp = ft_strchr(*token, '}')))
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
