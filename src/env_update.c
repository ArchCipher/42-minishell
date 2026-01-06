/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_update.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:39 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 14:41:04 by kmurugan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_env	*create_env(const char *envp);

/*
DESCRIPTION:
	Checks if the key is a valid identifier and returns a pointer to the '='
	character, or NULL if invalid or no assignment found.
	Returns pointer to '=' for 'KEY=value' or 'KEY+=value', NULL otherwise.

NOTE:
	KEY must start with an alphabet or underscore and can only contain
	alphabets, numbers and underscores.
*/

const char	*is_valid_identifier(const char *s)
{
	if (!*s || (*s != '_' && !ft_isalpha(*s)))
		return (NULL);
	s++;
	while (*s && (*s != '=' && *s != '+'))
	{
		if (*s != '_' && !ft_isalnum(*s))
			return (NULL);
		s++;
	}
	if (*s == '+')
		s++;
	if (*s != '=')
		return (NULL);
	return (s);
}

/*
DESCRIPTION:
	Creates a new environment variable if t_env *env is NULL, otherwise updates
	the value of the existing environment variable.
	Returns 0 on success, 1 on failure.
*/

int	update_env(t_shell *shell, const char *arg, t_env *env)
{
	const char	*equal;
	char		*value;

	if (!env)
	{
		env = create_env(arg);
		if (!env)
			return (1);
		lstadd_back((void **)&shell->env, env, shell->env_tail, TYPE_ENV);
		shell->env_tail = env;
		return (0);
	}
	equal = is_valid_identifier(arg);
	if (!equal)
		return (1);
	if (equal > arg && *(equal - 1) == '+' && env->value)
		value = ft_strjoin(env->value, equal + 1);
	else
		value = ft_strdup(equal + 1);
	if (!value)
		return (free(env->value), 1);
	free(env->value);
	env->value = value;
	env->exported = true;
	return (0);
}

char	*create_key(const char *envp, const char *equal)
{
	if (!equal)
		return (ft_strdup(envp));
	if (equal > envp && *(equal - 1) == '+')
		return (ft_strndup(envp, equal - envp - 1));
	return (ft_strndup(envp, equal - envp));
}

char	*create_value(const char *equal)
{
	if (!equal)
		return (NULL);
	return (ft_strdup(equal + 1));
}

/*
DESCRIPTION:
	Creates a new environment variable from the environment variable string.
	Splits the environment variable string into key and value if '=' is present.
	Returns the new environment variable if created, NULL otherwise.
*/

static t_env	*create_env(const char *envp)
{
	t_env		*new;
	const char	*equal;

	new = malloc(sizeof(t_env));
	if (!new)
		return (NULL);
	new->exported = false;
	new->next = NULL;
	equal = is_valid_identifier(envp);
	new->key = create_key(envp, equal);
	if (!new->key)
		return (free(new), NULL);
	new->value = create_value(equal);
	if (equal && !new->value)
		return (free(new->key), free(new), NULL);
	new->exported = true;
	return (new);
}

