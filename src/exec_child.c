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

static void		exec_child(t_cmd *cmd, int *fd, int prev_fd, t_shell *shell);
static char		**env_to_envp(t_env *env);
static size_t	envp_size(t_env *env);
static void		child_dup_error(int *fd, int prev_fd);

/*
DESCRIPTION:
	If forks a child preocess, and if there is a next command,
		it creates a pipe and then forks.
	If fork succeeds, it executes the child,
		closes fds appropriately and stores the
	read_end of pipe for next command if there is a next command.
	Returns 0 on success and 1 on pipe or fork error.

	PIPE: a pipe with 2 fds with read and write ends are created.
		fd[0] connects to read end, fd[1] to write end.
	FORK: On success,
		a child process is created. It returns 0 to the child process and
		the process ID of the child to the parent process.
	On error, both pipe and fork return -1.
*/

int	fork_with_pipe(t_cmd *cmd, int *prev_fd, t_shell *shell)
{
	int	fd[2];
	int	*pipe_fd;

	pipe_fd = NULL;
	if (cmd->next && pipe(fd) == -1)
		return (perror(MINI), 1);
	if (cmd->next)
		pipe_fd = fd;
	cmd->exec.pid = fork();
	if (cmd->exec.pid == -1)
	{
		close_pipe_fds(pipe_fd, *prev_fd);
		return (perror(MINI), 1);
	}
	if (cmd->exec.pid == 0)
		exec_child(cmd, pipe_fd, *prev_fd, shell);
	if (*prev_fd != -1)
		close(*prev_fd);
	if (pipe_fd)
	{
		close(pipe_fd[1]);
		*prev_fd = pipe_fd[0];
	}
	return (0);
}

/*
DESCRIPTION:
	It sets up signals to default and duplicates required fds adn closes all fds.
	It then builds path if necessary and executes the command. If execve returns,
		cleans up and prints error.

	EXECVE:
	When a program is executed as a result of an execve() call,
		it is entered as follows:
		int main(int argc, char **argv, char **envp);
	As the execve() function overlays the current process image with a new
    process image, the successful call has no process to return to.
	If execve() does return (to the calling process, an error has occurred);
	the return value will be
		-1 and the global variable errno is set to indicate the error.
*/

static void	exec_child(t_cmd *cmd, int *fd, int prev_fd, t_shell *shell)
{
	char		*path;
	char		**envp;

	setup_child_handler();
	if (fd && (dup2(fd[1], STDOUT_FILENO) == -1))
		child_dup_error(fd, prev_fd);
	if (prev_fd != -1 && (dup2(prev_fd, STDIN_FILENO) == -1))
		child_dup_error(fd, prev_fd);
	close_pipe_fds(fd, prev_fd);
	if (setup_redirs(cmd->redirs) == -1)
		exit(1);
	path = cmd->args[0];
	if (cmd->exec.builtin != -1)
		exit(exec_builtin(cmd, shell));
	path = get_valid_path(cmd->args[0], shell->env);
	envp = env_to_envp(shell->env);
	if (!envp)
		exit(1);
	execve(path, cmd->args, envp);
	if (!ft_strchr(cmd->args[0], '/'))
		free(path);
	free(envp);
	perror(MINI);
	exit(1);
}

/*
need to free all exported ones.
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

/*
DESCRIPTION:
	closes fds, prints error message and exits child on error.
*/

static void	child_dup_error(int *fd, int prev_fd)
{
	close_pipe_fds(fd, prev_fd);
	perror(MINI);
	exit(1);
}
