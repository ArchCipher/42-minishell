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
