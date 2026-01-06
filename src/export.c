/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:52 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 14:41:18 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int		print_exported_vars(t_env *env);
static t_env	**env_list_to_array(t_env *env, size_t len);
static size_t	count_exported_vars(t_env *env);

/*
DESCRIPTION:
	Export with no args prints the exported environment variables in
	alphabetical order. Export with args updates or creates the specific
	environment variable. It prints error if the key is not a valid identifier.
	Returns 0 on success, 1 on error.
*/

int	exec_export(char **args, t_shell *shell)
{
	char	*cmd;

	cmd = *(args++);
	if (!*args)
		return (print_exported_vars(shell->env));
	if (**args == '-' && (*args)[1])
		return (perr_tok_msg(cmd, *args, 2, E_OPTION), 2);
	while (*args)
	{
		if (!is_valid_identifier(*args))
			return (perr_msg(cmd, *args, E_EXPORT, true), 1);
		if (update_env(shell, *args, env_lookup(shell->env, *args)))
			return (perror(MINI), 1);
		if (!shell->pwd && !strncmp(*args, "PWD", 3) && (*args)[3] == '=')
			shell->pwd = env_lookup(shell->env, *args);
		else if (!shell->oldpwd && !strncmp(*args, "OLDPWD", 6)
			&& (*args)[6] == '=')
			shell->oldpwd = env_lookup(shell->env, *args);
		else if (!shell->home && !strncmp(*args, "HOME", 4)
			&& (*args)[4] == '=')
			shell->home = env_lookup(shell->env, *args);
		args++;
	}
	return (0);
}

/*
DESCRIPTION:
	Prints the exported environment variables in alphabetical order.
	Returns 0 on success, 1 on error.

FORMAT:
	declare -x KEY="value"
	declare -x KEY
*/

static int	print_exported_vars(t_env *env)
{
	t_env	**arr;
	size_t	len;
	size_t	i;
	int		ret;

	len = count_exported_vars(env);
	if (!len)
		return (0);
	arr = env_list_to_array(env, len);
	if (!arr)
		return (perror(MINI), 1);
	ft_qsort_env(arr, 0, len - 1);
	ret = 0;
	i = 0;
	while (i < len)
	{
		if (arr[i]->exported && arr[i]->value && ft_dprintf(STDOUT_FILENO,
				"declare -x %s=\"%s\"\n", arr[i]->key, arr[i]->value) < 0)
			ret = 1;
		else if (arr[i]->exported && !arr[i]->value && ft_dprintf(STDOUT_FILENO,
				"declare -x %s\n", arr[i]->key) < 0)
			ret = 1;
		i++;
	}
	return (free(arr), ret);
}

/*
DESCRIPTION:
	Converts the environment list to an array of pointers to t_env.
	Returns the array of pointers on success, NULL on failure.
*/

static t_env	**env_list_to_array(t_env *env, size_t len)
{
	t_env	**arr;
	size_t	i;

	arr = malloc(sizeof(t_env *) * len);
	if (!arr)
		return (NULL);
	i = 0;
	while (i < len)
	{
		if (env->exported)
			arr[i++] = env;
		env = env->next;
	}
	return (arr);
}

/*
DESCRIPTION:
	Calculates and returns the number of environment variables.
*/
static size_t	count_exported_vars(t_env *env)
{
	size_t	i;

	i = 0;
	while (env)
	{
		if (env->exported)
			i++;
		env = env->next;
	}
	return (i);
}

/*
DESCRIPTION:
	Checks if the key is a valid identifier.
	Returns 0 if not a valid identifier, 1 otherwise.

NOTE:
	KEY must start with an alphabet or underscore and can only contain
	alphabets, numbers and underscores.
*/

int	is_valid_identifier(const char *s)
{
	if (!*s)
		return (0);
	if (*s != '_' && !ft_isalpha(*s))
		return (0);
	s++;
	while (*s && *s != '=')
	{
		if (*s != '_' && !ft_isalnum(*s))
			return (0);
		s++;
	}
	return (1);
}
