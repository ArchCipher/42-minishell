/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_parse.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 14:10:54 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 14:42:15 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char			*exit_status_to_str(int n, char *num);
static const char	*expand_env_var(char **token, char *end, t_env *env);
static int			is_valid_var_subst(char *token, size_t len);
static size_t		count_digits(int n, int base);

/*
DESCRIPTION:
	Wrapper function to expand the variable in the token.
	Returns the status variable or the expanded variable on success,
	((char *)-1) on invalid substitution.

NOTE:
	As NULL is a valid expansion, (char *)-1 is used as a sentinel value for
	invalid substitution. Must not be dereferenced, but only compared to detect
	invalid substitution.
*/

const char	*get_var(char **token, char *end, t_shell *shell)
{
	char	exit_code[4];

	if (**token == '~')
	{
		(*token)++;
		if (!shell->home || !shell->home->value)
			return (getenv("HOME"));
		return (shell->home->value);
	}
	(*token)++;
	if (**token != '?')
		return (expand_env_var(token, end, shell->env));
	(*token)++;
	return (exit_status_to_str(shell->status, exit_code));
}

/*
DESCRIPTION:
	Converts and returns the status variable as a string within the range of
	0-255.
*/

static char	*exit_status_to_str(int n, char *num)
{
	size_t	len;

	n = n & EXIT_STATUS_MASK;
	len = count_digits(n, 10);
	num[len] = '\0';
	if (n == 0)
		num[0] = '0';
	while (len-- > 0 && n)
	{
		num[len] = (n % 10) + '0';
		n /= 10;
	}
	return (num);
}

/*
DESCRIPTION:
	Expands the variable in the token.
	Returns the expanded variable on success, ((char *)-1) on invalid
	substitution.

NOTE:
	As NULL is a valid expansion, (char *)-1 is used as a sentinel value for
	invalid substitution and malloc failure. Must not be dereferenced, but only
	compared to detect invalid substitution.
*/

static const char	*expand_env_var(char **token, char *end, t_env *env)
{
	const char	*env_var;
	char		*start;
	char		*tmp;

	start = *token;
	while (*token < end && (**token == '_' || ft_isalnum(**token)))
		(*token)++;
	if (*token == start && **token == '{')
	{
		if (!is_valid_var_subst(*token, end - *token))
			return (((char *)-1));
		start = *token + 1;
		*token = ft_memchr(*token, '}', end - *token);
	}
	tmp = ft_strndup(start, *token - start);
	if (!tmp)
		return (perror(MINI), ((char *)-1));
	env_var = ft_getenv(env, tmp);
	free(tmp);
	if (**token == '}')
		(*token)++;
	return (env_var);
}

/*
DESCRIPTION:
	Checks if the substitution is valid.
	Returns 1 if valid, 0 if invalid.
*/

static int	is_valid_var_subst(char *token, size_t len)
{
	size_t	i;

	if (token[len - 1] == '\'' || token[len - 1] == '\"')
		len--;
	if (len == 1)
		return (perr_msg(E_ENV, NULL, NULL, false), 0);
	i = 1;
	while (i < len && (token[i] == '_' || ft_isalnum(token[i])))
		i++;
	if (token[i] != '}')
	{
		ft_dprintf(STDERR_FILENO, "%s: $", MINI);
		write(STDERR_FILENO, token, len);
		ft_dprintf(STDERR_FILENO, ": %s\n", E_ENV);
		return (0);
	}
	return (1);
}

/*
DESCRIPTION:
	Calculates the length of the number in the given base.
	Returns the length of the number.
*/

static size_t	count_digits(int n, int base)
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
