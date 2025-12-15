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

/*
void echo(t_node *current)
{
    bool nl;

    nl = true;
    current = current->next;
    if (ft_strncmp(current->token, "-n", 2) == 0)
    {
        nl = false;
        current = current->next;
    }
    while (current && current->type == word)
    {
        // handle quotes, env vars etc
        if (*(current->token) == '\'' || *(current->token) == '\"')
        {
            (current->token)++;
            current->len -= 2;
        }
        write(1, current->token, current->len);
        current = current->next;
    }
    if (nl == true)
            write(1, "\n", 1);
}

void parse_tokens(t_node *tokens)
{
    t_node *current;

    current = tokens;
    while (current)
    {
        if (ft_strncmp(current->token, "echo", 4) == 0)
            echo(current);
        else if (ft_strncmp(current->token, "pwd", 3) == 0)
            pwd();
        else if(ft_strncmp(current->token, "exit", 4) == 0)
            exit(0);
        current = current->next;
    }
}
*/