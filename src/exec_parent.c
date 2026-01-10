#include "minishell.h"

static void	restore_fds(int o_stdin, int o_stdout, int o_stderr,
				t_shell *shell);

/*
DESCRIPTION:
	Executes builtin in parent and restores standard input and output
	fds after execution.
NOTE:
	On exit from shell, "exit\n" should be written to stdout, before exec_exit
	is called. exec_exit() error messages should be printed after "exit\n".
*/

int	exec_in_parent(t_list *cmds, t_shell *shell)
{
	t_cmd	*cmd;
	int	o_stdin;
	int	o_stdout;
	int	o_stderr;

	cmd = get_cmd(cmds);
	o_stdout = dup(STDOUT_FILENO);
	if (o_stdout == -1)
		return (perror(MINI), 1);
	o_stdin = dup(STDIN_FILENO);
	if (o_stdin == -1)
		return (close(o_stdout), perror(MINI), 1);
	o_stderr = dup(STDERR_FILENO);
	if (o_stderr == -1)
		return (close(o_stdout), close(o_stdin), perror(MINI), 1);
	if (setup_redirs(cmd->redirs))
		return (restore_fds(o_stdin, o_stdout, o_stderr, shell), 1);
	if (isatty(o_stdout) && cmd->exec.builtin == BUILTIN_EXIT)
		write(o_stdout, "exit\n", 5);
	shell->status = exec_builtin(cmd, shell);
	restore_fds(o_stdin, o_stdout, o_stderr, shell);
	if (cmd->exec.builtin == BUILTIN_EXIT && (!cmd->args[1] || (cmd->args[1]
				&& !cmd->args[2]) || (cmd->args[2] && errno == EINVAL)))
		exit_shell(cmds, shell);
	return (shell->status);
}

/*
DESCRIPTION:
	Restores standard input and output fds after execution.
	Returns the exit status of the builtin execution or 1 on error.
*/

static void	restore_fds(int o_stdin, int o_stdout, int o_stderr, t_shell *shell)
{
	if ((dup2(o_stdin, STDIN_FILENO) == -1)
		|| (dup2(o_stdout, STDOUT_FILENO) == -1)
		|| (dup2(o_stderr, STDERR_FILENO) == -1))
	{
		perror(MINI);
		shell->status = 1;
	}
	close(o_stdin);
	close(o_stdout);
	close(o_stderr);
}
