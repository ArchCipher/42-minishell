#include "minishell.h"

int is_redir(char c)
{
    return (c == '>' || c == '<');
}

int is_connector(char *s)
{
    return (*s == '|' || (*s == '&' && s[1] == '&'));
}

int is_parenthesis(char c)
{
    return (c == '(' || c == ')');
}

int is_word_delimiter(char *s)
{
    return (ft_isspace(*s) || is_redir(*s) || is_connector(s) || is_parenthesis(*s));
}

t_token_type get_flag(char *s, t_token_type flag)
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
