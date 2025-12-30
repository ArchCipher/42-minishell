/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:36 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:25:13 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
ERR is (char *)-1, just used as a sentinal value for error.
Must not be dereferenced, but only compared to detect error.
*/

int	copy_var(t_string *str, const char *var, size_t last)
{
	size_t		var_len;

	if (var == ERR_PTR)
		return (0);
	if (!var)
		return (1);
	var_len = ft_strlen(var);
	if (str->cap < str->len + var_len + last)
	{
		str->cap = str->len + var_len + last;
		str->s = ft_realloc(str->s, str->len, (str->cap) + 1);
		if (!str->s)
			return (perror(MINI), 0);
	}
	ft_memcpy(str->s + (str->len), var, var_len);
	str->len += var_len;
	return (1);
}
bool dollar_expandable(char *s, char *end)
{
	return (*s == '$' && s + 1 < end && (ft_strchr(EXPANDABLE, s[1]) || ft_isalnum(s[1])));
}
