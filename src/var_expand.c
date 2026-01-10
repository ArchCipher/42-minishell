/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_expand.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:36 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 14:42:12 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
DESCRIPTION:
	Initializes a t_expand structure with the specified capacity.
	Allocates size + 1 bytes and sets cap to size, len to 0.
	Returns initialized t_expand. If malloc fails, returns t_expand with
	all fields set to 0/NULL.
*/

t_expand	init_expand(char *src, size_t size)
{
	t_expand	str;

	str.dst = malloc(size + 1);
	if (!str.dst)
		return (str);
	str.cap = size;
	str.len = 0;
	str.src = src;
	str.src_end = src + size;
	return (str);
}

/*
DESCRIPTION:
	Expands and appends the value of the environment variable to the string.
	Expects token to point at the '$' character. Increments token to skip '$'
	before calling get_var.
	Returns 0 on success, 1 if malloc() fails or on invalid substitution.

NOTE:
	As NULL is a valid expansion, (char *)-1 is used as a sentinel value for
	invalid substitution. Must not be dereferenced, but only compared to detect
	invalid substitution.
*/

int	append_var(t_expand *s, t_shell *shell)
{
	const char	*var;
	size_t		var_len;

	var = get_var(&s->src, shell);
	if (var == ((char *)-1))
		return (1);
	if (!var)
		return (0);
	var_len = ft_strlen(var);
	if (s->cap < s->len + var_len + (s->src_end - s->src))
	{
		s->cap = s->len + var_len + (s->src_end - s->src);
		s->dst = ft_realloc(s->dst, s->len, (s->cap) + 1);
		if (!s->dst)
			return (perror(MINI), 1);
	}
	ft_memcpy(s->dst + (s->len), var, var_len);
	s->len += var_len;
	return (0);
}

/*
DESCRIPTION:
	Checks if the character is expandable  ($, _, or alphanumeric).
	Returns true if the character is expandable, false otherwise.
*/

int	dollar_expandable(char *s, char *end)
{
	return (*s == '$' && s + 1 < end && (ft_strchr(EXPANDABLE, s[1])
			|| ft_isalnum(s[1])));
}
