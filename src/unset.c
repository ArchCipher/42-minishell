/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:45:22 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 14:42:10 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	del_one_env(t_shell *shell, t_list *env, t_list *prev);

/*
DESCRIPTION:
	Unsets the environment variables.
	Returns 0 on success, 1 if the variable is not a valid identifier.
*/

int	exec_unset(char **args, t_shell *shell)
{
	t_list	*prev;
	t_list	*env;
	char	*cmd;

	cmd = *(args++);
	if (!*args)
		return (0);
	if (**args == '-' && (*args)[1])
		return (perr_msg(cmd, *args, E_OPTION, true), 2);
	while (*args)
	{
		if (!is_valid_identifier(*args) || ft_strchr(*args, '='))
			return (perr_msg(cmd, *args, E_EXPORT, true), 1);
		if (!ft_strcmp(*args, "PWD"))
			shell->pwd = NULL;
		else if (!ft_strcmp(*args, "OLDPWD"))
			shell->oldpwd = NULL;
		else if (!ft_strcmp(*args, "HOME"))
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

static void	del_one_env(t_shell *shell, t_list *env, t_list *prev)
{
	if (!shell->env || !env)
		return ;
	if (!prev)
		shell->env = env->next;
	else
		prev->next = env->next;
	if (env == shell->env_last)
		shell->env_last = prev;
	ft_lstdelone(env, free_env);
}
