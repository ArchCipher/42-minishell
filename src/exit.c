/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:47 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/07 18:19:52 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	free_env(t_env *env);
static int	status_atoi(const char *str);

/*
DESCRIPTION:
	Cleans up the resources and exits the shell.
	It also restores the original terminal settings and clears the history.
*/

void	exit_shell(int exit_code, t_cmd *cmds, t_shell *shell)
{
	free_cmds(cmds);
	free_env(shell->env);
	if (isatty(STDIN_FILENO))
		tcsetattr(STDIN_FILENO, TCSANOW, &shell->original_term);
	rl_clear_history();
	exit(exit_code);
}

/*
DESCRIPTION:
	Frees the environment list and their content.
*/

static void	free_env(t_env *env)
{
	t_env	*tmp;

	tmp = env;
	while (env)
	{
		free(env->key);
		if (env->value)
			free(env->value);
		tmp = env;
		env = env->next;
		free(tmp);
	}
}

/*
DESCRIPTION:
	Executes the exit builtin and returns the exit status in range of 0-255.
	Checks if the argument is a valid numeric value and prints error if not.
*/

int	exec_exit(char **s)
{
	int	n;

	errno = 0;
	if (s[1] == NULL)
		return (0);
	n = status_atoi(s[1]) & EXIT_STATUS_MASK;
	if (errno != EINVAL && s[2])
		return (perr_msg(*s, E_MANY_ARGS, NULL, false), 1);
	if (!*s[1] || errno == EINVAL)
		return (perr_msg(*s, s[1], E_EXIT_CODE, false), EXIT_NUMERIC_ERROR);
	return (n);
}

/*
DESCRIPTION:
	Converts the string to an integer and returns the exit status.
	Checks if the string is a valid numeric value and sets errno if not.
*/

static int	status_atoi(const char *str)
{
	long	num;
	int		sign;

	errno = 0;
	num = 0;
	sign = 1;
	if (*str == '-')
		sign = -1;
	if (*str == '-' || *str == '+')
		str++;
	while (ft_isdigit(*str))
	{
		if (sign == 1 && (LONG_MAX - (num * 10)) <= (*str - '0'))
			return ((int)LONG_MAX);
		if (sign == -1 && (LONG_MAX - (num * 10)) <= (*str - '0') - 1)
			return ((int)LONG_MIN);
		num = (num * 10) + (*str - '0');
		str++;
	}
	if (*str)
		errno = EINVAL;
	return ((int)(num * sign));
}
