/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 14:42:04 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 14:42:05 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
DESCRIPTION:
	Allocates a token and returns it.
	Returns the token on success, NULL on failure.
*/

t_list	*create_token(char *raw, t_token_type type, size_t len)
{
	t_list	*new;
	t_token	*token;

	new = ft_lstnew(sizeof(t_token));
	if (!new)
		return (perror(MINI), NULL);
	token = get_tok(new);
	token->raw = raw;
	token->word = NULL;
	token->type = type;
	token->len = len;
	return (new);
}

/*
DESCRIPTION:
	Deletes the current token from the token list and frees its contents.
*/

void	del_one_token(t_list **head, t_list *prev, t_list **cur)
{
	t_list	*tmp;

	if (!cur || !*cur)
		return ;
	tmp = (*cur)->next;
	if (!prev)
		*head = tmp;
	else
		prev->next = tmp;
	ft_lstdelone(*cur, free_token);
	*cur = tmp;
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

/*
DESCRIPTION:
	Checks if a string is fully quoted (starts and ends with matching quotes).
	Returns 1 if fully quoted, 0 otherwise.
*/

int	is_fully_quoted(const char *raw, size_t len)
{
	const char	*p;

	if (len < 2)
		return (0);
	if (*raw != '\'' && *raw != '\"')
		return (0);
	p = ft_memchr(raw + 1, *raw, len - 1);
	if (!p)
		return (0);
	return (p == raw + len - 1);
}

/*
DESCRIPTION:
	Copies src to dst while removing quote characters.
	Returns pointer to the null terminator in dst.
*/

char	*remove_quotes(char *dst, const char *src)
{
	t_token_type	old_flag;
	t_token_type	flag;

	old_flag = WORD;
	while (*src)
	{
		flag = update_quote_flag(*src, old_flag);
		if (flag == old_flag)
			*dst++ = *src++;
		else
			src++;
		old_flag = flag;
	}
	*dst = 0;
	return (dst);
}
