/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_lookup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:39 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 14:41:04 by kmurugan          ###   ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	const char	*equal;
	size_t		key_len;

	*prev = NULL;
	key_len = ft_strlen(arg);
	equal = is_valid_identifier(arg);
	if (equal && !*equal)
		equal = NULL;
	if (equal && equal > arg && *(equal - 1) == '+')
		key_len = equal - arg - 1;
	else if (equal)
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
