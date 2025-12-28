/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:39 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 19:52:52 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	envp_size(t_env *env);

/*
env FORMAT:
	KEY=value

print only exported values

Returns 0
*/

int	exec_env(t_env *env)
{
	while (env)
	{
		if (env->exported && env->value)
			printf("%s=%s\n", env->key, env->value);
		env = env->next;
	}
	return (0);
}

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
need to free all exported ones.
*/
char	**env_to_envp(t_env *env)
{
	char	**envp;
	size_t	size;
	size_t	i;

	size = envp_size(env);
	envp = malloc(sizeof(char *) * (size + 1));
	if (!envp)
		return (perror(MINI), NULL);
	i = 0;
	while (env)
	{
		if (env->exported && env->value)
		{
			envp[i] = ft_strjoin3(env->key, "=", env->value);
			if (!envp[i])
				return (perror(MINI), envp[i] = NULL, free_envp(envp), NULL);
			i++;
		}
		env = env->next;
	}
	envp[i] = NULL;
	return (envp);
}

static size_t	envp_size(t_env *env)
{
	size_t	i;

	i = 0;
	while (env)
	{
		if (env->exported && env->value)
			i++;
		env = env->next;
	}
	return (i);
}
