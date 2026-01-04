#include "minishell.h"

int	is_type_redir(t_token_type t)
{
	return (t == REDIR_IN || t == REDIR_OUT || t == APPEND || t == HEREDOC);
}

int	is_type_con(t_token_type t)
{
	return (t == PIPE_CHAR || t == AND || t == OR);
}

t_token_type	update_quote_flag(char c, t_token_type flag)
{
	if (c == '\'' && flag == WORD)
		return (SQUOTE);
	else if (c == '\"' && flag == WORD)
		return (DQUOTE);
	else if ((c == '\'' && flag == SQUOTE) || (c == '\"' && flag == DQUOTE))
		return (WORD);
	return (flag);
}

/*
DESCRIPTION:
	Prints the error message and frees the tokens.
	to make it nicer print the tok-len chars instead of 1 char
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
}

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
