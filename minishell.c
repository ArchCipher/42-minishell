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
	t_shell	shell;
	char	*input;
	t_token	*tokens;
	t_cmd	*cmds;

	(void)ac;
	(void)av;
	shell.status = 0;
	shell.envp = envp;
	init_signals(&shell.original_term);
	while (1)
	{
		input = readline(PROMPT);
		if (g_signal == SIGINT)
			handle_shell_signal(&shell.status);
		if (!input)
			break ;
		add_history(input);
		tokens = tokenise_input(input);
		tokens = parse_tokens(tokens, shell.status);
		cmds = build_ast(tokens);
		free(input);
		if (cmds && !process_heredoc(cmds, &shell))
			shell.status = exec_cmds(cmds, envp);
		free_cmds(cmds);
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &shell.original_term);// should be done during any exit from shell
	rl_clear_history();	
}
