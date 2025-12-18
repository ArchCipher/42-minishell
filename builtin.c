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

void	exec_pwd(void)
{
	char	*buf;

	buf = NULL;
	buf = getcwd(buf, 0);
	if (!buf)
		return (perror("Error: "));
	write(STDOUT_FILENO, buf, ft_strlen(buf));
    write(STDOUT_FILENO, "\n", 1);
	free(buf);
}

void	exec_cd(const char *path)
{
	if (chdir(path) == -1)
		perror("Error: ");
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

void exec_echo(char **args)
{
    bool nl;

    nl = true;
    if (!args || !*args)
        return ;
    if (ft_strcmp(*args, "-n") == 0)
    {
        nl = false;
        args++;
    }
    while (*args)
    {
        write(STDOUT_FILENO, *args, ft_strlen(*args));
        if (!args[1])
            break ;
        write(STDOUT_FILENO, " ", 1);
        args++;
    }
    if (nl == true)
        write(STDOUT_FILENO, "\n", 1);
}

void    exec_exit(char *s)
{
    if (s == NULL)
        exit(0);
    exit(ft_atoi(s));
}