/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:36 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 16:41:06 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
DESCRIPTION:
	Appends the value of the environment variable to the string.
	Returns 0 on success, 1 if malloc() fails or on invalid substitution.

NOTE:
	As NULL is a valid expansion, (char *)-1 is used as a sentinel value for
	invalid substitution. Must not be dereferenced, but only compared to detect
	invalid substitution.
*/

int	append_var(t_string *str, const char *var, size_t last)
{
	size_t	var_len;

	if (var == ((char *)-1))
		return (1);
	if (!var)
		return (0);
	var_len = ft_strlen(var);
	if (str->cap < str->len + var_len + last)
	{
		str->cap = str->len + var_len + last;
		str->s = ft_realloc(str->s, str->len, (str->cap) + 1);
		if (!str->s)
			return (perror(MINI), 1);
	}
	ft_memcpy(str->s + (str->len), var, var_len);
	str->len += var_len;
	return (0);
}

/*
DESCRIPTION:
	Checks if the character is expandable  ($, _, or alphanumeric).
	Returns true if the character is expandable, false otherwise.
*/

bool	dollar_expandable(char *s, char *end)
{
	return (*s == '$' && s + 1 < end && (ft_strchr(EXPANDABLE, s[1])
			|| ft_isalnum(s[1])));
}
