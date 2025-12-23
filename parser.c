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

static char	*handle_word(char *token, char *end, int status);
static int	handle_dollar(char **token, char *end, t_string *str, int status);
static char	*expand_var(char **token, char *end);

/*
maybe no need to strndup other than word tokens,
they can be null with just the token->type.
*/
t_token	*parse_tokens(t_token *tokens, int status)
{
	t_token	*current;

	if (!tokens)
		return (NULL);
	current = tokens;
	while (current)
	{
		if (current->type == word)
		{
			current->token = handle_word(current->token, current->token + current->len, status);
			if (!current->token)
				return (free_tokens(tokens, true, current), NULL);
		}
		else if (!current->next)
			return (printf("%s: %s`newline'\n", MINI, E_PARSE), free_tokens(tokens, true, current), NULL);
		else if (current->next && current->next->type != word)
			return (printf("%s: %s`%c'\n", MINI, E_PARSE, current->next->token[0]), free_tokens(tokens, true, current), NULL);
		current = current->next;
	}
	return (tokens);
}

static char	*handle_word(char *token, char *end, int status)
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
		if (*token == '\'' && flag == word && ft_strchr(token + 1, *token))
			flag = squote;
		else if (*token == '\"' && flag == word && ft_strchr(token + 1, *token))
			flag = dquote;
		else if((*token == '\'' && flag == squote) || (*token == '\"' && flag == dquote))
			flag = word;
		else if (!(*token == '$' && flag != squote && (token[1] == '?' || token[1] == '_' || token[1] == '{' || token[1] == '\'' || token[1] == '\"' || ft_isalnum(token[1]))))
			str.str[(str.i)++] = *token;
		if (*token == '$' && flag != squote && (token[1] == '?' || token[1] == '_' || token[1] == '{' || ft_isalnum(token[1])))
		{
			if (!handle_dollar(&token, end, &str, status))
				return (NULL);
		}
		else
			token++;
	}
	str.str[str.i] = 0;
	return (str.str);
}

static size_t	ft_numlen(int n, int base)
{
	size_t	count;

	count = 0;
	if (n <= 0)
		count++;
	while (n)
	{
		count++;
		n /= base;
	}
	return (count);
}

char	*itoa_status(int n, char *num)
{
	int		sign;
	size_t	len;

	sign = 1;
	n = n % EXIT_STATUS_MOD;
	len = ft_numlen(n, 10);
	num[len] = '\0';
	if (n == 0)
		num[0] = '0';
	else if (n < 0)
	{
		num[0] = '-';
		sign = -1;
	}
	while (len-- > 0 && n)
	{
		num[len] = ((n % 10) * sign) + '0';
		n /= 10;
	}
	return (num);
}

static int	handle_dollar(char **token, char *end, t_string *str, int status)
{
	char	*var;
	char	exit_code[4];
	size_t	var_len;

	(*token)++;
	var_len = 0;
	if (**token == '?')
	{
		(*token)++;
		var = itoa_status(status, exit_code);
	}
	else
		var = expand_var(token, end);
	if (var == (char *)-1)
		return (free(str->str), str->str = NULL, 0);
	if (var)
		var_len = ft_strlen(var);
	if (str->cap < str->i + var_len + (end - *token))
	{
		str->cap = str->i + var_len + (end - *token);
		str->str = ft_realloc(str->str, str->i, (str->cap) + 1);
		if (!str->str)
			return (perror(MINI), 0);
	}
	if (var)
		ft_memcpy(str->str + (str->i), var, var_len);
	str->i += var_len;
	return (1);
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
	else if (**token == '{')
		return (printf("%s: %s: %s\n",MINI, *token, E_ENV), (char *)-1);
	temp = ft_strndup(start, *token - start);
	env_var = getenv(temp);
	free(temp);
	if (**token == '}')
		(*token)++;
	return (env_var);
}
