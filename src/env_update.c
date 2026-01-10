/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_update.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:39 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 14:41:04 by kmurugan          ###   ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_list	*create_env(const char *envp);
static int		update_value(const char *equal, const char *arg, t_env *env);
static int		get_key_value(const char *envp, t_env *new);

/*
DESCRIPTION:
	Creates a new environment variable if t_env *env is NULL, otherwise updates
	the value of the existing environment variable.
	Returns 0 on success, 1 on failure.
*/

int	update_env(t_shell *shell, const char *arg, t_list *envs)
{
	t_list		*new;
	const char	*equal;

	if (!envs)
	{
		new = create_env(arg);
		if (!new)
			return (1);
		ft_lstadd_back(&shell->env, new, &shell->env_last);
		return (0);
	}
	equal = is_valid_identifier(arg);
	if (!equal)
		return (1);
	if (*equal)
		return (update_value(equal, arg, get_env(envs)));
	get_env(envs)->exported = true;
	return (0);
}

/*
DESCRIPTION:
	Checks if the key is a valid identifier and returns a pointer to the '='
	character, or NULL if invalid or no assignment found.
	Returns pointer to '=' for 'KEY=value' or 'KEY+=value', pointer to null
	terminator if valid identifier without '='.

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
	if (!*s)
		return (s);
	if (*s == '+')
		s++;
	if (*s != '=')
		return (NULL);
	return (s);
}

/*
DESCRIPTION
	Updates or appends the value of environemnt variable.
*/
static int	update_value(const char *equal, const char *arg, t_env *env)
{
	char	*value;

	if (!equal || !arg || !env)
		return (0);
	if (equal > arg && *(equal - 1) == '+' && env->value)
		value = ft_strjoin(env->value, equal + 1);
	else
		value = ft_strdup(equal + 1);
	if (!value)
		return (1);
	free(env->value);
	env->value = value;
	env->exported = true;
	return (0);
}

/*
DESCRIPTION:
	Creates a new environment variable from the environment variable string.
	Splits the environment variable string into key and value if '=' is present.
	Returns the new environment variable if created, NULL otherwise.
*/

static t_list	*create_env(const char *envp)
{
	t_list		*new;
	t_env		*env;

	new = ft_lstnew(sizeof(t_env));
	if (!new)
		return (perror(MINI), NULL);
	env = get_env(new);
	env->exported = false;
	env->key = NULL;
	env->value = NULL;
	if (get_key_value(envp, env))
		return (ft_lstdelone(new, free_env), NULL);
	env->exported = true;
	return (new);
}

static int	get_key_value(const char *envp, t_env *new)
{
	const char	*equal;

	equal = is_valid_identifier(envp);
	if (equal && !*equal)
		equal = NULL;
	if (!equal)
		new->key = ft_strdup(envp);
	else if (equal > envp && *(equal - 1) == '+')
		new->key = ft_strndup(envp, equal - envp - 1);
	else
		new->key = ft_strndup(envp, equal - envp);
	if (!new->key)
		return (1);
	if (equal && *equal)
	{
		new->value = ft_strdup(equal + 1);
		if (!new->value)
			return (free(new->key), 1);
	}
	return (0);
}
