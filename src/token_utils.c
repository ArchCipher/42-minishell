#include "minishell.h"

/*
DESCRIPTION:
	Allocates a token and returns it.
	Returns the token on success, NULL on failure.
*/

t_token	*create_token(void *token, t_token_type type, size_t len)
{
	t_token	*new;

	new = malloc(sizeof(t_token));
	if (!new)
		return (perror(MINI), NULL);
	new->raw = token;
	new->word = NULL;
	new->type = type;
	new->len = len;
	new->quoted = false;
	new->next = NULL;
	return (new);
}

/*
DESCRIPTION:
	Deletes the current token from the token list and frees its memory.
*/

void	del_one_token(t_token **head, t_token *prev, t_token **cur)
{
	t_token	*next;

	next = (*cur)->next;
	if (!prev)
		*head = (*cur)->next;
	else
		prev->next = (*cur)->next;
	free((*cur)->word);
	free(*cur);
	if (prev)
		*cur = prev;
	else
		*cur = next;
}

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
