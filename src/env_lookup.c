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

const char	*ft_getenv(t_list *envs, const char *key)
{
	t_env	*env;

	while (envs && envs->content)
	{
		env = get_env(envs);
		if (!ft_strcmp(env->key, key))
			return (env->value);
		envs = envs->next;
	}
	return (NULL);
}

/*
DESCRIPTION:
	Wrapper function to lookup the environment variable in the list.
	Returns the environment variable if found, NULL otherwise.
*/

t_list	*env_lookup(t_list *envs, const char *arg)
{
	t_list	*prev;

	return (env_lookup_prev(envs, &prev, arg));
}

/*
DESCRIPTION:
	Looks up the environment variable in the list and updates the previous
	pointer. Returns the environment variable if found, NULL otherwise.
*/

t_list	*env_lookup_prev(t_list *envs, t_list **prev, const char *arg)
{
	const char	*equal;
	size_t		key_len;

	if (!envs || !envs->content || !arg)
		return (NULL);
	*prev = NULL;
	key_len = ft_strlen(arg);
	equal = is_valid_identifier(arg);
	if (equal && !*equal)
		equal = NULL;
	if (equal && equal > arg && *(equal - 1) == '+')
		key_len = equal - arg - 1;
	else if (equal)
		key_len = equal - arg;
	while (envs && envs->content)
	{
		if (ft_strlen(get_env(envs)->key) == key_len
			&& !ft_strncmp(get_env(envs)->key, arg, key_len))
			return (envs);
		*prev = envs;
		envs = envs->next;
	}
	return (NULL);
}
