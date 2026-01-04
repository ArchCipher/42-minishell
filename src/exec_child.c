/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:41 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:48:25 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int		setup_child_fds(int *fd, int prev_fd);
static void		exec_child(t_cmd *cmd, int *fd, t_shell *shell);
static char		**env_to_envp(t_env *env);
static size_t	envp_size(t_env *env);

/*
DESCRIPTION:
	Forks a child process. It also creates a pipe if there is a next command.
	If pipe() and fork() succeeds, it executes the child process, closes fds
	appropriately and saves the read_end of the pipe if there is a next command.
	Returns 0 on success and 1 on error.

	pipe(): Creates a pipe with 2 fds as read and write ends.
		fd[0] is the read end and fd[1] is the write end.
	fork(): On success, a child process is created. It returns 0 to the child
		process and the process ID of the child to the parent process.
	On error, both pipe and fork return -1.
*/

int	fork_with_pipe(t_cmd *cmd, t_shell *shell)
{
	int	fd[2];
	int	*pipe_fd;

	pipe_fd = NULL;
	if (cmd->next && cmd->next->con == PIPE_CHAR && pipe(fd) == -1)
		return (perror(MINI), 1);
	if (cmd->next && cmd->next->con == PIPE_CHAR)
		pipe_fd = fd;
	cmd->exec.pid = fork();
	if (cmd->exec.pid == -1)
		return (close_fds_error(pipe_fd, cmd->exec.p_fd));
	if (cmd->exec.pid == 0)
		exec_child(cmd, pipe_fd, shell);
	if (cmd->exec.p_fd != -1)
		close(cmd->exec.p_fd);
	if (!pipe_fd)
		return (0);
	close(pipe_fd[1]);
	cmd->next->exec.p_fd = pipe_fd[0];
	return (0);
}

/*
DESCRIPTION:
	It executes the command by setting up SIGINT and SIGQUIT signals to SIG_DFL,
	duplicating the required fds, closing all fds, setting up redirections
	and building path if necessary.
	If execve() returns, it prints error and exits.
	Exits with 0 on success and non-zero on failure.
	As it is a child process, it should not return.

	execve(): Executes the command. When a program is executed as a result of
	an execve() call, it is entered as follows:
		int main(int argc, char **argv, char **envp);
	As the execve() function overlays the current process image with a new
    process image, the successful call has no process to return to.
	If execve() does return to the calling process, an error has occurred.
	The return value will be -1 and errno is set to indicate the error.
*/

static int	setup_child_fds(int *fd, int prev_fd)
{
	if (set_signal_handlers(SIG_DFL, SIG_DFL))
		return (1);
	if (fd && (dup2(fd[1], STDOUT_FILENO) == -1))
		return (1);
	if (prev_fd != -1 && (dup2(prev_fd, STDIN_FILENO) == -1))
		return (1);
	close_pipe_fds(fd, prev_fd);
	return (0);
}

static void	exec_child(t_cmd *cmd, int *fd, t_shell *shell)
{
	char		*path;
	char		**envp;

	if (setup_child_fds(fd, cmd->exec.p_fd))
		exit (close_fds_error(fd, cmd->exec.p_fd));
	if (setup_redirs(cmd->redirs))
		exit(EXIT_FAILURE);
	if (cmd->sub)
		exit(exec_cmds(cmd->sub, shell));
	if (!cmd->args || !cmd->args[0])
		exit (EXIT_SUCCESS);
	if (cmd->exec.builtin != -1)
		exit(exec_builtin(cmd, shell));
	path = get_valid_path(cmd->args[0], shell->env);
	envp = env_to_envp(shell->env);
	if (!envp)
		exit(EXIT_FAILURE);
	execve(path, cmd->args, envp);
	perror(MINI);
	if (!ft_strchr(cmd->args[0], '/'))
		free(path);
	free(envp);
	exit(EXIT_FAILURE);
}

/*
DESCRIPTION:
	Converts the environment list to a null-terminated array of strings.
	Returns the array of strings on success, NULL on failure.
*/
static char	**env_to_envp(t_env *env)
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
				return (perror(MINI), free_arr(envp), NULL);
			i++;
		}
		env = env->next;
	}
	envp[i] = NULL;
	return (envp);
}

/*
DESCRIPTION:
	Calculates the number of exported environment variables.
	Returns the number of exported environment variables.
*/

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
