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
	printf("%s\n", buf);
	free(buf);
}

void	exec_cd(const char *path)
{
	if (chdir(path) == -1)
		perror("Error: ");
}

void exec_echo(char **args)
{
    bool nl;

    nl = true;
    if (ft_strcmp(*args, "-n") == 0)
    {
        nl = false;
        args++;
    }
    while (*args)
    {
        printf("%s", *args);
        if (!args[1])
            break ;
        printf(" ");
        args++;
    }
    if (nl == true)
        printf("\n");
}

void    exec_exit(char *s)
{
    if (s == NULL)
        exit(0);
    exit(ft_atoi(s));
}