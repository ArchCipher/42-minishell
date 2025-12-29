/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 13:47:52 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:53:15 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*handle_word(char *token, char *end, t_shell *shell);
static int	token_is_expandable(char *s, e_token_type *flag, char *end, t_string *str);
static int del_one_token(t_token **tokens, t_token **cur, t_token *prev);
static void	error_free_tokens(t_token *tokens, t_token *current);

/*
strndup only word tokens, others are pointers input with just the token->type.
!!! now returns error for logical OR (||)
*/
t_token	*parse_tokens(t_token *tokens, t_shell *shell)
{
	t_token	*current;
	t_token	*prev;

	if (!tokens)
		return (NULL);
	current = tokens;
	prev = NULL;
	while (current)
	{
		if (current->type == word)
		{
			current->token = handle_word(current->token, current->token
					+ current->len, shell);
			if (!current->token)
				return (free_tokens(tokens, true, current), NULL);
			if (del_one_token(&tokens, &current, prev))
				continue ;
		}
		else if (!current->next || (current->type == pipe_char && current->next->type == pipe_char) || (current->type != pipe_char && current->next->type != word))
			return (error_free_tokens(tokens, current), NULL);
		prev = current;
		current = current->next;
	}
	return (tokens);
}

/*
when accessing token + 1 or token[1],
check if token + 1 < end and then check token[1] or use token + 1
would be nice to check end quote, but subject doesn't require this
	if (*token == '\'' && flag == word && ft_memchr(token + 1, '\'', end - token
		- 1))
*/

static char	*handle_word(char *token, char *end, t_shell *shell)
{
	e_token_type	flag;
	t_string		str;

	str.str = malloc((end - token) + 1);
	if (!str.str)
		return (perror(MINI), NULL);
	str.cap = end - token;
	str.i = 0;
	flag = word;
	while (token < end)
	{
		if (token_is_expandable(token, &flag, end, &str))
		{
			if (!handle_dollar(&token, end, &str, shell))
				return (free(str.str), NULL);
		}
		else
			token++;
	}
	str.str[str.i] = 0;
	return (str.str);
}

static int	token_is_expandable(char *s, e_token_type *flag, char *end, t_string *str)
{
	if (*s == '\'' && *flag == word)
		*flag = squote;
	else if (*s == '\"' && *flag == word)
		*flag = dquote;
	else if ((*s == '\'' && *flag == squote) || (*s == '\"' && *flag == dquote))
		*flag = word;
	else if (!(*s == '$' && *flag != squote && s + 1 < end
				&& (ft_strchr(EXPANDABLE, s[1]) || ft_isalnum(s[1]))))
		str->str[(str->i)++] = *s;
	if ((*s == '$' && *flag != squote && s + 1 < end
			&& (ft_strchr(EXPANDABLE, s[1]) || ft_isalnum(s[1]))))
		return (1);
	return (0);
}

static int del_one_token(t_token **tokens, t_token **cur, t_token *prev)
{
	t_token *tmp;

	tmp = *cur;
	if (!(!*tmp->token && !prev))
		return (0);
	*cur = tmp->next;
	free(tmp->token);
	if (!prev)
		*tokens = tmp->next;
	else
		prev->next = tmp->next;
	free(tmp);
	return (1);
}

static void	error_free_tokens(t_token *tokens, t_token *current)
{
	if (!current->next)
		ft_dprintf(STDERR_FILENO, "%s: %s`newline'\n", MINI, E_PARSE);
	else
		ft_dprintf(STDERR_FILENO, "%s: %s`%c'\n", MINI, E_PARSE, current->next->token[0]);	
	free_tokens(tokens, true, current);
}
