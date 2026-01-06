/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 14:10:19 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 21:42:40 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_path(const char *filename, t_env *env);
static char	*build_path(const char *filename, char *full_path);
static void	child_path_error(const char *filename, char *msg, int exit_code,
				char *path);

/*
DESCRIPTION:
	Gets the valid path for the filename and returns it.
	Returns the valid path on success, exits on errors.
	((char *)-1) is used as a sentinel value for malloc failure.

NOTE:
	Child only logic and should not be used in parent process.
*/

char	*get_valid_path(const char *filename, t_env *env)
{
	struct stat	sb;
	char		*path;

	if (ft_strchr(filename, '/'))
		path = (char *)filename;
	else
		path = get_path(filename, env);
	if (path == ((char *)-1))
	{
		perror(MINI);
		exit(EXIT_FAILURE);
	}
	if (!path)
		child_path_error(filename, E_CMD, EXIT_CMD_NOT_FOUND, path);
	if (*filename == '.' && !filename[1])
		child_path_error(filename, E_ARG, EXIT_INVAL_OPTION, path);
	if (stat(path, &sb) == -1)
		child_path_error(filename, strerror(errno), EXIT_CMD_NOT_FOUND, path);
	if (!S_ISREG(sb.st_mode))
		child_path_error(filename, E_DIR, EXIT_CANNOT_EXEC, path);
	if (access(path, X_OK))
		child_path_error(filename, strerror(errno), EXIT_CANNOT_EXEC, path);
	return (path);
}

/*
DESCRIPTION:
	Retrieves $PATH using ft_getenv(), and returns fully built path.
	Returns fully built path on success, NULL if $PATH is not set or is empty,
	((char *)-1) on malloc() failure.
*/

static char	*get_path(const char *filename, t_env *env)
{
	const char	*path;
	char		*full_path;

	path = ft_getenv(env, "PATH");
	if (!path)
		return (NULL);
	full_path = strdup(path);
	if (!full_path)
		return (((char *)-1));
	return (build_path(filename, full_path));
}

/*
DESCRIPTION:
	Builds the path for the filename using $PATH variable.
	Splits full_path [$(PATH)] by : using ft_strtok_r and checks its validity.
	For each directory in $PATH, it builds the path "dir/filename".
	When path is empty string, it means current directory: "./filename".
	Returns the built path on success, NULL if no path is found, ((char *)-1) on
	malloc() failure.
*/
static char	*build_path(const char *filename, char *full_path)
{
	t_strtok	path;
	struct stat	sb;

	path.token = ft_strtok_r(full_path, ":", &path.p);
	while (path.token)
	{
		if (!*(path.token))
			path.str = ft_strjoin("./", filename);
		else
			path.str = ft_strjoin3(path.token, "/", filename);
		if (!path.str)
			return (free(full_path), ((char *)-1));
		if (stat(path.str, &sb) == 0)
			break ;
		free(path.str);
		path.str = NULL;
		path.token = ft_strtok_r(NULL, ":", &path.p);
	}
	free(full_path);
	return (path.str);
}

/*
DESCRIPTION:
	Prints the error message and exits with the appropriate exit code.
*/

static void	child_path_error(const char *filename, char *msg, int exit_code,
		char *path)
{
	perr_msg(filename, msg, NULL, false);
	if (!ft_strchr(filename, '/') && path)
		free(path);
	exit(exit_code);
}
