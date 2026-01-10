/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 14:35:35 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/07 11:31:06 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	do_cd(char **args, t_shell *shell);
static char	*get_env_value(t_list *lst);

/*
DESCRIPTION:
	Executes the cd builtin command using chdir().
	Updates the PWD and OLDPWD environment variables if PWD and OLDPWD exist.
	Returns 0 on success, 1 if chdir() or getcwd() fails.
*/

#ifdef __linux__

int	exec_cd(char **args, t_shell *shell)
{
	if (args[1] && args[2] && args[1][0] != '-')
		return (perr_msg(*args, E_MANY_ARGS, NULL, false), 1);
	if (args[1] && args[2] && args[1][0] == '-' && !args[1][1])
		return (perr_msg(*args, E_MANY_ARGS, NULL, false), 1);
	if (args[1] && args[2] && args[3])
		return (perr_msg(*args, E_MANY_ARGS, NULL, false), 1);
	return (do_cd(args, shell));
}
#else

int	exec_cd(char **args, t_shell *shell)
{
	return (do_cd(args, shell));
}
#endif

/*
DESCRIPTION:
	Changes the current working directory to the specified directory.
	If no directory is specified, changes to HOME. Returns 0 on success, 1 on
	error.
*/

static int	do_cd(char **args, t_shell *shell)
{
	char		*home;
	char		*oldpwd;
	const char	*dir;

	dir = args[1];
	home = get_env_value(shell->home);
	oldpwd = get_env_value(shell->oldpwd);
	if (!dir && !home)
		return (perr_msg(*args, E_HOME, NULL, false), 1);
	else if (!dir || (dir[0] == '-' && dir[1] == '-' && !dir[2] && !args[2]))
		dir = home;
	else if (!dir || (dir[0] == '-' && dir[1] == '-' && !dir[2] && args[2]))
		dir = args[2];
	else if (dir && dir[0] == '-' && dir[1])
		return (perr_tok_msg(*args, args[1], 2, E_OPTION), 2);
	else if (!ft_strcmp(dir, "-") && !oldpwd)
		return (perr_msg(*args, E_OLDPWD, NULL, false), 1);
	else if (!ft_strcmp(dir, "-"))
		dir = oldpwd;
	if (chdir(dir) == -1)
		return (perr_msg(*args, dir, strerror(errno), false), 1);
	update_pwds(shell);
	if (args[1] && args[1][0] == '-' && !args[1][1])
		exec_pwd();
	return (0);
}

/*
DESCRIPTION:
	Updates the PWD and OLDPWD environment variables.

NOTE:
	OLDPWD value is freed, then set to point to the previous PWD.
	PWD value is updated to the current working directory.
	PWD value should not be freed here because its previous memory is now owned
	by OLDPWD.
*/

void	update_pwds(t_shell *shell)
{
	t_env	*pwd;
	t_env	*oldpwd;

	if (!shell->pwd || !shell->pwd->content)
	{
		if (update_env(shell, "PWD", NULL))
			return ;
		shell->pwd = env_lookup(shell->env, "PWD");
	}
	if (!shell->oldpwd || !shell->oldpwd->content)
	{
		if (update_env(shell, "OLDPWD", NULL))
			return ;
		shell->oldpwd = env_lookup(shell->env, "OLDPWD");
	}
	pwd = get_env(shell->pwd);
	oldpwd = get_env(shell->oldpwd);
	if (oldpwd->value)
		free(oldpwd->value);
	oldpwd->value = pwd->value;
	pwd->value = getcwd(NULL, 0);
}

static char	*get_env_value(t_list *lst)
{
	t_env	*env;

	env = get_env(lst);
	if (env)
		return (env->value);
	return (NULL);
}
