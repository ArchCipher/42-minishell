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

static void    init_shell(char **envp, t_shell *shell);

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

	(void)av;
	if (ac != 1)
		return (ft_dprintf(STDERR_FILENO, "%s: %s\n", MINI, E_MANY_ARGS), 1);
	init_shell(envp, &shell);
	while (1)
	{
		input = readline(PROMPT);
		if (g_signal == SIGINT)
			handle_shell_signal(&shell.status);
		if (!input)
			break ;
		add_history(input);
		tokens = tokenise_input(input);
		tokens = parse_tokens(tokens, &shell);
		cmds = build_ast(tokens);
		free(input);
		if (cmds && !process_heredoc(cmds, &shell))
			shell.status = exec_cmds(cmds, &shell);
		free_cmds(cmds);
	}
	exit_shell(0, NULL, &shell);
}

static void	update_pwd(t_shell *shell)
{
	t_env	*old_pwd;
	t_env	*pwd;

	old_pwd = env_lookup(shell->env, "OLDPWD");
	if (!old_pwd)
	{
		if (update_env(shell, "OLDPWD", NULL))
			return ;
		old_pwd = env_lookup(shell->env, "OLDPWD");	
	}
	old_pwd->exported = false;
	if (old_pwd->value)
		free(old_pwd->value);
	old_pwd->value = NULL;
	pwd = env_lookup(shell->env, "PWD");
	if (!pwd)
	{
		if (update_env(shell, "PWD", NULL))
			return ;
		pwd = env_lookup(shell->env, "PWD");
	}
	if (pwd->value)
		free(pwd->value);
	pwd->value = getcwd(NULL, 0);
}

static void    init_shell(char **envp, t_shell *shell)
{
    init_signals(&shell->original_term);
    shell->status = 0;
    shell->env = NULL;
    while(*envp)
    {
        if (update_env(shell, *envp, NULL))
            exit_shell(1, NULL, shell);
        envp++;
    }
	update_pwd(shell);
}

/*
exits from shell should cleanup...
garbage collector + exit
*/

void exit_shell(int exit_code, t_cmd *cmds, t_shell *shell)
{
	free_cmds(cmds);
    free_env(shell->env);
	if (isatty(STDIN_FILENO))
        tcsetattr(STDIN_FILENO, TCSANOW, &shell->original_term);
    rl_clear_history();
    exit(exit_code);
}
