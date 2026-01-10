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

static int	status_atoi(const char *str);

/*
DESCRIPTION:
	Cleans up the resources and exits the shell.
	It also restores the original terminal settings and clears the history.
*/

void	exit_shell(t_list *cmds, t_shell *shell)
{
	ft_lstclear(&cmds, free_cmd);
	ft_lstclear(&shell->env, free_env);
	if (isatty(STDIN_FILENO))
		tcsetattr(STDIN_FILENO, TCSANOW, &shell->original_term);
	rl_clear_history();
	exit(shell->status);
}

/*
DESCRIPTION:
	Executes the exit builtin and returns the exit status in range of 0-255.
	Checks if the argument is a valid numeric value and prints error if not.
*/

int	exec_exit(char **args, t_shell *shell)
{
	int	n;

	errno = 0;
	if (args[1] == NULL)
		return (shell->status);
	n = status_atoi(args[1]) & EXIT_STATUS_MASK;
	if (!errno && args[2])
		return (perr_msg(*args, E_MANY_ARGS, NULL, false), 1);
	if (!*args[1] || errno)
		return (perr_msg(*args, args[1], E_EXIT_CODE, false),
			EXIT_NUMERIC_ERROR);
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
			return (errno = ERANGE, (int)LONG_MAX);
		if (sign == -1 && (LONG_MAX - (num * 10)) <= (*str - '0') - 1)
			return (errno = ERANGE, (int)LONG_MIN);
		num = (num * 10) + (*str - '0');
		str++;
	}
	if (*str)
		errno = EINVAL;
	return ((int)(num * sign));
}
