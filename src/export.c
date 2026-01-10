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

static int		print_exported_vars(t_list *envs);
static void		update_shell_env(char *arg, t_shell *shell);
static t_env	**env_list_to_array(t_list *envs, size_t len);
static size_t	count_exported_vars(t_list *envs);

/*
DESCRIPTION:
	Export with no args prints the exported environment variables in
	alphabetical order. Export with args updates or creates the specific
	environment variable. It prints error if the key is not a valid identifier.
	Returns 0 on success, 1 on error.
*/

int	exec_export(char **args, t_shell *shell)
{
	char		*cmd;
	const char	*equal;

	cmd = *(args++);
	if (!*args)
		return (print_exported_vars(shell->env));
	if (**args == '-' && (*args)[1])
		return (perr_tok_msg(cmd, *args, 2, E_OPTION), 2);
	while (*args)
	{
		equal = is_valid_identifier(*args);
		if (!equal)
			return (perr_msg(cmd, *args, E_EXPORT, true), 1);
		if (update_env(shell, *args, env_lookup(shell->env, *args)))
			return (perror(MINI), 1);
		update_shell_env(*args, shell);
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

static int	print_exported_vars(t_list *envs)
{
	t_env	**arr;
	size_t	len;
	size_t	i;
	int		ret;

	len = count_exported_vars(envs);
	if (!len)
		return (0);
	arr = env_list_to_array(envs, len);
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

static void	update_shell_env(char *arg, t_shell *shell)
{
	if (!shell->pwd && !ft_strncmp(arg, "PWD", 3) && (!arg[3] || arg[3] == '='
			|| (arg[3] == '+' && arg[4] == '=')))
		shell->pwd = env_lookup(shell->env, arg);
	else if (!shell->oldpwd && !ft_strncmp(arg, "OLDPWD", 6) && (!arg[6]
			|| arg[6] == '=' || (arg[6] == '+' && arg[7] == '=')))
		shell->oldpwd = env_lookup(shell->env, arg);
	else if (!shell->home && !ft_strncmp(arg, "HOME", 4) && (!arg[4]
			|| arg[4] == '=' || (arg[4] == '+' && arg[5] == '=')))
		shell->home = env_lookup(shell->env, arg);
}

/*
DESCRIPTION:
	Converts the environment list to an array of pointers to t_env.
	Returns the array of pointers on success, NULL on failure.
*/

static t_env	**env_list_to_array(t_list *envs, size_t len)
{
	t_env	*env;
	t_env	**arr;
	size_t	i;

	arr = malloc(sizeof(t_env *) * len);
	if (!arr)
		return (NULL);
	i = 0;
	while (i < len)
	{
		env = get_env(envs);
		if (env->exported)
			arr[i++] = env;
		envs = envs->next;
	}
	return (arr);
}

/*
DESCRIPTION:
	Calculates and returns the number of environment variables.
*/
static size_t	count_exported_vars(t_list *envs)
{
	size_t	i;

	i = 0;
	while (envs && envs->content)
	{
		if (get_env(envs)->exported)
			i++;
		envs = envs->next;
	}
	return (i);
}
