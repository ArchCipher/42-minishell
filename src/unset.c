/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:45:22 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 21:41:22 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	del_one_env(t_shell *shell, t_env *env, t_env *prev);

/*
DESCRIPTION:
	Unsets the environment variables.
	Returns 0 on success, 1 if the variable is not a valid identifier.
*/

int	exec_unset(char **args, t_shell *shell)
{
	t_env		*prev;
	t_env		*env;
	char		*cmd;

	cmd = *(args++);
	if (!*args)
		return (0);
	while (*args)
	{
		if (!is_valid_identifier(*args))
			return (perr_msg(cmd, *args, E_EXPORT, true), 1);
		if (!strcmp(*args, "PWD"))
			shell->pwd = NULL;
		else if (!strcmp(*args, "OLDPWD"))
			shell->oldpwd = NULL;
		else if (!strcmp(*args, "HOME"))
			shell->home = NULL;
		env = env_lookup_prev(shell->env, &prev, *args);
		if (env)
			del_one_env(shell, env, prev);
		args++;
	}
	return (0);
}

/*
DESCRIPTION:
	Deletes one environment variable from the list.
	Updates the head and tail pointers of the list if necessary.
*/

static void	del_one_env(t_shell *shell, t_env *env, t_env *prev)
{
	if (prev)
		prev->next = env->next;
	else
		shell->env = env->next;
	if (env == shell->env_tail)
		shell->env_tail = prev;
	free(env->key);
	free(env->value);
	free(env);
}
