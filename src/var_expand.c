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
	Initializes a t_string structure with the specified capacity.
	Allocates size + 1 bytes and sets cap to size, len to 0.
	Returns initialized t_string. If malloc fails, returns t_string with
	all fields set to 0/NULL.
*/

t_string	alloc_tstring(size_t size)
{
	t_string	str;

	str.s = malloc(size + 1);
	if (!str.s)
		return (str);
	str.cap = size;
	str.len = 0;
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

int	append_var(t_string *str, char **token, char *end, t_shell *shell)
{
	const char	*var;
	size_t		var_len;
	size_t		last;

	var = get_var(token, end, shell);
	if (var == ((char *)-1))
		return (1);
	if (!var)
		return (0);
	var_len = ft_strlen(var);
	last = end - *token;
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

int	dollar_expandable(char *s, char *end)
{
	return (*s == '$' && s + 1 < end && (ft_strchr(EXPANDABLE, s[1])
			|| ft_isalnum(s[1])));
}
