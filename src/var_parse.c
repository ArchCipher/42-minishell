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

static void		status_int_to_str(t_shell *shell);
static const char	*expand_env_var(char **token, t_list *envs);
// static int		is_valid_var_subst(char *token, size_t len);
static size_t	count_digits(int n, int base);

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

const char	*get_var(char **str, t_shell *shell)
{
	t_env	*home;

	if (**str == '~')
	{
		(*str)++;
		home = get_env(shell->home);
		if (home && home->value)
			return (home->value);
		return (NULL);
	}
	(*str)++;
	if (**str != '?')
		return (expand_env_var(str, shell->env))
	(*str)++;
	status_int_to_str(shell);
	return (shell->exit_code);
}

/*
DESCRIPTION:
	Converts and returns the status variable as a string within the range of
	0-255.
*/

static void	status_int_to_str(t_shell *shell)
{
	int		n;
	size_t	len;

	n = shell->status & EXIT_STATUS_MASK;
	len = count_digits(n, 10);
	shell->exit_code[len] = '\0';
	if (n == 0)
		shell->exit_code[0] = '0';
	while (len-- > 0 && n)
	{
		shell->exit_code[len] = (n % 10) + '0';
		n /= 10;
	}
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

// maybe should err when no closing brackets

static const char	*expand_env_var(char **str, t_list *envs)
{
	const char	*var;
	char		*start;
	char		*tmp;

	if (**str == '{')
		(*str)++;
	start = *str;
	while (**str && (**str == '_' || ft_isalnum(**str)))
		(*str)++;
	if (*str - start == 0)
		return (perr_msg(E_ENV, NULL, NULL, false), ((char *)-1));
	tmp = ft_strndup(start, *str - start);
	if (!tmp)
		return (perror(MINI), ((char *)-1));
	var = ft_getenv(envs, tmp);
	free(tmp);
	if (**str == '}')
		(*str)++;
	return (var);
}

/*
DESCRIPTION:
	Checks if the substitution is valid.
	Returns 1 if valid, 0 if invalid.
*/
// ft_dprintf("%s: %s\n", MINI, E_ENV);

// static int	is_valid_var_subst(char *token, size_t len)
// {
// 	size_t	i;

// 	if (token[len - 1] == '\'' || token[len - 1] == '\"')
// 		len--;
// 	if (len == 1)
// 		return (perr_msg(E_ENV, NULL, NULL, false), 0);
// 	i = 1;
// 	while (i < len && (token[i] == '_' || ft_isalnum(token[i])))
// 		i++;
// 	if (token[i] != '}')
// 	{
// 		ft_dprintf(STDERR_FILENO, "%s: $", MINI);
// 		write(STDERR_FILENO, token, len);
// 		ft_dprintf(STDERR_FILENO, ": %s\n", E_ENV);
// 		return (0);
// 	}
// 	return (1);
// }

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
