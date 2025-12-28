/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:36 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:25:13 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char			*itoa_status(int n, char *num);
static const char	*expand_var(char **token, char *end, t_env *env);
static int			valid_substitution(char *token, size_t len);
static size_t		ft_numlen(int n, int base);

/*
ERR is (char *)-1, just used as a sentinal value for error.
Must not be dereferenced, but only compared to detect error.
*/

int	handle_dollar(char **token, char *end, t_string *str, t_shell *shell)
{
	const char	*var;
	char		exit_code[4];
	size_t		var_len;

	(*token)++;
	var_len = 0;
	if (**token == '?')
	{
		(*token)++;
		var = itoa_status(shell->status, exit_code);
	}
	else
		var = expand_var(token, end, shell->env);
	if (var == ERR_PTR)
		return (0);
	if (!var)
		return (1);
	var_len = ft_strlen(var);
	if (str->cap < str->i + var_len + (end - *token))
	{
		str->cap = str->i + var_len + (end - *token);
		str->str = ft_realloc(str->str, str->i, (str->cap) + 1);
		if (!str->str)
			return (perror(MINI), 0);
	}
	ft_memcpy(str->str + (str->i), var, var_len);
	str->i += var_len;
	return (1);
}

static char	*itoa_status(int n, char *num)
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

static const char	*expand_var(char **token, char *end, t_env *env)
{
	const char	*env_var;
	char		*start;
	char		*tmp;

	start = *token;
	while (*token < end && (**token == '_' || ft_isalnum(**token)))
		(*token)++;
	if (*token == start && **token == '{' && valid_substitution(*token, end
			- *token))
	{
		start = *token + 1;
		*token = ft_memchr(*token, '}', end - *token);
	}
	else if (**token == '{')
		return (ERR_PTR);
	tmp = ft_strndup(start, *token - start);
	if (!tmp)
		return (perror(MINI), ERR_PTR);
	env_var = ft_getenv(env, tmp);
	free(tmp);
	if (**token == '}')
		(*token)++;
	return (env_var);
}

static int	valid_substitution(char *token, size_t len)
{
	char	*tmp_str;
	size_t	i;

	if (token[len - 1] == '\'' || token[len - 1] == '\"')
		len--;
	if (len == 1)
		return (ft_dprintf(STDERR_FILENO, "%s: %s\n", MINI, E_ENV), 0);
	tmp_str = ft_strndup(token, len);
	if (!tmp_str)
		return (perror(MINI), 0);
	i = 1;
	while (i < len && (token[i] == '_' || ft_isalnum(token[i])))
		i++;
	if (token[i] != '}')
		return (ft_dprintf(STDERR_FILENO, "%s: $%s: %s\n", MINI, tmp_str,
				E_ENV), free(tmp_str), 0);
	free(tmp_str);
	return (1);
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
