#include "minishell.h"

static int	do_cd(char **args, t_shell *shell);

/*
DESCRIPTION:
	Executes the cd builtin command using chdir().
	Updates the PWD and OLDPWD environment variables if PWD and OLDPWD exist.
	Returns 0 on success, 1 if chdir() or getcwd() fails.
*/

#if __LINUX__

int	exec_cd(char **args, t_shell *shell)
{
	if (args[1] && args[2])
		return (perr_msg(*args, E_MANY_ARGS, NULL, false), 1);
	return (do_cd(args, shell));
}
#else

int	exec_cd(char **args, t_shell *shell)
{
	return (do_cd(args, shell));
}
#endif

static int	do_cd(char **args, t_shell *shell)
{
	const char	*dir;
	char		*tmp;

	tmp = NULL;
	dir = args[1];
	if ((!dir || (*dir == '~' && !dir[1])) && (!shell->home
			|| !shell->home->value))
		return (perr_msg(*args, E_HOME, NULL, false), 1);
	else if (!dir || (*dir == '~' && !dir[1]))
		dir = shell->home->value;
	else if (*dir == '~' && dir[1] == '/')
	{
		tmp = ft_strjoin(shell->home->value, dir + 1);
		dir = tmp;
	}
	if (chdir(dir) == -1)
		return (free(tmp), perr_msg(*args, args[1], strerror(errno), false), 1);
	free(tmp);
	update_pwds(shell);
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
	if (!shell->pwd)
	{
		if (update_env(shell, "PWD", NULL))
			return ;
		shell->pwd = env_lookup(shell->env, "PWD");
	}
	if (!shell->oldpwd)
	{
		if (update_env(shell, "OLDPWD", NULL))
			return ;
		shell->oldpwd = env_lookup(shell->env, "OLDPWD");
	}
	if (shell->oldpwd->value)
		free(shell->oldpwd->value);
	shell->oldpwd->value = shell->pwd->value;
	shell->pwd->value = getcwd(NULL, 0);
}
