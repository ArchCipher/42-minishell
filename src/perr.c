#include "minishell.h"

/*
DESCRIPTION:
	Prints a parsing error message with the token that caused the error.
	Prints s with backticks, if sis NULL, prints "newline" instead.
*/

void	perr_token(char *s, size_t len)
{
	if (!s)
	{
		s = "newline";
		len = 7;
	}
	ft_dprintf(STDERR_FILENO, "%s: %s `", MINI, E_PARSE);
	write(STDERR_FILENO, s, len);
	write(STDERR_FILENO, "'\n", 2);
	errno = EINVAL;
}

/*
DESCRIPTION:
	Prints a formatted error message to stderr.
	If backtick is true, s2 is printed with backticks, otherwise with colons.
*/

void	perr_msg(const char *s1, const char *s2, const char *s3, bool backtick)
{
	ft_dprintf(STDERR_FILENO, "%s: %s", MINI, s1);
	if (s2 && !backtick)
		ft_dprintf(STDERR_FILENO, ": %s", s2);
	else if (s2 && backtick)
		ft_dprintf(STDERR_FILENO, ": `%s'", s2);
	if (s3)
		ft_dprintf(STDERR_FILENO, ": %s", s3);
	write(STDERR_FILENO, "\n", 1);
}

// bash: export: --: invalid option
// bash: $T: ambiguous redirect
// no backticks

/*
DESCRIPTION:
	Prints an error message for a token-related error.
	If s is NULL, prints "newline" instead.
*/

void	perr_tok_msg(char *cmd, char *s, size_t len, const char *msg)
{
	if (!s)
	{
		s = "newline";
		len = 7;
	}
	ft_dprintf(STDERR_FILENO, "%s: ", MINI);
	if (cmd)
		ft_dprintf(STDERR_FILENO, "%s: ", cmd);
	write(STDERR_FILENO, s, len);
	if (msg)
		ft_dprintf(STDERR_FILENO, ": %s\n", msg);
}

