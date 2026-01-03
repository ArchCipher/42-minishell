#include "minishell.h"

int is_type_redir(t_token_type t)
{
    return (t == REDIR_IN || t == REDIR_OUT || t == APPEND || t == HEREDOC);
}

int is_type_con(t_token_type t)
{
    return (t == PIPE_CHAR || t == AND || t == OR);
}

t_token_type update_quote_flag(char *s, t_token_type flag)
{
    if (*s == '\'' && flag == WORD)
		return (SQUOTE);
	else if (*s == '\"' && flag == WORD)
		return (DQUOTE);
	else if ((*s == '\'' && flag == SQUOTE) || (*s == '\"' && flag == DQUOTE))
		return (WORD);
    return (flag);
}

/*
DESCRIPTION:
	Prints the error message and frees the tokens.
    to make it nicer print the tok-len chars instead of 1 char
*/

void	perr_token(char *s, size_t	len)
{
	if (!s)
	{
		s = "newline";
		len = 7;
	}
	ft_dprintf(FD_ERR, "%s: %s `", MINI, E_PARSE);
	write(FD_ERR, s, len);
	write(FD_ERR, "'\n", 2);
}

void	perr_msg(const char *s1, const char *s2, const char *s3, bool backtick)
{
	ft_dprintf(FD_ERR, "%s: %s", MINI, s1);
	if (s2 && !backtick)
		ft_dprintf(FD_ERR, ": %s", s2);
	else if (s2 && backtick)
		ft_dprintf(FD_ERR, ": `%s'", s2);
	if (s3)
		ft_dprintf(FD_ERR, ": %s", s3);
	write(FD_ERR, "\n", 1);
}
