/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:39 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 14:41:04 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_env	*create_env(const char *envp);

/*
DESCRIPTION:
	Retrieves the value of the environment variable.
	Returns the value of the environment variable if found, NULL otherwise.
*/

const char	*ft_getenv(t_env *env, const char *key)
{
	while (env)
	{
		if (!ft_strcmp(env->key, key))
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

/*
DESCRIPTION:
	Wrapper function to lookup the environment variable in the list.
	Returns the environment variable if found, NULL otherwise.
*/

t_env	*env_lookup(t_env *env, const char *arg)
{
	t_env	*prev;

	return (env_lookup_prev(env, &prev, arg));
}

/*
DESCRIPTION:
	Looks up the environment variable in the list and updates the previous
	pointer. Returns the environment variable if found, NULL otherwise.
*/

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

/*
DESCRIPTION:
	Creates a new environment variable if t_env *env is NULL, otherwise updates
	the value of the existing environment variable.
	Returns 0 on success, 1 on failure.
*/

int	update_env(t_shell *shell, const char *arg, t_env *env)
{
	char	*equal;

	if (!env)
	{
		env = create_env(arg);
		if (!env)
			return (1);
		lstadd_back((void **)&shell->env, env, shell->env_tail, TYPE_ENV);
		shell->env_tail = env;
		return (0);
	}
	equal = ft_memchr(arg, '=', ft_strlen(arg));
	if (equal)
	{
		if (env->value)
			free(env->value);
		env->value = ft_strdup(equal + 1);
		if (!env->value)
			return (1);
	}
	env->exported = true;
	return (0);
}

/*
DESCRIPTION:
	Creates a new environment variable from the environment variable string.
	Splits the environment variable string into key and value if '=' is present.
	Returns the new environment variable if created, NULL otherwise.
*/

static t_env	*create_env(const char *envp)
{
	t_env	*new;
	char	*equal;

	new = malloc(sizeof(t_env));
	if (!new)
		return (NULL);
	new->value = NULL;
	new->exported = false;
	new->next = NULL;
	equal = ft_memchr(envp, '=', ft_strlen(envp));
	if (equal)
		new->key = ft_strndup(envp, equal - envp);
	else
		new->key = ft_strdup(envp);
	if (!new->key)
		return (free(new), NULL);
	if (equal)
	{
		new->value = ft_strdup(equal + 1);
		if (!new->value)
			return (free(new->key), free(new), NULL);
		new->exported = true;
	}
	return (new);
}
