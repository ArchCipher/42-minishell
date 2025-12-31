#include "minishell.h"

static char *get_path(const char *filename, t_env *env);
static char *build_path(const char *filename, char *full_path);
static void	child_path_error(const char *filename, char *msg, int exit_code,
				char *path);

/*
DESCRIPTION:
	Gets the valid path for the filename and returns it.
	Returns the valid path on success, exits on errors.
	ERR_PTR is used as a sentinal value for malloc failure.

NOTE:
	Child only logic and should not be used in parent process.
*/

char  *get_valid_path(const char *filename, t_env *env)
{
	struct stat	sb;
	char        *path;

	if (ft_strchr(filename, '/'))
		path = (char *)filename;
	else
        path = get_path(filename, env);
	if (path == ERR_PTR)
	{
		perror(MINI);
		exit(EXIT_FAILURE);
	}
	if (!path)
		child_path_error(filename, E_PATH, EXIT_CMD_NOT_FOUND, path);
	if (stat(path, &sb) == -1)
		child_path_error(filename, strerror(errno), EXIT_CMD_NOT_FOUND, path);
	if (!S_ISREG(sb.st_mode))
		child_path_error(filename, E_DIR, EXIT_CANNOT_EXEC, path);
	if (access(path, X_OK))
		child_path_error(filename, strerror(errno), EXIT_CANNOT_EXEC, path);
	return (path);
}

/*
DESCRIPTION
    Retrieves $PATH using ft_getenv(), and returns fully built path.
	Returns fully built path on success, NULL if $PATH is not set or is empty,
	ERR_PTR on malloc() failure.
*/
static char    *get_path(const char *filename, t_env *env)
{
    const char  *path;
    char        *full_path;

    path = ft_getenv(env, "PATH");
	if (!path)
        return (NULL);
	full_path = strdup(path);
    if (!full_path)
		return (ERR_PTR);
    return (build_path(filename, full_path));
}

/*
DESCRIPTION:
	Builds the path for the filename using $PATH variable.
	Splits full_path [$(PATH)] by : using ft_strtok_r and checks its validity.
	For each directory in $PATH, it builds the path "dir/filename".
	When path is empty string, it means current directory: "./filename".
	Returns the built path on success, NULL if no path is found, ERR_PTR on
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
			return (free(full_path), ERR_PTR);
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
	ft_dprintf(STDERR_FILENO, "%s: %s: %s\n", MINI, filename, msg);
	if (!ft_strchr(filename, '/') && path)
		free(path);
	exit(exit_code);
}
