/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_no_args.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:50 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 19:52:03 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int		comp(const void *a, const void *b);
static t_env	**env_to_array(t_env *env, size_t len);
static size_t	env_size(t_env *env);

int	export_no_args(t_env *env)
{
	t_env	**arr;
	size_t	len;
	size_t	i;

	len = env_size(env);
	if (len == 0)
		return (0);
	arr = env_to_array(env, len);
	if (!arr)
		return (1);
	ft_qsort(arr, len, sizeof(t_env *), &comp);
	i = 0;
	while (i < len)
	{
		if (arr[i]->exported)
			printf("declare -x %s=\"%s\"\n", arr[i]->key, arr[i]->value);
		else
			printf("declare -x %s\n", arr[i]->key);
		i++;
	}
	free(arr);
	return (0);
}

/*
comparator recieves a pointer to the element, not the element itself.
*/
static int	comp(const void *a, const void *b)
{
	return (ft_strcmp((*(const t_env **)a)->key, (*(const t_env **)b)->key));
}

static t_env	**env_to_array(t_env *env, size_t len)
{
	t_env	**arr;
	size_t	i;

	arr = malloc(sizeof(t_env *) * len);
	if (!arr)
		return (perror(MINI), NULL);
	i = 0;
	while (i < len)
	{
		arr[i++] = env;
		env = env->next;
	}
	return (arr);
}

static size_t	env_size(t_env *env)
{
	size_t	i;

	i = 0;
	while (env)
	{
		i++;
		env = env->next;
	}
	return (i);
}
