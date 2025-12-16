/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:27:07 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/10 14:19:50 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	cleanup_linux(void);

/*
	NAME
		minishell
	DESCRIPTION:
		mini shell with following built-in commands:
		◦ echo with option -n
		◦ cd with only a relative or absolute path
		◦ pwd with no options
		◦ export with no options
		◦ unset with no options
		◦ env with no options or arguments
		◦ exit with no options
	EXTERNAL FUNC(S)
		open, close, read, write, malloc, free, perror, exit
*/

int	main(int ac, char **av, char **envp)
{
	char	*input;
	t_token	*tokens;
	t_cmd	*cmds;
	int		status;

	(void)ac;
	(void)av;
	(void)envp;
	while (1)
	{
		input = readline("$> ");
		if (!input)
			break ;
		add_history(input);
		tokens = tokenise_input(input);
		tokens = parse_tokens(tokens);
		cmds = build_ast(tokens);
		if (cmds)
			status = exec_cmds(cmds, envp, &status);
		free_cmds(cmds);
		free(input);
	}
	cleanup_linux();
}
#ifdef __linux__

void	cleanup_linux(void)
{
	rl_clear_history();
}
#else

void	cleanup_linux(void)
{
}
#endif