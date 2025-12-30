/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:45:22 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:51:29 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	del_one_env(t_shell *shell, t_env *env, t_env *prev);

/*
unset	0
*/

int	exec_unset(t_shell *shell, char **args)
{
	t_env	*prev;
	t_env	*env;

	if (!*args)
		return (0);
	while (*args)
	{
		if (!is_valid_identifier(*args))
			return (ft_dprintf(STDERR_FILENO, "%s: unset: `%s': %s\n", MINI,
					*args, E_EXPORT), 1);
		env = env_lookup_prev(shell->env, &prev, *args);
		if (env)
			del_one_env(shell, env, prev);
		args++;
	}
	return (0);
}

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

t_env	*env_lookup_prev(t_env *env, t_env **prev, const char *arg)
{
	char	*equal;
	size_t	key_len;

	*prev = NULL;
	key_len = ft_strlen(arg);
	equal = ft_memchr(arg, '=', key_len);
	if (equal)
		key_len = equal - arg;
	while (env)
	{
		if (ft_strlen(env->key) == key_len && !ft_strncmp(env->key, arg,
				key_len))
			return (env);
		*prev = env;
		env = env->next;
	}
	return (NULL);
}
