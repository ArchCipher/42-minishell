/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_checks.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 14:42:01 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 14:42:02 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	is_redir(char c)
{
	return (c == '>' || c == '<');
}

int	is_connector(char *s)
{
	return (*s == '|' || (*s == '&' && s[1] == '&'));
}

int	is_parenthesis(char c)
{
	return (c == '(' || c == ')');
}

int	is_word_delimiter(char *s)
{
	return (ft_isspace(*s) || is_redir(*s) || is_connector(s)
		|| is_parenthesis(*s));
}
