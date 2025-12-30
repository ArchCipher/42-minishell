#include "minishell.h"

/*
must update tail in the caller function
*/

void	lstadd_back(void **head, void *new, void *last, e_node_type type)
{
	if (!head || !new)
		return ;
	if (*head && !last)
	{
		ft_dprintf(STDERR_FILENO, "lstadd_back: last is NULL\n");
		return ;
	}
	if (!*head)
		*head = new;
	else
	{
		if (type == TYPE_TOKEN)
			((t_token *)last)->next = (t_token *)new;
		else if (type == TYPE_CMD)
			((t_cmd *)last)->next = (t_cmd *)new;
		else if (type == TYPE_REDIR)
			((t_redir *)last)->next = (t_redir *)new;
		else if (type == TYPE_ENV)
			((t_env *)last)->next = (t_env *)new;
	}
}
