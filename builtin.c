/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 14:01:35 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/10 14:04:34 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
echo	0 unless write error
pwd	0 unless error
cd	0 on success, 1 on failure
exit	does not return
export	0 / 1 depending on validity
unset	0
env	0
command not found	127
permission denied	126
*/

int exec_pwd(void)
{
	char	*buf;

	buf = NULL;
	buf = getcwd(buf, 0);
	if (!buf)
		return (perror(MINI), 1);
	if (write(STDOUT_FILENO, buf, ft_strlen(buf))== -1 || write(STDOUT_FILENO, "\n", 1) == -1)
        return (free(buf), 1);
	free(buf);
    return (0);
}

int exec_cd(const char *path)
{
	if (chdir(path) == -1)
		return (perror(MINI), 1);
    return (0);
}

/*
This doesn't work with printf for echo -n to infile.
It needs to be fflush(force flushed) in this case to make it work.

printf() is part of C standard I/O (stdio), which uses buffers:
stdout is line-buffered when connected to a terminal
stdout is fully-buffered when connected to a file or pipe
Buffered data is only flushed:
On newline (\n) if line-buffered
When buffer is full
Or when you explicitly call fflush(stdout)

So must use write
*/

int exec_echo(char **args)
{
    bool    nl;

    nl = true;
    if (!args || !*args)
        return (write(1, "\n", 1));
    if (ft_strcmp(*args, "-n") == 0)
    {
        nl = false;
        args++;
    }
    while (*args)
    {
        if (write(STDOUT_FILENO, *args, ft_strlen(*args)) == -1)
            return (1);
        if (!args[1])
            break ;
        if (write(STDOUT_FILENO, " ", 1) == -1)
            return (1);
        args++;
    }
    if (nl == true && write(STDOUT_FILENO, "\n", 1) == -1)
        return (1);
    return (0);
}

int	simple_atoi(const char *str)
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
    if (!(*str >= '0' && *str <= '9'))
        return (errno = EINVAL, EXIT_NUMERIC_ERROR);
	while (*str >= '0' && *str <= '9')
	{
		if (sign == 1 && (LONG_MAX - (num * 10)) <= (*str - '0'))
			return ((int)LONG_MAX);
		if (sign == -1 && (LONG_MAX - (num * 10)) <= (*str - '0') - 1)
			return ((int)LONG_MIN);
		num = (num * 10) + (*str - '0');
		str++;
	}
	return ((int)(num * sign));
}

/*
The env utility exits 0 on success, and >0 if an error occurs.
An exit status of 126 indicates that utility was found, but could not be executed.
An exit status of 127 indicates that utility could not be found.

int exec_env();
*/

void    exec_exit(char *s)
{
    int i;

    if (s == NULL)
        exit(0);
    i = simple_atoi(s) % EXIT_STATUS_MOD;
    if (errno == EINVAL)
        printf("%s: exit: %s: numeric argument required\n", MINI, s);
    exit(i);
}