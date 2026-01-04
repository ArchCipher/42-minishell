#include "libft.h"

int	is_redir(char c)
{
	return (c == '>' || c == '<');
}

int	is_connector(char *s)
{
	return (*s == '|' || (*s == '&' && s[1] == '&'));
}

int	is_parenthesis(char c)
{
	return (c == '(' || c == ')');
}

int	is_word_delimiter(char *s)
{
	return (ft_isspace(*s) || is_redir(*s) || is_connector(s)
		|| is_parenthesis(*s));
}
