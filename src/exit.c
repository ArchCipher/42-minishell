/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:47 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 19:52:01 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	status_atoi(const char *str);
/*
prints numeric argument required for any non numeric value
(floats, letters, numbers mixed with letters etc)
*/

int	exec_exit(char **s)
{
	int	n;

	if (*s == NULL)
		return (0);
	n = status_atoi(*s) % EXIT_STATUS_MOD;
	if (errno == EINVAL)
		ft_dprintf(STDERR_FILENO, "%s: exit: %s: %s\n", MINI, *s, E_EXIT_CODE);
	else if (s[1])
	{
		ft_dprintf(STDERR_FILENO, "%s: exit: %s\n", MINI, E_MANY_ARGS);
		n = 1;
	}
	return (n);
}

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
	while (*str >= '0' && *str <= '9')
	{
		if (sign == 1 && (LONG_MAX - (num * 10)) <= (*str - '0'))
			return ((int)LONG_MAX);
		if (sign == -1 && (LONG_MAX - (num * 10)) <= (*str - '0') - 1)
			return ((int)LONG_MIN);
		num = (num * 10) + (*str - '0');
		str++;
	}
	if (*str)
	{
		errno = EINVAL;
		return (EXIT_NUMERIC_ERROR);
	}
	return ((int)(num * sign));
}
