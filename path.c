#include "minishell.h"

static char *get_path(const char *filename, t_env *env);
static char *build_path(const char *filename, char *full_path);
static void	child_path_error(const char *filename, char *msg, int exit_code,
				char *path);

/*
0:  success
1:  general error
126:    found but not executable (insufficient permissions)
127:    command not found

normal exit codes: 0-127
128 + N:    fatal error signal N (ex: 137 is 128 + 9 for SIGKILL)
255:    exit status out of range (exit takes only integer args in the range 0
	- 255)
non numeric argument for exit also return 255

If buils_path returns NULL path, or if stat(path) fails,
	it exits with code 127(command not found)
It checks S_ISREG(sb.st_mode) to check if it is a directory.
It checks access(path, X_OK) for execute permissions.

An exit status of 126 indicates that utility was found,
	but could not be executed.
An exit status of 127 indicates that utility could not be found.
*/

char  *get_valid_path(const char *filename, t_env *env)
{
	struct stat	sb;
	char        *path;

	if (ft_strchr(filename, '/'))
		path = (char *)filename;
	else
        path = get_path(filename, env);
	if (!path)
		child_path_error(filename, E_PATH, EXIT_CMD_NOT_FOUND, path);
	if (stat(path, &sb) == -1)
		child_path_error(filename, strerror(errno), EXIT_CMD_NOT_FOUND, path);
	if (!S_ISREG(sb.st_mode))
		child_path_error(filename, E_DIR, EXIT_CANNOT_EXEC, path);
	if (!(access(path, X_OK) == 0))
		child_path_error(filename, strerror(errno), EXIT_CANNOT_EXEC, path);
	return (path);
}

/*
DESCRIPTION
    Retrieves $PATH using ft_getenv() (similar to getenv()), duplicates it
    and returns fully built_path
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
    {
        perror(MINI);
        exit(1);
    }
    return (build_path(filename, full_path));
}

/*
DESCRIPTION
	splits full_path [$(PATH)] by : using ft_strtok_r.
	For each directory in $PATH, builds dir + "/" + cmd.

	When path is empty "", it means current directory->
        create full string with "./" such as "./cmd"

	This is child only logic and doesn't return invalid path. It exits on error.
	Caution: !!!cannot be used in parent process!!!
*/
static char	*build_path(const char *filename, char *full_path)
{
	t_path_vars	vars;
	struct stat	sb;

	vars.path = ft_strtok_r(full_path, ":", &vars.p);
	while (vars.path)
	{
		if (!*(vars.path))
			vars.new_path = ft_strjoin("./", filename);
		else
			vars.new_path = ft_strjoin3(vars.path, "/", filename);
		if (!vars.new_path)
		{
			free(full_path);
			perror(MINI);
			exit(1);
		}
		if (stat(vars.new_path, &sb) == 0)
			break ;
		free(vars.new_path);
		vars.new_path = NULL;
		vars.path = ft_strtok_r(NULL, ":", &vars.p);
	}
	free(full_path);
	return (vars.new_path);
}

static void	child_path_error(const char *filename, char *msg, int exit_code,
		char *path)
{
	ft_dprintf(STDERR_FILENO, "%s: %s: %s\n", MINI, filename, msg);
	if (!ft_strchr(filename, '/') && path)
		free(path);
	exit(exit_code);
}
