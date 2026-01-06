/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_split.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 14:41:34 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 17:36:27 by kmurugan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	insert_token(t_token **head, t_token **prev, t_token *next,
				const char *tok);

/*
DESCRIPTION:
	Splits a word token containing spaces into multiple word tokens.
	The original token is deleted and replaced with new tokens created from
	the space-separated words. Returns 0 on success, 1 on failure.
*/

int	split_word_token(t_token **head, t_token **cur, t_token *prev)
{
	t_strtok	str;

	str.str = ft_strdup((*cur)->word);
	if (!str.str)
		return (1);
	del_one_token(head, prev, cur);
	str.token = ft_strtok_r(str.str, IS_SPACE, &str.p);
	while (str.token)
	{
		if (insert_token(head, &prev, *cur, str.token))
			return (free(str.str), 1);
		str.token = ft_strtok_r(NULL, IS_SPACE, &str.p);
	}
	free(str.str);
	return (0);
}

/*
DESCRIPTION:
	Inserts a new word token into the token list at the specified position.
	Returns 0 on success, 1 on failure.
*/

static int	insert_token(t_token **head, t_token **prev, t_token *next,
				const char *tok)
{
	t_token	*new;
	char	*word;

	word = ft_strdup(tok);
	if (!word)
		return (1);
	new = create_token(word, WORD, ft_strlen(word));
	if (!new)
		return (free(word), 1);
	new->word = word;
	new->next = next;
	if (!*prev)
		*head = new;
	else
		(*prev)->next = new;
	*prev = new;
	return (0);
}

