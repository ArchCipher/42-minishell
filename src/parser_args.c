/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_args.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 14:41:34 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/11 17:19:20 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
DESCRIPTION:
	Splits a word token containing spaces into multiple word tokens.
	The original token is deleted and replaced with new tokens created from
	the space-separated words. Returns 0 on success, 1 on failure.
*/

int	split_args(char ***args, size_t *cap, size_t *j, char *s)
{
	char	*tok;
	char	*p;
	char	*tmp;

	tok = ft_strtok_r(s, IS_SPACE, &p);
	while (tok)
	{
		tmp = ft_strdup(tok);
		if (!tmp)
			return (1);
		if (append_arg(args, cap, j, tmp))
			return (free(tmp), 1);
		tok = ft_strtok_r(NULL, IS_SPACE, &p);
	}
	free(s);
	return (0);
}

int	append_arg(char ***args, size_t *cap, size_t *len, char *s)
{
	char	**tmp;

	if (*len + 1 >= *cap)
	{
		tmp = ft_realloc(*args, (*cap) * sizeof(char *), (*cap + 4)
				* sizeof(char *));
		if (!tmp)
			return (1);
		*args = tmp;
		*cap += 4;
	}
	(*args)[(*len)++] = s;
	return (0);
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

